//
// Created by tobi on 4/11/17.
//

#include "systems.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include "PosComponent.h"
#include "SkillComponent.h"
#include "ColGrid.h"
#include "ColShape.h"
#include "CPruneSweep.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include <WRenderer.h>
#include <WVecMath.h>
#include <algorithm>

#include <iostream>
void debug_draw_update(GameWorld &world, const std::vector<unsigned int> &entities) {
    WTransform zero_tf;
    WRenderer::set_mode(PLines);
    for (const auto &tri : world.grid().get_objects()) {
        tri->add_gfx_lines(zero_tf);
    }

    for (const auto entity : entities) {
        auto &shape = world.cshape_c(entity).shape;
        const auto &transform = world.pos_c(entity).global_transform;
        shape->add_gfx_lines(transform);
    }
}

void static_col_update(GameWorld &world, const std::vector<unsigned int> &entities) {

    for (const auto entity : entities) {
        // position
        auto &transform = world.pos_c(entity).global_transform;
        auto &pos = world.pos_c(entity).position;
        auto &rot = world.pos_c(entity).rotation;
        auto parent = world.pos_c(entity).parent;

        // collision
        auto &result = world.static_col_c(entity).col_result;
        auto &shape = world.cshape_c(entity).shape;

        //circle to world
        shape->transform(transform);

        // overwrite result
        result = ColResult();

        auto colliders = world.grid().find_colliders(shape);
        for (const auto &tri : colliders) {
            auto cr = shape->collides(*tri);
            if (cr.collides) {
                if (cr.depth > result.depth) {
                    result = cr;
                }
            }
        }
        // in this example it's okay to reset the transformation here.
        // this deals only with the environmental collision.
        // (what to do about moving platforms)
        shape->transform(glm::inverse(transform));

        if (result.collides && result.epa_it < 21) {
            auto move_back = result.normal * -result.depth;
            pos += move_back;

            // player can stand on slopes
            if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
                WVec correction = w_slide(WVec(move_back.x, 0), result.normal);
                if (correction.x != 0) {
                    correction *= move_back.x / correction.x;
                }
                pos -= correction;
            }

            // slide movement and collide again
            //circle to world
            transform = glm::rotate(glm::translate(WTransform(), pos), rot) * world.pos_c(parent).global_transform;
            shape->transform(transform);

            ColResult second_result;

            for (const auto &tri : colliders) {
                auto cr = shape->collides(*tri);
                if (cr.collides) {
                    if (cr.depth> second_result.depth) {
                        second_result = cr;
                    }
                }
            }
            shape->transform(glm::inverse(transform));

            if (second_result.collides) {
                WVec correction = find_directed_overlap(second_result, WVec(-move_back.y, move_back.x));
                pos += correction;

                transform = glm::rotate(glm::translate(WTransform(), pos), rot) * world.pos_c(parent).global_transform;
            }

            // call back
            get_static_col_response(world.static_col_c(entity).col_response)(world, entity);
        }
    }
}

void input_update(GameWorld &world, const std::vector<unsigned int> &entities) {
    for (const auto entity : entities) {
        auto &ic = world.input_c(entity);
        get_input_func(ic.input_func)(world, entity);
    }
}

void move_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities) {
    for (const auto entity : entities) {

        auto &mc = world.move_c(entity);
        auto &pc = world.pos_c(entity);

        auto old_accel = mc.accel;

        mc.accel = WVec(0, c_gravity);
        mc.accel += mc.additional_force / mc.mass;

        mc.velocity += old_accel * dt;

        // check transistions
        for (MoveState m: get_trans_funcs(mc.moveset, mc.movestate)) {
            if (get_trans_func(m)(world, entity)) {
                get_to_func(m)(world, entity);
                break;
            }
        }
        get_accel_func(mc.movestate)(world, entity);

        mc.velocity += dt * (mc.accel - old_accel) / 2.0f;
        auto motion = dt * (mc.velocity + mc.accel * dt / 2.0f);
        pc.position += motion;

        pc.global_transform = glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation) * world.pos_c(pc.parent).global_transform;

        mc.additional_force = {0, 0};
    }
}


void ground_move_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities) {
    for (const auto entity : entities) {

        auto &gc = world.ground_move_c(entity);
        gc.air_time += dt;
    }
}

void fly_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities) {
    for (const auto entity : entities) {

        auto &fc = world.fly_c(entity);
        fc.timer += dt;
        if (fc.timer > fc.c_accel_time) {
            fc.timer = fc.c_accel_time;
        }
    }
}

