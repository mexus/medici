#include "calculator.h"

#include <cstdlib>
#include <thread>

namespace dream_hacking {
        
        logxx::Log Calculator::cLog("Calculator");

        Calculator::Calculator() {
                ichingData.hexagramCheck = false;
                ichingData.testIChingBalance = false;
        }

        Calculator::~Calculator() {
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

        bool Calculator::Calculate(time_t timeLimit, size_t threadsCount, const MaximizationFunction & maximizationFunction)
        {
                S_LOG("Calculate");
                
                std::vector<std::thread> threads;
                
                using namespace std::chrono;
                auto start = steady_clock::now();
                
                maximumValue = 0;
                result.clear();
                
                variantsChecked = 0;
                interrupt = false;
                
                this->maximizationFunction = maximizationFunction;
                
                for (size_t i = 0; i < threadsCount; ++i)
                        threads.emplace_back(&Calculator::CalculationThread, this, i, timeLimit);
                
                for (size_t i = 0; i < threadsCount; ++i){
                        std::thread& thread = threads[i];
                        thread.join();
                }
                
                auto end = steady_clock::now();
                double duration = duration_cast<milliseconds>(end - start).count() * 1E-3;
                lastPerformance = static_cast<double>(variantsChecked) / duration;
                log(logxx::notice) << "Total " << variantsChecked << " decks checked in " << duration << "s" << logxx::endl;
                
                return !result.empty();
        }

        bool Calculator::TestDeck(const std::shared_ptr<Medici>& d, IChingData &iChingData) const {
		//Bottleneck is still selector.Test
                return selector.Test(d->GetDeck()) && d->Collapse(true) && IChingTest(d, iChingData) ;
        }
        
        namespace {
                template<typename T, typename... Args>
                std::unique_ptr<T> make_unique(Args&&... args)
                {
                    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
                }
        }
        
        void Calculator::Maximization(const std::shared_ptr<Medici>& deck) {
                S_LOG("Maximization");
                unsigned int value = maximizationFunction(*deck.get());
                if (value >= maximumValue) {
                        if (value > maximumValue)
                                result.clear();
                        maximumValue = value;
                        auto deckCopy = CopyDeck(deck);
                        result.push_back(deckCopy);

                        auto &s = log(logxx::notice) << "Found deck \n";
                        PrintDeck(deckCopy, s);
                        s << "\nValue = " << value << logxx::endl;
                }
        }

        void Calculator::CalculationThread(size_t threadNumber, time_t timeLimit)
        {
                S_LOG("CalculationThread");
                time_t start(time(nullptr));
                unsigned int seed = start - threadNumber * 10;
                IChingData threadIChingData = ichingData;

                auto testingDeck = std::make_shared<Medici>();
                testingDeck->SetDeck(Deck::GenerateDeck());
                unsigned long long int localVariantsChecked(0);
                while (!interrupt && time(nullptr) <= start + timeLimit){
                        ++localVariantsChecked;
                        if (TestDeck(testingDeck, threadIChingData)){
                                std::lock_guard<std::mutex> lk(mCommonVars);
                                if (maximizationFunction){
                                        Maximization(testingDeck);
                                } else {
                                        result.push_back(testingDeck);
                                        interrupt = true;
                                        break;
                                }
                        }
                        testingDeck->Mix(Calculator::rnd, &seed);
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

        bool Calculator::IChingTest(const std::shared_ptr<Medici>& m, IChingData &iChingData) {
                if (iChingData.IsActive()){
                        auto &iching = iChingData.iching;
                        iching.LoadFromDeck(m);
                        if (iChingData.testIChingBalance && !iching.IsBalanced())
                                return false;
                        else if (iChingData.hexagramCheck && !iching.CheckHexagram(iChingData.hexagramSuit, iChingData.hexagram))
                                return false;
                        else
                                return true;
                } else
                        return true;
        }

        void Calculator::Interrupt() {
                interrupt = true;
        }

        CalculationResult Calculator::GetResult() const {
                return result;
        }

        void Calculator::SetConditions(const ComplexRangeSelector& cond) {
                selector = cond;
        }

        void Calculator::SetConditions() {
                selector.Clear();
        }

        void Calculator::SetIChingSuitHex(PlayingCard::Suit s, const Hexagram& hex) {
                ichingData.hexagramCheck = true;
                ichingData.hexagramSuit = s;
                ichingData.hexagram = hex;
        }

        void Calculator::SetIChingSuitHex(){
                ichingData.hexagramCheck = false;
        }
        
        void Calculator::SetIChingTestBalance(bool flag) {
                ichingData.testIChingBalance = flag;
        }

        std::shared_ptr<Medici> Calculator::CopyDeck(const std::shared_ptr<Medici>& d) {
                return std::make_shared<Medici>(*d.get());
        }

} // namespace dream_hacking
