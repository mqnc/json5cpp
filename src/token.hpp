
#pragma once

#include <variant>
#include "jsonvalue.hpp"

namespace JSON5{

enum class TokenType{
    boolean,
    eof,
    identifier,
    null,
    numeric,
    punctuator,
    string,
    incomplete
};

struct Token{
    TokenType type;
    std::variant<Value, char32_t> value;
    size_t line;
    size_t column;
};

Token incomplete {TokenType::incomplete, null, 0, 0};

}
