#ifndef UI_H
#define UI_H

#include "game.h"

void printCard(Card c);
void showHand(Hand *h, const char *name, int hidden);
void showResult(Game *g, Result r);

#endif
