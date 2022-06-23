// #define STAGE 1 // 1->file 2->cmd
#define FILENAME "./test/simple.c"

#include "0-color.h"
#include "0-driver.h"
#include <fstream>

using namespace std;

int main(int argc, char **argv) {

    Limc::Driver driver;

    ifstream file(FILENAME);
    driver.set_entry(&file);

    int res = driver.parse();
    string line = string(50, '-');

    if (!res) {
        cout << BOLD_GREEN << "Parse Finish!" << RESET_COLOR << endl;
    } else {
        cout << BOLD_RED << "Parse Failed!" << RESET_COLOR << endl;
        return 1;
    }

    driver.analyze();
    cout << driver.print() << endl;
    return 0;
}
