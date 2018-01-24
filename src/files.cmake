include(${CMAKE_CURRENT_LIST_DIR}/schemas/schemas.cmake)

set(SOURCE_FILES
    src/Game.cpp
    src/main.cpp
    src/Actor.cpp
    src/BCurve.cpp
    src/Camera.cpp
    src/Editor.cpp
    src/Engine.cpp
    src/Island.cpp
    src/WInput.cpp
    src/NavMesh.cpp
    src/SceneIO.cpp
    src/systems.cpp
    src/WShader.cpp
    src/ColShape.cpp
    src/steering.cpp
    src/WTexture.cpp
    src/Collision.cpp
    src/GameWorld.cpp
    src/WRenderer.cpp
    src/Components.cpp
    src/EditorDraw.cpp
    src/FrameTimer.cpp
    src/CPruneSweep.cpp
    src/MoveSystems.cpp
    src/Pathfinding.cpp
    src/Protagonist.cpp
    src/SimpleFlyer.cpp
    src/triangulate.cpp
    src/EditorStates.cpp
    src/EntityEditor.cpp
    src/PosComponent.cpp
    src/TagComponent.cpp
    src/SkillComponent.cpp
    src/HurtBoxComponent.cpp
    src/CollisionComponent.cpp
    src/imgui_impl_glfw_gl3.cpp
    src/StatusEffectComponent.cpp
    src/PatrolComponent.cpp
    src/AIComponent.cpp
    src/SkillMelee.cpp
    src/SkillLounge.cpp
    src/NTimes.cpp
    src/EnemyInRange.cpp
    src/TestEnemyAI.cpp
    src/GoToEnemy.cpp
    src/StatusEffectKnockback.cpp
    src/StatusEffectHitstun.cpp
    src/SkillDisk.cpp
    src/Entity.cpp
    )
