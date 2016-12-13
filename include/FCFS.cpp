// SYSC 4001 - Final Project
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#include <queue>
#include "Scheduler.hpp"
#include "FCFS.hpp"

void FCFS::schedule(PCB newPCB) {
    q.push(newPCB);
}

bool FCFS::getNext(PCB &nextPCB) {
    if (!q.empty()) {
        nextPCB = q.front();
        q.pop();
        return true;
    }
    return false;
}
