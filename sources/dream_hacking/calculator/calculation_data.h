#ifndef CALCULATION_DATA_H
#define	CALCULATION_DATA_H

#include "calculator.h"

namespace dream_hacking {

        struct CalculationData {
                time_t timeLimit;
                unsigned long long int variantsChecked;

                unsigned int maximumValue;
                MaximizationFunction maximizationFunction;
                
                CalculationResult result;
                std::mutex mCommonVars;
                std::atomic_bool interrupt;
        };

} // dream_hacking

#endif	/* CALCULATION_DATA_H */

