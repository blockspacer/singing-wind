//
// Created by jammer on 08/04/17.
//

#include "Game.h"

Game::Game() {
    update_camera_follow();
}

void Game::update(Engine &engine) {
    if (!engine.get_focus()) {
        return;
    }
    const sf::RenderWindow &window = engine.get_window();
    auto mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    m_timer.update();
    while (m_timer.pop_fixed()) {
        m_game_world.step_fixed(c_fixed_timestep, {mouse.x, mouse.y});
    }
    m_camera.update(m_game_world, engine);
}

void Game::draw(sf::RenderWindow &window) {
    m_game_world.draw(window);
}

void Game::unpause() {
    EngineState::unpause();
    m_timer.reset();
    update_camera_follow();

}

void Game::update_camera_follow() {
    for (unsigned int i = 0 ; i < m_game_world.m_id_c.size() ; ++i) {
        if (m_game_world.m_id_c[i] == "player" and m_game_world.m_entities[i].any()) {
            m_camera.m_entity_to_follow = i;
        }
    }
}
