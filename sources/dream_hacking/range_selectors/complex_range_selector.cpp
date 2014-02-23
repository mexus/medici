#include "complex_range_selector.h"

namespace dream_hacking {

        ComplexRangeSelector::ComplexRangeSelector() {
        }

        ComplexRangeSelector::~ComplexRangeSelector() {
        }

        bool ComplexRangeSelector::Test(const std::vector<PlayingCard>& deck) const{
                auto localSelectors = selectors;
                
                size_t len = deck.size();
                for (size_t i = start; i < len; ++i){
                        const PlayingCard& card = deck[i];
                        auto it = localSelectors.begin();
                        while (it != localSelectors.end()){
                                const PRangeSelector& selector = *it;
                                if (i > selector->to){
                                        if (!selector->DefaultReturn())
                                                return false;
                                        it = localSelectors.erase(it);
                                } else if (i >= selector->from){
                                        auto res = selector->CheckCard(card);
                                        if (res == RangeSelector::Error)
                                                return false;
                                        else if (res == RangeSelector::Ok)
                                                it = localSelectors.erase(it);
                                        else
                                                ++it;
                                } else
                                        ++it;
                        }
                }
                
                for (auto it = localSelectors.begin(); it != localSelectors.end(); ++it){
                        PRangeSelector& selector = *it;
                        if (!selector->DefaultReturn())
                                return false;
                }
                return true;
        }
        
        void ComplexRangeSelector::AddRangeSelectors() {
        }


        void ComplexRangeSelector::AddRangeSelector(const RangeSelector& s) {
                selectors.emplace_back(s.Copy());
                if (!init || s.from < start){
                        start = s.from;
                        init = true;
                }
        }

        void ComplexRangeSelector::Clear() {
                selectors.clear();
        }


} // namespace dream_hacking

