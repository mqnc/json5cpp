
#include "test.h"
#include "test_utf8.hpp"
#include "test_parse.hpp"
#include "test_errors.hpp"
#include "test_stringify.hpp"

int main(){

	testUTF8();
	testParse();
	testErrors();
	testStringify();

	report();

	return status();
}