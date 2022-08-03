#include "utils.h"

int generate_id() 
{
    static int s_entity_id{ 0 };
    return s_entity_id++;
}
