#ifndef SCANNER_H
#define SCANNER_H

#include "parser.hpp"

#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer Limc_FlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL Limc::Parser::symbol_type Limc::Scanner::get_next_token()

namespace Limc
{
class Scanner : public yyFlexLexer
{
   public:
    explicit Scanner(Driver& driver) : driver(driver) {}

    ~Scanner() override = default;

    virtual Limc::Parser::symbol_type get_next_token();

   private:
    Driver& driver;
};
}  // namespace Limc

#endif  // SCANNER_H
