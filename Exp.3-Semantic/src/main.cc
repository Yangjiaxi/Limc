#include "0-color.h"
#include "0-driver.h"
#include "cxxopts.hpp"
#include <fstream>

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;

int main(int argc, char **argv) {
    cxxopts::Options options("Limc", " - Limc Is a Minimal Compiler. [C-Like]");
    options.add_options()("f,file", " - File name", cxxopts::value<std::string>());
    options.add_options()("t,tree", " - Print tree");

    auto opts_res = options.parse(argc, argv);

    Limc::Driver driver;

    // choose input file
    string filename;
    if (opts_res.count("f")) {
        filename = opts_res["f"].as<std::string>();
    } else if (opts_res.count("file")) {
        filename = opts_res["file"].as<std::string>();
    }
    ifstream file(filename);
    if (filename.empty()) {
        cout << "Input program from cmd: " << endl;
        // cout.flush();
        driver.set_entry(&cin);
    } else {
        driver.set_entry(&file);
    }

    int res = driver.parse();

    // print AST or not
    if (opts_res.count("t") || opts_res.count("tree")) {
        string line = string(50, '-');
        cout << line << endl;
        cout << driver.print() << endl;
        cout << line << endl;
    }

    // end
    if (!res) {
        cout << BOLD_GREEN << "Parse Finish!" << RESET_COLOR << endl;
    } else {
        cout << BOLD_RED << "Parse Failed!" << RESET_COLOR << endl;
    }

    driver.analyze();
    return res;
}
