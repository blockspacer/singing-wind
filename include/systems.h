//
// Created by tobi on 4/11/17.
//

#ifndef SINGING_WIND_SYSTEMS_H
#define SINGING_WIND_SYSTEMS_H

#include "WindDefs.h"
#include "Components.h"

class GameWorld;

const bset c_col_test_components({ (1 << CPosition) | (1 << CDebugDraw) });
void col_test_update(GameWorld &world, const WVec &mouse);

const bset c_debug_draw_components{ (1 << CPosition) | (1 << CDebugDraw) };
void debug_draw_update(GameWorld &world, sf::RenderWindow &window);

const bset c_static_col_components{(1 << CPosition) | (1 << CStaticCol) | (1 << CMove)};
void static_col_update(GameWorld &world);


#endif //SINGING_WIND_SYSTEMS_H
