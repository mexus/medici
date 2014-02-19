#include <algorithm>
#include <limits>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "../sources/logxx/logxx.h"
#include "../sources/patience/medici.h"
#include "../sources/dream_hacking/range_selectors/range_selector.h"
#include "../sources/dream_hacking/calculator.h"

static logxx::Log cLog("test");

unsigned int randomSeed;
size_t rnd(size_t i){ return rand_r(&randomSeed) % i; }

void PrintDeck(const std::vector<PlayingCard>& deck, std::ostream& s, bool abbrevations = true){
        S_LOG("PrintDeck");
        for (auto it = deck.begin(); it != deck.end(); ++it){
                const PlayingCard &card = *it;
                s << card.Print(abbrevations);
                if (it + 1 != deck.end())
                        s << ", ";
        }
}

void PrintDeck(const Deck& d, std::ostream& s, bool abbrevations = true){
        PrintDeck(d.GetDeck(), s, abbrevations);
}

bool TestCard(){
        S_LOG("TestCard");
        PlayingCard card;
        bool reachedEnd(false);
        int cnt(0);
        while (!reachedEnd){
                log(logxx::debug) << "Card: " << card.Print(true) << " <-> " << card.Print() << logxx::endl;
                card.Next(reachedEnd);
                ++cnt;
        }
        log(logxx::notice) << "Total " << cnt << " cards" << logxx::endl;
        return cnt == 36;
        
}

template<class T1, class T2>
double Percent(T1 i, T2 total){
        return 100.0 * static_cast<double>(i) / static_cast<double>(total);
}

unsigned long long Factorial(size_t val){
        unsigned long long res(1);
        for (size_t i = 2; i <= val; ++i)
                res *= i;
        return res;
}

bool TestStatisticsMixing(){
        S_LOG("TestStatisticsMixing");
        double maxDuplicates = 0.5; // in percents
        std::vector< std::vector<PlayingCard> > decks;
        static const unsigned long sequencesCount = 1E6;
        decks.reserve(sequencesCount);
        log(logxx::info) << "Generating " << sequencesCount << " decks" << logxx::endl;
        auto testDeck = Deck::GenerateDeck();
        decks.push_back(testDeck);
        for (unsigned long i = 0; i < sequencesCount; ++i){
                Deck::Mix(testDeck, rnd);
                decks.push_back(testDeck);
        }
        std::sort(decks.begin(), decks.end());
        unsigned long duplicates(0);
        auto start = decks.begin();
        while ((start = std::adjacent_find(start, decks.end())) != decks.end()){
                ++duplicates;
                ++start;
        }
        
        double duplicatesPercent = Percent(duplicates, sequencesCount);
        log(logxx::info) << "Found " << duplicates << " duplicates (" << duplicatesPercent << "%)" << logxx::endl;
        if (duplicatesPercent > maxDuplicates){
                log(logxx::error) << "Too many duplicates" << logxx::endl;
                return false;
        } else
                return true;
}

bool TestStatisticsReaching(){
        S_LOG("TestStatisticsReaching");
        double maxError = 5.0; // in percents
        for (size_t maxCards = 3; maxCards <= 7; ++maxCards){
                auto initDeck = Deck::GenerateDeck();
                initDeck.resize(maxCards);
                auto testDeck = initDeck;

                static const int tests = 1E4;
                double averageCnt(0.0);
                for (int i = 0; i < tests; ++i){
                        unsigned long cnt (0);
                        do {
                                Deck::Mix(testDeck, rnd);
                                ++cnt;
                        } while (testDeck != initDeck);
                        averageCnt += cnt;
                }
                averageCnt /= static_cast<double>(tests);
                auto uniqueCnt = Factorial(maxCards);
                double error = Percent(averageCnt, uniqueCnt);
                log(logxx::info, std::to_string(maxCards) + " cards") <<
                        "Initial deck reached in " << averageCnt << " permutations (average), should be " << uniqueCnt << ": " << 
                        error << "%" << logxx::endl;
                if (std::abs(error - 100.0) > maxError){
                        log(logxx::error) << "Error is too large" << logxx::endl;
                        return false;
                }
        }
        
        return true;
}

