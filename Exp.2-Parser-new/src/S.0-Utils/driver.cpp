#include "driver.h"
#include <sstream>

using namespace Limc;

Driver::Driver()
    : tokens(), scanner(*this), parser(scanner, *this), loc(location()) {}

int Driver::parse() {
    loc.initialize();
    return parser.parse();
}

void Driver::clear() {
    loc.initialize();
    tokens.clear();
}

string Driver::pretty_print() const {
    stringstream s;
    s << "Abstract Syntax Tree:" << endl;
    for (const auto &token : tokens) {
        s << token.pretty_print() << endl;
    }
    return s.str();
}

void Driver::switchInputStream(istream *is) {
    loc.initialize();
    scanner.switch_streams(is, nullptr);
    tokens.clear();
}

void Driver::addToken(const Token &token) { tokens.push_back(token); }

void Driver::inc_x(int x) {
    loc.columns(x);
    loc.step();
}

void Driver::inc_y(int y) {
    loc.lines(y);
    loc.step();
}

Limc::location &Driver::getLocation() { return loc; }
