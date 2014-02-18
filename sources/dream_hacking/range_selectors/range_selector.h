#ifndef RANGE_SELECTOR_H
#define	RANGE_SELECTOR_H

#include "../../logxx/logxx.h"
#include "../card_selector.h"
#include <vector>

namespace dream_hacking {
        
        class ComplexRangeSelector;

        class RangeSelector {
        public:
                RangeSelector(size_t from, size_t to);
                virtual ~RangeSelector();
                
                bool Test(const std::vector<PlayingCard> &) const;
                void AddCard(const CardSelector&);
                void ClearCards();
                
                virtual RangeSelector* Copy() const = 0;
        protected:
                size_t from, to;
                
                std::vector<CardSelector> cardsSelector;
                
                virtual bool DefaultReturn() const = 0;
                
                enum CheckResult {
                        Continue,
                        Error,
                        Ok
                };
                
                virtual CheckResult CheckCard(const PlayingCard&) const = 0;
                
                
                bool TestCardSelectors(const PlayingCard&) const;
                
                static logxx::Log cLog;
                
                friend ComplexRangeSelector;

        };
        
} // namespace dream_hacking

#include "universal_range_selector.h"
#include "existential_range_selector.h"
#include "complex_range_selector.h"

#endif	/* RANGE_SELECTOR_H */