void TestMixPerformance(){
        S_LOG("TestMixPerformance");
        static const long int permutations = 1E8;
        auto initDeck = Deck::GenerateDeck();
        time_t start(time(nullptr));
        for (long int i = 0; i < permutations; ++i)
                Deck::Mix(initDeck, rnd);
        log(logxx::info) << permutations << " permutations done in " << time(nullptr) - start << " seconds" << logxx::endl;
}

bool TestMedici(){
        S_LOG("TestMedici");
        Medici deck;
        deck.SetDeck(Medici::FromString(
                "Вч 9к Тч Вк 7п Xк 9п 7ч 9ч Тб 6ч Дп 8ч Кб Тп Xп Вб 8п 7к 9б 6п Дч Кч Вп Xч Кп 8к Дб Xб 8б 6к Дк 7б Тк 6б Кк"));
        if (!deck.Collapse()){
                auto &s = log(logxx::error) << "Deck should collapse, but it does not: \n";
                PrintDeck(deck, s);
                s << logxx::endl;
                return false;
        }
        
        deck.SetDeck(Medici::FromString(
                "Xч 8к Тч Вк 7п Xк 9п 7ч 9ч Тб 6ч Дп 8ч Кб Тп Xп Вб 8п 7к 9б 6п Дч Кч Вп Вч Кп 9к Дб Xб 8б 6к Дк 7б Тк 6б Кк"));
        if (deck.Collapse()){
                auto &s = log(logxx::error) << "Deck should not collapse, but it does: \n";
                PrintDeck(deck, s);
                s << logxx::endl;
                return false;
        }
        return true;
}

void TestMediciCollapsePerformance(){
        S_LOG("TestMediciCollapsePerformance");
        static const unsigned long int sequences = 1E6;
        static const unsigned int tests = 5;
        unsigned long long int totalCollapses(0);
        
        time_t testStart(time(nullptr));
        for (unsigned int test = 0; test < tests; ++test){
                Medici deck;
                deck.SetDeck(Deck::GenerateDeck());
                for (unsigned long int i = 0; i < sequences; ++i){
                        deck.Mix(rnd);
                        if (deck.Collapse())
                                ++totalCollapses;
                }
        }
        double collapsingPercent = Percent(totalCollapses, tests * sequences);
        double duration = time(nullptr) - testStart;
        log(logxx::info) << tests << " test done in " << duration << " seconds, average collapsed percent: "
                << collapsingPercent << "%" <<logxx::endl;
        log(logxx::info) << "Average analizing performance: \n" << 
                "\t" << static_cast<double>(sequences * tests) / duration << " sequences in a second\n" <<
                "\t" << static_cast<double>(totalCollapses) / duration << " collapsing sequences in a second"
                << logxx::endl;
}

