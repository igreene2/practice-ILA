#include <MAX.h>
#include <systemc.h>
#include <ap_int.h>


// signed greater than comparison
sc_biguint<8> MAX::Greatest(sc_biguint<8> comp1, sc_biguint<8> comp2) {
  
  sc_bigint<8> comp1_s = comp1;
  sc_bigint<8> comp2_s = comp2;
  ap_int<8> comp_1 = comp1_s.to_int();
  ap_int<8> comp_2 = comp2_s.to_int();
  ap_int<8> out = (comp_1 > comp_2) ? comp_1 : comp_2;
  sc_bigint<8> result_s = out.to_int();
  sc_biguint<8> result = result_s;
  return result;

}
