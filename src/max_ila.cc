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
            m.NewBvInput("addr_in", 8);
            m.NewBvInput("data_in", 8);

            // internal arch state (registers)
            m.NewBvState("start_addr", 16);
            m.NewBvState("array_len" , 16);

            // the memory: 160 bytes
            // check on this declaration
            m.NewMemState("mem", 8, 1);

            // the output
            m.NewBvState("result", 8); 

            // internal state
            m.NewBvState("child_flag", 1);
            m.NewBvState("child_state", 2);
            m.AddInit(m.state("child_flag") == 0);
            m.AddInit(m.state("child_state") == 0);
            std::cout << "declared all the state\n";
            // m is valid if...
            m.SetValid(m.input("start_addr") > 0x00 & m.input("start_addr") < 0xA4);


        { // START_ADDR
            std::cout << "inside START_ADDR\n";
            auto instr = m.NewInstr("START_ADDR");
            instr.SetDecode((m.input("mode") == 1) & (m.input("addr_in") == 0xA1));

            instr.SetUpdate(m.state("start_addr"), m.input("data_in")); // update a start_addr

            // guarantees no change
            // if not specified, it means it allows any change
            instr.SetUpdate(m.state("array_len") , m.state("array_len"));

        }


        { // ARRAY_LEN
            std::cout << "inside ARRAY_LEN\n";
            // See child-ILA for details
            auto instr = m.NewInstr("ARRAY_LEN");
            instr.SetDecode((m.input("mode") == 1) & (m.input("addr_in") == 0xA2)); // is addr_in meant to be a decode thing?
            
            instr.SetUpdate(m.state("array_len"), m.input("data_in"));
            instr.SetUpdate(m.state("start_addr"), m.state("start_addr"));

        }

        { // MAX_COMPUTE
            auto instr = m.NewInstr("MAX_COMPUTE");
            instr.SetDecode((m.input("mode") == 1) & (m.input("addr_in") == 0xA3));

            instr.SetUpdate(m.state("child_flag"), BvConst(1, 1));
            instr.SetUpdate(m.state("child_state"), BvConst(0, 2));

            DefineMaxChild(m);

            // guarantees no change when the instruction executes
            // if you don't write them, that means no guarantees
            instr.SetUpdate(m.state("start_addr"), m.state("start_addr"));
            instr.SetUpdate(m.state("array_len"), m.state("array_len"));
        }

        { // STORE_MAX
            auto instr = m.NewInstr("STORE_MAX");
            instr.SetDecode(m.input("mode") == 1); // check this condition

            auto update_memory_at_addrin = ilang::Store(m.state("mem"), m.input("addr_in"), m.state("result"));

            // guarantee no change
            instr.SetUpdate(m.state("start_addr"), m.state("start_addr"));
            instr.SetUpdate(m.state("array_len"), m.state("array_len"));
            instr.SetUpdate(m.state("result"), m.state("result"));

        }

        return m;
    }




    };
};