#include "../game/AnmTextureDecode.hpp"
#include "ApplicationState.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <vector>

namespace th13::sdl {

void ApplicationState::connect_record_menus() {
    player_data_host.count_records = [](i32 difficulty) { return FileStore::player_data_record_count(difficulty); };
    player_data_host.read_record = [this](i32 difficulty, i32 character, i32 ordinal, TitlePlayerDataRecord& out) {
        return files.player_data_record(difficulty, character, ordinal, out);
    };
    player_data_host.read_summary = [this](i32 difficulty, i32 character, TitlePlayerDataSummary& out) {
        return files.player_data_summary(difficulty, character, out);
    };
    spell_practice_host.encountered = [this](i32 id) {
        const auto* counters = files.spell_counters(4, id);
        return counters && counters->attempts[0] > 0;
    };
    spell_practice_host.captured = [this](i32 id) {
        const auto* counters = files.spell_counters(4, id);
        return counters && (counters->captures[0] > 0 || counters->captures[1] > 0);
    };
    spell_practice_host.named = [this](i32 id) {
        const auto* counters = files.spell_counters(4, id);
        return counters && (counters->attempts[0] > 0 || counters->attempts[1] > 0);
    };
    if (spell_catalog.load(
            [this](const std::string& name, std::vector<u8>& bytes) { return read(name, bytes, false); }))
        spell_practice_host.catalog = &spell_catalog;
}

bool ApplicationState::sound(i32 id) { return audio_device.enqueue(id) || fail(audio_device.error); }

bool ApplicationState::hud_text(const HudTextDraw& value) {
    return captions.enqueue(value, 0, value.style.shadow) || fail(captions.error);
}

bool ApplicationState::clear_title_overlay() { return true; }

bool ApplicationState::queue_title_music(i32 code, i32 value, const std::string& name) {
    return audio_device.queue_music(code, value, name) || fail(audio_device.error);
}

bool ApplicationState::clear_current_wave() { return true; }

bool ApplicationState::title_exit(i32 value) {
    if (pending_destination == i32(Destination::Run)) return true;
    pending_destination = value;
    return true;
}

bool ApplicationState::fade_out_title_music() {
    return audio_device.queue_music(5, 0, "FadeOut") || fail(audio_device.error);
}

bool ApplicationState::begin_transition(u32& animation) {
    if (!begin_run_transition()) return false;
    animation = run_transition.handle();
    return true;
}

void ApplicationState::set_practice(bool enabled, i32 section, i32 life, i32 bomb, i32 power, i32 trance) {
    practice.enabled = enabled;
    practice.section = section;
    practice.life = life;
    practice.bomb = bomb;
    practice.power = power;
    practice.trance = trance;
}
i32 ApplicationState::practice_stage() const {
    if (practice.enabled) return th13::resolve_practice(practice, selected_stage).stage;
    return active_practice.applied ? active_practice.stage : 0;
}

bool ApplicationState::start_game(i32 stage) {
    active_practice = th13::begin_thprac(practice, progress, stage);
    return queue_run(stage);
}

bool ApplicationState::queue_run(i32 stage) {
    selected_stage = active_practice.applied ? active_practice.stage : stage;
    selection_player = HudPlayerState{};
    selection_player.extra_lives = std::max(0, progress.configured_lives - 1);
    if ((progress.run_mode_flags & 0x30u) == 0x20u) {
        selection_player.extra_lives = 0;
        selection_player.bombs = 0;
        selection_player.power = selection_player.max_power;
    } else {
        progress.selected_spell_id = -1;
    }
    const i32 entry_steps = (selected_stage > 1 || (progress.run_mode_flags & 0x30u) == 0x20u) ? 4 : 1;
    selection_player.power = std::min(entry_steps * selection_player.power_step, selection_player.max_power);
    selection_score = HudScoreState{};
    progress.continues = 0;
    progress.life_piece_threshold_index = 0;
    progress.point_items = 0;
    trance_reset_fresh_entry(progress);
    if (active_practice.applied) {
        if (active_practice.life >= 0) selection_player.extra_lives = active_practice.life;
        if (active_practice.bomb >= 0) selection_player.bombs = active_practice.bomb;
        if (active_practice.power >= 0) selection_player.power = active_practice.power;
        if (active_practice.trance >= 0) progress.ending_points = active_practice.trance;
    }
    progress.run_mode_flags &= ~kTranceActiveFlag;
    progress.stage_clear_bonus = 0;
    progress.good_endings = 0;
    progress.new_run = true;
    pending_destination = i32(Destination::Run);
    return true;
}

bool ApplicationState::can_start_spell_practice(const SpellPracticeCommand& command) const {
    if (command.action != SpellPracticeAction::Launch || command.stage < 1 || command.stage > 7 ||
        command.character < 0 || command.character >= 4 || !ecl_difficulty_mask(command.rank) || command.spell_id < 0 ||
        command.spell_id >= kSaveSpellSlots)
        return false;
    const auto* entry = spell_catalog.variant(command.stage, command.spell_slot, command.difficulty);
    if (!entry || entry->id != command.spell_id || entry->rank != command.rank ||
        !spell_practice_host.spell_variant_available(command.stage, command.spell_slot, command.difficulty))
        return false;
    return true;
}

bool ApplicationState::start_spell_practice(const SpellPracticeCommand& command) {
    if (!can_start_spell_practice(command)) return false;
    progress.stage = progress.starting_stage = command.stage;
    progress.character = command.character;
    progress.difficulty = command.rank;
    progress.selected_spell_id = command.spell_id;
    progress.run_mode_flags = (progress.run_mode_flags & ~0x70u) | 0x20u;
    progress.practice_mode = 0;
    return start_game(command.stage);
}

bool ApplicationState::request_destination(i32 raw) {
    switch (raw) {
    case 0xc:
        if (pending_destination != i32(Destination::Run)) pending_destination = i32(Destination::NextStage);
        return true;
    case 0xf:
        if (pending_destination != i32(Destination::Run)) pending_destination = i32(Destination::Ending);
        return true;
    case 0x10:
        pending_destination = i32(Destination::AfterEnding);
        return true;
    case 4:
        if (pending_destination != i32(Destination::Run) && pending_destination != i32(Destination::NextStage))
            pending_destination = i32(Destination::Title);
        return true;
    case 0xa:
    case 0xe:
        if (pending_destination != i32(Destination::NextStage)) pending_destination = i32(Destination::Run);
        return true;
    default:
        return fail("Unmapped post-game destination: " + std::to_string(raw));
    }
}

bool ApplicationState::character_cleared(i32 character, i32 difficulty) { return files.cleared(character, difficulty); }

bool ApplicationState::extra_unlocked(i32 character) { return files.extra_unlocked(character); }

bool ApplicationState::practice_stage_played(i32 character, i32 difficulty, i32 stage) {
    return files.practice_stage_played(character, difficulty, stage);
}

void ApplicationState::apply_volumes(i32 bgm, i32 effect) {
    audio_device.music_volume = bgm;
    audio_device.effect_volume = effect;
}

bool ApplicationState::persist_options(const TitleOptionVolumes& volumes) {
    files.bgm_volume = volumes.bgm;
    files.effect_volume = volumes.effect;
    return files.save();
}

bool ApplicationState::open_key_config() {
    if (!title) return false;
    ApplicationState* self = this;
    title->connect_key_config(
        [self] { return self->raw_pad_buttons != 0; }, [self] { return self->raw_pad_buttons; },
        [self] { return self->files.pad_bindings; },
        [self](const std::array<i16, 5>& bindings) { return self->persist_pad_bindings(bindings); });
    return true;
}

void ApplicationState::load_music_room_catalog() {
    music_room.clear();
    std::vector<u8> bytes;
    if (!read("musiccmt.txt", bytes, false)) return;
    std::vector<std::string> lines;
    const u8* const base = bytes.data();
    for (size_t at = 0; at < bytes.size();) {
        const u8* begin = base + at;
        const u8* end = std::find(begin, base + bytes.size(), '\n');
        size_t length = size_t(end - begin);
        if (length && begin[length - 1] == '\r') --length;
        lines.emplace_back(reinterpret_cast<const char*>(begin), length);
        at = size_t(end - base) + (end != base + bytes.size() ? 1 : 0);
    }
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].size() < 4 || lines[i][0] != '@') continue;
        const size_t slash = lines[i].find('/');
        if (slash == std::string::npos) continue;
        MusicRoomTrack track;
        track.name = lines[i].substr(slash + 1);
        for (u32 k = 0; k < 9; ++k) {
            if (i + 1 >= lines.size() || (!lines[i + 1].empty() && lines[i + 1][0] == '@')) break;
            ++i;
            if (k == 0)
                track.title = lines[i];
            else
                track.comment[k - 1] = lines[i];
        }
        music_room.push_back(std::move(track));
    }
}

