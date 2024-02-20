
#include "test.h"
#include "test_parse.hpp"
#include "test_errors.hpp"
#include "test_stringify.hpp"

int main(){

	testParse();
	testErrors();
	testStringify();

	report();

	return status();
}