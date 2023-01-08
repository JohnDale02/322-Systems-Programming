#include "gofish.h"



int callGame(){   // shuffles deck, deals cards and begins asking between user and computer
	shuffle();
	deal_player_cards(&user);
	deal_player_cards(&computer);
	
	while ((game_over(&user) != 1)&& (game_over(&computer) != 1)){
		askForCard();
		CaskForCard();
	}
return 0;
}


int printHand(struct player* target){  // prints hand of target (for user)
	struct hand* finder;
	finder = target->card_list;
	printf("\nPlayer 1's Hand - ");
	while( finder != NULL ) {   // from iterate for every card in hand 
		printf("%c%c ", finder->top.rank, finder->top.suit); // print 
		finder = finder->next;  // set hand pointer to next 		
	}
	return 0;
}

int printCHand(struct player* target){  // prints hand of target (for computer)
	struct hand* finder;
	finder = target->card_list;
	printf("\nComputer's Hand - ");
	while( finder != NULL ) {   // from iterate for every card in hand 
		printf("%c%c ", finder->top.rank, finder->top.suit); // print 
		finder = finder->next;  // set hand pointer to next 		
	}
	
	return 0;
}

int printBook(struct player* target){  // PRINT BOOK FOR USER
	
	printf("\nPlayer 1 Book - ");
	
	for(int i=0; i < target->numBook; i++){	
		printf(" %c", target->book[i]);
	}
	return 0;
	}

int printCBook(struct player* target){  // PRINT BOOK FOR COMPUTER
	
	printf("\nPlayer 2 Book - ");
	
	for(int i=0; i < target->numBook; i++){	
		printf(" %c",target->book[i]);
	}
	printf("\n");
	return 0;
	}


int printHB(){
	printHand(&user);  // printing Player 1's hand 
	//printCHand(&computer); // print Computer's Hand DELTE LATER
	printBook(&user);  // printing player 1's book
	printCBook(&computer);  // printing player 2's book
	return 0;
	}



int printRank(struct player* target, char r){   // takes in rank and prints all cards that match
	struct hand* temp;
	temp = target->card_list;
	while(temp != NULL){
		if(temp->top.rank == r){
			printf(" %c%c",temp->top.rank, temp->top.suit);
			temp = temp->next;
		}
		else{
		temp = temp->next;
		}
	}	
	printf("\n");
return 0;
}

int askForCard(){
	
	if (user.numBook != 7 && computer.numBook != 7){ // if nobody has 7 books
		printHB();
		if(user.hand_size == 0){			// if we are left with no cards after this book
			struct card* cardAdded = next_card();  // draw a card
			printf("\t- Player 1 draws %c%c\n", cardAdded->rank, cardAdded->suit);
			
			add_card(&user, cardAdded);
		}
		struct card* cardAdded;
		int cannotDraw = 0;
		int anotherTurn = 0;
		char Userguess = user_play(&user);
		if (search(&computer,Userguess)==1){  // if P2 does have rank 
			printf("\t- Player 2 has");    // print what they have
			printRank(&computer, Userguess);
			printf("\t- Player 1 has");
			printRank(&user, Userguess);  // print what we have
			anotherTurn = 1;		// grant another turn
		}
		else{
			printf("\t- Player 2 has no %c's\n", Userguess);  // dont have it 
			cannotDraw = 1;					
			cardAdded = next_card();		// draw a card
			printf("\t- Go Fish, Player 1 draws %c%c\n", cardAdded->rank, cardAdded->suit);
			add_card(&user, cardAdded);
		}
	
	int currBook = user.numBook;
	transfer_cards(&computer,&user, Userguess);
	check_add_book(&user);
	if(currBook != user.numBook){  // we have added a book
		printf("\t- Player 1 books %c\n", user.book[user.numBook-1]);  // print book name
		if(user.hand_size == 0){			// if we are left with no cards after this book
			struct card* cardAdded = next_card();  // draw a card
			printf("\t- Player 1 draws %c%c\n", cardAdded->rank, cardAdded->suit);
			
			add_card(&user, cardAdded);
		}	
	}
	if (anotherTurn == 1 ){ // if we got card from guessing give another turn
		printf("\t- Player 1 gets another turn\n");
		askForCard();
	}	
}
return 0;
}

