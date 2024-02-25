
#pragma once

#include <array>
#include <vector>
#include <string>
#include <optional>
#include <charconv>
#include <cstdint>

#include "utf8.hpp"
#include "util.hpp"
#include "jsonvalue.hpp"
#include "parserstate.hpp"
#include "token.hpp"

namespace JSON5 {

struct StringifyOptions {
	// int so the user can write {4} instead of {4u}
	std::variant<std::string, int> space;
	std::optional<char> quote = std::nullopt;
};

class Stringifier {
	std::string indent;
	//std::vector<std::string> propertyList;
	//let replacerFunc
	std::string gap;
	std::optional<char> quote;

	template <class>
	static inline constexpr bool always_false_v = false;

public:
	std::string stringify(
		const Value& value,
		const StringifyOptions options = {}
	) {
		indent = "";
		//propertyList = {};
		//let replacerFunc
		gap = "";
		quote = options.quote;

		// if (
		//     replacer != null &&
		//     typeof replacer === 'object' &&
		//     !Array.isArray(replacer)
		// ) {
		//     space = replacer.space
		//     quote = replacer.quote
		//     replacer = replacer.replacer
		// }

		// if (typeof replacer === 'function') {
		//     replacerFunc = replacer
		// } else if (Array.isArray(replacer)) {
		//     propertyList = []
		//     for (const v of replacer) {
		//         let item

		//         if (typeof v === 'string') {
		//             item = v
		//         } else if (
		//             typeof v === 'number' ||
		//             v instanceof String ||
		//             v instanceof Number
		//         ) {
		//             item = String(v)
		//         }

		//         if (item !== undefined && propertyList.indexOf(item) < 0) {
		//             propertyList.push(item)
		//         }
		//     }
		// }

		// if (space instanceof Number) {
		//     space = Number(space)
		// } else if (space instanceof String) {
		//     space = String(space)
		// }

		// if (typeof space === 'number') {
		//     if (space > 0) {
		//         space = Math.min(10, Math.floor(space))
		//         gap = '          '.substr(0, space)
		//     }
		// } else if (typeof space === 'string') {
		//     gap = space.substr(0, 10)
		// }
		if (std::holds_alternative<int>(options.space)) {
			int i = std::get<int>(options.space);
			if (i < 0) { i = 0; }
			if (i > 10) { i = 10; }
			gap = std::string(i, ' ');
		}
		else {
			gap = std::get<std::string>(options.space).substr(0, 10);
		}
		return serializeProperty("", Object {{"", value}});
	}

private:
	void replaceAll(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
	}

	std::string join(const std::vector<std::string>& items, const std::string& separator) {
		if (items.empty()) { return ""; }
		std::string result = items[0];
		for (size_t i = 1; i < items.size(); ++i) {
			result += separator + items[i];
		}
		return result;
	}

