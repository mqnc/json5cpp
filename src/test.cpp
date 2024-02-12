
#include <string>
#include <iostream>
#include "parse.hpp"
#include "stringify.hpp"

#define THROW_ON_FAILURE

size_t numPassed = 0;
size_t numFailed = 0;

void expect(bool success, std::string msg) {
	if (success) {
		std::cout << "\033[32mpassed: " << msg << "\033[0m\n";
		numPassed++;
	}
	else {
		std::string fail = std::string("\033[31mfailed: ") + msg + "\033[0m\n";
#ifdef THROW_ON_FAILURE
		throw std::runtime_error(fail);
#else
		std::cerr << fail;
		numFailed++;
#endif
	}
}

int main() {

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

	return EXIT_SUCCESS;
}
