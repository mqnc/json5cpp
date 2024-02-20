
#pragma once

#include <string>
#include <iostream>
#include <sstream>

#include "test.h"
#include "parse.hpp"

void testParse() {

	expect(
		JSON5::parse("{}")
			== JSON5::Object {},
		"parses empty objects"
	);

	expect(
		JSON5::parse("{\"a\":1}")
			== JSON5::Object {{"a", 1.0}},
		"parses double string property names"
	);

	expect(
		JSON5::parse("{'a':1}")
			== JSON5::Object {{"a", 1.0}},
		"parses single string property names"
	);

	expect(
		JSON5::parse("{a:1}")
			== JSON5::Object {{"a", 1.0}},
		"parses unquoted property names"
	);

	expect(
		JSON5::parse("{$_:1,_$:2,a\u200C:3}")
			== JSON5::Object {{"$_", 1.0}, {"_$", 2.0}, {"a\u200C", 3.0}},
		"parses special character property names"
	);

	expect(
		JSON5::parse("{ùńîċõďë:9}")
			== JSON5::Object {{"ùńîċõďë", 9.0}},
		"parses unicode property names"
	);

	expect(
		JSON5::parse("{\\u0061\\u0062:1,\\u0024\\u005F:2,\\u005F\\u0024:3}")
			== JSON5::Object {{"ab", 1.0}, {"$_", 2.0}, {"_$", 3.0}},
		"parses escaped property names"
	);

	expect( // tough one:
		std::get<JSON5::Object>(JSON5::parse("{\"__proto__\":1}")).at("__proto__")
			== 1.0,
		"preserves __proto__ property names"
	);

	expect(
		JSON5::parse("{abc:1,def:2}")
			== JSON5::Object {{"abc", 1.0}, {"def", 2.0}},
		"parses multiple properties"
	);

	expect(
		JSON5::parse("{a:{b:2}}")
			== JSON5::Object {{"a", JSON5::Object {{"b", 2.0}}}},
		"parses nested objects"
	);

	expect(
		JSON5::parse("[]")
			== JSON5::Array {},
		"parses empty arrays"
	);

	expect(
		JSON5::parse("[1]")
			== JSON5::Array {1.0},
		"parses array values"
	);

	expect(
		JSON5::parse("[1,2]")
			== JSON5::Array {1.0, 2.0},
		"parses multiple array values"
	);

	expect(
		JSON5::parse("[1,[2,3]]")
			== JSON5::Array {1.0, JSON5::Array {2.0, 3.0}},
		"parses nested arrays"
	);

	expect(
		JSON5::parse("null")
			== JSON5::null,
		"parses nulls"
	);

	expect(
		JSON5::parse("true")
			== true,
		"parses true"
	);

	expect(
		JSON5::parse("false")
			== false,
		"parses false"
	);

	expect(
		JSON5::parse("[0,0.,0e0]")
			== JSON5::Array {0.0, 0.0, 0.0},
		"parses leading zeroes"
	);

	expect(
		JSON5::parse("[1,23,456,7890]")
			== JSON5::Array {1.0, 23.0, 456.0, 7890.0},
		"parses integers"
	);

	expect(
		JSON5::parse("[-1,+2,-.1,-0]")
			== JSON5::Array {-1.0, 2.0, -0.1, -0.0},
		"parses signed numbers"
	);

	expect(
		JSON5::parse("[.1,.23]")
			== JSON5::Array {0.1, 0.23},
		"parses leading decimal points"
	);

	expect(
		JSON5::parse("[1.0,1.23]")
			== JSON5::Array {1.0, 1.23},
		"parses fractional numbers"
	);

	expect(
		JSON5::parse("[1e0,1e1,1e01,1.e0,1.1e0,1e-1,1e+1]")
			== JSON5::Array {1.0, 10.0, 10.0, 1.0, 1.1, 0.1, 10.0},
		"parses exponents"
	);

	expect(
		JSON5::parse("[0x1,0x10,0xff,0xFF]")
			== JSON5::Array {1.0, 16.0, 255.0, 255.0},
		"parses hexadecimal numbers"
	);

	expect(
		JSON5::parse("[Infinity,-Infinity]")
			== JSON5::Array {
				std::numeric_limits<double>::infinity(),
				-std::numeric_limits<double>::infinity()
			},
		"parses signed and unsigned Infinity"
	);

	expect(
		std::isnan(std::get<double>(JSON5::parse("NaN"))),
		"parses NaN"
	);

	expect(
		std::isnan(std::get<double>(JSON5::parse("-NaN"))),
		"parses signed NaN"
	);

	expect(
		JSON5::parse("1")
			== 1.0,
		"parses 1"
	);

	expect(
		JSON5::parse("+1.23e100")
			== 1.23e100,
		"parses +1.23e100"
	);

	expect(
		JSON5::parse("0x1")
			== double(0x1),
		"parses bare hexadecimal number"
	);

	// expect( // doesn't work in C++
	// 	JSON5::parse("-0x0123456789abcdefABCDEF")
	// 		== -0x0123456789abcdefABCDEF,
	// 	"parses bare long hexadecimal number"
	// );

	expect(
		JSON5::parse("\"abc\"")
			== "abc",
		"parses double quoted strings"
	);

	expect(
		JSON5::parse("'abc'")
			== "abc",
		"parses single quoted strings"
	);

	expect(
		JSON5::parse(R"(['"',"'"])")
			== JSON5::Array {"\"", "'"},
		"parses quotes in strings"
	);

	expect(
		JSON5::parse("'\\b\\f\\n\\r\\t\\v\\0\\x0f\\u01fF\\\n\\\r\n\\\r\\\u2028\\\u2029\\a\\'\\\"'")
			== std::string() + "\b\f\n\r\t\v" + '\0' + "\x0f\u01FFa'\"",
		"parses escaped characters"
	);

	std::stringstream cerrCapture;
	std::streambuf* cerrBackup = std::cerr.rdbuf(cerrCapture.rdbuf());
	expect(
		JSON5::parse("'\u2028\u2029'")
			== "\u2028\u2029",
		"parses line and paragraph separators ..."
	);
	std::cerr.rdbuf(cerrBackup);
	expect(
		cerrCapture.str().find("not valid ECMAScript")
			!= std::string::npos,
		"... with a warning"
	);

	expect(
		JSON5::parse("{//comment\n}")
			== JSON5::Object {},
		"parses single-line comments"
	);

	expect(
		JSON5::parse("{}//comment")
			== JSON5::Object {},
		"parses single-line comments at end of input"
	);

	expect(
		JSON5::parse("{/*comment\n** */}")
			== JSON5::Object {},
		"parses multi-line comments"
	);

	expect(
		JSON5::parse("{\t\v\f \u00A0\uFEFF\n\r\u2028\u2029\u2003}")
			== JSON5::Object {},
		"parses whitespace"
	);

	// a reviver was not implemented since it is not really sensible in C++
}
