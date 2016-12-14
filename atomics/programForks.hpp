// SYSC 4001 - System Call Interface
// Atomic Model - Program Fork
// Nomvember 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#ifndef PROGRAM_FORK_HPP
#define PROGRAM_FORK_HPP

#include <math.h> 
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>
#include <boost/simulation/pdevs/atomic.hpp>

#include "../data_structures/message.hpp" 

using namespace boost::simulation::pdevs;
using namespace boost::simulation;
using namespace std;


/**
 * @class Program Fork
*/
template<class TIME, class MSG>
class ProgramFork : public pdevs::atomic<TIME, MSG>{ 
private:

    // PARAMETERS
    int PID;
    string doneIO;
    string writeSCI;
    string readSCI;
    string ctrlIn;
    string exitSCI;
    string forkSCI;
  
    // STATE VARIABLES
    bool executing;
    TIME next_internal;
    int fork_count;
  
public:

    /**
    * @constructor 
    */
    explicit ProgramFork(int pid) noexcept { 
        PID = pid;
        doneIO = "IOApp" + to_string(PID);
        ctrlIn = "ctrlApp" + to_string(PID);
        writeSCI = "writeSCI";
        readSCI = "readSCI";
        exitSCI = "exitSCI";
        forkSCI = "forkSCI";

        executing = false;
        srand (time(NULL));
        next_internal = pdevs::atomic<TIME, MSG>::infinity;
        fork_count = 0;
    }

    /**
    * @Internal
    */
    void internal() noexcept {  
        next_internal = rand() % 100 + 1;
        fork_count++;

        if (fork_count > 15){
            next_internal = pdevs::atomic<TIME, MSG>::infinity;
        }
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
        vector<MSG> out_put;
        MSG aux;

        if (fork_count < 15){
            aux.port = forkSCI;
        }else {
            aux.port = exitSCI;
        }

        aux.value = PID;
        out_put.push_back(aux);
        return out_put;
    }

    /**
    *@external
    */
    void external(const std::vector<MSG>& mb, const TIME& t) noexcept {    
        if (next_internal != pdevs::atomic<TIME, MSG>::infinity) {
            next_internal = next_internal - t;
        }

        for (int i = 0; i < mb.size(); i++) {
            if (mb[i].port == ctrlIn) {
                executing = true;
                next_internal = rand() % 100 + 1;
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