bool TestCardSelector(){
        S_LOG("TestCardSelector");
        PlayingCard card{PlayingCard::Ten, PlayingCard::Diamonds};
        dream_hacking::CardSelector c1(card);
        if (!c1.Test(card)){
                log(logxx::error, "same card") << "CardSelector::Test returned false, should be true" << logxx::endl;
                return false;
        }
        dream_hacking::CardSelector c2(card);
        if (c1 != c2){
                log(logxx::error, "same card") << "{CardSelector}s not equal, but should be" << logxx::endl; ;
                return false;
        }
        dream_hacking::CardSelector c3(PlayingCard::Hearts);
        if (c1 == c3){
                log(logxx::error, "another suit") << "{CardSelector}s are equal, but should not be" << logxx::endl; ;
                return false;
        }
        if (c3.Test(card)){
                log(logxx::error, "another suit") << "CardSelector::Test returned true, should be false" << logxx::endl;
                return false;
        }
        dream_hacking::CardSelector c4(card, true);
        if (c1 == c4){
                log(logxx::error, "inverse") << "{CardSelector}s are equal, but should be inversed" << logxx::endl;
                return false;
        }
        if (c4.Test(card)){
                log(logxx::error, "inverse") << "CardSelector::Test returned true, should be false" << logxx::endl;
                return false;
        }
        
        dream_hacking::CardSelector suitTest(card.GetSuit());
        if (!suitTest.Test(card)){
                log(logxx::error, "suitTest") << "CardSelector::Test returned false, should be true" << logxx::endl;
                return false;
        }
        
        if (!suitTest.Test({PlayingCard::Ace, card.GetSuit()})){
                log(logxx::error, "suitTest", "same suit") << "CardSelector::Test returned false, should be true" << logxx::endl;
                return false;
        }
        
        if (suitTest.Test({PlayingCard::Ace, PlayingCard::Clubs})){
                log(logxx::error, "suitTest", "another suit") << "CardSelector::Test returned true, should be false" << logxx::endl;
                return false;
        }
        
        return true;
}

bool TestUniversalRangeSelector(){
        S_LOG("TestUniversalRangeSelector");
        using namespace dream_hacking;
        std::vector<PlayingCard> deck = Deck::FromString("Xп 8б Тк Кч 7к 6п Вк Дч");
        
        UniversalRangeSelector conditions(1, 3);
        conditions.AddCard(PlayingCard("Кч"));
        conditions.AddCard(PlayingCard::Clubs);
        conditions.AddCard(PlayingCard::Eight);
        if (!conditions.Test(deck)){
                log(logxx::error) << "RangeSelector::Test returned false, should be true" << logxx::endl;
                return false;
        }
        
        conditions.ClearCards();
        conditions.AddCard({PlayingCard("Кч"), true});
        if (conditions.Test(deck)){
                log(logxx::error, "inverse") << "RangeSelector::Test returned true, should be false" << logxx::endl;
                return false;
        }
        
        return true;
}

bool TestExistentialRangeSelector(){
        S_LOG("TestExistentialRangeSelector");
        using namespace dream_hacking;
        std::vector<PlayingCard> deck = Deck::FromString("Xп 8б Тк Кч 7к 6п Вк Дч");
        
        ExistentialRangeSelector conditions(2, 4);
        conditions.AddCard(PlayingCard::Six);
        conditions.AddCard(PlayingCard::Ace);
        conditions.AddCard(PlayingCard::Queen);
        if (!conditions.Test(deck)){
                log(logxx::error) << "RangeSelector::Test returned false, should be true" << logxx::endl;
                return false;
        }
        
        conditions.ClearCards();
        conditions.AddCard(PlayingCard::Six);
        conditions.AddCard(PlayingCard::Jack);
        conditions.AddCard(PlayingCard::Queen);
        if (conditions.Test(deck)){
                log(logxx::error, "not exists") << "RangeSelector::Test returned true, should be false" << logxx::endl;
                return false;
        }
        
        conditions.ClearCards();
        conditions.AddCard({PlayingCard::Ten, true});
        if (!conditions.Test(deck)){
                log(logxx::error, "inverse") << "RangeSelector::Test returned false, should be true" << logxx::endl;
        }
        
        conditions.ClearCards();
        conditions.AddCard(PlayingCard::Queen);
        if (conditions.Test(deck)){
                log(logxx::error, "inverse") << "RangeSelector::Test returned true, should be false" << logxx::endl;
                return false;
        }
        
        return true;
}

