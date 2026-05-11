#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PLAYER 0
#define DEALER 1

// Function Prototypes
void deal_card(int *total);
void show_hand(char *name, int total);
int dealer_logic(int *dealer_total);
int check_blackjack(int total);
int check_bust(int total);

int main() {

    int player_total = 0;
    int dealer_total = 0;

    int choice;

    srand(time(NULL));

    printf("=== BLACKJACK ===\n\n");

    // Initial Cards
    deal_card(&player_total);
    deal_card(&player_total);

    deal_card(&dealer_total);
    deal_card(&dealer_total);

    printf("Dealer has:\n");
    printf("[Hidden Card]\n");
    printf("Visible Total: %d\n\n", dealer_total / 2);

    show_hand("Player", player_total);

    // Player Turn
    while (1) {

        if (check_blackjack(player_total)) {
            printf("BLACKJACK! You win!\n");
            return 0;
        }

        if (check_bust(player_total)) {
            printf("You busted! Dealer wins.\n");
            return 0;
        }

        printf("\n1. Hit\n");
        printf("2. Stand\n");
        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 1) {

            deal_card(&player_total);

            printf("\nYou drew a card.\n");
            show_hand("Player", player_total);

        } else if (choice == 2) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }

    // Dealer Turn
    printf("\n=== DEALER TURN ===\n");

    show_hand("Dealer", dealer_total);

    dealer_logic(&dealer_total);

    // Final Results
    printf("\n=== FINAL RESULTS ===\n");

    show_hand("Player", player_total);
    show_hand("Dealer", dealer_total);

    if (dealer_total > 21) {
        printf("Dealer busted! You win!\n");
    } else if (player_total > dealer_total) {
        printf("You win!\n");
    } else if (dealer_total > player_total) {
        printf("Dealer wins!\n");
    } else {
        printf("It's a tie!\n");
    }

    return 0;
}

// Adds random card to total using pointers
void deal_card(int *total) {

    int card = (rand() % 13) + 1;

    // Face cards = 10
    if (card > 10) {
        card = 10;
    }

    // Ace = 11
    else if (card == 1) {
        card = 11;
    }

    *total += card;

    printf("Card drawn: %d\n", card);
}

// Displays current hand total
void show_hand(char *name, int total) {

    printf("%s Total: %d\n", name, total);
}

// Dealer hits until 17 or higher
int dealer_logic(int *dealer_total) {

    while (*dealer_total < 17) {

        printf("Dealer hits...\n");

        deal_card(dealer_total);

        printf("Dealer Total: %d\n", *dealer_total);
    }

    printf("Dealer stands.\n");

    return *dealer_total;
}

// Checks for blackjack
int check_blackjack(int total) {

    if (total == 21) {
        return 1;
    }

    return 0;
}

// Checks for bust
int check_bust(int total) {

    if (total > 21) {
        return 1;
    }

    return 0;
}