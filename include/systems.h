//
// Created by tobi on 4/11/17.
//

#ifndef SINGING_WIND_SYSTEMS_H
#define SINGING_WIND_SYSTEMS_H

#include "Components.h"
#include <vector>
#include <unordered_map>

class GameWorld;

const bset c_debug_draw_components{ (1 << CPosition) | (1 << CDebugDraw) | (1 << CColShape)};
void debug_draw_update(GameWorld &world, const std::vector<unsigned int> &entities);

const bset c_static_col_components{(1 << CPosition) | (1 << CStaticCol) | (1 << CColShape)};
void static_col_update(GameWorld &world, const std::vector<unsigned int> &entities);

const bset c_input_components{(1 << CInput)};
void input_update(GameWorld &world, const std::vector<unsigned int> &entities);

const bset c_move_components{(1 << CPosition) | (1 << CMove)};
void move_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities);

const bset c_ground_move_components{(1 << CMove)};
void ground_move_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities);

const bset c_fly_components{(1 << CFly)};
void fly_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities);

const bset c_path_components{(1 << CPosition) | (1 << CPathing)};
void path_update(GameWorld &world, const std::vector<unsigned int> &entities);

const bset c_skill_components{(1 << CSkill) | (1 << CInput)};
void skill_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities);

const bset c_dyn_col_components{(1 << CPosition) | (1 << CDynCol) | (1 << CColShape)};
void dyn_col_update(GameWorld &world, std::unordered_map<unsigned int, bool> &entities);

#endif //SINGING_WIND_SYSTEMS_H
