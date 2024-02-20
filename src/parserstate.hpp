
#pragma once

enum class State{
    deflt,
    comment,
    multiLineComment,
    multiLineCommentAsterisk,
    singleLineComment,
    value,
    identifierNameStartEscape,
    identifierName,
    identifierNameEscape,
    sign,
    zero,
    decimalInteger,
    decimalPointLeading,
    decimalPoint,
    decimalFraction,
    decimalExponent,
    decimalExponentSign,
    decimalExponentInteger,
    hexadecimal,
    hexadecimalInteger,
    string,
    start,
    beforePropertyName,
    afterPropertyName,
    beforePropertyValue,
    afterPropertyValue,
    beforeArrayValue,
    afterArrayValue,
    end
};
