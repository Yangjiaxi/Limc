#include "0-color.h"
#include "0-driver.h"
#include <fstream>

using namespace std;

void err() {
    cout << "Usage: ./limc <path/to/input.c>" << endl;
    exit(1);
}

int main(int argc, char **argv) {
    Limc::Driver driver;

    if (argc != 2) {
        err();
    }

    string input_file(argv[1]);

    if (input_file.empty()) {
        err();
    }

    ifstream input(input_file);

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
    driver.print_ir();
    return 0;
}
