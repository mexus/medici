#include "deck.h"
#include <algorithm>

logxx::Log Deck::cLog("Deck");

Deck::Deck() {
        S_LOG("Deck");
        deck.emplace_back();
        bool reachedEnd(false);
        int i(0);
        while(!reachedEnd){
                PlayingCard newCard(deck[i++]);
                newCard.Next(reachedEnd);
                if (!reachedEnd)
                        deck.push_back(newCard);
        }
//        log(logxx::debug) << i << " card in a deck" << logxx::endl;
}

Deck::Deck(const Deck& old) :  collapses(old.collapses), deck(old.deck) {
}

Deck::Deck(const std::vector<PlayingCard>& deck) : deck(deck){
}

Deck::Deck(const std::string& s, char separator){
        std::string tmp;
        for (auto it = s.begin(); it != s.end(); ++it){
                char c = *it;
                if (c == separator || it + 1 == s.end()){
                        if (it + 1 == s.end())
                                tmp += c;
                        deck.emplace_back(tmp);
                        tmp.clear();
                } else
                        tmp += c;
        }
}

Deck::~Deck() {
}

void Deck::Print(bool abbrevation) const {
        S_LOG("Print");
        auto &s = log(logxx::info);
        for (auto it = deck.begin(); it != deck.end(); ++it){
                const PlayingCard &card = *it;
                s << card.Print(abbrevation);
                if (it + 1 != deck.end())
                        s << ", ";
        }
        s << logxx::endl;
}

bool operator ==(const Deck& lhs, const Deck& rhs){
        return lhs.deck == rhs.deck;
}

bool operator < (const Deck& lhs, const Deck& rhs){
        return lhs.deck < rhs.deck;
}

bool Deck::Next() {
        return std::next_permutation(deck.begin(), deck.end());
}

bool CollapsingCondition(const std::vector<PlayingCard> &d){
        return d.size() == 2;
}

void Deck::PartialCollapse(std::vector<PlayingCard> &d){
        S_LOG("PartialCollapse");
        // one, two, ..., left, middle, right
        
        unsigned int collapseCnt(0);
        //auto currentIt = --(d.rbegin().base());
        //auto rightIt = currentIt;
        std::unique_ptr<PlayingCard> startCollapse;
        auto rightIt = --(d.rbegin().base());
        while (d.size() > 2){
                if (rightIt == d.begin() || rightIt == d.begin() + 1){
                        ++rightIt;
                } else {
//                        auto middleIt = rightIt - 1;
                        auto leftIt = rightIt - 2;

                        const PlayingCard& right = *rightIt;
//                        const PlayingCard& middle = *middleIt;
                        const PlayingCard& left = *leftIt;

//                        log(logxx::debug) << left.Print(true) << " " << middle.Print(true) << " " << right.Print(true) << logxx::endl;
                        if (right.GetNumber() == left.GetNumber() ||
                                right.GetSuit() == left.GetSuit()){
                                rightIt = d.erase(leftIt);
                                ++collapseCnt;
                                if (!startCollapse){
                                        startCollapse.reset(new PlayingCard(right));
                                }
                        } else if (rightIt + 1 == d.end()){
                                break;
                        } else {
                                ++rightIt;
                        }
                }
        }
        if (collapseCnt != 0){
                collapses[*(startCollapse.release())] = collapseCnt;
        }
}

bool Deck::Collapse() {
        S_LOG("Collapse");
        collapses.clear();
        std::vector<PlayingCard> collapsingDeck;
//        Print(true);
        for(auto it = deck.begin(); it != deck.end(); ++it){
                collapsingDeck.push_back(*it);
                if (collapsingDeck.size() > 2)
                        PartialCollapse(collapsingDeck);
        }
        
        if (CollapsingCondition(collapsingDeck)){
//                log(logxx::info) << "Collapses: " << logxx::endl;
//                for (auto &pair : collapses){
//                        log(logxx::info) << pair.first.Print() << ": " << pair.second << logxx::endl;
//                }
                return true;
        } else
                return false;
}

void Deck::SetDeck(const std::vector<PlayingCard>& deck) {
        this->deck = deck;
}

std::vector<PlayingCard> Deck::GetDeck() const {
        return deck;
}


//void Deck::AddDeck(const std::vector<PlayingCard>& deck) {
//        for (auto &card : deck){
//                
//        }
//}
