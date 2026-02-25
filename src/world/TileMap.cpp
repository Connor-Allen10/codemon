#include "TileMap.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

static std::string trim(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && std::isspace(static_cast<unsigned char>(s[a]))) a++;
    size_t b = s.size();
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) b--;
    return s.substr(a, b - a);
}

bool TileMap::inBounds(int tx, int ty) const {
    return tx >= 0 && ty >= 0 && tx < static_cast<int>(mWidth) && ty < static_cast<int>(mHeight);
}

TileType TileMap::tileAt(int tx, int ty) const {
    if (!inBounds(tx, ty)) return TileType::Wall; // out-of-bounds blocks movement
    return mTiles[static_cast<size_t>(ty) * mWidth + static_cast<size_t>(tx)];
}

bool TileMap::loadFromCSV(const std::string& csvPath, unsigned tileSize) {
    mTileSize = tileSize;

    std::ifstream in(csvPath);
    if (!in.is_open()) {
        std::cerr << "TileMap: Failed to open " << csvPath << "\n";
        return false;
    }

    std::vector<std::vector<int>> rows;
    std::string line;

    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;

        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            cell = trim(cell);
            if (cell.empty()) continue;
            row.push_back(std::stoi(cell));
        }

        if (!row.empty()) rows.push_back(row);
    }

    if (rows.empty()) {
        std::cerr << "TileMap: CSV has no data\n";
        return false;
    }

    const size_t w = rows[0].size();
    for (const auto& r : rows) {
        if (r.size() != w) {
            std::cerr << "TileMap: CSV is not rectangular\n";
            return false;
        }
    }

    mWidth = static_cast<unsigned>(w);
    mHeight = static_cast<unsigned>(rows.size());
    mTiles.assign(static_cast<size_t>(mWidth) * mHeight, TileType::Wall);

    for (unsigned y = 0; y < mHeight; ++y) {
        for (unsigned x = 0; x < mWidth; ++x) {
            int v = rows[y][x];
            TileType t = TileType::Wall;
            if (v == 0) t = TileType::Path;
            else if (v == 1) t = TileType::Grass;
            else if (v == 2) t = TileType::Wall;
            mTiles[static_cast<size_t>(y) * mWidth + x] = t;
        }
    }

    // Build vertex array (two triangles per tile = 6 verts)
    mVerts.setPrimitiveType(sf::PrimitiveType::Triangles);
    mVerts.resize(static_cast<size_t>(mWidth) * mHeight * 6);

    auto colorFor = [](TileType t) {
        switch (t) {
            case TileType::Path:  return sf::Color(60, 60, 60);
            case TileType::Grass: return sf::Color(40, 120, 40);
            case TileType::Wall:  return sf::Color(110, 110, 110);
        }
        return sf::Color::Magenta;
    };

    for (unsigned y = 0; y < mHeight; ++y) {
        for (unsigned x = 0; x < mWidth; ++x) {
            const TileType t = tileAt(static_cast<int>(x), static_cast<int>(y));
            const sf::Color c = colorFor(t);

            const float px = static_cast<float>(x * mTileSize);
            const float py = static_cast<float>(y * mTileSize);
            const float s  = static_cast<float>(mTileSize);

            sf::Vertex* tri = &mVerts[(static_cast<size_t>(y) * mWidth + x) * 6];

            tri[0].position = {px,     py};
            tri[1].position = {px + s, py};
            tri[2].position = {px + s, py + s};

            tri[3].position = {px,     py};
            tri[4].position = {px + s, py + s};
            tri[5].position = {px,     py + s};

            for (int i = 0; i < 6; ++i) tri[i].color = c;
        }
    }

    return true;
}

bool TileMap::isRectPassable(const sf::FloatRect& rect) const {
    // SFML 3 rect uses rect.position and rect.size
    const float leftPx   = rect.position.x;
    const float topPx    = rect.position.y;
    const float rightPx  = rect.position.x + rect.size.x - 1.f;
    const float bottomPx = rect.position.y + rect.size.y - 1.f;

    const int left   = static_cast<int>(std::floor(leftPx   / mTileSize));
    const int top    = static_cast<int>(std::floor(topPx    / mTileSize));
    const int right  = static_cast<int>(std::floor(rightPx  / mTileSize));
    const int bottom = static_cast<int>(std::floor(bottomPx / mTileSize));

    for (int ty = top; ty <= bottom; ++ty) {
        for (int tx = left; tx <= right; ++tx) {
            TileType t = tileAt(tx, ty);
            if (!RulesFor(t).passable) return false;
        }
    }
    return true;
}

bool TileMap::isEncounterAt(const sf::Vector2f& worldPos) const {
    const int tx = static_cast<int>(std::floor(worldPos.x / mTileSize));
    const int ty = static_cast<int>(std::floor(worldPos.y / mTileSize));
    return RulesFor(tileAt(tx, ty)).encounter;
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mVerts, states);
}