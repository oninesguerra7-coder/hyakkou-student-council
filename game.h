#ifndef GAME_H
#define GAME_H

#include "constants.h"

typedef enum {
    NO_RESULT,
    PLAYER_WIN,
    DEALER_WIN,
    PLAYER_BUST,
    DEALER_BUST,
    TIE
} Result;

typedef struct {
    char rank;
    char suit;
} Card;

typedef struct {
    Card cards[MAX_CARDS];
    int count;
    int total;
    int aces;
} Hand;

typedef struct {
    Hand player;
    Hand dealer;

    int money;
    int bet;

    Card deck[DECK_SIZE];
    int deckIndex;
} Game;

void initializeGame(Game *g);
void playRound(Game *g);

#endif
