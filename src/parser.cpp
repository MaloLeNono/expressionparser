#include "parser.h"
#include <charconv>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <vector>

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

std::vector<Token> tokenize(const std::string& expression) {
    std::vector<Token> tokens{};

    size_t i{};
    while (i < expression.length()) {
        const char c{expression[i]};

        if (std::isspace(static_cast<unsigned char>(c))) {
            i++;
            continue;
        }

        switch (c) {
            case '+':
                tokens.emplace_back(TokenType::Plus);
                i++;
                break;
            case '-': {
                const bool isUnary{tokens.empty() ||
                    (tokens.back().tokenType != TokenType::RightParen &&
                    tokens.back().tokenType != TokenType::Number)};
                tokens.emplace_back(isUnary ? TokenType::UnaryMinus : TokenType::Minus);
                i++;
                break;
            }
            case '*':
                tokens.emplace_back(TokenType::Star);
                i++;
                break;
            case '/':
                tokens.emplace_back(TokenType::Slash);
                i++;
                break;
            case '^':
                tokens.emplace_back(TokenType::Cheveron);
                i++;
                break;
            case '(':
                tokens.emplace_back(TokenType::LeftParen);
                i++;
                break;
            case ')':
                tokens.emplace_back(TokenType::RightParen);
                i++;
                break;
            default:
                if (std::isdigit(static_cast<unsigned char>(c))) {
                    double value{};
                    auto [ptr, ec]{std::from_chars(expression.data() + i, expression.data() + expression.size(),value)};
                    if (ec != std::errc{})
                        throw std::invalid_argument("Invalid number");
                    tokens.emplace_back(TokenType::Number, value);
                    i = ptr - expression.data();
                } else
                    throw std::invalid_argument("Invalid token");
        }
    }

    return tokens;
}

std::vector<Token> shuntingYard(const std::vector<Token>& tokens) {
    std::vector<Token> outputQueue{};
    std::vector<Token> operatorStack;
    std::unordered_map<TokenType, int> precedenceMap;
    precedenceMap[TokenType::UnaryMinus] = 5;
    precedenceMap[TokenType::Cheveron] = 4;
    precedenceMap[TokenType::Star] = 3;
    precedenceMap[TokenType::Slash] = 3;
    precedenceMap[TokenType::Plus] = 2;
    precedenceMap[TokenType::Minus] = 2;

    for (Token token : tokens) {
        if (token.tokenType == TokenType::Number)
            outputQueue.push_back(token);
        else if (token.tokenType == TokenType::LeftParen)
            operatorStack.push_back(token);
        else if (token.tokenType == TokenType::RightParen) {
            while (!operatorStack.empty() && operatorStack.back().tokenType != TokenType::LeftParen) {
                outputQueue.push_back(operatorStack.back());
                operatorStack.pop_back();
            }

            if (operatorStack.empty())
                throw std::invalid_argument("Mismatched right parenthesis");

            operatorStack.pop_back();
        }
        else {
            const bool isRightAssociative{token.tokenType == TokenType::UnaryMinus || token.tokenType == TokenType::Cheveron};

            while (!operatorStack.empty() &&
                   operatorStack.back().tokenType != TokenType::LeftParen &&
                   (precedenceMap[operatorStack.back().tokenType] > precedenceMap[token.tokenType] ||
                   (precedenceMap[operatorStack.back().tokenType] == precedenceMap[token.tokenType] && !isRightAssociative))) {
                outputQueue.push_back(operatorStack.back());
                operatorStack.pop_back();
            }

            operatorStack.push_back(token);
        }
    }

    while (!operatorStack.empty()) {
        if (operatorStack.back().tokenType == TokenType::LeftParen)
            throw std::invalid_argument("Mismatched left parenthesis");

        outputQueue.push_back(operatorStack.back());
        operatorStack.pop_back();
    }

    return outputQueue;
}

double parseTokens(const std::vector<Token>& tokens) {
    const std::vector outputQueue{shuntingYard(tokens)};
    std::vector<Token> stack{};

    for (size_t i{}; i < outputQueue.size(); ++i) {
        if (outputQueue[i].tokenType == TokenType::Number)
            stack.push_back(outputQueue[i]);
        else if (outputQueue[i].tokenType == TokenType::UnaryMinus) {
            const double a{stack.back().value.value()};
            stack.pop_back();
            stack.emplace_back(TokenType::Number, -a);
        }
        else {
            const double a{stack.back().value.value()};
            stack.pop_back();
            const double b{stack.back().value.value()};
            stack.pop_back();

            double c{};
            switch (outputQueue[i].tokenType) {
                case TokenType::Plus:
                    c = b + a;
                    break;
                case TokenType::Minus:
                    c = b - a;
                    break;
                case TokenType::Star:
                    c = b * a;
                    break;
                case TokenType::Slash:
                    if (a == 0)
                        throw std::runtime_error("Division by zero is not allowed");
                    c = b / a;
                    break;
                case TokenType::Cheveron:
                    c = std::pow(b, a);
                    break;
                default: ;
            }
            stack.emplace_back(TokenType::Number, c);
        }
    }

    if (stack.size() > 1 || stack.empty())
        throw std::invalid_argument("Invalid expression");

    return stack[0].value.value();
}

double parseExpression(const std::string &expression) {
    const std::vector tokens{tokenize(expression)};
    return parseTokens(tokens);
}
