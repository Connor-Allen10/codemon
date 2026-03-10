/**
 * @file test_challenge_loader.cpp
 * @brief Unit tests for ChallengeLoader class.
 */

#include <gtest/gtest.h>
#include "../src/debug/ChallengeLoader.hpp"
#include <fstream>
#include <filesystem>

// Test that loader initializes with defaults when file not found
TEST(ChallengeLoaderTest, InitializesWithDefaults) {
    Debug::ChallengeLoader loader("nonexistent_file.txt");
    
    // Should have fallback defaults
    EXPECT_GT(loader.getChallengeCount(), 0);
    
    // Should be able to get random challenge
    auto challenge = loader.getRandomChallenge();
    EXPECT_TRUE(challenge.has_value());
    EXPECT_FALSE(challenge->prompt.empty());
    EXPECT_FALSE(challenge->solution.empty());
}

// Test that loader can parse valid challenge file
TEST(ChallengeLoaderTest, LoadsFromValidFile) {
    // Create temporary test file
    const std::string testFile = "test_challenges_temp.txt";
    {
        std::ofstream out(testFile);
        out << "# Test challenges\n";
        out << "Fix typo: retun|return 0;|return\n";
        out << "Add semicolon: int x = 5|int x = 5;|;\n";
        out << "\n"; // Empty line should be ignored
        out << "# Comment line\n";
        out << "Fix cout: cout|std::cout << \"Hi\";|std::\n";
    }
    
    Debug::ChallengeLoader loader(testFile);
    
    // Should have loaded 3 challenges (comments/blank lines ignored)
    EXPECT_EQ(loader.getChallengeCount(), 3);
    
    // Verify first challenge
    auto challenge = loader.getChallengeByIndex(0);
    ASSERT_TRUE(challenge.has_value());
    EXPECT_EQ(challenge->prompt, "Fix typo: retun");
    EXPECT_EQ(challenge->solution, "return 0;");
    EXPECT_EQ(challenge->keywordHint, "return");
    
    // Cleanup
    std::filesystem::remove(testFile);
}

// Test getChallengeByIndex bounds checking
TEST(ChallengeLoaderTest, GetByIndexBoundsCheck) {
    Debug::ChallengeLoader loader("nonexistent.txt");
    
    size_t count = loader.getChallengeCount();
    ASSERT_GT(count, 0);
    
    // Valid index should return challenge
    auto valid = loader.getChallengeByIndex(0);
    EXPECT_TRUE(valid.has_value());
    
    // Out of bounds should return nullopt
    auto invalid = loader.getChallengeByIndex(count + 100);
    EXPECT_FALSE(invalid.has_value());
}

// Test that random selection returns different challenges
TEST(ChallengeLoaderTest, RandomSelectionVaries) {
    Debug::ChallengeLoader loader("nonexistent.txt");
    
    // Get 10 random challenges and check they're not all identical
    std::set<std::string> prompts;
    for (int i = 0; i < 10; ++i) {
        auto challenge = loader.getRandomChallenge();
        ASSERT_TRUE(challenge.has_value());
        prompts.insert(challenge->prompt);
    }
    
    // With 8 defaults, we should see at least 2 different prompts in 10 tries
    EXPECT_GE(prompts.size(), 2);
}

// Test malformed file handling
TEST(ChallengeLoaderTest, HandlesMalformedLines) {
    const std::string testFile = "test_malformed_temp.txt";
    {
        std::ofstream out(testFile);
        out << "Valid challenge|return 0;|return\n";
        out << "Missing solution field\n"; // Only one field - should be skipped
        out << "|empty prompt|keyword\n"; // Empty prompt - should be skipped
        out << "Good prompt|good solution|\n"; // Empty keyword is OK
    }
    
    Debug::ChallengeLoader loader(testFile);
    
    // Should have loaded only 2 valid challenges (first and last)
    EXPECT_EQ(loader.getChallengeCount(), 2);
    
    // Cleanup
    std::filesystem::remove(testFile);
}

// Test that keyword hint can be optional
TEST(ChallengeLoaderTest, OptionalKeywordHint) {
    const std::string testFile = "test_optional_keyword.txt";
    {
        std::ofstream out(testFile);
        out << "With keyword|solution|keyword\n";
        out << "Without keyword|solution|\n";
        out << "Also without|solution\n"; // Missing third field entirely
    }
    
    Debug::ChallengeLoader loader(testFile);
    
    EXPECT_EQ(loader.getChallengeCount(), 3);
    
    auto withKeyword = loader.getChallengeByIndex(0);
    EXPECT_EQ(withKeyword->keywordHint, "keyword");
    
    auto withoutKeyword1 = loader.getChallengeByIndex(1);
    EXPECT_TRUE(withoutKeyword1->keywordHint.empty());
    
    auto withoutKeyword2 = loader.getChallengeByIndex(2);
    EXPECT_TRUE(withoutKeyword2->keywordHint.empty());
    
    // Cleanup
    std::filesystem::remove(testFile);
}
