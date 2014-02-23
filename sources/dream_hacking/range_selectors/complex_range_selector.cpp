#include "complex_range_selector.h"

namespace dream_hacking {

        ComplexRangeSelector::ComplexRangeSelector() {
        }

        ComplexRangeSelector::~ComplexRangeSelector() {
        }

        bool ComplexRangeSelector::Test(const std::vector<PlayingCard>& deck) const{
                if (selectors.empty()){
                        return true;
                } else {
                        size_t len = deck.size();
                        size_t selectorsCnt = selectors.size();
                        for (size_t i = 0; i < selectorsCnt; ++i){
                                const PRangeSelector & selector = selectors[i];
                                bool satisfied = false;
                                for (size_t j = selector->from; j <= std::min(selector->to, len - 1); ++j){
                                        const PlayingCard& card = deck[j];
                                        auto res = selector->CheckCard(card);
                                        if (res == RangeSelector::Error)
                                                return false;
                                        else if (res == RangeSelector::Ok){
                                                satisfied = true;
                                                break;
                                        }
                                }
                                if (!satisfied && !selector->DefaultReturn())
                                        return false;
                        }
                        
                        return true;
                }
        }
        
        void ComplexRangeSelector::AddRangeSelectors() {
        }


        void ComplexRangeSelector::AddRangeSelector(const RangeSelector& s) {
                selectors.emplace_back(s.Copy());
                if (selectors.empty() || s.from < start)
                        start = s.from;
        }

        void ComplexRangeSelector::Clear() {
                selectors.clear();
        }


} // namespace dream_hacking

