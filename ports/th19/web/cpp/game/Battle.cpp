#include "Battle.hpp"
#include "GameWorld.hpp"
#include <cmath>

namespace th19 {

void BattleWorld::init(i32 opponent, i32 chara_1p, i32 chara_2p){
    opponent_ = opponent;
    p1 = PlayerState{}; p2 = PlayerState{};
    cpu = CpuState{};
    boss_card = SpellCard{}; extra_card = SpellCard{};
    score = ScoreState{}; p1_stock = SpiritStock{}; p2_stock = SpiritStock{};

    // 1P is always present; 2P present unless it's a CPU/network opponent slot
    p1.present = true; p1.chara = chara_1p; p1.side = 0;
    p2.chara = chara_2p; p2.side = 1;
    if (opponent == 0)      p2.present = true;   // local 2P
    else if (opponent == 1) cpu.present = true;  // CPU opponent
    else                    p2.present = true;   // network: remote 2P
}

void BattleWorld::fire_boss_attack(i32 side){
    // FUN_0050d800: fires the opponent's "BossAttack" ECL sub. Verified facts:
    //   - sets the global side to 1 - self.side (target = opposite of firing player)
    //   - spawns "BossAttack" via EnemyInf spawner FUN_0050d410 at the player position
    //   - calls the player vtable method +0x44 with arg 4 (sound/effect trigger)
    boss_card.active = true;
    boss_card.is_extra = false;
    boss_card.level = (side == 0) ? p2.boss_level : p1.boss_level;
    boss_card.timer = 0;
    boss_card.target_side = 1 - side;
}

void BattleWorld::fire_extra_attack(i32 side){
    // FUN_0050d740 (thiscall variant): fires the player's own Extra Attack. Same
    // side/target logic as BossAttack; additionally stores an extra param into the
    // per-side slot (verified in th19_boss.c).
    extra_card.active = true;
    extra_card.is_extra = true;
    extra_card.level = (side == 0) ? p1.extra_level : p2.extra_level;
    extra_card.timer = 0;
    extra_card.target_side = 1 - side;
}

void BattleWorld::end_spell_card(){
    boss_card.active = false;
    extra_card.active = false;
}

i32 BattleWorld::release_spirit_kind(i32 side) const{
    const SpiritStock& s = (side == 0) ? p1_stock : p2_stock;
    return s.active_kind;   // +0x145a, loaded by ins_1034/1035
}

void BattleWorld::tick_cpu_ai(){
    if (!cpu.present) return;
    // FUN_004fa2a0 main AI tick:
    //   - frame counter +0x3c increments while mode==2 and not paused
    //   - aggression level +0x38 ramps up after 0x707 frames, capped at 100,
    //     stepping every {240,600,1200,1000000} frames (difficulty table 0x5aa630)
    //   - tracks opponent position (+0x68c/+0x690) as the aim/dodge target
    static const i32 ramp_interval[4] = {240, 600, 1200, 1000000};
    ++cpu.frame;
    i32 d = cpu.difficulty; if (d < 0) d = 0; else if (d > 3) d = 3;
    if (cpu.frame > 0x707 && cpu.ai_level < 100 && (cpu.frame % ramp_interval[d]) == 0) {
        ++cpu.ai_level;
    }
    // track the human player (the side this CPU is NOT controlling)
    const PlayerState& self  = (cpu.side == 0) ? p1 : p2;
    const PlayerState& human = (cpu.side == 0) ? p2 : p1;
    cpu.target_x = human.x;
    cpu.target_y = human.y;

    // FUN_004f9ac0 core decision: scan the object lists and decide movement.
    // For each enemy/bullet compute relative position + distance; move toward the
    // nearest collectible spirit, away from the nearest threat.
    cpu.move_x = 0; cpu.move_y = 0;
    cpu.want_fire = false;
    if (!world_) return;

    // nearest spirit (collectible) -> steer toward it
    float best_collect = 1e18f;
    float collect_dx = 0, collect_dy = 0;
    for (const auto& s : world_->spirits){
        if (!s.active) continue;
        float dx = s.x - self.x, dy = s.y - self.y;
        float dist = dx*dx + dy*dy;
        if (dist < best_collect){ best_collect = dist; collect_dx = dx; collect_dy = dy; }
    }
    // nearest enemy (threat/blocker) -> steer away
    float best_threat = 1e18f;
    float threat_dx = 0, threat_dy = 0;
    for (const auto& e : world_->enemies){
        float dx = e.x - self.x, dy = e.y - self.y;
        float dist = dx*dx + dy*dy;
        if (dist < best_threat){ best_threat = dist; threat_dx = dx; threat_dy = dy; }
    }
    // nearest bullet (immediate threat) -> steer away harder
    for (const auto& b : world_->bullets){
        float dx = b.x - self.x, dy = b.y - self.y;
        float dist = dx*dx + dy*dy;
        if (dist < best_threat){ best_threat = dist; threat_dx = dx; threat_dy = dy; }
    }
    // combine: collect pull + threat push (simple weighted steer)
    const float collect_w = 1.0f;
    const float threat_w  = 3.0f;
    if (best_collect < 1e18f){
        float len = std::sqrt(collect_dx*collect_dx + collect_dy*collect_dy);
        if (len > 0.0001f){ cpu.move_x += collect_w * collect_dx/len; cpu.move_y += collect_w * collect_dy/len; }
    }
    if (best_threat < 1e18f){
        float len = std::sqrt(threat_dx*threat_dx + threat_dy*threat_dy);
        if (len > 0.0001f){ cpu.move_x -= threat_w * threat_dx/len; cpu.move_y -= threat_w * threat_dy/len; }
    }
    // shoot when aggression is high enough (level-gated)
    cpu.want_fire = (cpu.ai_level >= 50);
}

void BattleWorld::tick(){
    tick_cpu_ai();
    if (boss_card.active){
        if (++boss_card.timer >= 0) { /* card expiry handled by ECL BossAttack sub */ }
    }
    if (extra_card.active){
        if (++extra_card.timer >= 0) { /* card expiry handled by ECL ExtraAttack sub */ }
    }
}
}
