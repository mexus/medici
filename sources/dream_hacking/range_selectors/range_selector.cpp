#include <vector>

#include "range_selector.h"

namespace dream_hacking {
        
        logxx::Log RangeSelector::cLog("RangeSelector");

        RangeSelector::RangeSelector(size_t from, size_t to) :
        from(from), to(to)
        {
        }

        RangeSelector::~RangeSelector() {
        }

        void RangeSelector::AddCard(const CardSelector& newCard) {
                S_LOG("AddCard");
                for(auto & card : cardsSelector){
                        if (card == newCard){
                                auto &s = log(logxx::warning) << "Card already in a list: ";
                                bool inverse;
                                const PlayingCard::Suit *suit;
                                const PlayingCard::Number *number;
                                newCard.GetParams(inverse, suit, number);
                                if (inverse)
                                        s << "not ";
                                if (number)
                                        s << PlayingCard::PrintNumber(*number, false) << " ";
                                if (suit)
                                        s << PlayingCard::PrintSuit(*suit, false) << " ";
                                s << logxx::endl;
                                return ;
                        }
                }
                cardsSelector.push_back(newCard);
        }

        void RangeSelector::ClearCards() {
                cardsSelector.clear();
        }

        bool RangeSelector::TestCardSelectors(const PlayingCard& testCard) const {
                for (auto & cardSelector : cardsSelector){
                        if (cardSelector.Test(testCard))
                                return true;
                }
                return false;
        }

        bool RangeSelector::Test(const std::vector<PlayingCard>& deck) const {
                for (size_t i = from; i <= to; ++i){
                        const PlayingCard& card = deck[i];
                        auto res = CheckCard(card);
                        if (res == RangeSelector::Error)
                                return false;
                        else if (res == RangeSelector::Ok)
                                return true;
                }
                return DefaultReturn();
        }


} // namespace dream_hacking
