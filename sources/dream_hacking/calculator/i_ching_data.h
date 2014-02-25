#ifndef CALCULATOR_I_CHING_DATA_H
#define	CALCULATOR_I_CHING_DATA_H

#include "../i-ching/i_ching.h"
#include "calculator.h"

#include <atomic>

namespace dream_hacking {

        struct IChingData {
                bool testIChingBalance;
                bool hexagramCheck;
                
                PlayingCard::Suit       hexagramSuit;
                Hexagram                hexagram;
                
                IChing iching;
                
                bool IsActive() const{
                        return testIChingBalance || hexagramCheck;
                }
        };
        
} // namespace dream_hacking
#endif	/* CALCULATOR_I_CHING_DATA_H */

