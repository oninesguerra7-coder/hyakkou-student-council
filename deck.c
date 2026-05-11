//202513231
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char* deck(int choice); // function prototype

int main() {
    int choice;

    srand(time(NULL)); // seed random once

    while (1) {
        printf("Hit(1) or Stand(0)? ");
        scanf("%d", &choice);

        char* card = deck(choice);

        if (choice == 1) {
            if (card != NULL)
                printf("%s\n", card);
        }

        if (choice == 0) {
            break;
        }
    }

    return 0;
}

char* deck(int choice) {
    static int deck[52];
    static int initialized = 0;
    static int index = 0;
    static char drawnCard[3];

    char suits[] = {'h', 'd', 'c', 's'};
    char ranks[] = {'2', '3', '4', '5', '6', '7',
                    '8', '9', 'T', 'J', 'Q', 'K', 'A'};

    // Initialize and shuffle deck only once
    if (!initialized) {
        for (int i = 0; i < 52; i++)
            deck[i] = i;

        // Shuffle deck
        for (int i = 0; i < 52; i++) {
            int j = rand() % 52;
            int temp = deck[i];
            deck[i] = deck[j];
            deck[j] = temp;
        }

        initialized = 1;
    }

    // No more cards
    if (index >= 52) {
        printf("Out of cards!\n");
        return NULL;
    }

    // Draw card
    int card = deck[index++];
    int suitIndex = card / 13;
    int rankIndex = card % 13;

    drawnCard[0] = ranks[rankIndex];
    drawnCard[1] = suits[suitIndex];
    drawnCard[2] = '\0';

    return drawnCard;
}
