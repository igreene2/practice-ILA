// File: main.cc

#include <max_ila.h>
#include <fstream>
#include <ilang/util/log.h>


using namespace ilang;

int main() {
  SetToStdErr(1);
  // get the ILA model
  auto max = max::GetMaxIla("max");
  std::cout << "successfully constructed max model";

  ILA_INFO << "#instr: " << max.instr_num();
  ILA_INFO << "#input: " << max.input_num();
  ILA_INFO << "#state: " << max.state_num();
  for (auto i = 0; i < max.instr_num(); i++) {
    ILA_INFO << "instr." << i << " " << max.instr(i);
  }

  std::cout << "successfully printed state stuff";

  ExportSysCSim(max, "./sim_model", false);

  // // example - export ILA to Verilog
  // ILA_INFO << "Export " << hlscnn << " to hlscnn.v";
  // std::ofstream fw("hlscnn.v");
  // hlscnn.ExportToVerilog(fw);
  // fw.close();

  return 0;
}