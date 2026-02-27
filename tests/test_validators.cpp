// test_validators.cpp
// Unit tests for the Validators namespace helper functions.
//
// These tests verify the string normalization and comparison logic used
// throughout the debug challenge validation system.  The helpers ensure that
// players don't have to match formatting/case exactly to have their fixes
// accepted.

#include <gtest/gtest.h>
#include "debug/Validators.hpp"

using namespace Validators;

// Test that the sanitize function removes all whitespace (spaces, tabs,
// newlines) and converts the string to lowercase.
TEST(ValidatorsTest, SanitizeRemovesWhitespace) {
    std::string raw = "  Hello \n World\t";
    EXPECT_EQ(sanitize(raw), "helloworld");
}

// Test that exactMatch correctly identifies logically equivalent code strings
// despite differences in spacing and case.  The solution "if(x==0){return; }"
// and input "IF ( x == 0 ) { RETURN ; }" both normalize to the same string,
// so they should match.
TEST(ValidatorsTest, ExactMatchIgnoresSpacingCase) {
    std::string sol = "if(x==0){return; }";
    std::string input = "IF ( x == 0 ) { RETURN ; }";
    EXPECT_TRUE(exactMatch(input, sol));
}

// Test that containsKeyword searches for a substring after normalization.
// The code contains "bar" but not "baz", so the first call should return
// true and the second false.
TEST(ValidatorsTest, ContainsKeywordWorks) {
    std::string code = "void foo() { bar(); }";
    EXPECT_TRUE(containsKeyword(code, "bar"));
    EXPECT_FALSE(containsKeyword(code, "baz"));
}