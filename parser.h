#ifndef EXPRESSIONPARSER_PARSER_H
#define EXPRESSIONPARSER_PARSER_H
#include <optional>
#include <string>

enum class TokenType {
    Number,
    Plus,
    Minus,
    UnaryMinus,
    Star,
    Slash,
    Cheveron,
    LeftParen,
    RightParen
};

struct Token {
    TokenType tokenType{};
    std::optional<double> value{};
};

double parseExpression(const std::string &expression);

#endif //EXPRESSIONPARSER_PARSER_H
