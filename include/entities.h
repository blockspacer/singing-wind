//
// Created by jammer on 11/04/17.
//

#ifndef SINGING_WIND_ENTITIES_H
#define SINGING_WIND_ENTITIES_H

#include <unordered_map>
#include <functional>
#include "Protagonist.h"
#include "TestEnemy.h"
#include "ColShape.h"
#include "Components.h"

class GameWorld;

namespace Entities {
    unsigned int create_root(GameWorld &game_world, const WVec &position, unsigned int parent);
}

const std::unordered_map<StaticColResponse, std::function<void(const ColResult &, GameWorld &, unsigned int)>> static_col_responses = {
        {StaticColResponse::Actor, protagonist::on_static_collision}
};

const std::unordered_map<InputFunc, std::function<void(GameWorld &world, unsigned int entity)>> input_funcs = {
        {InputFunc::Protagonist, protagonist::handle_inputs},
        {InputFunc::TestEnemy, TestEnemy::handle_inputs}
};

#endif //SINGING_WIND_ENTITIES_H
