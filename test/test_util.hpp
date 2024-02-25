
#include "test.h"
#include "util.hpp"

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <string>
#include <sstream>

inline bool endsWith(std::string const& value, std::string const& ending) {
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void testUtil() {

	struct Entry {
		char32_t value;
		std::string name;
		std::string category;
	};

	std::ifstream file("UnicodeData.txt");
	std::vector<Entry> entries;
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			Entry entry;
			std::istringstream iss(line);
			std::string value;
			std::getline(iss, value, ';');
			entry.value = std::stoi(value, 0, 16);
			std::getline(iss, entry.name, ';');
			std::getline(iss, entry.category, ';');
			entries.push_back(entry);
		}
		file.close();
	} else {
		expect(false, "File UnicodeData.txt must be in working directory.\n"
			"Can be downloaded here: https://www.unicode.org/Public/10.0.0/ucd/UnicodeData.txt");
		return;
	}

	auto entry = entries.begin();
	size_t spaceSeparatorErrors = 0;
	size_t idStartErrors = 0;
	size_t idContinueErrors = 0;
	size_t digitErrors = 0;
	size_t hexDigitErrors = 0;
	std::string currentCategory = "";

	std::set<std::string> spaceSeparatorCategories = {"Zs"};
	std::set<std::string> idStartCategories = {"Lu", "Ll", "Lt", "Lm", "Lo", "Nl"};
	std::set<std::string> idContinueExtraCategories = {"Mn", "Mc", "Nd", "Pc"};
	std::string digits = "0123456789";
	std::string hexDigits = "0123456789abcdefABCDEF";

	// int numIdStarts = 0;
	// int numIdContinues = 0;
	// int numSpaceSeparators = 0;

	for (char32_t cp = 0; cp <= 0x110000; cp++) {

		bool reallyIsSpaceSeparator =
			spaceSeparatorCategories.count(currentCategory) == 1
			&& cp != ' ' // space
			&& cp != 0xa0; // and nbsp are handled separately in the parser
		bool reallyIsIdStartChar =
			idStartCategories.count(currentCategory) == 1
			|| cp == '$'
			|| cp == '_';
		bool reallyIsIdContinueChar =
			reallyIsIdStartChar
			|| idContinueExtraCategories.count(currentCategory) == 1
			|| cp == 0x200c
			|| cp == 0x200d;
		bool reallyIsDigit = cp < 128 && digits.find(cp) != std::string::npos;
		bool reallyIsHexDigit = cp < 128 && hexDigits.find(cp) != std::string::npos;

		// numIdStarts += reallyIsIdStartChar;
		// numIdContinues += reallyIsIdContinueChar;
		// numSpaceSeparators += reallyIsSpaceSeparator;

		if (util::isSpaceSeparator(cp) != reallyIsSpaceSeparator) { spaceSeparatorErrors++; }
		if (util::isIdStartChar(cp) != reallyIsIdStartChar) { idStartErrors++; }
		if (util::isIdContinueChar(cp) != reallyIsIdContinueChar) { idContinueErrors++; }
		if (util::isDigit(cp) != reallyIsDigit) { digitErrors++; }
		if (util::isHexDigit(cp) != reallyIsHexDigit) { hexDigitErrors++; }

		if (entry != entries.end()) {
			entry++;
			if (entry == entries.end()) {
				// cp is beyond the last character in the data sheet
				currentCategory = "-";
			}
			else if (entry->value > cp + 1) {
				if (endsWith(entry->name, "Last>")) {
					// we're in a group
					currentCategory = entry->category;
				}
				else {
					// we're in a gap (noncharacter or unassigned)
					currentCategory = "-";
				}
				entry--;
			}
			else {
				// entry->value == cp+1
				currentCategory = entry->category;
			}
		}
	}

	expect(spaceSeparatorErrors == 0, "properly recognizes Unicode space separators");
	expect(idStartErrors == 0, "properly recognizes Unicode identifier start characters");
	expect(idContinueErrors == 0, "properly recognizes Unicode identifier continue characters");
	expect(digitErrors == 0, "properly recognizes digit characters");
	expect(hexDigitErrors == 0, "properly recognizes hex digit characters");

	expect(
		!util::isIdStartChar(0xb4)
			&& util::isIdStartChar(0xb5) // micro sign
			&& !util::isIdStartChar(0xb6)
			&& !util::isIdStartChar(0x33ff)
			&& util::isIdStartChar(0x3400) // <CJK Ideograph Extension A, First>
			&& util::isIdStartChar(0x4db5) // <CJK Ideograph Extension A, Last>
			&& !util::isIdStartChar(0x4db6),
		"and there was no off-by-one error in the test"
	);

}
