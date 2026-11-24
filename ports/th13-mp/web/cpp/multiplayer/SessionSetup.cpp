#include "SessionSetup.hpp"

namespace th13::multiplayer {

bool SessionSetup::valid() const {
    if (keep_bullets_on_miss && (!challenge || version == LegacyVersion)) return false;
    if ((version != LegacyVersion && version != Version) || mode > 2 ||
        (version == LegacyVersion && mode != 1) ||
        prediction_limit != MaxRollbackFrames || measured_prediction > 2 ||
        (mode != 2 && measured_prediction) ||
        (mode == 0 && (automatic || requested_delay || input_delay)))
        return false;
    if (!session_id || player_count < 2 || player_count > 3 ||
        local_player >= player_count || difficulty > 5 || seed > 65535 ||
        requested_delay > 9 || input_delay > 9 || prediction_reserve < 1 ||
        prediction_reserve > 2 || (automatic && requested_delay) ||
        !(build[0] | build[1] | build[2] | build[3]))
        return false;
    for (unsigned seat = 0; seat < 3; ++seat)
        if (selections[seat] >= MaxCharacters ||
            (seat >= player_count && selections[seat]))
            return false;
    return true;
}

bool DecodeSessionSetup(SessionSetup& out, const std::uint32_t* words, std::size_t count) {
    if (!words || count != SessionSetup::Words ||
        (words[0] != SessionSetup::LegacyVersion && words[0] != SessionSetup::Version) ||
        words[14] > 2 || (words[0] == SessionSetup::LegacyVersion && words[14] != 1) ||
        words[15] > 1 || words[21] > 3 || words[21] == 2)
        return false;
    SessionSetup setup;
    setup.version = words[0];
    setup.mode = words[14];
    setup.player_count = words[1];
    setup.local_player = words[2];
    setup.difficulty = words[3];
    setup.seed = words[4];
    setup.session_id = words[5] | (std::uint64_t(words[6]) << 32);
    setup.requested_delay = setup.input_delay = words[7];
    for (unsigned seat = 0; seat < 3; ++seat) setup.selections[seat] = words[8 + seat];
    setup.automatic = words[15];
    setup.prediction_reserve = words[16];
    for (unsigned i = 0; i < 4; ++i) setup.build[i] = words[17 + i];
    setup.challenge = (words[21] & 1) != 0;
    setup.keep_bullets_on_miss = (words[21] & 2) != 0;
    if (!setup.valid()) return false;
    out = setup;
    return true;
}

std::array<std::uint32_t, SessionSetup::Words> EncodeSessionSetup(const SessionSetup& setup) {
    std::array<std::uint32_t, SessionSetup::Words> words{};
    words[0] = setup.version;
    words[1] = setup.player_count;
    words[2] = setup.local_player;
    words[3] = setup.difficulty;
    words[4] = setup.seed;
    words[5] = std::uint32_t(setup.session_id);
    words[6] = std::uint32_t(setup.session_id >> 32);
    words[7] = setup.requested_delay;
    for (unsigned seat = 0; seat < 3; ++seat) words[8 + seat] = setup.selections[seat];
    words[14] = setup.mode;
    words[15] = setup.automatic;
    words[16] = setup.prediction_reserve;
    for (unsigned i = 0; i < 4; ++i) words[17 + i] = setup.build[i];
    words[21] = std::uint32_t(setup.challenge) + 2 * std::uint32_t(setup.keep_bullets_on_miss);
    return words;
}

std::uint32_t GameplayAbi(const SessionSetup& setup) {
    std::uint32_t hash = 2166136261u;
    const auto word = [&](std::uint32_t value) {
        for (unsigned i = 0; i < 4; ++i) {
            hash ^= (value >> (i * 8)) & 255;
            hash *= 16777619u;
        }
    };
    word(0x54483133u);
    word(SessionSetup::GameplayVersion);
    word(SessionSetup::RulesVersion);
    word(setup.player_count);
    word(setup.difficulty);
    for (auto selection : setup.selections) word(selection);
    word(std::uint32_t(setup.challenge) + 2 * std::uint32_t(setup.keep_bullets_on_miss));
    word(setup.automatic);
    word(setup.requested_delay);
    word(setup.input_delay);
    word(setup.prediction_reserve);
    for (auto build : setup.build) word(build);
    if (setup.mode != 1) {
        word(0x52423600u);
        word(setup.mode);
        word(setup.prediction_limit);
    }
    return hash ? hash : 1;
}

}
