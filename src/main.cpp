// Strogatz.cpp : This file contains the 'main' function.

#include <iostream>
#include <numeric>
#include <random>
#include <set>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EntityCircle.h"
#include "Graph.h"
#include "Map.h"
#include "Text.h"
#include "utils.h"

extern const int edge_buffer{ 10 };
extern const int game_height{ 1200 };
extern const int game_width{ 1600 };
constexpr float move_speed{ 200.f };
constexpr int window_height{ 900 };
constexpr int window_width{ 1200 };


int main()
{

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Strogatz");
    window.setVerticalSyncEnabled(true);

    sf::Texture map_texture;
    load_texture(map_texture);

    sf::Text text;
    sf::Font font;
    std::string time_str{ "Time: " };
    get_text(time_str.append("0"), text, font);
    
    sf::Clock clock;
    float time_counter{ 0 };
    constexpr float time_step{ 1.0f / 60.0f };
    time_period_t time_period_counter{ 0 };
    unsigned int frame_counter{ 0 };
    constexpr unsigned int frames_per_period{ 600 };
    bool draw_links{ false };

    Graph graph{ time_period_counter };
    Map map{ map_texture, graph };
    EntityCircle* player_entity{ get_entity_circle(time_period_counter) };
    sf::Vector2f movement{};
    player_entity->get_shape().setPosition(window_width/2, window_height/2);

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
                // we iterate over links and entities twice in one frame unnecessarily due to this:
                if (time_period_counter % 2 == 0) graph.kill_entities(time_period_counter);
                if (time_period_counter % 20 == 0) graph.forward_propagate_beliefs();
                if (time_period_counter % 15 == 0) graph.seed_cliques_and_leaders();
                if (time_period_counter % 10 == 0)
                {
                    if (graph.is_near_link_limit()) graph.reserve_more_links();
                    graph.propagate_entities(time_period_counter);
                }
            }

            if (uniform_distribution_float(0, 1) < graph.get_rewire_prob())
                graph.rewire_random_edge();
            if (uniform_distribution_float(0, 1) < graph.get_new_edge_prob())
                graph.add_random_edge(graph.get_nr_of_entities() - 1);

            ++frame_counter;
            sf::Event event;

            // Clear the window with black color (doesnt activate until 
            // window.display(), so has no immediate impact)
            window.clear(sf::Color::Black);
            movement = get_movement(move_speed * time_counter);
            map.draw(window, movement.x, movement.y);
            if (draw_links) graph.draw_links(window);
            graph.draw_entities(window, movement.x, movement.y);
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
                    if (event.key.code == sf::Keyboard::Space)
                    {
                        draw_links = !draw_links;
                    }
                    break;
                }
                case sf::Event::MouseButtonPressed:
                {
                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        draw_links = !draw_links;
                    }
                    break;
                }

                default:
                    break;
                }

            }

            // to draw to a texture instead of a window: 
            // https://www.sfml-dev.org/tutorials/2.5/graphics-draw.php#off-screen-drawing
            // this is so you can do event handling in the main loop's
            // thread (which is advised) and rendering in another thread.
            // This is a good idea.
            window.draw(text);
            window.draw(player_entity->get_shape());
            window.display();
            time_counter = 0;
        }
    }
    return 0;
}
