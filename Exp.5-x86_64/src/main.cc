#include "0-color.h"
#include "0-driver.h"
#include <fstream>

using namespace std;

void err() {
    cout << "Usage: ./limc <path/to/input.c> <path/to/output.s>" << endl;
    exit(1);
}

int main(int argc, char **argv) {
    Limc::Driver driver;

    if (argc != 3) {
        err();
    }

    string input_file(argv[1]);
    string output_file(argv[2]);

    if (input_file.empty() || output_file.empty()) {
        err();
    }

    ifstream input(input_file);
    ofstream output(output_file, ios::trunc);

    driver.set_entry(&input);

    if (!driver.parse()) {
        cout << BOLD_RED << "Parse Failed!" << RESET_COLOR << endl;
        return 1;
    }

    if (!driver.analyze()) {
        cout << BOLD_RED << "Analyze Failed!" << RESET_COLOR << endl;
        return 1;
    }

    // cout << driver.print() << endl;

    driver.gen_ir();
    // driver.print_ir();

    driver.alloc_reg();
    // driver.print_ir();

    driver.gen_x86_64();
    driver.dump_x86_64(output);

    output.close();
    return 0;
}
