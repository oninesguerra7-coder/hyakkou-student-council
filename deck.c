#include "deck.h"

#include <stdlib.h>

void shuffleDeck(Game *g) {
    // Start from last card and swap backwards
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        
        int j = rand() % (i + 1); // random index
        // swap cards
        Card temp = g->deck[i];
        g->deck[i] = g->deck[j];
        g->deck[j] = temp;
    }
    g->deckIndex = 0;
}

Card drawCard(Game *g) { // to draw one card

    if (g->deckIndex >= DECK_SIZE)
        shuffleDeck(g); // reshuffle deck

    return g->deck[g->deckIndex++];
}
