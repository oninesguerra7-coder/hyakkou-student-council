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
