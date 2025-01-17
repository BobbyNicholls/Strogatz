#include "utils.h"

id_type generate_id()
{
    static id_type s_entity_id{ 0 };
    return s_entity_id++;
}


void load_texture(sf::Texture& texture, const std::string& texture_name)
{
    if (!texture.loadFromFile("textures/" + texture_name + ".png"))
    {
        std::cout << "Failed to load texture.\n";
    }
}


sf::Vector2f get_movement(const float move_distance)
{
    float x_move_distance{ 0.f };
    float y_move_distance{ 0.f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) x_move_distance += move_distance;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) x_move_distance -= move_distance;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) y_move_distance += move_distance;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) y_move_distance -= move_distance;

    return sf::Vector2f(x_move_distance, y_move_distance);
}


//bool check_for_double_linkage(Graph& graph)
//{
//    size_t link_problems[link_limit][2]{};
//    bool return_value{ false };
//    for (size_t i{ 0 }; i < graph.links.size(); ++i)
//    {
//        EntityCircle* a{ graph.links[i]->from };
//        EntityCircle* b{ graph.links[i]->to };
//        if (a == b)
//        {
//            std::cout << "self linkage: ";
//            return_value = true;
//            link_problems[i][0] = 1;
//        }
//        for (size_t j{ i + 1 }; j < graph.links.size(); ++j)
//        {
//            EntityCircle* a_compare{ graph.links[j]->from };
//            EntityCircle* b_compare{ graph.links[j]->to };
//            if ((a == a_compare) && (b == b_compare))
//            {
//                std::cout << "i: " << i << ", j: " << j << ", repeat links: ";
//                return_value = true;
//                link_problems[i][1] = 1;
//            }
//        }
//    }
//    return return_value;
//}


//bool check_for_entity_position_bug(Graph& graph)
//{
//    for (EntityCircle* entity : graph.entities)
//    {
//        if (entity)
//        {
//            sf::Vector2f pos{ entity->get_shape().getPosition() };
//            if (pos.x < 0)
//            {
//                std::cout << "Problem, pos x is: " << pos.x << '\n';
//                return true;
//            }
//            else if (pos.y < 0)
//            {
//                std::cout << "Problem, pos y is: " << pos.y << '\n';
//                return true;
//            }
//        }
//    }
//    return false;
//}