#include "calculator.h"
#include <cstdlib>

namespace dream_hacking {

        Calculator::Calculator(const ComplexRangeSelector& selector, time_t timeLimit) : selector(selector), timeLimit(timeLimit) {
        }

        Calculator::~Calculator() {
        }

        ComplexRangeSelector& Calculator::AccessConditions() {
                return selector;
        }

        std::shared_ptr<Medici> Calculator::Calculate() {
                time_t start(time(nullptr));
                unsigned int seed = start;
                
                auto deck = std::make_shared<Medici>();
                deck->SetDeck(Deck::GenerateDeck());
                bool found(false);
                while (time(nullptr) < start + timeLimit){
                        if (selector.Test(deck->GetDeck())){
                                if (deck->Collapse()){
                                        found = true;
                                        break;
                                }
                        }
                        deck->Mix(Calculator::rnd, &seed);
                }
                if (found)
                        return deck;
                else
                        return nullptr;
        }

        size_t Calculator::rnd(size_t i, unsigned int* seed) {
                return rand_r(seed) % i;
        }

} // namespace dream_hacking
