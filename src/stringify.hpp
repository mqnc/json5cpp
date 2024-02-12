
// #include <stack>
#include <array>
#include <vector>
#include <string>
#include <charconv>

#include "utf8.hpp"
#include "util.hpp"
#include "jsonvalue.hpp"
#include "parserstate.hpp"
#include "token.hpp"

namespace JSON5 {

class Stringifier {
	//std::stack<Object*> stack;
	std::string indent;
	//std::vector<std::string> propertyList;
	//let replacerFunc
	std::string gap;
	//std::string quote;

	template <class>
	static inline constexpr bool always_false_v = false;

public:
	std::string stringify(
		const Value& value,
		const std::variant<std::string, size_t> space = ""
	) {
		//stack = {};
		indent = "";
		//propertyList = {};
		//let replacerFunc
		gap = "";
		//std::string quote;

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
		if (std::holds_alternative<size_t>(space)) {
			gap = std::string(std::min(std::get<size_t>(space), 10uL), ' ');
		}
		else {
			gap = std::get<std::string>(space).substr(0, 10);
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

		for (size_t i = 0; i < value.size(); i++) {
			const char c = value[i]; // todo: UTF8
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
					product += util::isDigit(value[i + 1]) ? "\\x00" : "\\0"; // todo: UTF8
					continue;
				case '\\': product += "\\\\"; continue;
				case '\b': product += "\\b"; continue;
				case '\f': product += "\\f"; continue;
				case '\n': product += "\\n"; continue;
				case '\r': product += "\\r"; continue;
				case '\t': product += "\\t"; continue;
				case '\v': product += "\\v"; continue;
					//case '\u2028': product += "\\u2028"; continue; // todo
					//case '\u2029': product += "\\u2029"; continue;
			}

			if (c < ' ') {
				char hexString[5];
				std::sprintf(hexString, "\\x%02X", c);
				product += hexString;
				continue;
			};

			product += c;
		}

		// todo: consider user choice for quote
		const char quoteChar = (numSingleQuotes <= numDoubleQuotes) ? '\'' : '"';

		replaceAll(product, std::string() + quoteChar, std::string("\\") + quoteChar);

		return quoteChar + product + quoteChar;
	}

	std::string serializeObject(const Object& value) {
		// if (stack.indexOf(value) >= 0) {
		// 	throw TypeError('Converting circular structure to JSON5')
		// }

		//stack.push(value);

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

		//stack.pop();
		indent = stepback;
		return final;
	}

	std::string serializeKey(const std::string& key) {
		if (key.size() == 0) {
			return quoteString(key, true);
		}

		const UTF8Peek firstChar = peekUTF8(key);
		if (firstChar.status != UTF8Peek::Status::ok) {
			throw std::runtime_error("invalid UTF-8 sequence in key");
		}
		if (!util::isIdStartChar(firstChar.codepoint)) {
			return quoteString(key, true);
		}

		std::string_view rest = std::string_view(key).substr(firstChar.bytesRead);
		while (rest.size() > 0) {
			const UTF8Peek nextChar = peekUTF8(rest);
			if (nextChar.status != UTF8Peek::Status::ok) {
				throw std::runtime_error("invalid UTF-8 sequence in key");
			}
			if (!util::isIdContinueChar(nextChar.codepoint)) {
				return quoteString(key, true);
			}
			rest = rest.substr(nextChar.bytesRead);
		}

		return key;
	}

	std::string serializeArray(const Array& value) {
		// if (stack.indexOf(value) >= 0) {
		// 	throw TypeError('Converting circular structure to JSON5')
		// }

		// stack.push(value)

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

		//stack.pop()
		indent = stepback;
		return final;
	}
};

std::string stringify(
	const Value& value,
	const std::variant<std::string, size_t> space = ""
) {
	return Stringifier {}.stringify(value, space);
}

}
