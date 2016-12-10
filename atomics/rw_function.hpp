// SYSC 4001 - System Call Interface
// Atomic Model - Read/Write Function
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#ifndef RW_FUNCTION_ATOMIC_HPP
#define RW_FUNCTION_ATOMIC_HPP

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
 * @class RWFucntion
*/

template<class TIME, class MSG>
class RWFunction : public pdevs::atomic<TIME, MSG>{ 
private:

    // PARAMETERS
    string ID;
    string doneIO;
    string startIO;
    int functionNumber;
  
    //STATE VARIABLES
    bool active;
    TIME next_internal;
  

public:

    /**
    * @constructor 
    */
    explicit RWFunction(string id) noexcept {
        ID = id; 
        doneIO = string("doneIO");
        startIO = string("startIO");
        active = false;
        functionNumber = 1;
        next_internal = pdevs::atomic<TIME, MSG>::infinity;
    }

    /**
    * @Internal
    */
    void internal() noexcept {  
        active = !active;
        if (active) {
            next_internal = 10;
        }else{
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
        out_put.clear();
        if (active){
            aux.port = doneIO;
        }else {
            aux.port = startIO;
        }
        aux.value = functionNumber;
        out_put.push_back(aux);
        return out_put;
    }

  /**
   *@external
   */
  void external(const std::vector<MSG>& mb, const TIME& t) noexcept {    
      if (next_internal != pdevs::atomic<TIME, MSG>::infinity){
          next_internal = next_internal - t;
      }

      for (int i = 0; i < mb.size(); i++) {
          MSG aux = mb[i];
          if (aux.port == ID) {
              next_internal = 0;
              functionNumber = aux.value;
          }
      }
  }

  /**
   * @confluence 
  */
  virtual void confluence(const std::vector<MSG>& mb, const TIME& t) noexcept {
      external(mb, t);
  }
};

#endif 
