#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

// ANSI Color Codes
#define RED   "[31m"
#define GRN   "[32m"
#define YEL   "[33m"
#define CYN   "[36m"
#define MGT   "[35m"
#define RESET "[0m"

// Game Constants
#define MAX_CARDS 15
#define DECK_SIZE 52
#define BLACKJACK 21
#define DEALER_STAND 17
#define STARTING_MONEY 1000

// Represents a player/dealer
// Stores:
// - hand
// - number of cards
// - current total

typedef struct {

    char hand[MAX_CARDS][3];
    int count;
    int total;

} Player;

// Represents the entire game state

typedef struct {

    Player player;
    Player dealer;

    int hideDealer;
    int round;

} Game;

// Represents the deck
// deck[] stores shuffled card indices
// index tracks next card to draw

typedef struct {

    int deck[DECK_SIZE];
    int index;

} Deck;

// Function Prototypes

void initializeDeck(Deck *deck);
void drawCard(Deck *deck, char *dest);
void addCard(Player *p, Deck *deck);
void updateTotal(Player *p);
void printHand(Player *p, int hideFirst);
void displayBoard(Game *g);
void displayFinal(Game *g);
int playerTurn(Game *g, Deck *deck);
void dealerTurn(Game *g, Deck *deck);
char determineWinner(Game *g);
void updateMoney(int *money, int bet, char result);
void results(Game *g, char result);
void clearInputBuffer();
char getChoice(const char *prompt, const char valid1, const char valid2);
int getBet(int money);

int main() {

    Game game;
    Deck deck;

    int money = STARTING_MONEY;

    // Shuffle and initialize deck
    initializeDeck(&deck);

    // Main game loop
    while (money > 0) {

        int bet;
        char result;

        // Reset game state every round
        game.round++;
        game.hideDealer = 1;

        game.player.count = 0;
        game.player.total = 0;

        game.dealer.count = 0;
        game.dealer.total = 0;

        printf("
%sYou have $%d%s
", CYN, money, RESET);

        // Ask player for bet
        bet = getBet(money);

        // Initial card dealing
        addCard(&game.player, &deck);
        addCard(&game.dealer, &deck);
        addCard(&game.player, &deck);
        addCard(&game.dealer, &deck);

        printf("
%s| ROUND %d |%s
", CYN, game.round, RESET);

        displayBoard(&game);

        // Check immediate blackjack/bust
        result = determineWinner(&game);

        if (result != 'n') {

            updateMoney(&money, bet, result);
            results(&game, result);
            continue;
        }

        // Double down option
        char dd = getChoice("Double down? (y/n): ", 'y', 'n');

        if (dd == 'y') {

            // Ensure enough money exists
            if (bet * 2 > money) {
                printf("%sNot enough money to double down.%s
", RED, RESET);
            }
            else {

                // Double bet amount
                bet *= 2;

                // Player draws exactly one card
                addCard(&game.player, &deck);

                printf("
You doubled down and drew: %s%s%s
",
                       YEL,
                       game.player.hand[game.player.count - 1],
                       RESET);

                displayBoard(&game);

                // Immediate bust/blackjack check
                result = determineWinner(&game);

                // Dealer only plays if game not over
                if (result == 'n') {
                    dealerTurn(&game, &deck);
                    result = determineWinner(&game);
                }

                updateMoney(&money, bet, result);
                results(&game, result);

                continue;
            }
        }

        // Player action loop
        while (1) {

            int stand = playerTurn(&game, &deck);

            // Immediate result checking
            result = determineWinner(&game);

            if (result != 'n') {

                updateMoney(&money, bet, result);
                results(&game, result);
                break;
            }

            // Dealer plays after stand
            if (stand) {

                dealerTurn(&game, &deck);

                result = determineWinner(&game);

                updateMoney(&money, bet, result);
                results(&game, result);
                break;
            }
        }
    }

    printf("
%sYou ran out of money! Game Over.%s
", RED, RESET);

    return 0;
}

// Initializes and shuffles deck using Fisher-Yates shuffle

void initializeDeck(Deck *deck) {

    srand(time(NULL));

    // Fill deck with ordered values
    for (int i = 0; i < DECK_SIZE; i++) {
        deck->deck[i] = i;
    }

    // Shuffle deck
    for (int i = DECK_SIZE - 1; i > 0; i--) {

        int j = rand() % (i + 1);

        int temp = deck->deck[i];
        deck->deck[i] = deck->deck[j];
        deck->deck[j] = temp;
    }

    deck->index = 0;
}

// Draws one card from deck
// Stores card string into destination buffer

void drawCard(Deck *deck, char *dest) {

    char suits[] = {'h', 'd', 'c', 's'};

    char ranks[] = {
        '2', '3', '4', '5',
        '6', '7', '8', '9',
        'T', 'J', 'Q', 'K', 'A'
    };

    // Reshuffle when deck is empty
    if (deck->index >= DECK_SIZE) {
        initializeDeck(deck);
    }

    int card = deck->deck[deck->index++];

    int suitIndex = card / 13;
    int rankIndex = card % 13;

    dest[0] = ranks[rankIndex];
    dest[1] = suits[suitIndex];
    dest[2] = '�';
}

// Adds a card to a player's hand
// Updates total immediately

void addCard(Player *p, Deck *deck) {

    drawCard(deck, p->hand[p->count]);
    p->count++;

    updateTotal(p);
}

// Computes blackjack hand value
// Properly handles ace conversion (11 -> 1)

void updateTotal(Player *p) {

    int total = 0;
    int aces = 0;

    for (int i = 0; i < p->count; i++) {

        char rank = p->hand[i][0];

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

    // Convert aces from 11 to 1 if busting
    while (total > BLACKJACK && aces > 0) {

        total -= 10;
        aces--;
    }

    p->total = total;
}

// Prints hand
// Can optionally hide first dealer card

void printHand(Player *p, int hideFirst) {

    for (int i = 0; i < p->count; i++) {

        if (i == 0 && hideFirst) {
            printf("%s??%s ", YEL, RESET);
        }
        else {
            printf("%s%s%s ", YEL, p->hand[i], RESET);
        }
    }

    printf("
");
}

// Displays current game board

void displayBoard(Game *g) {

    printf("
Player Hand: ");
    printHand(&g->player, 0);
    printf("Value: %s%d%s

", MGT, g->player.total, RESET);

    printf("Dealer Hand: ");
    printHand(&g->dealer, g->hideDealer);

    if (g->hideDealer) {
        printf("Value: %s?%s

", MGT, RESET);
    }
    else {
        printf("Value: %s%d%s

", MGT, g->dealer.total, RESET);
    }
}

// Reveals final board state

void displayFinal(Game *g) {

    g->hideDealer = 0;

    printf("
Final Board:
");

    displayBoard(g);
}
