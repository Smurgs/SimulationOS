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
#include <boost/simulation/pdevs/atomic.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/simulation.hpp>

#include "../../vendor/britime.hpp"
#include "../../vendor/input_event_stream.hpp"
#include "../../data_structures/message.hpp"
// INCLUDE THE HEADERS OF YOUR ATOMICS USING THE RELATIVE DIRECTION TO YOUR ACTUAL FOLDER
/*Example*/
//#include "../../atomics/filterPort.hpp"



using namespace std;
using namespace boost::simulation;
using namespace boost::simulation::pdevs;
using namespace boost::simulation::pdevs::basic_models;


using hclock = chrono::high_resolution_clock;
using Time =BRITime; //IF YOU ARE USING BRITIME AS TIME CLASS DO NOT CHANGE ANYTHING. ELSE, BRITime IS YOUR TIME TYPE
using Message = Message_t; //IF YOU ARE USING MESSAGE_T AS MESSAGE CLASS DO NOT CHANGE ANYTHING. ELSE, Message_t IS YOUR MSG TYPE


int main(int argc, char ** argv) {

  if (argc < 2) {
   cout << "you are using this program with wrong parameters. Te program should be invoked as follow:";
   cout << argv[0] << " path to the input file " << endl;
   return 1;  
  }
  
  string test_file = argv[1];
  ifstream file(test_file);
  string str;
  string file_contents;
  while (getline(file, str)){
    file_contents += str;
    file_contents.push_back('\n');
  }  
    
  string m_input;
  m_input = file_contents;

  cout << "model input:" << endl;
  cout << m_input << endl;

  shared_ptr<istringstream> pointer_iss{ new istringstream{} };
  pointer_iss->str(m_input);

  auto input_test_generator = make_atomic_ptr<input_event_stream<Time, Message, Time, Message>, shared_ptr<istringstream>, Time>(pointer_iss, Time(0),
                [](const string& s, Time& t_next, Message& m_next)->void{ //parsing function
        
        string aux;
        m_next.clear();          
        istringstream ss;
                    
        ss.str(s);

        //CONVERT THE INPUT FILE TO MESSAGES.
	//IMPLEMENT HERE THE PARSING FUNCTION JUST FOR ONE LINE.
        //THE INPUT FILE MUST BE A TXT. THE FILE CANNOT HAVE BLANK LINE AT THE END
        //TIME IN THE TXT FILE HAVE TO BE GREATER OR EQUAL THAN ONE THE PREVIOUS LINE.
        
        /*Example*/          

        //ss >> t_next; //takes the two fist numbers in the line. The time is defined as a numerator and denominator        
        //ss >> m_next.port;
        //ss >> m_next.value;
        
        
        string thrash;
        ss >> thrash;
        if ( 0 != thrash.size()) throw exception();         
  });
  
  
  // DEFINE ALL YOUR ATOMIC MODELS
  auto AtomicModelNameInstance = make_atomic_ptr< AtomicName<Time,Message>, parametersType>(parametersValue); 
  //CHANGE AtomicModelNameInstance TO THE NAME YOU GIVE TO THE INSTANCE OF YOUR ATOMIC MODEL
  //CHANGE AtomicName TO THE CLASS NAME OF YOUR ATOMIC MODEL. IT WAS DEFINED IN THE FOLDER atomics IN A .hpp FILE
  //CHANGE parametersType TO THE PARAMETERS TYPE OF YOUR ATOMIC MODEL CLASS (SEPARATE THEM BY COMMA). IF THERE ARE NO PARAMETERS IN THE CLASE, DELETE ", parametersType"
  //CHANGE parametersValue TO THE VALUE OF PARAMETERS TYPE TAKES. IF NO PARAMETERS, DELETE "parametersValue"

  /*Examples*/
  // auto Subnet1 = make_atomic_ptr<Subnet<Time,Message>, string, int>(string("net1"), 1);
  // auto PersonD = make_atomic_ptr<Person<Time,Message>>();
  // auto PersonC = make_atomic_ptr<Person<Time,Message>>();
  

  // DEFINE ALL YOUR COUPLED MODELS. EACH COUPLED MODEL IS LIKE AN ATOMIC WHEN YOU DEFINE A HIHER LEVEL.  THE NAME OF THE HIGHEST LEVEL MODEL IS CALL TOP.

  shared_ptr<flattened_coupled<Time, Message>> TOP(new flattened_coupled<Time, Message>( //CHANGE TOP TO THE NAME OF YOUR COUPLED MODEL
  {}, // NAMES OF THE MODELS IN THE COUPLED MODEL
  {}, // LIST OF EIC SEPAREATED BY COMMA (",")
  {}, // LIST IC DIFINED AS "{inputmodel, outputmodel}" AND SEPARATED BY COMMAS (",")
  {}// LIST OF EOC SEPAREATED BY COMMA (",")
  ));  
  /* Example*/
  // shared_ptr<flattened_coupled<Time, Message>> NET(new flattened_coupled<Time, Message>(
  // {input_test_generator, Subnet1}, // NAMES OF THE MODELS IN THE COUPLED MODEL
  // {}, // LIST OF EIC SEPAREATED BY ,
  // {{input_test_generator, Subnet1}}, // LIST IC DIFINED AS {inputmodel, outputmodel} AND SEPARATED BY COMMAS
  // {Subnet1}// LIST OF EOC
  // ));  
  
  // shared_ptr<flattened_coupled<Time, Message>> TOP(new flattened_coupled<Time, Message>(
  // {NET, PersonD, Person C}, // NAMES OF THE MODELS IN THE COUPLED MODEL
  // {}, // LIST OF EIC SEPAREATED BY ,
  // {{PersonD, NET}, {NET, PersonC}}, // LIST IC DIFINED AS {inputmodel, outputmodel} AND SEPARATED BY COMMAS
  // {PersonD, Person C}// LIST OF EOC
  // ));  
  
 
    cout << "Preparing runner" << endl;
    Time initial_time = Time(0);
    ofstream out_data("atomic_test_output.txt");  // atomic_test_output.txt IS THE NAME OF THE OUTPUT FILE WHERE YOU WANT TO STORE THE OUTPUT
    runner<Time, Message> r(TOP, initial_time, out_data, [](ostream& os, Message m){ os << m;});
    // IF YOUR TOP MODEL NAME IS NOT "TOP", CHANGE "TOP" BY YOUR TOP MODEL NAME
    Time end_time = Time(1000000); //FINISHING TIME OF THE SIMULATION. CHANGE 1000000 TO THE AMOUNT YOU WANT.

    cout << "Starting simulation until time: " << end_time << "seconds" << endl;
    auto start = hclock::now();
    end_time = r.runUntil(end_time);
    auto elapsed = chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>> (hclock::now() - start).count();     
    cout << "Finished simulation with time: " << end_time << "sec" << endl;
    cout << "Simulation took: " << elapsed << "sec" << endl;
    return 0;
}



    

   
