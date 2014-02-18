#ifndef CARD_SELECTOR_H
#define	CARD_SELECTOR_H

#include "../deck/playing_card.h"

namespace dream_hacking {

class CardSelector {
public:
        CardSelector(PlayingCard::Suit, bool inverse = false);
        CardSelector(PlayingCard::Number, bool inverse = false);
        CardSelector(PlayingCard::Number, PlayingCard::Suit, bool inverse = false);
        CardSelector(const PlayingCard&, bool inverse = false);
        virtual ~CardSelector();
        
        bool Test(const PlayingCard&) const;
        
        void GetParams(bool &inverse, const PlayingCard::Suit* &suit, const PlayingCard::Number* &number) const;
        
        friend bool operator == (const CardSelector& lhs, const CardSelector& rhs);
        friend bool operator != (const CardSelector& lhs, const CardSelector& rhs);
        
protected:
        
        PlayingCard::Suit suit;
        PlayingCard::Number number;
        bool inverse = false;
        
        bool suitSet = false;
        bool numberSet = false;
};

bool operator == (const CardSelector& lhs, const CardSelector& rhs);
bool operator != (const CardSelector& lhs, const CardSelector& rhs);

} // namespace dream_hacking

#endif	/* CARD_SELECTOR_H */

