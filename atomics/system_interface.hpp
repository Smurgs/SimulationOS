// SYSC 4001 - System Call Interface
// Atomic Model - SystemInterface
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#ifndef SYSTEM_INTERFACE_ATOMIC_HPP
#define SYSTEM_INTERFACE_ATOMIC_HPP

#include <math.h> 
#include <assert.h>
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
 * @class Program
*/
template<class TIME, class MSG>
class SystemInterface : public pdevs::atomic<TIME, MSG>{ 
private:

    // PARAMETERS
    string writeSCI; 
    string readSCI;
    string exitSCI;
    string forkSCI;
    string doneIO;
    string processComplete;
    string processFork;
   
    //STATE VARIABLES
    TIME next_internal;
    vector<MSG> out_put;

public:

    /**
    * @constructor 
    */
    explicit SystemInterface() noexcept {   
        writeSCI = string("writeSCI");
        readSCI = string("readSCI");
        exitSCI = string("exitSCI");
        forkSCI = string("forkSCI");
        doneIO = string("doneIO");
        processComplete = string("processComplete");
        processFork = string("processFork");
        out_put.clear();

        next_internal = pdevs::atomic<TIME, MSG>::infinity;
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
        MSG aux;
        for (int i = 0; i < mb.size(); i++) {

            if (mb[i].port == writeSCI) {
                next_internal = 0;
                aux.value = mb[i].value;
                aux.port = "WriteFunction" + to_string(int(mb[i].value));
                out_put.push_back(aux);
            
            }else if (mb[i].port == readSCI) {
                next_internal = 0;
                aux.value = mb[i].value;
                aux.port = "ReadFunction" + to_string(int(mb[i].value));
                out_put.push_back(aux);

            }else if (mb[i].port == doneIO) {
                next_internal = 0;
                aux.value = mb[i].value;
                aux.port = "IOApp" + to_string(int(mb[i].value));
                out_put.push_back(aux);

            }else if (mb[i].port == exitSCI) {
                next_internal = 0;
                aux.value = mb[i].value;
                aux.port = processComplete;
                out_put.push_back(aux);

            }else if (mb[i].port == forkSCI) {
                next_internal = 0;
                aux.value = mb[i].value;
                aux.port = processFork;
                out_put.push_back(aux);
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
