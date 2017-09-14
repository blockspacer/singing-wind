#include "Melee.h"
#include "GameWorld.h"
#include "Components.h"
#include "PosComponent.h"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include "ColShape.h"
#include "TagComponent.h"
#include "SkillComponent.h"
#include "LifeTimeComponent.h"
#include "HurtBoxComponent.h"
#include "MoveSystems.h"
#include "WVecMath.h"
#include "Components.h"
#include "StatusEffectComponent.h"

void melee_skill_hurtfunc(GameWorld &world, unsigned int victim, unsigned int attacker) {
    // knockback
    auto dir = w_normalize(world.pos_c(victim).position - world.pos_c(attacker).position);
    world.move_c(victim).velocity = dir * 400.f;
    auto kb = statuseffects::knockback();
    kb.timer = .6f;
    statuseffects::add_effect(world, victim, kb);
    auto hs = statuseffects::hitstun();
    hs.timer = .1f;
    statuseffects::add_effect(world, victim, hs);
    // TODO: damage
}

void melee_skill_on_hit(GameWorld &world, unsigned int attacker, unsigned int victim) {
    auto dir = w_normalize(world.pos_c(attacker).position - world.pos_c(victim).position);
    world.move_c(attacker).velocity = dir * 200.f;
    auto kb = statuseffects::knockback();
    kb.timer = .1f;
    statuseffects::add_effect(world, attacker, kb);
    auto hs = statuseffects::hitstun();
    hs.timer = .1f;
    statuseffects::add_effect(world, attacker, hs);

    world.fly_c(attacker).timer = -0.5f;
}

void melee_skill::move_buildup(GameWorld &, unsigned int ) {

}

void melee_skill::move_channel(GameWorld &world, unsigned int entity) {
    const auto &pc = world.pos_c(entity);
    auto &mc = world.move_c(entity);

    mc.accel = 1500.f * w_rotated_deg(WVec(0, -1), pc.rotation);
}

void melee_skill::Skill::channel_start(GameWorld &world, unsigned int entity) {
    // set movestate for caster
    auto &mc = world.move_c(entity);
    mc.special = SpecialMoveState::MeleeChannel;

    // create hurtbox
    auto hurtbox = world.create_entity();
    bset comps;
    for (auto i : {CPosition, CColShape, CDynCol, CDebugDraw, CTag, CLifeTime}) {
        comps.set(i);
    }
    
    world.entities()[hurtbox] = comps;
    world.name_c(hurtbox) = "melee_skill_hurtbox";

    // pos
    auto &pc = world.pos_c(hurtbox);
    pc.parent = entity;
    pc.position = WVec(30, -35);
    pc.rotation = 0;
    build_global_transform(world, hurtbox);
    // col shape
    auto &csc = world.cshape_c(hurtbox);
    csc.shape = std::shared_ptr<ColShape>(new ColCapsule(35, 15));

    // tags
    auto &tc = world.tag_c(hurtbox);
    tc.set(static_cast<int>(Tags::Hurtbox));

    // dyn col
    auto &dc = world.dyn_col_c(hurtbox);
    dc.col_response = DynColResponse::HurtBox;

    // lifetime
    auto &lc = world.lifetime_c(hurtbox);
    lc.timer = .3;

    // hurtbox
    auto &hb = world.hurtbox_c(hurtbox);
    hb.owner = entity;
    hb.hurt_function = melee_skill_hurtfunc;
    hb.on_hit = melee_skill_on_hit;
}

void melee_skill::Skill::channel_end(GameWorld & world, unsigned int entity) {
    reset_special(world, entity, SpecialMoveState::MeleeChannel);
}

melee_skill::Skill::Skill() : SkillBase(0.1, 0.2, 1, 2, SkillID::Melee) {}
