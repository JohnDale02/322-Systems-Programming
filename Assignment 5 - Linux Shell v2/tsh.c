/* John Dale Assignment 5
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

void handleRdType(char* cmd1, char* cmd2, int rdType, int fd);  // function definition for handling redirection & piping

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{   
    pid_t pid;      // PID for job
    pid_t pid2;    // PID for piping
    sigset_t mask;   // mask of SIGNALS
    char *argv[MAXARGS];     // array of command line arguments
    
    int rdType = 0;   // redirection type (assigned as integer)
    char* cmdCut = cmdline;    // cut command line 
    char* cmd1;         // 1st file for redirection
    char* cmd2 = NULL;  // 2nd file for redirection
    int fdArray[2];   // file descriptor array for piping

    if (strstr(cmdline, "2>")){  // if redirecting stderr to file
        rdType = 1;    
        if (strsep(&cmdCut, "2>") < 0){
            printf("StrSep() error");
            exit(-1);
        }  // cut at 2>
        cmd1 = cmdCut;   // send over fileName after 2>
    }
    
    else if (strstr(cmdline, ">>")){   // if appending stdout to file
        rdType = 2;
        if (strsep(&cmdCut, ">>") < 0){
            printf("StrSep() error");
            exit(-1);
        }  // cut at >>
        cmd1 = cmdCut;   // send over filename after >> 
    }
    
    if (strstr(cmdline, "<") && strstr(cmdline, ">")){  // redirecting both
        rdType = 5;
        if (strsep(&cmdCut, "<") < 0){
            printf("StrSep() error");
            exit(-1);
        }  // first change cmdCut to point to < fileName
        cmd1 = strsep(&cmdCut, ">");  // cmd1 = return of strsep, and overwrite > (this is in.txt)
        if (cmd1 < 0){
            printf("StrSep() error");
            exit(-1);
        }
        cmd2 = cmdCut;      // cmd2 = cmdCut, which has been cut twice, and will point to fileName after > (file.txt)
    }
    
    else if (strstr(cmdline, ">")){  // if redirecting stdin to file
        rdType = 3;
        strsep(&cmdCut, ">");  // cut command line at >
        cmd1 = cmdCut;      // send over filename after > 
    }

    else if (strstr(cmdline, "<")){  // if redirecting stdout file 
        rdType = 4; 
        if (strsep(&cmdCut, "<") < 0){
            printf("StrSep() error");
            exit(-1);
        }  // cut command line at <
        cmd1 = cmdCut;         // send over fileName after <
    }

    if (strstr(cmdline, "|")){  // if we are piping
        rdType = 6;
        if (strsep(&cmdCut, "|") < 0){
            printf("StrSep() error");
            exit(-1);
        }  // cut at the pipe character
        
        if (pipe(fdArray) < 0){  // creates pipe and puts fdArray file descriptors
            printf("Pipe Error"); // into reading and writing ends of pipes
            exit(-1);
        }  
        cmd1 = cmdCut;  // cmd1 = points to fileName after pipe 
  }

    int bg = parseline(cmdline, argv);   // builds argv array and tells us if bg or fg (1 == bg)
    if(argv[0] == NULL){  // if nothing is written to line, do nothing
        return; 
    }
    if (builtin_cmd(argv) == 0) {  // if no build in commands are specified
         
        sigemptyset(&mask);      // empty mask, add sigchilds, and block them 
        sigaddset(&mask, SIGCHLD);  // DOING THIS SO NO SIGCHILDS RECIEVED BEFORE JOB IS ADDED 

        if((pid = fork()) == 0){       // fork and run program in child

            if (rdType){    // if there is some redirection or piping
                handleRdType(cmd1, cmd2, rdType, fdArray[1]); // handle file descriptors (dup2(), open, and close)
            }  
            if (setpgid(0,0) < 0) {     // giving every child unique process group 
                unix_error("Process Group Error"); // checking return of setpgid 
            }    
            if(execve(argv[0], argv, environ) < 0 ){  // run program in child 
                printf("%s: Command not found. \n", argv[0]); // check return 
                exit(0);       // exit 
            }
        }
        else if (pid < 0){   // check fork return 
            unix_error("Fork pid ERROR");
        }

        else{    // if you are the parent 

            if (rdType == 6){   // if we are piping
                close(fdArray[1]);
                parseline(cmd1, argv);
                if ((pid2 = fork()) == 0 ){  // if we are spawned child of fork
                    rdType = 9;  // special type for child of forked process
                    handleRdType(cmd1, cmd2, rdType, fdArray[0]);  // call our handle with rdType9 
                    if (execve(argv[0], argv, environ) < 0){
                        printf("%s: Command not found. \n", argv[0]); // check return 
                        exit(0);       // exit 
                    }  // execve the program
                    exit(0);
                }
                else if (pid2 < 0){   // check fork return 
                    unix_error("Fork pid2 ERROR");
                }
                close(fdArray[0]);  // close the parents FD (to prevent memory leak)
            }

            if (!bg) {                      // if foreground job 
                addjob(jobs, pid, FG, cmdline);  // add job with FG state
                sigprocmask(SIG_UNBLOCK, &mask, NULL);  // unblock SIGCHILDS
                waitfg(pid);                        // execute FG job
                waitfg(pid2);
                }   
            

            else {                              // if bg job 
                addjob(jobs, pid, BG, cmdline);     // add job with BG state
                sigprocmask(SIG_UNBLOCK, &mask, NULL);  // unblock sigchilds
                printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
            }
        }    
    }
    return; 
}

char* cleanFile(char* cmd, int option){   // takes in cmdCut and returns clean file names
    int i = 0 ;
    
    while ((*cmd == ' ') || (*cmd == '<') || (*cmd == '>')){  // first get rid of spaces and '>'
        ++cmd;  // increment to next string
    }
    
    while ((cmd[i] != '\0') && (cmd[i] != ' ')){ // while character isnt null or space --> keep incrementing
        i++; 
    }
   
    if (option == 1){   // only used for middle file with double redirection ex) /bin/process < THIS > dontCare
        cmd[i] = '\0';  // need to add null character to pass into open() 
    }
    if (option == 0){  // need to add null character to pass into open()
        cmd[i-1] = '\0';
    }
    return cmd; // returning filename with null terminator at end 
}

void handleRdType(char* cmd1, char* cmd2, int rdType, int fd){

    if (rdType == 1){  // if redirecting stderr (2>)
        int fdReturned = open(cleanFile(cmd1, 0), O_CREAT|O_TRUNC|O_WRONLY, S_IRWXU | S_IRWXG);
        
        if (fdReturned < 0){  // checking open() system call
            printf("Error in open()");
            exit(-1);
        }

        if (dup2(fdReturned, 2) < 0){
            printf("Error in dup2 rdType 1");
            exit(-1);
        }   // copy to fd2 = stderror
        return; 
    }

    if (rdType == 2){  // if appending stdout to file (>>)_
        int fdReturned = open(cleanFile(cmd1, 0), O_APPEND|O_CREAT|O_WRONLY, S_IRWXU | S_IRWXG); 
        
        if (fdReturned < 0){  // checking open() system call
            printf("Error in open()");
            exit(-1);
        }
        
        if (dup2(fdReturned, 1) < 0){    // copy to fd1 = stdout
            printf("Error in dup2 rdType 2");
            exit(-1);
        } 
        return;
    }

    if (rdType == 3){  // if redirecting stdout (>)
        int fdReturned = open(cleanFile(cmd1, 0), O_CREAT|O_TRUNC|O_WRONLY, S_IRWXU | S_IRWXG); 

        if (fdReturned < 0){  // checking open() system call
            printf("Error in open()");
            exit(-1);
        }

        if (dup2(fdReturned, 1) < 0){   // copy to fd1 = stdout
            printf("Error in dup2 rdType 3");
            exit(-1);
        }   
        return;
    }

    if (rdType == 4){  // if redirecting stdin (<)
        int fdReturned = open(cleanFile(cmd1, 0), O_RDONLY);

        if (fdReturned < 0){  // checking open() system call
            printf("Error in open()");
            exit(-1);
        }

        if (dup2(fdReturned, 0) < 0){    // copy to fd0 = stdin
            printf("Error in dup2 rdType 4");
            exit(-1);
        }  
        return; 
    }

    if (rdType == 5){   // redirecting both (< and >)
        int fdReturned = open(cleanFile(cmd1, 1), O_RDONLY);

        if (fdReturned < 0){  // checking open() system call
            printf("Error in open()");
            exit(-1);
        }

        if (dup2(fdReturned, 0) < 0){        // first copy to fd0 = stdin
            printf("Error in dup2 rdType 5 1st call");
            exit(-1);
        } 

        int fdReturned2 = open(cleanFile(cmd2, 0), O_CREAT|O_TRUNC|O_WRONLY, S_IRWXU | S_IRWXG);

        if (fdReturned2 < 0){  // checking open() system call
            printf("Error in open()");
            exit(-1);
        }

        if (dup2(fdReturned2, 1) < 0){   // second copy to fd1 = stdout
            printf("Error in dup2 rdType 5 2nd call");
            exit(-1);
        }  
        return;
    }

    if (rdType == 6){  // if we are piping 
        if (dup2(fd, 1) < 0){    // copy passed fdArray to stdout file descriptor
            printf("Error in dup2 rdType 6");
            exit(-1);
        }  
        
        if (close(fd) < 0){ // close the fd 
            printf("Error in close()");
            exit(-1);
        }  
        return;
    }

    if (rdType == 9){  // piping continued for forked child process
        if (dup2(fd, 0) < 0){   // copy passed fdArray to stdin file descriptor
            printf("Error in dup2 rdType 9");
            exit(-1);
        }    
        // parent must close this fd
        return;
    }
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
	buf++;
	delim = strchr(buf, '\'');
    }
    else {
	delim = strchr(buf, ' ');
    }

    while (delim) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
	return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	argv[--argc] = NULL;
    }
    //printf("Background in parse statemnt: %d\n", bg);
    return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char **argv) 
{
    if ( strcmp(argv[0], "quit") == 0 ){    // if user enter's quit 
        exit(0);                            // exit 
    }

    else if ( strcmp(argv[0], "jobs") == 0) {  // Print job list 
        listjobs(jobs);
        return 1;
    }

    else if ( (strcmp(argv[0], "bg") == 0) || ( strcmp(argv[0], "fg") == 0)) {  // if want to run job in bg of fg
        do_bgfg(argv);
        return 1;
    }
    return 0;  // if returns 0, NOT BUILT IN 
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    struct job_t *cjob;    // create a new job struct
    cjob = NULL;           // initialize job to NULL 
    char* id = argv[1];    // second argument should be job id 
    int valid = 0;         // is there a valid PID or JID 
    

// ---------------- Either PID, JID, NULL, OR Not specified Correctly ---------------------------

    if ( (strcmp(argv[0], "fg") == 0) && id == NULL ){    // specified fg but no pid
        printf("fg command requires PID or %%jobid argument\n");
            return;
    }
    if ( (strcmp(argv[0], "bg") == 0) && id == NULL ){  // specified bg but no pid
        printf("bg command requires PID or %%jobid argument\n");
            return;
    }
    else if (id[0] == '%'){           // specified JID 
        int jid = atoi(&id[1]);       // convert string into int value (JID)

        if ((cjob = getjobjid(jobs, jid)) == 0){  // job with JID doesnt exitst 
            printf("%%%d: No such job\n", jid);
            return;
        }
        else{                                   // there is a job with specified JID
            valid = 1;                          // set Valid == 1 
        }
    }
    else if (isdigit(id[0])){          // specified PID
        int pid = atoi(id);            // convert string into int value (PID)

        if ((cjob = getjobpid(jobs, pid)) == 0){    // job with PID doesnt exist
            printf("(%d): No such process\n", pid); 
            return;
        }
        else{                                  // there is a job with specified PID
            valid = 1;                          // set Valid == 1 
        }
    }
    
    else if ((strcmp(argv[0], "fg") == 0) && valid == 0 ){  // fg specified but invalid PID or JID 
        printf("fg: argument must be a PID or %%jobid\n");
        return;
    }
    else if ((strcmp(argv[0], "bg") == 0) && valid == 0 ){  // bg specified but invalid PID or JID 
        printf("bg: argument must be a PID or %%jobid\n");
        return; 
    }


    else{              // PID or JID is invalid 
        printf("PID or JID not specified correctly");
        return;
    }

// --------------- Have the job we need to change to FG or BG ------------------------------- 
    
     // Send Continue Signal to process group... 
    if(kill(-(cjob->pid), SIGCONT) < 0){    // Necessary if FG job stopped so not waiting in waitfg infinitely  
        printf("Cannot continue process\n");
    }

    
    if (strcmp(argv[0], "bg") == 0){   // if it is a background job
        cjob->state = BG;              // set state to BG
        printf("[%d] (%d) %s", cjob->jid, cjob->pid, cjob->cmdline);

    }

    else if (strcmp(argv[0], "fg") == 0){    // if it is a foreground job
        cjob->state = FG;                   // set state to FG
        waitfg(cjob->pid);                  // execute job by calling waitfg
    
    }

    else{
        printf("%s not FG or BG", argv[0]);   // somehow not BG or FG
    }
    return ;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */ 
