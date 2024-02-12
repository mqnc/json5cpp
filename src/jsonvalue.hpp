#pragma once

#include <variant>
#include <string>
#include <vector>
#include <map>

namespace JSON5{

struct Null {};
const Null null;

struct JsonArray;
struct JsonObject;

using JsonValue = std::variant<
	Null,
	bool,
	double,
	std::string,
	JsonArray,
	JsonObject
	>;

struct JsonArray: public std::vector<JsonValue> {
	using std::vector<JsonValue>::vector;
};

struct JsonObject: public std::map<std::string, JsonValue> {
	using std::map<std::string, JsonValue>::map;
};

bool operator==(Null, Null) { return true; }

bool operator==(const JsonValue& lhs, const JsonValue& rhs) {
	return std::visit( [](const auto& left, const auto& right) {
			if constexpr (std::is_same_v<decltype(left), decltype(right)>) {
				return left == right;
			} else {
				return false;
			}
		}, lhs, rhs);
}

}