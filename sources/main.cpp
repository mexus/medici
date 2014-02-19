#include "dream_hacking/calculator.h"

logxx::Log cLog("main");



void PrintDeck(const std::shared_ptr<Medici> deck, std::ostream& s){
        S_LOG("PrintDeck");
        auto cardsDeck = deck->GetDeck();
        auto collapses = deck->GetCollapses();
        
        s << "<";
        for (auto it = cardsDeck.begin(); it != cardsDeck.end(); ++it){
                const PlayingCard &card = *it;
                s << card.Print(true);
                if (collapses.find(card) != collapses.end() && it + 1 != cardsDeck.end())
                        s << "> <";
                else if (it + 1 != cardsDeck.end())
                        s << " ";
        }
        s << ">\n";
        
        //std::map<PlayingCard, unsigned int>
        for (auto it= collapses.begin(); it != collapses.end(); ++it){
                s << it->first.Print(true) << ": " << it->second << "\n";
        }
}

int main(int argc, char** argv) {
        S_LOG("main");
        using namespace dream_hacking;
        logxx::GlobalLogLevel(logxx::warning);
        
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
        
        time_t timeLimit = 15;
        if (argc > 1)
                timeLimit = std::atoi(argv[1]);
        
        if (argc > 2)
                calc.SetThreads(std::atoi(argv[2]));
        
        auto deck = calc.Calculate(timeLimit, [&targetCard](const std::shared_ptr<Medici>& d) -> unsigned int {
                return d->GetCollapses(targetCard);
        });
        
        log(logxx::info) << "Performance: " << (unsigned long long int)calc.GetLastPerformance() << " decks per second" << logxx::endl;
        
        if (!deck){
                log(logxx::error) << "No sequences found" << logxx::endl;
                return 1;
        } else {
                auto &s = log(logxx::info);
                PrintDeck(deck, s);
                s << logxx::endl;
                return 0;
        }
}

