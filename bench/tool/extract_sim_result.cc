#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    string file_in;
    string file_out;

    file_in = argv[1];
    file_out = argv[2];

		ifstream fin;
    fin.open(file_in, ios::in);
		
		string temp, addr, data;
    json json_out;
    vector<json> result_v;

   	while(getline(fin, temp)) {
       if (temp.find("[[ILA-RESULT]]") == string::npos)
          continue;
       temp = temp.substr(temp.find("addr")+5);
//       cout << temp << endl;
       stringstream data_line;
       data_line << temp;
       getline(data_line, addr, ';');
       getline(data_line, temp, ':');

       for (int i = 0; i < 8; i++) {
           json result;
           getline(data_line, data, '_');
           //cout << addr << '\t' << data << endl;
           result = {{"addr", stoi(addr, nullptr, 16)+2*i},
               				{"data", stoi(data, nullptr, 16)}};
					 result_v.push_back(result);
       }
    }

    json_out["SPAD1 result"] = result_v;
    ofstream fout;
    fout.open(file_out, ios::out | ios::trunc);
    fout << setw(2) << json_out;
    fout.close();
    fin.close();
    
    return 0;
}
