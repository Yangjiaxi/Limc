#ifndef _REPORT_H
#define _REPORT_H

#include "location.hh"
#include <vector>

using namespace std;

namespace Limc {

enum Level {
    Info,
    Warning,
    Error,
};

class Report {
  public:
    Report();

    Report &report_msg(const string &msg, unsigned enb);
    Report &report_level(Level lvl);
    Report &report_loc(const location &loc);

    string &          get_msg();
    unsigned          get_errno();
    Level             get_level();
    vector<location> &get_loc();

  private:
    string           msg;
    unsigned         error_number;
    vector<location> locs;
    Level            level;
    /*
        0 -> info       BLUE
        1 -> warning    YELLOW
        2 -> error      RED
    */
};
} // namespace Limc

#endif // _REPORT_H
