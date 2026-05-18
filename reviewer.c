#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
    ANSI color codes for colored terminal output
*/
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define CYN   "\x1B[36m"
#define MGT   "\x1B[35m"
#define RESET "\x1B[0m"

/*
    Game constants
*/
#define MAX_CARDS      15
#define DECK_SIZE      52
#define BLACKJACK      21
#define DEALER_STAND   17
#define STARTING_MONEY 1000


/*
    Enum for possible round outcomes

    Enums give names to integer values.
    Example:
        PLAYER_WIN might internally be 1
*/
typedef enum {

    NO_RESULT,
    PLAYER_WIN,
    DEALER_WIN,
    PLAYER_BUST,
    DEALER_BUST,
    TIE

} Result;


/*
    Structure for a single playing card

    Example card:
        rank = 'A'
        suit = 'h'
    means Ace of Hearts
*/
typedef struct {

    char rank;
    char suit;

} Card;


/*
    Structure for a hand

    cards[] -> stores all cards in the hand
    count   -> number of cards currently held
    total   -> current blackjack value
    aces    -> number of aces currently valued as 11
*/
typedef struct {

    Card cards[MAX_CARDS];

    int count;
    int total;
    int aces;

} Hand;


/*
    Main game structure

    player    -> player's hand
    dealer    -> dealer's hand

    money     -> player's current money
    bet       -> current round bet

    deck[]    -> the entire deck of cards
    deckIndex -> current position in deck
*/
typedef struct {

    Hand player;
    Hand dealer;

    int money;
    int bet;

    Card deck[DECK_SIZE];
    int deckIndex;

} Game;


/* Function prototypes */
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


/*
    Main function
*/
int main() {

    /*
        Create a Game variable named game
    */
    Game game;

    /*
        &game means:
        "address of game"

        We pass the memory address so the function
        can directly modify the original variable.
    */
    initializeGame(&game);

    /*
        Keep playing while the player still has money
    */
    while (game.money > 0) {

        playRound(&game);
    }

    printf("\n%sYou ran out of money! Game over.%s\n",
           RED, RESET);

    return 0;
}


/*
    Initializes the game
*/
void initializeGame(Game *g) {

    /*
        g is a POINTER to a Game structure.

        The arrow operator ->
        accesses members THROUGH a pointer.

        Example:
            g->money

        means:
            "access money inside the Game
             structure pointed to by g"

        Equivalent to:
            (*g).money
    */

    g->money = STARTING_MONEY;

    /*
        Arrays for suits and ranks
    */
    char suits[] = {'h', 'd', 'c', 's'};

    char ranks[] = {
        '2', '3', '4', '5',
        '6', '7', '8', '9',
        'T', 'J', 'Q', 'K', 'A'
    };

    int index = 0;

    /*
        Create all 52 cards
    */
    for (int s = 0; s < 4; s++) {

        for (int r = 0; r < 13; r++) {

            /*
                Access current card in deck
                and assign its rank/suit
            */
            g->deck[index].rank = ranks[r];
            g->deck[index].suit = suits[s];

            index++;
        }
    }

    /*
        Seed random generator using current time
    */
    srand(time(NULL));

    shuffleDeck(g);
}


/*
    Fisher-Yates shuffle algorithm
*/
void shuffleDeck(Game *g) {

    /*
        Start from last card
        and swap backwards
    */
    for (int i = DECK_SIZE - 1; i > 0; i--) {

        /*
            Random index from 0 to i
        */
        int j = rand() % (i + 1);

        /*
            Swap cards
        */
        Card temp = g->deck[i];
        g->deck[i] = g->deck[j];
        g->deck[j] = temp;
    }

    /*
        Reset deck position
    */
    g->deckIndex = 0;
}


/*
    Draws one card from deck
*/
Card drawCard(Game *g) {

    /*
        If deck is empty,
        reshuffle automatically
    */
    if (g->deckIndex >= DECK_SIZE)
        shuffleDeck(g);

    /*
        Return current card,
        then move deckIndex forward

        deckIndex++
        uses current value first,
        THEN increments by 1
    */
    return g->deck[g->deckIndex++];
}


