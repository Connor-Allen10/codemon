#include <gtest/gtest.h>
#include "debug/Validators.hpp"

using namespace Validators;

TEST(ValidatorsTest, SanitizeRemovesWhitespace) {
    std::string raw = "  Hello \n World\t";
    EXPECT_EQ(sanitize(raw), "helloworld");
}

TEST(ValidatorsTest, ExactMatchIgnoresSpacingCase) {
    std::string sol = "if(x==0){return; }";
    std::string input = "IF ( x == 0 ) { RETURN ; }";
    EXPECT_TRUE(exactMatch(input, sol));
}

TEST(ValidatorsTest, ContainsKeywordWorks) {
    std::string code = "void foo() { bar(); }";
    EXPECT_TRUE(containsKeyword(code, "bar"));
    EXPECT_FALSE(containsKeyword(code, "baz"));
}