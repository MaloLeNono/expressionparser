#include <iostream>
#include <string>
#include "parser.h"

int main() {
    std::cout << "Enter the expression to evaluate\n";
    std::string expression{};
    std::getline(std::cin, expression);

    try {
        std::cout << parseExpression(expression) << '\n';
    } catch (std::exception& e) {
        std::cerr << "There was an error parsing: " << e.what() << '\n';
        return 1;
    }

    return 0;
}