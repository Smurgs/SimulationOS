// SYSC 4001 - Final project
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE

#include "queues.h"
#include "Scheduler.hpp"

class PriorityQueue : public Scheduler {
    private: 
        node *queue;
    public:
        PriorityQueue();        
        void schedule (PCB newPCB);
        bool getNext (PCB &nextPCB);
};

#endif
