// Strogatz.cpp : This file contains the 'main' function.

#include <iostream>
#include "Entity.h"

int main()
{
    std::cout << "Welcome to Strogatz!\n";
    Entity entity;
    entity.propagate_beliefs();
}
