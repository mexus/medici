#include "calculator.h"
#include "i-ching/i_ching.h"
#include <cstdlib>
#include <atomic>
#include <thread>

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

        void Calculator::SetThreads(size_t n) {
                threadsCount = n;
        }

        std::shared_ptr<Medici> Calculator::Calculate(time_t timeLimit,
                        const std::function<unsigned int (const std::shared_ptr<Medici>&)> & maximizationFunction)
        {
                S_LOG("Calculate");
                
                std::mutex mCommonVars;
                unsigned int maximumValue = 0;
                unsigned long long int variantsChecked(0);
                std::shared_ptr<Medici> idealDeck;
                std::atomic_bool interrupt(false);
                
                std::vector<std::thread> threads;
                
                using namespace std::chrono;
                steady_clock::time_point start = steady_clock::now();
                
                for (size_t i = 0; i < threadsCount; ++i){
                        threads.emplace_back([this, &maximizationFunction, &mCommonVars, &maximumValue, &idealDeck, &interrupt,
                                &variantsChecked, i, timeLimit]()
                        {
                                time_t start(time(nullptr));
                                unsigned int seed = start - i * 10;
                                
                                auto testingDeck = std::make_shared<Medici>();
                                testingDeck->SetDeck(Deck::GenerateDeck());
                                unsigned long long int localVariantsChecked(0);
                                while (!interrupt && time(nullptr) < start + timeLimit){
                                        ++localVariantsChecked;
                                        if (selector.Test(testingDeck->GetDeck())){
                                                if (testingDeck->Collapse()){
                                                        if (IChingBalanced(testingDeck)){
                                                                if (maximizationFunction){
                                                                        unsigned int value = maximizationFunction(testingDeck);
                                                                        std::lock_guard<std::mutex> lk(mCommonVars);
                                                                        if (value > maximumValue){
                                                                                maximumValue = value;
                                                                                idealDeck.reset(new Medici(* testingDeck.get()));

                                                                                auto &s = log(logxx::debug) << "Found deck \n";
                                                                                PrintDeck(testingDeck, s);
                                                                                s << "\nValue = " << value << logxx::endl;
                                                                        }
                                                                } else {
                                                                        idealDeck = testingDeck;
                                                                        interrupt = true;
                                                                        break;
                                                                }
                                                        }
                                                }
                                        }
                                        testingDeck->Mix(Calculator::rnd, &seed);
                                }
                                std::lock_guard<std::mutex> lk(mCommonVars);
                                variantsChecked += localVariantsChecked;
                        });
                }
                
                for (size_t i = 0; i < threadsCount; ++i){
                        std::thread& thread = threads[i];
                        thread.join();
                }
                
                steady_clock::time_point end = steady_clock::now();
                double duration = duration_cast<milliseconds>(end - start).count() * 1E-3;
                lastPerformance = static_cast<double>(variantsChecked) / duration;
                log(logxx::notice) << "Total " << variantsChecked << " decks checked in " << duration << "s" << logxx::endl;
                
                return idealDeck;
        }

        size_t Calculator::rnd(size_t i, unsigned int* seed) {
                return rand_r(seed) % i;
        }

        double Calculator::GetLastPerformance() const {
                return lastPerformance;
        }

        void Calculator::SetIChingBalanced(bool v) {
                onlyIChingBalanced = v;
        }

        bool Calculator::IChingBalanced(const std::shared_ptr<Medici>& m) const {
                if (onlyIChingBalanced){
                        IChing iching;
                        iching.LoadFromDeck(*m.get());
                        return iching.IsBalanced();
                } else
                        return true;
        }

} // namespace dream_hacking
