// test_challenge_loader.cpp
// Unit tests for Debug::ChallengeLoader file parsing and fallback behavior.

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "debug/ChallengeLoader.hpp"

namespace {

std::filesystem::path writeTempFile(const std::string& fileName,
									const std::string& content) {
	const auto path = std::filesystem::temp_directory_path() / fileName;
	std::ofstream out(path);
	out << content;
	out.close();
	return path;
}

} // namespace

TEST(ChallengeLoaderTest, FallsBackToDefaultsWhenFileMissing) {
	Debug::ChallengeLoader loader("/tmp/this_file_does_not_exist_12345.txt");

	EXPECT_TRUE(loader.hasAnyChallenges());
	EXPECT_EQ(loader.getChallengeCount(), 3u);
}

TEST(ChallengeLoaderTest, LoadsMultilineBlockFormat) {
	const std::string fileContent =
		"BEGIN_CHALLENGE\n"
		"PROMPT:\n"
		"You see:\n"
		"retun 0;\n"
		"\n"
		"Type the fix:\n"
		"SOLUTION:\n"
		"return 0;\n"
		"KEYWORD: return\n"
		"END_CHALLENGE\n";

	const auto filePath = writeTempFile("codemon_challenge_loader_multiline.txt", fileContent);
	Debug::ChallengeLoader loader(filePath.string());

	ASSERT_EQ(loader.getChallengeCount(), 1u);
	const auto challenge = loader.getChallengeByIndex(0);
	ASSERT_TRUE(challenge.has_value());

	EXPECT_NE(challenge->prompt.find("You see:"), std::string::npos);
	EXPECT_NE(challenge->prompt.find("retun 0;"), std::string::npos);
	EXPECT_EQ(challenge->solution, "return 0;");
	EXPECT_EQ(challenge->keywordHint, "return");

	std::error_code ec;
	std::filesystem::remove(filePath, ec);
}

TEST(ChallengeLoaderTest, LoadsLegacyPipeFormatAndDecodesEscapes) {
	const std::string fileContent =
		"Fix typo\\nretun 0;|return 0;|return\n";

	const auto filePath = writeTempFile("codemon_challenge_loader_legacy.txt", fileContent);
	Debug::ChallengeLoader loader(filePath.string());

	ASSERT_EQ(loader.getChallengeCount(), 1u);
	const auto challenge = loader.getChallengeByIndex(0);
	ASSERT_TRUE(challenge.has_value());

	EXPECT_EQ(challenge->prompt, "Fix typo\nretun 0;");
	EXPECT_EQ(challenge->solution, "return 0;");
	EXPECT_EQ(challenge->keywordHint, "return");

	std::error_code ec;
	std::filesystem::remove(filePath, ec);
}

TEST(ChallengeLoaderTest, SkipsMalformedEntriesButKeepsValidOnes) {
	const std::string fileContent =
		"# malformed legacy line missing solution\n"
		"bad_line_without_delimiters\n"
		"BEGIN_CHALLENGE\n"
		"PROMPT:\n"
		"\n"
		"SOLUTION:\n"
		"return 0;\n"
		"KEYWORD: return\n"
		"END_CHALLENGE\n"
		"BEGIN_CHALLENGE\n"
		"PROMPT:\n"
		"You see:\n"
		"if (x = 1) {\n"
		"\treturn x;\n"
		"}\n"
		"SOLUTION:\n"
		"if (x == 1) {\n"
		"\treturn x;\n"
		"}\n"
		"KEYWORD: ==\n"
		"END_CHALLENGE\n";

	const auto filePath = writeTempFile("codemon_challenge_loader_malformed.txt", fileContent);
	Debug::ChallengeLoader loader(filePath.string());

	ASSERT_EQ(loader.getChallengeCount(), 1u);
	const auto challenge = loader.getChallengeByIndex(0);
	ASSERT_TRUE(challenge.has_value());
	EXPECT_EQ(challenge->keywordHint, "==");

	std::error_code ec;
	std::filesystem::remove(filePath, ec);
}

TEST(ChallengeLoaderTest, UsesDefaultsWhenFileHasNoValidChallenges) {
	const std::string fileContent =
		"# comment only\n"
		"\n";

	const auto filePath = writeTempFile("codemon_challenge_loader_empty.txt", fileContent);
	Debug::ChallengeLoader loader(filePath.string());

	EXPECT_TRUE(loader.hasAnyChallenges());
	EXPECT_EQ(loader.getChallengeCount(), 3u);

	std::error_code ec;
	std::filesystem::remove(filePath, ec);
}
