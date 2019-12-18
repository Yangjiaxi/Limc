// #define STAGE 1 // 1->file 2->cmd
#define FILENAME "../test/simple.c"

#include "0-color.h"
#include "0-driver.h"
#include "cxxopts.hpp"
#include <fstream>

using namespace std;

int main(int argc, char **argv) {

    Limc::Driver driver;

    // cxxopts::Options options("Limc", " - Limc Is a Minimal Compiler. [C-Like]");
    // options.add_options()("f,file", " - File name", cxxopts::value<std::string>());
    // options.add_options()("t,tree", " - Print tree");
    //
    // auto opts_res = options.parse(argc, argv);
    //
    // string filename;
    // if (opts_res.count("f")) {
    //     filename = opts_res["f"].as<std::string>();
    // } else if (opts_res.count("file")) {
    //     filename = opts_res["file"].as<std::string>();
    // }
    // ifstream file(filename);
    // if (filename.empty()) {
    //     cout << "Input program from cmd: " << endl;
    //     driver.set_entry(&cin);
    // } else {
    //     driver.set_entry(&file);
    // }

    ifstream file(FILENAME);
    driver.set_entry(&file);

    int    res  = driver.parse();
    string line = string(50, '-');

    // if (opts_res.count("t") || opts_res.count("tree")) {
    //     cout << driver.print() << endl;
    // }

    // end
    if (!res) {
        cout << BOLD_GREEN << "Parse Finish!" << RESET_COLOR << endl;
    } else {
        cout << BOLD_RED << "Parse Failed!" << RESET_COLOR << endl;
        return 1;
    }

    driver.analyze();
    cout << driver.print() << endl;
    // driver.gen_ir();
    // if (opts_res.count("t") || opts_res.count("tree")) {
    //     cout << driver.print() << endl;
    // }
    return 0;
}
