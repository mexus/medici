#include "i_ching.h"
#include <algorithm>

namespace dream_hacking {
        
        logxx::Log IChing::cLog("I-Ching");

        IChing::IChing() {
        }

        IChing::~IChing() {
        }

        IChing& IChing::LoadFromDeck(const Medici& d) {
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
                
                return *this;
        }

        IChing& IChing::LoadFromDeck(const std::shared_ptr<Medici> &deck){
                if (!deck){
                        throw std::logic_error("Can't load IChing from a nullptr");
                }
                return LoadFromDeck(*deck.get());
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
        
        bool IsYin(HexagramState state){
                return state == OpenedLine || state == OpenedLineStrong || state == OpenedLineWeak;
        }
        
        bool IChing::CheckHexagram(PlayingCard::Suit s, const Hexagram &l) const{
                const Hexagram &r = hexagrams.at(s);
                for (int i = 0; i < 6; ++i){
                        bool leftSolid = (l[i] == SolidLine || l[i] == SolidLineStrong || l[i] == SolidLineWeak);
                        bool rightSolid = (r[i] == SolidLine || r[i] == SolidLineStrong || r[i] == SolidLineWeak);
                        if (leftSolid != rightSolid)
                                return false;
                }
                return true;
        }

        bool IChing::IsBalanced() const {
                S_LOG("IsBalanced");
                for (int i = 0; i < 6; ++i){
                        unsigned short int yins(0), yangs(0);
                        for (auto &pair : hexagrams){
                                if (IsYin(pair.second[i]))
                                        ++yins;
                                else
                                        ++yangs;
                        }
                        if (yins != yangs){
                                log(logxx::debug, i) << "Line is not balanced: " << yangs << " yangs and "
                                        << yins << " yins" << logxx::endl;
                                return false;
                        }
                }
                return true;
        }

        Hexagram IChing::HexagramFromTrigrams(const Trigram& lower, const Trigram& upper) {
                Hexagram res;
                for (size_t i = 0; i != 3; ++i){
                        res[i] = lower[i];
                        res[i + 3] = upper[i];
                }
                return res;
        }

        void IChing::GenerateHexNumbers(std::map<Hexagram, unsigned short>& hexNumber, std::map<unsigned short, Hexagram>& numberHex) {
                std::vector<Trigram> trigrams{
                        {SolidLine, SolidLine, SolidLine},
                        {SolidLine, OpenedLine, OpenedLine},
                        {OpenedLine, SolidLine, OpenedLine},
                        {OpenedLine, OpenedLine, SolidLine},
                        {OpenedLine, OpenedLine, OpenedLine},
                        {OpenedLine, SolidLine, SolidLine},
                        {SolidLine, OpenedLine, SolidLine},
                        {SolidLine, SolidLine, OpenedLine}
                };
                std::vector<unsigned short> numbers{
                         1, 34,  5, 26, 11,  9, 14, 43,
                        25, 51,  3, 27, 24, 42, 21, 17,
                         6, 40, 29,  4,  7, 59, 64, 47,
                        33, 62, 39, 52, 15, 53, 56, 31,
                        12, 16,  8, 23,  2, 20, 35, 45,
                        44, 32, 48, 18, 46, 57, 50, 28,
                        13, 55, 63, 22, 36, 37, 30, 49,
                        10, 54, 60, 41, 19, 61, 38, 58
                };
                
                size_t cnt(0);
                for (Trigram const& lower: trigrams){
                        for (Trigram const & upper : trigrams){
                                Hexagram hex = HexagramFromTrigrams(lower, upper);
                                unsigned short number = numbers[cnt++];
                                hexNumber[hex] = number;
                                numberHex[number] = hex;
                        }
                }
        }

} //namespace dream_hacking 
