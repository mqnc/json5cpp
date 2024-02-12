import re
import hashlib

def replaceFirstAfter(string, start, find, replace):
    start_index = string.find(start)
    if start_index == -1:
        return string
    find_index = string.find(find, start_index + len(start))
    if find_index == -1:
        return string
    replaced_string = string[:find_index] + replace + string[find_index + len(find):]
    return replaced_string

def replaceWithin(string, start, end, find, replace):
    start_index = string.find(start)
    end_index = string.find(end, start_index + len(start))
    
    if start_index != -1 and end_index != -1:
        substring_between = string[start_index + len(start):end_index]
        replaced_substring = substring_between.replace(find, replace)
        modified_string = (
            string[:start_index + len(start)] + 
            replaced_substring + 
            string[end_index:]
        )
        return modified_string
    
    return string

def replaceFromTo(string, start, end, replace):
    start_index = string.find(start)
    end_index = string.find(end, start_index + len(start))
    
    if start_index != -1 and end_index != -1:
        return (
            string[:start_index] + 
            replace + 
            string[end_index + len(end):]
        )
    
    return string

with open("js_reference/lib/parse.js", "r") as f:
	js = f.read()

beginning = js.split("const lexStates = {", 1)[0]
beginning_hash = hashlib.sha256(beginning.encode()).hexdigest()
js = js[len(beginning):]

if(beginning_hash != "5faf9754d998da50dd8a207108920134ae58232c7f3202f8cf7223023b812064"):
	raise RuntimeError("reference implementation has changed;"
        + " adapt C++ implementation at the end of the transpiler and include"
		+ " the new hash: " + beginning_hash)



states=[]
insideLexer = False

lines = js.split("\n")
for line in lines:
	# match = re.search(r'function (\w+) \((.*?)\)', line)
	# if match:
	# 	forwardDeclarations.append(f"auto {match.group(1)} ({match.group(2)});")
	
	if line == "const lexStates = {":
		insideLexer = True

	if insideLexer:
		match = re.search(r'    (\w+) \(\) {', line)
		if match:
			states.append(match.group(1).replace("default", "deflt"))

		if line.startswith("}"):
			insideLexer = False

tokens = sorted(set(re.findall(r"newToken\('(\w+)'", js)))
tokens.append("incomplete")

cpp = js

cpp = replaceFromTo(cpp, "function peek", "\n}",
"""char32_t peek () {
	if (pos < source.size()) {
		return source[pos]; // TODO UTF-8
	}
	return EndOfInput;
}""")

cpp = replaceFromTo(cpp, "function formatChar", "\n}", "$FORMATCHAR")

cpp = replaceWithin(
	cpp, "const lexStates = {", "\n}\n",
	"return\n", "return incomplete\n")

cpp = replaceWithin(
	cpp, "const lexStates = {", "\n}\n",
	"\n    },\n", "\n    } break\n")
cpp = replaceWithin(
	cpp, "const parseStates = {", "\n}\n",
	"\n    },\n", "\n    } break\n")

cpp = replaceFirstAfter(
	cpp, "const lexStates = {", "\n}\n", "\n  }\n\n  return incomplete\n}\n")
cpp = replaceFirstAfter(
	cpp, "const parseStates = {", "\n}\n", "\n  }\n}\n")

cpp = ( cpp
	.replace("const lexStates = {", 
		"Token consumeCharacter (State s) {\n  switch (s) {") \
	.replace("const parseStates = {",
		"void consumeToken (State s) {\n  switch (s) {") \
	.replace("function newToken (type, value) ",
		"Token newToken (TokenType type, std::variant<Value, char32_t> value = null) ") \
	.replace("function literal (s) {\n    for (const c of s) {",
		"void literal ($CONST std::string& s) {\n    for ($CONST auto& c: s) { // TODO UTF-8")
	.replace("function hexEscape", "char32_t hexEscape")
	.replace("function invalid", "std::runtime_error invalid")
	.replace("invalidChar (c)", "invalidChar (char32_t c)")
	.replace("function unicodeEscape", "char32_t unicodeEscape")
	.replace("function escape", "std::optional<char32_t> escape")
	.replace("return ''", "return std::nullopt")
	.replace("function separatorChar (c)", "void separatorChar (char32_t c)")
	.replace(
"""function syntaxError (message) {
    const err = new SyntaxError(message)
    err.lineNumber = line
    err.columnNumber = column
    return err
}""",
"""std::runtime_error syntaxError($CONST std::string& message) {
	return std::runtime_error(std::to_string(line) + ":" + std::to_string(column) + ": " + message)
}""")
	.replace("console.warn", "std::cerr << ")
	.replace("util.", "util::")
	.replace("parseInt(buffer, 16)", "std::stoi(buffer, 0, 16)")
	.replace("Number(", "std::stod(")
	.replace("sign * 0", "sign * 0.0")
	.replace("buffer += escape()", '{ auto esc = escape(); if (esc) { buffer += *esc; } }')
	.replace(".length", ".size()")
	.replace("let value", "Value value")
	.replace("switch (token.value)", "switch (std::get<char32_t>(token.value))")
	.replace("switch (token.type) {", "switch (token.type) { default: break")
	.replace("value = []", "value = Array {}")
	.replace("value = {}", "value = Object {}")
	.replace("value = token.value", "value = std::get<Value>(token.value)")
)

cpp = replaceFromTo(cpp, "    if (root === undefined)", "\n    }\n",
"""	if (stack.size() == 0) {
		root = value
	} else {
		Value& parent = stack.top()
		if (std::holds_alternative<Array>(parent)) {
			std::get<Array>(parent).push_back(value)
		} else {
			auto result = std::get<Object>(parent).insert({key, value})
			if (!result.second) {
				throw std::runtime_error("Key " + key + " already exists in object")
			}
		}
	}
"""
)

