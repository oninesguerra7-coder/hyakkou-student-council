#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// ANSI color codes
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define CYN   "\x1B[36m"
#define MGT   "\x1B[35m"
#define RESET "\x1B[0m"
// game constants
#define MAX_CARDS      15
#define DECK_SIZE      52
#define BLACKJACK      21
#define DEALER_STAND   17
#define STARTING_MONEY 1000

typedef enum {
    // possible round outcomes
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

    int count; // number of cards currently held
    int total; // current blackjack value
    int aces; // number of aces currently valued as 11

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

void shuffleDeck(Game *g);
Card drawCard(Game *g);

void resetHand(Hand *h);
void addCard(Hand *h, Card c);

void printCard(Card c);
void showHand(Hand *h, const char *name, int hidden);

void initialDeal(Game *g);

Result checkImmediate(Game *g);
Result determineWinner(Game *g);

void dealerTurn(Game *g);
int playerTurn(Game *g);

void updateMoney(Game *g, Result r);

void showResult(Game *g, Result r);

void playRound(Game *g);

void clearInputBuffer();
char getChoice(const char *prompt, const char *valid);

int main() {

    Game game;
    initializeGame(&game);
    
    while (game.money > 0) { // while player still has money

        playRound(&game);
    }

    printf("\n%sYou ran out of money! Game over.%s\n",
           RED, RESET);

    return 0;
}


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

void resetHand(Hand *h) {

    h->count = 0;
    h->total = 0;
    h->aces = 0;
}

void addCard(Hand *h, Card c) {

    h->cards[h->count++] = c; // store card in next available slot
    
    if (c.rank >= '2' && c.rank <= '9') {

        h->total += c.rank - '0'; // char to integer
    }
    // face cards
    else if (c.rank == 'T' ||
             c.rank == 'J' ||
             c.rank == 'Q' ||
             c.rank == 'K') {

        h->total += 10;
    }
        
    else if (c.rank == 'A') { // ace is initially 11

        h->total += 11;
        h->aces++;
    }

    while (h->total > BLACKJACK && h->aces > 0) {
    // change ace value to 1 if hand > 21
        h->total -= 10;
        h->aces--;
    }
}

void printCard(Card c) {
    printf("%s%c%c%s", YEL, c.rank, c.suit, RESET);
}

void showHand(Hand *h, const char *name, int hidden) {

    printf("%s hand: ", name);
    
    for (int i = 0; i < h->count; i++) {
        
        if (i == 0 && hidden) { // hide dealer first card

            printf("%s[?]%s ", YEL, RESET);
        }

        else {

            printCard(h->cards[i]);
            printf(" ");
        }
    }

    printf("\n");
    
    if (hidden) {

        char r = h->cards[1].rank; // dealer's visible card

        int value = (r >= '2' && r <= '9') ? // if 2 < value < 9
                    r - '0' : (r == 'A') ? // display value
                    11 : 10; // 11 if ace, 10 if face card

        printf("Value: %s%d+%s\n\n", MGT, value, RESET);
    }

    else
        printf("Value: %s%d%s\n\n",
               MGT,
               h->total,
               RESET);
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

void showResult(Game *g, Result r) {

    printf("\nROUND RESULT\n\n");

    showHand(&g->player, "Player", 0);
    showHand(&g->dealer, "Dealer", 0);

    switch (r) { 

        case PLAYER_WIN:
            printf("%sPlayer wins!%s\n", GRN, RESET);
            break;

        case DEALER_WIN:
            printf("%sDealer wins!%s\n", RED, RESET);
            break;

        case PLAYER_BUST:
            printf("%sPlayer busts!%s\n", RED, RESET);
            break;

        case DEALER_BUST:
            printf("%sDealer busts!%s\n", GRN, RESET);
            break;

        case TIE:
            printf("%sTie!%s\n", YEL, RESET);
            break;

        default:
            break;
    }

    printf("%sMoney: $%d%s\n\n", CYN, g->money, RESET);
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

void clearInputBuffer() {
    // proper input handling
    int c;
    while ((c = getchar()) != '\n' &&
           c != EOF); // clear leftover character in buffer
}

char getChoice(
    const char *prompt,
    const char *valid
) {

    char input[100];

    while (1) {

        printf("%s", prompt);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
        // fully read the input
            continue;
        }

        if (input[1] != '\n') {
        // if more than one character in input
            printf(
                "Enter only one character.\n"
            );

            continue;
        }

        char c = input[0];
        // check character validity
        for (int i = 0;
             valid[i] != '\0';
             i++) {

            if (c == valid[i])
                return c;
        }

        printf("Invalid input.\n");
    }
}
