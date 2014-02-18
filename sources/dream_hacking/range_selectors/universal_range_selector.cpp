#include "universal_range_selector.h"

namespace dream_hacking {

        UniversalRangeSelector::UniversalRangeSelector(size_t from, size_t to) : RangeSelector(from, to) {
        }

        UniversalRangeSelector::~UniversalRangeSelector() {
        }

        RangeSelector* UniversalRangeSelector::Copy() const {
                return new UniversalRangeSelector(*this);
        }

//        bool UniversalRangeSelector::Test(const std::vector<PlayingCard>& deck) const {
//                for (size_t i = from; i <= to; ++i){
//                        const PlayingCard & card = deck[i];
//                        if (!TestCardSelectors(card))
//                                return false;
//                }
//                return true;
//        }

        RangeSelector::CheckResult UniversalRangeSelector::CheckCard(const PlayingCard& card) const {
                if (!TestCardSelectors(card))
                        return RangeSelector::Error;
                else
                        return RangeSelector::Continue;
        }
        
        bool UniversalRangeSelector::DefaultReturn() const {
                return true;
        }

} // namespace dream_hacking 
