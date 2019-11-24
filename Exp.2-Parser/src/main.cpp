#include <fstream>
#include "driver.h"
#include "color.h"

int main(int argc, char** argv)
{
    Limc::Driver driver;
    ifstream input(argv[1]);
    driver.switchInputStream(argc == 2 ? &input : &cin);
    string line = string(50, '-');
    cout << "Parsing..." << endl << line << endl;
    int res = driver.parse();
    cout << line << endl;
    if (!res) {
        cout << BOLD_GREEN << "Parse Finish!" << RESET_COLOR << endl;
    } else {
        cout << BOLD_RED << "Parse Failed!" << RESET_COLOR << endl;
    }
    return res;
}
