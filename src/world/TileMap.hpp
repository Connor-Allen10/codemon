#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include <vector>

enum class TileType : uint8_t {
    Path  = 0,   // walkable, no encounters
    Grass = 1,   // walkable, encounterable
    Wall  = 2    // blocked
};

struct TileRules {
    bool passable;
    bool encounter;
};

inline TileRules RulesFor(TileType t) {
    switch (t) {
        case TileType::Path:  return {true,  false};
        case TileType::Grass: return {true,  true };
        case TileType::Wall:  return {false, false};
    }
    return {true, false};
}

class TileMap : public sf::Drawable {
public:
    bool loadFromCSV(const std::string& csvPath, unsigned tileSize);

    unsigned tileSize() const { return mTileSize; }
    unsigned widthTiles() const { return mWidth; }
    unsigned heightTiles() const { return mHeight; }

    sf::Vector2u worldPixelSize() const { return {mWidth * mTileSize, mHeight * mTileSize}; }

    // Returns false if rect overlaps any blocked tile OR is out-of-bounds.
    bool isRectPassable(const sf::FloatRect& rect) const;

    // Returns true if world position is on a grass tile (encounterable).
    bool isEncounterAt(const sf::Vector2f& worldPos) const;

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    bool inBounds(int tx, int ty) const;
    TileType tileAt(int tx, int ty) const;

private:
    unsigned mTileSize = 48;
    unsigned mWidth = 0;
    unsigned mHeight = 0;

    std::vector<TileType> mTiles;

    // Colored geometry for fast drawing (two triangles per tile)
    sf::VertexArray mVerts;
};