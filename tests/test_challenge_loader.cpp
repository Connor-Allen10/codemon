// test_challenge_loader.cpp
// Unit tests for Debug::ChallengeLoader file parsing and fallback behavior.

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <string>
#include <cstdlib>		// Needed for std::getenv functions.

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

class EnvVarGuard {
public:
	// Saves and restores an environment variable so tests don't leak
	// process-wide env changes into other tests.
	explicit EnvVarGuard(const std::string& key)
	: mKey(key)
	, mHadOriginal(false) {
		if (const char* existing = std::getenv(mKey.c_str()); existing != nullptr) {
			mHadOriginal = true;
			mOriginal = existing;
		}
	}

	~EnvVarGuard() {
		if (mHadOriginal) {
			set(mOriginal);
		} else {
			unset();
		}
	}

	void set(const std::string& value) {
#ifdef _WIN32
		_putenv_s(mKey.c_str(), value.c_str());
#else
		setenv(mKey.c_str(), value.c_str(), 1);
#endif
	}

	void unset() {
#ifdef _WIN32
		_putenv_s(mKey.c_str(), "");
#else
		unsetenv(mKey.c_str());
#endif
	}

private:
	std::string mKey;
	std::string mOriginal;
	bool mHadOriginal;
};

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

TEST(ChallengeLoaderTest, LoadsBundledChallengesFromCanonicalDataPath) {
	const std::initializer_list<const char*> candidates = {
		"assets/data/challenges.txt",
		"src/assets/data/challenges.txt",
		"../assets/data/challenges.txt",
		"../src/assets/data/challenges.txt"
	};

	std::string resolvedPath;
	for (const char* path : candidates) {
		if (std::filesystem::exists(path)) {
			resolvedPath = path;
			break;
		}
	}

	ASSERT_FALSE(resolvedPath.empty()) << "Expected bundled challenges file in canonical data locations";

	Debug::ChallengeLoader loader(resolvedPath);
	EXPECT_TRUE(loader.hasAnyChallenges());
	EXPECT_GT(loader.getChallengeCount(), 3u);
}

TEST(ChallengeLoaderTest, VerboseToggleOffSuppressesLoaderLogs) {
	// With toggle unset, ChallengeLoader should remain quiet even when
	// it falls back to defaults.
	EnvVarGuard guard("CODEMON_VERBOSE_CHALLENGE_LOADER");
	guard.unset();

	testing::internal::CaptureStdout();
	testing::internal::CaptureStderr();

	Debug::ChallengeLoader loader("/tmp/this_file_does_not_exist_verbose_off.txt");
	(void)loader;

	const std::string out = testing::internal::GetCapturedStdout();
	const std::string err = testing::internal::GetCapturedStderr();

	EXPECT_EQ(out.find("[ChallengeLoader]"), std::string::npos);
	EXPECT_EQ(err.find("[ChallengeLoader]"), std::string::npos);
}

TEST(ChallengeLoaderTest, VerboseToggleOnEmitsLoaderLogs) {
	// With toggle enabled, loader diagnostics should be emitted.
	EnvVarGuard guard("CODEMON_VERBOSE_CHALLENGE_LOADER");
	guard.set("1");

	testing::internal::CaptureStdout();
	testing::internal::CaptureStderr();

	Debug::ChallengeLoader loader("/tmp/this_file_does_not_exist_verbose_on.txt");
	(void)loader;

	const std::string out = testing::internal::GetCapturedStdout();
	const std::string err = testing::internal::GetCapturedStderr();

	EXPECT_NE(out.find("Initialized 3 default challenges"), std::string::npos);
	EXPECT_NE(err.find("Could not load from file, using defaults"), std::string::npos);
}