bool TestComplexRangeSelector(){
        S_LOG("TestComplexRangeSelector");
        using namespace dream_hacking;
        std::vector<PlayingCard> deck = Deck::FromString(
                "Вч 9к Тч Вк 7п Xк 9п 7ч 9ч Тб 6ч Дп 8ч Кб Тп Xп Вб 8п 7к 9б 6п Дч Кч Вп Xч Кп 8к Дб Xб 8б 6к Дк 7б Тк 6б Кк");
        //       0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35
        
        ExistentialRangeSelector target(19, 24);
        target.AddCard({PlayingCard::Ten, PlayingCard::Hearts});
        
        UniversalRangeSelector ownActions(3, 7);
        ownActions.AddCard({PlayingCard::Ace, true});
        
        ExistentialRangeSelector firstCard(0, 0);
        firstCard.AddCard({PlayingCard::Jack});
        
        ExistentialRangeSelector secondCard(1, 1);
        secondCard.AddCard({PlayingCard::Nine});
        
        ExistentialRangeSelector thirdCard(2, 2);
        thirdCard.AddCard({PlayingCard::Ace});
        thirdCard.AddCard({PlayingCard::Ten});
        
        ComplexRangeSelector conditions;
        conditions.AddRangeSelectors(target, ownActions, firstCard, secondCard, thirdCard);
        
        if (!conditions.Test(deck)){
                log(logxx::error) << "ComplexRangeSelector::Test returned false, but it should be true" << logxx::endl;
                return false;
        }
        
        deck = Deck::FromString(
                "Вч 9к Тч Вк 7п Xк 9п 7ч 9ч Тб 6ч Дп 8ч Xч Тп Xп Вб 8п 7к 9б 6п Дч Кч Вп Кб Кп 8к Дб Xб 8б 6к Дк 7б Тк 6б Кк");
        if (conditions.Test(deck)){
                log(logxx::error) << "ComplexRangeSelector::Test returned true, but it should be false" << logxx::endl;
                return false;
        }
        
        deck = Deck::FromString(
                "Тч 9к Вч Вк 7п Xк 9п 7ч 9ч Тб 6ч Дп 8ч Кб Тп Xп Вб 8п 7к 9б 6п Дч Кч Вп Xч Кп 8к Дб Xб 8б 6к Дк 7б Тк 6б Кк");
        if (conditions.Test(deck)){
                log(logxx::error) << "ComplexRangeSelector::Test returned true, but it should be false" << logxx::endl;
                return false;
        }
        
        deck = Deck::FromString(
                "Вч 9к Тч Вк Тб Xк 9п 7ч 9ч 7п 6ч Дп 8ч Кб Тп Xп Вб 8п 7к 9б 6п Дч Кч Вп Xч Кп 8к Дб Xб 8б 6к Дк 7б Тк 6б Кк");
        if (conditions.Test(deck)){
                log(logxx::error) << "ComplexRangeSelector::Test returned true, but it should be false" << logxx::endl;
                return false;
        }
        
        return true;
}
        
inline bool ExampleConditions(const std::vector<PlayingCard> &deck, const PlayingCard &target){
        decltype(deck.begin()) targetBegin = deck.begin() + 19;
        decltype(deck.begin()) targetEnd = deck.begin() + 25;
        
        decltype(deck.begin()) ownActionsBegin = deck.begin() + 3;
        decltype(deck.begin()) ownActionsEnd = deck.begin() + 9;
        
        return 
                deck[0].GetNumber() == PlayingCard::Jack &&
                deck[1].GetNumber() == PlayingCard::Nine &&
                (deck[2].GetNumber() == PlayingCard::Ace || deck[2].GetNumber() == PlayingCard::Ten) &&
                std::find(targetBegin, targetEnd, target) != targetEnd &&
                std::find_if(ownActionsBegin, ownActionsEnd, [](const PlayingCard& p)->bool{
                        return p.GetNumber() == PlayingCard::Ace;
                }) == ownActionsEnd &&
                deck[35].GetNumber() == PlayingCard::Ten;
}

