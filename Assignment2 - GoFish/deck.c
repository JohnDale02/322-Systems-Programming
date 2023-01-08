#include "deck.h"

int shuffle(){
	
	// May need vaiable to check if game has been played, if yes, shuffle. if not... DONT DO ANYHTING
	printf("\n\nShuffling Deck....\n");

	char suits[4] = {'D','H','S','C'};  // Suits
	char ranks[13] = {'2','3','4','5','6','7','8','9','T','J','Q','K','A'}; // Ranks
	int count = 0; 	// create counter variable for 52 cards
	int i, j, y;

	for(i=0; i<4; i++) {
		for(j=0; j<13; j++) {

			deck_instance.list[count].suit = suits[i];
			deck_instance.list[count].rank = ranks[j];

			count++;  // increment counter
			deck_instance.top_card++; // need to increase top card counter
			}
		}// Initializing DECK RANK AND SUITS

	//deck_instance.top_card--; 						// need to have 51 to match array positions
	for(int x=0;x<52;x++){

		//printf("Card %d: = %c%c\n\n", x, deck_instance.list[x].suit, deck_instance.list[x].rank);  // pre- shuffle deck
	}

	srand(time(NULL));  							// initializing time

	for(y=0; y<52; y++){  							// For loop to change the positions of the cards randomly
		int randIndex = rand() % 51;   		        		// gives random index from 0-51
		struct card TempCard =	deck_instance.list[y];  		// creating temp card struct
		deck_instance.list[y] = deck_instance.list[randIndex]; 		// cardY = CardRand
		deck_instance.list[randIndex]  = TempCard;			// cardRand = CardY

	}// PREFORMS SHUFFLE switches index's of 52 cards

	//for(int b=0; b<52; b++){
	//printf("Card: %d: = %c%c\n\n", b, deck_instance.list[b].suit, deck_instance.list[b].rank); }//printing Deck

	if( (y!=52) || (i!=4) || (j!=13) ){   // check if every loop finished 
		printf("SHUFFLE NOT DONE CORRECTLY\n\n");
		return 1;
	}
	
	else{
		return 0;
	}

/* * Function: shuffle
 * --------------------
 *  Initializes deck_instance and shuffles it.
 *  Resets the deck if a prior game has been played.
 * 
 *  returns: 0 if no error, and non-zero on error
 */ 

}  // End of SHUFFLE


int deal_player_cards(struct player* target){

	for(int x=0; x<7; x++){

	add_card(target,next_card());   // adding next card on deck to hand of target
	}
	
	if(deck_instance.top_card == 45 || deck_instance.top_card == 38){

	return 0;
	}
	else{
		printf("CARDS NOT DEALT CORRECTLY\n\n");
		 return 1;}
/*
 * Function: deal_player_cards
 * ---------------------------
 *  Deal 7 random cards to the player specified in the function.
 *  Remove the dealt cards from the deck. 
 *
 *  target: pointer to the player to be dealt cards
 *
 *  returns: 0 if no error, and non-zero on error
 */
}

struct card* next_card( ){
		deck_instance.top_card--;
	return &(deck_instance.list[deck_instance.top_card]);  // returning address of top card in deck
	
/*
 * Function: next_card
 * -------------------
 *  Return a pointer to the top card on the deck.
 *  Removes that card from the deck. 
 *
 *  returns: pointer to the top card on the deck.
 */
}




size_t deck_size( ){
	
	printf("deck size: %d\n", deck_instance.top_card);  
	return deck_instance.top_card;

/*
 * Function: size
 * --------------
 *  Return the number of cards left in the current deck.
 *
 *  returns: number of cards left in the deck.
 */
	

}