void waitfg(pid_t pid)   // takes in PID of process, is that matches PID of current FG job, then wait
{
    while (pid == fgpid(jobs)){   // while pid == foreground job pid, sleep and let it finish
        sleep(1);             
    }
    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    pid_t pid;
    int status;

    // waitpid() reaps all zombie children......
    // WNOHANG prevents waitpid from blocking returns 0 if nothing dies.........
    // WUNTRACED allows for waitpid() to return if a child is stopped or killed in other ways than waiting

    while((pid = waitpid(-1, &status, WUNTRACED | WNOHANG)) > 0 ){  // waiting for child to terminate
        
        if ( WIFSIGNALED(status) ){  // child was terminated (could be from SIGINT)  
            int jid = pid2jid(pid);  
            printf("Job [%d] (%d) terminated by signal %d\n", jid, pid, WTERMSIG(status));  // print signal terminated by 
            deletejob(jobs, pid);  // delete job from list 
        }
        else if( WIFSTOPPED(status) ){   // child was stopped (could be from SIGSTP)   
            struct job_t *cjob;
            int jid = pid2jid(pid);
            cjob = getjobpid(jobs, pid);
            printf("Job [%d] (%d) stopped by signal %d\n", jid, pid, WSTOPSIG(status)); // print signal stopped by 
            cjob->state = ST;        // change state of process to stopped
       
        }
        
        else if (WIFEXITED(status)){    // did the process exit normally?
            deletejob(jobs, pid);     // if yes, delete job from list 
        } 
    }
    return;
}
/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig)   
{
    int pid;
    pid = fgpid(jobs);   // get FG pid
    
    if (pid != 0 ){     // pid has to be valid
        if (kill(-pid, SIGINT) < 0){    // send SIGING signal to every job in process group 
            unix_error("SIGINT ERROR");
        } 
    }
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    int pid;
    pid = fgpid(jobs);  // get FG pid 

    if (pid !=0 ){   // the pid has to be valid 
        if (kill(-pid, SIGTSTP) < 0){    // send stop signal to all processes in process group 
            unix_error("SIGSTP ERROR");
        } 
    }
    return;
}
/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid > max)
	    max = jobs[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
    int i;
    
    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == 0) {
	    jobs[i].pid = pid;
	    jobs[i].state = state;
	    jobs[i].jid = nextjid++;
	    if (nextjid > MAXJOBS)
		nextjid = 1;
	    strcpy(jobs[i].cmdline, cmdline);
  	    if(verbose){
	        printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
	}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == pid) {
	    clearjob(&jobs[i]);
	    nextjid = maxjid(jobs)+1;
	    return 1;
	}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].state == FG)
	    return jobs[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid)
	    return &jobs[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid) 
{
    int i;

    if (jid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid == jid)
	    return &jobs[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs) 
{
    int i;
    
    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid != 0) {
	    printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
	    switch (jobs[i].state) {
		case BG: 
		    printf("Running ");
		    break;
		case FG: 
		    printf("Foreground ");
		    break;
		case ST: 
		    printf("Stopped ");
		    break;
	    default:
		    printf("listjobs: Internal error: job[%d].state=%d ", 
			   i, jobs[i].state);
	    }
	    printf("%s", jobs[i].cmdline);
	}
    }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}
