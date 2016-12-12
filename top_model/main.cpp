// SYSC 4001
// Final Project
// Stefan Murga
// Nwakpadolu Soluzochukwu John

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

#include "../vendor/britime.hpp"
#include "../vendor/input_event_stream.hpp"
#include "../data_structures/message.hpp"
#include "../atomics/program.hpp"
#include "../atomics/rw_function.hpp"
#include "../atomics/system_interface.hpp"
#include "../atomics/kernel.hpp"


using namespace std;
using namespace boost::simulation;
using namespace boost::simulation::pdevs;
using namespace boost::simulation::pdevs::basic_models;

using hclock = chrono::high_resolution_clock;
using Time =BRITime; 
using Message = Message_t; 


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

        ss >> t_next; //takes the two fist numbers in the line. The time is defined as a numerator and denominator        
        ss >> m_next.port;
        ss >> m_next.value;
        
        string thrash;
        ss >> thrash;
        if ( 0 != thrash.size()) throw exception();         
    });

    cout << "Creating atomic models" << endl;
  
    // DEFINE ATOMIC MODELS
    auto kernel = make_atomic_ptr< Kernel<Time,Message>>();
    auto program1 = make_atomic_ptr< Program<Time,Message>, int>(1); 
    auto program2 = make_atomic_ptr< Program<Time,Message>, int>(2); 
    auto sci = make_atomic_ptr< SystemInterface<Time,Message>>(); 
    auto write1 = make_atomic_ptr< RWFunction<Time,Message>, string>(string("WriteFunction1")); 
    auto write2 = make_atomic_ptr< RWFunction<Time,Message>, string>(string("WriteFunction2")); 
    auto read1 = make_atomic_ptr< RWFunction<Time,Message>, string>(string("ReadFunction1")); 
    auto read2 = make_atomic_ptr< RWFunction<Time,Message>, string>(string("ReadFunction2")); 
  

    shared_ptr<flattened_coupled<Time, Message>> TOP(new flattened_coupled<Time, Message>( 
    {input_test_generator, kernel, program1, program2, sci, write1, write2, read1, read2}, 
    {}, 
    {{input_test_generator, kernel},{sci, kernel}, {kernel, program1}, {kernel, program2},{program1, sci}, {program2, sci}, {sci, write1}, {write1, sci}, {sci, write2}, {write2, sci},{sci, read1}, {read1, sci}, {sci, read2}, {read2, sci},{sci, program1}, {sci, program2}, {write1,kernel},{write2,kernel},{read1,kernel},{read2,kernel}}, 
    {kernel, program1, program2, sci, write1, write2, read1, read2}
    ));  
 
    cout << "Preparing runner" << endl;
    Time initial_time = Time(0);
    ofstream out_data("output.txt");  
    runner<Time, Message> r(TOP, initial_time, out_data, [](ostream& os, Message m){ os << m;});
    Time end_time = Time(2500); 

    cout << "Starting simulation until time: " << end_time << "seconds" << endl;
    auto start = hclock::now();
    end_time = r.runUntil(end_time);
    auto elapsed = chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>> (hclock::now() - start).count();     
    cout << "Finished simulation with time: " << end_time << "sec" << endl;
    cout << "Simulation took: " << elapsed << "sec" << endl;
    return 0;
}

