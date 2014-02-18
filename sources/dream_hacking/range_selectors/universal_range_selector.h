#ifndef UNIVERSAL_RANGE_SELECTOR_H
#define	UNIVERSAL_RANGE_SELECTOR_H

#include "range_selector.h"

namespace dream_hacking {

        class UniversalRangeSelector : public RangeSelector {
        public:
                UniversalRangeSelector(size_t from, size_t to);
                virtual ~UniversalRangeSelector();

                virtual RangeSelector* Copy() const;

        protected:
                virtual bool DefaultReturn() const;;
                virtual RangeSelector::CheckResult CheckCard(const PlayingCard&) const;

        };
        
} // namespace dream_hacking 

#endif	/* UNIVERSAL_RANGE_SELECTOR_H */

