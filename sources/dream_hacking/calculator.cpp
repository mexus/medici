#include "calculator.h"
#include "i-ching/i_ching.h"
#include <cstdlib>
#include <thread>

namespace dream_hacking {
        
        logxx::Log Calculator::cLog("Calculator");

        Calculator::Calculator() {

        }

        Calculator::Calculator(const ComplexRangeSelector& selector) : selector(selector) {
        }

        Calculator::~Calculator() {
        }

        ComplexRangeSelector& Calculator::AccessConditions() {
                return selector;
        }

        void Calculator::PrintDeck(const Medici& deck, std::ostream& s) {
                if (s.good()){
                        auto cardsDeck = deck.GetDeck();
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
                        const std::function<unsigned int (const Medici&)> & maximizationFunction)
        {
                S_LOG("Calculate");
                
                std::vector<std::thread> threads;
                
                using namespace std::chrono;
                auto start = steady_clock::now();
                
                variantsChecked = 0;
                interrupt = false;
                idealDeck.reset();
                
                for (size_t i = 0; i < threadsCount; ++i)
                        threads.emplace_back(&Calculator::CalculationThread, this, i, timeLimit, maximizationFunction);
                
                for (size_t i = 0; i < threadsCount; ++i){
                        std::thread& thread = threads[i];
                        thread.join();
                }
                
                auto end = steady_clock::now();
                double duration = duration_cast<milliseconds>(end - start).count() * 1E-3;
                lastPerformance = static_cast<double>(variantsChecked) / duration;
                log(logxx::notice) << "Total " << variantsChecked << " decks checked in " << duration << "s" << logxx::endl;
                
                return idealDeck;
        }

        bool Calculator::TestDeck(Medici& d, IChing& iching) const {
		//Bottleneck is selector.Test
                return selector.Test(d.GetDeck()) && d.Collapse(true) && (!iChingAnalize || IChingBalanced(d, iching)) ;
        }
        
        void Calculator::Maximization(Medici& deck, const MaximizationFunction& f) {
                S_LOG("Maximization");
                unsigned int value = f(deck);
                std::lock_guard<std::mutex> lk(mCommonVars);
                if (value > maximumValue){
                        maximumValue = value;
                        idealDeck = std::make_shared<Medici>(deck);

                        auto &s = log(logxx::notice) << "Found deck \n";
                        PrintDeck(deck, s);
                        s << "\nValue = " << value << logxx::endl;
                }
        }

        void Calculator::CalculationThread(size_t threadNumber, time_t timeLimit,
                        const MaximizationFunction & maximizationFunction)
        {
                S_LOG("CalculationThread");
                time_t start(time(nullptr));
                unsigned int seed = start - threadNumber * 10;
                IChing iching;

                Medici testingDeck;
                testingDeck.SetDeck(Deck::GenerateDeck());
                unsigned long long int localVariantsChecked(0);
                while (!interrupt && time(nullptr) < start + timeLimit){
                        ++localVariantsChecked;
                        if (TestDeck(testingDeck, iching)){
                                if (maximizationFunction){
                                        Maximization(testingDeck, maximizationFunction);
                                } else {
                                        std::lock_guard<std::mutex> lk(mCommonVars);
                                        idealDeck = std::make_shared<Medici>(testingDeck);
                                        interrupt = true;
                                        break;
                                }
                        }
                        testingDeck.Mix(Calculator::rnd, &seed);
                }
                std::lock_guard<std::mutex> lk(mCommonVars);
                variantsChecked += localVariantsChecked;
        }


        size_t Calculator::rnd(size_t i, unsigned int* seed) {
                return rand_r(seed) % i;
        }

        double Calculator::GetLastPerformance() const {
                return lastPerformance;
        }

        void Calculator::ActivateIChingAnalyze() {
                iChingAnalize = true;
        }

        bool Calculator::IChingBalanced(Medici& m, IChing &iching) {
                return iching.LoadFromDeck(m).IsBalanced();
        }

        void Calculator::Interrupt() {
                interrupt = true;
        }

} // namespace dream_hacking
