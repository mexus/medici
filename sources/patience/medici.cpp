#include "medici.h"

logxx::Log Medici::cLog(Patience::cLog, "Medici");

Medici::Medici() : Patience() {
}

Medici::~Medici() {
}

std::map<PlayingCard, unsigned int> Medici::GetCollapses() const {
        return collapses;
}

unsigned int Medici::GetCollapses(const PlayingCard& card) const {
        auto it = collapses.find(card);
        return (it == collapses.end()) ? 0 : it->second;
}

bool Medici::CollapsingCondition(const std::vector<PlayingCard> &d){
        return d.size() == 2;
}

void Medici::PartialCollapse(std::vector<PlayingCard> &d){
        S_LOG("PartialCollapse");
        //deck numeration:
        //one, two, ..., left, middle, right
        
        unsigned int collapseCnt(0);
        std::unique_ptr<PlayingCard> startCollapse;
        auto rightIt = --(d.rbegin().base());
        while (d.size() > 2){
                if (rightIt == d.begin() || rightIt == d.begin() + 1){
                        ++rightIt;
                } else {
                        auto leftIt = rightIt - 2;

                        const PlayingCard& right = *rightIt;
                        const PlayingCard& middle = *(rightIt - 1);
                        const PlayingCard& left = *leftIt;

                        if (right.GetNumber() == left.GetNumber() ||
                                right.GetSuit() == left.GetSuit()){
                                mobiles.insert(middle);
                                if (mobiles.find(left) == mobiles.end())
                                        stationars.insert(left);
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
                collapses[*(startCollapse.get())] = collapseCnt;
        }
}

bool Medici::Collapse() {
        collapses.clear();
        mobiles.clear();
        stationars.clear();
        std::vector<PlayingCard> collapsingDeck;
        for(auto it = deck.begin(); it != deck.end(); ++it){
                collapsingDeck.push_back(*it);
                if (collapsingDeck.size() > 2)
                        PartialCollapse(collapsingDeck);
        }
        
        collapsed = CollapsingCondition(collapsingDeck);
        return collapsed;
}

const std::set<PlayingCard>& Medici::GetMobiles() const {
        return mobiles;
}

const std::set<PlayingCard>& Medici::GetStationars() const {
        return stationars;
}

bool Medici::IsCollapsed() const {
        return collapsed;
}
