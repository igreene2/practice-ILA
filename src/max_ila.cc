/// \file the ila example of max accelerator: 
/// can compute the max values in a specified array,
//  receives MMIO instruction
/// Isabel Greene (igreene@princeton.edu)
///

#include <ilang/ilang++.h>
#include <max_ila.h>
#include <ilang/util/log.h>

namespace ilang {
namespace max {

    Ila GetMaxIla(const std::string& model_name) {
            // construct the model
            auto m = ilang::Ila("MAX");
            std::cout << "made MAX\n";
            // inputs
            // 0 - read, 1 - write
            m.NewBoolInput("mode");
            m.NewBvInput("addr_in", 16);
            m.NewBvInput("data_in", 16);

            // internal arch state (registers)
            m.NewBvState("start_addr", 16);
            m.NewBvState("array_len" , 16);

            // the memory: 160 bytes
            // check on this declaration
            m.NewMemState("mem", 16, 8);

            // the output
            m.NewBvState("result", 8); 

            // internal state
            m.NewBvState("child_flag", 1);
            m.NewBvState("child_state", 2);
            m.AddInit(m.state("child_flag") == 0);
            m.AddInit(m.state("child_state") == 0);
            std::cout << "declared all the state\n";
            // m is valid if...
            m.SetValid(m.input("addr_in") > 0x00 & m.input("addr_in") < 0xA4);
            std::cout << "did the valid\n";


        { // START_ADDR
            std::cout << "inside START_ADDR\n";
            auto instr = m.NewInstr("START_ADDR");
            instr.SetDecode((m.input("mode") == 1) & (m.input("addr_in") == 0xA1));

            instr.SetUpdate(m.state("start_addr"), m.input("data_in")); // update a start_addr

            

        }


        { // ARRAY_LEN
            std::cout << "inside ARRAY_LEN\n";
            // See child-ILA for details
            auto instr = m.NewInstr("ARRAY_LEN");
            instr.SetDecode((m.input("mode") == 1) & (m.input("addr_in") == 0xA2)); // is addr_in meant to be a decode thing?
            
            instr.SetUpdate(m.state("array_len"), m.input("data_in"));
           

        }

        { // MAX_COMPUTE
            std::cout << "inside MAX_COMPUTE\n";
            auto instr = m.NewInstr("MAX_COMPUTE");
            instr.SetDecode((m.input("mode") == 1) & (m.input("addr_in") == 0xA3));

            instr.SetUpdate(m.state("child_flag"), BvConst(1, 1));
            instr.SetUpdate(m.state("child_state"), BvConst(0, 2));
            std::cout << "going to the kiddies\n";
            DefineMaxChild(m);

       
        }

        { // STORE_DATA
            std::cout << "inside STORE_DATA\n";
            auto instr = m.NewInstr("STORE_DATA");
            instr.SetDecode(m.input("mode") == 1); // check this condition

            auto update_memory_at_addrin = Store(m.state("mem"), m.input("addr_in"), m.input("data_in"));
            instr.SetUpdate(m.state("mem"), update_memory_at_addrin);
            std::cout << "outside STORE_DATA\n";


        }

        return m;
    }




    };
};