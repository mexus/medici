#ifndef COMPLEX_RANGE_SELECTOR_H
#define	COMPLEX_RANGE_SELECTOR_H

#include "range_selector.h"
#include <shadow.h>

namespace dream_hacking {

        class ComplexRangeSelector {
        public:
                ComplexRangeSelector();
                virtual ~ComplexRangeSelector();
                
                void AddRangeSelectors();
                void AddRangeSelector(const RangeSelector&);
               
                template<class ...Args>
                void AddRangeSelectors(const RangeSelector& s, Args... args){
                        AddRangeSelector(s);
                        AddRangeSelectors(args...);
                }

                bool Test(const std::vector<PlayingCard> &) const;
                
                void Clear();
        protected:
                typedef std::shared_ptr<RangeSelector> PRangeSelector;
                std::vector<PRangeSelector> selectors;
                
                bool init = false;
                size_t start = 0;
        };

} // namespace dream_hacking

#endif	/* COMPLEX_RANGE_SELECTOR_H */

