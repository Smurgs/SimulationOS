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
    int process_count = 0;
    while (getline(file, str)){
        process_count++;
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
    std::vector<std::shared_ptr<model<Time>>> models;
    std::vector<std::shared_ptr<model<Time>>> eic;
    std::vector<std::pair<std::shared_ptr<model<Time>>, std::shared_ptr<model<Time>>>> ic;
    std::vector<std::shared_ptr<model<Time>>> eoc;

    auto kernel = make_atomic_ptr< Kernel<Time,Message>, int>(process_count);
    auto sci = make_atomic_ptr< SystemInterface<Time,Message>>(); 

    // Add to model
    models.push_back(input_test_generator);
    models.push_back(kernel);
    models.push_back(sci);

    // Add pair to ic vector
    ic.push_back(std::make_pair(input_test_generator, kernel));
    ic.push_back(std::make_pair(sci, kernel));

    // Add to eoc
    eoc.push_back(kernel);
    eoc.push_back(sci);

    for (int i = 1; i <= process_count; i++){
        // Create program model and its read/write models
        auto program = make_atomic_ptr<Program<Time, Message>, int>(i);
        auto write = make_atomic_ptr< RWFunction<Time,Message>, string>(string("WriteFunction") + to_string(i)); 
        auto read = make_atomic_ptr< RWFunction<Time,Message>, string>(string("ReadFunction") + to_string(i)); 

        // Add to models vector
        models.push_back(program);
        models.push_back(write);
        models.push_back(read);

        // Add pairs to ic vector
        ic.push_back(std::make_pair(program, sci));
        ic.push_back(std::make_pair(kernel, program));
        ic.push_back(std::make_pair(sci, read));
        ic.push_back(std::make_pair(sci, write));
        ic.push_back(std::make_pair(read, sci));
        ic.push_back(std::make_pair(write, sci));
        ic.push_back(std::make_pair(write, kernel));
        ic.push_back(std::make_pair(read, kernel));

        // Add to eoc
        eoc.push_back(program);
        eoc.push_back(write);
        eoc.push_back(read);
    }

  
    shared_ptr<flattened_coupled<Time, Message>> TOP(new flattened_coupled<Time, Message>( 
    models,
    eic,
    ic,
    eoc));  
 
    cout << "Preparing runner" << endl;
    Time initial_time = Time(0);
    ofstream out_data("output.txt");  
    runner<Time, Message> r(TOP, initial_time, out_data, [](ostream& os, Message m){ os << m;});
    Time end_time = Time(100000); 

    cout << "Starting simulation until time: " << end_time << "seconds" << endl;
    auto start = hclock::now();
    end_time = r.runUntil(end_time);
    auto elapsed = chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>> (hclock::now() - start).count();     
    cout << "Finished simulation with time: " << end_time << "sec" << endl;
    cout << "Simulation took: " << elapsed << "sec" << endl;
    return 0;
}

