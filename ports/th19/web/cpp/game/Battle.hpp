// th19 versus battle state machine (skeleton).
//
// Reverse-engineered facts (all from th19.exe decompile, see memory/2026-10-08.md):
//   - Battle = 1P vs 2P (local / CPU / Photon network). FUN_00548df0 news a
//     0x14c48-byte PlayerInf and, only when opponent is CPU (param_1==1), a CPUInf.
//   - Spell cards: PlayerInf+0xbc == 3 => Extra Attack (player's own card),
//     otherwise Boss Attack (opponent's card). +0xc4 = Extra level, +0xc8 = Boss level.
//     FUN_0050d980 prints "Extra Attack !!" / "Boss Attack Level %d";
//     FUN_0050d800/0050d740 fire the "BossAttack" ECL sub, 1-param_1[0x33] picks 1P/2P side.
//   - aura.anm is loaded for the spell-card aura (FUN_00548df0).
//   - Network: Key1P (+0x98) / Key2P (+0x358) synced via Photon (FUN_0055f6c0).
//   - Score: ScoreInf saves a replay; "Score %llu / Chara / Rank / Stage / Slow Rate".
//   - Animal-spirit kind -> ECL sub (FUN_0051b480):
//       1 = GhostPure, 2 = GhostWolf, 3 = GhostOtterL/R (by direction), 4 = GhostEagle.
//   - Spirit appearance id (ins_1028 f: 10/20/30/32/40/50/70/80) is an enemy.anm sprite id.
//
// This skeleton encodes the state + transitions. Rendering/AI fill in next.
#pragma once
#include "Types.hpp"
#include <string>
#include <vector>

namespace th19 {
class GameWorld;   // forward: the ECL object world (enemies/bullets/spirits lists)

// --- animal spirit kinds (verified: FUN_0051b480 dispatch) ---
enum class SpiritKind : i32 { None=0, Pure=1, Wolf=2, Otter=3, Eagle=4 };

// kind -> ECL sub name (spawned via EnemyInf spawner FUN_0050d410; verified FUN_0051b480).
// Otter L/R is chosen by spawn x direction: FUN_0051b480 case 3 sets "GhostOtterR" by
// default and switches to "GhostOtterL" when x >= 0.
inline const char* spirit_kind_sub(SpiritKind k, float x=0.0f){
    switch(k){
        case SpiritKind::Pure:  return "GhostPure";
        case SpiritKind::Wolf:  return "GhostWolf";
        case SpiritKind::Otter: return (x >= 0.0f) ? "GhostOtterL" : "GhostOtterR";
        case SpiritKind::Eagle: return "GhostEagle";
        default:                return "GhostPure";
    }
}

// Second-stage pure-spirit burst subs (verified FUN_0042e850): fired when a released
// pure spirit grazes/collects. Enemy flag bit 28 (+0x63a0 >> 28 & 1) clear -> "GhostPure2G";
// else alternate "GhostPure2" / "GhostPure2b" by a per-player counter (PlayerInf+300) % 3.
inline const char* spirit_pure_burst_sub(bool graze, i32 seq){
    if (graze) return "GhostPure2G";
    return (seq % 3 != 0) ? "GhostPure2" : "GhostPure2b";
}

// --- spell-card mode (verified: PlayerInf+0xbc) ---
enum class SpellMode : i32 { None=0, Extra=3 };  // 3 == Extra Attack; anything else = Boss Attack

// --- per-player state (mirrors the verified PlayerInf offsets we care about) ---
struct PlayerState {
    bool  present=false;
    float x=0, y=0;          // playfield position (center-origin, y-down)
    i32   life=0;            // player life / HP
    i32   chara=0;           // character id (pl00..pl18)
    // spell-card fields (verified offsets)
    i32   spell_mode=0;      // +0xbc: 3=Extra Attack (own card), else Boss Attack
    i32   extra_level=0;     // +0xc4
    i32   boss_level=0;      // +0xc8
    i32   side=0;            // +0x33: 0=1P, 1=2P (picks which side fires BossAttack)
    // input keys (Photon Key1P/Key2P, +0x98/+0x358)
    u32   keys=0;
};

// --- CPU opponent state (CPUInf, 0x48 bytes, inherits TaskInf; present only vs CPU) ---
struct CpuState {
    bool  present=false;
    i32   difficulty=0;      // 0..3 -> aggression ramp interval {240,600,1200,1000000} frames
    // reverse-engineered CPUInf fields (FUN_004fa2a0 main AI tick):
    i32   frame=0;           // +0x3c frame counter (incr while mode==2 and not paused)
    i32   ai_level=0;        // +0x38 aggression level (starts ramping after 0x707 frames, cap 100)
    float target_x=0,target_y=0; // +0x24/+0x2c opponent position (tracked target)
    i32   side=0;            // +0x40 which side this CPU controls (0=1P, 1=2P)
    // movement decision output (FUN_004f9ac0 scan result):
    float move_x=0, move_y=0;   // desired move direction (toward collect / away from threat)
    bool  want_fire=false;      // aggression-gated shoot decision
};

// --- a spell card in flight ---
struct SpellCard {
    bool  active=false;
    bool  is_extra=false;    // true = player's Extra Attack; false = opponent's Boss Attack
    i32   level=0;
    i32   timer=0;           // frames remaining
    i32   target_side=0;     // verified: FUN_0050d800 sets side = 1 - self.side (opposite of firing player)
};

// --- score / replay summary (ScoreInf) ---
struct ScoreState {
    u64   score=0;
    i32   rank=0;
    i32   stage=0;
    float slow_rate=0;
    i32   chara=0;
};

// --- collected animal-spirit stock (drives ExtraAttack release via ins_1034/1035) ---
struct SpiritStock {
    i32 slot[2]={0,0};       // +0x38 / +0x3c collected kinds (clamped 0..8)
    i32 active_kind=0;       // +0x145a, loaded by ins_1034/1035
    i32 count=0;             // number of collected spirits
};

// --- the versus battle world ---
class BattleWorld {
public:
    // opponent type: 0 = local 2P, 1 = CPU, 2 = network (Photon)
    void init(i32 opponent, i32 chara_1p, i32 chara_2p);
    void bind_world(GameWorld* w) noexcept {world_=w;}   // link the ECL object lists
    void tick();                          // one frame: AI + spell-card timers + score

    PlayerState p1, p2;
    CpuState     cpu;
    SpellCard    boss_card;               // opponent's Boss Attack
    SpellCard    extra_card;              // player's Extra Attack
    ScoreState   score;
    SpiritStock  p1_stock, p2_stock;

    // spell-card activation (drives the "BossAttack"/ExtraAttack ECL flow)
    void fire_boss_attack(i32 side);      // opponent card
    void fire_extra_attack(i32 side);     // player card
    void end_spell_card();

    // spirit release (ExtraAttack ins_1037) — uses active_kind from the stock
    i32  release_spirit_kind(i32 side) const;

    i32  opponent() const {return opponent_;}
private:
    void tick_cpu_ai();
    i32  opponent_;
    GameWorld* world_=nullptr;   // linked ECL world (enemies/bullets/spirits for the AI scan)
};
}
