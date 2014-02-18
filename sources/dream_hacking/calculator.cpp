#include "calculator.h"

namespace dream_hacking {

        Calculator::Calculator(const ComplexRangeSelector& selector, time_t timeLimit) : selector(selector), timeLimit(timeLimit) {
        }

        Calculator::~Calculator() {
        }

        ComplexRangeSelector& Calculator::AccessConditions() {
                return selector;
        }

        std::shared_ptr<Medici> Calculator::Calculate() {
                srand(time(nullptr));
                time_t start(time(nullptr));
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
                        deck->Mix(Calculator::rnd);
                }
                if (found)
                        return deck;
                else
                        return nullptr;
        }

        size_t Calculator::rnd(size_t i) {
                return std::rand() % i;
        }

} // namespace dream_hacking
