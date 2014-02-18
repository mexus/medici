#ifndef EXISTENTIAL_RANGE_SELECTOR_H
#define	EXISTENTIAL_RANGE_SELECTOR_H

#include "range_selector.h"

namespace dream_hacking {

        class ExistentialRangeSelector : public RangeSelector {
        public:
                ExistentialRangeSelector(size_t from, size_t to);
                virtual ~ExistentialRangeSelector();

                virtual RangeSelector* Copy() const;

        protected:
                virtual bool DefaultReturn() const;
                virtual RangeSelector::CheckResult CheckCard(const PlayingCard&) const;
                
        };
        
} //  namespace dream_hacking
        
#endif	/* EXISTENTIAL_RANGE_SELECTOR_H */

