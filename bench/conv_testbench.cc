#include <systemc>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>

#include "nlohmann/json.hpp"
#include "max_ila.h"

using json = nlohmann::json;

SC_MODULE(Source) {
  sc_in<bool> clk{"clk"};

  sc_out< sc_biguint<1> > max_if_wr;
  sc_out< sc_biguint<1> > max_if_rd;
  sc_out< sc_biguint<32> > max_addr_in;
  sc_out< sc_biguint<8> > max_data_in[16];

  sc_out< sc_biguint<1> > input_done;

  SC_CTOR(Source) {
    SC_THREAD(source_input);
    sensitive << clk.pos();
  }

  void source_input() {
    // reset the port
    max_if_wr = 0;
    max_if_rd = 0;
    max_addr_in = 0;
    for (auto i = 0; i < 16; i++) {
      max_data_in[i] = 0;
    }

    input_done = 0;
    // read program fragment from file
    std::ifstream fin;
    fin.open("./sim_info/test_input_max.json", ios::in);
    
    //parse the json file
    json cmd_seq;
    fin >> cmd_seq;

    // pass the command to the ports
    for (int i = 0; i < cmd_seq["program fragment"].size(); i++) {
      max_if_rd = std::stoi(cmd_seq["program fragment"][i]["is_rd"].get<std::string>(), nullptr, 16);
      max_if_wr = std::stoi(cmd_seq["program fragment"][i]["is_wr"].get<std::string>(), nullptr, 16);
      // fetch the address
      std::string addr = cmd_seq["program fragment"][i]["addr"].get<std::string>();
      max_addr_in = std::stoi(addr, nullptr, 16);
      // extract each data byte from data
      std::string data = cmd_seq["program fragment"][i]["data"].get<std::string>();
      for (int j = 0; j < 16; j++) {
        max_data_in[j] = std::stoi(data.substr(30-2*j,2), nullptr, 16);
      }
      wait(10, SC_NS);
    }

    input_done = 1;
}

};

SC_MODULE(testbench) {
  // SC_HAS_PROCESS(testbench);
  max max_inst;
  Source src;

  sc_clock clk;
  sc_signal< sc_biguint<1> > _if_wr_signal;
  sc_signal< sc_biguint<1> > max_if_rd_signal;
  sc_signal< sc_biguint<32> > max_addr_signal;
  sc_signal< sc_biguint<8> > max_data_signal[16];

  sc_signal< sc_biguint<1> > input_done;

  SC_CTOR(testbench) :
    clk("clk", 1, SC_NS),
    max_inst("max_inst"),
    src("source")
  {
    // binding the signals
    src.clk(clk);
    src.max_if_rd(max_if_rd_signal);
    src.max_if_wr(max_if_wr_signal);
    src.max_addr_in(max_addr_signal);
    for (int i = 0; i < 16; i++) {
      src.max_data_in[i](max_data_signal[i]);
    }
    src.input_done(input_done);

    // // connecting signals to max
    max_inst.max_if_rd_in(max_if_rd_signal);
    max_inst.max_if_wr_in(max_if_wr_signal);
    max_inst.max_addr_in_in(max_addr_signal);
    max_inst.max_data_in_0_in(max_data_signal[0]);
    max_inst.max_data_in_1_in(max_data_signal[1]);
    max_inst.max_data_in_2_in(max_data_signal[2]);
    max_inst.max_data_in_3_in(max_data_signal[3]);
    max_inst.max_data_in_4_in(max_data_signal[4]);
    max_inst.max_data_in_5_in(max_data_signal[5]);
    max_inst.max_data_in_6_in(max_data_signal[6]);
    max_inst.max_data_in_7_in(max_data_signal[7]);
    max_inst.max_data_in_8_in(max_data_signal[8]);
    max_inst.max_data_in_9_in(max_data_signal[9]);
    max_inst.max_data_in_10_in(max_data_signal[10]);
    max_inst.max_data_in_11_in(max_data_signal[11]);
    max_inst.max_data_in_12_in(max_data_signal[12]);
    max_inst.max_data_in_13_in(max_data_signal[13]);
    max_inst.max_data_in_14_in(max_data_signal[14]);
    max_inst.max_data_in_15_in(max_data_signal[15]);


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
    std::cout << "SPAD1: result: " << std::endl;

		// validate simulation result
    int err = 0;
    json max_result;
    std::ifstream result_in;
    result_in.open("./sim_info/sim_result.json", ios::in);
		result_in >> max_result;

    for (int i = 0; i < max_result["SPAD1 result"].size(); i++) {
			int sim_addr = max_result["SPAD1 result"][i]["addr"];
      int sim_data = max_result["SPAD1 result"][i]["data"];
      int ila_addr = sim_addr - 0x24000;
    //  cout << max_result["SPAD1 result"][i] << '\t' << sim_addr << '\t' << sim_data << endl;
      sc_biguint<8> byte_0 = max_inst.max_scratch_pad_1[ila_addr];
      sc_biguint<8> byte_1 = max_inst.max_scratch_pad_1[ila_addr+1];
    	sc_biguint<16> data = byte_1;
      data = (data << 8) + byte_0;
      if (data.to_uint() != sim_data) {
      	err++;
      	std::cout << "error found: " << std::hex << sim_addr << "::" << ila_addr << '\t' << "sim_data::ila_data" << '\t' << sim_data << "::" << data << std::endl;
      }
    }

    if (err == 0)
        std::cout << "testbench passed" << std::endl;
/*
    for (int i = 0; i < 0x40; i++) {
    	std::cout << "addr: " << std::hex << 0x24000 + i*0x10 << '\t' << "data: ";
			for (int j = 0; j < 8; j++) {
      	sc_biguint<8> byte_0 = max_inst.max_scratch_pad_1[i*0x10 + 2*j];
        sc_biguint<8> byte_1 = max_inst.max_scratch_pad_1[i*0x10 + 2*j+1];
      	sc_biguint<16> data = byte_1;
        data = (data << 8) + byte_0;
        std::cout << std::hex << data.to_uint() << " ";
      }
      std::cout << std::endl;
    }
    */

    sc_stop();
  }
};

int sc_main(int argc, char *argv[]) {
  std::cout << "test start" << std::endl;
  testbench tb("tb");
  sc_start();

  return 0;
}
