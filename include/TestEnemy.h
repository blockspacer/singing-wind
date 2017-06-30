#ifndef TESTENEMY_H
#define TESTENEMY_H

class GameWorld;
struct ColResult;

namespace TestEnemy {
    void on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity);
    
    void handle_inputs(GameWorld &world, unsigned int entity);

    void simple_flying(GameWorld &world, unsigned int entity);
}

#endif /* TESTENEMY_H */