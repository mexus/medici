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
                
                void LoadFromDeck(const Medici &);
                std::map<PlayingCard::Suit, Hexagram> hexagrams;
        private:
                static logxx::Log cLog;
                
                static HexagramState GetState(const std::set<PlayingCard>& stationars, const PlayingCard&);
                static HexagramState GetState(const std::set<PlayingCard>& stationars, const PlayingCard& first, const PlayingCard& second);

        };
        
} //namespace dream_hacking 

#endif	/* I_CHING_H */

