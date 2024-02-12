

#include <string_view>
#include <stack>
#include <cmath>
#include <limits>
#include <variant>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <optional>
#include <cstdio>

#include "utf8.hpp"
#include "util.hpp"
#include "jsonvalue.hpp"
#include "parserstate.hpp"
#include "token.hpp"

namespace JSON5{

const char32_t EndOfInput = char32_t(-1);

class EncodingBuffer{
    std::string data{};
public:
    EncodingBuffer& operator=(char32_t c) {
        data = toUTF8(c);
        return *this;
    }
    EncodingBuffer& operator+=(char32_t c) {
        data += toUTF8(c);
        return *this;
    }
    const std::string& str(){ return data; }
    void clear() { data.clear(); }
};

class Parser{

std::string_view source;
State parseState;
std::stack<Value*> stack;
size_t pos;
size_t line;
size_t column;
Token token;
std::string key;
Value root;

public:
Value parse (std::string_view text) {
    source = text;
    parseState = State::start;
    stack = {};
    pos = 0;
    line = 1;
    column = 0;
    token = incomplete;
    key = "";
    root = null;

    do {
        token = lex();

        // This code is unreachable.
        // if (!parseStates[parseState]) {
        //     throw invalidParseState()
        // }

        consumeToken(parseState);
    } while (token.type != TokenType::eof);

    // if (typeof reviver === 'function') {
    //     return internalize({'': root}, '', reviver)
    // }

    return root;
}
private:

State lexState;
EncodingBuffer buffer;
bool doubleQuote;
int sign;
char32_t c;

Token lex () {
    lexState = State::deflt;
    buffer.clear();
    doubleQuote = false;
    sign = 1;

    for (;;) {
        c = peek().c;

        // This code is unreachable.;
        // if (!lexStates[lexState]) {
        //     throw invalidLexState(lexState);
        // };

        const Token token = consumeCharacter(lexState);
        if (token.type != TokenType::incomplete) {
            return token;
        }
    }
}

struct Peek {char32_t c; uint32_t n;};
Peek peek () {
    const UTF8Peek result = peekUTF8(source.substr(pos));
    switch(result.status){
        case UTF8Peek::Status::ok: return {result.codepoint, result.bytesRead};
        case UTF8Peek::Status::endOfString: return {EndOfInput, 0};
        case UTF8Peek::Status::invalid: throw std::runtime_error("invalid UTF-8 byte sequence");
    }
}

char32_t read () {
    const auto [c, n] = peek();

    if (c == '\n') {
        line++;
        column = 0;
    } else if (c != EndOfInput) {
        column += 1;
    } else {
        column++;
    };

    if (c != EndOfInput) {
        pos += n;
    };

    return c;
};

Token consumeCharacter (State s) {
  switch (s) {
    case State::deflt: {
        switch (c) {
        case '\t':
        case '\v':
        case '\f':
        case ' ':
        case 0x00A0:
        case 0xFEFF:
        case '\n':
        case '\r':
        case 0x2028:
        case 0x2029:
            read();
            return incomplete;

        case '/':
            read();
            lexState = State::comment;
            return incomplete;

        case EndOfInput:
            read();
            return newToken(TokenType::eof);
        };

        if (util::isSpaceSeparator(c)) {
            read();
            return incomplete;
        };

        // This code is unreachable.;
        // if (!lexStates[parseState]) {
        //     throw invalidLexState(parseState);
        // };

        return consumeCharacter(parseState);
    } break;

    case State::comment: {
        switch (c) {
        case '*':
            read();
            lexState = State::multiLineComment;
            return incomplete;

        case '/':
            read();
            lexState = State::singleLineComment;
            return incomplete;
        };

        throw invalidChar(read());
    } break;

    case State::multiLineComment: {
        switch (c) {
        case '*':
            read();
            lexState = State::multiLineCommentAsterisk;
            return incomplete;

        case EndOfInput:
            throw invalidChar(read());
        };

        read();
    } break;

    case State::multiLineCommentAsterisk: {
        switch (c) {
        case '*':
            read();
            return incomplete;

        case '/':
            read();
            lexState = State::deflt;
            return incomplete;

        case EndOfInput:
            throw invalidChar(read());
        };

        read();
        lexState = State::multiLineComment;
    } break;

    case State::singleLineComment: {
        switch (c) {
        case '\n':
        case '\r':
        case 0x2028:
        case 0x2029:
            read();
            lexState = State::deflt;
            return incomplete;

        case EndOfInput:
            read();
            return newToken(TokenType::eof);
        };

        read();
    } break;

    case State::value: {
        switch (c) {
        case '{':
        case '[':
            return newToken(TokenType::punctuator, read());

        case 'n':
            read();
            literal("ull");
            return newToken(TokenType::null, null);

        case 't':
            read();
            literal("rue");
            return newToken(TokenType::boolean, true);

        case 'f':
            read();
            literal("alse");
            return newToken(TokenType::boolean, false);

        case '-':
        case '+':
            if (read() == '-') {
                sign = -1;
            };

            lexState = State::sign;
            return incomplete;

        case '.':
            buffer = read();
            lexState = State::decimalPointLeading;
            return incomplete;

        case '0':
            buffer = read();
            lexState = State::zero;
            return incomplete;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            buffer = read();
            lexState = State::decimalInteger;
            return incomplete;

        case 'I':
            read();
            literal("nfinity");
            return newToken(TokenType::numeric, std::numeric_limits<double>::infinity());

        case 'N':
            read();
            literal("aN");
            return newToken(TokenType::numeric, std::nan(""));

        case '"':
        case '\'':
            doubleQuote = (read() == '"');
            buffer.clear();
            lexState = State::string;
            return incomplete;
        };

        throw invalidChar(read());
    } break;

    case State::identifierNameStartEscape: {
        if (c != 'u') {
            throw invalidChar(read());
        };

        read();
        const char32_t u = unicodeEscape();
        switch (u) {
        case '$':
        case '_':
            break;

        default:
            if (!util::isIdStartChar(u)) {
                throw invalidIdentifier();
            };

            break;
        };

        buffer += u;
        lexState = State::identifierName;
    } break;

    case State::identifierName: {
        switch (c) {
        case '$':
        case '_':
        case 0x200C:
        case 0x200D:
            buffer += read();
            return incomplete;

        case '\\':
            read();
            lexState = State::identifierNameEscape;
            return incomplete;
        };

        if (util::isIdContinueChar(c)) {
            buffer += read();
            return incomplete;
        };

        return newToken(TokenType::identifier, buffer.str());
    } break;

    case State::identifierNameEscape: {
        if (c != 'u') {
            throw invalidChar(read());
        };

        read();
        const char32_t u = unicodeEscape();
        switch (u) {
        case '$':
        case '_':
        case 0x200C:
        case 0x200D:
            break;

        default:
            if (!util::isIdContinueChar(u)) {
                throw invalidIdentifier();
            };

            break;
        };

        buffer += u;
        lexState = State::identifierName;
    } break;

    case State::sign: {
        switch (c) {
        case '.':
            buffer = read();
            lexState = State::decimalPointLeading;
            return incomplete;

        case '0':
            buffer = read();
            lexState = State::zero;
            return incomplete;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            buffer = read();
            lexState = State::decimalInteger;
            return incomplete;

        case 'I':
            read();
            literal("nfinity");
            return newToken(TokenType::numeric, sign * std::numeric_limits<double>::infinity());

        case 'N':
            read();
            literal("aN");
            return newToken(TokenType::numeric, std::nan(""));
        };

        throw invalidChar(read());
    } break;

    case State::zero: {
        switch (c) {
        case '.':
            buffer += read();
            lexState = State::decimalPoint;
            return incomplete;

        case 'e':
        case 'E':
            buffer += read();
            lexState = State::decimalExponent;
            return incomplete;

        case 'x':
        case 'X':
            buffer += read();
            lexState = State::hexadecimal;
            return incomplete;
        };

        return newToken(TokenType::numeric, sign * 0.0);
    } break;

    case State::decimalInteger: {
        switch (c) {
        case '.':
            buffer += read();
            lexState = State::decimalPoint;
            return incomplete;

        case 'e':
        case 'E':
            buffer += read();
            lexState = State::decimalExponent;
            return incomplete;
        };

        if (util::isDigit(c)) {
            buffer += read();
            return incomplete;
        };

        return newToken(TokenType::numeric, sign * std::stod(buffer.str()));
    } break;

    case State::decimalPointLeading: {
        if (util::isDigit(c)) {
            buffer += read();
            lexState = State::decimalFraction;
            return incomplete;
        };

        throw invalidChar(read());
    } break;

    case State::decimalPoint: {
        switch (c) {
        case 'e':
        case 'E':
            buffer += read();
            lexState = State::decimalExponent;
            return incomplete;
        };

        if (util::isDigit(c)) {
            buffer += read();
            lexState = State::decimalFraction;
            return incomplete;
        };

        return newToken(TokenType::numeric, sign * std::stod(buffer.str()));
    } break;

    case State::decimalFraction: {
        switch (c) {
        case 'e':
        case 'E':
            buffer += read();
            lexState = State::decimalExponent;
            return incomplete;
        };

        if (util::isDigit(c)) {
            buffer += read();
            return incomplete;
        };

        return newToken(TokenType::numeric, sign * std::stod(buffer.str()));
    } break;

    case State::decimalExponent: {
        switch (c) {
        case '+':
        case '-':
            buffer += read();
            lexState = State::decimalExponentSign;
            return incomplete;
        };

        if (util::isDigit(c)) {
            buffer += read();
            lexState = State::decimalExponentInteger;
            return incomplete;
        };

        throw invalidChar(read());
    } break;

    case State::decimalExponentSign: {
        if (util::isDigit(c)) {
            buffer += read();
            lexState = State::decimalExponentInteger;
            return incomplete;
        };

        throw invalidChar(read());
    } break;

    case State::decimalExponentInteger: {
        if (util::isDigit(c)) {
            buffer += read();
            return incomplete;
        };

        return newToken(TokenType::numeric, sign * std::stod(buffer.str()));
    } break;

    case State::hexadecimal: {
        if (util::isHexDigit(c)) {
            buffer += read();
            lexState = State::hexadecimalInteger;
            return incomplete;
        };

        throw invalidChar(read());
    } break;

    case State::hexadecimalInteger: {
        if (util::isHexDigit(c)) {
            buffer += read();
            return incomplete;
        };

        return newToken(TokenType::numeric, sign * std::stod(buffer.str()));
    } break;

    case State::string: {
        switch (c) {
        case '\\':
            read();
            { auto esc = escape(); if (esc) { buffer += *esc; } };
            return incomplete;

        case '"':
            if (doubleQuote) {
                read();
                return newToken(TokenType::string, buffer.str());
            };

            buffer += read();
            return incomplete;

        case '\'':
            if (!doubleQuote) {
                read();
                return newToken(TokenType::string, buffer.str());
            };

            buffer += read();
            return incomplete;

        case '\n':
        case '\r':
            throw invalidChar(read());

        case 0x2028:
        case 0x2029:
            separatorChar(c);
            break;

        case EndOfInput:
            throw invalidChar(read());
        };

        buffer += read();
    } break;

    case State::start: {
        switch (c) {
        case '{':
        case '[':
            return newToken(TokenType::punctuator, read());

        // This code is unreachable since the default lexState handles eof.;
        // case EndOfInput:
        //     return newToken(TokenType::eof);
        };

        lexState = State::value;
    } break;

    case State::beforePropertyName: {
        switch (c) {
        case '$':
        case '_':
            buffer = read();
            lexState = State::identifierName;
            return incomplete;

        case '\\':
            read();
            lexState = State::identifierNameStartEscape;
            return incomplete;

        case '}':
            return newToken(TokenType::punctuator, read());

        case '"':
        case '\'':
            doubleQuote = (read() == '"');
            lexState = State::string;
            return incomplete;
        };

        if (util::isIdStartChar(c)) {
            buffer += read();
            lexState = State::identifierName;
            return incomplete;
        };

        throw invalidChar(read());
    } break;

    case State::afterPropertyName: {
        if (c == ':') {
            return newToken(TokenType::punctuator, read());
        };

        throw invalidChar(read());
    } break;

    case State::beforePropertyValue: {
        lexState = State::value;
    } break;

    case State::afterPropertyValue: {
        switch (c) {
        case ',':
        case '}':
            return newToken(TokenType::punctuator, read());
        };

        throw invalidChar(read());
    } break;

    case State::beforeArrayValue: {
        if (c == ']') {
            return newToken(TokenType::punctuator, read());
        };

        lexState = State::value;
    } break;

    case State::afterArrayValue: {
        switch (c) {
        case ',':
        case ']':
            return newToken(TokenType::punctuator, read());
        };

        throw invalidChar(read());
    } break;

    case State::end: {
        // This code is unreachable since it's handled by the default lexState.;
        // if (c == EndOfInput) {
        //     read();
        //     return newToken(TokenType::eof);
        // };

        throw invalidChar(read());
    };
  };

  return incomplete;
};

Token newToken (TokenType type, std::variant<Value, char32_t> value = null) {
    return {
        type,
        value,
        line,
        column,
    };
};

void literal (const std::string& s) {
    for (const char& c: s) {
        const char32_t p = peek().c;

        if (p != char32_t(c)) {
            throw invalidChar(read());
        };

        read();
    };
};

std::optional<char32_t> escape () {
    const char32_t c = peek().c;
    switch (c) {
    case 'b':
        read();
        return '\b';

    case 'f':
        read();
        return '\f';

    case 'n':
        read();
        return '\n';

    case 'r':
        read();
        return '\r';

    case 't':
        read();
        return '\t';

    case 'v':
        read();
        return '\v';

    case '0':
        read();
        if (util::isDigit(peek().c)) {
            throw invalidChar(read());
        };

        return '\0';

    case 'x':
        read();
        return hexEscape();

    case 'u':
        read();
        return unicodeEscape();

    case '\n':
    case 0x2028:
    case 0x2029:
        read();
        return std::nullopt;

    case '\r':
        read();
        if (peek().c == '\n') {
            read();
        };

        return std::nullopt;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        throw invalidChar(read());

    case EndOfInput:
        throw invalidChar(read());
    };

    return read();
};

char32_t hexEscape () {
    std::string buffer = "";
    char32_t c = peek().c;

    if (!util::isHexDigit(c)) {
        throw invalidChar(read());
    };

    buffer += read();

    c = peek().c;
    if (!util::isHexDigit(c)) {
        throw invalidChar(read());
    };

    buffer += read();

    return char32_t(std::stoi(buffer, 0, 16));
};

char32_t unicodeEscape () {
    std::string buffer = "";
    uint8_t count = 4;

    while (count-- > 0) {
        const char32_t c = peek().c;
        if (!util::isHexDigit(c)) {
            throw invalidChar(read());
        };

        buffer += read();
    };

    return char32_t(std::stoi(buffer, 0, 16));
};

void consumeToken (State s) {
  switch (s) {
    case State::start: {
        if (token.type == TokenType::eof) {
            throw invalidEOF();
        };

        push();
    } break;

    case State::beforePropertyName: {
        switch (token.type) {
        case TokenType::identifier:
        case TokenType::string:
            key = std::get<std::string>(std::get<Value>(token.value));
            parseState = State::afterPropertyName;
            return;

        case TokenType::punctuator:
            // This code is unreachable since it's handled by the lexState.;
            // if (token.value != '}') {
            //     throw invalidToken();
            // };

            pop();
            return;

        case TokenType::eof:
            throw invalidEOF();

        default: ;
        };

        // This code is unreachable since it's handled by the lexState.;
        // throw invalidToken();
    } break;

    case State::afterPropertyName: {
        // This code is unreachable since it's handled by the lexState.;
        // if (token.type != TokenType::punctuator || token.value != ':') {
        //     throw invalidToken();
        // };

        if (token.type == TokenType::eof) {
            throw invalidEOF();
        };

        parseState = State::beforePropertyValue;
    } break;

    case State::beforePropertyValue: {
        if (token.type == TokenType::eof) {
            throw invalidEOF();
        };

        push();
    } break;

    case State::beforeArrayValue: {
        if (token.type == TokenType::eof) {
            throw invalidEOF();
        };

        if (token.type == TokenType::punctuator && std::get<char32_t>(token.value) == ']') {
            pop();
            return;
        };

        push();
    } break;

    case State::afterPropertyValue: {
        // This code is unreachable since it's handled by the lexState.;
        // if (token.type != TokenType::punctuator) {
        //     throw invalidToken();
        // };

        if (token.type == TokenType::eof) {
            throw invalidEOF();
        };

        switch (std::get<char32_t>(token.value)) {
        case ',':
            parseState = State::beforePropertyName;
            return;

        case '}':
            pop();
        };

        // This code is unreachable since it's handled by the lexState.;
        // throw invalidToken();
    } break;

    case State::afterArrayValue: {
        // This code is unreachable since it's handled by the lexState.;
        // if (token.type != TokenType::punctuator) {
        //     throw invalidToken();
        // };

        if (token.type == TokenType::eof) {
            throw invalidEOF();
        };

        switch (std::get<char32_t>(token.value)) {
        case ',':
            parseState = State::beforeArrayValue;
            return;

        case ']':
            pop();
        };

        // This code is unreachable since it's handled by the lexState.;
        // throw invalidToken();
    } break;

    case State::end: {
        // This code is unreachable since it's handled by the lexState.;
        // if (token.type != TokenType::eof) {
        //     throw invalidToken();
        // };
    };

    default: ;
  };
};

void push () {
    Value temp;

    switch (token.type) {
    case TokenType::punctuator:
        switch (std::get<char32_t>(token.value)) {
        case '{':
            temp = Object {};
            break;

        case '[':
            temp = Array {};
            break;
        };

        break;

    case TokenType::null:
    case TokenType::boolean:
    case TokenType::numeric:
    case TokenType::string:
        temp = std::get<Value>(token.value);
        break;

    default:;
    // This code is unreachable.;
    // default:
    //     throw invalidToken();
    };

    Value* value;
    if (stack.size() == 0) {
        root = temp;
        value = &root;
    } else {
        Value& parent = *stack.top();
        if (std::holds_alternative<Array>(parent)) {
            std::get<Array>(parent).push_back(temp);
            value = &std::get<Array>(parent).back();
        } else {
            auto result = std::get<Object>(parent).insert({key, temp});
            if (!result.second) {
                throw std::runtime_error("Key " + key + " already exists in object");
            };
            value = &(result.first->second);
        };
    };

    if ((std::holds_alternative<Object>(*value) || std::holds_alternative<Array>(*value))) {
        stack.push(value);

        if (std::holds_alternative<Array>(*value)) {
            parseState = State::beforeArrayValue;
        } else {
            parseState = State::beforePropertyName;
        };
    } else {
        const Value& current = *stack.top();
        if (std::holds_alternative<Null>(current)) {
            parseState = State::end;
        } else if (std::holds_alternative<Array>(current)) {
            parseState = State::afterArrayValue;
        } else {
            parseState = State::afterPropertyValue;
        };
    };
};

void pop () {
    stack.pop();

    if (stack.size() == 0){
        parseState = State::end;
    }
    else{
        const Value& current = *stack.top();
        if (std::holds_alternative<Array>(current)) {
            parseState = State::afterArrayValue;
        } else {
            parseState = State::afterPropertyValue;
        }
    };
};

// This code is unreachable.;
// std::runtime_error invalidParseState () {
//     return new Error("JSON5: invalid parse state '" + std::to_string(parseState) + "'");
// };

// This code is unreachable.;
// std::runtime_error invalidLexState (state) {
//     return new Error("JSON5: invalid lex state '" + std::to_string(state) + "'");
// };

std::runtime_error invalidChar (char32_t c) {
    if (c == EndOfInput) {
        return syntaxError("JSON5: invalid end of input at " + std::to_string(line) + ":" + std::to_string(column) + "");
    };

    return syntaxError("JSON5: invalid character '" + formatChar(c) + "' at " + std::to_string(line) + ":" + std::to_string(column) + "");
};

std::runtime_error invalidEOF () {
    return syntaxError("JSON5: invalid end of input at " + std::to_string(line) + ":" + std::to_string(column) + "");
};

// This code is unreachable.;
// std::runtime_error invalidToken () {
//     if (token.type == TokenType::eof) {
//         return syntaxError("JSON5: invalid end of input at " + std::to_string(line) + ":" + std::to_string(column) + "");
//     };

//     const auto c = char32_t(token.value.codePointAt(0));
//     return syntaxError("JSON5: invalid character '" + formatChar(c) + "' at " + std::to_string(line) + ":" + std::to_string(column) + "");
// };

std::runtime_error invalidIdentifier () {
    column -= 5;
    return syntaxError("JSON5: invalid identifier character at " + std::to_string(line) + ":" + std::to_string(column) + "");
};

void separatorChar (char32_t c) {
    std::cerr << ("JSON5: '" + formatChar(c) + "' in strings is not valid ECMAScript; consider escaping");
};

std::string formatChar(char32_t c) {
    const std::map<char32_t, std::string> replacements = {
        {'\'', "\\'"},
        {'"', "\\\""},
        {'\\', "\\\\"},
        {'\b', "\\b"},
        {'\f', "\\f"},
        {'\n', "\\n"},
        {'\r', "\\r"},
        {'\t', "\\t"},
        {'\v', "\\v"},
        {'\0', "\\0"},
        {0x2028, "\\u2028"},
        {0x2029, "\\u2029"},
    };

    if (replacements.count(c) > 0) {
        return replacements.at(c);
    };

    if (c < ' ') {
        char hexString[5];
        std::sprintf(hexString, "\\x%02X", c);
        return hexString;
    };

    return toUTF8(c);
};

std::runtime_error syntaxError(const std::string& message) {
    return std::runtime_error(std::to_string(line) + ":" + std::to_string(column) + ": " + message);
};

};

Value parse(std::string_view text){
	return Parser{}.parse(text);
}

}