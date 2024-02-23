
#pragma once

#include "test.h"
#include "parse.hpp"

void testErrors() {

	try {
		JSON5::parse("");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 1,
			"throws on empty documents"
		);
	}

	try {
		JSON5::parse("//a");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 4,
			"throws on documents with only comments"
		);
	}

	try {
		JSON5::parse("/a");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'a'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 2,
			"throws on incomplete single line comments"
		);
	}

	try {
		JSON5::parse("/**");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 4,
			"throws on unterminated multiline comment closings"
		);
	}

	try {
		JSON5::parse("a");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'a'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 1,
			"throws on invalid characters in values"
		);
	}

	try {
		JSON5::parse("{\\a:1}");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'a'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on invalid characters in identifier start escapes"
		);
	}

	try {
		JSON5::parse("{\\u0021:1}");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid identifier character") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 2,
			"throws on invalid identifier start characters"
		);
	}

	try {
		JSON5::parse("{a\\a:1}");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'a'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 4,
			"throws on invalid characters in identifier continue escapes"
		);
	}

	try {
		JSON5::parse("{a\\u0021:1}");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid identifier character") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on invalid identifier continue characters"
		);
	}

	try {
		JSON5::parse("-a");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'a'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 2,
			"throws on invalid characters following a sign"
		);
	}

	try {
		JSON5::parse(".a");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'a'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 2,
			"throws on invalid characters following a leading decimal point"
		);
	}

	try {
		JSON5::parse("1ea");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'a'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on invalid characters following an exponent indicator"
		);
	}

	try {
		JSON5::parse("1e-a");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'a'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 4,
			"throws on invalid characters following an exponent sign"
		);
	}

	try {
		JSON5::parse("0xg");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'g'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on invalid characters following a hexadecimal indicator"
		);
	}

	try {
		JSON5::parse("\"\n \"");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character '\\n'") == 0
				&& e.lineNumber == 2
				&& e.columnNumber == 0,
			"throws on invalid new lines in strings"
		);
	}

	try {
		JSON5::parse("\"");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 2,
			"throws on unterminated strings"
		);
	}

	try {
		JSON5::parse("{!:1}");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character '!'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 2,
			"throws on invalid identifier start characters in property names"
		);
	}

	try {
		JSON5::parse("{a!1}");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character '!'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on invalid characters following a property name"
		);
	}

	try {
		JSON5::parse("{a:1!}");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character '!'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 5,
			"throws on invalid characters following a property value"
		);
	}

	try {
		JSON5::parse("[1!]");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character '!'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on invalid characters following an array value"
		);
	}

	try {
		JSON5::parse("tru!");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character '!'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 4,
			"throws on invalid characters in literals"
		);
	}

	try {
		JSON5::parse("\"\\");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on unterminated escapes"
		);
	}

	try {
		JSON5::parse("\"\\xg\"");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'g'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 4,
			"throws on invalid first digits in hexadecimal escapes"
		);
	}

	try {
		JSON5::parse("\"\\x0g\"");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'g'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 5,
			"throws on invalid second digits in hexadecimal escapes"
		);
	}

	try {
		JSON5::parse("\"\\u000g\"");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character 'g'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 7,
			"throws on invalid unicode escapes"
		);
	}

	for (size_t i = 1; i <= 9; i++) {
		try {
			JSON5::parse("'\\" + std::to_string(i) + "'");
		}
		catch (const JSON5::SyntaxError& e) {
			expect(
				std::string(e.what()).find(
					"JSON5: invalid character '" + std::to_string(i) + "'"
					) == 0
					&& e.lineNumber == 1
					&& e.columnNumber == 3,
				"throws on escaped digit " + std::to_string(i)
			);
		}
	}

	try {
		JSON5::parse("'\\01'");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character '1'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 4,
			"throws on octal escape"
		);
	}

	try {
		JSON5::parse("1 2");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character '2'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on multiple values"
		);
	}

	try {
		JSON5::parse("\x01");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid character '\\x01'") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 1,
			"throws with control characters escaped in the message"
		);
	}

	try {
		JSON5::parse("{");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 2,
			"throws on unclosed objects before property names"
		);
	}

	try {
		JSON5::parse("{a");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on unclosed objects after property names"
		);
	}

	try {
		JSON5::parse("{a:");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 4,
			"throws on unclosed objects before property values"
		);
	}

	try {
		JSON5::parse("{a:1");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 5,
			"throws on unclosed objects after property values"
		);
	}

	try {
		JSON5::parse("[");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 2,
			"throws on unclosed arrays before values"
		);
	}

	try {
		JSON5::parse("[1");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid end of input") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 3,
			"throws on unclosed arrays after values"
		);
	}

	try {
		JSON5::parse("\xff");
	}
	catch (const JSON5::SyntaxError& e) {
		expect(
			std::string(e.what()).find("JSON5: invalid unicode byte sequence") == 0
				&& e.lineNumber == 1
				&& e.columnNumber == 1,
			"throws on invalid unicode byte sequence"
		);
	}

}
