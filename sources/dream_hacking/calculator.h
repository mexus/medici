#ifndef CALCULATOR_H
#define	CALCULATOR_H

#include "../patience/medici.h"
#include "range_selectors/range_selector.h"
#include "i-ching/i_ching.h"
#include <functional>

namespace dream_hacking {
        class Calculator {
        public:
                Calculator(const ComplexRangeSelector& conditions);
                virtual ~Calculator();
                
                void SetThreads(size_t);
                void SetIChingBalanced(bool);
                
                std::shared_ptr<Medici> Calculate(time_t timeLimit = 10,
                        const std::function<unsigned int (const std::shared_ptr<Medici>&)> & maximizationFunction = nullptr);
                ComplexRangeSelector& AccessConditions();
                
                double GetLastPerformance() const;
        protected:
                static size_t rnd(size_t, unsigned int* seed);
                static logxx::Log cLog;
                size_t threadsCount = 1;
                double lastPerformance = 0.0;
                bool onlyIChingBalanced = false;
                
                IChing iching;
                bool IChingBalanced(const std::shared_ptr<Medici>&) const;
                
        private:
                ComplexRangeSelector selector;
                
                static void PrintDeck(const std::shared_ptr<Medici>&, std::ostream &);

        };
} //namespace dream_hacking

#endif	/* CALCULATOR_H */
