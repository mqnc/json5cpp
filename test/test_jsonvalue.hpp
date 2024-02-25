
#include "test.h"
#include "jsonvalue.hpp"

#include <cmath>

void testJsonValue() {
	JSON5::Value null = JSON5::null;
	JSON5::Value boolean = true;
	JSON5::Value number = 1.0;
	JSON5::Value string = "abc";
	JSON5::Value jsonArray = JSON5::Array {true, 1.0, "abc"};
	JSON5::Value jsonObject = JSON5::Object {{"a", 1.0}};

	expect(
		JSON5::isNull(null)
			&& !JSON5::isBoolean(null)
			&& !JSON5::isNumber(null)
			&& !JSON5::isString(null)
			&& !JSON5::isArray(null)
			&& !JSON5::isObject(null),
		"identifies null"
	);

	expect(
		!JSON5::isNull(boolean)
			&& JSON5::isBoolean(boolean)
			&& !JSON5::isNumber(boolean)
			&& !JSON5::isString(boolean)
			&& !JSON5::isArray(boolean)
			&& !JSON5::isObject(boolean),
		"identifies boolean"
	);

	expect(
		!JSON5::isNull(number)
			&& !JSON5::isBoolean(number)
			&& JSON5::isNumber(number)
			&& !JSON5::isString(number)
			&& !JSON5::isArray(number)
			&& !JSON5::isObject(number),
		"identifies number"
	);

	expect(
		!JSON5::isNull(string)
			&& !JSON5::isBoolean(string)
			&& !JSON5::isNumber(string)
			&& JSON5::isString(string)
			&& !JSON5::isArray(string)
			&& !JSON5::isObject(string),
		"identifies string"
	);

	expect(
		!JSON5::isNull(jsonArray)
			&& !JSON5::isBoolean(jsonArray)
			&& !JSON5::isNumber(jsonArray)
			&& !JSON5::isString(jsonArray)
			&& JSON5::isArray(jsonArray)
			&& !JSON5::isObject(jsonArray),
		"identifies array"
	);

	expect(
		!JSON5::isNull(jsonObject)
			&& !JSON5::isBoolean(jsonObject)
			&& !JSON5::isNumber(jsonObject)
			&& !JSON5::isString(jsonObject)
			&& !JSON5::isArray(jsonObject)
			&& JSON5::isObject(jsonObject),
		"identifies object"
	);

	expect(null == null, "json5 null == json5 null");
	expect(JSON5::Value {true} == JSON5::Value {true}, "json5 true == json5 true");
	expect(JSON5::Value {true} != JSON5::Value {false}, "json5 true != json5 false");
	expect(JSON5::Value {true} == true, "json5 true == C++ true");
	expect(true == JSON5::Value {true}, "C++ true == json5 true");
	expect(JSON5::Value {true} != false, "json5 true != C++ false");
	expect(JSON5::Value {null} != false, "json5 null != C++ false");
	expect(JSON5::Value {1.0} == JSON5::Value {1.0}, "json5 1.0 == json5 1.0");
	expect(JSON5::Value {1.0} != JSON5::Value {2.0}, "json5 1.0 != json5 2.0");
	expect(JSON5::Value {1.0} == 1.0, "json5 1.0 == C++ 1.0");
	expect(JSON5::Value {1.0} != 2.0, "json5 1.0 != C++ 2.0");
	expect(JSON5::Value {1.0} != true, "json5 1.0 != C++ true");
	expect(JSON5::Value {std::nan("")} != JSON5::Value {std::nan("")}, "json5 NaN != json5 NaN");
	expect(JSON5::Value {std::nan("")} != std::nan(""), "json5 NaN != C++ NaN");
	expect(JSON5::Value {"abc"} == JSON5::Value {"abc"}, "json5 \"abc\" == json5 \"abc\"");
	expect(JSON5::Value {"abc"} != JSON5::Value {"abcd"}, "json5 \"abc\" != json5 \"abcd\"");
	expect(JSON5::Value {"abc"} == std::string {"abc"}, "json5 \"abc\" == C++ string \"abc\"");
	expect(JSON5::Value {"abc"} == "abc", "json5 \"abc\" == C++ const char* \"abc\"");
	expect(JSON5::Value {"1.0"} != JSON5::Value {1.0}, "json5 \"1.0\" != json5 1.0 (sorry, JavaScript)");
	expect(JSON5::Value {JSON5::Array {1.0, "a"}} == JSON5::Value {JSON5::Array {1.0, "a"}},
		"json5 value{json5 array{1.0, \"a\"}} == json5 value{json5 array{1.0, \"a\"}}");
	expect(JSON5::Value {JSON5::Array {1.0, "a"}} != JSON5::Value {JSON5::Array {1.0, "b"}},
		"json5 value{json5 array{1.0, \"a\"}} != json5 value{json5 array{1.0, \"b\"}}");
	expect(JSON5::Value {JSON5::Array {1.0, "a"}} == JSON5::Array {1.0, "a"},
		"json5 value{json5 array{1.0, \"a\"}} == json5 array{1.0, \"a\"}");
	expect(JSON5::Array {1.0, "a"} == JSON5::Array {1.0, "a"},
		"json5 array{1.0, \"a\"} == json5 array{1.0, \"a\"}");
	expect(JSON5::Value {JSON5::Object {{"a", 1.0}}} == JSON5::Value {JSON5::Object {{"a", 1.0}}},
		"json5 value{json5 object{a:1.0}} == json5 value{json5 object{a:1.0}}");
	expect(JSON5::Value {JSON5::Object {{"a", 1.0}}} != JSON5::Value {JSON5::Object {{"a", 2.0}}},
		"json5 value{json5 object{a:1.0}} != json5 value{json5 object{a:2.0}}");
	expect(JSON5::Value {JSON5::Object {{"a", 1.0}}} == JSON5::Object {{"a", 1.0}},
		"json5 value{json5 object{a:1.0}} == json5 object{a:1.0}");
	expect(JSON5::Object {{"a", 1.0}} == JSON5::Object {{"a", 1.0}},
		"json5 object{a:1.0} == json5 object{a:1.0}");
}
