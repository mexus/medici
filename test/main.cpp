#include <algorithm>
#include <limits>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "../sources/logxx/logxx.h"
#include "../sources/patience/medici.h"
#include "../sources/dream_hacking/range_selectors/range_selector.h"
#include "../sources/dream_hacking/calculator.h"
#include "../sources/dream_hacking/i-ching/i_ching.h"

static logxx::Log cLog("test");

unsigned int randomSeed;
size_t optimalThreads = 0;

size_t rnd(size_t i){ return rand_r(&randomSeed) % i; }
void PrintHexagrams(const dream_hacking::IChing &iching, const std::string& label, logxx::LogLevel logLevel);

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

void PrintDeck(const Medici& d, std::ostream& s, bool abbrevations = true){
        S_LOG("PrintDeck");
        if (d.IsCollapsed()){
                auto deck = d.GetDeck();
                auto collapses = d.GetCollapses();
                auto &stationars = d.GetStationars();
                s << "<";
                for (auto it = deck.begin(); it != deck.end(); ++ it){
                        const PlayingCard& card = *it;
                        std::string cardText = card.Print(abbrevations);
                        if (stationars.find(card) != stationars.end())
                                s << "[" << cardText << "]";
                        else
                                s << cardText;
                        if (it + 1 != deck.end() && collapses.find(card) != collapses.end()){
                                s << "> <";
                        } else if (it + 1 != deck.end())
                                s << " ";
                }
                s << ">";
        } else
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
        log(logxx::info) << "OK" << logxx::endl;
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
        log(logxx::notice) << "Generating " << sequencesCount << " decks" << logxx::endl;
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
        log(logxx::notice) << "Found " << duplicates << " duplicates (" << duplicatesPercent << "%)" << logxx::endl;
        if (duplicatesPercent > maxDuplicates){
                log(logxx::error) << "Too many duplicates" << logxx::endl;
                return false;
        } else{
                log(logxx::info) << "OK" << logxx::endl;
                return true;
        }
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
                log(logxx::notice, std::to_string(maxCards) + " cards") <<
                        "Initial deck reached in " << averageCnt << " permutations (average), should be " << uniqueCnt << ": " << 
                        error << "%" << logxx::endl;
                if (std::abs(error - 100.0) > maxError){
                        log(logxx::error) << "Error is too large" << logxx::endl;
                        return false;
                }
        }
        
        log(logxx::info) << "OK" << logxx::endl;
        return true;
}

void TestMixPerformance(){
        S_LOG("TestMixPerformance");
        using namespace std::chrono;
        
        static const long int permutations = 1E8;
        auto initDeck = Deck::GenerateDeck();
        
        auto start = steady_clock::now();
        for (long int i = 0; i < permutations; ++i)
                Deck::Mix(initDeck, rnd);
        double duration = duration_cast<milliseconds>(steady_clock::now() - start).count() * 1E-3;
        log(logxx::info) << permutations << " permutations done in " << duration << " seconds = " << (int)(
                static_cast<double>(permutations) / duration) << " permutations per second" << logxx::endl;
        
        log(logxx::info) << "OK" << logxx::endl;
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
        
        log(logxx::info) << "OK" << logxx::endl;
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
        log(logxx::notice) << tests << " test done in " << duration << " seconds, average collapsed percent: "
                << collapsingPercent << "%" <<logxx::endl;
        log(logxx::info) << "Average analizing performance: \n" << 
                "\t" << static_cast<double>(sequences * tests) / duration << " sequences in a second\n" <<
                "\t" << static_cast<double>(totalCollapses) / duration << " collapsing sequences in a second"
                << logxx::endl;
        
        log(logxx::info) << "OK" << logxx::endl;
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
        
        log(logxx::info) << "OK" << logxx::endl;
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
        
        log(logxx::info) << "OK" << logxx::endl;
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
        
        log(logxx::info) << "OK" << logxx::endl;
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
        
        log(logxx::info) << "OK" << logxx::endl;
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
        
        if (calc.Calculate()){
                Medici result = calc.GetResult();
                auto &s = log(logxx::notice) << "Found: \n";
                PrintDeck(result, s);
                s << logxx::endl;
        } else {
                log(logxx::error) << "Not found any combinations!" << logxx::endl;
                return false;
        }
        
        static const time_t timeLimit = 5;
        bool res = calc.Calculate(timeLimit, [&targetCard](const Medici& d) -> unsigned int {
                return d.GetCollapses(targetCard);
        });
        if (performance)
                *performance = calc.GetLastPerformance();
        
        if (res){
                Medici result = calc.GetResult();
                auto &s = log(logxx::notice) << "Found: \n";
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
        
        log(logxx::info, std::to_string(threads) + " threads") << "OK" << logxx::endl;
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

                log(logxx::debug) << "All threads created, waiting for a second" << logxx::endl;
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
                        log(logxx::notice, threadsCount) << "Average performance: " <<
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
        if (maxPerformance == 0){
                log(logxx::error) << "Performance is zero!" << logxx::endl;
        } else
                log(logxx::info) << "OK" << logxx::endl;
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
                log(logxx::notice, threadsCount) << "Found " << duplicates << " duplicates (" << duplicatesPercent << "%)" << logxx::endl;                
                
                if (duplicatesPercent > maxDuplicates){
                        log(logxx::error) << "Too many duplicates" << logxx::endl;
                        break;
                }
                
        }
        if (threadsCount > 1){
                log(logxx::info) << "OK" << logxx::endl;
                return true;
        } else
                return false;
}

