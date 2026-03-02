#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <filesystem>

// Your header lives at src/world/TileMap.hpp
#include "world/TileMap.hpp"

static std::string writeTempCSV(const std::string& contents) {
    namespace fs = std::filesystem;
    fs::path p = fs::temp_directory_path() / "codemon_tilemap_test.csv";

    std::ofstream out(p.string(), std::ios::trunc);
    out << contents;
    out.close();

    return p.string();
}

// --- UNIT/COMPONENT TEST ---
// Verifies CSV parsing + tile rules via public API (dimensions, encounters, passability)
TEST(TileMapTest, LoadFromCSV_BasicAndRulesWork) {
    // 0 = Path (passable, no encounter)
    // 1 = Grass (passable, encounter)
    // 2 = Wall (blocked)
    //
    // 3x2 map:
    // row0: 0,1,2
    // row1: 2,0,1
    const std::string csv =
        "0,1,2\n"
        "2,0,1\n";

    const std::string path = writeTempCSV(csv);

    TileMap map;
    ASSERT_TRUE(map.loadFromCSV(path, /*tileSize=*/10));

    EXPECT_EQ(map.getWidth(), 3u);
    EXPECT_EQ(map.getHeight(), 2u);
    EXPECT_EQ(map.getTileSize(), 10u);

    // Encounter checks (test points inside each tile)
    // Tile (1,0) is Grass => encounter true
    EXPECT_TRUE(map.isEncounterAt(sf::Vector2f(1 * 10.f + 1.f, 0 * 10.f + 1.f)));

    // Tile (0,0) is Path => encounter false
    EXPECT_FALSE(map.isEncounterAt(sf::Vector2f(0 * 10.f + 1.f, 0 * 10.f + 1.f)));

    // Passability checks using a small rect inside a tile
    // Tile (2,0) is Wall => not passable
    EXPECT_FALSE(map.isRectPassable(sf::FloatRect({2 * 10.f + 1.f, 0 * 10.f + 1.f}, {2.f, 2.f})));

    // Tile (1,1) is Path => passable
    EXPECT_TRUE(map.isRectPassable(sf::FloatRect({1 * 10.f + 1.f, 1 * 10.f + 1.f}, {2.f, 2.f})));
}

// --- VALIDATION TEST ---
// Non-rectangular CSV should fail (your implementation explicitly checks this)
TEST(TileMapTest, LoadFromCSV_FailsOnNonRectangularCSV) {
    // First row has 3 entries, second has 2 => should fail
    const std::string csv =
        "0,1,2\n"
        "2,0\n";

    const std::string path = writeTempCSV(csv);

    TileMap map;
    EXPECT_FALSE(map.loadFromCSV(path, /*tileSize=*/10));
}

// --- VALIDATION TEST ---
// Out-of-bounds should block movement (tileAt returns Wall when out-of-bounds)
TEST(TileMapTest, IsRectPassable_FalseWhenOutOfBounds) {
    const std::string csv =
        "0,0\n"
        "0,0\n";
    const std::string path = writeTempCSV(csv);

    TileMap map;
    ASSERT_TRUE(map.loadFromCSV(path, /*tileSize=*/10));

    // Rect partially outside left/top => should be not passable
    EXPECT_FALSE(map.isRectPassable(sf::FloatRect({-1.f, -1.f}, {2.f, 2.f})));
}

// --- REGRESSION TEST ---
// Degenerate collision rectangles should not be considered passable.
TEST(TileMapTest, IsRectPassable_FalseForDegenerateRect) {
    const std::string csv =
        "0,0\n"
        "0,0\n";
    const std::string path = writeTempCSV(csv);

    TileMap map;
    ASSERT_TRUE(map.loadFromCSV(path, /*tileSize=*/10));

    EXPECT_FALSE(map.isRectPassable(sf::FloatRect({5.f, 5.f}, {0.f, 2.f})));
    EXPECT_FALSE(map.isRectPassable(sf::FloatRect({5.f, 5.f}, {2.f, 0.f})));
}
