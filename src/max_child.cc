#include <ilang/ilang++.h>
#include <max_ila.h>
#include <ilang/util/log.h>

namespace ilang {
namespace max {


void DefineMaxChild(Ila& m) {
  
    auto child = m.NewChild("max_calc");
    auto is_child_valid = (m.state("child_flag") == BvConst(1, 1));
    child.SetValid(is_child_valid);

    // hold the max
    auto max  = child.NewBvState("max", 8);
    // get compare from memory and see if it is new max
    auto compare = child.NewBvState("compare", 8);
    // make sure we only do this array_len times
    auto byte_cnt = child.NewBvState("byte_cnt", 16);

    // make sure this is how you init to 0
    child.AddInit(byte_cnt == 0);
    child.AddInit(max == 0);

    { // START (0) load first two from array 
        auto instr = child.NewInstr("START");
        instr.SetDecode(m.state("child_state") == BvConst(0, 2));

        // add first byte at start_addr -> check thats how load works
        instr.SetUpdate(max, Load(m.state("mem"), m.state("start_addr")));
        // load the next byte after start_addr to compare to first
        instr.SetUpdate(compare, Load(m.state("mem"), m.state("start_addr") + 1));
        // update the byte_cnt to reflect the two bytes read
        instr.SetUpdate(byte_cnt, byte_cnt + 2);
        // move to the COMPARE state
        instr.SetUpdate(m.state("child_state"), BvConst(1, 2));

    }

    // COMPARE (1) things loaded 
    {
        auto instr = child.NewInstr("COMPARE");
        instr.SetDecode(m.state("child_state") == BvConst(1, 2));
        // byte_cnt does not increase
        instr.SetUpdate(byte_cnt, byte_cnt);
        // compare does not change 
        instr.SetUpdate(compare, compare);
        // set max to be larger of max and compare 
        instr.SetUpdate(max, Ite(max > compare, max, compare));
        // move to LOAD state
        instr.SetUpdate(m.state("child_state"), BvConst(2, 2));

    }

    // LOAD (2) next array value to compare
    {   
        auto instr = child.NewInstr("LOAD");
        instr.SetDecode(m.state("child_state") == BvConst(2, 2));
        
        // load the next byte into compare 
        instr.SetUpdate(compare, Load(m.state("mem"), m.state("start_addr") + byte_cnt));
        // increase the byte_cnt by 1
        instr.SetUpdate(byte_cnt, byte_cnt + 1);
        // if we've seen the array_len of bytes then we move to STORE, if not we move to COMPARE
        // check and make sure this logic works (we see all bytes)
        instr.SetUpdate(m.state("child_state"), Ite(byte_cnt == m.state("array_len"), BvConst(3, 2), BvConst(1, 2)));

    }

    // STORE (3) in result 
    {
        auto instr = child.NewInstr("STORE");
        instr.SetDecode(m.state("child_state") == BvConst(3, 2));

        // if we've seen all the bytes then we can store the max into result 
        instr.SetUpdate(m.state("result"), max);
        // we no longer need to stay in child instructions so the flag is 0
        instr.SetUpdate(m.state("child_flag"), BvConst(0, 1));
        // byte_cnt remains the same 
        instr.SetUpdate(byte_cnt, byte_cnt);
        // check about updating all state variables in all instructions




    }

}


}; // namespace max

}; // namespace ilang
