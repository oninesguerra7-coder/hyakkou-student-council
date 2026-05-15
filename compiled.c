#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

char* drawCard();
int player_turn();
char check_win();
int handValue(char hand[][3], int count);
void printHand(char hand[][3], int count, int hideFirst);

struct game {

    char p_hand[15][3];
    char d_hand[15][3];

    int p_count;
    int d_count;

    int chips;
};

struct game hands;

int hidedealer;

int main() {
    int round = 0; // round counter
    while (1) {

        // intitialize hands to 0 (reset every round)
        hands.p_count = 0;
        hands.d_count = 0;
        hidedealer = 1;

        // initial player cards
        strcpy(hands.p_hand[hands.p_count++], drawCard());
        strcpy(hands.p_hand[hands.p_count++], drawCard());

        // initial dealer cards
        strcpy(hands.d_hand[hands.d_count++], drawCard());
        strcpy(hands.d_hand[hands.d_count++], drawCard());

        round++;
        printf("\n| ROUND %d |\n\n", round);

        // display player hand
        printf("Player hand: ");
        printHand(hands.p_hand, hands.p_count, 0);
        printf("Value: %d\n\n",
               handValue(hands.p_hand, hands.p_count));

        // display dealer hand
        printf("Dealer hand: ");
        printHand(hands.d_hand, hands.d_count, hidedealer);
        printf("Value: ?\n\n");

        while (1) {

            char result = check_win(); // early check for 21

            if (result == 'p') {
                hidedealer = 0;
                printf("Dealer hand: ");
                printHand(hands.d_hand, hands.d_count, hidedealer);
                printf("Player wins!\n");
                break;
            }

            else if (result == 'd') {
                hidedealer = 0;
                printf("Dealer hand: ");
                printHand(hands.d_hand, hands.d_count, hidedealer);
                printf("Dealer wins!\n");
                break;
            }

            else if (result == '0') {
                hidedealer = 0;
                printf("Player busts!\n");
                printHand(hands.d_hand, hands.d_count, hidedealer);
                break;
            }

            else if (result == '1') {
                hidedealer = 0;
                printf("Dealer busts!\n");
                printHand(hands.d_hand, hands.d_count, hidedealer);
                break;
            }

            else if (result == 't') {
                hidedealer = 0;
                printf("Tie!\n");
                printHand(hands.d_hand, hands.d_count, hidedealer);
                break;
            }

            int stand = player_turn();

            if (stand) {

                hidedealer = 0;

                int d = handValue(hands.d_hand, hands.d_count);

                printf("\nDealer hits.\n\n");

                while (d < 17) {

                    strcpy(hands.d_hand[hands.d_count], drawCard());
                    hands.d_count++;

                    printf("Dealer drew: %s\n",
                           hands.d_hand[hands.d_count - 1]);

                    d = handValue(hands.d_hand, hands.d_count);
                }

                break;
            }
        }
    }

    return 0;
}

int handValue(char hand[][3], int count) {

    int total = 0;
    int aces = 0;

    for (int i = 0; i < count; i++) {

        char rank = hand[i][0];

        if (rank >= '2' && rank <= '9') {
            total += rank - '0';
        }

        else if (rank == 'T' ||
                 rank == 'J' ||
                 rank == 'Q' ||
                 rank == 'K') {

            total += 10;
        }

        else if (rank == 'A') {

            total += 11;
            aces++;
        }
    }

    while (total > 21 && aces > 0) {

        total -= 10;
        aces--;
    }

    return total;
}

char check_win() {

    int p = handValue(hands.p_hand, hands.p_count);
    int d = handValue(hands.d_hand, hands.d_count);

    return (p == 21 && d == 21) ? 't' :
           (p == 21) ? 'p' :
           (d == 21) ? 'd' :
           (p > 21) ? '0' :
           (d > 21) ? '1' :
           'n';
}

int player_turn() {

    char choice;

    printf("Hit (h) or Stand (s): ");
    scanf(" %c", &choice);

    if (choice == 'h' || choice == 'H') {

        strcpy(hands.p_hand[hands.p_count], drawCard());
        hands.p_count++;

        printf("\nYou drew: %s\n",
               hands.p_hand[hands.p_count - 1]);

        printf("\nPlayer hand: ");
        printHand(hands.p_hand, hands.p_count, 0);

        printf("Value: %d\n\n",
               handValue(hands.p_hand, hands.p_count));

        printf("Dealer hand: ");
        printHand(hands.d_hand, hands.d_count, hidedealer);

        printf("Value: ?\n\n");

        return 0; // continue round
    }

    else if (choice == 's' || choice == 'S') {

        hidedealer = 0;

        int p = handValue(hands.p_hand, hands.p_count);
        int d = handValue(hands.d_hand, hands.d_count);

        printf("\nPlayer stands.\n\n");

        printf("Player hand: ");
        printHand(hands.p_hand, hands.p_count, 0);
        printf("Value: %d\n\n", p);

        printf("Dealer hand: ");
        printHand(hands.d_hand, hands.d_count, hidedealer);
        printf("Value: %d\n\n", d);

        if (d < 17) {

            printf("Dealer hits.\n\n");

            while (d < 17) {

                strcpy(hands.d_hand[hands.d_count], drawCard());
                hands.d_count++;

                printf("Dealer drew: %s\n",
                       hands.d_hand[hands.d_count - 1]);

                d = handValue(hands.d_hand, hands.d_count);
            }
        }

        if (p > d)
            printf("Player wins!\n");

        else if (d > p)
            printf("Dealer wins!\n");

        else
            printf("Tie!\n");

        return 1; // end round
    }

    else {
        printf("\nInvalid input. Use 'h' or 's'.\n\n");
        return 0; // let player try again
    }
}

void printHand(char hand[][3], int count, int hideFirst) {

    for (int i = 0; i < count; i++) {

        if (i == 0 && hideFirst) {
            printf("?? ");
        }

        else {
            printf("%s ", hand[i]);
        }
    }

    printf("\n");
}

char *drawCard() {

    static int deck[52];
    static int initialized = 0;
    static int index = 0;

    static char drawnCard[3];

    char suits[] = {'h', 'd', 'c', 's'};

    char ranks[] = {
        '2', '3', '4', '5',
        '6', '7', '8', '9',
        'T', 'J', 'Q', 'K', 'A'
    };

    if (!initialized) {

        srand(time(NULL));

        for (int i = 0; i < 52; i++)
            deck[i] = i;

        for (int i = 0; i < 52; i++) {

            int j = rand() % 52;

            int temp = deck[i];
            deck[i] = deck[j];
            deck[j] = temp;
        }

        initialized = 1;
    }

    if (index >= 52) {

        initialized = 0;
        index = 0;

        return drawCard();
    }

    int card = deck[index++];

    int suitIndex = card / 13;
    int rankIndex = card % 13;

    drawnCard[0] = ranks[rankIndex];
    drawnCard[1] = suits[suitIndex];
    drawnCard[2] = '\0';

    return drawnCard;
}