void skill_update(GameWorld &world, float dt, const std::vector<unsigned int> &entities) {
    for (const auto entity : entities) {
        auto &sc = world.skill_c(entity);
        auto &ic = world.input_c(entity);

        // handle skill input
        if (ic.att_melee[0]) {
            cast_skill(world, entity, SkillID::Melee);
        }

        for (auto &s : sc.skills) {
            s.timer -= dt;

            if (s.id == sc.active) {
                auto fn = get_skill_func(s.skillstate, s.id);
                if (fn) {
                fn(world, entity);
                }
            }

            switch (s.skillstate) {
                case SkillState::BuildUp : {
   	                if (s.timer <= 0) {
                        s.skillstate = SkillState::Channel;
                        s.timer = s.c_time_channel;
   	                }
                    break;
                }
                case SkillState::Channel : {
         	        if (s.timer <= 0) {
 	                    s.skillstate = SkillState::Recover;
 	                    s.timer = s.c_time_recover;
 	       	        } 
               	    break;
           	    }
                case SkillState::Recover : {
 	          	    if (s.timer <= 0) {
                        s.skillstate = SkillState::Cooldown;
                        s.timer = s.c_time_cooldown;
               	    }
               	    break;
           	    }
                case SkillState::Cooldown : {
                    if (s.timer <= 0) {
                        s.skillstate = SkillState::Ready;
                    }
                    break;
                }
                case SkillState::Ready : {
             	    if (sc.active == s.id) {
                        sc.active = SkillID::None;
                    }
                }
            }
        }
    }
}

void dyn_col_update(GameWorld &world, std::unordered_map<unsigned int, bool> &entities) {
    auto &prune_sweep = world.prune_sweep();
    auto &boxes = prune_sweep.get_boxes();
    std::vector<unsigned int> to_delete;
    
    for (auto &box : boxes) {
        // has dyn_col_comp
        if (entities.count(box.entity) > 0) {
            auto &shape = world.cshape_c(box.entity).shape;
            entities.at(box.entity) = true;
            box.mins = world.pos_c(box.entity).position - shape->get_radius();
            box.maxs = world.pos_c(box.entity).position + shape->get_radius();
        } else {
            // todo move to delete list
            to_delete.push_back(box.entity);
        }
    }

    for (const auto &pair : entities) {
        auto checked = pair.second;
        auto entity = pair.first;
        if (!checked) {        
            auto &shape = world.cshape_c(entity).shape;
            const auto &pos = world.pos_c(entity).position;
            boxes.emplace_back(PSBox{pos - shape->get_radius(), pos + shape->get_radius(), entity});
        }
    }
    // todo remove boxes from delete list
    for (auto ent : to_delete) {
        boxes.erase(std::remove_if(boxes.begin(), boxes.end(), [ent] (const PSBox& b) 
                    {return b.entity == ent;}));
    }
    for (auto &b : boxes) {
        assert(entities.count(b.entity) > 0);
    }
    for (auto &pair : entities) {
        assert(std::find_if(boxes.begin(), boxes.end(), [pair] (const PSBox& b) {
                    return b.entity == pair.first;
                    }) != boxes.end());
    }
    // prune and sweep
    prune_sweep.prune_and_sweep();

    for (auto &pair : prune_sweep.get_pairs()) {
        unsigned int a = pair.first;
        unsigned int b = pair.second;

        auto &shape_a = world.cshape_c(a).shape;
        auto &shape_b = world.cshape_c(b).shape;
        
        shape_a->transform(world.pos_c(a).global_transform);
        shape_b->transform(world.pos_c(b).global_transform);
        auto result = shape_a->collides(*shape_b);
        shape_a->transform(glm::inverse(world.pos_c(a).global_transform));
        shape_b->transform(glm::inverse(world.pos_c(b).global_transform));
        

        if (result.collides) {
            auto &dc = world.dyn_col_c(a);
            dc.col_result = result;
            dc.collided = b;
            auto fn = get_dynamic_col_response(dc.col_response);
            if (fn) {
                fn(world, a);
            }
            auto &dcb = world.dyn_col_c(b);
            dcb.col_result = result;
            dcb.col_result.normal *= -1.f;
            dcb.collided = a;
            fn = get_dynamic_col_response(dcb.col_response);
            if (fn) {
                fn(world, b);
            }
        }
    }
}
