#pragma once

#include <string>

using namespace std;

enum TokenType {
    ACCESS,
    DECLARE,
    SCOPE,
    STARTSCOPE,
    ENDSCOPE,
    UNKNOWN
};
const string TOKENS[] = { "ACCESS", "DECLARE", "SCOPE","{", "}", "UNKNOWN" };
const char CH_END_EXPRESSION = ';';
const string ROOT_SCOPE = "Root";