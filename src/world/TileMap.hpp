#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class TileType : int {
    Path  = 0, // walkable, NOT encounterable
    Grass = 1, // walkable, encounterable
    Wall  = 2  // NOT walkable
};

struct TileRules {
    bool passable = false;
    bool encounter = false;
};

inline TileRules RulesFor(TileType t) {
    switch (t) {
        case TileType::Path:  return { true,  false };
        case TileType::Grass: return { true,  true  };
        case TileType::Wall:  return { false, false };
        default:              return { false, false };
    }
}

class TileMap : public sf::Drawable {
public:
    TileMap();
    explicit TileMap(unsigned tileSizePx);

    bool loadFromCSV(const std::string& csvPath, unsigned tileSizePx);

    unsigned getTileSize() const { return mTileSize; }
    unsigned getWidth() const { return mWidth; }
    unsigned getHeight() const { return mHeight; }

    bool isRectPassable(const sf::FloatRect& rect) const;
    bool overlapsImpassable(const sf::FloatRect& rect) const;
    bool isEncounterAt(const sf::Vector2f& worldPos) const;

private:
    bool inBounds(int tx, int ty) const;
    TileType tileAt(int tx, int ty) const;

    void rebuildMesh();
    sf::Color colorFor(TileType t) const;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    unsigned mTileSize = 48;
    unsigned mWidth = 0;
    unsigned mHeight = 0;

    std::vector<TileType> mTiles;
    sf::VertexArray mVerts;

    sf::Texture mGrassTexture;
    sf::Texture mPathTexture;
};