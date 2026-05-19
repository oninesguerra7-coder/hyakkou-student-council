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
