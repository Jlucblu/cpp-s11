#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

// �������� ��� �������
void PrintTree(ostream& dst, const path& p, const int offset = 0) {
    std::string space(offset, ' ');
    dst << space + p.filename().string() + "\n";

    std::vector<std::filesystem::path> catalog;
    for (const auto& d : std::filesystem::directory_iterator(p)) {
        catalog.push_back(d);
    }
    std::sort(catalog.begin(), catalog.end(), 
        [](const std::filesystem::path& left, const std::filesystem::path& right) {
        return left.string() > right.string();
        });

    for (const auto& path : catalog) {
        auto status = std::filesystem::status(path);
        if (status.type() == std::filesystem::file_type::directory) {
            PrintTree(dst, path, 2);
        }
        else {
            dst << space + "  " + path.filename().string() + "\n";
        }
    }
}

int main() {
    error_code err;
    filesystem::remove_all("test_dir", err);
    filesystem::create_directories("test_dir"_p / "a"_p, err);
    filesystem::create_directories("test_dir"_p / "b"_p, err);

    ofstream("test_dir"_p / "b"_p / "f1.txt"_p);
    ofstream("test_dir"_p / "b"_p / "f2.txt"_p);
    ofstream("test_dir"_p / "a"_p / "f3.txt"_p);

    ostringstream out;
    PrintTree(out, "test_dir"_p);
    assert(out.str() ==
        "test_dir\n"
        "  b\n"
        "    f2.txt\n"
        "    f1.txt\n"
        "  a\n"
        "    f3.txt\n"sv
    );
}
