#include "0-driver.h"
#include "0-color.h"
#include "util.h"
#include <sstream>

using namespace Limc;

Driver::Driver()
    : tokens(), scanner(*this), parser(scanner, *this), analyzer(*this), ir_maker(*this),
      loc(location()), input_file(), funcs_ir() {}

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

    assert(tokens.size() == 1);
    auto &token = tokens[0];

    analyzer.stmt(token);
    print_reports();
}

void Driver::gen_ir() {
    cout << BOLD_GREEN << "Generating IR..." << RESET_COLOR << endl;

    assert(tokens.size() == 1);
    auto &token = tokens[0];
    funcs_ir    = ir_maker.gen_ir(token);
    cout << BOLD_GREEN << "Finish!" << RESET_COLOR << endl;
}

string Driver::print() const {
    stringstream s;

    assert(tokens.size() == 1);
    auto &token = tokens[0];
    
    s << token.print() << endl;
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

        ss << BOLD_RED << "error: "
           << "[Limc - " << report.get_errno() << "]" << endl;
        ss << RESET_COLOR << msg << endl;
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
