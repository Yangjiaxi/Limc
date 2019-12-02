#include "0-driver.h"
#include "0-color.h"
#include <iomanip>
#include <sstream>

using namespace Limc;

Driver::Driver()
    : tokens(), scanner(*this), parser(scanner, *this), analyzer(*this), loc(location()),
      input_file() {}

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

static stringstream &
build_str(stringstream &ss, const string &value, const char *color, const unsigned width = 0) {
    if (width)
        ss << color << setw(width) << value << RESET_COLOR;
    else
        ss << color << value << RESET_COLOR;
    return ss;
}

static stringstream &build_str(stringstream &ss, const string &value, const unsigned width = 0) {
    if (width)
        ss << setw(width) << value << RESET_COLOR;
    else
        ss << value << RESET_COLOR;
    return ss;
}

void table_head() {
    stringstream ss;

    build_str(ss, " | ", RED);
    build_str(ss, "Name", BLUE, 7);

    build_str(ss, " | ", RED);
    build_str(ss, "Scope", YELLOW, 5);

    build_str(ss, " | ", RED);
    build_str(ss, "Alias", BOLD_YELLOW, 10);

    build_str(ss, " | ", RED);
    build_str(ss, "Type", GREEN, 15);

    build_str(ss, " | ", RED);
    build_str(ss, "Func?", MAGENTA, 8);

    build_str(ss, " | ", RED);
    build_str(ss, "Params", CYAN);

    cout << ss.str() << endl;
    cout << string(100, '-') << endl;
}

void Driver::analyze() {
    errors.str("");
    string line(50, '-');
    table_head();
    for (auto &token : tokens) {
        analyzer.walk(token);
        cout << line << endl << "Errors: " << endl << errors.str() << endl;
    }
    cout << BOLD_YELLOW << "Analyze Finish." << RESET_COLOR << endl;
}

string Driver::print() const {
    stringstream s;
    for (const auto &token : tokens) {
        s << token.print() << endl;
    }
    return s.str();
}

void Driver::set_entry(istream *is) {
    loc.initialize();
    istream *file_is = is;
    string   s;
    while (!is->eof()) {
        std::getline(*file_is, s);
        input_file.push_back(s);
    }
    is->clear();
    is->seekg(0);
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

    ss << BOLD_RED << "semantic error: " << RESET_COLOR << msg << endl;
    ss << BLUE << "  -->" << RESET_COLOR << " : " << YELLOW << loc << RESET_COLOR << endl;

    auto &[begin, end] = loc;
    if (begin.line == end.line) {
        ss << CYAN << setw(4) << begin.line << RESET_COLOR << " | " << input_file[begin.line - 1]
           << endl;
        ss << "     | " << string(begin.column - 1, ' ') << BOLD_RED
           << string(end.column - begin.column, '^') << RESET_COLOR << endl;
    }
    ss << BOLD_MAGENTA << line << RESET_COLOR << endl;
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