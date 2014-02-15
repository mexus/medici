#include <algorithm>
#include <limits>

#include "../sources/logxx/logxx.h"
#include "../sources/deck.h"

static logxx::Log cLog("test");
        
inline bool Conditions(const std::vector<PlayingCard> &deck, const PlayingCard &target){
        decltype(deck.begin()) targetBegin = deck.begin() + 19;
        decltype(deck.begin()) targetEnd = deck.begin() + 25;
        
        decltype(deck.begin()) ownActionsBegin = deck.begin() + 3;
        decltype(deck.begin()) ownActionsEnd = deck.begin() + 9;
        
        return 
                deck[0].GetNumber() == PlayingCard::Jack &&
                deck[1].GetNumber() == PlayingCard::Nine &&
                (deck[2].GetNumber() == PlayingCard::Ace || PlayingCard::Ten) &&
                std::find(targetBegin, targetEnd, target) != targetEnd &&
                std::find_if(ownActionsBegin, ownActionsEnd, [](const PlayingCard& p)->bool{
                        return p.GetNumber() == PlayingCard::Ace;
                }) == ownActionsEnd &&
                deck[35].GetNumber() == PlayingCard::Ten;
}

int main() {
        S_LOG("main");
//        log(logxx::info) << "Testing card" << logxx::endl;
//        PlayingCard card;
//        bool reachedEnd(false);
//        int cnt(0);
//        while (!reachedEnd){
//                log(logxx::info) << "Card: " << card.Print(true) << " -> " << card.Print() << logxx::endl;
//                card.Next(reachedEnd);
//                ++cnt;
//        }
//        log(logxx::info) << "Total " << cnt << " cards" << logxx::endl;
//        if (cnt != 36)
//                return 1;
//        
//        Deck initialDeck;
//        log(logxx::info) << "Deck:" << logxx::endl;
//        initialDeck.Print(false);
//        log(logxx::info) << "Deck in abbrevations:" << logxx::endl;
//        initialDeck.Print(true);
//        
//        std::vector<Deck> decks;
//        for (int i = 0; i < 100000; ++i){
//                decks.push_back(initialDeck);
//                if (!initialDeck.Next()){
//                        log(logxx::error) << "Reached an end of sequences!" << logxx::endl;
//                        return 1;
//                }
//        }
//        std::sort(decks.begin(), decks.end());
//        if (std::adjacent_find(decks.begin(), decks.end()) != decks.end()){
//                log(logxx::error) << "Found duplicate" << logxx::endl;
//                return 1;
//        } else
//                log(logxx::info) << "No duplicates found" << logxx::endl;
//        
//        static const long int permutations = 1E8;
//        time_t start(time(nullptr));
//        for (long int i = 0; i < permutations; ++i)
//                initialDeck.Next();
//        log(logxx::info) << permutations << " permutations done in " << time(nullptr) - start << " seconds" << logxx::endl;
        
        //PlayingCard card(std::pair<PlayingCard::Suit, PlayingCard::Number>(PlayingCard::Hearts, PlayingCard::Seven));
//        PlayingCard card{PlayingCard::Seven, PlayingCard::Hearts};
        //Тб 7п Кк Тч 9к Кп 6п 8п 10ч 7к 8к Вп 6ч 6к Вч Тп Дп Вк 7б 7ч 9п 10п 10к Вб Кч 10б 8ч 9ч Кб Дч Дб 8б Дк 6б Тк 9б
//        std::string deskStr = "Вч 9к Тч Вк 7п Xк 9п 7ч 9ч Тб 6ч Дп 8ч Кб Тп Xп Вб 8п 7к 9б 6п Дч Кч Вп Xч Кп 8к Дб Xб 8б 6к Дк 7б Тк 6б Кк";
//        Deck test(deskStr);
//        log(logxx::info) << "Collapsed? " << std::boolalpha << test.Collapse() << logxx::endl;
//        Deck test({
//                {PlayingCard::Ace, PlayingCard::Hearts}
//        });
        //std::vector<PlayingCard> headerDeck({PlayingCard("В"), PlayingCard("9"), PlayingCard("Т")});
        
//        Deck deck;
//        deck.Print(true);

        PlayingCard target("Xч");
        Deck testDeck;
        Deck idealDeck;
        std::vector<PlayingCard> deck = testDeck.GetDeck();
        unsigned int maxCollapses(0);
        for (long long int i = 0; i < std::numeric_limits<decltype(i)>::max(); ++i){
                if (Conditions(deck, target)){
                        testDeck.SetDeck(deck);
                        if (testDeck.Collapse()){
                                auto targetCollapses = testDeck.collapses[target];
                                if (targetCollapses > maxCollapses){
                                        maxCollapses = targetCollapses;
                                        testDeck.Print(true);
                                        log(logxx::info) << target.Print() << " collapses: " << targetCollapses << logxx::endl;
                                        idealDeck = testDeck;
                                }
//                                log(logxx::info) << "Collapses: " << logxx::endl;
//                                for (auto &pair : testDeck.collapses){
//                                        log(logxx::info) << pair.first.Print() << ": " << pair.second << logxx::endl;
//                                }
//                                break;
                        }
                } else {
                        static time_t start(time(nullptr));
                        time_t now(time(nullptr));
                        static const time_t interval = 60;
                        if (now > start + interval){
                                log(logxx::info) << i << logxx::endl;
                                break;
//                                start = now;
//                                Deck test(deck);
//                                test.Print(true);
                        }
                }
//                if (std::find(deck.begin(), deck.end(), target) != end()){
//                        Deck test(deck);
//                        test.Print(true);
//                        break;
//                }
//                if (!std::next_permutation(deck.begin(), deck.end()))
//                        break;
                std::random_shuffle(deck.begin(), deck.end());
        }
        log(logxx::info) << "Selected deck: " << logxx::endl;
        idealDeck.Print(true);
        log(logxx::info) << "Collapses: " << logxx::endl;
        for (auto &pair : idealDeck.collapses){
                log(logxx::info) << pair.first.Print() << ": " << pair.second << logxx::endl;
        }
        
        return 0;
}