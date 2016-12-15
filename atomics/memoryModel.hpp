// SYSC 4001 - OS Sim
// Atomic Model - Memory Model
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#ifndef MEMORY_MODEL_HPP
#define MEMORY_MODEL_HPP

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

#define MEMORY_OK       1
#define MEMORY_NOT_OK   2

using namespace boost::simulation::pdevs;
using namespace boost::simulation;
using namespace std;

// Struct will be used to keep track of assigned mem blocks
typedef struct MemoryRecord {
    int PID;
    int start;
    int size;
} MemoryRecord;


/**
 * @class Memory Model
*/
template<class TIME, class MSG>
class MemoryModel : public pdevs::atomic<TIME, MSG>{ 
private:

    // PARAMETERS
    string requestMemory;
    string releaseMemory;
    string responseMemory;
    int memorySize;
    char *memory;
    MemoryRecord memoryTable[20];
  
    // STATE VARIABLES
    TIME next_internal;
    vector<MSG> out_put;

  
public:

    /**
    * @constructor 
    */
    explicit MemoryModel(int total_memory) noexcept { 
        requestMemory = string("requestMemory");
        releaseMemory = string("releaseMemory");
        responseMemory = string("responseMemory");
        memorySize = total_memory;

        // Create dynamic array 
        memory = new char [memorySize];
        for (int i = 0; i < memorySize; i++){
            *(memory + i) = 0;
        }

        // Create memory table
        for (int i = 0; i < 20; i++){
            memoryTable[i].PID = i + 1;
            memoryTable[i].start = -1;
            memoryTable[i].size = -1;
        }
        TIME next_internal = pdevs::atomic<TIME, MSG>::infinity;
        out_put.clear();
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
            // Request block of memory 
            if (mb[i].port == requestMemory){
                int PID = mb[i].value;
                int block_size = mb[i].value2;

                bool memoryFreeFlag = true;
                // If memory request was too large
                if ((memorySize - block_size) <= 0) {
                    memoryFreeFlag = false;
                }

                // Search for hole of sufficient size
                for (int j = 0; j <= (memorySize - block_size); j++){
                    memoryFreeFlag = true;
                    for (int k = 0; k < block_size; k++){
                        if (*(memory + j + k) != 0) {
                            memoryFreeFlag = false;
                            break;
                        }
                    }
                    
                    if (memoryFreeFlag){
                        // Record memory as taken in memory table
                        memoryTable[PID-1].start = j;
                        memoryTable[PID-1].size = block_size;

                        // Mark the memory as taken in the array
                        for (int k = 0; k < block_size; k++){
                            *(memory + j + k) = 1;
                        }

                        break;
                    }
                }

                
                // Prep output message
                next_internal = 0;
                aux.port = responseMemory;
                aux.value = PID;
                if (memoryFreeFlag){
                    aux.value2 = MEMORY_OK;
                }else{
                    aux.value2 = MEMORY_NOT_OK;
                }
                out_put.push_back(aux);

            // Release block of memory
            }else if (mb[i].port == releaseMemory) {
                int PID = mb[i].value;
                int start = memoryTable[PID-1].start;
                int size = memoryTable[PID-1].size;

                // Release mem
                if (memoryTable[PID-1].start != -1){
                    for (int j = start; j < start + size; j++){
                        *(memory + j) = 0;
                    }

                    //Update memory table
                    memoryTable[PID-1].start = -1;
                    memoryTable[PID-1].size = -1;
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
