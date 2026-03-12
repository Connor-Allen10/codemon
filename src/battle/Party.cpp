#include "Party.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <unordered_map>
#include <utility>

namespace {
std::string toLower(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});
	return value;
}

std::string trim(const std::string& text) {
	std::size_t begin = 0;
	std::size_t end = text.size();
	while (begin < end && std::isspace(static_cast<unsigned char>(text[begin]))) {
		++begin;
	}
	while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
		--end;
	}
	return text.substr(begin, end - begin);
}

bool readNamesFile(std::unordered_map<std::string, std::vector<std::string>>& namesByTexture,
				   std::initializer_list<const char*> paths) {
	for (const char* path : paths) {
		std::ifstream input(path);
		if (!input.is_open()) {
			continue;
		}

		std::string line;
		while (std::getline(input, line)) {
			const std::string cleaned = trim(line);
			if (cleaned.empty() || cleaned[0] == '#') {
				continue;
			}

			const auto commaPos = cleaned.find(',');
			if (commaPos == std::string::npos) {
				continue;
			}

			const std::string textureKey = toLower(trim(cleaned.substr(0, commaPos)));
			if (textureKey.empty()) {
				continue;
			}

			std::vector<std::string> parsedNames;
			std::size_t cursor = commaPos;
			while (true) {
				const auto openQuote = cleaned.find('"', cursor);
				if (openQuote == std::string::npos) {
					break;
				}
				const auto closeQuote = cleaned.find('"', openQuote + 1);
				if (closeQuote == std::string::npos) {
					break;
				}

				parsedNames.push_back(cleaned.substr(openQuote + 1, closeQuote - openQuote - 1));
				cursor = closeQuote + 1;
			}

			if (!parsedNames.empty()) {
				namesByTexture[textureKey] = parsedNames;
			}
		}

		return true;
	}

	return false;
}

const std::unordered_map<std::string, std::vector<std::string>>& codemonNamesByTexture() {
	static const std::unordered_map<std::string, std::vector<std::string>> names = [] {
		std::unordered_map<std::string, std::vector<std::string>> loaded;
		const bool loadedFromFile = readNamesFile(loaded, {
			"codemonNames.txt",
			"../codemonNames.txt",
			"../../codemonNames.txt"
		});

		if (!loadedFromFile || loaded.empty()) {
			loaded["forest_mon.png"] = {"Bambi", "Clover", "Douglass"};
			loaded["fire_mon.png"] = {"Blaze", "Ember", "Sparky"};
			loaded["swamp_mon.png"] = {"Murky", "Bogg", "Gloop"};
			loaded["void_mon.png"] = {"Nova", "Ether", "Starlord"};
			loaded["water_mon.png"] = {"Poseidon", "Shelly", "Flow"};
			loaded["rocky_mon.png"] = {"Rocky", "Crystal", "Jeff"};
			loaded["fire_bird_mon.png"] = {"Swooper", "Bennu", "Simorgh"};
			loaded["candy_mon.png"] = {"SweeTooth", "Candee", "Cookie"};
			loaded["zombie_mon.png"] = {"Oozey", "Jumper", "Frank"};
		}

		return loaded;
	}();
	return names;
}
}

Codemon::Codemon(std::string textureFile, std::string displayName)
	: mTextureFile(std::move(textureFile)), mDisplayName(std::move(displayName)) {}

const std::string& Codemon::textureFile() const {
	return mTextureFile;
}

const std::string& Codemon::displayName() const {
	return mDisplayName;
}

Party::Party() {
	mMembers.emplace_back("forest_mon.png", randomNameForTexture("forest_mon.png"));
}

const std::vector<Codemon>& Party::members() const {
	return mMembers;
}

bool Party::empty() const {
	return mMembers.empty();
}

const Codemon& Party::addCodemon(const std::string& textureFile) {
	mMembers.emplace_back(textureFile, randomNameForTexture(textureFile));
	return mMembers.back();
}

std::string Party::randomNameForTexture(const std::string& textureFile) {
	const auto& namesMap = codemonNamesByTexture();
	const auto key = toLower(textureFile);
	const auto it = namesMap.find(key);

	if (it != namesMap.end() && !it->second.empty()) {
		const std::vector<std::string>& options = it->second;
		const std::size_t idx = static_cast<std::size_t>(std::rand()) % options.size();
		return options[idx];
	}

	const auto dot = textureFile.find('.');
	if (dot == std::string::npos) {
		return textureFile;
	}
	return textureFile.substr(0, dot);
}
