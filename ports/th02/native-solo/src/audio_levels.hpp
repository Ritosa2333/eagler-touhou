#pragma once

namespace soew {
struct AudioLevels {
    static constexpr int step=5, maximum=100;
    int music=25, effects=25;
    bool operator==(const AudioLevels&)const=default;
};

// Source trims match the reference PMD mix before applying the player's level.
inline constexpr float midi_output_trim=1.6f;
inline constexpr float akyu_output_trim=0.70f;
}
