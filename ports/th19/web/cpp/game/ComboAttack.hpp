#pragma once
namespace th19 {
// ExtraAttackInf 0x50df40, death rewards 0x51d4e0 (EXE 1.10c).
struct ComboAttack {
    int extraCredit = 0;
    int bossCredit = 0;
    int extraRecovery = 0;
    int extraCooldown = 0;
    int bossCooldown = 0;
    unsigned frame = 0;

    void on_death(int kind, int previousCombo) {
        if (previousCombo <= 4 || kind < 1 || kind > 6) return;
        if (extraCooldown == 0) extraCredit += 15;
        if (bossCooldown == 0) bossCredit += 15;
    }

    // Bits 0/1 request EX/Boss. Boss occupancy is evaluated at dispatch.
    unsigned tick() {
        unsigned attacks = 0;
        if (extraCredit > 0) {
            if (extraCredit >= 100) {
                attacks |= 1;
                extraCredit = -extraRecovery;
                extraRecovery += 100;
                extraCooldown = 60;
            }
            if (frame % 4 == 0) --extraCredit;
        } else if (extraCredit < 0) {
            ++extraCredit;
        }
        if (extraRecovery > 0) --extraRecovery;
        if (extraCooldown > 0) --extraCooldown;
        if (bossCredit > 0) {
            if (bossCredit >= 200) {
                attacks |= 2;
                bossCredit = 0;
                bossCooldown = 240;
            }
            if (frame % 4 == 0 && bossCredit > 0) --bossCredit;
        }
        if (bossCooldown > 0) --bossCooldown;
        ++frame;
        return attacks;
    }
};
}