bool TestMultiThreadCalculator(){
        size_t maxThreads = 6;
        bool res = true;
        
        double maxPerf(0.0);
        
        S_LOG("TestMultiThreadCalculator");
        auto storedLevel = logxx::GlobalLogLevel();
        logxx::GlobalLogLevel(logxx::warning);
        
        for (size_t threads = 1; threads <= maxThreads; ++threads){
                double perf(0.0);
                if (TestCalculator(threads, "TestMultiThreadCalculator", &perf)){
                        log(logxx::notice, threads) << (long long int)perf << " decks per second" << logxx::endl;
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
        if (res){
                log(logxx::info) << "OK" << logxx::endl;
                return true;
        } else
                return false;
}

bool TestMobilesAndStationars(){
        S_LOG("TestMobilesAndStationars");
        static const std::string deckStr = "Вч 9ч Тч Дп 6б Тп 7п Кб 9б 9п 6ч Кк Кп Дк Xп 8б 7б 7ч Вб Вп 6п Дб Вк Xк 8п Тб 9к 6к Xб Кч Тк Xч 8ч 8к Дч 7к";
        Medici deck;
        deck.SetDeck(Deck::FromString(deckStr));
        if (deck.Collapse(true)){
                auto &s0 = log(logxx::debug);
                PrintDeck(deck, s0);
                s0 << logxx::endl;
                
                auto &stationars = deck.GetStationars();
                auto &mobiles = deck.GetMobiles();
                auto &s = log(logxx::debug, "stationars");
                for (auto &card: stationars){
                        s << card.Print(true) << " ";
                }
                s << logxx::endl;
                auto &s2 = log(logxx::debug, "mobiles");
                for (auto &card: mobiles){
                        s2 << card.Print(true) << " ";
                }
                s2 << logxx::endl;
                
                std::set<PlayingCard> etalonStationars {
                        PlayingCard("Вч"), PlayingCard("Тч"), PlayingCard("Дп"), PlayingCard("7п"), PlayingCard("9б"), PlayingCard("9п"),
                        PlayingCard("Кк"), PlayingCard("Xп"), PlayingCard("8б"), PlayingCard("7б"), PlayingCard("Вб"), PlayingCard("6п"),
                        PlayingCard("Вк"), PlayingCard("Xк"), PlayingCard("Тб"), PlayingCard("6к"), PlayingCard("Кч"), PlayingCard("8ч")
                };
                
                if (etalonStationars != stationars){
                        log(logxx::error) << "Stationars are not equal" << logxx::endl;
                        return false;
                } else {
                        for (auto &mobileCard : mobiles){
                                if (etalonStationars.find(mobileCard) != etalonStationars.end()){
                                        log(logxx::error, mobileCard.Print(true)) << "Mobile card, but should be stationar" << logxx::endl;
                                        return false;
                                }
                        }
                }
                log(logxx::info) << "OK" << logxx::endl;
                return true;
        } else {
                log(logxx::error) << "Deck should collapse" << logxx::endl;
                return false;
        }
}

void PrintHexagrams(const dream_hacking::IChing &iching, const std::string& label, logxx::LogLevel logLevel){
        S_LOG(label + "::PrintHexagrams");
        using namespace dream_hacking;
        for (auto &suitHex : iching.hexagrams){
                auto &suit = suitHex.first;
                auto &hex = suitHex.second;

                auto &s = log(logLevel) << PlayingCard::PrintSuit(suit, false) << "\n";
                if (!s.good())
                        break;
                for (size_t i = 0; i != 6; ++i){
                        const HexagramState& state = hex.at(5 - i);
                        if (state == SolidLine || state == SolidLineStrong || state == SolidLineWeak)
                                s << "======";
                        else
                                s << "==__==";
                        if (i != 5)
                                s << "\n";
                }
                s << logxx::endl;
        }
}

bool TestIChing(){
        S_LOG("Test I-Ching");
        using namespace dream_hacking;
        static const std::string deckStr = "Вч 9ч Тч Дп 6б Тп 7п Кб 9б 9п 6ч Кк Кп Дк Xп 8б 7б 7ч Вб Вп 6п Дб Вк Xк 8п Тб 9к 6к Xб Кч Тк Xч 8ч 8к Дч 7к";
        Medici deck;
        deck.SetDeck(Deck::FromString(deckStr));
        if (deck.Collapse(true)){
                IChing iching;
                iching.LoadFromDeck(deck);
                PrintHexagrams(iching, "Test I-Ching", logxx::debug);
                std::map<PlayingCard::Suit, Hexagram> etalonHexagrams{
                        {PlayingCard::Hearts, {SolidLineStrong, SolidLineWeak, SolidLine, SolidLineWeak, OpenedLine, OpenedLine}},
                        {PlayingCard::Diamonds, {OpenedLineWeak, SolidLineWeak, SolidLine, SolidLineWeak, SolidLine, OpenedLine}},
                        {PlayingCard::Clubs, {SolidLineStrong, OpenedLineStrong, OpenedLine, SolidLineWeak, OpenedLine, SolidLine}},
                        {PlayingCard::Spades, {OpenedLineWeak, OpenedLineStrong, OpenedLine, OpenedLineStrong, SolidLine, SolidLine}}
                };
                
                for (auto &suitHex : iching.hexagrams){
                        auto &suit = suitHex.first;
                        auto &hex = suitHex.second;
                        auto &etalonHex = etalonHexagrams[suit];
                        
                        if (hex != etalonHex){
                                log(logxx::error, PlayingCard::PrintSuit(suit)) << "Hexagrams are not equal" << logxx::endl;
                                return false;
                        }
                }
                log(logxx::info) << "OK" << logxx::endl;
                return true;
        } else {
                log(logxx::error) << "Deck should collapse" << logxx::endl;
                return false;
        }
}

bool TestIChingBalanced(){
        S_LOG("Test I-Ching balanced");
        using namespace dream_hacking;
        IChing iching;
        std::map<PlayingCard::Suit, Hexagram> unbalancedHexagrams{
                {PlayingCard::Hearts, {SolidLineStrong, SolidLineWeak, SolidLine, SolidLineWeak, OpenedLine, OpenedLine}},
                {PlayingCard::Diamonds, {OpenedLineWeak, SolidLineWeak, SolidLine, SolidLineWeak, SolidLine, OpenedLine}},
                {PlayingCard::Clubs, {SolidLineStrong, OpenedLineStrong, OpenedLine, SolidLineWeak, OpenedLine, SolidLine}},
                {PlayingCard::Spades, {OpenedLineWeak, OpenedLineStrong, OpenedLine, OpenedLineStrong, SolidLine, SolidLine}}
        };
        iching.hexagrams = unbalancedHexagrams;
        if (iching.IsBalanced()){
                log(logxx::error) << "Hexagrams are unbalanced, but reported as balanced" << logxx::endl;
                return false;
        }
        std::map<PlayingCard::Suit, Hexagram> balancedHexagrams{
                {PlayingCard::Hearts, {SolidLineStrong, SolidLineWeak, SolidLine, SolidLineWeak, OpenedLine, OpenedLine}},
                {PlayingCard::Diamonds, {OpenedLineWeak, SolidLineWeak, SolidLine, OpenedLineWeak, SolidLine, OpenedLine}},
                {PlayingCard::Clubs, {SolidLineStrong, OpenedLineStrong, OpenedLine, SolidLineWeak, OpenedLine, SolidLine}},
                {PlayingCard::Spades, {OpenedLineWeak, OpenedLineStrong, OpenedLine, OpenedLineStrong, SolidLine, SolidLine}}
        };
        iching.hexagrams = balancedHexagrams;
        if (!iching.IsBalanced()){
                log(logxx::error) << "Hexagrams are balanced, but reported as unbalanced" << logxx::endl;
                return false;
        }
        log(logxx::info) << "OK" << logxx::endl;
        return true;
}

bool TestIChingCalculator(){
        S_LOG("Tets I-Ching calculator");
        using namespace dream_hacking;
        
        Calculator calc;
        calc.ActivateIChingAnalyze();
        
        PlayingCard targetCard("Тч");
        ComplexRangeSelector &conditions = calc.AccessConditions();
        
        ExistentialRangeSelector targetRange(19, 24);
        targetRange.AddCard(targetCard);
        
        UniversalRangeSelector ownActions(3, 7);
	ownActions.AddCard({PlayingCard::Six});
	ownActions.AddCard({PlayingCard::Seven});
	ownActions.AddCard({PlayingCard::Nine});
	ownActions.AddCard({PlayingCard::Jack});
	ownActions.AddCard({PlayingCard::Queen});
        
        ExistentialRangeSelector firstCard(0, 0);
        firstCard.AddCard({PlayingCard::Jack});
        
        ExistentialRangeSelector secondCard(1, 1);
        secondCard.AddCard({PlayingCard::Nine});
        
        ExistentialRangeSelector thirdCard(2, 2);
        thirdCard.AddCard({PlayingCard::Ace});
        thirdCard.AddCard({PlayingCard::Ten});
        
        conditions.AddRangeSelectors(targetRange, ownActions);
        time_t timeLimit = 60;
        calc.SetThreads(optimalThreads);
        bool idealDeck = calc.Calculate(timeLimit);
        log(logxx::info, "Conditional test", std::to_string(optimalThreads) + " threads") << "Performance: " << (int)calc.GetLastPerformance() << " decks per second" << logxx::endl;
        if (!idealDeck){
                log(logxx::error, "Conditional test") << "Can't find any I-Ching balanced deck in " << timeLimit << "s, " << logxx::endl;
                return false;
        } else {
                auto &s = log(logxx::debug);
                Medici deck = calc.GetResult();
                PrintDeck(deck, s);
                s << logxx::endl;
                IChing iching;
                iching.LoadFromDeck(deck);
                PrintHexagrams(iching, "TestIChingCalculator", logxx::debug);
        }
        
        log(logxx::info) << "OK" << logxx::endl;
        return true;
}

#define RUN_TEST(function) \
if (!function()) \
        log(logxx::error, #function) << "TEST FAILED" << logxx::endl;

int main() {
        S_LOG("main");
        logxx::GlobalLogLevel(logxx::warning);
        randomSeed = time(nullptr);
        
        RUN_TEST(TestCard);
        RUN_TEST(TestStatisticsMixing);
        RUN_TEST(TestStatisticsReaching);
        TestMixPerformance();
        RUN_TEST(TestMedici);
        TestMediciCollapsePerformance();
        RUN_TEST(TestCardSelector);
        RUN_TEST(TestUniversalRangeSelector);
        RUN_TEST(TestExistentialRangeSelector);
        RUN_TEST(TestComplexRangeSelector);
        RUN_TEST(TestCalculator);
        TestMultithreadPerformance();
        RUN_TEST(TestMultithreadStatistics);
        RUN_TEST(TestMultiThreadCalculator);
        RUN_TEST(TestMobilesAndStationars);
        RUN_TEST(TestIChing);
        RUN_TEST(TestIChingBalanced);
        RUN_TEST(TestIChingCalculator);
        
        return 0;
}
