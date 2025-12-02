#ifndef PHYSICS_H
#define PHYSICS_H

#include <genesis.h>
#include "entity.h"

void handle_all_entities();
void handle_physics(Entity* e);
void handle_input(Entity* e);
void prepareEntitys(Entity* e);

#endif