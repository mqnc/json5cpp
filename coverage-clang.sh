
set -e

echo cleanup previous builds...
rm -rf build
mkdir build
cd build

echo building test_suite...
clang++ -std=c++17 -I../src -g -Wall -Wextra -Wpedantic -Werror -fprofile-instr-generate -fcoverage-mapping -o test_suite ../test/main.cpp

echo running tests...
LLVM_PROFILE_FILE="test.profraw" ./test_suite

echo collecting coverage data...
llvm-profdata merge -sparse test.profraw -o report.profdata
mkdir report
llvm-cov show ./test_suite -show-line-counts-or-regions -Xdemangler c++filt -Xdemangler -n -instr-profile=report.profdata -format=html > report/index.html
llvm-cov report ./test_suite -instr-profile=report.profdata > report/summary.txt
