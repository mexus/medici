#include "i_ching.h"
#include <algorithm>

namespace dream_hacking {
        
        logxx::Log IChing::cLog("I-Ching");

        IChing::IChing() {
        }

        IChing::~IChing() {
        }

        void IChing::LoadFromDeck(const Medici& d) {
                auto &stationars = d.GetStationars();
                
                for (auto &suit : std::set<PlayingCard::Suit>{PlayingCard::Clubs, PlayingCard::Hearts, PlayingCard::Diamonds, PlayingCard::Spades}){
                        auto &hexagram = hexagrams[suit];
                        hexagram[6 - 1] = GetState(stationars, {PlayingCard::Ace, suit});
                        hexagram[5 - 1] = GetState(stationars, {PlayingCard::King, suit});
                        hexagram[4 - 1] = GetState(stationars, {PlayingCard::Queen, suit}, {PlayingCard::Jack, suit});
                        hexagram[3 - 1] = GetState(stationars, {PlayingCard::Ten, suit});
                        hexagram[2 - 1] = GetState(stationars, {PlayingCard::Six, suit}, {PlayingCard::Eight, suit});
                        hexagram[1 - 1] = GetState(stationars, {PlayingCard::Nine, suit}, {PlayingCard::Seven, suit});
                }
        }

        HexagramState IChing::GetState(const std::set<PlayingCard>& stationars, const PlayingCard& card) {
                S_LOG("GetState");
                auto end = stationars.end();
                bool stationar = std::find(stationars.begin(), end, card) != end;
                HexagramState state = stationar ? OpenedLine : SolidLine;
                
                auto &s = log(logxx::debug);
                if (s.good()){
                        if (stationar)
                                s << "[" << card.Print(true) << "]";
                        else 
                                s << "(" << card.Print(true) << ")";
                        s << " ";
                        if (state == SolidLine)
                                s << "======";
                        else
                                s << "==__==";
                        s << logxx::endl;
                }
                
                return state;
        }

        HexagramState IChing::GetState(const std::set<PlayingCard>& stationars, const PlayingCard& first, const PlayingCard& second) {
                S_LOG("GetState");
                auto begin = stationars.begin();
                auto end = stationars.end();
                bool firstStationar = (std::find(begin, end, first) != end);
                bool secondStationar = (std::find(begin, end, second) != end);
                
                HexagramState state;
                
                if (firstStationar && secondStationar)
                        state = OpenedLineWeak;
                else if (!firstStationar && !secondStationar)
                        state = SolidLineStrong;
                else if (!firstStationar && secondStationar)
                        state = SolidLineWeak;
                //else if (firstStationar && !secondStationar)
                else
                        state = OpenedLineStrong;
                
                auto &s = log(logxx::debug);
                if (s.good()){
                        if (firstStationar)
                                s << "[" << first.Print(true) << "]";
                        else 
                                s << "(" << first.Print(true) << ")";
                        s << " ";
                        if (secondStationar)
                                s << "[" << second.Print(true) << "]";
                        else 
                                s << "(" << second.Print(true) << ")";
                        s << " ";
                        if (state == SolidLine || state == SolidLineStrong || state == SolidLineWeak)
                                s << "======";
                        else
                                s << "==__==";
                        s << logxx::endl;
                }
                
                return state;
        }

} //namespace dream_hacking 
