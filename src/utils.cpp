#include "utils.h"

id_t generate_id()
{
    static id_t s_entity_id{ 0 };
    return s_entity_id++;
}
