#pragma once

#include <string>
#include <iostream>

size_t numPassed = 0;
size_t numFailed = 0;

void expect(bool success, std::string msg) {
	if (success) {
		std::cout << "\033[32mpassed: " << msg << "\033[0m\n";
		numPassed++;
	}
	else {
		std::cerr << "\033[31mfailed: " << msg << "\033[0m\n";
		numFailed++;
	}
}

void report(){
	std::cout << "passed tests: " << numPassed << "\n";
	if (numFailed > 0) { std::cout << "\033[31m"; }
	std::cout << "failed tests: " << numFailed << "\033[0m\n\n";
}

auto status(){
	return numFailed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
