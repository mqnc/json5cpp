
#pragma once

#include <string>
#include <limits>
#include <cmath>
#include <iostream>

#include "test.h"
#include "stringify.hpp"

const std::string _0 {'\0'};

void testStringify() {

	expect(
		JSON5::stringify(JSON5::Object {})
			== "{}",
		"stringifies empty objects"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"a", 1.0}})
			== "{a:1}",
		"stringifies unquoted property names"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"a-b", 1.0}})
			== "{'a-b':1}",
		"stringifies single quoted string property names"
	);


	expect(
		JSON5::stringify(JSON5::Object {{"a'", 1.0}})
			== "{\"a'\":1}",
		"stringifies double quoted string property names"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"", 1.0}})
			== "{'':1}",
		"stringifies empty string property names"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"$_", 1.0}, {"_$", 2.0}, {"a\u200C", 3.0}})
			== "{$_:1,_$:2,a\u200C:3}",
		"stringifies special character property names"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"ùńîċõďë", 9.0}})
			== "{ùńîċõďë:9}",
		"stringifies unicode property names"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"\\\b\f\n\r\t\v" + _0 + "\x01", 1.0}})
			== "{'\\\\\\b\\f\\n\\r\\t\\v\\0\\x01':1}",
		"stringifies escaped property names"
	);

	expect(
		JSON5::stringify(JSON5::Object {{_0 + _0 + "1", 1.0}})
			== "{'\\0\\x001':1}",
		"stringifies escaped null character property names"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"abc", 1.0}, {"def", 2.0}})
			== "{abc:1,def:2}",
		"stringifies multiple properties"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"a", JSON5::Object {{"b", 2.0}}}})
			== "{a:{b:2}}",
		"stringifies nested objects"
	);

	expect(
		JSON5::stringify(JSON5::Array {})
			== "[]",
		"stringifies empty arrays"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0})
			== "[1]",
		"stringifies array values"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0, 2.0})
			== "[1,2]",
		"stringifies multiple array values"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0, JSON5::Array {2.0, 3.0}})
			== "[1,[2,3]]",
		"stringifies nested arrays"
	);

	expect(
		JSON5::stringify(JSON5::null)
			== "null",
		"stringifies nulls"
	);

	// attempting to stringify functions would not compile

	expect(
		JSON5::stringify(true)
			== "true",
		"stringifies true"
	);

	expect(
		JSON5::stringify(false)
			== "false",
		"stringifies false"
	);

	expect(
		JSON5::stringify(-1.2)
			== "-1.2",
		"stringifies numbers"
	);

	expect(
		JSON5::stringify(JSON5::Array {
			std::numeric_limits<double>::infinity(),
			-std::numeric_limits<double>::infinity(),
			std::nan("")
		})
			== "[Infinity,-Infinity,NaN]",
		"stringifies non-finite numbers"
	);

	expect(
		JSON5::stringify("abc")
			== "'abc'",
		"stringifies single quoted strings"
	);

	expect(
		JSON5::stringify("abc'")
			== "\"abc'\"",
		"stringifies double quoted strings"
	);

	expect(
		JSON5::stringify("\\\b\f\n\r\t\v" + _0 + "\x0f")
			== "'\\\\\\b\\f\\n\\r\\t\\v\\0\\x0f'",
		"stringifies escaped characters"
	);

	expect(
		JSON5::stringify(_0 + _0 + "1")
			== "'\\0\\x001'",
		"stringifies escaped null characters"
	);

	expect(
		JSON5::stringify("'\"")
			== "'\\'\"'",
		"stringifies escaped single quotes"
	);

	expect(
		JSON5::stringify("''\"")
			== "\"''\\\"\"",
		"stringifies escaped double quotes"
	);

	expect(
		JSON5::stringify("\u2028\u2029")
			== "'\\u2028\\u2029'",
		"stringifies escaped line and paragraph separators"
	);

	// // conversion of custom data to json still has to be figured out

	// t.strictSame(
	//     JSON5.stringify(new Date('2016-01-01T00:00:00.000Z')),
	//     "'2016-01-01T00:00:00.000Z'",
	//     'stringifies using built-in toJSON methods'
	// )

	// t.test('stringifies using user defined toJSON methods', t => {
	//     function C () {}
	//     Object.assign(C.prototype, {toJSON () { return {a: 1, b: 2} }})
	//     assert.strictEqual(JSON5.stringify(new C()), '{a:1,b:2}')
	//     t.end()
	// })

	// t.test('stringifies using user defined toJSON(key) methods', t => {
	//     function C () {}
	//     Object.assign(C.prototype, {toJSON (key) { return (key === 'a') ? 1 : 2 }})
	//     assert.strictEqual(JSON5.stringify({a: new C(), b: new C()}), '{a:1,b:2}')
	//     t.end()
	// })

	// t.test('stringifies using toJSON5 methods', t => {
	//     function C () {}
	//     Object.assign(C.prototype, {toJSON5 () { return {a: 1, b: 2} }})
	//     assert.strictEqual(JSON5.stringify(new C()), '{a:1,b:2}')
	//     t.end()
	// })

	// t.test('stringifies using toJSON5(key) methods', t => {
	//     function C () {}
	//     Object.assign(C.prototype, {toJSON5 (key) { return (key === 'a') ? 1 : 2 }})
	//     assert.strictEqual(JSON5.stringify({a: new C(), b: new C()}), '{a:1,b:2}')
	//     t.end()
	// })

	// t.test('calls toJSON5 instead of toJSON if both are defined', t => {
	//     function C () {}
	//     Object.assign(C.prototype, {
	//         toJSON () { return {a: 1, b: 2} },
	//         toJSON5 () { return {a: 2, b: 2} },
	//     })
	//     assert.strictEqual(JSON5.stringify(new C()), '{a:2,b:2}')
	//     t.end()
	// })

	// // circular objects are impossible with the currently used data structure

	// t.test('throws on circular objects', t => {
	//     let a = {}
	//     a.a = a
	//     assert.throws(() => { JSON5.stringify(a) }, TypeError, 'Converting circular structure to JSON5')
	//     t.end()
	// })

	// t.test('throws on circular arrays', t => {
	//     let a = []
	//     a[0] = a
	//     assert.throws(() => { JSON5.stringify(a) }, TypeError, 'Converting circular structure to JSON5')
	//     t.end()
	// })

	expect(
		JSON5::stringify(JSON5::Array {1.0})
			== "[1]",
		"does not indent when no value is provided"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0}, {0})
			== "[1]",
		"does not indent when 0 is provided"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0}, {""})
			== "[1]",
		"does not indent when an empty string is provided"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0}, {2})
			== "[\n  1,\n]",
		"indents n spaces when a number is provided"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0}, {11})
			== "[\n          1,\n]",
		"does not indent more than 10 spaces when a number is provided"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0}, {-1})
			== "[1]",
		"does not indent when a negative number is provided"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0}, {"\t"})
			== "[\n\t1,\n]",
		"indents with the string provided"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0}, {"           "})
			== "[\n          1,\n]",
		"does not indent more than 10 characters of the string provided"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0}, {2})
			== "[\n  1,\n]",
		"indents in arrays"
	);

	expect(
		JSON5::stringify(JSON5::Array {1.0, JSON5::Array {2.0}, 3.0}, {2})
			== "[\n  1,\n  [\n    2,\n  ],\n  3,\n]",
		"indents in nested arrays"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"a", 1.0}}, {2})
			== "{\n  a: 1,\n}",
		"indents in objects"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"a", JSON5::Object {{"b", 2.0}}}}, {2})
			== "{\n  a: {\n    b: 2,\n  },\n}",
		"indents in nested objects"
	);

	// replacer is not yet(?) implemented

	// t.strictSame(
	//     JSON5.stringify({a: 1, b: 2, 3: 3}, ['a', 3]),
	//     "{a:1,'3':3}",
	//     'filters keys when an array is provided'
	// )

	// t.strictSame(
	//     JSON5.stringify({a: 1, b: 2, 3: 3, false: 4}, ['a', 3, false]),
	//     "{a:1,'3':3}",
	//     'only filters string and number keys when an array is provided'
	// )

	// t.strictSame(
	//     // eslint-disable-next-line no-new-wrappers
	//     JSON5.stringify({a: 1, b: 2, 3: 3}, [new String('a'), new Number(3)]),
	//     "{a:1,'3':3}",
	//     'accepts String and Number objects when an array is provided'
	// )

	// t.strictSame(
	//     JSON5.stringify({a: 1, b: 2}, (key, value) => (key === 'a') ? 2 : value),
	//     '{a:2,b:2}',
	//     'replaces values when a function is provided'
	// )

	// t.strictSame(
	//     JSON5.stringify({a: {b: 1}}, function (k, v) { return (k === 'b' && this.b) ? 2 : v }),
	//     '{a:{b:2}}',
	//     'sets `this` to the parent value'
	// )

	// t.test('is called after toJSON', t => {
	//     function C () {}
	//     Object.assign(C.prototype, {toJSON () { return {a: 1, b: 2} }})
	//     assert.strictEqual(
	//         JSON5.stringify(new C(), (key, value) => (key === 'a') ? 2 : value),
	//         '{a:2,b:2}'
	//     )
	//     t.end()
	// })

	// t.test('is called after toJSON5', t => {
	//     function C () {}
	//     Object.assign(C.prototype, {toJSON5 () { return {a: 1, b: 2} }})
	//     assert.strictEqual(
	//         JSON5.stringify(new C(), (key, value) => (key === 'a') ? 2 : value),
	//         '{a:2,b:2}'
	//     )
	//     t.end()
	// })

	// t.strictSame(
	//     JSON5.stringify(
	//         {a: 1},
	//         (key, value) => {
	//             JSON5.stringify({}, null, 4)
	//             return value
	//         },
	//         2
	//     ),
	//     '{\n  a: 1,\n}',
	//     'does not affect space when calls are nested'
	// )

#if (__cplusplus >= 202002L)
	expect(
		JSON5::stringify(JSON5::Array {1.0}, {.space = 2})
			== "[\n  1,\n]",
		"accepts space as an option"
	);
#endif

	auto quote = [](char c) {
		JSON5::StringifyOptions result;
		result.quote = c;
		return result;
	};

	expect(
		JSON5::stringify(JSON5::Object {{"a\"", "1\""}}, quote('"'))
			== "{\"a\\\"\":\"1\\\"\"}",
		"uses double quotes if provided"
	);

	expect(
		JSON5::stringify(JSON5::Object {{"a'", "1'"}}, quote('\''))
			== "{'a\\'':'1\\''}",
		"uses single quotes if provided"
	);
}
