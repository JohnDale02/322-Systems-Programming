#include "player.h"

int add_card(struct player* target, struct card* new_card){
	
	struct hand* tmpHand;  			// creating hand pointer object
	tmpHand = (struct hand*)malloc(sizeof(struct hand));   // creating hand object space
	
	if ( tmpHand == NULL ){ return -1;}  
	
	tmpHand->top = *new_card;   // top holds card to be added
	tmpHand->next = target->card_list;  // next points to current hand pointer

	target->card_list = tmpHand;   // pointer in hand updated
	target->hand_size++;  // increment hand size to know how many cards
 
	return 0;
/*	
 * Function: add_card
 * -------------------
 *  Add a new card to the player's hand. 
 *
 *  target: the target player
 *  new_card: pointer to the new card to add
 *
 *  returns: return 0 if no error, non-zero otherwise
 */
}


int remove_card(struct player* target, struct card* old_card){
	
	struct hand* iterator;  // creating iterating hand pointer to look for card
	struct hand* previous = NULL;  // remembering previous for when we delete

	iterator = target->card_list;   // starting at first card in hand
	
	if (iterator == NULL){ return -1;}  // No cards to Remove

	int Found = 0;
	while (iterator != NULL){   // while still more cards in linked list
	
		if(iterator->top.rank == old_card->rank && iterator->top.suit == old_card->suit ){   // if card is what we want to remove
			if(previous == NULL){ 
			target->card_list = iterator->next;
			target->hand_size--;
			Found = 1;
			}

			else{
			previous->next = iterator->next;
			target->hand_size--;      // decreasing hand size
			Found = 1;
			}
		}
			previous = iterator;
			iterator= iterator->next;
	}	
		

		if(Found == 1){
				free(iterator);   // have to free what we remove
			return 0;  // saying we DID find the card (return 1
				}
		else{
			return 1;     // not there ERROR, return 1
		}
/*
 * Function: remove_card
 * ---------------------
 *  Remove a card from the player's hand. 
 *
 *  target: the target player
 *  old_card: pointer to the old card to remove
 *
 *  returns: return 0 if no error, non-zero otherwise
 */

}

