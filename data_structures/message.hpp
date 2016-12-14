#ifndef BOOST_SIMULATION_MESSAGE_HPP
#define BOOST_SIMULATION_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>

#define PROGRAM_IO      1
#define PROGRAM_FORK    2

using namespace std;

/*******************************************/
/**************** Message_t ****************/
/*******************************************/
struct Message_t{

  Message_t(){}
  Message_t(string i_port, float i_value, int i_value2)
   :port(i_port), value(i_value), value2(i_value2){}

  string  port;
  float   value;
  int value2;

  void clear() {
   Message_t(); 
  }
};

istream& operator>> (istream& is, Message_t& msg);

ostream& operator<<(ostream& os, const Message_t& msg);


#endif // BOOST_SIMULATION_MESSAGE_HPP