	template <typename K, typename H>
	std::string serializeProperty(const K& key, const H& holder) {
		const Value& value = holder.at(key);
		// if (!std::holds_alternative<Null>(value)) {
		//     if (typeof value.toJSON5 === 'function') {
		//         value = value.toJSON5(key)
		//     } else if (typeof value.toJSON === 'function') {
		//         value = value.toJSON(key)
		//     }
		// }

		// if (replacerFunc) {
		//     value = replacerFunc.call(holder, key, value)
		// }

		// if (value instanceof Number) {
		//     value = Number(value)
		// } else if (value instanceof String) {
		//     value = String(value)
		// } else if (value instanceof Boolean) {
		//     value = value.valueOf()
		// }

		return std::visit( [this](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, Null>) {
					return std::string("null");
				}
				else if constexpr (std::is_same_v<T, bool>) {
					return std::string(arg ? "true" : "false");
				}
				else if constexpr (std::is_same_v<T, std::string>) {
					return quoteString(arg);
				}
				else if constexpr (std::is_same_v<T, double>) {
					if (std::isnan(arg)) { return std::string("NaN"); }
					if (std::isinf(arg)) { return std::string(arg > 0 ? "Infinity" : "-Infinity"); }
					std::array<char, 64> buffer;
					auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), arg);
					return std::string(buffer.data(), result.ptr - buffer.data());
				}
				else if constexpr (std::is_same_v<T, Array>) {
					return serializeArray(arg);
				}
				else if constexpr (std::is_same_v<T, Object>) {
					return serializeObject(arg);
				}
				else {
					static_assert(always_false_v<T>, "unhandled variant alternative");
				}
			}, value);
	}

	std::string quoteString(const std::string& value, bool = true) {
		size_t numSingleQuotes = 0;
		size_t numDoubleQuotes = 0;

		std::string product = "";

		std::string_view rest = value;
		while (rest.size() > 0) {
			const UTF8Peek peek = peekUTF8(rest);
			if (peek.status != UTF8Peek::Status::ok) {
				throw std::runtime_error("JSON5: invalid UTF-8 sequence in string");
			}
			const char32_t c = peek.codepoint;
			rest = rest.substr(peek.bytesRead);

			switch (c) {
				case '\'':
					numSingleQuotes++;
					product += c;
					continue;
				case '"':
					numDoubleQuotes++;
					product += c;
					continue;
				case '\0':
					product += util::isDigit(peekUTF8(rest).codepoint) ? "\\x00" : "\\0";
					continue;
				case '\\': product += "\\\\"; continue;
				case '\b': product += "\\b"; continue;
				case '\f': product += "\\f"; continue;
				case '\n': product += "\\n"; continue;
				case '\r': product += "\\r"; continue;
				case '\t': product += "\\t"; continue;
				case '\v': product += "\\v"; continue;
				case 0x2028: product += "\\u2028"; continue;
				case 0x2029: product += "\\u2029"; continue;
			}

			if (c < ' ') {
				char hexString[5];
				std::sprintf(hexString, "\\x%02x", c);
				product += hexString;
				continue;
			};

			product += c;

		}

		const char quoteChar = quote ? *quote
			: (numSingleQuotes <= numDoubleQuotes) ? '\'' : '"';

		replaceAll(product, std::string() + quoteChar, std::string("\\") + quoteChar);

		return quoteChar + product + quoteChar;
	}

	std::string serializeObject(const Object& value) {

		std::string stepback = indent;
		indent += gap;

		//let keys = propertyList || Object.keys(value);
		std::vector<std::string> partial = {};
		for (const auto& [key, _]: value) {
			const std::string propertyString = serializeProperty(key, value);
			//if (propertyString !== undefined) {
			std::string member = serializeKey(key) + ':';
			if (gap != "") {
				member += ' ';
			}
			member += propertyString;
			partial.push_back(member);
			//}
		}

		std::string final;
		if (partial.size() == 0) {
			final = "{}";
		}
		else {
			std::string properties;
			if (gap == "") {
				properties = join(partial, ",");
				final = '{' + properties + '}';
			}
			else {
				std::string separator = ",\n" + indent;
				properties = join(partial, separator);
				final = "{\n" + indent + properties + ",\n" + stepback + "}";
			}
		}

		indent = stepback;
		return final;
	}

	std::string serializeKey(const std::string& key) {
		if (key.size() == 0) {
			return quoteString(key, true);
		}

		const UTF8Peek firstChar = peekUTF8(key);
		if (firstChar.status != UTF8Peek::Status::ok) {
			throw std::runtime_error("JSON5: invalid UTF-8 sequence in key");
		}
		if (!util::isIdStartChar(firstChar.codepoint)) {
			return quoteString(key, true);
		}

		std::string_view rest = std::string_view(key).substr(firstChar.bytesRead);
		while (rest.size() > 0) {
			const UTF8Peek nextChar = peekUTF8(rest);
			if (nextChar.status != UTF8Peek::Status::ok) {
				throw std::runtime_error("JSON5: invalid UTF-8 sequence in key");
			}
			if (!util::isIdContinueChar(nextChar.codepoint)) {
				return quoteString(key, true);
			}
			rest = rest.substr(nextChar.bytesRead);
		}

		return key;
	}

	std::string serializeArray(const Array& value) {

		std::string stepback = indent;
		indent += gap;

		std::vector<std::string> partial = {};
		for (size_t i = 0; i < value.size(); i++) {
			const std::string propertyString = serializeProperty(i, value);
			//partial.push((propertyString !== undefined) ? propertyString : 'null');
			partial.push_back(propertyString);
		}

		std::string final;
		if (partial.size() == 0) {
			final = "[]";
		}
		else {
			std::string properties;
			if (gap == "") {
				properties = join(partial, ",");
				final = '[' + properties + ']';
			}
			else {
				std::string separator = ",\n" + indent;
				properties = join(partial, separator);
				final = "[\n" + indent + properties + ",\n" + stepback + "]";
			}
		}

		indent = stepback;
		return final;
	}
};

std::string stringify(
	const Value& value,
	const StringifyOptions options = {}
) {
	return Stringifier {}.stringify(value, options);
}

}
