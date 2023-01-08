#ifndef CARD_H
#define CARD_H

/*
  Valid suits: C, D, H, and S
  Valid ranks: 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K, A
*/
struct card   // use to create cards, which we will load into deck
{
  char suit;    // suit (C,D,H,S)
  char rank;   // two chars for rank (for 10)
};

/*
  Linked list of cards in hand.
    top: first card in hand
    next: pointer to next card in hand
*/
struct hand
{
  struct card top;    // first card in hand (set with handinst.top = *(add_card))
  struct hand* next;    // next card in hand
};

#endif
