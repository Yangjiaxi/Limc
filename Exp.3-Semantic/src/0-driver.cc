#include "0-driver.h"
#include "0-color.h"
#include "util.h"
#include <sstream>

using namespace Limc;

Driver::Driver()
    : tokens(), scanner(*this), parser(scanner, *this), analyzer(*this), loc(location()),
      input_file() {}

Token &Driver::get_root() { return tokens.at(0); }

int Driver::parse() {
    reports.clear();
    loc.initialize();
    auto res = parser.parse();
    print_reports();
    return res;
}

void Driver::clear() {
    loc.initialize();
    tokens.clear();
}

void Driver::analyze() {
    reports.clear();
    string line(50, '-');
    for (auto &token : tokens) {
        analyzer.stmt(token);
        print_reports();
    }
    // cout << BOLD_YELLOW << "Analyze Finish." << RESET_COLOR << endl;
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

Report &Driver::report() {
    reports.emplace_back();
    return reports.back();
}

void Driver::print_reports() {
    stringstream ss;
    string       line(50, '-');
    for (auto &report : reports) {
        auto &msg   = report.get_msg();
        auto  level = report.get_level();
        auto &loc   = report.get_loc();

        ss << BOLD_RED << "error: " << RESET_COLOR << msg << endl;
        ss << BLUE << "  -->" << RESET_COLOR << " : " << YELLOW << loc[0] << RESET_COLOR << endl;

        auto &[begin, end] = loc[0];
        if (begin.line == end.line) {
            ss << CYAN << setw(4) << begin.line << RESET_COLOR << " | "
               << input_file[begin.line - 1] << endl;
            ss << "     | " << string(begin.column - 1, ' ') << BOLD_RED
               << string(end.column - begin.column, '^') << RESET_COLOR << endl;
        }
        ss << BOLD_MAGENTA << line << RESET_COLOR << endl;
    }
    cout << ss.str();
}

// string Driver::gen_error(const string &msg, const location &loc) {
//     stringstream ss;
//     string       line(50, '-');

//     ss << BOLD_RED << "error: " << RESET_COLOR << msg << endl;
//     ss << BLUE << "  -->" << RESET_COLOR << " : " << YELLOW << loc << RESET_COLOR << endl;

//     auto &[begin, end] = loc;
//     if (begin.line == end.line) {
//         ss << CYAN << setw(4) << begin.line << RESET_COLOR << " | " << input_file[begin.line - 1]
//            << endl;
//         ss << "     | " << string(begin.column - 1, ' ') << BOLD_RED
//            << string(end.column - begin.column, '^') << RESET_COLOR << endl;
//     }
//     ss << BOLD_MAGENTA << line << RESET_COLOR << endl;
//     return ss.str();
// }

// void Driver::add_error(const string &msg, const Token &token) {
//     add_error(msg, token.get_loc().value());
// }

// void Driver::add_error(const string &msg, const location &loc) { errors << gen_error(msg, loc); }

// void Driver::add_error(const string &msg) {
//     // enhence later
//     errors << YELLOW << msg << RESET_COLOR;
// }