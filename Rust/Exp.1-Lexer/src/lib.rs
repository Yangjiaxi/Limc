pub mod preprocess;
pub mod token;

#[macro_use]
extern crate lazy_static;

// Token type
#[derive(Debug, PartialEq, Clone)]
pub enum TokenType {
    Num(i32),            // Number literal
    Str(String, usize),  // String literal. (str, len)
    CharLiteral(String), // Char literal.
    Ident(String),       // Identifier
    Param(usize),        // Function-like macro parameter
    Arrow,               // ->
    Extern,              // "extern"
    Typedef,             // "typedef"
    Int,                 // "int"
    Char,                // "char"
    Void,                // "void"
    Struct,              // "struct"
    Plus,                // +
    Minus,               // -
    Mul,                 // *
    Div,                 // /
    And,                 // &
    Dot,                 // .
    Comma,               // ,
    Exclamation,         // !
    Question,            // ?
    VerticalBar,         // |
    Hat,                 // ^
    Colon,               // :
    HashMark,            // #
    If,                  // "if"
    Else,                // "else"
    For,                 // "for"
    Do,                  // "do"
    While,               // "while"
    Break,               // "break"
    EQ,                  // ==
    NE,                  // !=
    LE,                  // <=
    GE,                  // >=
    Semicolon,           // ;
    LeftParen,           // (
    RightParen,          // )
    LeftBracket,         // [
    RightBracket,        // ]
    LeftBrace,           // {
    RightBrace,          // }
    LeftAngleBracket,    // <
    RightAngleBracket,   // >
    Equal,               // =
    LogOr,               // ||
    LogAnd,              // &&
    Shl,                 // <<
    Inc,                 // ++
    Dec,                 // --
    MulEQ,               // *=
    DivEQ,               // /=
    ModEQ,               // %=
    AddEQ,               // +=
    SubEQ,               // -=
    ShlEQ,               // <<=
    ShrEQ,               // >>=
    BitandEQ,            // &=
    XorEQ,               // ^=
    BitorEQ,             // |=
    Shr,                 // >>
    Mod,                 // %
    Return,              // "return"
    Sizeof,              // "sizeof"
    Alignof,             // "_Alignof"
    NewLine,             // preprocessor-only token
}

// Character Kind
#[derive(Debug, PartialEq)]
pub enum CharacterType {
    Whitespace, // ' '
    NewLine,    // ' \n'
    Alphabetic,
    Digit,
    NonAlphabetic(char),
    Unknown(char),
}

impl TokenType {
    fn new_single_letter(c: char) -> Option<Self> {
        use self::TokenType::*;
        match c {
            '+' => Some(Plus),
            '-' => Some(Minus),
            '*' => Some(Mul),
            '/' => Some(Div),
            '&' => Some(And),
            ';' => Some(Semicolon),
            '=' => Some(Equal),
            '(' => Some(LeftParen),
            ')' => Some(RightParen),
            '[' => Some(LeftBracket),
            ']' => Some(RightBracket),
            '{' => Some(LeftBrace),
            '}' => Some(RightBrace),
            '<' => Some(LeftAngleBracket),
            '>' => Some(RightAngleBracket),
            ',' => Some(Comma),
            '.' => Some(Dot),
            '!' => Some(Exclamation),
            '?' => Some(Question),
            '|' => Some(VerticalBar),
            '^' => Some(Hat),
            '%' => Some(Mod),
            ':' => Some(Colon),
            '#' => Some(HashMark),
            _ => None,
        }
    }
}
