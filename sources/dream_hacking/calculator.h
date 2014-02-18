#ifndef CALCULATOR_H
#define	CALCULATOR_H

#include "../patience/medici.h"
#include "range_selectors/range_selector.h"
#include <functional>

namespace dream_hacking {
        class Calculator {
        public:
                Calculator(const ComplexRangeSelector& conditions);
                virtual ~Calculator();
                
                std::shared_ptr<Medici> Calculate(time_t timeLimit = 10, const std::function<unsigned int (const std::shared_ptr<Medici>&)> & maximizationFunction = nullptr);
                ComplexRangeSelector& AccessConditions();
        protected:
                static size_t rnd(size_t, unsigned int* seed);
                static logxx::Log cLog;
                
        private:
                ComplexRangeSelector selector;
                
                static void PrintDeck(const std::shared_ptr<Medici>&, std::ostream &);

        };
} //namespace dream_hacking

#endif	/* CALCULATOR_H */
