#include "card_selector.h"

namespace dream_hacking {

        CardSelector::CardSelector(PlayingCard::Suit s, bool inverse) : suit(s), inverse(inverse), suitSet(true) {
        }

        CardSelector::CardSelector(PlayingCard::Number n, bool inverse) : number(n), inverse(inverse), numberSet(true) {
        }

        CardSelector::CardSelector(PlayingCard::Number n, PlayingCard::Suit s, bool inverse) : suit(s), number(n), inverse(inverse),
                suitSet(true), numberSet(true){

        }

        CardSelector::CardSelector(const PlayingCard & c, bool inverse) : CardSelector(c.GetNumber(), c.GetSuit(), inverse) {
        }
        

        bool CardSelector::Test(const PlayingCard& c) const {
                bool equal = 
                        (!suitSet || (suit == c.GetSuit())) && 
                        (!numberSet || (number == c.GetNumber()));
                return equal != inverse;
        }

        CardSelector::~CardSelector() {
        }

        void CardSelector::GetParams(bool& inverse, const PlayingCard::Suit*& suit, const PlayingCard::Number*& number) const {
                inverse = this->inverse;
                suit = suitSet ? &(this->suit) : nullptr;
                number = numberSet ? &(this->number) : nullptr;
        }
        
bool operator == (const CardSelector& lhs, const CardSelector& rhs){
        return
                (lhs.inverse == rhs.inverse) &&
                (lhs.numberSet == rhs.numberSet) &&
                (lhs.suitSet == rhs.suitSet) &&
                (!lhs.numberSet || (lhs.number == rhs.number)) &&
                (!lhs.suitSet|| (lhs.suit == rhs.suit));
}

bool operator != (const CardSelector& lhs, const CardSelector& rhs){
        return !(lhs == rhs);
}

}// namespace dream_hacking