bool TestCalculator(size_t threads = 1, const std::string &label = "TestCalculator", double *performance = nullptr){
        D_LOG(label);
        using namespace dream_hacking;
        
        PlayingCard targetCard{PlayingCard::Ten, PlayingCard::Hearts};
        
        ExistentialRangeSelector target(19, 24);
        target.AddCard(targetCard);
        
        UniversalRangeSelector ownActions(3, 7);
        ownActions.AddCard({PlayingCard::Ace, true});
        
        ExistentialRangeSelector firstCard(0, 0);
        firstCard.AddCard({PlayingCard::Jack});
        
        ExistentialRangeSelector secondCard(1, 1);
        secondCard.AddCard({PlayingCard::Nine});
        
        ExistentialRangeSelector thirdCard(2, 2);
        thirdCard.AddCard({PlayingCard::Ace});
        thirdCard.AddCard({PlayingCard::Ten});
        
        ComplexRangeSelector conditions;
        conditions.AddRangeSelectors(target, ownActions, firstCard, secondCard, thirdCard);
        
        Calculator calc(conditions);
        calc.SetThreads(threads);
        
        auto deck = calc.Calculate();
        if (deck){
                Medici result = *(deck.get());
                auto &s = log(logxx::info) << "Found: \n";
                PrintDeck(result, s);
                s << logxx::endl;
        } else {
                log(logxx::error) << "Not found any combinations!" << logxx::endl;
                return false;
        }
        
        deck = calc.Calculate(15, [&targetCard](const std::shared_ptr<Medici>& d) -> unsigned int {
                return d->GetCollapses(targetCard);
        });
        if (performance)
                *performance = calc.GetLastPerformance();
        
        if (deck){
                Medici result = *(deck.get());
                auto &s = log(logxx::info) << "Found: \n";
                PrintDeck(result, s);
                auto val = result.GetCollapses(targetCard);
                s << "\nValue: " << val << logxx::endl;
                if (val < 1){
                        log(logxx::error) << "Not found any positive-valued sequence" << logxx::endl;
                        return false;
                }
        } else {
                log(logxx::error) << "Not found any combinations!" << logxx::endl;
                return false;
        }
        
        return true;
}

size_t rnd(size_t i, unsigned int *seed){
        return rand_r(seed) % i;
}

void TestMultithreadPerformance(){
        S_LOG("TestMultithreadPerformance");
        
        unsigned int maxPerformance(0);
        
        for (size_t threadsCount = 1; threadsCount <= 5; ++threadsCount){
                size_t ready(0);
                std::mutex mCreate, mStart, mDone;
                std::condition_variable cvCreate, cvStart, cvDone;

                static const size_t count = 1E6;

                std::vector<std::thread> threads;
                threads.reserve(threadsCount);
                for (size_t i = 0; i < threadsCount; ++i){
                        threads.emplace_back([&mCreate, &cvCreate, &ready, &mStart, &cvStart, &mDone, &cvDone, i](){
                                {
                                        std::lock_guard<std::mutex> lg(mCreate);
                                        ready += 1;
                                }
                                Medici deck;
                                deck.SetDeck(Deck::GenerateDeck());
                                unsigned int rndSeed = i * 10;
                                log(logxx::debug, i) << "Thread started" << logxx::endl;
                                cvCreate.notify_all();
                                {
                                        std::unique_lock<std::mutex> lk(mStart);
                                        cvStart.wait(lk);
                                }
                                log(logxx::debug, i) << "Execution begin" << logxx::endl;

                                for (size_t i = 0; i < count; ++i){
                                        deck.Mix([&rndSeed](size_t i)->size_t{
                                                return rand_r(&rndSeed) % i;
                                        });
                                        deck.Collapse();
                                }

                                log(logxx::debug, i) << "Execution done" << logxx::endl;
                                {
                                        std::lock_guard<std::mutex> lg(mDone);
                                        ready += 1;
                                }
                                cvDone.notify_all();
                        });
                }

                {
                        std::unique_lock<std::mutex> lk(mCreate);
                        cvCreate.wait(lk, [&ready, &threadsCount]()->bool{return ready == threadsCount;});
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }

                log(logxx::debug) << "All thread created, waiting for a second" << logxx::endl;
                ready = 0;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                
                unsigned int performance(0);

                cvStart.notify_all();
                {
                        using namespace std::chrono;
                        steady_clock::time_point start = steady_clock::now();
                        std::unique_lock<std::mutex> lk(mDone);
                        cvDone.wait(lk, [&ready, &threadsCount]()->bool{return ready == threadsCount;});
                        steady_clock::time_point end = steady_clock::now();
                        double duration = duration_cast<milliseconds>(end - start).count() * 1E-3;
                        performance = static_cast<double>(count * threadsCount) / duration;
                        log(logxx::info, threadsCount) << "Average performance: " <<
                                performance << 
                                " collapse attempts per second" << logxx::endl;
                }

                for (size_t i = 0; i < threadsCount; ++i){
                        std::thread &thread = threads[i];
                        thread.join();
                }
                
                if (performance > maxPerformance){
                        maxPerformance = performance;
                } else {
                        log(logxx::info) << "Max performance reached at threads = " << threadsCount - 1 << logxx::endl;
                        break ;
                }
        }
}

