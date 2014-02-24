#include "existential_range_selector.h"

namespace dream_hacking {

        ExistentialRangeSelector::ExistentialRangeSelector(size_t from, size_t to) : RangeSelector(from, to) {
        }

        ExistentialRangeSelector::~ExistentialRangeSelector() {
        }

        RangeSelector* ExistentialRangeSelector::Copy() const {
                return new ExistentialRangeSelector(*this);
        }

//        bool ExistentialRangeSelector::Test(const std::vector<PlayingCard>& deck) const {
//                for (size_t i = from; i <= to; ++i){
//                        const PlayingCard & card = deck[i];
//                        if (TestCardSelectors(card))
//                                return true;
//                }
//                return false;
//        }

        RangeSelector::CheckResult ExistentialRangeSelector::CheckCard(const PlayingCard& card) const {
                if (TestCardSelectors(card))
                        return RangeSelector::Ok;
                else
                        return RangeSelector::Continue;
        }

        bool ExistentialRangeSelector::DefaultReturn() const {
                return false;
        }

} //  namespace dream_hacking