char check_add_book(struct player* target){
	
	int numCards = target->hand_size;
	if (numCards < 4){   // cant have group of 4 if less than 4 cards
		return 0;
	}
	else{
			int x = 1;
			int twoCount = 0; 
			int threeCount = 0;  // Initializing all counter for 4 of a kind
			int fourCount = 0;
			int fiveCount = 0;
			int sixCount = 0;
			int sevenCount = 0;
			int eightCount = 0;
			int nineCount = 0;
			int TCount = 0;
			int JCount = 0;
			int QCount = 0;
			int KCount = 0;
			int ACount = 0;

		struct hand* currCard;

		currCard = target->card_list;  // setting variable to hand pointer
		while(currCard != NULL){      // if reach last card or past 8 (shouldnt ever get to 8)

			if(currCard->top.rank == '2'){  // if rank ==2
				twoCount++;		// increment respective counter
				}
			if(currCard->top.rank == '3'){  // if rank ==3
				threeCount++;		// increment respective counter
				}
			if(currCard->top.rank == '4'){  // if rank ==4
				fourCount++;		// increment respective counter
				}
			if(currCard->top.rank == '5'){  // if rank ==5
				fiveCount++;		// increment respective counter
				}
			if(currCard->top.rank == '6'){  // if rank ==6
				sixCount++;		// increment respective counter
				}
			if(currCard->top.rank == '7'){  // if rank ==7
				sevenCount++;		// increment respective counter
				}
			if(currCard->top.rank == '8'){  // if rank ==8
				eightCount++;		// increment respective counter
				}
			if(currCard->top.rank == '9'){  // if rank ==9
				nineCount++;		// increment respective counter
				}
			if(currCard->top.rank == 'T'){  // if rank ==T
				TCount++;		// increment respective counter
				}
			if(currCard->top.rank == 'J'){  // if rank ==J
				JCount++;		// increment respective counter
				}
			if(currCard->top.rank == 'Q'){  // if rank ==Q
				QCount++;		// increment respective counter
				}
			if(currCard->top.rank == 'K'){  // if rank ==K
				KCount++;		// increment respective counter
				}
			if(currCard->top.rank == 'A'){  // if rank ==A
				ACount++;		// increment respective counter
				}
			currCard = currCard->next;	   // INCREMENT CARD INCREMENTOR 
			x++;	
	}
//printf("\n\nTwo(%d) Three(%d) Four(%d) Five(%d) Six(%d) Seven(%d) Eight(%d) Nine(%d) Tens(%d) Jacks(%d) Queesn(%d) Kings(%d) Aces(%d)\n\n", twoCount, threeCount, fourCount, fiveCount, sixCount, sevenCount, eightCount, nineCount, TCount, JCount, QCount, KCount, ACount);
		
		struct hand* deleter;
		int countToFour;

		if (target->numBook < 7){

		if(twoCount == 4){
			target->book[target->numBook] = '2';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour != 4){
				if(deleter->top.rank == '2'){
					remove_card(target, &(deleter->top));
					//printf("Removed %d 2's\n", countToFour);
				countToFour++;
				deleter = deleter->next;
				}
			else{
			deleter = deleter->next;
			}}}
		
		if(threeCount == 4){
			target->book[target->numBook] = '3';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == '3'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d 3's\n", countToFour);
			countToFour++;
			deleter = deleter->next;
}
			else{
			deleter = deleter->next;
			}}}

		if(fourCount == 4){
			target->book[target->numBook] = '4';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == '4'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d 4's\n", countToFour);
			countToFour++;
			deleter = deleter->next;
}
			else{
			deleter = deleter->next;
			}}}

		if(fiveCount == 4){
			target->book[target->numBook] = '5';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == '5'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d 5's\n", countToFour);
				countToFour++;
				deleter = deleter->next;
}
			else{
			deleter = deleter->next;
			}}}

		if(sixCount == 4){
			target->book[target->numBook] = '6';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == '6'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d 6's\n", countToFour);
			countToFour++;
			deleter = deleter->next;
}
			else{
			deleter = deleter->next;
			}}}

		if(sevenCount == 4){
			target->book[target->numBook] = '7';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == '7'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d 7's\n", countToFour);
			countToFour++;
			deleter = deleter->next;
}
			else{
			deleter = deleter->next;
			}}}

		if(eightCount == 4){
			target->book[target->numBook] = '8';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == '8'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d 8's\n", countToFour);
			countToFour++;
			deleter = deleter->next;
}
			else{
			deleter = deleter->next;
			}}}

		if(nineCount == 4){
			target->book[target->numBook] = '9';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == '9'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d 9's\n", countToFour);
			countToFour++;
			deleter = deleter->next;
}
			else{
			deleter = deleter->next;
			}}}

		if(TCount == 4){
			target->book[target->numBook] = 'T';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == 'T'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d T's\n", countToFour);
			countToFour++;
			deleter = deleter->next;
}
			else{
			deleter = deleter->next;
			}}}

		if(JCount == 4){
			target->book[target->numBook] = 'J';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == 'J'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d J's\n", countToFour);
			deleter = deleter->next;
			countToFour++;
			
}
			else{
			deleter = deleter->next;
			}}}

		if(QCount == 4){
			target->book[target->numBook] = 'Q';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == 'Q'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d Q's\n", countToFour);
			countToFour++;
			deleter = deleter->next;
}
			else{
			deleter = deleter->next;
			}}}

		if(KCount == 4){
			target->book[target->numBook] = 'K';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == 'K'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d K's\n", countToFour);
			deleter = deleter->next;
			countToFour++;
}
			else{
			deleter = deleter->next;
			}}}

		if(ACount == 4){
			target->book[target->numBook] = 'A';
			target->numBook++;
			deleter = target->card_list;
			countToFour = 0;
			while(countToFour < 4){
			if(deleter->top.rank == 'A'){
				remove_card(target, &(deleter->top));
				//printf("Removed %d A's\n", countToFour);
			deleter = deleter->next;
			countToFour++;
}
			else{
			deleter = deleter->next;
			}}}
			}  // end of if numBOok < 7
			else{ printf("\nNumbook is 7, END GAME\n");}
		
		}
return 0;
 }

/*
 * Function: check_add_book
 * ------------------------
 *  Check if a player has all 4 cards of the same rank.
 *  If so, remove those cards from the hand, and add the rank to the book.
 *  Returns after finding one matching set of 4, so should be called after adding each a new card.
 * 
 *  target: pointer to the player to check
 *  
 *  Return: a char that indicates the book that was added; return 0 if no book added.
 */

