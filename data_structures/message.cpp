#include <math.h> 
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "message.hpp"

/***************************************************/
/************* Output stream ************************/
/***************************************************/

ostream& operator<<(ostream& os, const Message_t& msg) {
  os << msg.port << " " << msg.value << " " << msg.value2 << " " << msg.value3 << " " << msg.value4;
  return os;
}

/***************************************************/
/************* Input stream ************************/
/***************************************************/

istream& operator>> (istream& is, Message_t& msg) {
  is >> msg.port;
  is >> msg.value;
  is >> msg.value2;
  is >> msg.value3;
  is >> msg.value4;
  return is;
}
