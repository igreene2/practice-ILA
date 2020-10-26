#include <MAX.h>
#include <systemc.h>
#include <ap_int.h>


// signed greater than comparison
sc_biguint<32> MAX::Greatest(sc_biguint<16> comp1, sc_biguint<16> comp2) {
  
  sc_bigint<16> comp1_s = comp1;
  sc_bigint<16> comp2_s = comp2;
  ap_int<16> comp_1 = comp1_s.to_int();
  ap_int<16> comp_2 = comp2_s.to_int();
  ap_int<16> out = (comp_1 > comp_2) ? comp_1 : comp_2;
  sc_bigint<16> result_s = out.to_int();
  sc_biguint<16> result = result_s;
  return result;

}
