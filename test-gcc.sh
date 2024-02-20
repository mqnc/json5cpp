
echo cleanup previous builds...
rm -rf build
mkdir build
cd build

echo building test_suite...
g++ -std=c++17 -I../src -g -Wall -Wextra -Wpedantic -Werror --coverage -o test_suite ../test/main.cpp

echo running tests...
./test_suite

echo collecting coverage data...
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info

