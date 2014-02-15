#ifndef PLAYING_CARD_H
#define	PLAYING_CARD_H

#include <string>
#include <map>
#include <utility>

class PlayingCard {
public:
        enum Suit {
                Hearts,
                Diamonds,
                Clubs,
                Spades
        };
        
        enum Number {
                Six,
                Seven,
                Eight,
                Nine,
                Ten,
                Jack,
                Queen,
                King,
                Ace
        };
        
        static const std::map<Suit, std::string> suitsNames;
        static const std::map<Number, std::string> numbersNames;
        
        static const std::map<Suit, std::string> suitsNamesAbbrevations;
        static const std::map<Number, std::string> numbersNamesAbbrevations;
        
        PlayingCard();
        PlayingCard(Number, Suit);
        PlayingCard(const PlayingCard&);
        PlayingCard(const std::string&);
        virtual ~PlayingCard();
        
        Suit GetSuit() const;
        Number GetNumber() const;
        
        PlayingCard& Next(bool &reachedEnd);
        std::string Print(bool abbrevation = false) const;
        
        friend bool operator == (const PlayingCard& lhs, const PlayingCard& rhs);
        friend bool operator < (const PlayingCard& lhs, const PlayingCard& rhs);
private:
        Suit suit;
        Number number;
};

bool operator == (const PlayingCard& lhs, const PlayingCard& rhs);
bool operator < (const PlayingCard& lhs, const PlayingCard& rhs);

#endif	/* PLAYING_CARD_H */

