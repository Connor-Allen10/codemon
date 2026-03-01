#include "TileMap.hpp"

#include <fstream>
#include <sstream>
#include <cctype>
#include <cmath>
#include <algorithm>

TileMap::TileMap() : TileMap(48) {}

TileMap::TileMap(unsigned tileSizePx)
: mTileSize(tileSizePx) {
    mVerts.setPrimitiveType(sf::PrimitiveType::Triangles);
}

bool TileMap::inBounds(int tx, int ty) const {
    return tx >= 0 && ty >= 0
        && tx < static_cast<int>(mWidth)
        && ty < static_cast<int>(mHeight);
}

TileType TileMap::tileAt(int tx, int ty) const {
    if (!inBounds(tx, ty)) return TileType::Wall; // out-of-bounds is solid
    return mTiles[static_cast<size_t>(ty) * mWidth + static_cast<size_t>(tx)];
}

sf::Color TileMap::colorFor(TileType t) const {
    // Path  = light tan
    // Grass = lighter non-neon green
    // Wall  = dark forest green
    switch (t) {
        case TileType::Path:  return sf::Color(210, 190, 140); // tan
        case TileType::Grass: return sf::Color(120, 170, 120); // soft green
        case TileType::Wall:  return sf::Color(20,  70,  30);  // forest green
        default:              return sf::Color::Magenta;
    }
}

bool TileMap::loadFromCSV(const std::string& csvPath, unsigned tileSizePx) {
    mTileSize = tileSizePx;

    std::ifstream file(csvPath);
    if (!file.is_open()) return false;

    std::vector<std::vector<int>> rows;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            // trim
            cell.erase(cell.begin(), std::find_if(cell.begin(), cell.end(),
                [](unsigned char ch){ return !std::isspace(ch); }));
            cell.erase(std::find_if(cell.rbegin(), cell.rend(),
                [](unsigned char ch){ return !std::isspace(ch); }).base(), cell.end());

            if (cell.empty()) continue;
            row.push_back(std::stoi(cell));
        }

        if (!row.empty()) rows.push_back(row);
    }

    if (rows.empty()) return false;

    const int h = static_cast<int>(rows.size());
    const int w = static_cast<int>(rows[0].size());

    for (const auto& r : rows) {
        if (static_cast<int>(r.size()) != w) return false;
    }

    mWidth = static_cast<unsigned>(w);
    mHeight = static_cast<unsigned>(h);
    mTiles.assign(static_cast<size_t>(mWidth) * mHeight, TileType::Wall);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int v = rows[y][x];
            TileType t = TileType::Wall;
            if (v == 0) t = TileType::Path;
            else if (v == 1) t = TileType::Grass;
            else if (v == 2) t = TileType::Wall;

            mTiles[static_cast<size_t>(y) * mWidth + static_cast<size_t>(x)] = t;
        }
    }

    rebuildMesh();
    return true;
}

void TileMap::rebuildMesh() {
    mVerts.clear();
    mVerts.setPrimitiveType(sf::PrimitiveType::Triangles);

    const float ts = static_cast<float>(mTileSize);

    // 6 verts per tile (2 triangles)
    mVerts.resize(static_cast<size_t>(mWidth) * mHeight * 6);

    size_t i = 0;
    for (unsigned y = 0; y < mHeight; ++y) {
        for (unsigned x = 0; x < mWidth; ++x) {
            const TileType t = tileAt(static_cast<int>(x), static_cast<int>(y));
            const sf::Color c = colorFor(t);

            const float px = static_cast<float>(x) * ts;
            const float py = static_cast<float>(y) * ts;

            // SFML 3: no Vertex(position,color) ctor — assign fields manually
            sf::Vertex v0; v0.position = {px,      py     }; v0.color = c;
            sf::Vertex v1; v1.position = {px + ts, py     }; v1.color = c;
            sf::Vertex v2; v2.position = {px + ts, py + ts}; v2.color = c;
            sf::Vertex v3; v3.position = {px,      py + ts}; v3.color = c;

            // two triangles: (v0, v1, v2) and (v0, v2, v3)
            mVerts[i + 0] = v0;
            mVerts[i + 1] = v1;
            mVerts[i + 2] = v2;

            mVerts[i + 3] = v0;
            mVerts[i + 4] = v2;
            mVerts[i + 5] = v3;

            i += 6;
        }
    }
}

bool TileMap::isRectPassable(const sf::FloatRect& rect) const {
#if SFML_VERSION_MAJOR >= 3
    const float leftF   = rect.position.x;
    const float topF    = rect.position.y;
    const float rightF  = rect.position.x + rect.size.x - 1.f;
    const float bottomF = rect.position.y + rect.size.y - 1.f;
#else
    const float leftF   = rect.left;
    const float topF    = rect.top;
    const float rightF  = rect.left + rect.width - 1.f;
    const float bottomF = rect.top + rect.height - 1.f;
#endif

    const int left   = static_cast<int>(std::floor(leftF   / mTileSize));
    const int top    = static_cast<int>(std::floor(topF    / mTileSize));
    const int right  = static_cast<int>(std::floor(rightF  / mTileSize));
    const int bottom = static_cast<int>(std::floor(bottomF / mTileSize));

    for (int ty = top; ty <= bottom; ++ty) {
        for (int tx = left; tx <= right; ++tx) {
            const TileType t = tileAt(tx, ty);
            if (!RulesFor(t).passable) return false;
        }
    }
    return true;
}

bool TileMap::overlapsImpassable(const sf::FloatRect& rect) const {
    return !isRectPassable(rect);
}

bool TileMap::isEncounterAt(const sf::Vector2f& worldPos) const {
    const int tx = static_cast<int>(std::floor(worldPos.x / mTileSize));
    const int ty = static_cast<int>(std::floor(worldPos.y / mTileSize));
    const TileType t = tileAt(tx, ty);
    return RulesFor(t).encounter;
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mVerts, states);
}