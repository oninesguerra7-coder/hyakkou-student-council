#include <stdio.h>
#include "game.h"

int main() {

    Game game;

    initializeGame(&game);

    while (game.money > 0) {
        playRound(&game);
    }

    printf("Game over.\n");

    return 0;
}
