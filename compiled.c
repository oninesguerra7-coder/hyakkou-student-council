#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define CYN   "\x1B[36m"
#define MGT   "\x1B[35m"
#define RESET "\x1B[0m"

char* drawCard();
int player_turn();
void results(char result);
char check_win(int stand);
int handValue(char hand[][3], int count);
void printHand(char hand[][3], int count, int hideFirst);

struct game {

    char p_hand[15][3];
    char d_hand[15][3];

    int p_count;
    int d_count;
};

struct game hands;

int hidedealer;

int main() {

    int round = 0;
    int money = 1000;
    while (1) {

        int bet;

        printf("\n%sYou have $%d.%s\n", CYN, money, RESET);
        printf("Input amount to bet: ");
        scanf("%d", &bet);

        if (bet > money) {
            printf("You don't have enough money to bet that amount.\n");
            continue;
        }
        
        hands.p_count = 0;
        hands.d_count = 0;
        hidedealer = 1;

        // initial deal
        for (int i = 0; i < 2; i++) {

            strcpy(hands.p_hand[hands.p_count++], drawCard());
            strcpy(hands.d_hand[hands.d_count++], drawCard());
        }

        round++;

        int p = handValue(hands.p_hand, hands.p_count);

        printf("\n%s| ROUND %d |%s\n\n", CYN, round, RESET);

        printf("Player hand: ");
        printHand(hands.p_hand, hands.p_count, 0);
        printf("Value: %s%d%s\n\n", MGT, p, RESET);

        printf("Dealer hand: ");
        printHand(hands.d_hand, hands.d_count, hidedealer);
        printf("Value: %s?%s\n\n", MGT, RESET);

        while (1) {

            int stand = player_turn();

            char result = check_win(stand);

            if (result != 'n') {

                // update money
                if (result == 'p' || result == '1') {
                    money += bet;
                }
                else if (result == 'd' || result == '0') {
                    money -= bet;
                }

                results(result);

                printf("%sMoney: $%d%s\n",
                    CYN, money, RESET);

                // game over if broke
                if (money <= 0) {
                    printf("%sYou ran out of money! Game over.%s\n",
                        RED, RESET);
                    return 0;
                }

                break;
            }
        }
    }

    return 0;
}

void results(char result) {

    int d = handValue(hands.d_hand, hands.d_count);

    hidedealer = 0;

    printf("\nDealer final hand: ");
    printHand(hands.d_hand, hands.d_count, hidedealer);
    printf("Value: %s%d%s\n\n", MGT, d, RESET);

    switch(result) {

        case 'p':
            printf("%sPlayer wins!%s\n", GRN, RESET);
            break;

        case 'd':
            printf("%sDealer wins!%s\n", RED, RESET);
            break;

        case '0':
            printf("%sPlayer busts!%s\n", RED, RESET);
            break;

        case '1':
            printf("%sDealer busts!%s\n", GRN, RESET);
            break;

        case 't':
            printf("%sTie!%s\n", YEL, RESET);
            break;
    }
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

char check_win(int stand) {

    int p = handValue(hands.p_hand, hands.p_count);
    int d = handValue(hands.d_hand, hands.d_count);

    if (p > 21) return '0';
    if (d > 21) return '1';

    if (p == 21 && d == 21) return 't';
    if (p == 21) return 'p';
    if (d == 21) return 'd';

    if (stand) {

        while (d < 17) {

            strcpy(hands.d_hand[hands.d_count++], drawCard());

            printf("\n%sDealer drew:%s %s%s%s\n",
                   CYN, RESET,
                   YEL, hands.d_hand[hands.d_count - 1], RESET);

            d = handValue(hands.d_hand, hands.d_count);
        }

        if (p > d) return 'p';
        if (d > p) return 'd';

        return 't';
    }

    return 'n';
}

int player_turn() {

    char choice;

    printf("Hit (h) or Stand (s): ");
    scanf(" %c", &choice);

    if (choice == 'h') {

        strcpy(hands.p_hand[hands.p_count++], drawCard());

        // recompute AFTER drawing
        int p = handValue(hands.p_hand, hands.p_count);

        printf("\nYou drew: %s%s%s\n",
               YEL, hands.p_hand[hands.p_count - 1], RESET);

        printf("\nPlayer hand: ");
        printHand(hands.p_hand, hands.p_count, 0);
        printf("Value: %s%d%s\n\n", MGT, p, RESET);

        printf("Dealer hand: ");
        printHand(hands.d_hand, hands.d_count, hidedealer);
        printf("Value: %s?%s\n\n", MGT, RESET);

        return 0;
    }

    else if (choice == 's') {

        hidedealer = 0;

        int p = handValue(hands.p_hand, hands.p_count);
        int d = handValue(hands.d_hand, hands.d_count);

        printf("\nPlayer stands.\n\n");

        printf("Player hand: ");
        printHand(hands.p_hand, hands.p_count, 0);
        printf("Value: %s%d%s\n\n", MGT, p, RESET);

        printf("Dealer hand: ");
        printHand(hands.d_hand, hands.d_count, hidedealer);
        printf("Value: %s%d%s\n\n", MGT, d, RESET);

        return 1;
    }

    else {

        printf("\nInvalid input. Use 'h' or 's'.\n\n");
        return 0;
    }
}

void printHand(char hand[][3], int count, int hideFirst) {

    for (int i = 0; i < count; i++) {

        if (i == 0 && hideFirst) {
            printf("%s??%s ", YEL, RESET);
        }

        else {
            printf("%s%s%s ", YEL, hand[i], RESET);
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

        // Fisher-Yates shuffle
        for (int i = 51; i > 0; i--) {

            int j = rand() % (i + 1);

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
