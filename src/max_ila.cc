/// \file the ila example of max accelerator: 
/// can compute the max values in a specified array,
//  receives MMIO instruction
/// Isabel Greene (igreene@princeton.edu)
///


//MAX::MAX()
    //: // construct the model
      /*model("AES"),
      // I/O interface: this is where the commands come from.
      cmd    (model.NewBvInput("cmd"    , 2 )), 
      cmdaddr(model.NewBvInput("cmdaddr", 16)),
      cmddata(model.NewBvInput("cmddata", 8 )),*/

    // construct the model
      auto m = ilang::Ila("MAX");
      // inputs
    // 0 - read, 1 - write
      auto mode = m.NewBoolInput("mode");
  
      auto addr_in = m.NewBvInput("addr_in", 8)),
      auto data_in = m.NewBvInput("data_in", 8 )),

      
      // internal arch state.
      //address(model.NewBvState("aes_address", 16 )),
      auto start_addr = m.NewBvState("start_addr", 16 )),
      //length (model.NewBvState("aes_length" , 16 )),
      auto array_len = m.NewBvState("array_len" , 16 )),
      /*key    (model.NewBvState("aes_key"    , 128)),
      counter(model.NewBvState("aes_counter", 128)),
      status (model.NewBvState("aes_status" , 2  )),*/


      // the memory: shared state
      //xram   (model.NewMemState("XRAM"      , 16, 8)),
// make a mem

      // The encryption function :
      // 128b plaintext x 128b key -> 128b ciphertext
      // FuncRef(name, range, domain1, domain2 )
     /* aes128(FuncRef("aes128",               // define a function
                          SortRef::BV(128),  // range: 128-bit
                          SortRef::BV(128),  // domain: 128-bit
                          SortRef::BV(128))),//      by 128-bit*/
      // the output
      //outdata(model.NewBvState("outdata", 8)) 
      auto out = m.NewBvState("result", 1)) {

  // AES fetch function -- what corresponds to instructions
 // model.SetFetch(Concat(cmd, Concat(cmdaddr, cmddata)));
    m.SetFetch(Concat(mode, Concat(addr_in, data_in)));
  // Valid instruction: cmd == 1 or cmd == 2
  //model.SetValid((cmd == 1) | (cmd == 2));
  
 // Valid instruction: mode == 1
  m.SetValid(mode == 1);

  // some shortcuts
  //auto is_status_idle = status == AES_STATE_IDLE;

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

  { // MAX_COMPUTER
    auto instr = m.NewInstr("MAX_COMPUTE");

    instr.SetDecode((mode == 1) & (addr_in == 0xA3));
    
    // do max computing work here 
    // must cycle through and do comparisions
    // need to know where to start and how big data is
    // just do like a for loop from start addr and cycle to array_len

    

    instr.SetUpdate(outdata, slice_read(length, cmdaddr, AES_LENGTH, 2, 8));

    // guarantees no change when the instruction executes
    // if you don't write them, that means no guarantees
    instr.SetUpdate(key, key);
    instr.SetUpdate(address, address);
    instr.SetUpdate(length, length);
    // but not the following two:
    //instr.SetUpdate(status, status);
    //instr.SetUpdate(counter, counter);
  }

  { // STORE_MAX
    auto instr = m.NewInstr("SOTRE_MAX");

    instr.SetDecode(mode == 1);

    instr.SetUpdate(outdata, slice_read(address, cmdaddr, AES_ADDR, 2, 8));

    // guarantee no change
    instr.SetUpdate(key, key);
    instr.SetUpdate(address, address);
    instr.SetUpdate(length, length);
    // but not the following two:
    // instr.SetUpdate(status, status);
    // instr.SetUpdate(counter, counter);
  }




  
}