/*
    Clears hand data
*/
void resetHand(Hand *h) {

    h->count = 0;
    h->total = 0;
    h->aces = 0;
}


/*
    Adds card to hand
*/
void addCard(Hand *h, Card c) {

    /*
        Store card in next available slot
    */
    h->cards[h->count++] = c;

    /*
        count++:
        use current count first,
        then increment afterwards
    */

    /*
        Number cards 2-9
    */
    if (c.rank >= '2' && c.rank <= '9') {

        /*
            Convert char digit to integer

            Example:
                '7' - '0' = 7
        */
        h->total += c.rank - '0';
    }

    /*
        Face cards worth 10
    */
    else if (c.rank == 'T' ||
             c.rank == 'J' ||
             c.rank == 'Q' ||
             c.rank == 'K') {

        h->total += 10;
    }

    /*
        Ace initially worth 11
    */
    else if (c.rank == 'A') {

        h->total += 11;
        h->aces++;
    }

    /*
        If total exceeds 21,
        convert Ace from 11 to 1

        subtract 10 because:
            11 -> 1 difference is 10
    */
    while (h->total > BLACKJACK && h->aces > 0) {

        h->total -= 10;
        h->aces--;
    }
}


/*
    Prints one card
*/
void printCard(Card c) {

    printf("%s%c%c%s",
           YEL,
           c.rank,
           c.suit,
           RESET);
}


/*
    Displays a hand
*/
void showHand(Hand *h, const char *name, int hidden) {

    printf("%s hand: ", name);

    /*
        Print every card
    */
    for (int i = 0; i < h->count; i++) {

        /*
            Hide first dealer card
        */
        if (i == 0 && hidden) {

            printf("%s??%s ", YEL, RESET);
        }

        else {

            printCard(h->cards[i]);
            printf(" ");
        }
    }

    printf("\n");

    /*
        Hide dealer value if needed
    */
    if (hidden)
        printf("Value: %s?%s\n\n", MGT, RESET);

    else
        printf("Value: %s%d%s\n\n",
               MGT,
               h->total,
               RESET);
}


/*
    Deals initial 2 cards each
*/
void initialDeal(Game *g) {

    for (int i = 0; i < 2; i++) {

        addCard(&g->player, drawCard(g));
        addCard(&g->dealer, drawCard(g));
    }
}


/*
    Checks for immediate outcomes
*/
Result checkImmediate(Game *g) {

    int p = g->player.total;
    int d = g->dealer.total;

    if (p > BLACKJACK)
        return PLAYER_BUST;

    if (d > BLACKJACK)
        return DEALER_BUST;

    if (p == BLACKJACK && d == BLACKJACK)
        return TIE;

    if (p == BLACKJACK)
        return PLAYER_WIN;

    if (d == BLACKJACK)
        return DEALER_WIN;

    return NO_RESULT;
}


/*
    Compare totals after turns end
*/
Result determineWinner(Game *g) {

    int p = g->player.total;
    int d = g->dealer.total;

    if (p > d)
        return PLAYER_WIN;

    if (d > p)
        return DEALER_WIN;

    return TIE;
}


/*
    Dealer draws until reaching 17
*/
void dealerTurn(Game *g) {

    printf("\nDealer turn...\n\n");

    while (g->dealer.total < DEALER_STAND) {

        addCard(&g->dealer, drawCard(g));

        printf("Dealer drew: ");

        /*
            dealer.count - 1
            gets last card added
        */
        printCard(g->dealer.cards[g->dealer.count - 1]);

        printf("\n");
    }

    printf("\n");
}


/*
    Handles player action

    Returns:
        1 if player stands
        0 otherwise
*/
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

        return 0;
    }

    printf("\nPlayer stands.\n\n");

    return 1;
}


/*
    Updates player's money
*/
void updateMoney(Game *g, Result r) {

    /*
        Player gains money
    */
    if (r == PLAYER_WIN ||
        r == DEALER_BUST) {

        g->money += g->bet;
    }

    /*
        Player loses money
    */
    else if (r == DEALER_WIN ||
             r == PLAYER_BUST) {

        g->money -= g->bet;
    }

    /*
        Tie changes nothing
    */
}


