// Strogatz.cpp : This file contains the 'main' function.

#include <iostream>

#include "Entity.h"

int main()
{
    std::cout << "Welcome to Strogatz!\n";
    Entity entity;
    std::cout << entity.get_id() << '\n';
    Entity entity1;
    std::cout << entity1.get_id() << '\n';
    entity.propagate_beliefs();
}
