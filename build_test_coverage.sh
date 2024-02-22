
set -e

echo cleanup previous builds...
rm -rf build
mkdir build
cd build
mkdir report

BUILD_STATUS_BADGE=report/build_status.svg
TEST_STATUS_BADGE=report/test_status.svg
COVERAGE_BADGE=report/coverage.svg
DETAILED_COVERAGE=report/detailed_coverage.html
COVERAGE_SUMMARY=report/coverage_summary.txt

echo building test_suite...
{
	clang++ -std=c++17 -I../src -g -Wall -Wextra -Wpedantic -Werror -fprofile-instr-generate -fcoverage-mapping -o test_suite ../test/main.cpp
	echo build successful
	curl https://img.shields.io/badge/build-passing-brightgreen > $BUILD_STATUS_BADGE
} || {
	echo build failed
	curl https://img.shields.io/badge/build-failing-red > $BUILD_STATUS_BADGE
	curl https://img.shields.io/badge/tests-not_run-silver > $TEST_STATUS_BADGE
	curl https://img.shields.io/badge/coverage-none-silver > $COVERAGE_BADGE
	exit 1
}

echo running tests...
{
	LLVM_PROFILE_FILE="test.profraw" ./test_suite
	echo tests passed
	curl https://img.shields.io/badge/tests-passing-brightgreen > $TEST_STATUS_BADGE
} || {
	echo tests failed
	curl https://img.shields.io/badge/tests-failing-red > $TEST_STATUS_BADGE
}

echo collecting coverage data...
llvm-profdata merge -sparse test.profraw -o report.profdata
llvm-cov show ./test_suite -show-line-counts-or-regions -Xdemangler c++filt -Xdemangler -n -instr-profile=report.profdata -format=html > $DETAILED_COVERAGE
llvm-cov report ./test_suite -instr-profile=report.profdata > $COVERAGE_SUMMARY
coverage=$(sed 's/.* //g' "$COVERAGE_SUMMARY" | tail -n 1)
cat $COVERAGE_SUMMARY
curl https://img.shields.io/badge/coverage-${coverage}25-brightgreen > $COVERAGE_BADGE
echo "coverage: $coverage"