int search(struct player* target, char rank){
	struct hand* tempHand;
	tempHand = target->card_list;
	for(int i=0; i < target->hand_size; i++){
		if (tempHand->top.rank == rank){
	//printf("YES\n\n");
	return 1;
	}
	else{
	//printf("Not Found yet\n\n");
	tempHand = tempHand->next;

	}
}
return 0;	
/*
 * Function: search
 * ----------------
 *  Search a player's hand for a requested rank.
 *  
 *  rank: the rank to search for
 *  target: the player (and their hand) to search
 *
 *  Return: If the player has a card of that rank, return 1, else return 0
 */
}


int transfer_cards(struct player* src, struct player* dest, char inrank){

	int cTrans = 0;  // counter for number of cards transfered


		struct hand* currentCard;  // creating card to search through
		struct hand* nxtHand;  // creating a value to hold next pointer as we remove card

		currentCard = src->card_list;  // target's hand pointer
	
		while(currentCard != NULL ){  // find all cards (search through all cards in hand)
			if( currentCard->top.rank == inrank){

				add_card(dest, &currentCard->top);  // adding card to destination

				nxtHand = currentCard->next;  // saving data of next pointer into variable

				remove_card(src, &currentCard->top);  // remove card

				currentCard = nxtHand;  // load next pointer to look for more cards
				cTrans++;
			
				}
			else{
				currentCard = currentCard->next;  // going to next card
			}
		}
	return cTrans;
	
	
/*
 * Function: transfer_cards
 * ------------------------
 *   Transfer cards of a given rank from the source player's 
 *   hand to the destination player's hand. Remove transferred
 *   cards from the source player's hand. Add transferred cards
 *   to the destination player's hand.
 *   
 *   src: a pointer to the source player
 *   dest: a pointer to the destination player
 *   rank: the rank to transfer
 *
 *   Return: 0 if no cards found/transferred, <0 if error, otherwise 
 *   return value indicates number of cards transferred
 */   

	return 0; 
}

int game_over(struct player* target){
	int bookCount = 0;

	for(int i=0; i<7; i++){
		if(target->book[i] != 0){
		bookCount++;
		}
		else{
		}
	}
	if(bookCount == 7){
		return 1;
		}
	

	return 0; 


/*
 * Function: game_over
 * -------------------
 *   Boolean function to check if a player has 7 books yet
 *   and the game is over
 *
 *   target: the player to check
 *   
 *   Return: 1 if game is over, 0 if game is not over
 */
}

int reset_player(struct player* target){
	deck_instance.top_card = 0;
	for(int i=0; i<7; i++){
	target->book[i] = 0;
	}
	target->numBook = 0;

	while(target->hand_size !=0){
	remove_card(target, &target->card_list->top);
	}	
return 0;
}

char computer_play(struct player* target){
	
	srand(time(NULL));
	int randIndex = rand() % target->hand_size;
	char rankToPass;
	
	struct hand* tmpHand;
	tmpHand = target->card_list;
	int counter = 0;
	int Found = 0;
	
	while ( Found == 0){
		if (counter == randIndex){
			rankToPass = tmpHand->top.rank;
			Found = 1;
		}
		else{
			tmpHand = tmpHand->next;
			counter++;
		}
	}
/* 
 * Function: computer_play
 * -----------------------
 *
 *   Select a rank randomly to play this turn. The player must have at least
 *   one card of the selected rank in their hand.
 *
 *   target: the player's hand to select from
 *
 *   Rank: return a valid selected rank
 */

	return rankToPass;
}


char user_play(struct player* target){
	char guess[200]; // to hold rank of 
	int found = 0;
	
	while(found == 0 && user.numBook != 7 && computer.numBook !=7){		// for all ranks
		if (user.reset == 1){
		}
		else{
		printf("Player 1's Turn, enter a rank: ");
		}
		fgets(guess, 300, stdin);
		if(search(target, guess[0]) == 1){
			return guess[0];
			
		}
		else{	
			if(user.reset == 1){
				user.reset = 0;
			}
			else{
			printf("Error - must have at least one card from rank to play\n");
			}
		}
	
  	}
return 0;
	
/* 
 * Function: user_play
 * -------------------
 *
 *   Read standard input to get rank user wishes to play.  Must perform error
 *   checking to make sure at least one card in the player's hand is of the 
 *   requested rank.  If not, print out "Error - must have at least one card from rank to play"
 *   and then re-prompt the user.
 *
 *   target: the player's hand to check
 * 
 *   returns: return a valid selected rank
 */

}

