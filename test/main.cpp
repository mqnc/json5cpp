
#include "test.h"
#include "test_utf8.hpp"
#include "test_util.hpp"
#include "test_jsonvalue.hpp"
#include "test_parse.hpp"
#include "test_errors.hpp"
#include "test_stringify.hpp"

int main(){

	testUTF8();
	testUtil();
	testJsonValue();
	testParse();
	testErrors();
	testStringify();

	report();

	return status();
}

