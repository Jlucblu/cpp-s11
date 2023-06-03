#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

// реализуйте эту функцию:
void CreateFiles(const string& config_file) {
    std::string line;
    std::ifstream ist(config_file);
    std::ofstream out;

    while (getline(ist, line)) {
        if (line[0] == '>') {
            out << std::string(line.begin() + 1, line.end()) << std::endl;
        }
        else {
            out.close();
            out.open(line);
        }
    }
}

string GetLine(istream& in) {
    string s;
    getline(in, s);
    return s;
}

void TestCreateFiles() {
    ofstream("test_config.txt"s) << "a.txt\n"
        ">10\n"
        ">abc\n"
        "b.txt\n"
        ">123"sv;

    CreateFiles("test_config.txt"s);
    ifstream in_a("a.txt"s);
    assert(GetLine(in_a) == "10"s && GetLine(in_a) == "abc"s);

    ifstream in_b("b.txt"s);
    assert(GetLine(in_b) == "123"s);
}
