#include "game.h"
#include "deck.h"
#include "hand.h"
#include "ui.h"
#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void initializeGame(Game *g) {

    g->money = STARTING_MONEY;
    
    char suits[] = {'h', 'd', 'c', 's'};
    char ranks[] = {
        '2', '3', '4', '5',
        '6', '7', '8', '9',
        'T', 'J', 'Q', 'K', 'A'
    };

    int index = 0;
    
    for (int s = 0; s < 4; s++) {

        for (int r = 0; r < 13; r++) {
            // assign rank/suit
            g->deck[index].rank = ranks[r];
            g->deck[index].suit = suits[s];

            index++;
        }
    }
    
    srand(time(NULL));
    shuffleDeck(g);
}

void initialDeal(Game *g) {
// deal 2 cards initially
    for (int i = 0; i < 2; i++) {

        addCard(&g->player, drawCard(g));
        addCard(&g->dealer, drawCard(g));
    }
}

Result checkImmediate(Game *g) {

    int p = g->player.total;
    int d = g->dealer.total;

    int playerBlackjack =
        (p == BLACKJACK &&
         g->player.count == 2); // check if only 2 cards and 21

    int dealerBlackjack =
        (d == BLACKJACK &&
         g->dealer.count == 2);

    // check for bust
    if (p > BLACKJACK)
        return PLAYER_BUST;

    if (d > BLACKJACK)
        return DEALER_BUST;
    
    //check for blackjack
    if (playerBlackjack &&
        dealerBlackjack)

        return TIE;

    if (playerBlackjack)
        return PLAYER_WIN;

    if (dealerBlackjack)
        return DEALER_WIN;

    return NO_RESULT;
}

Result determineWinner(Game *g) {

    int p = g->player.total; // store player total in p
    int d = g->dealer.total; // store dealer total in d

    if (p > d)
        return PLAYER_WIN;

    if (d > p)
        return DEALER_WIN;

    return TIE;
}

void dealerTurn(Game *g) {

    printf("\nDealer turn...\n\n");

    while (g->dealer.total < DEALER_STAND) { // hit until 17

        addCard(&g->dealer, drawCard(g));

        printf("Dealer drew: ");
        printCard(g->dealer.cards[g->dealer.count - 1]);
        printf("\n");
    }
    
    printf("\n");
}

int playerTurn(Game *g) {

    char choice = getChoice(
        "Hit (h) or Stand (s): ",
        "hs"
    );

    /*
        Player hits
    */
    if (choice == 'h') {

        addCard(&g->player, drawCard(g));

        printf("\nYou drew: ");

        printCard(g->player.cards[g->player.count - 1]);

        printf("\n\n");

        showHand(&g->player, "Player", 0);
        showHand(&g->dealer, "Dealer", 1);

        return 0; // continue playing
    }

    printf("\nPlayer stands.\n\n");

    return 1; // else stop round
}

void updateMoney(Game *g, Result r) {

    if (r == PLAYER_WIN ||
        r == DEALER_BUST) {
    // add money to player
        g->money += g->bet;
    }
        
    else if (r == DEALER_WIN ||
             r == PLAYER_BUST) {
    // deduct money from player
        g->money -= g->bet;
    }
}

void playRound(Game *g) {

    static int round = 1;

    printf("\n%s| ROUND %d |%s\n\n", CYN,
           round++,
           RESET);

    printf("%sMoney: $%d%s\n\n", CYN,
           g->money,
           RESET);

    while (1) { // input validation

        printf("Enter bet amount: ");

        if (scanf("%d", &g->bet) != 1) {
        // if invalid input
            clearInputBuffer();

            printf("Invalid input.\n");

            continue;
        }

        clearInputBuffer();

        if (g->bet <= 0) {

            printf("Bet must be positive.\n");
            continue;
        }

        if (g->bet > g->money) {

            printf("Not enough money.\n");
            continue;
        }

        break;
    }
    
    resetHand(&g->player);
    resetHand(&g->dealer);

    initialDeal(g);

    showHand(&g->player, "Player", 0);
    showHand(&g->dealer, "Dealer", 1);

    Result result = checkImmediate(g);
    
    if (result != NO_RESULT) {  // immediate blackjack / bust

        updateMoney(g, result);

        showResult(g, result);

        return;
    }

    char dd = getChoice("Double down? (y/n): ", "yn");

    if (dd == 'y') {

        if (g->bet * 2 > g->money) {
            // if enough money to double down
            printf("Not enough money to double down.\n");
        }

        else {
            
            g->bet *= 2; // double the bet
            addCard(&g->player, drawCard(g)); // draw a card

            printf("\nYou drew: ");
            printCard(g->player.cards[g->player.count - 1]); // display drawn card
            printf("\n\n");
            showHand(&g->player, "Player", 0);

            result = checkImmediate(g);

            if (result == NO_RESULT) {
            // start dealer turn
                dealerTurn(g);

                result = checkImmediate(g);

                if (result == NO_RESULT)
                    result = determineWinner(g);
            }

            updateMoney(g, result);
            showResult(g, result);

            return;
        }
    }

    int stand = 0;

    while (!stand) { // until the player stands

        stand = playerTurn(g);
        result = checkImmediate(g);

        if (result != NO_RESULT) {

            updateMoney(g, result);
            showResult(g, result);
            return;
            
        }
    }
    
    dealerTurn(g);

    result = checkImmediate(g);

    if (result == NO_RESULT)
        result = determineWinner(g);

    updateMoney(g, result);
    showResult(g, result);
}