int CaskForCard(){
	if( computer.numBook != 7 && user.numBook !=7){  
		printHB();
		if(computer.hand_size == 0){    // if they have no more cards, cannot have 0 cards, add card
				struct card* cardAdded = next_card();   
				printf("\t- Player 2 draws %c%c\n", cardAdded->rank, cardAdded->suit);  // print card
				add_card(&computer, cardAdded);  
			}
		int cannotDraw = 0;
		int anotherTurn = 0;
		struct card* cardAdded;
		char Computerguess = computer_play(&computer);  // getting computer guess
		printf("Player 2's Turn, enter a rank: %c\n", Computerguess);  // print computers guess
		if (search(&user,Computerguess) == 1){  // if user has a card of that rank
			printf("\t- Player 1 has");  
			printRank(&user, Computerguess);  // print player 1's cards of that rank
			printf("\t- Player 2 has");   // print player 2's cards of that rank
			printRank(&computer, Computerguess);
			anotherTurn = 1;    // give computer another turn 	
		}
		else{							// if user doesnt have card of that rank
			printf("\t- Player 1 has no %c's\n", Computerguess);	
			cardAdded = next_card();		// get card added
			cannotDraw = 1;				// set a variable saying already drawn a card
			printf("\t- Go Fish, Player 2 draws a card\n"); // print drawn card
			//printf("\t- Go Fish, Player 2 draws a %c%c\n", cardAdded->rank, cardAdded->suit); // print drawn card
			add_card(&computer, cardAdded);    // add card officially to computer deck
		 }
	
		int currBook = computer.numBook;   // current number of books for computer
		transfer_cards(&user,&computer, Computerguess); 	// transfer cards from user to computer (might be 0)
		check_add_book(&computer);				// check to see if computer got another book

		if(currBook != computer.numBook){ //if got another book, and not 7th
			printf("\t- Player 2 books %c\n", computer.book[computer.numBook-1]); // print what book
			
			if(computer.hand_size == 0){    // if they have no more cards, cannot have 0 cards, add card
				struct card* cardAdded = next_card();   
				printf("\t- Player 2 draws %c%c\n", cardAdded->rank, cardAdded->suit);  // print card
				add_card(&computer, cardAdded);  
			}
		}
		if (anotherTurn == 1){  // if they have not drawn a card from wrong guess, or get another turn
			printf("\t- Player 2 gets another turn\n");
			CaskForCard();
		}
	}  // closing first if	
return 0;
}

int whoWon(){
	if (user.numBook == 7){
		printHB();
		printf("Player 1 Won! ");
		printf("%d-%d\n", user.numBook, computer.numBook);

	}
	if (computer.numBook == 7){
		printHB();
		printf("Player 2 Won! ");
		printf("%d-%d\n", computer.numBook, user.numBook);
	}
return 0;
}

int playAgain(){
	char yn;
	int found = 0;
	while(found == 0){		// for all ranks
		printf("Do you want to play again? (y/n) ");
		scanf(" %c", &yn);
		if(yn == 'y' || yn  == 'n'){
			found = 1;
		}
		else{
			printf("Error - must be y or n\n");
			continue;
		}
	}

	if( yn == 'y'){
		user.reset = 1;
		return 0;
	}
	if (yn == 'n'){
		printf("EXITING\n");
		return 1;
		
	}
return 0;
}




int main(int args, char* argv[]) {  // why do we have args, char* argv[] (what are these refering too?) 

	int game = 1;  // still want to play game

	while(game == 1){
	reset_player(&user);   		 // resetting user 
	reset_player(&computer);  	 // resetting computer

	callGame(); 			 // shuffle, deal cards, ask 
	whoWon();  			 // print who won
		if (playAgain() == 1){   // if they inputted y then play again, if not break
			break; 
		}
	
	}
}


