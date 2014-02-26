#ifndef I_CHING_H
#define	I_CHING_H

#include "../../patience/medici.h"

#include <array>

enum HexagramState{
        SolidLine,
        OpenedLine,
        SolidLineStrong,
        SolidLineWeak,
        OpenedLineStrong,
        OpenedLineWeak
};
typedef std::array<HexagramState, 6> Hexagram;

namespace dream_hacking {

        class IChing {
        public:
                IChing();
                virtual ~IChing();
                
                IChing& LoadFromDeck(const Medici &);
                IChing& LoadFromDeck(const std::shared_ptr<Medici> &);
                std::map<PlayingCard::Suit, Hexagram> hexagrams;
                
                bool IsBalanced() const;
                bool CheckHexagram(PlayingCard::Suit, const Hexagram &) const;
        private:
                static logxx::Log cLog;
                bool strongBalance = false; //Not using as for now
                
                static HexagramState GetState(const std::set<PlayingCard>& stationars, const PlayingCard&);
                static HexagramState GetState(const std::set<PlayingCard>& stationars, const PlayingCard& first, const PlayingCard& second);

        };
        
} //namespace dream_hacking 

#endif	/* I_CHING_H */

