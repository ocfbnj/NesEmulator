#include <chrono>
#include <thread>

#include "EmulatorWindow.h"

EmulatorWindow::EmulatorWindow()
    : window(sf::VideoMode{Width * PixelSize, Height * PixelSize},
             "NesEmulator",
             sf::Style::Titlebar | sf::Style::Close),
      quads(sf::PrimitiveType::Quads, 4 * Width * Height) {
    for (int i = 0; i != Height; i++) {
        for (int j = 0; j != Width; j++) {
            int n = (i * Height + j) * 4;

            int xOffset = i * PixelSize;
            int yOffset = j * PixelSize;

            quads[n + 0].position = sf::Vector2f(float(xOffset + i), float(yOffset + j));
            quads[n + 1].position = sf::Vector2f(float(xOffset + i + PixelSize + 1), float(yOffset + j));
            quads[n + 2].position = sf::Vector2f(float(xOffset + i + PixelSize + 1), float(yOffset + j + PixelSize + 1));
            quads[n + 3].position = sf::Vector2f(float(xOffset + i), float(yOffset + j + PixelSize + 1));

            setColor(i, j, sf::Color(i, j, 100));
        }
    }
}

bool EmulatorWindow::isOpen() {
    return window.isOpen();
}

void EmulatorWindow::handleEvent() {
    sf::Event event{};

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void EmulatorWindow::draw() {
    window.clear(sf::Color::Black);

    window.draw(quads);
}

void EmulatorWindow::display() {
    window.display();

    std::this_thread::sleep_for(std::chrono::milliseconds{1000 / 60});
}

void EmulatorWindow::setColor(int i, int j, sf::Color color) {
    int n = (i * Height + j) * 4;
    quads[n + 0].color = color;
    quads[n + 1].color = color;
    quads[n + 2].color = color;
    quads[n + 3].color = color;
}
