#ifndef CALCULATOR_H
#define	CALCULATOR_H

#include <patience/medici.h>
#include "../range_selectors/range_selector.h"
#include "../i-ching/i_ching.h"

#include "i_ching_data.h"

#include <atomic>
#include <functional>

namespace dream_hacking {
        
	typedef std::vector<std::shared_ptr<Medici> > CalculationResult;
        typedef std::function<unsigned int (const Medici&)> MaximizationFunction;
        
        class Calculator {
        public:
                Calculator();
                virtual ~Calculator();
                
                bool Calculate(time_t timeLimit = 10, size_t threads = 1, const MaximizationFunction & maximizationFunction = nullptr);
                
                void SetConditions(const ComplexRangeSelector&);
                void SetConditions();
                
                void SetIChingTestBalance(bool);
                void SetIChingSuitHex(PlayingCard::Suit, const Hexagram&);
                void SetIChingSuitHex();
                
                double GetLastPerformance() const;
                CalculationResult GetResult() const;
                
                void Interrupt();
        protected:
                static size_t rnd(size_t, unsigned int* seed);
                static logxx::Log cLog;
                
                double lastPerformance = 0.0;
                unsigned long long int variantsChecked;

                unsigned int maximumValue;
                MaximizationFunction maximizationFunction;
                
                std::atomic_bool interrupt;
                CalculationResult result;
                std::mutex mCommonVars;
                
                IChingData ichingData;
                
                static bool IChingTest(Medici&, IChingData &iChingData);
                void CalculationThread(size_t threadNumber, time_t timeLimit);
                
                bool TestDeck(Medici&, IChingData &iChingData) const;
                void Maximization(const std::shared_ptr<Medici>&);
                
        private:
                ComplexRangeSelector selector;
                
                static void PrintDeck(const Medici&, std::ostream &);

        };
} //namespace dream_hacking

#endif	/* CALCULATOR_H */
