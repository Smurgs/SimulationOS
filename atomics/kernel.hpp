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
#include "../include/PriorityQueue.hpp"
#include "memoryModel.hpp"
#include "../data_structures/message.hpp" 
#include "../data_structures/structures.h" 

#define SCHEDULER_FCFS      1
#define SCHEDULER_PRIORITY  2

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
    string processFork;
    string requestMemory;
    string releaseMemory;
    string responseMemory;
    Scheduler *scheduler;
  
    // STATE VARIABLES
    bool processing;
    TIME next_internal;
    vector<MSG> out_put;
    queue<PCB> q;
    PCB table[20];

    /*
     * Returns index of first free PCB, or -1 on failure
     */
    int get_free_pcb(){
        int i = 0;
        while (!(table[i].state==STATE_TERMINATED || table[i].state==STATE_UNDEFINED)){
            i++;
            if(i >= 20){
                return -1;
            }
        }
        return i;
    }
  
public:

    /**
    * @constructor 
    */
    explicit Kernel(int number_of_processes, int scheduler_option) noexcept { 
        startIO = string("startIO");
        doneIO = string("doneIO");
        appIn = string("appIn");
        processComplete = string("processComplete");
        processFork = string("processFork");
        requestMemory = string("requestMemory");
        releaseMemory = string("releaseMemory");
        responseMemory = string("responseMemory");
        
        // Set scheduler
        if (scheduler_option == SCHEDULER_FCFS){
            scheduler = new FCFS();
        }else if (scheduler_option == SCHEDULER_PRIORITY){
            scheduler = new PriorityQueue();
        }

        processing = false;
        TIME next_internal = pdevs::atomic<TIME, MSG>::infinity;
        out_put.clear();

        // Init PCB table
        for (int i = 0; i < 20; i++){
            table[i] = { char(i+1), STATE_UNDEFINED };
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
            if (mb[i].port == appIn){
                // Set PCB data
                table[int(mb[i].value)-1].type = mb[i].value2;
                table[int(mb[i].value)-1].priority = mb[i].value3;

                // Request memory
                aux.port = requestMemory;
                aux.value = mb[i].value;        // PID
                aux.value2 = mb[i].value4;      // Memory block size
                out_put.push_back(aux);

            }else if (mb[i].port == responseMemory){
                // Read memory request response
                if (mb[i].value2 == MEMORY_NOT_OK){
                    cout << "Memory not available. Failed to start process" << endl;
                    table[int(mb[i].value)-1].state = STATE_UNDEFINED;
                    continue;
                }

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
                        aux.value2 = temp.type;
                        out_put.push_back(aux);
                        table[temp.PID-1].state = STATE_RUNNING;
                    }
                }

            // IO complete
            }else if (mb[i].port == doneIO){
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
                        aux.value2 = temp.type;
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
                    aux.value2 = temp.type;
                    out_put.push_back(aux);
                    table[temp.PID-1].state = STATE_RUNNING;

                    // Set next internal
                    next_internal = 0;
                }

            }else if (mb[i].port == processComplete) {
                // Mark running process as terminated
                table[int(mb[i].value)-1].state = STATE_TERMINATED;
                processing = false;

                // Send message to MemoryModel to release memory
                MSG memMsg;
                memMsg.port = releaseMemory;
                memMsg.value = mb[i].value;
                out_put.push_back(memMsg);

                // Start executing next process if available
                PCB temp;
                if (scheduler->getNext(temp)){
                    processing = true;
                    aux.port = "ctrlApp" + to_string(temp.PID);
                    aux.value = temp.PID;
                    aux.value2 = temp.type;
                    out_put.push_back(aux);
                    table[temp.PID-1].state = STATE_RUNNING;
                    
                    // Set next internal
                    next_internal = 0;
                }

            }else if (mb[i].port == processFork) {
                //Find free PCB
                int index = get_free_pcb();

                if (index != -1){
                    // Prepare PCB for queue
                    table[index].type = mb[i].value2;
                    table[index].priority = mb[i].value3;

                    // Request memory
                    aux.port = requestMemory;
                    aux.value = table[index].PID;
                    aux.value2 = mb[i].value4;      // Memory block size
                    out_put.push_back(aux);

                    next_internal = 0;

                }else{
                    cout << "No PCB available. Failed to fork." << endl;
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
