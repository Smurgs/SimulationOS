// SYSC 4001 - Final Project
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#ifndef SCHEDULER_INTERFACE
#define SCHEDULER_INTERFACE

#include "../data_structures/structures.h"

class Scheduler {
    public:
        virtual void schedule(PCB newPCB) = 0;
        virtual bool getNext(PCB &nextPCB) = 0;
};

#endif
