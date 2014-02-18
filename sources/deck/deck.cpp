#include "deck.h"
#include <algorithm>

logxx::Log Deck::cLog("Deck");

Deck::Deck() {
}

Deck::~Deck() {
}

std::vector<PlayingCard> Deck::GenerateDeck() {
        std::vector<PlayingCard> deck;
        deck.emplace_back();
        bool reachedEnd(false);
        int i(0);
        while(!reachedEnd){
                PlayingCard newCard(deck[i++]);
                newCard.Next(reachedEnd);
                if (!reachedEnd)
                        deck.push_back(newCard);
        }
        return deck;
}

std::vector<PlayingCard> Deck::FromString(const std::string& s, const std::string& separator) {
        S_LOG("FromString");
        std::vector<PlayingCard> deck;
        size_t startPos (0);
        try {
                while (true){
                        size_t pos = s.find(separator, startPos);
                        if (pos == std::string::npos){
                                deck.emplace_back(s.substr(startPos));
                                break;
                        } else {
                                size_t length = pos - startPos;
                                deck.emplace_back(s.substr(startPos, length));
                                startPos = pos + 1;
                        }
                }
        } catch (const std::exception& e){
                log(logxx::error) << "Can't make a desk from string {" << s << "}: \n" << e.what() << logxx::endl;
                deck.clear();
        }
        return deck;
}


bool operator ==(const Deck& lhs, const Deck& rhs){
        return lhs.deck == rhs.deck;
}

bool operator < (const Deck& lhs, const Deck& rhs){
        return lhs.deck < rhs.deck;
}

void Deck::SetDeck(const std::vector<PlayingCard>& deck) {
        this->deck = deck;
}

std::vector<PlayingCard> Deck::GetDeck() const {
        return deck;
}

void Deck::Mix(std::vector<PlayingCard>& deck, const std::function<size_t(size_t)> &rnd) {
        size_t second;
        do {
                second = rnd(deck.size());
        } while (second == 0);
        std::swap(deck[0], deck[second]);
}