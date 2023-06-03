#pragma once

#include <string>

// напишите эту функцию
inline bool DecodeRLE(const std::string& src_name, const std::string& dst_name) {
    using namespace std;

    ifstream input(src_name, ios::binary);
    if (!input) {
        return false;
    }

    ofstream output(dst_name, ios::binary);
    if (!output) {
        return false;
    }

    while (input) {
        unsigned char header;
        header = static_cast<unsigned char>(input.get());

        int block_type = (header & 1);
        int data_size = (header >> 1) + 1;

        if (block_type == 0) {
            for (int i = 0; i < data_size; i++) {
                char ch = input.get();
                if (!input.eof()) {
                    output << ch;
                }
            }
        }
        else {
            char ch = input.get();
            if (!input.eof()) {
                std::string buffer(data_size, ch);
                output << buffer;
            }
        }
    }

    return true;
}