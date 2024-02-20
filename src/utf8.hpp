
#pragma once

#include <iostream>
#include <string>
#include <string_view>

struct UTF8Peek {
	char32_t codepoint;
	uint16_t bytesRead;

	enum class Status { ok, endOfString, invalid };
	Status status;
};

UTF8Peek peekUTF8(std::string_view source) {

	if (source.empty()) {
		return {0, 0, UTF8Peek::Status::endOfString};
	}

	uint8_t u_source0 = source[0];

	if (u_source0 <= 0b0111'1111) {
		return {u_source0, 1, UTF8Peek::Status::ok};
	}
	else if (
		(u_source0 & 0b1110'0000) == 0b1100'0000
		&& source.size() >= 2
		&& (source[1] & 0b1100'0000) == 0b1000'0000
	) {
		return {
			((char32_t(u_source0) & 0b0001'1111) << 6)
				| (char32_t(source[1]) & 0b0011'1111),
			2, UTF8Peek::Status::ok
		};
	}
	else if (
		(u_source0 & 0b1111'0000) == 0b1110'0000
		&& source.size() >= 3
		&& (source[1] & 0b1100'0000) == 0b1000'0000
		&& (source[2] & 0b1100'0000) == 0b1000'0000
	) {
		return {
			((char32_t(u_source0) & 0b0000'1111) << 12)
				| ((char32_t(source[1]) & 0b0011'1111) << 6)
				| (char32_t(source[2]) & 0b0011'1111),
			3, UTF8Peek::Status::ok
		};
	}
	else if (
		(u_source0 & 0b1111'1000) == 0b1111'0000
		&& source.size() >= 4
		&& (source[1] & 0b1100'0000) == 0b1000'0000
		&& (source[2] & 0b1100'0000) == 0b1000'0000
		&& (source[3] & 0b1100'0000) == 0b1000'0000
	) {
		return {
			((char32_t(u_source0) & 0b0000'0111) << 18)
				| ((char32_t(source[1]) & 0b0011'1111) << 12)
				| ((char32_t(source[2]) & 0b0011'1111) << 6)
				| (char32_t(source[3]) & 0b0011'1111),
			4, UTF8Peek::Status::ok
		};
	}

	return {0, 0, UTF8Peek::Status::invalid};
}

std::string toUTF8(char32_t codepoint) {

	if (codepoint <= 0b1111111) {
		return {char(codepoint)};
	}
	else if (codepoint <= 0b11111'111111) {
		return {
			char((codepoint >> 6) | 0b1100'0000),
			char((codepoint & 0b0011'1111) | 0b1000'0000)
		};
	}
	else if (codepoint <= 0b1111'111111'111111) {
		return {
			char((codepoint >> 12) | 0b1110'0000),
			char(((codepoint >> 6) & 0b0011'1111) | 0b1000'0000),
			char((codepoint & 0b0011'1111) | 0b1000'0000)
		};
	} else if (codepoint <= 0x10FFFF) {
		return {
			char((codepoint >> 18) | 0b1111'0000),
			char(((codepoint >> 12) & 0b0011'1111) | 0b1000'0000),
			char(((codepoint >> 6) & 0b0011'1111) | 0b1000'0000),
			char((codepoint & 0b0011'1111) | 0b1000'0000)
		};
	}
	return "";
}
