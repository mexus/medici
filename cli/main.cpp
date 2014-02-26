#include <dream_hacking/calculator/calculator.h>
#include <dream_hacking/i-ching/i_ching.h>

logxx::Log cLog("main");



void PrintDeck(const std::shared_ptr<Medici>& deck, std::ostream& s){
        S_LOG("PrintDeck");
        auto cardsDeck = deck->GetDeck();
        auto collapses = deck->GetCollapses();
        auto &stationars = deck->GetStationars();
        
        s << "<";
        for (auto it = cardsDeck.begin(); it != cardsDeck.end(); ++it){
                const PlayingCard &card = *it;
                std::string cardText = card.Print(true);
                if (stationars.find(card) != stationars.end()){
                        s << "[" << cardText << "]";
                } else
                        s << cardText;
                if (collapses.find(card) != collapses.end() && it + 1 != cardsDeck.end())
                        s << "> <";
                else if (it + 1 != cardsDeck.end())
                        s << " ";
        }
        s << ">\n";
        
        auto N = collapses.size();
        decltype(N) i = 0;
        for (auto it= collapses.begin(); it != collapses.end(); ++it){
                s << it->first.Print(true) << ": " << it->second;
                if (++i != N)
                        s << ", ";
        }
}

void PrintIChing(const dream_hacking::IChing& iching){
        S_LOG("Print I-Ching");
        for (auto &suitHex : iching.hexagrams){
                auto &suit = suitHex.first;
                auto &hex = suitHex.second;

                auto &s = log(logxx::info) << PlayingCard::PrintSuit(suit, false) << "\n";
                for (size_t i = 0; i != 6; ++i){
                        auto& state = hex.at(5 - i);
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

dream_hacking::ComplexRangeSelector GenerateConditions(const PlayingCard &targetCard){
        using namespace dream_hacking;
        ExistentialRangeSelector target(19, 24);
        target.AddCard(targetCard);
        
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
        
        ComplexRangeSelector conditions;
        conditions.AddRangeSelectors(target, ownActions, firstCard, secondCard, thirdCard);
        
        return conditions;
}

int main(int argc, char** argv) {
        S_LOG("main");
        using namespace dream_hacking;
//        logxx::GlobalLogLevel(logxx::notice);
        logxx::GlobalLogLevel(logxx::warning);
        
        time_t timeLimit = 15;
        size_t threads = 1;
        PlayingCard targetCard;
        if (argc > 1){
                try {
                        targetCard = PlayingCard(argv[1]);
                } catch (const std::logic_error& e){
                        log(logxx::error) << e.what() << logxx::endl;
                        return 1;
                }
                log(logxx::info) << "Setting {" << targetCard.Print() << "} as a target card" << logxx::endl;
        } else {
                log(logxx::error) << "At least card should be specified" << logxx::endl;
                return 1;
        }
        if (argc > 2) {
                timeLimit = std::atoi(argv[2]);
                log(logxx::info) << "Set time limit to " << timeLimit << " seconds" << logxx::endl;
        }
        if (argc > 3){
                threads = std::atoi(argv[3]);
                log(logxx::info) << "Running calculation in " << threads << " threads" << logxx::endl;
        }
                
        
        auto conditions = GenerateConditions(targetCard);
        Calculator calc;
        calc.SetConditions(conditions);
        calc.SetIChingTestBalance(true);
        
        bool res = calc.Calculate(timeLimit, threads, [&targetCard](const Medici & d) -> unsigned int {
                return d.GetCollapses(targetCard);
        });
        
        log(logxx::info) << "Performance: " << (unsigned long long int)calc.GetLastPerformance() << " decks per second" << logxx::endl;
        
        if (!res){
                log(logxx::error) << "No sequences found" << logxx::endl;
                return 1;
        } else {
                auto &s = log(logxx::info);
                std::shared_ptr<Medici> deck = calc.GetResult()[0];
                deck->Collapse(true);
                PrintDeck(deck, s);
                s << logxx::endl;
                IChing iching;
                iching.LoadFromDeck(deck);
                PrintIChing(iching);
                return 0;
        }
}

