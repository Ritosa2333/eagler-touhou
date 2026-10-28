// 标题期平台服务（th15 ApplicationTitle.cpp 裁剪：音乐请求先落桩）。
#include "ApplicationState.hpp"
#include <algorithm>

namespace th13::sdl {

void ApplicationState::connect_record_menus() {
    player_data_host.count_records = [](i32 difficulty) {
        return FileStore::player_data_record_count(difficulty);
    };
    player_data_host.read_record = [this](i32 difficulty, i32 character, i32 ordinal,
                                         TitlePlayerDataRecord& out) {
        return files.player_data_record(difficulty, character, ordinal, out);
    };
    player_data_host.read_summary = [this](i32 difficulty, i32 character,
                                          TitlePlayerDataSummary& out) {
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
    // The original catalog is shared with production ECL; missing assets stay
    // unavailable and retain the catalog error for diagnosis.
    if (spell_catalog.load([this](const std::string& name, std::vector<u8>& bytes) {
            return read(name, bytes, false);
        }))
        spell_practice_host.catalog = &spell_catalog;
}

bool ApplicationState::sound(i32 id) { return audio_device.enqueue(id) || fail(audio_device.error); }

bool ApplicationState::hud_text(const HudTextDraw& value) {
    return captions.enqueue(value, 0, value.style.shadow) || fail(captions.error);
}

bool ApplicationState::clear_title_overlay() {
    // 过场覆盖层（th15 transition overlay）随 Stage 转场分期接入；标题无残留。
    return true;
}

bool ApplicationState::queue_title_music(i32 code, i32 value, const std::string& name) {
    return audio_device.queue_music(code, value, name) || fail(audio_device.error);
}

bool ApplicationState::clear_current_wave() { return true; }

bool ApplicationState::title_exit(i32 value) {
    // StartGame 与 Quit 共享 TitleScreen 值 2：开局请求已排队时不覆盖（th15 同款守卫）。
    if (pending_destination == i32(Destination::Run)) return true;
    pending_destination = value;
    return true;
}

bool ApplicationState::fade_out_title_music() {
    return audio_device.queue_music(5, 0, "FadeOut") || fail(audio_device.error);
}

bool ApplicationState::begin_transition(u32& animation) {
    animation = animations.create(8, 0, -1, 0);  // effect bank 转场特效
    return animation != 0;
}

bool ApplicationState::start_game(i32 stage) {
    selected_stage = stage;
    // 新局/Retry 不能沿用上一局的零残机、灵力、得分；换关则不经过此入口。
    selection_player = HudPlayerState{};
    selection_player.extra_lives = std::max(0, progress.configured_lives - 1);
    // 0042b990: Spell Practice has no spare lives/bombs and starts at full power.
    // Keep the selected card on Retry; ordinary starts clear the old selection.
    if ((progress.run_mode_flags & 0x30u) == 0x20u) {
        selection_player.extra_lives = 0;
        selection_player.bombs = 0;
        selection_player.power = selection_player.max_power;
    } else {
        progress.selected_spell_id = -1;
    }
    selection_score = HudScoreState{};
    progress.continues = 0;
    progress.life_piece_threshold_index = 0;
    progress.ending_points = 0;
    progress.run_mode_flags &= ~kTranceActiveFlag;
    progress.stage_clear_bonus = 0;
    progress.good_endings = 0;
    progress.new_run = true;
    pending_destination = i32(Destination::Run);
    return true;
}

bool ApplicationState::can_start_spell_practice(const SpellPracticeCommand& command) const {
    // 004576c0 writes stage, character, intrinsic rank and selected card together.
    if (command.action != SpellPracticeAction::Launch || command.stage < 1 ||
        command.stage > 7 || command.character < 0 || command.character >= 4 ||
        !ecl_difficulty_mask(command.rank) || command.spell_id < 0 ||
        command.spell_id >= kSaveSpellSlots)
        return false;
    const auto* entry = spell_catalog.variant(command.stage, command.spell_slot,
                                               command.difficulty);
    if (!entry || entry->id != command.spell_id || entry->rank != command.rank ||
        !spell_practice_host.spell_variant_available(command.stage, command.spell_slot,
                                                      command.difficulty))
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
    // 0043c1c0 destination 7 -> 0042c490(0): this is not Stage Practice.
    progress.practice_mode = 0;
    return start_game(command.stage);
}

bool ApplicationState::request_destination(i32 raw) {
    // 对局终了的 supervisor 请求（raw = DAT_004dcc24 真值，results-flow.md §1）。
    // 接线本阶段覆盖主线：0xc 下一关 / 0xf 结局 / 4 战后标题 / 0xa 重开 / 0xe
    // 重开（续关计费变体不在此层区分）。其余（练习 0xd/演示等）分期时补。
    // 开局请求已排队时不覆盖（th15 title_exit 同款守卫；RunScene 转发失败即
    // 回传，不静默丢 dcc24 请求）。
    switch (raw) {
    case 0xc:
        if (pending_destination != i32(Destination::Run))
            pending_destination = i32(Destination::NextStage);
        return true;
    case 0xf:
        if (pending_destination != i32(Destination::Run))
            pending_destination = i32(Destination::Ending);
        return true;
    case 0x10:
        // Ending 脚本 END（0x416610 置 DAT_004dcc24=0x10）→ 0x43c1c0 case0x10
        // 拆卸回标题（变体 3）。
        pending_destination = i32(Destination::AfterEnding);
        return true;
    case 4:
        if (pending_destination != i32(Destination::Run) &&
            pending_destination != i32(Destination::NextStage))
            pending_destination = i32(Destination::Title);
        return true;
    case 0xa:
    case 0xe:
        if (pending_destination != i32(Destination::NextStage))
            pending_destination = i32(Destination::Run);
        return true;
    default:
        return fail("Unmapped post-game destination: " + std::to_string(raw));
    }
}

// ---- TitleMenuServices / TitleOptionPlatform（r4/menu-extras 契约）----

bool ApplicationState::character_cleared(i32 character, i32 difficulty) {
    // 真实存档门：score 块 +0x56dc*char + 0x7e4 + diff*4 != 0（0x451450 case0）。
    // 缺档/坏档 FileStore 已按原版语义取全 0 → 全隐，与真机无存档一致。
    return files.cleared(character, difficulty);
}

bool ApplicationState::extra_unlocked(i32 character) {
    // Extra 解锁位（0x44bec0 读 0x1b26b+char*2 bit0x10 / 0x1b26c+char*2；
    // 0x44c570 case0 汇成 +0x5c40 bit1）。缺档 = 0 = 禁用，与真机无存档一致。
    return files.extra_unlocked(character);
}

bool ApplicationState::practice_stage_played(i32 character, i32 difficulty, i32 stage) {
    return files.practice_stage_played(character, difficulty, stage);
}

void ApplicationState::apply_volumes(i32 bgm, i32 effect) {
    // 0044ed70 侧 DAT_004e476c/70 写的 port 等价：即编即效写设备真值——
    // effect_volume 走 0x4613f3 per-voice 曲线、music_volume 混音块实时读
    //（AudioDevice.cpp :287-297/:306，实测生效非仅存字段）。
    audio_device.music_volume = bgm;
    audio_device.effect_volume = effect;
}

bool ApplicationState::persist_options(const TitleOptionVolumes& volumes) {
    // 离屏随存档落盘（原版 scoreth13.dat；网页 = FileStore.save()）。
    // 失败如实 false + error——菜单记录错误、不假成功、不阻断返回主菜单。
    files.bgm_volume = volumes.bgm;
    files.effect_volume = volumes.effect;
    return files.save();
}

bool ApplicationState::open_key_config() {
    // キー配置（case4 → screen 4 = 0x44fcc0）宿主接线：BrowserRuntime 每帧把
    // SDL 原始按钮位图写入 raw_pad_buttons（00471510 的 DAT_004e52e8 等价），
    // 五键/swapped 值来自 FileStore v3 pad_bindings；保存走同一 save()。
    // 网页无独立手柄连接位：raw 位非 0 才视为有输入（无手柄时学习自然为空，
    // 不伪造连接/默认 true）。标题场景由本对象独占，title 必存在。
    if (!title) return false;
    ApplicationState* self = this;
    title->connect_key_config(
        [self] { return self->raw_pad_buttons != 0; },
        [self] { return self->raw_pad_buttons; },
        [self] { return self->files.pad_bindings; },
        [self](const std::array<i16, 5>& bindings) {
            return self->persist_pad_bindings(bindings);
        });
    return true;
}

// ---- TitleMusicRoomPlatform（musicroom-host.md；r4/menu-extras §六）----

void ApplicationState::load_music_room_catalog() {
    music_room.clear();
    std::vector<u8> bytes;
    if (!read("musiccmt.txt", bytes, false)) return;  // 缺资产 = 空目录（如实）
    // 0x4724c0 语义：cp932 文本；'@' 起块（@bgm/播放名），块内 1 曲名行 +
    // 8 注释行；'#' 头部注释与空档天然跳过（不以 '@' 开头）。CRLF 行尾剥 \r。
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
        for (u32 k = 0; k < 9; ++k) {  // 曲名行 + 8 注释行；遇下一 '@' 提前止
            if (i + 1 >= lines.size() || (!lines[i + 1].empty() && lines[i + 1][0] == '@'))
                break;
            ++i;
            if (k == 0) track.title = lines[i];
            else track.comment[k - 1] = lines[i];
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
    // 0x4554f0 Z 路径落档：置位即存（写失败留 files.error 如实，不吞不假成功）。
    files.set_music_heard(track);
    (void)files.save();
}

bool ApplicationState::play_track(i32 track) {
    // 0x43adc0(0, 曲名) → 461830(1,0)：曲名+".wav" 入预载槽 0；
    // 461830(2,0)：播该槽（AudioDevice case 1/2 同构，0x460dd0 起停序列）。
    if (track < 0 || u32(track) >= music_room.size()) return false;
    const std::string wav = music_room[u32(track)].name + ".wav";
    return audio_device.queue_music(1, 0, wav) && audio_device.queue_music(2, 0, "");
}

bool ApplicationState::play_locked_response() {
    return audio_device.queue_music(3, 0, "");  // 0x461830(3/4,0)：锁定曲停曲响应
}

bool ApplicationState::resume_title_music() {
    // 0x43adc0(0,"th13_00")：退出收面回标题曲。
    return audio_device.queue_music(1, 0, "th13_00.wav") && audio_device.queue_music(2, 0, "");
}

}  // namespace th13::sdl