bool TestMultithreadStatistics(){
        S_LOG("TestMultithreadStatistics");
        
        static const double maxDuplicates = 0.5; // in percents
        
        size_t threadsCount = 1;
        for (; threadsCount <= 5; ++threadsCount){
                static const size_t count = 1E5;

                std::mutex mDecks;
                std::vector< std::vector<PlayingCard> > decks;
                decks.reserve(count * threadsCount);
                
                std::vector<std::thread> threads;
                threads.reserve(threadsCount);
                for (size_t i = 0; i < threadsCount; ++i){
                        threads.emplace_back([&decks, &mDecks, i](){
                                std::vector< std::vector<PlayingCard> > localDecks;
                                localDecks.reserve(count);
                                std::vector<PlayingCard> cardsDeck = Deck::GenerateDeck();
                                unsigned int rndSeed = i * 10;

                                localDecks.push_back(cardsDeck);
                                for (size_t i = 0; i < count; ++i){
                                        Deck::Mix(cardsDeck, [&rndSeed](size_t i)->size_t{
                                                return rand_r(&rndSeed) % i;
                                        });
                                        localDecks.push_back(cardsDeck);
                                }
                                
                                std::lock_guard<std::mutex> lg(mDecks);
                                decks.insert(decks.end(), localDecks.begin(), localDecks.end());
                        });
                }

                std::this_thread::sleep_for(std::chrono::seconds(1));

                for (size_t i = 0; i < threadsCount; ++i){
                        std::thread &thread = threads[i];
                        thread.join();
                }
                
                std::sort(decks.begin(), decks.end());
                unsigned long duplicates(0);
                auto start = decks.begin();
                while ((start = std::adjacent_find(start, decks.end())) != decks.end()){
                        ++duplicates;
                        ++start;
                }
                
                double duplicatesPercent = Percent(duplicates, count * threadsCount);
                log(logxx::info, threadsCount) << "Found " << duplicates << " duplicates (" << duplicatesPercent << "%)" << logxx::endl;                
                
                if (duplicatesPercent > maxDuplicates){
                        log(logxx::error) << "Too many duplicates" << logxx::endl;
                        break;
                }
                
        }
        return threadsCount > 1;
}

