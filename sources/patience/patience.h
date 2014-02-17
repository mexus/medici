#ifndef PATIENCE_H
#define	PATIENCE_H

#include "../deck/deck.h"

class Patience : public Deck {
public:
        Patience();
        virtual ~Patience();
        
        virtual bool Collapse() = 0;
protected:
        static logxx::Log cLog;

};

#endif	/* PATIENCE_H */

