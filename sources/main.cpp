#include "dream_hacking/calculator.h"
#include "dream_hacking/i-ching/i_ching.h"

logxx::Log cLog("main");



void PrintDeck(const std::shared_ptr<Medici> deck, std::ostream& s){
        S_LOG("PrintDeck");
        auto cardsDeck = deck->GetDeck();
        auto collapses = deck->GetCollapses();
//        auto &mobiles = deck->GetMobiles();
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
        
        //std::map<PlayingCard, unsigned int>
        for (auto it= collapses.begin(); it != collapses.end(); ++it){
                s << it->first.Print(true) << ": " << it->second << "\n";
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
        logxx::GlobalLogLevel(logxx::warning);
        
        PlayingCard targetCard{PlayingCard::Ace, PlayingCard::Hearts};
        auto conditions = GenerateConditions(targetCard);
        Calculator calc(conditions);
        
        time_t timeLimit = 15;
        if (argc > 1)
                timeLimit = std::atoi(argv[1]);
        
        if (argc > 2)
                calc.SetThreads(std::atoi(argv[2]));
        
        std::shared_ptr<Medici> deck = calc.Calculate(timeLimit, [&targetCard](const std::shared_ptr<Medici>& d) -> unsigned int {
                return d->GetCollapses(targetCard);
        });
        
        log(logxx::info) << "Performance: " << (unsigned long long int)calc.GetLastPerformance() << " decks per second" << logxx::endl;
        
        if (!deck){
                log(logxx::error) << "No sequences found" << logxx::endl;
                return 1;
        } else {
                auto &s = log(logxx::info);
                deck->Collapse(true);
                PrintDeck(deck, s);
                s << logxx::endl;
                IChing iching;
                iching.LoadFromDeck(*deck.get());
                PrintIChing(iching);
                return 0;
        }
}