/*
    Displays round result
*/
void showResult(Game *g, Result r) {

    printf("\nROUND RESULT\n\n");

    showHand(&g->player, "Player", 0);
    showHand(&g->dealer, "Dealer", 0);

    switch (r) {

        case PLAYER_WIN:
            printf("%sPlayer wins!%s\n",
                   GRN, RESET);
            break;

        case DEALER_WIN:
            printf("%sDealer wins!%s\n",
                   RED, RESET);
            break;

        case PLAYER_BUST:
            printf("%sPlayer busts!%s\n",
                   RED, RESET);
            break;

        case DEALER_BUST:
            printf("%sDealer busts!%s\n",
                   GRN, RESET);
            break;

        case TIE:
            printf("%sTie!%s\n",
                   YEL, RESET);
            break;

        default:
            break;
    }

    printf("%sMoney: $%d%s\n\n",
           CYN,
           g->money,
           RESET);
}


/*
    Controls one full round
*/
void playRound(Game *g) {

    /*
        static keeps value between function calls

        round is initialized ONLY once
    */
    static int round = 1;

    printf("\n%s| ROUND %d |%s\n\n",
           CYN,
           round++,
           RESET);

    printf("%sMoney: $%d%s\n\n",
           CYN,
           g->money,
           RESET);

    /*
        Input validation loop
    */
    while (1) {

        printf("Enter bet amount: ");

        /*
            scanf returns number of successful inputs

            != 1 means invalid integer input
        */
        if (scanf("%d", &g->bet) != 1) {

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

    /*
        Prepare new round
    */
    resetHand(&g->player);
    resetHand(&g->dealer);

    initialDeal(g);

    showHand(&g->player, "Player", 0);
    showHand(&g->dealer, "Dealer", 1);

    Result result = checkImmediate(g);

    /*
        Handle blackjack/bust immediately
    */
    if (result != NO_RESULT) {

        updateMoney(g, result);

        showResult(g, result);

        return;
    }

    /*
        Ask for double down
    */
    char dd = getChoice(
        "Double down? (y/n): ",
        "yn"
    );

    if (dd == 'y') {

        /*
            Double bet check
        */
        if (g->bet * 2 > g->money) {

            printf("Not enough money to double down.\n");
        }

        else {

            /*
                Double the bet
            */
            g->bet *= 2;

            /*
                Draw exactly one card
            */
            addCard(&g->player, drawCard(g));

            printf("\nYou drew: ");

            printCard(
                g->player.cards[g->player.count - 1]
            );

            printf("\n\n");

            showHand(&g->player, "Player", 0);

            result = checkImmediate(g);

            /*
                If game still active,
                dealer takes turn
            */
            if (result == NO_RESULT) {

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

    /*
        Main player turn loop
    */
    int stand = 0;

    while (!stand) {

        /*
            !stand means:
            "while stand is false"
        */
        stand = playerTurn(g);

        result = checkImmediate(g);

        if (result != NO_RESULT) {

            updateMoney(g, result);

            showResult(g, result);

            return;
        }
    }

    /*
        Dealer turn after player stands
    */
    dealerTurn(g);

    result = checkImmediate(g);

    if (result == NO_RESULT)
        result = determineWinner(g);

    updateMoney(g, result);

    showResult(g, result);
}


/*
    Clears leftover input characters
*/
void clearInputBuffer() {

    int c;

    /*
        Keep reading until newline or EOF
    */
    while ((c = getchar()) != '\n' &&
           c != EOF);
}


/*
    Gets validated single-character input
*/
char getChoice(
    const char *prompt,
    const char *valid
) {

    char input[100];

    while (1) {

        printf("%s", prompt);

        /*
            fgets safely reads entire line
        */
        if (fgets(input,
                  sizeof(input),
                  stdin) == NULL) {

            continue;
        }

        /*
            Prevent multi-character input

            Example:
                "hss"

            input[1] would not be '\n'
        */
        if (input[1] != '\n') {

            printf(
                "Enter only one character.\n"
            );

            continue;
        }

        char c = input[0];

        /*
            Check if character is valid
        */
        for (int i = 0;
             valid[i] != '\0';
             i++) {

            if (c == valid[i])
                return c;
        }

        printf("Invalid input.\n");
    }
}
