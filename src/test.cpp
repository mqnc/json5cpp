
#include "parse.hpp"
#include "stringify.hpp"

int main(){

	JSON5::Value j1 = JSON5::parse("{JSON:5,}");
	JSON5::Value j2 = JSON5::Object{{"JSON", 5.0}};

	std::cout << JSON5::stringify(j1) << "\n";

	std::cout << (j1 == j2) << "\n";
	
	return EXIT_SUCCESS;
}