// SYSC 4001 - System Call Interface
// Atomic Model - Kernel
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <math.h> 
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <memory>
#include <iomanip>
#include <queue>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>
#include <boost/simulation/pdevs/atomic.hpp>

#include "../include/Scheduler.hpp"
#include "../include/FCFS.hpp"
#include "../data_structures/message.hpp" 
#include "../data_structures/structures.h" 

using namespace boost::simulation::pdevs;
using namespace boost::simulation;
using namespace std;

char STATE_WAITING = 1;
char STATE_READY = 2;
char STATE_RUNNING = 3;
char STATE_TERMINATED = 4;
char STATE_NEW = 5;
char STATE_UNDEFINED = 6;

/**
 * @class Kernel
*/
template<class TIME, class MSG>
class Kernel : public pdevs::atomic<TIME, MSG>{ 
private:

    // PARAMETERS
    string startIO;
    string doneIO;
    string appIn;
    string processComplete;
  
    // STATE VARIABLES
    bool processing;
    TIME next_internal;
    vector<MSG> out_put;
    queue<PCB> q;
    Scheduler *scheduler = new FCFS();
    PCB table[20];
  
public:

    /**
    * @constructor 
    */
    explicit Kernel(int number_of_processes) noexcept { 
        startIO = string("startIO");
        doneIO = string("doneIO");
        appIn = string("appIn");
        processComplete = string("processComplete");
        processing = false;
        TIME next_internal = pdevs::atomic<TIME, MSG>::infinity;
        out_put.clear();
        for (int i = 0; i < number_of_processes; i++){
            table[i] = { char(i+1), STATE_NEW };
        }
    }

    /**
    * @Internal
    */
    void internal() noexcept {  
        next_internal = pdevs::atomic<TIME, MSG>::infinity;
        out_put.clear();
    }

    /**
    * @advance
    */
    TIME advance() const noexcept { 
        return next_internal;
    }

    /**
    *@output
    */
    vector<MSG> out() const noexcept {    
        return out_put;
    }

    /**
    *@external
    */
    void external(const std::vector<MSG>& mb, const TIME& t) noexcept {    
        if (next_internal != pdevs::atomic<TIME, MSG>::infinity) {
            next_internal = next_internal - t;
        }

        MSG aux;
        for (int i = 0; i < mb.size(); i++) {
            // New process submitted by long term scheduler (AKA input file)
            // OR i/o complete
            if (mb[i].port == appIn || mb[i].port == doneIO){
                // Set PCB state to Ready
                table[int(mb[i].value)-1].state = STATE_READY;

                // Enqueue PCB
                scheduler->schedule(table[int(mb[i].value)-1]);                

                // Set next internal
                next_internal = 0;
                
                // If not running, get a PCB from queue and start it
                if (!processing){
                    processing = true;
                    PCB temp;
                    if (scheduler->getNext(temp)){
                        aux.port = "ctrlApp" + to_string(temp.PID);
                        aux.value = temp.PID;
                        out_put.push_back(aux);
                        table[temp.PID-1].state = STATE_RUNNING;
                    }
                }
                
            // Active process enters IO
            }else if (mb[i].port == startIO){
                // Change states
                table[int(mb[i].value)-1].state = STATE_WAITING;
                processing = false;
                
                // Start executing next process if available
                PCB temp;
                if (scheduler->getNext(temp)){
                    processing = true;
                    aux.port = "ctrlApp" + to_string(temp.PID);
                    aux.value = temp.PID;
                    out_put.push_back(aux);
                    table[temp.PID-1].state = STATE_RUNNING;

                    // Set next internal
                    next_internal = 0;
                }

            }else if (mb[i].port == processComplete) {
                // Mark running process as terminated
                table[int(mb[i].value)-1].state = STATE_TERMINATED;
                processing = false;

                // Start executing next process if available
                PCB temp;
                if (scheduler->getNext(temp)){
                    processing = true;
                    aux.port = "ctrlApp" + to_string(temp.PID);
                    aux.value = temp.PID;
                    out_put.push_back(aux);
                    table[temp.PID-1].state = STATE_RUNNING;
                    
                    // Set next internal
                    next_internal = 0;
                }
            }
        }
    }

    /**
    * @confluence 
    */
    virtual void confluence(const std::vector<MSG>& mb, const TIME& t) noexcept {
        external (mb, t);
    }
};

#endif 
