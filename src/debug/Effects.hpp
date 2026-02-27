#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

/**
 * @file Effects.hpp
 * @brief Lightweight visual/audio helpers for the debug challenge overlay.
 *
 * These inline routines are called by DebugState/WorldState when a player
 * submits a solution.  They provide immediate feedback without coupling the
 * core logic to the rendering subsystem.  You may replace them with more
 * sophisticated effects (particles, music, etc.) as the project evolves.
 */

namespace DebugEffects {

/**
 * @brief Flash a semi‑transparent green rectangle over the screen.
 *
 * Used to indicate success; the rectangle is drawn in the current view and
 * disappears the next frame when the overlay is cleared.
 */
inline void playSuccess(sf::RenderTarget& target) {
    sf::RectangleShape rect(target.getView().getSize());
    rect.setFillColor(sf::Color(0, 255, 0, 80));
    target.draw(rect);
}

/**
 * @brief Flash a semi‑transparent red rectangle over the screen.
 *
 * Signifies failure or an incorrect submission.
 */
inline void playFailure(sf::RenderTarget& target) {
    sf::RectangleShape rect(target.getView().getSize());
    rect.setFillColor(sf::Color(255, 0, 0, 80));
    target.draw(rect);
}

/**
 * @brief Play a short sound effect from a file path.
 *
 * We reuse a single static buffer to keep the implementation trivial; this is
 * fine for one‑shot debug sounds but would need a richer design for general
 * audio use.
 */
inline void playSound(const std::string& path) {
    static sf::SoundBuffer buffer; // reuse single buffer for simplicity
    if (buffer.loadFromFile(path)) {
        sf::Sound sound(buffer);
        sound.play();
    }
}

} // namespace DebugEffects
