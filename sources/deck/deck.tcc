#ifndef DECK_TCC
#define	DECK_TCC

#include "deck.h"

template<class RandomFunction, class ...RFArguments>
void Deck::Mix(RandomFunction rnd, RFArguments... rndArgs){
        Mix(deck, rnd, rndArgs...);
}

template<class RandomFunction, class ...RFArguments>
void Deck::Mix(std::vector<PlayingCard>& deck, RandomFunction rnd, RFArguments... rndArgs){
        size_t first, second;
        do {
                first = rnd(deck.size(), rndArgs...);
                second = rnd(deck.size(), rndArgs...);
        } while (second == first);
        std::swap(deck[first], deck[second]);
}

#endif	/* DECK_TCC */

