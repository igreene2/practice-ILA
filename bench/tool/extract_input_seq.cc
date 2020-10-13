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

    string file_path;
//    cout << "please specify the log file path" << endl;
//    cin >> file_path;
//    cout << "please specify output path" << endl;
    string file_out;
//    cin >> file_out;

    file_path = argv[1];
    file_out = argv[2];

    ifstream fin;
    fin.open(file_path, ios::in);

    string temp, mode, addr, data;

    json json_out;
    vector<json> command_v;

    int instr_cntr = 0;

    while(getline(fin, temp)){
        if (temp.find("[[ILA-INFO]]") == string::npos)
            continue;
//        cout << temp << endl;
        temp = temp.substr(temp.find("]]")+3);
        stringstream data_line;
        data_line << temp;
        getline(data_line, mode, ',');
        getline(data_line, addr, ',');
        getline(data_line, data);
        
        // format the data
        string data_format;
        data_format.append(32-data.length(),'0');
        data_format.append(data);
//        cout << "mode: " << mode << '\t' << "addr: " << addr << '\t' << "data: " << data << endl;
        if (mode.find("W") != string::npos) {
            json command;
            command = {{"_instr_No.", instr_cntr},
                        {"is_rd", "0"}, {"is_wr", "1"}, {"addr", addr}, {"data", data_format}};
            command_v.push_back(command);
            instr_cntr++;
        } else if (mode.find("R") != string::npos) {
            json command;
            command = {{"_instr_No.", instr_cntr},
                        {"is_rd", "1"}, {"is_wr", "0"}, {"addr", addr}, {"data", data_format}};
            command_v.push_back(command);
            instr_cntr++;
        } else {
            cout << "UNSUPPORTED TYPE FOUND! : " << temp << endl;
        }
    }

    json_out["program fragment"] = command_v;
    ofstream fout;
    fout.open(file_out, ios::out | ios::trunc);
    fout << setw(4) << json_out;
    fout.close();
    fin.close();

//    cout << json_out["command input sequence"].size() << endl;
//    for (auto i = 0; i < 10; i++) 
//        cout << json_out["command input sequence"][i] << endl;

    return 0;
}
