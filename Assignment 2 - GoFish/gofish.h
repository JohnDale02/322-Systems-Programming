#ifndef GOFISH_H
#define GOFISH_H

#include <stdlib.h>
#include "player.h"
#include "card.h"
#include "deck.h"
#include <time.h>

int callGame(); 


int printHand(struct player* target);
int printCHand(struct player* target);

int printBook(struct player* target);
int printCBook(struct player* target);

int printHB();

int printRank(struct player* target, char r);

int askForCard();
int CaskForCard();

int whoWon();
int playAgain();

int check_books();


/*
   Define any prototype functions
   for gofish.h here.
*/

#endif
