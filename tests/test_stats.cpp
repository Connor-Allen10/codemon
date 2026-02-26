#include <gtest/gtest.h>

// --- UNIT TEST ---
// Checks if the basic math for taking damage is correct
TEST(StatsTest, HealthReduction) {
    int currentHP = 100;
    int damageTaken = 40;
    currentHP -= damageTaken;
    
    // Expect currentHP to be exactly 60
    EXPECT_EQ(currentHP, 60);
}

// --- VALIDATION TEST ---
// Checks that our logic handles edge cases (preventing negative health)
TEST(StatsTest, HealthFloorValidation) {
    int currentHP = 20;
    int damageTaken = 50;
    currentHP -= damageTaken;
    
    // Logic we want to test: HP should never stay below 0
    if (currentHP < 0) {
        currentHP = 0;
    }
    
    EXPECT_EQ(currentHP, 0);
}