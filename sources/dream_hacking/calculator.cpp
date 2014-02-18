#include "calculator.h"
#include <cstdlib>

namespace dream_hacking {
        
        logxx::Log Calculator::cLog("Calculator");

        Calculator::Calculator(const ComplexRangeSelector& selector) : selector(selector) {
        }

        Calculator::~Calculator() {
        }

        ComplexRangeSelector& Calculator::AccessConditions() {
                return selector;
        }

        void Calculator::PrintDeck(const std::shared_ptr<Medici>& deck, std::ostream& s) {
                if (s.good()){
                        auto cardsDeck = deck->GetDeck();
                        for (auto it = cardsDeck.begin(); it != cardsDeck.end(); ++it){
                                const PlayingCard& card = *it;
                                s << card.Print(true);
                                if (it +1 != cardsDeck.end())
                                        s << ", ";
                        }
                }
        }

        std::shared_ptr<Medici> Calculator::Calculate(time_t timeLimit,
                        const std::function<unsigned int (const std::shared_ptr<Medici>&)> & maximizationFunction)
        {
                S_LOG("Calculate");
                time_t start(time(nullptr));
                unsigned int seed = start;
                
                unsigned int maximumValue = 0;
                auto testingDeck = std::make_shared<Medici>();
                std::shared_ptr<Medici> idealDeck;
                testingDeck->SetDeck(Deck::GenerateDeck());
                while (time(nullptr) < start + timeLimit){
                        if (selector.Test(testingDeck->GetDeck())){
                                if (testingDeck->Collapse()){
                                        if (maximizationFunction){
                                                unsigned int value = maximizationFunction(testingDeck);
                                                if (!idealDeck || value > maximumValue){
                                                        maximumValue = value;
                                                        idealDeck.reset(new Medici(* testingDeck.get()));
                                                        
                                                        auto &s = log(logxx::debug) << "Found deck \n";
                                                        PrintDeck(testingDeck, s);
                                                        s << "\nValue = " << value << logxx::endl;;
                                                }
                                        } else {
                                                idealDeck = testingDeck;
                                                break;
                                        }
                                }
                        }
                        testingDeck->Mix(Calculator::rnd, &seed);
                }
                return idealDeck;
        }

        size_t Calculator::rnd(size_t i, unsigned int* seed) {
                return rand_r(seed) % i;
        }

} // namespace dream_hacking
