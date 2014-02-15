#ifndef DECK_H
#define	DECK_H

#include "playing_card.h"
#include <vector>
#include "../sources/logxx/logxx.h"

class Deck {
public:
        Deck();
        Deck(const std::vector<PlayingCard>&);
        Deck(const Deck&);
        Deck(const std::string&, char separator = ' ');
        virtual ~Deck();
        
        std::vector<PlayingCard> GetDeck() const;
        void SetDeck(const std::vector<PlayingCard> &deck);
//        void AddDeck(const std::vector<PlayingCard> &deck);
        
        void Print(bool abbrevation = false) const;
        bool Next();
        
        bool Collapse();
        
        friend bool operator ==(const Deck& lhs, const Deck& rhs);
        friend bool operator < (const Deck& lhs, const Deck& rhs);
        
        std::map<PlayingCard, unsigned int> collapses;
private:
        std::vector<PlayingCard> deck;
        static logxx::Log cLog;
        void PartialCollapse(std::vector<PlayingCard> &);
};

bool operator ==(const Deck& lhs, const Deck& rhs);
bool operator < (const Deck& lhs, const Deck& rhs);

#endif	/* DECK_H */

