/**
 * @file Party.hpp
 * @brief Tracks Codemon in Player's party.
 */

#pragma once

#include <string>
#include <vector>

class Codemon {
public:
	Codemon(std::string textureFile, std::string displayName);

	const std::string& textureFile() const;
	const std::string& displayName() const;

private:
	std::string mTextureFile;
	std::string mDisplayName;
};

class Party {
public:
	Party();

	const std::vector<Codemon>& members() const;
	bool empty() const;

	const Codemon& addCodemon(const std::string& textureFile);

private:
	static std::string randomNameForTexture(const std::string& textureFile);

	std::vector<Codemon> mMembers;

};