cpp = re.sub(r"lexStates\[(\w+)\]\(\)", r"consumeCharacter(\1)", cpp)
cpp = re.sub(r"function (\w+) \(c\)", r"auto \1 (char32_t c)", cpp)
cpp = re.sub(r"    (\w+) \(\) \{", r"    case State::\1: {", cpp)
cpp = re.sub(r"(\w+)State = '(\w+)'", r"\1State = State::\2", cpp)
cpp = re.sub(r"\$\{(.*?)\}", r'" + std::to_string(\1) + "', cpp)
cpp = re.sub(r"Array.isArray\((\w+)\)", r"std::holds_alternative<Array>(\1)", cpp)
cpp = ( cpp
	.replace("std::to_string(formatChar(c))", "formatChar(c)")
	.replace("function push", "void push")
	.replace("function pop", "void pop")
	.replace("value !== null && typeof value === 'object'",
		  "(std::holds_alternative<Object>(value) || std::holds_alternative<Array>(value))")
	.replace("current == null", "std::holds_alternative<Null>(current)")
	.replace("stack[stack.size() - 1]", "stack.top()")
	.replace("const current", "$CONST Value& current")
	.replace("key = token.value", "key = std::get<std::string>(std::get<Value>(token.value))")
	.replace("token.value === ']'", "std::get<char32_t>(token.value) == ']'")
	.replace("c === undefined", "c == EndOfInput")
)

for token in tokens:
	cpp = cpp.replace(f"'{token}'", f"TokenType::{token}")

cpp = (cpp
	.replace("const ", "const auto ")
	.replace("let ", "auto ")
	.replace("function ", "auto ")
	.replace("===", "==")
	.replace("!==", "!=")
	.replace("Infinity", "std::numeric_limits<double>::infinity()")
	.replace("String.fromCodePoint(", "char32_t(")
	.replace("NaN", 'std::nan("")')
	.replace("''", '""')
	.replace('''"'"''', "'\\''")
	.replace("`", '"')
	.replace("State::default", "State::deflt")
	.replace("case undefined", "case EndOfInput")
	.replace("    },", "    }")
	.replace("\n", ";\n")
	.replace("\n;\n", "\n\n")
	.replace(":;", ":")
	.replace("{;", "{")
	.replace(",;", ",")
	.replace("$CONST", "const")
	.replace("$FORMATCHAR",
r"""std::string formatChar(char32_t c) {
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

	return std::to_string(c); // TODO should be an utf8 character, no number
}""")
)
cpp = re.sub(r"'(\w\w+)'", r'"\1"', cpp)
cpp = re.sub(r"'\\u(\w\w\w\w)'", r"0x\1", cpp)

sep = ",\n\t"
cpp = """

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

struct Null {};
const Null null;

struct Array;
struct Object;

using Value = std::variant<
	Null,
	bool,
	double,
	std::string,
	Array,
	Object
	>;

struct Array: public std::vector<Value> {
	using std::vector<Value>::vector;
};

struct Object: public std::map<std::string, Value> {
	using std::map<std::string, Value>::map;
};

""" \
	+ f"enum class State{{\n\t{sep.join(states)}\n}};\n\n" \
	+ f"enum class TokenType{{\n\t{sep.join(tokens)}\n}};" \
	+ """

const char32_t EndOfInput = char32_t(-1);

struct Token{
    TokenType type;
    std::variant<Value, char32_t> value;
    size_t line;
	size_t column;
};

Token incomplete {TokenType::incomplete, null, 0, 0};

class Parser{

std::string_view source;
State parseState;
std::stack<Value> stack;
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
std::string buffer;
bool doubleQuote;
int sign;
char32_t c;

Token lex () {
    lexState = State::deflt;
    buffer = "";
    doubleQuote = false;
    sign = 1;

    for (;;) {
        c = peek();

        // This code is unreachable.;
        // if (!lexStates[lexState]) {
        //     throw invalidLexState(lexState);
        // };

        const auto token = consumeCharacter(lexState);
        if (token.type != TokenType::incomplete) {
            return token;
        }
    }
}

struct Peek {char32_t c; uint32_t n;};
Peek peek () {
	auto result = peekUTF8(source.substr(pos));
	switch(result.status){
		case UTF8Peek::Status::ok: return {result.codepoint, result.bytesRead};
		case UTF8Peek::Status::endOfString: return {EndOfInput, 0};
		case UTF8Peek::Status::invalid: throw std::runtime_error("invalid UTF-8 byte sequence");
	}
}

char32_t read () {
    const auto [c, n] = peek();

    if (c == '\\n') {
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

""" + cpp + "\n\n};\n"

cpp = replaceFromTo(cpp, "void push ()", "\n};\n",
"""void push () {
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
        Value& parent = stack.top();
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
        stack.push(*value);

        if (std::holds_alternative<Array>(*value)) {
            parseState = State::beforeArrayValue;
        } else {
            parseState = State::beforePropertyName;
        };
    } else {
        const Value& current = stack.top();
        if (std::holds_alternative<Null>(current)) {
            parseState = State::end;
        } else if (std::holds_alternative<Array>(current)) {
            parseState = State::afterArrayValue;
        } else {
            parseState = State::afterPropertyValue;
        };
    };
};
""")

cpp = replaceFromTo(cpp, "void pop ()", "\n};\n",
"""void pop () {
    stack.pop();

    if (stack.size() == 0){
        parseState = State::end;
    }
    else{
        const Value& current = stack.top();
        if (std::holds_alternative<Array>(current)) {
            parseState = State::afterArrayValue;
        } else {
            parseState = State::afterPropertyValue;
        }
    };
};
""")

with open("tools/parse_.cpp", "w") as f:
	f.write(cpp)