i32 ApplicationState::track_count() const { return i32(music_room.size()); }

std::string ApplicationState::track_title(i32 track) {
    if (track < 0 || u32(track) >= music_room.size()) return {};
    return music_room[u32(track)].title;
}

std::string ApplicationState::track_comment(i32 track, i32 line) {
    if (track < 0 || u32(track) >= music_room.size() || line < 0 || line > 7) return {};
    return music_room[u32(track)].comment[u32(line)];
}

bool ApplicationState::track_heard(i32 track) const { return files.music_heard(track); }

void ApplicationState::mark_heard(i32 track) {
    files.set_music_heard(track);
    (void)files.save();
}

bool ApplicationState::play_track(i32 track) {
    if (track < 0 || u32(track) >= music_room.size()) return false;
    const std::string wav = music_room[u32(track)].name + ".wav";
    return audio_device.queue_music(1, 0, wav) && audio_device.queue_music(2, 0, "");
}

bool ApplicationState::play_locked_response() { return audio_device.queue_music(3, 0, ""); }

bool ApplicationState::resume_title_music() {
    return audio_device.queue_music(1, 0, "th13_00.wav") && audio_device.queue_music(2, 0, "");
}

namespace {
constexpr i32 kHelpBank = 19;
constexpr u32 kHelpPageTexture = 1;
constexpr i32 kHelpPageCount = 8;
}

