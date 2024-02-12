#pragma once

#include <variant>
#include <string>
#include <vector>
#include <map>

namespace JSON5 {

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

bool isNull(const Value value) { return std::holds_alternative<Null>(value); }
bool isBoolean(const Value value) { return std::holds_alternative<bool>(value); }
bool isNumber(const Value value) { return std::holds_alternative<double>(value); }
bool isString(const Value value) { return std::holds_alternative<std::string>(value); }
bool isArray(const Value value) { return std::holds_alternative<Array>(value); }
bool isObject(const Value value) { return std::holds_alternative<Object>(value); }

bool operator==(Null, Null) { return true; }

bool operator==(const Value& lhs, const Value& rhs) {
	return std::visit( [](const auto& left, const auto& right) {
			if constexpr (std::is_same_v<decltype(left), decltype(right)>) {
				return left == right;
			} else {
				return false;
			}
		}, lhs, rhs);
}

}
