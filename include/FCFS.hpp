// SYSC 4001 - Final project
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#ifndef FCFS_SCHEDULER
#define FCFS_SCHEDULER

#include <queue>
#include "Scheduler.hpp"

class FCFS : public Scheduler {
    private: 
        std::queue<PCB> q;
    public:
        
        void schedule (PCB newPCB);
        bool getNext (PCB &nextPCB);
};

#endif
