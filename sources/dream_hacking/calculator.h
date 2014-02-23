#ifndef CALCULATOR_H
#define	CALCULATOR_H

#include "../patience/medici.h"
#include "range_selectors/range_selector.h"
#include "i-ching/i_ching.h"
#include <atomic>
#include <functional>

namespace dream_hacking {
        class Calculator {
        public:
                Calculator();
                Calculator(const ComplexRangeSelector& conditions);
                virtual ~Calculator();
                
                void SetThreads(size_t);
                void ActivateIChingAnalyze();
                
                std::shared_ptr<Medici> Calculate(time_t timeLimit = 10,
                        const std::function<unsigned int (const Medici&)> & maximizationFunction = nullptr);
                ComplexRangeSelector& AccessConditions();
                
                double GetLastPerformance() const;
                
                void Interrupt();
        protected:
                static size_t rnd(size_t, unsigned int* seed);
                static logxx::Log cLog;
                size_t threadsCount = 1;
                double lastPerformance = 0.0;
                bool iChingAnalize = false;
                
                static bool IChingBalanced(Medici&, IChing &);
                
                //<---Thread data:
                std::mutex mCommonVars;
                unsigned int maximumValue = 0;
                std::shared_ptr<Medici> idealDeck;
                std::atomic_bool interrupt;
                unsigned long long int variantsChecked = 0;
                //Thread data--->
                
                void CalculationThread(size_t threadNumber, time_t timeLimit,
                        const std::function<unsigned int (const Medici&)> & maximizationFunction);
                
                bool TestDeck(Medici&, IChing&) const;
                
        private:
                ComplexRangeSelector selector;
                
                static void PrintDeck(const Medici&, std::ostream &);

        };
} //namespace dream_hacking

#endif	/* CALCULATOR_H */
