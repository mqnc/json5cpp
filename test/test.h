#pragma once

#include <string>
#include <iostream>

size_t numPassed = 0;
size_t numFailed = 0;

const auto RED = "\033[31m";
const auto GREEN = "\033[32m";
const auto RESET = "\033[0m";

void expect(bool success, std::string msg) {
	if (success) {
		std::cout << GREEN << "passed: " << msg << RESET << "\n";
		numPassed++;
	}
	else {
		std::cerr << RED << "failed: " << msg << RESET << "\n";
		numFailed++;
	}
}

void report() {
	std::cout << "passed tests: " << numPassed << "\n";
	bool f = numFailed > 0;
	std::cout << (f ? RED : "") << "failed tests: " << numFailed << (f ? RESET : "") << "\n\n";
}

auto status() {
	return numFailed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
