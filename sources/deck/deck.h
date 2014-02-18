#ifndef DECK_H
#define	DECK_H

#include <vector>
#include <functional>

#include "playing_card.h"
#include "../logxx/logxx.h"

class Deck {
public:
        Deck();
        virtual ~Deck();
        static std::vector<PlayingCard> GenerateDeck();
        static std::vector<PlayingCard> FromString(const std::string&, const std::string& separator = std::string(" "));
        
        const std::vector<PlayingCard>& GetDeck() const;
        void SetDeck(const std::vector<PlayingCard> &deck);
        
        friend bool operator ==(const Deck& lhs, const Deck& rhs);
        friend bool operator < (const Deck& lhs, const Deck& rhs);
        
        void Mix(const std::function<size_t(size_t)> &rnd);
        static void Mix(std::vector<PlayingCard>& deck, const std::function<size_t(size_t)> &rnd);
        
protected:
        std::vector<PlayingCard> deck;
        static logxx::Log cLog;
};

bool operator ==(const Deck& lhs, const Deck& rhs);
bool operator < (const Deck& lhs, const Deck& rhs);

#endif	/* DECK_H */

