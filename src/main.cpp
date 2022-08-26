// Strogatz.cpp : This file contains the 'main' function.

#include <iostream>
#include <numeric>
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
constexpr int window_height{ 600 };
constexpr int window_width{ 800 };


bool check_for_double_linkage(Graph& graph)
{
    bool return_val { false };
    id_t link_problems[link_limit][2]{};
    for (unsigned int i{ 0 }; i < graph.link_counter; ++i) 
    {
        id_t a{ graph.links[i][0] };
        id_t b{ graph.links[i][1] };
        if (a == b)
        {
            link_problems[i][0] = 1;
            return_val = true;
        }
        for (unsigned int j{ i+1 }; j < graph.link_counter; ++j)
        {
            id_t a_compare{ graph.links[j][0] };
            id_t b_compare{ graph.links[j][1] };
            if ((a == a_compare) && (b == b_compare))
            {
                link_problems[i][1] += 1;
                return_val = true;
            }
        }
    }
    return return_val;
}


int main()
{

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Strogatz");
    window.setVerticalSyncEnabled(true);
    sf::Text text;
    sf::Font font;
    std::string time_str{ "Time: " };
    get_text(time_str.append("0"), text, font);
    
    sf::Clock clock;
    float time_counter{ 0 };
    constexpr float time_step{ 1.0f / 60.0f };
    time_period_t time_period_counter{ 0 };
    unsigned int frame_counter{ 0 };
    constexpr unsigned int frames_per_period{ 60 };

    Graph graph{ get_barabasi_albert_graph(time_period_counter) };

    while (window.isOpen())
    {
        time_counter += clock.restart().asSeconds();

        if (time_counter >= time_step)
        {

            if (frame_counter >= frames_per_period)
            {
                frame_counter = 0;
                time_str = time_str.substr(0, 6);
                text.setString(time_str.append(std::to_string(++time_period_counter)));
                // we iterate over links twice in one frame unnecessarily due to this:
                forward_propagate_beliefs(graph);
                if (uniform_distribution_float(0,1) < graph.rewire_prob) rewire_random_edge(graph);
                //add_random_edge(graph, static_cast<int>(graph.entities.size()-1));

                if (time_period_counter % 10 == 0) propagate_entities(
                    graph, time_period_counter
                );

                if (check_for_double_linkage(graph))
                {
                    std::cout << "PROBLEM!";
                }

            }

            ++frame_counter;
            sf::Event event;

            // Clear the window with black color (doesnt activate until 
            // window.display(), so has no immediate impact)
            window.clear(sf::Color::Black);
            keyboard_move_entity(graph.entities[0]->get_shape(), move_speed, time_counter);
            draw_links(graph, window);
            draw_entities(graph, window);
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
