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

#include "../data_structures/message.hpp" 
#include "../data_structures/structures.h" 

using namespace boost::simulation::pdevs;
using namespace boost::simulation;
using namespace std;

char STATE_WAITING = 1;
char STATE_READY = 2;
char STATE_RUNNING = 3;

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
  
    // STATE VARIABLES
    bool processing;
    TIME next_internal;
    vector<MSG> out_put;
    queue<PCB> q;
    PCB table[2];
  
public:

    /**
    * @constructor 
    */
    explicit Kernel() noexcept { 
        startIO = string("startIO");
        doneIO = string("doneIO");
        appIn = string("appIn");
        processing = false;
        TIME next_internal = pdevs::atomic<TIME, MSG>::infinity;
        out_put.clear();
        table[0] = {1, STATE_READY};
        table[1] = {2, STATE_READY};
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
                q.push(table[int(mb[i].value)-1]);                
                next_internal = pdevs::atomic<TIME, MSG>::infinity;
                
                // If not running, get a PCB from queue and start it
                if (!processing){
                    processing = true;
                    PCB temp = q.front();
                    q.pop();
                    aux.port = "ctrlApp" + to_string(temp.PID);
                    aux.value = 1;
                    out_put.push_back(aux);
                    table[temp.PID-1].state = STATE_RUNNING;
                    next_internal = 0;
                }
                
            // Active process enters IO
            }else if (mb[i].port == startIO){
                table[int(mb[i].value)-1].state = STATE_WAITING;
                next_internal = pdevs::atomic<TIME, MSG>::infinity;
                processing = false;
                // Start executing next process if available
                if (!q.empty()){
                    processing = true;
                    PCB temp = q.front();
                    q.pop();
                    aux.port = "ctrlApp" + to_string(temp.PID);
                    aux.value = 1;
                    out_put.push_back(aux);
                    table[temp.PID-1].state = STATE_RUNNING;
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
