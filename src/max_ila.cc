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
            // inputs
            // 0 - read, 1 - write
            m.NewBoolInput("mode");
            m.NewBvInput("addr_in", 8),
            m.NewBvInput("data_in", 8),

            // internal arch state (registers)
            m.NewBvState("start_addr", 16),
            m.NewBvState("array_len" , 16),

            // the memory: 160 bytes
            auto mem = m.NewMemState("mem", 8, 1),

            // the output
            m.NewBvState("result", 1) 

            // AES fetch function -- what corresponds to instructions
            // model.SetFetch(Concat(cmd, Concat(cmdaddr, cmddata)));
            // How should I do fetch function?
            // I don't think I need to fetch because I'm not getting weird input from a processor
            //m.SetFetch(Concat(mode, Concat(addr_in, data_in)));
            
            // Valid -> For all these instructions mode must be 1?
            m.SetValid(mode == 1);

        // add instructions
        { // START_ADDR
            auto instr = m.NewInstr("START_ADDR");

            instr.SetDecode((m.input("mode") == 1) & (m.input("addr_in") == 0xA1));

            instr.SetUpdate(m.state("start_addr"), m.input("data_in")); // update a start_addr

            // guarantees no change
            // if not specified, it means it allows any change
            instr.SetUpdate(m.state("array_len") , m.state("array_len"));

        }


        { // ARRAY_LEN
            // See child-ILA for details
            auto instr = m.NewInstr("ARRAY_LEN");

            instr.SetDecode((m.input("mode") == 1) & (m.input("addr_in") == 0xA2)); // is addr_in meant to be a decode thing?
            
            instr.SetUpdate(m.state("array_len"), m.input("data_in"));
            instr.SetUpdate(m.state("start_addr"), m.state("start_addr"));

        }

        { // MAX_COMPUTE
            auto instr = m.NewInstr("MAX_COMPUTE");

            instr.SetDecode((m.input("mode") == 1) & (m.input("addr_in") == 0xA3));
            
            // do max computing work here 
            // must cycle through and do comparisions
            // need to know where to start and how big data is
            // just do like a for loop from start addr and cycle to array_len
            // what exactly am I cycling through 
            // Accelerator has an internal memory size is 160 Byte (memory address range 0x0 ~ 0x9F).

            int temp = 0; 
            // do I need to do any conversions here? like from binary??
            for (int i = m.state("start_addr"); i < m.state("array_len"); i++) {
                auto value_at_addr_i = ilang::Load(mem, i);
                if (value_at_addr_i > temp) {
                    temp = value_at_addr_i;
                }
            }

            instr.SetUpdate(m.state("result"), temp);

            // guarantees no change when the instruction executes
            // if you don't write them, that means no guarantees
            instr.SetUpdate(m.state("start_addr"), m.state("start_addr"));
            instr.SetUpdate(m.state("array_len"), m.state("array_len"));
        }

        { // STORE_MAX
            auto instr = m.NewInstr("STORE_MAX");

            instr.SetDecode(m.input("mode") == 1);

            auto update_memory_at_addrin = ilang::Store(mem, m.input("addr_in"), m.state("result"));

            // guarantee no change
            instr.SetUpdate(m.state("start_addr"), m.state("start_addr"));
            instr.SetUpdate(m.state("array_len"), m.state("array_len"));
            instr.SetUpdate(m.state("result"), m.state("result"));

        }

        return m;
    }




    };
};