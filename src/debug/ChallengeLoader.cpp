/**
 * @file ChallengeLoader.cpp
 * @brief Implementation of dynamic challenge loading system.
 */

#include "ChallengeLoader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace Debug {

ChallengeLoader::ChallengeLoader(const std::string& filePath) {
    // Try to load from file first
    if (!filePath.empty() && loadFromFile(filePath)) {
        std::cout << "[ChallengeLoader] Loaded " << mChallenges.size() 
                  << " challenges from " << filePath << "\n";
        return;
    }

    // Fallback to hardcoded defaults
    std::cerr << "[ChallengeLoader] Could not load from file, using defaults\n";
    initializeDefaults();
}

bool ChallengeLoader::loadFromFile(const std::string& filePath) {
    // For now, implement a simple line-based format
    // Format: prompt|solution|keyword (one challenge per line)
    // TODO: Replace with proper JSON parsing in future iteration
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    mChallenges.clear();
    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Parse line: prompt|solution|keyword
        std::istringstream iss(line);
        std::string prompt, solution, keyword;

        if (!std::getline(iss, prompt, '|')) continue;
        if (!std::getline(iss, solution, '|')) continue;
        std::getline(iss, keyword, '|'); // keyword is optional

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
        Challenge{
            "#inclde <iostream>\n int main(){\n    std::cout << \"Hello!\"; }",
            "#include <iostream>\n int main(){\n    std::cout << \"Hello!\"; }",
            ""
        },
        Challenge{
            "void push_front(int value) {\n    Node* new_node = new Node;\n    new_node->data = value;\n    new_node->next = head\n    head = new_node;\n    length++;\n}",
            "void push_front(int value) {\nNode* new_node = new Node;\nnew_node->data = value;\nnew_node->next = head;\nhead = new_node;\nlength++;\n}",
            ""
        },
        Challenge{
            "void sayhello(int n) {\n    if (x = 5) {\n    std::cout << \"Hello! \" << std::endl;\n    }\n}",
            "void sayhello(int n) {\n    if (x == 5) {\n    std::cout << \"Hello! \" << std::endl;\n    }\n}",
            ""
        },
        /*
        Challenge{
            "Fix bug: missing semicolon after 'int x = 5'",
            "int x = 5;",
            ";"
        },
        Challenge{
            "Fix bug: 'cout' should be 'std::cout'",
            "std::cout << \"Hello\";",
            "std::"
        },
        Challenge{
            "Fix bug: array index starts at 0, not 1",
            "arr[0]",
            "0"
        },
        Challenge{
            "Fix bug: use '==' not '=' in if condition",
            "if (x == 5)",
            "=="
        },
        Challenge{
            "Fix bug: close string with double quote",
            "\"Hello World\"",
            "\""
        },
        Challenge{
            "Fix bug: '#inclde' should be '#include'",
            "#include",
            "#include"
        },
        Challenge{
            "Fix bug: 'deleet' keyword should be 'delete'",
            "delete ptr;",
            "delete"
        }
        */
    };

    std::cout << "[ChallengeLoader] Initialized " << mChallenges.size() 
              << " default challenges\n";
}

std::optional<Challenge> ChallengeLoader::getRandomChallenge() const {
    if (mChallenges.empty()) {
        return std::nullopt;
    }

    std::uniform_int_distribution<size_t> dist(0, mChallenges.size() - 1);
    return mChallenges[dist(mRng)];
}

std::optional<Challenge> ChallengeLoader::getChallengeByIndex(size_t index) const {
    if (index >= mChallenges.size()) {
        return std::nullopt;
    }
    return mChallenges[index];
}

} // namespace Debug
