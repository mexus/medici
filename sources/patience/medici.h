#ifndef MEDICI_H
#define	MEDICI_H

#include <set>
#include "patience.h"

class Medici : public Patience {
public:
        Medici();
        virtual ~Medici();

        virtual bool Collapse();
        std::map<PlayingCard, unsigned int> GetCollapses() const;
        unsigned int GetCollapses(const PlayingCard&) const;
        
        const std::set<PlayingCard>& GetMobiles() const;
        const std::set<PlayingCard>& GetStationars() const;
        
        bool IsCollapsed() const;
private:
        std::map<PlayingCard, unsigned int> collapses;
        std::set<PlayingCard> mobiles, stationars;
        bool collapsed = false;
        
        static bool CollapsingCondition(const std::vector<PlayingCard> &);
        void PartialCollapse(std::vector<PlayingCard> &);
        
protected:
        static logxx::Log cLog;

};

#endif	/* MEDICI_H */

