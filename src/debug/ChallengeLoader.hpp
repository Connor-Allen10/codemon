/**
 * @file ChallengeLoader.hpp
 * @brief Dynamic loading of debug challenges from text files.
 * 
 * Provides utilities to load challenges from external data files and
 * select challenges randomly or by index for battle encounters.
 *
 * Supported file formats:
 * - Multiline block format (BEGIN_CHALLENGE / PROMPT / SOLUTION / KEYWORD)
 * - Legacy single-line format (prompt|solution|keyword)
 *
 * Logging note:
 * - Loader diagnostics are quiet by default.
 * - Set CODEMON_VERBOSE_CHALLENGE_LOADER=1 to enable loader logs.
 */

#pragma once

#include "DebugChallenge.hpp"
#include <vector>
#include <string>
#include <optional>
#include <random>

namespace Debug {

/**
 * @class ChallengeLoader
 * @brief Loads and manages a pool of debug challenges.
 * 
 * Supports:
 * - Loading challenges from multiline block and legacy pipe formats
 * - Random challenge selection
 * - Fallback to hardcoded challenges if file loading fails
 */
class ChallengeLoader {
public:
    /**
     * @brief Construct a loader and attempt to load challenges from file.
     * @param filePath Path to challenge file (optional)
     * 
     * If file loading fails, initializes with hardcoded default challenges.
     */
    explicit ChallengeLoader(const std::string& filePath = "");

    /**
     * @brief Get a random challenge from the loaded pool.
     * @return A challenge, or std::nullopt if pool is empty
     */
    std::optional<Challenge> getRandomChallenge() const;

    /**
     * @brief Get a specific challenge by index.
     * @param index Zero-based index into challenge pool
     * @return Challenge at index, or std::nullopt if out of bounds
     */
    std::optional<Challenge> getChallengeByIndex(size_t index) const;

    /**
     * @brief Get total number of loaded challenges.
     */
    size_t getChallengeCount() const { return mChallenges.size(); }

    /**
     * @brief Check if any challenges are loaded.
     */
    bool hasAnyChallenges() const { return !mChallenges.empty(); }

private:
    /**
    * @brief Load challenges from challenge text file.
     * @return true if successful, false otherwise
     */
    bool loadFromFile(const std::string& filePath);

    /**
     * @brief Initialize with hardcoded fallback challenges.
     * 
     * Used when file loading fails or no file is specified.
     */
    void initializeDefaults();

    std::vector<Challenge> mChallenges;
    mutable std::mt19937 mRng{std::random_device{}()};
};

} // namespace Debug
