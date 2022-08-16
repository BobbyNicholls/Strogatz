// Strogatz.cpp : This file contains the 'main' function.

#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "Text.h"
#include "utils.h"

extern const int edge_buffer{ 10 };
extern const int game_height{ 600 };
extern const int game_width{ 800 };
const float move_speed{ 200.f };

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    unsigned int link_counter{ 0 }; // just using int is better practice? see learncpp
    //EntityCircle* entities[entity_limit]{}; // consider dynamic allocation: https://www.learncpp.com/cpp-tutorial/dynamically-allocating-arrays/
    // re-sizing is computationally expensive, so set capacity here:
    std::vector<EntityCircle*> entities(entity_limit+1);
    //EntityCircle** entities{ new EntityCircle*[entity_limit] };
    id_t links[link_limit][2]{};
    std::cout << "Welcome to Strogatz!\n";
    for (int i{ 0 }; i < entity_limit; ++i)
    {
        // dynamically allocate an EntityCircle and assign the address to entity_pointer
        //EntityCircle* entity_pointer{ new EntityCircle };
        // value will be set to a null pointer if the integer allocation fails:
        EntityCircle* entity_pointer{ new (std::nothrow) EntityCircle }; 
        std::cout << "i = " << i << '\n';
        std::cout << "entities.size() = " << entities.size() << '\n';
        std::cout << "link_counter = " << link_counter << '\n';
        if (!entity_pointer) // handle case where new returned null
        {
            // Do error handling here
            std::cerr << "\n\nCould not allocate memory!!!\n\n";
        }
        else
        {
            entities[i] = entity_pointer;
            if (i == 1) link_entities(
                entities[0], entities[1], links, link_counter
            );
            if (i > 1)
            {
                add_semi_random_links(
                    entities, entities[i], links, link_counter
                );
            }
        }

    }
    
    sf::RenderWindow window(sf::VideoMode(game_width, game_height), "Strogatz");
    window.setVerticalSyncEnabled(true);
    // Never use both setVerticalSyncEnabled and setFramerateLimit at the 
    // Same time! They would badly mix and make things worse.
    // window.setFramerateLimit(60); //for if we want a specific frame rate,
    // not 100% reliable, especially for high framerates
    sf::Text title_text;
    sf::Font title_font;
    get_title_text("Welcome to Strogatz", title_text, title_font);
    
    sf::Clock clock;
    float time_counter{ 0 };
    constexpr float time_step{ 1.0f / 60.0f };

    while (window.isOpen())
    {
        time_counter += clock.restart().asSeconds();
        
        if (time_counter >= time_step)
        {
            keyboard_move_entity(entities[0]->get_shape(), move_speed, time_counter);
            for (int i{ 1 }; entities[i]; ++i)
            {
                random_move_entity(entities[i]->get_shape());
                slingshot_move_entity(entities[i]);
            }
            time_counter = 0;
        }

        // Check all the window's events that were triggered since the last
        // iteration of the loop.
        sf::Event event;
        // The pollEvent function returns true if an event was pending, or 
        // false if there was none.
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                // can put "are you sure?" code here
                window.close();
                break;

            case sf::Event::LostFocus:
                //myGame.pause();
                break;

            case sf::Event::GainedFocus:
                //myGame.resume();
                break;

            case sf::Event::KeyPressed:
                {
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
        for (unsigned int counter{ 0 }; counter < link_counter; ++counter)
        {
            sf::Vector2f pos0{ entities[links[counter][0]]->get_shape().getPosition() };
            sf::Vector2f pos1{ entities[links[counter][1]]->get_shape().getPosition() };
            float radius0{ entities[links[counter][0]]->get_radius() };
            float radius1{ entities[links[counter][0]]->get_radius() };
            sf::Vertex line[2]{
                sf::Vertex(sf::Vector2f(pos0.x + radius0, pos0.y + radius0)),
                sf::Vertex(sf::Vector2f(pos1.x + radius1, pos1.y + radius1))
            };
            window.draw(line, 2, sf::Lines);
        }
        for (EntityCircle* entity : entities)
        {
            if (!entity) break;
            window.draw(entity->get_shape());
        }
        //for (int i{ 0 }; i < entity_limit-1; ++i)
        //{
        //    window.draw(entities[i]->get_shape());
        //}
        window.display();
    }
    
    return 0;

}
