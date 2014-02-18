#ifndef CALCULATOR_H
#define	CALCULATOR_H

#include "../patience/medici.h"
#include "range_selectors/range_selector.h"

namespace dream_hacking {
        class Calculator {
        public:
                Calculator(const ComplexRangeSelector& conditions, time_t timeLimit = 60);
                virtual ~Calculator();
                
                std::shared_ptr<Medici> Calculate();
                
                ComplexRangeSelector& AccessConditions();
                
        protected:
                static size_t rnd(size_t);
                
        private:
                ComplexRangeSelector selector;
                const time_t timeLimit;

        };
} //namespace dream_hacking

#endif	/* CALCULATOR_H */
