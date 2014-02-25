#ifndef CALCULATOR_THREAD_DATA_H
#define	CALCULATOR_THREAD_DATA_H

#include <mutex>
#include <patience/medici.h>
#include <atomic>
#include "calculator.h"

namespace calculator_helpers {

        struct ThreadData {
                std::mutex mCommonVars;
                unsigned int maximumValue;
                std::atomic_bool interrupt;
                unsigned long long int variantsChecked;
                
                size_t threadNumber;
        };
        
} // namespace calculator_helpers
#endif	/* CALCULATOR_THREAD_DATA_H */

