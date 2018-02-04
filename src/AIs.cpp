#include "TestEnemyAI.h"
#include "Behaviours.h"
#include "GameWorld.h"
#include "AIComponent.h"

behaviour_tree::BehaviourTree
testenemy::get_tree(GameWorld& world, unsigned int entity)
{
  using namespace behaviour_tree;
  auto tree = WBehaviourTreeBuilder<BehaviourTree>(AITreeType::TestEnemy)
                .composite<Sequence>()
                .node<EnemyInRange>(world, entity, 200)
                //.end()
                // TODO: maybe do nothing?
                .end()
                .decorator<RunForFrames>(300)
                .node<GoToEnemy>(world, entity)
                .end()
                .end()
                .end()
                .node<Wander>(world, entity, 1.3, .4)
                .end()

                .end();
  return tree;
};