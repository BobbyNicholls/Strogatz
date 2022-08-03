// Strogatz.cpp : This file contains the 'main' function.

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Entity.h"

int main()
{
    std::cout << "Welcome to Strogatz!\n";
    Entity entity;
    std::cout << entity.get_id() << '\n';
    Entity entity1;
    std::cout << entity1.get_id() << '\n';
    entity.propagate_beliefs();

    sf::Window window(sf::VideoMode(800, 600), "Strogatz");
    window.setVerticalSyncEnabled(true);
    // Never use both setVerticalSyncEnabled and setFramerateLimit at the same time! They would badly mix and make things worse.
    //window.setFramerateLimit(60); //for if we want a specific frame rate, not 100% reliable, especially for high framerates

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) //The pollEvent function returns true if an event was pending, or false if there was none.
        {
            switch (event.type)
            {
                // window closed
            case sf::Event::Closed:
                // can put "are you sure?" code here
                window.close();
                break;

            default:
                break;
            }
        }
    }
    
    return 0;

}
