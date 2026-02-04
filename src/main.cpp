#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Codemon");
    while (window.isOpen()) {
        while (auto e = window.pollEvent()) {
            if (e->is<sf::Event::Closed>()) window.close();
        }
        window.clear();
        window.display();
    }
}
