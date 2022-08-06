// Strogatz.cpp : This file contains the 'main' function.

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Entity.h"
#include "Text.h"

int main()
{
    std::cout << "Welcome to Strogatz!\n";
    Entity entity;
    std::cout << entity.get_id() << '\n';
    Entity entity1;
    std::cout << entity1.get_id() << '\n';
    entity.propagate_beliefs();

    sf::RenderWindow window(sf::VideoMode(800, 600), "Strogatz");
    window.setActive(true);
    window.setVerticalSyncEnabled(true);
    // Never use both setVerticalSyncEnabled and setFramerateLimit at the 
    // Same time! They would badly mix and make things worse.
    //window.setFramerateLimit(60); //for if we want a specific frame rate,
    // not 100% reliable, especially for high framerates
    sf::Text title_text;
    sf::Font title_font;
    get_title_text("Welcome to Strogatz", title_text, title_font);
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // Check all the window's events that were triggered since the last
        // iteration of the loop.
        sf::Event event;
        // The pollEvent function returns true if an event was pending, or 
        // false if there was none.
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                // window closed
            case sf::Event::Closed:
                // can put "are you sure?" code here
                window.close();
                break;

            case sf::Event::LostFocus: // when the window is no longer the
                // active window
                //myGame.pause();
                break;

            case sf::Event::GainedFocus:
                //myGame.resume();
                break;

            case sf::Event::KeyPressed:
                {
                    sf::Vector2f pos{ title_text.getPosition() };
                    if (event.key.code == sf::Keyboard::D)
                    {
                        title_text.move((pos.x > 750.f) ? -5.f : 5.f, 0.f);
                    }
                    if (event.key.code == sf::Keyboard::A)
                    {
                        title_text.move((pos.x < 50.f) ? 5.f : -5.f, 0.f);
                    }
                    if (event.key.code == sf::Keyboard::S)
                    {
                        title_text.move(0.f, (pos.y > 550.f) ? -5.f : 5.f);
                    }
                    if (event.key.code == sf::Keyboard::W)
                    {
                        title_text.move(0.f, (pos.y < 50.f) ? 5.f : -5.f);
                    }

                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        std::cout << "the escape key was pressed" << '\n';
                    }
                    break;
                }
            case sf::Event::MouseButtonPressed:
            {
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    std::cout << "the right button was pressed" << '\n';
                    std::cout << "mouse x: " 
                        << event.mouseButton.x 
                        << std::endl; // get the position of the cursor
                    std::cout << "mouse y: " 
                        << event.mouseButton.y 
                        << std::endl;
                }
                break;
            }
            
            default:
                break;
            }
            // Clear the window with black color (doesnt activate until 
            // window.display() so has no immediate impact)
            window.clear(sf::Color::Black);
            /*
            
            This clear/draw/display cycle is the only good way to draw 
            things. Don't try other strategies, such as keeping pixels 
            from the previous 
            frame, "erasing" pixels, or drawing once and calling display 
            multiple times. You'll get strange results due to 
            double-buffering.

            Modern graphics hardware and APIs are really made for 
            repeated clear/draw/display cycles where everything is 
            completely refreshed at each iteration of the main loop. 
            Don't be scared to draw 1000 sprites 60 times per second, 
            you're far below the millions of triangles that your 
            computer can handle.
            
            */
            // end the current frame and takes what was drawn since the 
            // last call to display and displays it on the window
            
            // to draw to a texture instead of a window: 
            // https://www.sfml-dev.org/tutorials/2.5/graphics-draw.php#off-screen-drawing
            // for multi-threaded drawing: 
            // https://www.sfml-dev.org/tutorials/2.5/graphics-draw.php#drawing-from-threads
            // this is so you can do event handling in the main loop's
            // thread (which is advised) and rendering in another thread.
            // This is a good idea.
            
            window.draw(title_text);
            
            window.display();
        }
    }
    
    return 0;

}
