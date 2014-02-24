#ifndef MEDICI_H
#define	MEDICI_H

#include "patience.h"

class Medici : public Patience {
public:
        Medici();
        virtual ~Medici();

        virtual bool Collapse();
        std::map<PlayingCard, unsigned int> GetCollapses() const;
        unsigned int GetCollapses(const PlayingCard&) const;
private:
        std::map<PlayingCard, unsigned int> collapses;
        
        static bool CollapsingCondition(const std::vector<PlayingCard> &);
        void PartialCollapse(std::vector<PlayingCard> &);
        
protected:
        static logxx::Log cLog;

};

#endif	/* MEDICI_H */

