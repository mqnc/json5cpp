
#pragma once

#include "test.h"
#include "utf8.hpp"
#include <vector>

void testUTF8() {

	{
		UTF8Peek result = peekUTF8("");
		expect(
			result.codepoint == char32_t(-1)
				&& result.bytesRead == 0
				&& result.status == UTF8Peek::Status::endOfString,
			"reports end of UTF-8 stream"
		);
	}

	{
		std::string input = std::string {'\x00'} + "\x7f"
			"\xc2\x80" "\xdf\xbf"
			"\xe0\xa0\x80" "\xef\xbf\xbf"
			"\xed\x9f\xbf" "\xee\x80\x80" // just outside surrogate pairs
			"\xf0\x90\x80\x80" "\xf4\x8f\xbf\xbf";
		std::vector<char32_t> expectedCodepoints = {
			0x0, 0x7f,
			0x80, 0x7ff,
			0x800, 0xffff,
			0xd7ff, 0xe000,
			0x10000, 0x10ffff
		};
		std::vector<uint16_t> expectedBytesRead = {1, 1, 2, 2, 3, 3, 3, 3, 4, 4};

		size_t pos = 0;
		for (size_t i = 0; i < expectedCodepoints.size(); i++) {
			UTF8Peek result = peekUTF8(input.substr(pos));
			expect(
				result.codepoint == expectedCodepoints[i]
					&& result.bytesRead == expectedBytesRead[i]
					&& result.status == UTF8Peek::Status::ok,
				"returns correct codepoint and bytes read from UTF-8 stream"
			);
			pos += result.bytesRead;
		}
	}

	{
		std::vector<std::string> invalidSequences = {
			std::string {char(0b11000000), char(0b11000000)},
			std::string {char(0b11100000), char(0b10000000), char(0b11000000)},
			std::string {char(0b11100000), char(0b11000000), char(0b10000000)},
			"\xed\xa0\x80",
			"\xed\xbf\xbf",
			std::string {char(0b11110000), char(0b10000000), char(0b10000000), char(0b11000000)},
			std::string {char(0b11110000), char(0b10000000), char(0b11000000), char(0b10000000)},
			std::string {char(0b11110000), char(0b11000000), char(0b10000000), char(0b10000000)},
			std::string {char(0b11110100), char(0b10010000), char(0b10000000), char(0b10000000)},
			std::string {char(0b11111000)}
		};

		for (size_t i = 0; i < invalidSequences.size(); i++) {
			UTF8Peek result = peekUTF8(invalidSequences[i]);
			expect(
				result.codepoint == char32_t(-1)
					&& result.bytesRead == 0
					&& result.status == UTF8Peek::Status::invalid,
				"rejects invalid UTF-8 sequence"
			);
		}
	}

	{
		size_t rejections = 0;
		size_t successes = 0;
		size_t fails = 0;
		for (char32_t cp = 0; cp <= 0x110000; cp++) {
			std::string utf8 = toUTF8(cp);
			if ((cp >= 0xd800 && cp <= 0xdfff) || cp > 0x10ffff) {
				if (utf8 == "") { rejections++; }
				else { fails++; }
			}
			else {
				auto roundtrip = peekUTF8(utf8);
				if (roundtrip.codepoint == cp) { successes++; }
				else { fails++; }
			}
		}
		expect(rejections == 0x801,
			"rejects encoding surrogate and out-of-range codepoints");
		expect(successes == 0x110000 - 0x800,
			"passes UTF-8 encode decode roundtrip for all valid codepoints");
		expect(fails == 0, "and there was no miscount");
	}
}