bool TestMultiThreadCalculator(){
        size_t maxThreads = 6;
        bool res = true;
        
        double maxPerf(0.0);
        size_t optimalThreads(0);
        
        S_LOG("TestMultiThreadCalculator");
        auto storedLevel = logxx::GlobalLogLevel();
        logxx::GlobalLogLevel(logxx::warning);
        
        for (size_t threads = 1; threads <= maxThreads; ++threads){
                double perf(0.0);
                if (TestCalculator(threads, "TestMultiThreadCalculator", &perf)){
                        log(logxx::info, threads) << (long long int)perf << " decks per second" << logxx::endl;
                        if (perf > maxPerf){
                                maxPerf = perf;
                                optimalThreads = threads;
                        }
                } else {
                        log(logxx::error) << "TestCalculator has failed" << logxx::endl;
                        res = false;
                        break;
                }
        }
        
        log(logxx::info) << "Max performance: " << (int)maxPerf << " decks per second, at threads = " << optimalThreads << logxx::endl;
        
        logxx::GlobalLogLevel(storedLevel);
        return res;
}

#define RUN_TEST(function) \
if (!function()) \
        log(logxx::warning, #function) << "TEST FAILED" << logxx::endl;

int main() {
        S_LOG("main");
        logxx::GlobalLogLevel(logxx::notice);
//        randomSeed = time(nullptr);
//        RUN_TEST(TestCard);
//        RUN_TEST(TestStatisticsMixing);
//        RUN_TEST(TestStatisticsReaching);
//        TestMixPerformance();
//        RUN_TEST(TestMedici);
//        TestMediciCollapsePerformance();
//        RUN_TEST(TestCardSelector);
//        RUN_TEST(TestUniversalRangeSelector);
//        RUN_TEST(TestExistentialRangeSelector);
//        RUN_TEST(TestComplexRangeSelector);
//        RUN_TEST(TestCalculator);
//        logxx::GlobalLogLevel(logxx::debug);
//        TestMultithreadPerformance();
//        RUN_TEST(TestMultithreadStatistics);
        RUN_TEST(TestMultiThreadCalculator);

//	std::srand(time(nullptr));
//
//        PlayingCard target("Xч");
//        Medici testDeck;
//        Medici idealDeck;
//        std::vector<PlayingCard> deck = Deck::GenerateDeck();
////        unsigned int maxCollapses(0);
//        long int collapsed(0), conditionsMet(0);
//        for (long long int i = 0; i < std::numeric_limits<decltype(i)>::max(); ++i){
////                if (Conditions(deck, target)){
////                        testDeck.SetDeck(deck);
////                        if (testDeck.Collapse()){
////                                auto targetCollapses = testDeck.GetCollapses(target);
////                                if (targetCollapses > maxCollapses){
////                                        maxCollapses = targetCollapses;
////                                        PrintDeck(testDeck);
////                                        log(logxx::info) << target.Print() << " collapses: " << targetCollapses << logxx::endl;
////                                        idealDeck = testDeck;
////                                }
////                        }
////                } else {
//		if (Conditions(deck, target)){
//			++conditionsMet;
//                	testDeck.SetDeck(deck);
//	                if (testDeck.Collapse())
//	                        ++collapsed;
//		}
//                
//                        static time_t start(time(nullptr));
//                        time_t now(time(nullptr));
//                        static const time_t maxExecutionTime = 60;
//                        if (now > start + maxExecutionTime){
//                                log(logxx::info) << "Combinations: " << i << logxx::endl;
//                                break;
//                        }
////                }
//                //std::random_shuffle(deck.begin(), deck.end(), rnd);
//                Deck::Mix(deck, rnd);
//        }
//        log(logxx::info) << "Collapsed: " << collapsed << logxx::endl;
//        log(logxx::info) << "Conditions met: " << conditionsMet << logxx::endl;
        
//        log(logxx::info) << "Selected deck: " << logxx::endl;
//        PrintDeck(idealDeck);
//        log(logxx::info) << "Collapses: " << logxx::endl;
//        for (auto &pair : idealDeck.GetCollapses()){
//                log(logxx::info) << pair.first.Print() << ": " << pair.second << logxx::endl;
//        }
        
        
        
        
        return 0;
}
