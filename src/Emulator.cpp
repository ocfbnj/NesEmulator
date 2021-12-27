#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "NesEmulator/nes/NesFile.h"

#include "Emulator.h"

Emulator::Emulator(std::string_view nesFile) {
    std::optional<Cartridge> cartridge = loadNesFile(nesFile);
    if (!cartridge.has_value()) {
        throw std::runtime_error{"Cannot load the NES ROM"};
    }

    nes.insert(std::move(cartridge.value()));
    nes.powerUp();
}

void Emulator::run() {
    sf::RenderWindow window(sf::VideoMode(256 * 6, 240 * 6), "NES Emulator");

    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(false);

    while (window.isOpen()) {
        static const std::unordered_map<sf::Keyboard::Key, Joypad::Button> player1KeyMap{
            {sf::Keyboard::J, Joypad::Button::A},
            {sf::Keyboard::K, Joypad::Button::B},
            {sf::Keyboard::Space, Joypad::Button::Select},
            {sf::Keyboard::Enter, Joypad::Button::Start},
            {sf::Keyboard::W, Joypad::Button::Up},
            {sf::Keyboard::S, Joypad::Button::Down},
            {sf::Keyboard::A, Joypad::Button::Left},
            {sf::Keyboard::D, Joypad::Button::Right},
        };

        sf::Event event{};
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (auto it = player1KeyMap.find(event.key.code); it != player1KeyMap.end()) {
                    nes.getJoypad().press(it->second);
                }
                break;
            case sf::Event::KeyReleased:
                if (auto it = player1KeyMap.find(event.key.code); it != player1KeyMap.end()) {
                    nes.getJoypad().release(it->second);
                }
                break;
            default:
                break;
            }
        }

        do {
            nes.clock();
        } while (!nes.getPPU().isFrameComplete());

        sf::Texture texture;
        texture.create(256, 240);
        texture.update(nes.getPPU().getFrame().getPixels());

        sf::Sprite sprite;
        sprite.setTexture(texture);
        sprite.scale(6.0f, 6.0f);

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
}
