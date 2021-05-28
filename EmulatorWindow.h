#ifndef EMULATOR_WINDOW
#define EMULATOR_WINDOW

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class EmulatorWindow {
public:
    EmulatorWindow();

    bool isOpen();
    void handleEvent();
    void draw();
    void display();

    void setColor(int i, int j, sf::Color color);

private:
    static constexpr auto PixelSize = 3;
    static constexpr auto Width = 256;
    static constexpr auto Height = 240;

    sf::RenderWindow window;
    sf::VertexArray quads;
};

#endif
