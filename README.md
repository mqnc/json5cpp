# json5cpp
[![Build Status](https://mqnc.github.io/json5cpp/build_status.svg)](https://mqnc.github.io/json5cpp/build_log.txt)
[![Test Status](https://mqnc.github.io/json5cpp/test_status.svg)](https://mqnc.github.io/json5cpp/test_log.html)
[![Coverage](https://mqnc.github.io/json5cpp/coverage.svg)](https://mqnc.github.io/json5cpp/detailed_coverage.html)

C++ library for parsing and stringifying [JSON5](https://json5.org/) data

This is a port of the JavaScript [reference implementation](https://github.com/json5/json5) to C++.

## Project Status
The core functionality is completely implemented and the test coverage is as close to 100% as possible without causing compiler warnings. See the tests for usage examples.

## Design Principles
- Sticking to the reference implementation as much as sensible.
- No dependencies apart from the STL.
- Minimalism.

## Limitations
- Due to a lack of experience in writing safety-critical software, this library should probably not be used where security is important. Please open issues for any vulnerabilities you spot.
- So far, the number data type only supports doubles, no ints. You can of course store ints in doubles but only from -2^53 to 2^53 without losing precision.
- Only UTF-8 is supported (which includes ASCII), no UTF-16, no UTF-32, no ISO 8859 or Windows character encodings.
- Unquoted keys can be [ECMAScript Identifier Names](https://262.ecma-international.org/5.1/#sec-7.6) with Unicode 10.0 characters (because that is the version used in the reference implementation).
- Parsing objects with unquoted non-ASCII keys may take significantly longer as a linear search is performed for verifying the Unicode category of each character. Consider quoting the keys or sticking to ASCII.
- Stringifying objects with non-ASCII keys may also take longer with no way around it yet. Consider using ASCII keys.
- In JavaScript, you can pass a reviver function to transform JSON into your own datastructure during parse. This does not really work in C++ so it was left out. Transformation into your datastructure has to be done afterwards.
- There is also no replacer function for filtering and transforming your JSON data during stringification. That must be done beforehand.

## ToDos
- Provide fromJson and toJson facilities for basic STL types.
- Put a how-to-use demo into the readme.
- Implement integer data types (how to decide if a parsed integer is int64_t or uint64_t?).
- Implement a more efficient way of verifying identifier name Unicode categories (or provide an option to skip that entirely).
- Make the peek() function caching for speed up.
- Implement a replacer function for stringification if somebody needs that.
- Implement UTF-16 and UTF-32 encoding and decoding if somebody needs that.

## License
MIT

## Pleasantries
I hope you enjoy almost the elegance of JSON combined with the utility of comments, NaN and Infinity (among other actually really useful tweaks), all delivered with the performance of C++! Feel free to report issues and request pull requests!