bool ApplicationState::open_manual() {
    if (!manual_viewer) {
        manual_viewer = std::make_unique<ManualViewer>(animations, kHelpBank);
        manual_viewer->sound = [this](i32 id) { return sound(id); };
        manual_viewer->load_page = [this](i32 page) -> bool {
            if (page < 0 || page >= kHelpPageCount) return fail("Manual page index outside help.anm");
            char name[16];
            std::snprintf(name, sizeof name, "help_%02d.png", page + 1);
            std::vector<u8> bytes;
            if (!read(name, bytes, false) || bytes.empty())
                return fail(std::string("Manual page asset unavailable: ") + name);
            u32 width = 0, height = 0;
            std::vector<u8> rgba;
            if (!anm_decode_png(bytes.data(), u32(bytes.size()), width, height, rgba))
                return fail(std::string("Manual page PNG decode failed: ") + name);
            if (!width || !height || rgba.size() < size_t(width) * height * 4)
                return fail(std::string("Manual page PNG has no pixels: ") + name);
            auto* resource = animations.resource(kHelpBank);
            if (!resource) return fail("Manual help.anm bank unavailable");
            const u32 handle = graphics.texture(*resource, kHelpPageTexture);
            TextureImage* target = handle ? graphics.pixels(handle) : nullptr;
            if (!target || target->format != touhou::graphics::PixelFormat::Argb4444 ||
                target->pitch != target->width * 2)
                return fail("Manual help.anm blank texture unavailable");
            std::fill(target->pixels.begin(), target->pixels.end(), u8(0));
            const u32 copy_width = std::min(width, target->width);
            const u32 copy_height = std::min(height, target->height);
            for (u32 y = 0; y < copy_height; ++y) {
                auto* destination = target->pixels.data() + size_t(y) * target->pitch;
                const u8* source = rgba.data() + size_t(y) * width * 4;
                for (u32 x = 0; x < copy_width; ++x) {
                    const u8* pixel = source + size_t(x) * 4;
                    const u16 alpha = u16((u32(pixel[3]) * 15u + 127u) / 255u);
                    const u16 red = u16((u32(pixel[0]) * 15u + 127u) / 255u);
                    const u16 green = u16((u32(pixel[1]) * 15u + 127u) / 255u);
                    const u16 blue = u16((u32(pixel[2]) * 15u + 127u) / 255u);
                    const u16 value = u16((alpha << 12) | (red << 8) | (green << 4) | blue);
                    std::memcpy(destination + size_t(x) * 2, &value, 2);
                }
            }
            graphics.changed(handle, {0, 0, target->width, target->height});
            return true;
        };
    }
    if (!manual_viewer->open()) {
        fail(manual_viewer->error.empty() ? "Title manual viewer unavailable" : manual_viewer->error);
        return false;
    }
    return true;
}

bool ApplicationState::update_manual(u32 pressed, u32 repeated) {
    if (!manual_viewer) return fail("Title manual viewer unavailable");
    if (!manual_viewer->update(pressed, repeated)) {
        fail(manual_viewer->error.empty() ? "Title manual update failed" : manual_viewer->error);
        return false;
    }
    return true;
}

bool ApplicationState::manual_finished() const { return manual_viewer && manual_viewer->finished(); }

void ApplicationState::close_manual() {
    if (manual_viewer) manual_viewer->close();
}

}
