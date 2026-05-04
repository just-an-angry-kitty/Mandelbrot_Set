#include <SFML/Graphics.hpp>
#include "ComplexPlane.h"
#include <iostream>

using namespace sf;
using namespace std;


int main()
{
    // Divide by 2 for a smaller screen
    int width = VideoMode::getDesktopMode().width / 2;
    int height = VideoMode::getDesktopMode().height / 2;

    RenderWindow window(VideoMode(width, height), "Mandelbrot Set");
    window.setFramerateLimit(60);

    ComplexPlane complexPlane(width, height);

    Font font;
    if (!font.loadFromFile("minecraft-ten.ttf"))
    {
        font.loadFromFile("impact.ttf");
    }
    
    Text coolText;
    coolText.setFont(font);
    coolText.setCharacterSize(14);
    coolText.setFillColor(Color::Yellow);
    coolText.setPosition(10.0, 10.0);

    //Main loop 
    while (window.isOpen())
    {
        // Input
        Event event;
        while (window.pollEvent(event))
        {
            // Closes the window
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed)
            {
                Vector2i mousePos = { event.mouseButton.x, event.mouseButton.y };

                // Zoom in amd zoom out on clicks
                if (event.mouseButton.button == Mouse::Left)
                {
                    complexPlane.zoomIn();
                    complexPlane.setCenter(mousePos);
                }
                else if (event.mouseButton.button == Mouse::Right)
                {
                    complexPlane.zoomOut();
                    complexPlane.setCenter(mousePos);
                }
            }
            // For mouse coordinates
            if (event.type == Event::MouseMoved)
            {
                complexPlane.setMouseLocation({ event.mouseMove.x, event.mouseMove.y });
            }
        }
        // Closes the window
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        // Update 
        complexPlane.updateRender();
        complexPlane.loadText(coolText);

        //Draw 
        window.clear();
        window.draw(complexPlane);
        window.draw(coolText);
        window.display();
    }

    return 0;
}