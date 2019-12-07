#define STAGE 1 // 0->type 1->file 2->cmd
#define FILENAME "../test/simple.c"

#include "0-color.h"
#include "0-driver.h"
#include "cxxopts.hpp"
#include <fstream>

// test
// #include "0-driver.h"
// #include "0-token.h"
// #include "3-type.h"

using namespace std;

int main(int argc, char **argv) {

    Limc::Driver driver;

#if STAGE == 2
    cxxopts::Options options("Limc", " - Limc Is a Minimal Compiler. [C-Like]");
    options.add_options()("f,file", " - File name", cxxopts::value<std::string>());
    options.add_options()("t,tree", " - Print tree");

    auto opts_res = options.parse(argc, argv);

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
#elif STAGE == 1
    ifstream file(FILENAME);
    cout << "for " << FILENAME << endl;
    driver.set_entry(&file);
#endif

    int    res  = driver.parse();
    string line = string(50, '-');

#if STATE == 2
    // print AST or not
    if (opts_res.count("t") || opts_res.count("tree")) {
        cout << driver.print() << endl;
    }
#else
    cout << driver.print() << endl;
#endif

    // end
    if (!res) {
        cout << BOLD_GREEN << "Parse Finish!" << RESET_COLOR << endl;
    } else {
        cout << BOLD_RED << "Parse Failed!" << RESET_COLOR << endl;
    }

    driver.analyze();
    // if (opts_res.count("t") || opts_res.count("tree")) {
    //     cout << driver.print() << endl;
    // }
    return res;

    // string        input("struct { char a[10]; }; ");
    // istringstream ss(input);
    //
    // Limc::Driver driver;
    // driver.set_entry(&ss);
    // driver.parse();
    // cout << driver.print() << endl;
    //
    // auto root = driver.get_root().get_child(0);
    // auto type = Limc::Type::build_type(root, {10});
    //
    // cout << "Type: " << type.to_string() << endl;
    // return 0;
}
