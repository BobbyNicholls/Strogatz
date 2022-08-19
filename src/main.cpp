// Strogatz.cpp : This file contains the 'main' function.

#include <iostream>
#include <random>
#include <set>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "Graph.h"
#include "Text.h"
#include "utils.h"

extern const int edge_buffer{ 10 };
extern const int game_height{ 600 };
extern const int game_width{ 800 };
constexpr float move_speed{ 200.f };

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Graph graph{ get_graph() };

    sf::RenderWindow window(sf::VideoMode(game_width, game_height), "Strogatz");
    window.setVerticalSyncEnabled(true);
    // Never use both setVerticalSyncEnabled and setFramerateLimit at the 
    // Same time! They would badly mix and make things worse.
    sf::Text text;
    sf::Font font;
    std::string time_str{ "Time: " };
    get_text(time_str.append("0"), text, font);
    
    sf::Clock clock;
    float time_counter{ 0 };
    constexpr float time_step{ 1.0f / 60.0f };
    unsigned int time_period_counter{ 0 };
    unsigned int frame_counter{ 0 };
    constexpr unsigned int frames_per_period{ 600 };

    while (window.isOpen())
    {
        time_counter += clock.restart().asSeconds();
        
        if (frame_counter >= frames_per_period)
        {
            frame_counter = 0;
            time_str = time_str.substr(0, 6);
            text.setString(time_str.append(std::to_string(++time_period_counter)));
            graph.entities[0]->print_beliefs();
            for (int i{ 0 }; i < entity_limit-1; ++i)
            {
                graph.entities[graph.links[i][1]]->update_beliefs(
                    graph.entities[graph.links[i][0]]
                );
                graph.entities[graph.links[i][1]]->update_colour();
                graph.entities[graph.links[i][1]]->print_beliefs();
            }
        }

        if (time_counter >= time_step)
        {

            ++frame_counter;
            sf::Event event;

            // Clear the window with black color (doesnt activate until 
            // window.display(), so has no immediate impact)
            window.clear(sf::Color::Black);

            for (unsigned int i{ 0 }; i < graph.link_counter; ++i)
            {
                sf::Vector2f pos0{ graph.entities[graph.links[i][0]]->get_shape().getPosition() };
                sf::Vector2f pos1{ graph.entities[graph.links[i][1]]->get_shape().getPosition() };
                float radius0{ graph.entities[graph.links[i][0]]->get_radius() };
                float radius1{ graph.entities[graph.links[i][0]]->get_radius() };
                sf::Vertex line[2]{
                    sf::Vertex(sf::Vector2f(pos0.x + radius0, pos0.y + radius0)),
                    sf::Vertex(sf::Vector2f(pos1.x + radius1, pos1.y + radius1))
                };
                window.draw(line, 2, sf::Lines);
            }

            keyboard_move_entity(graph.entities[0]->get_shape(), move_speed, time_counter);
            window.draw(graph.entities[0]->get_shape());
            for (EntityCircle* entity :  graph.entities) // more efficient to iterate implicitly?
            {
                //random_move_entity(entity->get_shape());
                //slingshot_move_entity(entity);
                window.draw(entity->get_shape());
            }

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

            // to draw to a texture instead of a window: 
            // https://www.sfml-dev.org/tutorials/2.5/graphics-draw.php#off-screen-drawing
            // for multi-threaded drawing: 
            // https://www.sfml-dev.org/tutorials/2.5/graphics-draw.php#drawing-from-threads
            // this is so you can do event handling in the main loop's
            // thread (which is advised) and rendering in another thread.
            // This is a good idea.

            // should only be doing this once every frame:
            window.draw(text);
            window.display();
            time_counter = 0;
        }
    }
    
    return 0;

}
