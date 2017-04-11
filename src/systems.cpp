//
// Created by tobi on 4/11/17.
//

#include "systems.h"
#include "GameWorld.h"

void col_test_update(GameWorld &world, const WVec &mouse) {
    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], c_col_test_components)) {
            continue;
        }
        auto &transform = world.m_pos_c[entity].global_transform;
        auto &pos = world.m_pos_c[entity].position;
        auto &rot = world.m_pos_c[entity].rotation;
        auto &shape = world.m_debug_c[entity].shape;
        auto parent = world.m_pos_c[entity].parent;

        WVec vel = (mouse - transform.transformPoint(0, 0)) * 0.1f;
        pos += vel;

        for (auto &tri : world.m_grid.get_objects()) {
            tri->m_highlight = false;
        }

        //circle to world
        transform = WTransform::Identity;
        transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
        shape->transform(transform);

        ColResult result;

        auto colliders = world.m_grid.find_colliders(shape);
        for (const auto &tri : colliders) {
            auto cr = shape->collides(*tri);
            if (cr.collides) {
                tri->m_highlight = true;
                if (cr.e > result.e) {
                    result = cr;
                }
            }
        }
        // in this example it's okay to reset the transformation here.
        // this deals only with the environmental collision.
        // (what to do about moving platforms)
        shape->transform(transform.getInverse());

        if (result.collides) {
            auto move_back = find_directed_overlap(result, vel);
            pos += move_back;

            // slide movement and collide again
            vel = slide(-move_back, result.normal);
            pos += vel;
            //circle to world
            transform = WTransform::Identity;
            transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
            shape->transform(transform);

            result = ColResult();

            for (const auto &tri : colliders) {
                auto cr = shape->collides(*tri);
                if (cr.collides) {
                    tri->m_highlight = true;
                    if (cr.e> result.e) {
                        result = cr;
                    }
                }
            }
            shape->transform(transform.getInverse());

            if (result.collides) {
                move_back = find_directed_overlap(result, vel);
                pos += move_back;

                transform = WTransform::Identity;
                transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
            }
        }
    }
}

void debug_draw_update(GameWorld &world, sf::RenderWindow &window) {
    sf::VertexArray lines_va(sf::Lines);
    WTransform zero_tf;
    for (const auto &tri : world.m_grid.get_objects()) {
        tri->add_gfx_lines(lines_va, zero_tf);
    }

    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], c_debug_draw_components)) {
            continue;
        }
        auto &shape = world.m_debug_c[entity].shape;
        auto &transform = world.m_pos_c[entity].global_transform;
        shape->add_gfx_lines(lines_va, transform);
    }

    window.draw(lines_va);
}

void static_col_update(GameWorld &world) {

    for (unsigned int entity = 0; entity < world.m_entities.size(); ++entity) {
        if (!has_component(world.m_entities[entity], c_col_test_components)) {
            continue;
        }
        // position
        auto &transform = world.m_pos_c[entity].global_transform;
        auto &pos = world.m_pos_c[entity].position;
        auto &rot = world.m_pos_c[entity].rotation;
        auto parent = world.m_pos_c[entity].parent;

        // collision
        auto &result = world.m_static_col_c[entity].col_result;
        auto &shape = world.m_static_col_c[entity].shape;

        // movement
        auto &vel = world.m_move_c[entity].velocity;

        //circle to world
        transform = WTransform::Identity;
        transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
        shape->transform(transform);

        // overwrite result
        result = ColResult();

        auto colliders = world.m_grid.find_colliders(shape);
        for (const auto &tri : colliders) {
            auto cr = shape->collides(*tri);
            if (cr.collides) {
                if (cr.e > result.e) {
                    result = cr;
                }
            }
        }
        // in this example it's okay to reset the transformation here.
        // this deals only with the environmental collision.
        // (what to do about moving platforms)
        shape->transform(transform.getInverse());

        if (result.collides) {
            auto move_back = find_directed_overlap(result, vel);
            pos += move_back;

            // slide movement and collide again
            vel = slide(-move_back, result.normal);
            pos += vel;
            //circle to world
            transform = WTransform::Identity;
            transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
            shape->transform(transform);

            ColResult second_result;

            for (const auto &tri : colliders) {
                auto cr = shape->collides(*tri);
                if (cr.collides) {
                    tri->m_highlight = true;
                    if (cr.e> second_result.e) {
                        second_result = cr;
                    }
                }
            }
            shape->transform(transform.getInverse());

            if (second_result.collides) {
                move_back = find_directed_overlap(second_result, vel);
                pos += move_back;

                transform = WTransform::Identity;
                transform.combine(world.m_pos_c[parent].global_transform).translate(pos).rotate(rot);
            }

            // call back
            world.m_static_col_c[entity].on_static_col_cb(result, world, entity);
        }
    }
}
