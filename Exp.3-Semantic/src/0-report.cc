#include "0-report.h"

using namespace Limc;

Report::Report() : msg(), level(Level::Info), locs(){};

Report &Report::report_loc(const location &loc) {
    locs.push_back(loc);
    return *this;
}

Report &Report::report_msg(const string &_msg) {
    msg = _msg;
    return *this;
}

Report &Report::report_level(Level lvl) {
    level = lvl;
    return *this;
}

string &Report::Report::get_msg() { return msg; }

Level Report::get_level() { return level; }

vector<location> &Report::get_loc() { return locs; }