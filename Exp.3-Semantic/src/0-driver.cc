#include "0-driver.h"
#include "0-color.h"
#include <sstream>

using namespace Limc;

Driver::Driver()
    : tokens(), scanner(*this), parser(scanner, *this), analyzer(*this), loc(location()) {}

int Driver::parse() {
    errors.str("");
    loc.initialize();
    auto res = parser.parse();
    cout << "[Scan] and [Parse] Errors: " << errors.str() << endl;
    return res;
}

void Driver::clear() {
    loc.initialize();
    tokens.clear();
}

void Driver::analyze() {
    errors.str("");
    string line(50, '-');
    for (auto &token : tokens) {
        analyzer.walk(token);
        cout << line << endl << "Errors: " << endl << line << endl << errors.str() << endl;
    }
    cout << BOLD_YELLOW << "Analyze Finish." << RESET_COLOR << endl;
    for (const auto &token : tokens) {
        cout << token.print() << endl;
    }
}

string Driver::print() const {
    stringstream s;
    s << "Abstract Syntax Tree:" << endl;
    for (const auto &token : tokens) {
        s << token.print() << endl;
    }
    return s.str();
}

void Driver::set_entry(istream *is) {
    loc.initialize();
    scanner.switch_streams(is, nullptr);
    tokens.clear();
}

void Driver::add_token(const Token &token) { tokens.push_back(token); }

void Driver::inc_x(int x) {
    loc.step();
    loc.columns(x);
}

void Driver::inc_y(int y) {
    loc.lines(y);
    loc.step();
}

location &Driver::get_loc() { return loc; }

string Driver::gen_error(const string &msg, const location &loc) {
    stringstream ss;
    string       line(50, '-');

    ss << RED << "At: " << loc << endl;
    ss << YELLOW << msg << endl;
    ss << BLUE << line << RESET_COLOR << endl;

    return ss.str();
}

void Driver::add_error(const string &msg, const Token &token) {
    add_error(msg, token.get_loc().value());
}

void Driver::add_error(const string &msg, const location &loc) {
    errors << RED << "Error: " << msg << endl << YELLOW << "At: " << loc << RESET_COLOR << endl;
}

void Driver::add_error(const string &msg) {
    // enhence later
    errors << YELLOW << msg << RESET_COLOR;
}