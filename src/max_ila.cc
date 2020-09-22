/// \file the ila example of max accelerator: 
/// can compute the max values in a specified array,
//  receives MMIO instruction
/// Isabel Greene (igreene@princeton.edu)
///

#include <ilang/ilang++.h>

    // construct the model
    auto m = ilang::Ila("MAX");
    // inputs
    // 0 - read, 1 - write
    auto mode = m.NewBoolInput("mode");
    auto addr_in = m.NewBvInput("addr_in", 8),
    auto data_in = m.NewBvInput("data_in", 8 ),

    // internal arch state (registers)
    auto start_addr = m.NewBvState("start_addr", 16 ),
    auto array_len = m.NewBvState("array_len" , 16 ),

    // the memory: 160 bytes
    auto mem = m.NewMemState("mem", 16, 10),

    // the output
    auto result = m.NewBvState("result", 1) {

    // AES fetch function -- what corresponds to instructions
    // model.SetFetch(Concat(cmd, Concat(cmdaddr, cmddata)));
    m.SetFetch(Concat(mode, Concat(addr_in, data_in)));
    // Valid instruction: cmd == 1 or cmd == 2
    //model.SetValid((cmd == 1) | (cmd == 2));
    
    // Valid instruction: mode == 1
    m.SetValid(mode == 1);

  // add instructions
  { // START_ADDR
    auto instr = m.NewInstr("START_ADDR");

    instr.SetDecode((mode == 1) & (addr_in == 0xA1));

    instr.SetUpdate(start_addr, addr_in); // update a start_addr

    // guarantees no change
    // if not specified, it means it allows any change
    instr.SetUpdate(array_len , array_len);

  }


  { // ARRAY_LEN
    // See child-ILA for details
    auto instr = m.NewInstr("ARRAY_LEN");

    instr.SetDecode((mode == 1) & (addr_in == 0xA2)); // is addr_in meant to be a decode thing?
    
    instr.SetUpdate(array_len, data_in);
    //????????? is data_in the array_len? if not what is it?
    instr.SetUpdate(start_addr, start_addr);

  }

  { // MAX_COMPUTE
    auto instr = m.NewInstr("MAX_COMPUTE");

    instr.SetDecode((mode == 1) & (addr_in == 0xA3));
    
    // do max computing work here 
    // must cycle through and do comparisions
    // need to know where to start and how big data is
    // just do like a for loop from start addr and cycle to array_len
    // what exactly am I cycling through 
    // Accelerator has an internal memory size is 160 Byte (memory address range 0x0 ~ 0x9F).

    int temp = 0; 
    for (i = start_addr; i < array_len; i++) {
        if (mem[i] > temp) {
            temp = mem[i];
        }
    }

    instr.SetUpdate(result, temp);

    // guarantees no change when the instruction executes
    // if you don't write them, that means no guarantees
    instr.SetUpdate(start_addr, start_addr);
    instr.SetUpdate(array_len, array_len);
  }

  { // STORE_MAX
    auto instr = m.NewInstr("STORE_MAX");

    instr.SetDecode(mode == 1);

    instr.SetUpdate(mem[addr_in], result);

    // guarantee no change
    instr.SetUpdate(start_addr, start_addr);
    instr.SetUpdate(array_len, array_len);
    instr.SetUpdate(result, result);

  }




  
}