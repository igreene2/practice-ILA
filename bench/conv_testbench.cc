#include <systemc>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>

#include "nlohmann/json.hpp"
#include "MAX.h"

using json = nlohmann::json;

SC_MODULE(Source) {
  sc_in<bool> clk{"clk"};

  sc_signal< sc_biguint<16> > MAX_data_in;
  sc_signal< bool > MAX_mode;
  sc_signal< sc_biguint<16> > MAX_addr_in;

  sc_out < sc_biguint<1> > input_done;


  SC_CTOR(Source) {
    SC_THREAD(source_input);
    sensitive << clk.pos();
  }

  void source_input() {
    // reset the port
    MAX_data_in = 0;
    MAX_mode = 0;
    MAX_addr_in= 0;

    input_done = 0;
    // read program fragment from file
    std::ifstream fin;
    fin.open("./sim_info/test_input_max.json", ios::in);
    
    //parse the json file
    json cmd_seq;
    fin >> cmd_seq;

    // pass the command to the ports
    for (int i = 0; i < cmd_seq["program fragment"].size(); i++) {
      MAX_mode = std::stoi(cmd_seq["program fragment"][i]["mode"].get<std::string>(), nullptr, 16);
      std::string addr = cmd_seq["program fragment"][i]["addr"].get<std::string>();
      MAX_addr_in = std::stoi(addr, nullptr, 16);
      // extract each data byte from data
      MAX_data_in = std::stoi(cmd_seq["program fragment"][i]["data"].get<std::string>(), nullptr, 16);
     
      wait(10, SC_NS);
    }

    input_done = 1;
}

};

SC_MODULE(testbench) {
  // SC_HAS_PROCESS(testbench);
  MAX max_inst;
  Source src;

  sc_clock clk;

  sc_signal< bool > MAX_mode_sig;
  sc_signal< sc_biguint<16> > MAX_data_in_sig;
  sc_signal< sc_biguint<16> > MAX_addr_in_sig;

  sc_signal< sc_biguint<1> > input_done;


  SC_CTOR(testbench) :
    clk("clk", 1, SC_NS),
    max_inst("max_inst"),
    src("source")
  {
    // binding the signals 
    src.clk(clk);
    src.MAX_mode(MAX_mode_sig);
    src.MAX_addr_in(MAX_addr_in_sig);
    src.MAX_data_in(MAX_data_in_sig);
    src.input_done(input_done);
    

    // // connecting signals to max
    max_inst.MAX_mode_in(MAX_mode_sig);
    max_inst.MAX_addr_in_in(MAX_addr_in_sig);
    max_inst.MAX_data_in_in(MAX_data_in_sig);

    max_inst.instr_log;

 
    SC_THREAD(run);
  }

  void run() {
    max_inst.instr_log.open("./sim_info/instr_log_max.txt", ofstream::out | ofstream::trunc);

    std::cout << "start running" << std::endl;
    std::cout << "*********** simulation start ***********" << std::endl;
    wait(10, SC_NS);

    while (input_done == 0) {
		  std::cout << "current simulation time: " << '\t' << sc_time_stamp() << "\r" << std::flush;
      wait(10, SC_NS);
    }

    wait(100000, SC_NS);
    std::cout << '\n' << std::endl;
    std::cout << "************* sc_stop **************" << std::endl;
    max_inst.instr_log.close();
    std::cout << "result: " << std::endl;
    std::cout << max_inst.MAX_result;

	
    std::cout << "\ntestbench passed" << std::endl;

    sc_stop();
  }
};

int sc_main(int argc, char *argv[]) {
  std::cout << "test start" << std::endl;
  testbench tb("tb");
  sc_start();

  return 0;
}
