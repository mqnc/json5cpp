
#include "parse.hpp"
#include "stringify.hpp"

int main(){

	JSON5::JsonValue j1 = JSON5::Json5Parser{}.parse("{x:12.3, 'y':['a', \"'b'\"], \"'awa'\":'awa'}");
	JSON5::JsonValue j2 = JSON5::JsonObject{{"x", 12.30}};

	std::cout << JSON5::Json5Stringifier{}.stringify(j1) << "\n";

	std::cout << (j1 == j2);
	
	return EXIT_SUCCESS;
}