/**
 * @file ChallengeLoader.cpp
 * @brief Implementation of dynamic challenge loading system.
 */

#include "ChallengeLoader.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Debug {

namespace {

std::string trim(std::string s) {
	const auto notSpace = [](unsigned char c) { return !std::isspace(c); };

	while (!s.empty() && !notSpace(static_cast<unsigned char>(s.front()))) {
		s.erase(s.begin());
	}
	while (!s.empty() && !notSpace(static_cast<unsigned char>(s.back()))) {
		s.pop_back();
	}
	return s;
}

bool startsWith(const std::string& s, const std::string& prefix) {
	return s.rfind(prefix, 0) == 0;
}

std::string decodeEscapes(const std::string& input) {
	std::string out;
	out.reserve(input.size());

	for (std::size_t i = 0; i < input.size(); ++i) {
		if (input[i] == '\\' && i + 1 < input.size()) {
			switch (input[i + 1]) {
				case 'n': out.push_back('\n'); ++i; continue;
				case 't': out.push_back('\t'); ++i; continue;
				case 'r': out.push_back('\r'); ++i; continue;
				case '\\': out.push_back('\\'); ++i; continue;
				default: break;
			}
		}
		out.push_back(input[i]);
	}

	return out;
}

void appendLine(std::string& dst, const std::string& line) {
	if (!dst.empty()) {
		dst.push_back('\n');
	}
	dst += line;
}

} // namespace

ChallengeLoader::ChallengeLoader(const std::string& filePath) {
	if (!filePath.empty() && loadFromFile(filePath)) {
		std::cout << "[ChallengeLoader] Loaded " << mChallenges.size()
				  << " challenges from " << filePath << "\n";
		return;
	}

	std::cerr << "[ChallengeLoader] Could not load from file, using defaults\n";
	initializeDefaults();
}

bool ChallengeLoader::loadFromFile(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return false;
	}

	mChallenges.clear();
	std::string line;
	int lineNum = 0;

	while (std::getline(file, line)) {
		++lineNum;
		const std::string raw = trim(line);

		// Skip comments and empty lines.
		if (raw.empty() || raw[0] == '#') {
			continue;
		}

		// Multiline block format:
		// BEGIN_CHALLENGE
		// PROMPT:
		// ...any lines...
		// SOLUTION:
		// ...any lines...
		// KEYWORD: optional
		// END_CHALLENGE
		if (raw == "BEGIN_CHALLENGE") {
			std::string prompt;
			std::string solution;
			std::string keyword;

			enum class Section { None, Prompt, Solution };
			Section section = Section::None;
			bool foundEnd = false;

			while (std::getline(file, line)) {
				++lineNum;
				const std::string header = trim(line);

				if (header == "END_CHALLENGE") {
					foundEnd = true;
					break;
				}
				if (header == "PROMPT:") {
					section = Section::Prompt;
					continue;
				}
				if (header == "SOLUTION:") {
					section = Section::Solution;
					continue;
				}
				if (startsWith(header, "KEYWORD:")) {
					keyword = trim(header.substr(std::string("KEYWORD:").size()));
					continue;
				}

				if (section == Section::Prompt) {
					appendLine(prompt, line);
				} else if (section == Section::Solution) {
					appendLine(solution, line);
				}
			}

			if (!foundEnd) {
				std::cerr << "[ChallengeLoader] Unterminated BEGIN_CHALLENGE near line " << lineNum << "\n";
				continue;
			}

			prompt = trim(prompt);
			solution = trim(solution);
			if (prompt.empty() || solution.empty()) {
				std::cerr << "[ChallengeLoader] Skipping malformed multiline challenge near line " << lineNum << "\n";
				continue;
			}

			mChallenges.emplace_back(prompt, solution, keyword);
			continue;
		}

		// Legacy single-line format: prompt|solution|keyword
		std::istringstream iss(line);
		std::string prompt;
		std::string solution;
		std::string keyword;

		if (!std::getline(iss, prompt, '|')) {
			continue;
		}
		if (!std::getline(iss, solution, '|')) {
			continue;
		}
		std::getline(iss, keyword, '|');

		prompt = decodeEscapes(trim(prompt));
		solution = decodeEscapes(trim(solution));
		keyword = decodeEscapes(trim(keyword));

		if (prompt.empty() || solution.empty()) {
			std::cerr << "[ChallengeLoader] Skipping malformed line " << lineNum << "\n";
			continue;
		}

		mChallenges.emplace_back(prompt, solution, keyword);
	}

	return !mChallenges.empty();
}

void ChallengeLoader::initializeDefaults() {
	mChallenges = {
		Challenge{"Fix typo: retun 0;", "return 0;", "return"},
		Challenge{"Fix assignment in if: if (x = 5)", "if (x == 5)", "=="},
		Challenge{"Fix include typo: #inclde <iostream>", "#include <iostream>", "#include"}
	};

	std::cout << "[ChallengeLoader] Initialized " << mChallenges.size()
			  << " default challenges\n";
}

std::optional<Challenge> ChallengeLoader::getRandomChallenge() const {
	if (mChallenges.empty()) {
		return std::nullopt;
	}

	std::uniform_int_distribution<std::size_t> dist(0, mChallenges.size() - 1);
	return mChallenges[dist(mRng)];
}

std::optional<Challenge> ChallengeLoader::getChallengeByIndex(std::size_t index) const {
	if (index >= mChallenges.size()) {
		return std::nullopt;
	}
	return mChallenges[index];
}

} // namespace Debug
