#include "GameSession.hpp"
#include "GameConfig.hpp"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace th16 {

namespace {
// 0x42d200 / 0x41a390：Loading VM 位置 = 2 × (480, 392) 寄存器实参 = (960, 784)
constexpr Vec3 kLoadingVmPos{960.0f, 784.0f, 0.0f};

// 目录型 ReplayStore：读槽 0x447ef0 / 写槽 0x448400（经 ReplayRecording）
struct DiskReplayStore final : ReplayStore {
    GameSession* session = nullptr;
    bool slot_exists(int slot) override {
        if (session == nullptr || session->replay_directory().empty()) return false;
        return ReplayRecording::slot_exists(session->replay_directory().c_str(), slot);
    }
    void save(int slot, const char* name) override {
        if (session == nullptr) return;
        session->save_replay_slot(slot, name);
    }
    bool slot_summary(int slot, ReplaySummary& out) override {
        if (session == nullptr || session->replay_directory().empty()) return false;
        return ReplayRecording::slot_summary(session->replay_directory().c_str(), slot, out);
    }
};
}  // namespace


GameSession::GameSession() {
    sfx_events.effects = &sound;   // 各管理器的 0x45e150 / 0x45e1f0 / 0x45e2a0 请求即时进 SoundManager 队列
    // 原版 DAT_004df49c / DAT_004df4a0 只在 Option 画面（0x44dc70）写入——开机到首次进 Option 前为 0。
    // DAT_004c12a4 配置块在 BSS，进程起来是 0；0x43c050 读 th16.cfg 失败才写成 100/80。
    // harness 不读配置文件时保持这份 BSS 初值，Option 数字的前导零才和 title-oracle 一致。
    globals.bgm_volume = 0;
    globals.se_volume = 0;
    music.volume = 0;
    sound.master_volume = 0;
    // 0x43ba40：Supervisor 十条合成绘制链；0x46a3a0：AnmManager 各层绘制链。都在进程启动时登记、常驻
    compositor.screen_shake = &screen_shake;
    compositor.render_target_camera = &stage_camera;   // 同一全局 DAT_004c168c
    animations.geometry = &geometry;   // 0x4632f0 条带几何的屏幕常量
    compositor.init_cameras(geometry.window_scale);
    compositor.register_draw_chains(chains, renderer);
    register_layer_draw_chains();
    // 0x43b520：FrameTimer 绘制链 0x4b（0x426490 → 0x4263e0）；标题 / 结局不画。
    // 建为禁用：宿主给出墙钟（wall_clock_seconds ≥ 0）后帧泵 0x4262f0 测速时才启用，无墙钟的 harness 绘制黄金不含 "%2.1ffps"。
    global_fps_draw_chain_ = chains.create([this]() -> ChainResult {
        if (supervisor.current == SceneId::Title || supervisor.current == SceneId::Ending)
            return ChainResult::Continue;
        ascii.style.color = global_fps_color(global_fps_);
        ascii.printf_small(Vec3{588.0f, 470.0f, 0.0f}, "%2.1ffps", double(global_fps_));
        ascii.style.color = 0xffffffffu;
        return ChainResult::Continue;
    }, 0x4b, false);
}

bool GameSession::open(const u8* bytes, u32 size) {
    if (!archive_.open(bytes, size)) { failure = archive_.error; return false; }
    return true;
}

bool GameSession::save_replay_slot(int slot, const char* player_name) {
    if (replay_directory().empty()) {
        failure = "replay directory empty";
        return false;
    }
    const float slow = frame_timer.slow_rate();
    if (!replay_recording_.write_slot(replay_directory().c_str(), slot, player_name, globals.score, slow)) {
        failure = replay_recording_.error.empty() ? "replay save failed" : replay_recording_.error;
        return false;
    }
    globals.replay_slot = slot;
    if (on_saved) on_saved();
    return true;
}

bool GameSession::load_replay(const u8* bytes, u32 size) {
    replay_file_loaded_ = false;
    replay_cursor_ = ReplayPlaybackCursor{};
    if (!replay_file_.open(bytes, size)) {
        failure = replay_file_.error;
        return false;
    }
    return bind_loaded_replay_();
}

bool GameSession::load_replay_file(const char* path) {
    replay_file_loaded_ = false;
    replay_cursor_ = ReplayPlaybackCursor{};
    if (!replay_file_.open_file(path)) {
        failure = replay_file_.error;
        return false;
    }
    failure.clear();
    return bind_loaded_replay_();
}

bool GameSession::bind_loaded_replay_() {
    replay_file_loaded_ = true;
    replay_cursor_ = ReplayPlaybackCursor{};
    for (i32 i = 0; i < i32(replay_file_.stages.size()); ++i) {
        if (replay_file_.stages[size_t(i)].stage == i16(globals.stage)) {
            replay_cursor_.stage_index = i;
            break;
        }
    }
    if (!replay_file_.stages.empty()) {
        const auto& seg = replay_file_.stages[size_t(replay_cursor_.stage_index)];
        // 0x447760(1)：只有 DAT_004a6d88 取段头 +2 的种子；DAT_004a6d80 沿用（换关 0x449030 才两者同置）
        animations.visual_rng.seed = seg.rng_seed;
        animations.visual_rng.calls = 0;
        if (!seg.fps.empty()) replay_fps_byte_ = seg.fps[0];
    }
    return true;
}

const ReplayStageSegment* GameSession::current_replay_segment_() const {
    if (!replay_file_loaded_) return nullptr;
    const i32 i = replay_cursor_.stage_index;
    if (i < 0 || i >= i32(replay_file_.stages.size())) return nullptr;
    const ReplayStageSegment& seg = replay_file_.stages[size_t(i)];
    return seg.header.size() == ReplayFile::kStageHeaderSize ? &seg : nullptr;
}

i32 GameSession::replay_spell_time_(i32 index) const {
    // 0x417bc0：DAT_004a6f08 +0xd4 + DAT_004a5790 × 0x28 段头 +0x240 + 计次 × 4（无越界检查，超出 0x14 槽读进段头后的输入）
    if (!replay_file_loaded_ || index < 0) return 0;
    for (const ReplayStageSegment& seg : replay_file_.stages) {
        if (seg.stage != i16(globals.stage) || seg.header.size() != ReplayFile::kStageHeaderSize) continue;
        const size_t offset = size_t(kSpellTimeOffset) + size_t(index) * 4;
        u8 bytes[4]{};
        for (size_t k = 0; k < 4; ++k) {
            const size_t at = offset + k;
            if (at < seg.header.size()) bytes[k] = seg.header[at];
            else if (at - seg.header.size() < seg.inputs.size()) bytes[k] = seg.inputs[at - seg.header.size()];
        }
        i32 value = 0;
        std::memcpy(&value, bytes, 4);
        return value;
    }
    return 0;
}

void GameSession::replay_stage_start_() {
    Player& player = game.player;
    // 0x448eb0：+0x20c 清零（两种模式）；回放时 +0x214 才切到本关段并从段首读，记录时才开新段
    replay_ff_counter_ = 0;
    if (game.mode == 1) {
        if (replay_next_stage_index_ >= 0) {
            replay_cursor_.stage_index = replay_next_stage_index_;
            replay_next_stage_index_ = -1;
        }
        replay_cursor_.frame_index = 0;
        replay_cursor_.ended = false;
        const ReplayStageSegment* seg = current_replay_segment_();
        if (seg == nullptr || !game.player_live) return;
        // 0x4476d0（段头 +0xc）：定点坐标与浮点坐标、四个子机 +0xd4 吸附位；再 +0x23c → 低速标志、0x4440e0
        const u8* header = seg->header.data();
        std::memcpy(&player.fixed_x, header + 0xc, 4);
        std::memcpy(&player.fixed_y, header + 0x10, 4);
        player.pos.x = float(player.fixed_x) * 0.0078125f;
        player.pos.y = float(player.fixed_y) * 0.0078125f;
        for (PlayerOption& option : player.options) option.snap = 1;
        std::memcpy(&player.focus, header + 0x23c, 4);
        player.setup_options(animations, globals);
        return;
    }
    // 记录：0x449270 / 0x4491c0 换到本关段（此前的帧仍记在上一段）；换关（DAT_004c17d0 == 0）时重抄全局块；
    // 自机定点坐标 / 低速标志每段都写
    replay_recording_.begin_stage(i16(globals.stage), replay_stage_seed_);
    if (!globals.new_game) replay_recording_.set_stage_globals(globals);
    if (game.player_live) replay_recording_.set_stage_start(player.fixed_x, player.fixed_y, player.focus);
}

AnmResource* GameSession::resource(const char* name) {
    for (size_t i = 0; i < resource_names_.size(); ++i)
        if (resource_names_[i] == name) return resources_[i].get();
    return nullptr;
}

bool GameSession::load_anm(const char* name, AnmResource*& out) {
    if (AnmResource* existing = resource(name)) { out = existing; return true; }
    std::vector<u8> bytes;
    if (!archive_.read(name, bytes)) { failure = archive_.error; return false; }
    resources_.push_back(std::make_unique<AnmResource>());
    resource_names_.emplace_back(name);
    AnmResource& res = *resources_.back();
    if (!res.parse(bytes.data(), u32(bytes.size()), name)) { failure = std::string(name) + ": " + res.error; return false; }
    for (u32 i = 0; i < res.entries.size(); ++i) {
        const auto& e = res.entries[i];
        if (e.has_data || e.name.empty() || e.name[0] == '@') continue;
        std::vector<u8> external;
        if (!archive_.read(e.name.c_str(), external)) { failure = archive_.error; return false; }
        if (!res.attach_external_texture(i, external.data(), u32(external.size()))) { failure = res.error; return false; }
    }
    if (on_loaded && !on_loaded(res)) { if (failure.empty()) failure = "texture upload failed"; return false; }
    out = &res;
    return true;
}

bool GameSession::load_boot_banks() {
    AnmResource* ascii = nullptr;
    AnmResource* sig = nullptr;
    // 0x407cb0 AsciiManager：ascii.anm → 槽 2，更新链 4 与三条按组绘制的链
    if (!load_anm("ascii.anm", ascii)) return false;
    AnmFile* ascii_file = animations.load(AnmManager::kAsciiSlot, *ascii);
    if (ascii_file == nullptr) { failure = "ascii.anm load failed"; return false; }
    this->ascii.window_scale = geometry.window_scale;
    this->ascii.create(animations, *ascii_file);
    ascii_update_chain_ = chains.create([this]() -> ChainResult {
        this->ascii.update();
        return ChainResult::Continue;
    }, 4, true);
    chains.enable(ascii_update_chain_);
    ascii_draw_chains_[0] = chains.create([this]() -> ChainResult {
        this->ascii.draw_group(AsciiManager::Group::Ui, renderer, compositor.ui_camera);
        return ChainResult::Continue;
    }, 0x51, false);
    ascii_draw_chains_[1] = chains.create([this]() -> ChainResult {
        this->ascii.draw_field_group(AsciiManager::Group::Field, renderer, compositor.field_camera, compositor.ui_camera);
        return ChainResult::Continue;
    }, 0x35, false);
    ascii_draw_chains_[2] = chains.create([this]() -> ChainResult {
        this->ascii.draw_field_group(AsciiManager::Group::FieldLate, renderer, compositor.field_camera, compositor.ui_camera);
        return ChainResult::Continue;
    }, 0x42, false);
    // 四条链此时都还关着：启动幕装载完成（0x43b290）才一起打开
    // 0x43adc0：sig.anm → 槽 1
    if (!load_anm("sig.anm", sig)) return false;
    if (animations.load(1, *sig) == nullptr) { failure = "sig.anm load failed"; return false; }
    // 0x43adc0：thbgm.fmt（DAT_004db790）——缺失时原版报错退出，端口只让乐曲查表落到首曲
    {
        std::vector<u8> fmt;
        if (archive_.read("thbgm.fmt", fmt)) music.load_table(fmt.data(), fmt.size());
    }
    // 0x43adc0 余下同步装载：text.anm → 槽 0（DAT_004c17f8）；0x43ad20：front.anm → 槽 5；
    // 0x418790（EffectManager）：effect.anm → 槽 8、bullet.anm → 槽 7
    static constexpr struct { const char* name; int slot; } kBootBanks[] = {
        {"text.anm", 0}, {"front.anm", 5}, {"effect.anm", 8}, {"bullet.anm", 7},
    };
    for (const auto& bank : kBootBanks) {
        AnmResource* res = nullptr;
        if (!load_anm(bank.name, res)) return false;
        if (animations.load(bank.slot, *res) == nullptr) { failure = std::string(bank.name) + " load failed"; return false; }
    }
    // 0x43d970：text.anm 装好后取两张 '@R' 渲染目标，并建四个画面合成 VM
    compositor.create(animations, *animations.file(ScreenCompositor::kTextSlot));
    // 0x418a30：EffectManager 持 effect.anm / bullet.anm，并把 VM 回调（充能环等）挂到 AnmManager
    effects.create(animations, animations.file(8), animations.file(7));
    effect_update_chain_ = chains.create([this]() -> ChainResult {
        effects.sweep();
        return ChainResult::Continue;
    }, 0x1f, true);
    chains.enable(effect_update_chain_);
    // 0x418790 → 0x46e740 / 0x46e710：AnmManager 的两条推进链。UI 表（链 9）排在 GameManager 之前，暂停时仍推进；
    // 世界表（链 0x21）在 GameManager 返回 3 时与其它游戏链一起被截断，且装载中（flags bit0|bit2 且 bit1）跳过。
    // 速度不在这里抄：管理器的 game_speed 就是 Supervisor 的那一份（0x45f980 每次跑 VM 直接读 DAT_004a5788）
    ui_tick_chain_ = chains.create([this]() -> ChainResult {
        animations.tick_ui();
        return ChainResult::Continue;
    }, 9, true);
    chains.enable(ui_tick_chain_);
    world_tick_chain_ = chains.create([this]() -> ChainResult {
        if (game_live) {
            const u32 f = game.flags;
            if ((((f >> 2) | f) & 1u) != 0 && (f & 2u) != 0) return ChainResult::Continue;
        }
        animations.tick_world();
        return ChainResult::Continue;
    }, 0x21, true);
    chains.enable(world_tick_chain_);
    // 0x43adc0 → 0x4497e0：默认块（0x449720 的 noise 对 DAT_004a6d88 做 0x1ee 次单步随机，0 -> 0x41b6），再由 0x449880 覆盖读入的块
    scores.reset();
    for (u16& n : scores.stats.noise) n = animations.visual_rng.next16();
    if (!score_path.empty()) {
        std::ifstream in(score_path, std::ios::binary);
        if (in) {
            const std::vector<u8> bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            scores.load_image(bytes.data(), u32(bytes.size()));
        }
    }
    return true;
}

bool GameSession::begin_boot() {
    if (!load_boot_banks()) return false;

    supervisor.boot_frames = 0;
    supervisor.current = SceneId::Boot;

    // 0x43af60：启动幕绘制链（优先级 0x44）；0x43adc0 末尾启用
    boot_draw_chain_ = chains.create([this]() -> ChainResult {
        // 0x43b300：+0x630==1 时生成 sig 脚本 0；+0x634==1 且 ascii+0x19248 为空时
        // 0x40e5c0 生成脚本 0x11 @ (960, 784)（ebp-0x14 = 0x44700000 / 0x44440000）。之后 +0x638 递增。
        if (supervisor.boot_frames == 0) {
            if (AnmFile* sig = animations.file(1)) animations.spawn(*sig, 0, -1);
            spawn_loading_vm(kLoadingVmPos);
        }
        ++supervisor.boot_frames;
        return ChainResult::Continue;
    }, 0x44, false);
    chains.enable(boot_draw_chain_);

    // 0x43b290：装载完成——打开 AsciiManager 的更新 / 绘制链，请求 Title
    enable_ascii_chains();
    supervisor.boot_init_finished();
    return true;
}

void GameSession::enable_ascii_chains() {
    chains.enable(ascii_update_chain_);
    for (ChainEntry* entry : ascii_draw_chains_) chains.enable(entry);
}

bool GameSession::enter_title_assets() {
    AnmResource* title = nullptr;
    AnmResource* title_v = nullptr;
    // 0x44ac70：title.anm → 槽 0x10，title_v.anm → 槽 0x11
    if (!load_anm("title.anm", title)) return false;
    if (!load_anm("title_v.anm", title_v)) return false;
    if (animations.load(0x10, *title) == nullptr) { failure = "title.anm load failed"; return false; }
    if (animations.load(0x11, *title_v) == nullptr) { failure = "title_v.anm load failed"; return false; }
    supervisor.title_assets_ready = true;
    supervisor.title_menu_live = false;
    return true;
}

bool GameSession::activate_title_menu() {
    // 0x44ac21：卸 sig.anm（槽 1），摘启动幕绘制链
    animations.kill_file(1);
    animations.tick_world();
    animations.unload(1);
    if (boot_draw_chain_) { chains.remove(boot_draw_chain_); boot_draw_chain_ = nullptr; }

    AnmFile* title_file = animations.file(0x10);
    AnmFile* title_v_file = animations.file(0x11);
    if (title_file == nullptr || title_v_file == nullptr) { failure = "title banks missing"; return false; }

    // 0x44af80 case 0 → 0x44a560(1) → 0x44b5f0：标题对象在首次更新链调用时自行生成装饰/logo
    title.activate(title_file, title_v_file, animations.file(AnmManager::kAsciiSlot), globals);
    title.bgm = &music;   // activate 重置了标题对象：乐曲 / 音效 / 游戏速度指针在此重挂
    title.sound = &sound;
    title.game_speed = &supervisor.game_speed;
    title.scores = &scores;
    title.config_flags = &config_flags;
    title.keyboard_state = keyboard_state;
    title.pad_buttons = &pad_buttons;
    title.loading_vm = &game.loading_vm;
    title.text_paints = &text_into_texture;
    title.replay_directory = replay_directory();
    title.read_replay_slot = [this](int slot, ReplaySummary& out) {
        return !replay_directory().empty() && ReplayRecording::slot_summary(replay_directory().c_str(), slot, out);
    };
    // 0x448400 写失败（目录不可写 / 本局无记录段）时原版只是不落盘，标题照常回到槽列表
    title.write_replay_slot = [this](int slot, const char* name) {
        if (replay_directory().empty()) return false;
        if (!replay_recording_.write_slot(replay_directory().c_str(), slot, name, globals.score,
                                          frame_timer.slow_rate()))
            return false;
        if (on_saved) on_saved();
        return true;
    };
    title.current_replay_summary = [this]() { return replay_recording_.summary(); };
    title.on_replay_full_run = [this]() { replay_recording_.mark_full_run(i32(std::time(nullptr))); };
    title.on_screen_fade = [this](i32 kind, i32 duration, u32 color) { create_screen_fade(kind, duration, color); };
    title.on_transition_effect = [this]() {
        // 0x41a390 + 0x418ba0 + 0x46f0b0(..., 7)：Loading VM、过场种类 0、中断 7
        spawn_loading_vm(kLoadingVmPos);
        globals.transition_effect_id = effects.attach(0, nullptr, {});
        animations.interrupt(globals.transition_effect_id, 7);
    };
    title.load_help_anm = [this]() -> AnmFile* {
        if (AnmFile* loaded = animations.file(0x13)) return loaded;
        AnmResource* res = nullptr;
        if (!load_anm("help.anm", res)) return nullptr;
        return animations.load(0x13, *res);
    };
    {
        std::vector<u8> cmt;
        if (archive_.read("musiccmt.txt", cmt) && !cmt.empty()) {
            title.parse_music_cmt(reinterpret_cast<const char*>(cmt.data()), cmt.size());
        }
    }
    globals.scene_request = -1;

    // 0x44ac54：启用标题更新链（优先级 6 → 0x44af80）
    title_update_chain_ = chains.create([this]() -> ChainResult {
        title.update(animations, supervisor.input);
        title.draw(ascii);  // 0x44b530：关卡练习分 / 符卡 SCORE 等 Ascii 行
        if (globals.scene_request >= 0) {
            supervisor.request(SceneId(globals.scene_request));
            globals.scene_request = -1;
        }
        return ChainResult::Continue;
    }, 6, true);
    chains.enable(title_update_chain_);

    supervisor.title_menu_live = true;
    return true;
}

void GameSession::sample_input() {
    // 0x401d50：键盘位（宿主固定映射 + 注入）| 0x4018e0 手柄按钮表合成位
    supervisor.input.held = host_input | injected_input | globals.pad_input_bits(pad_buttons);
    supervisor.input.commit();
}

bool GameSession::load_config() {
    // 0x43c050：读 <游戏目录>/th16.cfg → 校验（失败整块回默认）→ 无论如何回写一份
    bool valid = false;
    if (!config_path.empty()) {
        std::ifstream in(config_path, std::ios::binary);
        if (in) {
            const std::vector<u8> bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            valid = config::decode(globals, bytes.data(), u32(bytes.size()));
        } else {
            config::reset(globals);
        }
    }
    music.volume = globals.bgm_volume;
    sound.master_volume = globals.se_volume;
    save_config();
    return valid;
}

bool GameSession::save_config() {
    if (config_path.empty()) return false;
    u8 image[config::kSize];
    config::encode(globals, image);
    std::ofstream out(config_path, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(image), std::streamsize(config::kSize));
    out.close();
    if (!out) return false;
    if (on_saved) on_saved();
    return true;
}

// 0x44af50 析构 / 0x43ce10 切换：摘标题更新链，标记 title*.anm VM 删除（0x46f270），释放槽
bool GameSession::enter_ending(bool extra) {
    // Extra 通关：0x43ce10 case 0x10 直接回标题（DAT_004a6f1c = 3），不建结局对象
    if (extra) {
        leave_ending();
        globals.title_origin = 3;  // → TitleScene 画面 0xe 名字输入
        supervisor.request(SceneId::Title);
        supervisor.current = supervisor.requested = SceneId::Title;
        return enter_title_assets() && activate_title_menu();
    }
    leave_ending();
    // 0x4191f0：continues==0 → 偶下标；装 e0N.msg；成绩见过位；Loading VM ascii 0x11
    ending.animations = &animations;
    ending.text_paints = &text_into_texture;
    ending.load_ending_anm = [this](int slot, const char* name) -> AnmFile* {
        AnmResource* res = nullptr;
        if (!load_anm(name, res) || res == nullptr) return nullptr;
        return animations.load(slot, *res);
    };
    ending.spawn_ending_anm = [this](AnmFile* file, i32 script, i32 /*slot_index*/) -> u32 {
        if (file == nullptr) return 0;
        return animations.spawn(*file, script, -1);
    };
    ending.kill_ending_anm = [this](u32 vm) {
        if (vm != 0) animations.kill(vm);
    };
    ending.read_archive = [this](const char* name, std::vector<u8>& out) -> bool {
        return archive_.read(name, out);
    };
    ending.create_fade = [this](i32 kind, i32 duration, u32 color) {
        create_screen_fade(kind, duration, color);
    };
    ending.begin(globals.shot, globals.shot_aux, globals.continues, &scores, globals.difficulty);
    ending.music = &music;   // begin 重置了结局对象：乐曲 / 游戏速度指针在此重挂
    ending.game_speed = &supervisor.game_speed;
    {
        std::vector<u8> msg;
        if (archive_.read(ending.msg_name.c_str(), msg)) {
            ending.load_msg(msg.data(), u32(msg.size()), ending.msg_name.c_str());
        } else {
            ending.load_msg(nullptr, 0, ending.msg_name.c_str());
        }
    }
    spawn_loading_vm(kLoadingVmPos);  // 0x4191f0：AsciiManager+0x19248 脚本 0x11 @ (960,784) 若尚未存在
    ending.spawn_line_vms(animations, animations.file(0));  // text.anm 槽 0：0x4197c0
    ending_live = true;
    ending_update_chain_ = chains.create([this]() -> ChainResult {
        if (!ending_live) return ChainResult::Continue;
        if (ending.update(supervisor.input)) {
            // 0x4196c0 剧本结束请求场景 0x10 → 0x43ce10 case 0x10：0x4196a0 析构后 DAT_004a6f1c = 3
            leave_ending();
            globals.title_origin = 3;
            supervisor.request(SceneId::Title);
        }
        return ChainResult::Continue;
    }, 0x23, true);
    chains.enable(ending_update_chain_);
    // 绘制链 0x43（LAB_004197b0）：原版恒返回 1（空操作）；链登记对齐优先级
    if (ending_draw_chain_) { chains.remove(ending_draw_chain_); ending_draw_chain_ = nullptr; }
    ending_draw_chain_ = chains.create([this]() -> ChainResult {
        (void)this;
        return ChainResult::Continue;
    }, 0x43, true);
    chains.enable(ending_draw_chain_);
    supervisor.current = supervisor.requested = SceneId::Ending;
    return true;
}

void GameSession::leave_ending() {
    if (ending_update_chain_) { chains.remove(ending_update_chain_); ending_update_chain_ = nullptr; }
    if (ending_draw_chain_) { chains.remove(ending_draw_chain_); ending_draw_chain_ = nullptr; }
    ending.destroy_line_vms(animations);
    ending_live = false;
    ending.live = false;
    // 保留 ending_index / finished / events 供 harness 在回标题后断言
}

void GameSession::leave_title() {
    if (title_update_chain_) { chains.remove(title_update_chain_); title_update_chain_ = nullptr; }
    title_events.insert(title_events.end(), title.events.begin(), title.events.end());
    // 0x44ad20：析构时对落叶发射器（ascii.anm，+0x73c）interrupt 1，发射器与其子落叶在脚本 case 1 后自删
    animations.interrupt(title.vm[TitleScene::kLeafEmitter], 1);
    title.close_help();
    title = TitleScene{};
    animations.kill_file(0x10);
    animations.kill_file(0x11);
    animations.tick_world();
    animations.unload(0x10);
    animations.unload(0x11);
    supervisor.title_menu_live = false;
    supervisor.title_assets_ready = false;
}

bool GameSession::start_game_direct(i32 replay_mode) {
    if (!load_boot_banks()) return false;
    enable_ascii_chains();   // 原版由启动幕 0x43b290 打开；直接进游戏时在此补上
    supervisor.current = SceneId::Game;
    supervisor.requested = SceneId::Game;
    return enter_game(replay_mode);
}

void GameSession::apply_replay_start(i32 stage_choice) {
    if (!replay_file_.stages.empty()) {
        // 标题 Replay：按关卡选择下标取段；demo（stage_choice < 0）：首段（0x447ef0 首个非空段）
        i32 stage_i = 0;
        if (stage_choice >= 0) {
            for (i32 i = 0; i < i32(replay_file_.stages.size()); ++i) {
                if (replay_file_.stages[i].stage == stage_choice + 1) { stage_i = i; break; }
            }
        }
        globals.stage = replay_file_.stages[stage_i].stage;
        globals.stage_copy = globals.stage;
        bind_loaded_replay_();   // 按新关卡重选段与种子
    }
    if (replay_file_.user.size() >= 0xa0) {
        // 0x447760(1)：用户块 +0x84 / +0x88 / +0x9c / +0x8c → 机体 / 副机体 / 子季节 / 难度
        globals.shot = read_i32(replay_file_.user.data() + 0x84);
        globals.shot_aux = read_i32(replay_file_.user.data() + 0x88);
        globals.difficulty = read_i32(replay_file_.user.data() + 0x8c);
        globals.season = read_i32(replay_file_.user.data() + 0x9c);
    }
}

bool GameSession::create_player() {
    // 0x42cb60 新游戏段（DAT_004c17d0 != 0）
    // Extra 面（第 7 关）把难度抬到 4：HUD 难度牌 / 得点表 / 续关表都按 Extra 取
    if (globals.stage == 7 && globals.difficulty < 4) globals.difficulty = 4;
    // 残机：模式 0 → 2；符卡练习 → 0（炸弹亦 0）；练习 → DAT_004a5bf8 ? 其值-1 : 9
    const i32 mode = globals.mode();
    // 得分 / 擦弹清零；得点值初值与上限按难度表（DAT_004922a8 / DAT_00492290 × 100；Extra 为 100000 / 1000000）
    static constexpr i32 kPointInitial[6] = {10000, 10000, 10000, 10000, 10000, 100000};
    static constexpr i32 kPointMax[6] = {500000, 500000, 500000, 500000, 500000, 1000000};
    const int diff = globals.difficulty < 0 ? 0 : globals.difficulty > 5 ? 5 : globals.difficulty;
    // 续关次数只在非 Extra 续关（DAT_004a5bec bit3 未置）时清零；0x42e590 清本局资源
    if ((globals.flags & 8u) == 0) globals.continues = 0;
    globals.graze = 0;
    globals.score = 0;
    globals.reset_run_resources();
    globals.point_value_initial = kPointInitial[diff] * 100;
    globals.point_value_max = kPointMax[diff] * 100;
    globals.point_value = i32((double(float(globals.point_value_initial)) / 100.0) * 100.0);
    if (mode == 2) {
        globals.lives = 0;
        globals.bombs = 0;
        // 0x42cb60：符卡练习最高分取成绩块 +0x970 + replay_slot×0x9c（SpellRecord::high_score），续关位清零
        const int spell = globals.replay_slot;
        if (spell >= 0 && spell < ScoreFile::kSpellRecords)
            globals.hiscore = i32(scores.blocks[globals.shot].spells[spell].high_score);
        globals.hiscore_continues = 0;
    }
    else if (mode == 0) globals.lives = 2;
    else globals.lives = globals.practice_lives == 0 ? 9 : globals.practice_lives - 1;
    // 剩余续关数（DAT_00492278 按难度：Extra 无续关）
    static constexpr i32 kContinues[6] = {5, 5, 5, 5, 0, 0};
    globals.continues_left = kContinues[diff];

    // 0x440fb0：pl0N.anm → 槽 9（DAT_00492ccc[shot+aux]）、pl0Nsub.anm → 槽 0x1e（DAT_00492cdc[season]）、.sht 两份
    static constexpr const char* kPlayerAnm[4] = {"pl00.anm", "pl02.anm", "pl03.anm", "pl01.anm"};
    static constexpr const char* kPlayerSubAnm[5] = {"pl00sub.anm", "pl02sub.anm", "pl03sub.anm", "pl01sub.anm", "pl04sub.anm"};
    static constexpr const char* kPlayerSht[4] = {"pl00.sht", "pl02.sht", "pl03.sht", "pl01.sht"};
    static constexpr const char* kPlayerSubSht[5] = {"pl00sub.sht", "pl02sub.sht", "pl03sub.sht", "pl01sub.sht", "pl04sub.sht"};
    const int shot = globals.shot + globals.shot_aux;
    if (shot < 0 || shot > 3 || globals.season < 0 || globals.season > 4) { failure = "player shot/season out of range"; return false; }
    AnmResource* pl_res = nullptr;
    AnmResource* sub_res = nullptr;
    if (!load_anm(kPlayerAnm[shot], pl_res)) return false;
    if (!load_anm(kPlayerSubAnm[globals.season], sub_res)) return false;
    AnmFile* pl_file = animations.load(9, *pl_res);
    AnmFile* sub_file = animations.load(0x1e, *sub_res);
    if (pl_file == nullptr || sub_file == nullptr) { failure = "player anm load failed"; return false; }
    std::vector<u8> sht_bytes, sub_sht_bytes;
    if (!archive_.read(kPlayerSht[shot], sht_bytes)) { failure = archive_.error; return false; }
    if (!archive_.read(kPlayerSubSht[globals.season], sub_sht_bytes)) { failure = archive_.error; return false; }
    if (!game.player.init(animations, globals, pl_file, sub_file, sht_bytes.data(), u32(sht_bytes.size()),
                          sub_sht_bytes.data(), u32(sub_sht_bytes.size()))) { failure = game.player.error; return false; }
    game.player_live = true;

    // 火力 / 季节量表初值
    const i32 per = globals.power_per_level, max = globals.power_max;
    auto clamp_full = [&]() {
        globals.power = per * 4;
        if (globals.power <= max) { if (globals.power < per) globals.power = per; }
        else globals.power = max;
    };
    if (mode == 2) {
        clamp_full();
        globals.season_gauge = globals.season_gauge_max < 0x186 ? globals.season_gauge_max : 0x186;
    } else if (globals.stage < 2) {
        globals.power = per > max ? max : per;
        globals.season_gauge = globals.season_gauge_max < 0 ? globals.season_gauge_max : 0;
    } else {
        clamp_full();
        globals.season_gauge = globals.season_gauge_max < 0 ? globals.season_gauge_max : 0;
    }
    game.player.setup_options(animations, globals);   // 0x4440e0
    globals.flags &= ~0x4u;

    // 0x447e20 / 0x447760：回放时以本关段头 +0x14 快照覆盖刚初始化的全局，再按回放槽号定模式（0x449190）；
    // 子机要到首帧 0x448eb0 才按新火力重排。录制时（新开局）在此刻抄快照
    if (game.mode == 1) {
        if (const ReplayStageSegment* seg = current_replay_segment_()) {
            load_replay_globals(globals, seg->header.data() + kReplayGlobalsOffset);
            globals.set_mode(globals.replay_slot < 0 ? 0 : 2);
        }
    } else {
        replay_recording_.snapshot_globals(globals, true);
    }
    // ReplayManager 登记三条链——记录 0x10、回放快进 0x22、FPS 文字 0x47
    replay_ff_counter_ = 0;
    replay_fps_byte_ = 60;
    replay_record_chain_ = chains.create([this]() -> ChainResult {
        // 回放 0x448e30→0x448130：从段缓冲取 6 字节；记录 0x448e20→0x447fd0：抄 held。
        if (game.mode == 1) {
            u32 held = 0, aux0 = 0, aux1 = 0;
            u8 fps = replay_fps_byte_;
            if (replay_file_loaded_) {
                const bool stepped = replay_playback_step(replay_file_, replay_cursor_, held, aux0, aux1, fps);
                replay_fps_byte_ = fps;
                if (stepped) {
                    game_input.held = held & 0xffffu;
                    // 原版另写 DAT_004a52d4 / DAT_004a52d8（aux）；边沿仍走 commit
                    game_input.commit();
                } else {
                    // 0x448130 段尾 / 终止帧：DAT_004a52c8 / d4 / d8 清零后直接返回（不走 0x418650）
                    game_input.previous = game_input.held;
                    game_input.held = game_input.pressed = game_input.released = 0;
                    if (replay_cursor_.terminated) {
                        // 0xffff×3：0x43f240 回标题菜单，+0x214 = -1 后不再读段
                        replay_cursor_.terminated = false;
                        if (menu_live) menu.open_return_to_title();
                    }
                }
            }
            ++replay_ff_counter_;
            return ChainResult::Continue;
        }
        // 0x447fd0：DAT_004a52c8 ← DAT_004a50b0；自动低速；边沿；每 30 帧 FPS；写 6 字节
        game_input.held = supervisor.input.held & 0xffffu;
        if ((globals.input_config & 0x200u) != 0 && (game_input.held & InputState::kShot) != 0 &&
            globals.power >= 10) {
            game_input.held |= InputState::kSlow;
        }
        game_input.commit();
        if (replay_ff_counter_ >= 0) {
            if ((replay_ff_counter_ % 30) == 0) {
                const float rounded = global_fps_ + 0.5f;
                replay_fps_byte_ = rounded >= 256.0f ? 255 : u8(i32(rounded));
            }
            // 换关期间（0x449030 → 0x448eb0）Replay 链照常运行，帧仍记进上一段；新开局首段要到 0x448eb0 才开
            if (game_live) {
                replay_recording_.tick(u16(game_input.held & 0xffffu), u16(game_input.pressed & 0xffffu),
                                       u16(game_input.released & 0xffffu), replay_fps_byte_);
            }
            ++replay_ff_counter_;
        }
        return ChainResult::Continue;
    }, 0x10, true);
    replay_playback_chain_ = chains.create([this]() -> ChainResult {
        // 0x448e40：只读 +0x20c；Restart 时整条更新链从头再跑（含 0x10），约 8× 逻辑帧
        return replay_fast_forward_result(game_live, (game.flags & GameManager::kLoading) != 0,
                                         game.mode, supervisor.input.held, replay_ff_counter_);
    }, 0x22, true);
    replay_draw_chain_ = chains.create([this]() -> ChainResult {
        // 0x448e90：装载中（flags&4）直接 Continue；否则 0x4482f0
        if (!game_live || (game.flags & GameManager::kLoading) != 0) return ChainResult::Continue;
        if (game.mode != 1) return ChainResult::Continue;
        ascii.style.color = replay_fps_color(replay_fps_byte_);
        ascii.printf(Vec3{383.0f, 450.0f, 0.0f}, "%3d", i32(replay_fps_byte_));
        ascii.style.color = 0xffffffffu;
        return ChainResult::Continue;
    }, 0x47, false);
    // 0x440fb0：自机更新链 0x16（0x443720 → 0x442560）与绘制链 0x1d（0x443730）
    // 0x442560：回放中（ReplayManager +0xc == 1）残机耗尽不进续关画面，自机照常复活
    game.player.on_game_over = [this]() { if (menu_live && game.mode != 1) menu.open_continue(); };
    game.player.sfx_sink = &sfx_events;
    player_update_chain_ = chains.create([this]() -> ChainResult {
        game.player.update(animations, globals, game_input, game.world, animations.file(8), geometry, supervisor.game_speed);
        harvest_bomb_events();   // 状态 1 / 决死中发动的炸弹（震屏 / HUD 事件）
        // 入场态 0x416d20 / 0x431a70 的消弹音效（0x47）与炸弹音效已直接进入 sfx_events，自机自身的音效随后并入
        harvest_player_sfx();
        harvest_menu_events();   // 0x43f350 在自机更新内打开续关画面时登记的音效
        return ChainResult::Continue;
    }, 0x16, true);
    player_draw_chain_ = chains.create([this]() -> ChainResult {
        game.player.draw(renderer);
        return ChainResult::Continue;
    }, 0x1d, false);
    chains.enable(replay_record_chain_);
    chains.enable(replay_playback_chain_);
    chains.enable(replay_draw_chain_);
    chains.enable(player_update_chain_);
    chains.enable(player_draw_chain_);
    return true;
}

bool GameSession::create_stage() {
    // 0x409db0 / 0x4097c0 / 0x40ac30：STD → 背景 ANM 进槽 3 + (stage & 1)；更新链 0x11（0x4097c0 建、0x40add0 启用）
    auto stage = std::make_unique<StageManager>();
    std::vector<u8> std_bytes;
    if (!archive_.read(game.entry->std_name, std_bytes)) { failure = archive_.error; return false; }
    if (!stage->load(std_bytes.data(), u32(std_bytes.size()), globals.stage, stage_camera)) { failure = stage->error; return false; }
    AnmResource* stage_anm = nullptr;
    if (!load_anm(stage->std.anm_name.c_str(), stage_anm)) return false;
    const int slot = 3 + (globals.stage & 1);
    // 0x46d020：槽已装着同一文件（快速重开未释放）时直接沿用
    AnmFile* loaded = animations.file(slot);
    stage->anm = (loaded != nullptr && loaded->resource == stage_anm) ? loaded : animations.load(slot, *stage_anm);
    if (stage->anm == nullptr) { failure = "stage anm load failed"; return false; }
    stage->camera_global = &stage_camera;
    stage->screen_clear_color = &compositor.background_color;
    stage->render_targets_ready = &compositor.render_targets_ready;
    stage->spellcard_flags = &spellcard.flags;
    stage->on_screen_fade = [this](i32 kind, i32 duration, u32 color) { create_screen_fade(kind, duration, color); };
    game.stage = std::move(stage);
    game.stage_live = true;
    game.enemies.stage = game.stage.get();
    animations.camera_pos = &stage_camera.pos;
    animations.camera_pos_offset = &stage_camera.pos_offset;
    animations.camera_facing_norm = &stage_camera.facing_norm;
    register_stage_chains(*game.stage);
    return true;
}

void GameSession::register_stage_chains(StageManager& stage) {
    // 0x4097c0：更新链 0x11 与绘制链 3 / 6 建为禁用，0x40add0（on_stage_started）启用。关卡对象由 unique_ptr 持有，
    // 摘链总在析构之前（remove_stage_chains），回调持裸指针即可
    StageManager* target = &stage;
    stage.update_chain = chains.create([this, target]() -> ChainResult {
        target->update(animations, supervisor.game_speed);
        return ChainResult::Continue;
    }, 0x11, true);
    stage.background_draw_chain = chains.create([this, target]() -> ChainResult {
        target->draw_background(animations, renderer, stage_camera, screen_shake);
        return ChainResult::Continue;
    }, 3, false);
    stage.foreground_draw_chain = chains.create([this, target]() -> ChainResult {
        target->draw_foreground(animations, renderer, stage_camera, screen_shake, supervisor.game_speed);
        return ChainResult::Continue;
    }, 6, false);
}

void GameSession::remove_stage_chains(StageManager& stage) {
    for (ChainEntry** entry : {&stage.update_chain, &stage.background_draw_chain, &stage.foreground_draw_chain}) {
        if (*entry != nullptr) chains.remove(*entry);
        *entry = nullptr;
    }
}

void GameSession::enable_stage_chains(StageManager& stage) {
    for (ChainEntry* entry : {stage.update_chain, stage.background_draw_chain, stage.foreground_draw_chain})
        if (entry != nullptr) chains.enable(entry);
}

void GameSession::unload_stage_anm(StageManager& stage) {
    // 0x4099a0 末：DAT_004a5bec bit0 清时释放背景 ANM 槽（0x46d770 标删该槽 VM 后释放；回收在下一次世界 tick）
    if ((globals.flags & 1u) != 0) return;
    const int slot = 3 + (stage.stage_number & 1);
    animations.kill_file(slot);
    animations.unload(slot);
}

bool GameSession::create_enemies() {
    // 0x41b340 / 0x41ae70：装 ECL，登记敌人更新链优先级 0x1a（0x41b4f0→0x41b3d0）
    auto read_ecl = [](void* user, const std::string& name, std::vector<u8>& data) -> bool {
        return static_cast<Archive*>(user)->read(name.c_str(), data);
    };
    auto load_enemy_anm = [](void* user, u32 slot, const std::string& name) -> bool {
        auto* self = static_cast<GameSession*>(user);
        AnmResource* res = nullptr;
        if (!self->load_anm(name.c_str(), res) || res == nullptr) return false;
        return self->animations.load(int(slot), *res) != nullptr;
    };
    game.enemies.read_user = &archive_;
    game.enemies.read_fn = read_ecl;
    game.enemies.anm_user = this;
    game.enemies.anm_fn = load_enemy_anm;
    game.enemies.bullets = game.bullets.get();
    game.enemies.items = game.items.get();
    game.enemies.lasers = game.lasers.get();
    game.enemies.effects = &effects;
    game.enemies.spellcard = &spellcard;
    game.enemies.stage = game.stage.get();
    game.enemies.render_targets_ready = &compositor.render_targets_ready;
    game.player.spellcard = &spellcard;
    if (game.enemies_live) {
        // 同一关重开（DAT_004a5bec bit0 / bit3）：0x42cb60 不重建 EnemyManager，只走 0x42ca00 复位管理器计时；
        // 敌机 id 计数、ECL 全局变量、HUD 隐藏位等沿用上一局
        game.enemies.player = game.player_live ? &game.player : nullptr;
        game.enemies.restart_timer();
    } else {
        if (!game.enemies.create(game.entry->ecl_name, globals, game.player_live ? &game.player : nullptr,
                                 animations.visual_rng, animations, u8(1u << (globals.difficulty & 31)))) {
            failure = game.enemies.error;
            return false;
        }
        game.enemies_live = true;
    }
    game.player.enemies = &game.enemies;
    game.player.items = game.items.get();
    game.player.effects = &effects;
    game.player.bullet_manager = game.bullets.get();
    game.player.global_game_speed = &supervisor.game_speed;
    game.enemies.global_game_speed = &supervisor.game_speed;
    game.enemies.gui = &game.gui;
    game.enemies.score_popups = game.score_popups_live ? &game.score_popups : nullptr;
    game.enemies.world = &game.world;
    if (enemy_update_chain_ == nullptr) {
        enemy_update_chain_ = chains.create([this]() -> ChainResult {
            if (game.enemies_live && !game.chains_suspended) {
                game.enemies.shake_requests.clear();
                game.enemies.sfx_sink = &sfx_events;   // 敌机受击 / 击破与 ECL 601 发射音效按 0x45e1f0 调用序直接汇总
                sync_bomb_state();
                game.enemies.update(supervisor.game_speed);
                if (sync_enemy_count) game.world.enemy_count = game.enemies.enemy_count;
                for (const auto& request : game.enemies.shake_requests) create_screen_shake(request);
                game.enemies.shake_requests.clear();
                if (!game.enemies.error.empty()) {
                    failure = game.enemies.error;
                    return ChainResult::BreakExit;
                }
            }
            return ChainResult::Continue;
        }, 0x1a, true);
    }
    return true;
}

bool GameSession::load_gui_stage_assets(AnmFile*& logo, std::vector<u8>& msg_bytes) {
    // 0x426c10：关卡 logo ANM → 槽 6（0x46d020：槽已占用时沿用）、按机体的 msg 文件
    logo = animations.file(Gui::kLogoSlot);
    if (logo == nullptr) {
        AnmResource* logo_res = nullptr;
        if (!load_anm(game.entry->logo_anm, logo_res)) return false;
        logo = animations.load(Gui::kLogoSlot, *logo_res);
        if (logo == nullptr) { failure = "stage logo anm load failed"; return false; }
    }
    const char* msg_name = game.entry->msg_names[(globals.shot + globals.shot_aux) & 3];
    if (!archive_.read(msg_name, msg_bytes)) { failure = archive_.error; return false; }
    return true;
}

void GameSession::bind_spellcard() {
    // 0x4178a0 / 0x417700：Spellcard（ECL 之后、Bomb 之后创建）；更新链 0x1e（0x417ee0 → 0x417930）、绘制链 0xc
    spellcard.reset();
    spellcard.animations = &animations;
    spellcard.enemies = &game.enemies;
    spellcard.player = game.player_live ? &game.player : nullptr;
    spellcard.globals = &globals;
    spellcard.stage = game.stage.get();
    spellcard.entry = game.entry;
    spellcard.sfx_sink = &sfx_events;
    spellcard.scores = &scores;
    spellcard.replay_playback = game.mode == 1;   // ReplayManager+0xc 由 0x447e20(GameManager+0xac) 赋值
    spellcard.text_paints = &text_into_texture;
    spellcard.record_time = [this](i32 index, i32 value) { replay_recording_.set_spell_time(index, value); };
    spellcard.replay_time = [this](i32 index) { return replay_spell_time_(index); };
    game.gui.spellcard_flags = &spellcard.flags;
    game.gui.spellcard = &spellcard;
    if (spellcard_update_chain_ == nullptr) {
        spellcard_update_chain_ = chains.create([this]() -> ChainResult {
            if (game.chains_suspended) return ChainResult::Continue;
            spellcard.update(supervisor.game_speed);
            return ChainResult::Continue;
        }, 0x1e, true);
    }
    if (spellcard_draw_chain_ == nullptr) {
        spellcard_draw_chain_ = chains.create([this]() {
            spellcard.draw_text(ascii);
            return ChainResult::Continue;
        }, 0x0c, false);
    }
}

void GameSession::register_game_chains() {
    // 0x42cb60 末：GameManager 更新链（优先级 0xf）；ScreenEffect / 上一关析构回调
    game_update_chain_ = chains.create([this]() -> ChainResult {
        return game.update(animations, globals, supervisor.game_speed) ? ChainResult::Continue : ChainResult::BreakExit;
    }, 0xf, true);
    game.game_input = &game_input;
    game.system_input = &supervisor.input;
    game.config_flags = &config_flags;
    game.scores = &scores;
    game.on_screen_fade = [this](i32 kind, i32 duration, u32 color) { create_screen_fade(kind, duration, color); };
    game.on_return_to_title = [this]() { if (menu_live) menu.open_return_to_title(); };
    game.on_frame_advanced = [this]() { compositor.tick(animations); };
    game.music = &music;
    game.on_transition_finished = [this]() { drain_sound(); };
    game.on_stage_started = [this]() {
        if (game.stage) enable_stage_chains(*game.stage);
        // 0x42dc50 / 0x42dee0 接着启用 GameMenu 绘制链与得分飘字的两条链
        if (menu_draw_chain_) chains.enable(menu_draw_chain_);
        if (popup_update_chain_) chains.enable(popup_update_chain_);
        if (popup_draw_chain_) chains.enable(popup_draw_chain_);
    };
    game.on_replay_stage_start = [this]() { replay_stage_start_(); };
    game.on_previous_stage_destroyed = [this](StageManager& stage) {
        remove_stage_chains(stage);
        unload_stage_anm(stage);
    };
}

void GameSession::mark_stage_reached() {
    // 0x42cb60 开头：GameManager+0xac == 0 的装载即置该关练习到达位（+0x519d）
    if (game.mode != 0) return;
    scores.mark_practice_reached(globals.shot + globals.shot_aux, globals.difficulty, globals.stage);
}

bool GameSession::enter_game(i32 replay_mode) {
    // 0x42d700：flags |= 4、+0xac = 回放标志；0x42cb60 装载线程（此处同步）。GameManager 对象已由 leave_game 复位
    //（快速重开时其中保留 EnemyManager 与 Loading VM）
    game.flags = GameManager::kLoading;
    game.mode = replay_mode;
    game.entry = &stage_entry(globals.stage);
    game_input = InputState{};
    // 各管理器链在 0x42cb60 中建为禁用，首帧 0x42d7b0 → 0x42dee0 统一启用；排在 GameManager（0xf）之前的 GameMenu 链 10
    // 因此从第 2 帧起才推进
    game.chains_suspended = true;
    // 0x42cb60 → 0x447e20 → 0x447760（Replay 构造）：重置 DAT_004a6d8c 调用计数；回放时 DAT_004a6d88 := 段头 +2
    // （DAT_004a6d80 不动：新开局不走 0x449030）
    if (replay_mode == 1) {
        if (const ReplayStageSegment* seg = current_replay_segment_()) animations.visual_rng.seed = seg->rng_seed;
    }
    animations.visual_rng.calls = 0;
    replay_next_stage_index_ = -1;
    replay_stage_seed_ = u16(animations.visual_rng.seed & 0xffffu);
    if (replay_mode != 1) {
        const i32 time32 = i32(std::time(nullptr));
        menu.ctx.replay_timestamp = time32;
        replay_recording_.begin_run(globals, time32);
    }

    mark_stage_reached();
    restart_play_clock();   // 0x42cb60 末：非回放重置 DAT_004a5c00
    // 0x42cb60 新游戏段末：非回放时本机体游玩次数 +1（+0x515c）
    if (game.mode == 0) scores.record_play(globals.shot + globals.shot_aux);
    // 0x42cb60：新游戏（DAT_004c17d0，0x43ce10 case 7 置位）先建 Player，再建 Gui / Stage / …
    globals.new_game = true;
    if (!create_player()) return false;
    if (!create_stage()) return false;
    // 0x42cb60：非演示时 0x43c440 停曲（符卡练习除外），再以 0x43c370 把关卡曲 / Boss 曲登记到槽 0 / 1
    if ((globals.flags & 0x40u) == 0) {
        if ((globals.flags & 0x30u) != 0x20u) music.stop_track();
        music.register_track(0, game.entry->bgm_name);
        music.register_track(1, game.entry->bgm_boss_name);
    }

    // 0x411dd0：BulletManager（bullet.anm 槽 7、effect.anm 槽 8）；更新链 0x1c（0x411880）、绘制链 0x25（0x411a30）
    {
        AnmFile* bullet_anm = animations.file(BulletManager::kBulletAnmSlot);
        if (bullet_anm == nullptr) { failure = "bullet.anm not loaded"; return false; }
        game.bullets = std::make_unique<BulletManager>(supervisor.game_speed);
        game.bullets->create(animations, *bullet_anm, animations.file(8), globals,
                             game.player_live ? &game.player : nullptr, &game.world, animations.visual_rng);
        game.bullets->sfx_sink = &sfx_events;
        game.bullets_live = true;
        bullet_update_chain_ = chains.create([this]() -> ChainResult {
            // 0x411880：GameManager 装载中（bit2）或结束位（bit0 经 >>2 映射）时跳过
            if (game.bullets_live && !game.chains_suspended && ((game.flags | (game.flags >> 2)) & 1u) == 0) {
                sync_bomb_state();
                game.bullets->update(supervisor.game_speed, (game.flags & 0x400u) != 0);
                // 被弹（0x443f10）在弹幕链内登记的自机音效本帧即生效
                if (game.player_live) harvest_player_sfx();
            }
            return ChainResult::Continue;
        }, 0x1c, true);
        bullet_draw_chain_ = chains.create([this]() -> ChainResult {
            if (game.bullets_live && !game.chains_suspended && (game.flags & GameManager::kLoading) == 0) game.bullets->draw(renderer);
            return ChainResult::Continue;
        }, 0x25, false);
    }

    // 0x42f440：ItemManager；更新链 0x1d（0x4308f0 → 0x42f4e0）、绘制链 0x21 / 0x13（0x4307a0）
    {
        game.items = std::make_unique<ItemManager>();
        game.items->create(animations, animations.file(BulletManager::kBulletAnmSlot), animations.file(8), globals,
                           game.player_live ? &game.player : nullptr, animations.game_rng);
        game.items->sfx_sink = &sfx_events;
        game.items_live = true;
        game.bullets->items = game.items.get();
        item_update_chain_ = chains.create([this]() -> ChainResult {
            // 0x4308f0：GameManager 装载中（bit2）/ 结束位（bit0 经 >>2 映射）/ 0x400 冻结时跳过
            if (game.items_live && !game.chains_suspended && ((game.flags | (game.flags >> 2)) & 1u) == 0 &&
                (game.flags & 0x400u) == 0) {
                game.items->dialogue = game.world.dialogue;
                game.items->focus_held = (game_input.held & 0x8u) != 0;
                sync_bomb_state();
                game.items->update(supervisor.game_speed);
            }
            return ChainResult::Continue;
        }, 0x1d, true);
        item_draw_chain_ = chains.create([this]() -> ChainResult {
            if (game.items_live && !game.chains_suspended && (game.flags & GameManager::kLoading) == 0) game.items->draw(renderer, false);
            return ChainResult::Continue;
        }, 0x21, false);
        item_draw_season_chain_ = chains.create([this]() -> ChainResult {
            if (game.items_live && !game.chains_suspended && (game.flags & GameManager::kLoading) == 0) game.items->draw(renderer, true);
            return ChainResult::Continue;
        }, 0x13, false);
    }

    // 0x4314a0：LaserManager（bullet.anm 槽 7、effect.anm 槽 8；sprite_mapper 2 / 3）；更新链 0x1b（0x4316b0）、绘制链 0x23（0x431720）
    {
        game.lasers = std::make_unique<LaserManager>();
        game.lasers->create(animations, *animations.file(BulletManager::kBulletAnmSlot), animations.file(8), *game.bullets,
                            game.player_live ? &game.player : nullptr, &game.enemies, &effects, &game.world);
        game.lasers->sfx_sink = &sfx_events;
        game.lasers_live = true;
        game.player.laser_manager = game.lasers.get();
        // Ex 0x8000000 在 0x413860 内当场建激光（同帧挂入激光链尾，下一帧起推进）
        game.bullets->on_laser_request = [this](const BulletLaserRequest& req) {
            if (game.lasers_live) game.lasers->spawn_from_bullet(req);
        };
        laser_update_chain_ = chains.create([this]() -> ChainResult {
            // 0x4316b0：GameManager 装载中（bit2）/ 结束位（bit0 经 >>2 映射）/ 0x400 冻结时跳过；bit1 时以速度 0 推进
            if (game.lasers_live && !game.chains_suspended && ((game.flags | (game.flags >> 2)) & 1u) == 0 &&
                (game.flags & 0x400u) == 0) {
                game.lasers->update((game.flags & 0x2u) != 0 ? 0.0f : supervisor.game_speed);
                // 被弹（0x443f10）在激光链内登记的自机音效本帧即生效
                if (game.player_live) harvest_player_sfx();
            }
            return ChainResult::Continue;
        }, 0x1b, true);
        laser_draw_chain_ = chains.create([this]() -> ChainResult {
            if (game.lasers_live && !game.chains_suspended && (game.flags & GameManager::kLoading) == 0) game.lasers->draw(renderer);
            return ChainResult::Continue;
        }, 0x23, false);
    }

    create_menu();
    // 0x449e50（GameMenu 之后）：得分飘字；两条链建为禁用，关卡首帧 0x42dc50 / 0x42dee0 启用（on_stage_started）
    {
        AnmFile* ascii_file = animations.file(AnmManager::kAsciiSlot);
        if (ascii_file == nullptr) { failure = "ascii.anm not loaded"; return false; }
        game.score_popups.create(*ascii_file);
        game.score_popups.player = game.player_live ? &game.player : nullptr;
        game.score_popups_live = true;
        if (game.items) game.items->score_popups = &game.score_popups;
        game.player.score_popups = &game.score_popups;
        popup_update_chain_ = chains.create([this]() -> ChainResult {
            if (game.score_popups_live) game.score_popups.update(supervisor.game_speed);
            return ChainResult::Continue;
        }, 0x14, true);
        popup_draw_chain_ = chains.create([this]() -> ChainResult {
            if (game.score_popups_live) game.score_popups.draw(renderer);
            return ChainResult::Continue;
        }, 0x2f, false);
    }
    if (!create_enemies()) return false;
    create_bombs();

    // 0x427ca0 / 0x426b00 / 0x426c10：Gui——front.anm（槽 5）、关卡 logo ANM → 槽 6、按机体的 msg 文件；
    // 更新链 0x20（0x429af0 → 0x427cf0）。原版在 Bullet 之前创建，这里放到 ECL 之后以便直接绑定各管理器指针
    {
        AnmFile* logo = nullptr;
        std::vector<u8> msg_bytes;
        if (!load_gui_stage_assets(logo, msg_bytes)) return false;
        game.gui.enemies = &game.enemies;
        game.gui.bullets = game.bullets.get();
        game.gui.lasers = game.lasers.get();
        game.gui.player = game.player_live ? &game.player : nullptr;
        game.gui.world = &game.world;
        game.gui.globals = &globals;
        game.gui.input = &game_input;
        game.gui.geometry = &geometry;
        renderer.draw_hooks = &game.gui;   // 绘制钩子 5（对话文字跟随）
        game.gui.replay_playback = game.mode != 0;
        game.gui.spellcard_flags = &spellcard.flags;
        game.gui.game_flags = &game.flags;
        game.gui.bomb = game.bombs.main();
        game.gui.season_release = game.bombs.release();
        game.gui.scores = &scores;
        game.gui.music = &music;
        // 各子系统反向持有 Gui：残机 / 炸弹格刷新、弹出通知、extend 都在原版调用点直接发生
        game.player.gui = &game.gui;
        if (game.items) game.items->gui = &game.gui;
        spellcard.gui = &game.gui;
        game.gui.on_continue_screen = [this]() { if (menu_live) menu.open_continue(); };
        game.gui.on_practice_result = [this]() { if (menu_live) menu.open_practice_result(); };
        game.gui.on_return_to_title = [this]() { if (menu_live) menu.open_return_to_title(); };
        game.gui.on_final_clear = [this]() {
            game.flags |= GameManager::kStageClear;
            // 0x442560 的射击条件直接读 GameManager+0x88 的 0x4000：通关结算起自机不再射击
            game.world.stage_clear = true;
            game.end_counter = 0;
            accumulate_play_time();
        };
        game.gui.sfx_sink = &sfx_events;
        game.gui.text_paints = &text_into_texture;
        if (!game.gui.init(animations, animations.file(5), logo, *game.entry, std::move(msg_bytes))) {
            failure = game.gui.error;
            return false;
        }
        game.gui_live = true;
        gui_update_chain_ = chains.create([this]() -> ChainResult {
            if (game.gui_live) {
                game.gui.update(supervisor.game_speed);
                harvest_menu_events();   // 对话 -2 / 练习过关在 Gui 更新内打开菜单时登记的音效
                if (game.gui.dialogue) game.gui.dialogue->text_requests.clear();  // 文字贴图：渲染接入前无状态影响
                if (!game.gui.error.empty()) {
                    failure = game.gui.error;
                    return ChainResult::BreakExit;
                }
            }
            return ChainResult::Continue;
        }, 0x20, true);
        chains.enable(gui_update_chain_);
        // 0x426b00：两条绘制链建为禁用，0x426d70（关卡首帧 init_hud）启用。0x33 的回调只返回 1；0x30 画 HUD 文字
        gui_draw_chains_[0] = chains.create([]() { return ChainResult::Continue; }, 0x33, false);
        gui_draw_chains_[1] = chains.create([this]() {
            if (game.gui_live) game.gui.draw_hud_text(ascii);
            return ChainResult::Continue;
        }, 0x30, false);
        game.gui.on_hud_init = [this]() { for (ChainEntry* entry : gui_draw_chains_) chains.enable(entry); };
    }

    bind_spellcard();
    chains.enable(spellcard_update_chain_);
    chains.enable(spellcard_draw_chain_);
    register_game_chains();
    // 0x42cb60 末：+0x90 = 0x3c、清 flags&4、DAT_004a5bec &= 0xfffffff4（只清 bit0 / bit1 / bit3，模式位 4-5 保留）、0x418420 启用链
    game.transition_length = 0x3c;
    game.flags &= ~GameManager::kLoading;
    globals.flags &= ~0xbu;
    game.timer.set(0);
    chains.enable(game_update_chain_);
    // 关卡链（0x11）要等首帧 0x42dc50 → 0x40add0 才启用（on_stage_started）
    if (bullet_update_chain_) chains.enable(bullet_update_chain_);
    if (bullet_draw_chain_) chains.enable(bullet_draw_chain_);
    if (item_update_chain_) chains.enable(item_update_chain_);
    if (item_draw_chain_) chains.enable(item_draw_chain_);
    if (item_draw_season_chain_) chains.enable(item_draw_season_chain_);
    if (laser_update_chain_) chains.enable(laser_update_chain_);
    if (laser_draw_chain_) chains.enable(laser_draw_chain_);
    if (enemy_update_chain_) chains.enable(enemy_update_chain_);
    game_live = true;
    return true;
}

bool GameSession::restart_game(SceneId reason, i32 replay_mode) {
    // 0x42d200 开头按场景请求决定重开方式：
    //   0xa / 0xb（暂停菜单重开）：Loading VM（ascii.anm 0x11）放到 (960, 784)；与开局同一关 → bit0 快速重开
    //   0xe（Extra 续关）：0x41a390 Loading VM（XMM1 = 480 / XMM2 = 392 寄存器传参，坐标加倍 = (960, 784)）；
    //   关卡不同 → 续关次数 +1（上限 9）并置 bit3；无论如何置 bit0
    //   0x13（符卡练习重开）：整体销毁重建
    // bit0 / bit3 之下 EnemyManager 只复位不析构、关卡背景 ANM 槽不释放（0x4099a0 末）
    const bool same_stage = globals.stage_copy == globals.stage;
    if (reason == SceneId::Restart || reason == SceneId::RestartPractice) {
        spawn_loading_vm(kLoadingVmPos);
        if (same_stage) globals.flags |= 1u;
    } else if (reason == SceneId::RestartExtra) {
        spawn_loading_vm(kLoadingVmPos);
        if (!same_stage) {
            if (++globals.continues > 9) globals.continues = 9;
            globals.flags |= 8u;
        }
        globals.flags |= 1u;
    }
    leave_game();
    globals.stage = globals.stage_copy;
    return enter_game(replay_mode);
}

void GameSession::spawn_loading_vm(const Vec3& pos) {
    // 0x41a390 / 0x42d200：AsciiManager 的 "Loading" VM（ascii.anm 0x11，层 -1）只在不存在时创建
    if (game.loading_vm != 0) return;
    if (AnmFile* ascii = animations.file(AnmManager::kAsciiSlot))
        game.loading_vm = animations.spawn_effect(*ascii, 0x11, -1, pos, 0.0f);
}

bool GameSession::next_stage() {
    // 0x43ce10 case 0xc：DAT_004c17d0（新开局）= 0；当前场景 7 → 0x42d780 销毁；请求改回 7；0x42d700(模式) 重建
    globals.new_game = false;
    // ---- 0x42d780 → 0x42d200（DAT_004a5bec bit1 路径）----
    save_scores();   // 0x449a00
    globals.flags &= ~3u;
    supervisor.game_speed = 1.0f;
    // 0x41a390：AsciiManager "Loading" VM；0x42d274 以 XMM1 = 480 / XMM2 = 392 传参（Ghidra 漏掉），函数内加倍 = (960, 784)
    spawn_loading_vm(kLoadingVmPos);
    globals.flags |= 2u;
    // 0x427970：Gui 删 Msg、释放 msg；(bec & 9) == 0 → 卸 logo 槽 6
    if (game.gui_live) {
        game.gui.end_stage();
        if ((globals.flags & 9u) == 0) {
            animations.kill_file(Gui::kLogoSlot);
            animations.unload(Gui::kLogoSlot);
        }
    }
    // 上一关若仍存活（连续两次换关）先析构；当前关转入 DAT_004a6d9c，链 0x11 继续运行
    if (game.previous_stage) {
        remove_stage_chains(*game.previous_stage);
        unload_stage_anm(*game.previous_stage);
        game.previous_stage.reset();
    }
    game.previous_stage = std::move(game.stage);
    // 暂停 / Bullet / Replay 链禁用（0x42dee0 再启用）；0x4184a0 Item 复位
    game.chains_suspended = true;
    if (game.items_live) game.items->reset();
    // (bec & 9) == 0：0x41b1a0 EnemyManager 析构（ECL ANIM 槽卸载），否则 0x4185b0 复位；随后无条件
    // 0x46f270 标删 effect.anm / bullet.anm 全部 VM、0x40da90 Bomb 析构、0x417790 Spellcard 析构
    if ((globals.flags & 9u) == 0) {
        if (game.enemies_live) { game.enemies.destroy(); game.enemies_live = false; }
    } else if (game.enemies_live) {
        game.enemies.reset();
    }
    animations.kill_file(8);
    animations.kill_file(BulletManager::kBulletAnmSlot);
    destroy_bombs();
    spellcard.destroy();
    if (game_update_chain_) { chains.remove(game_update_chain_); game_update_chain_ = nullptr; }
    unload_bgm();      // 0x45ed00(3) + DAT_004dc0f0 清空
    sound.suspend();   // 0x42e4d0
    // 0x42d200 末：场地清屏色回到黑（换关路径 bit0 必为 0，不会走全透明分支）
    compositor.background_color = (globals.flags & 1u) != 0 ? 0u : 0xff000000u;
    globals.post_load_frames = 1;
    // 0x45cd30 / 0x45cf10：原版换关装载期间结束仍在运行的震屏。
    // 这里同步完成装载，更新链看不到装载标记，因此在切换关卡时清除。
    clear_screen_shake();

    // ---- 0x42d700 + 0x42cb60（DAT_004c17d0 == 0）----
    mark_stage_reached();
    restart_play_clock();   // 0x42cb60 末：非回放重置 DAT_004a5c00
    game.flags = GameManager::kLoading;
    game.timer = Timer{};
    game.transition_frame = 0;
    game.transition_length = 0;
    game.end_counter = 0;
    game.entry = &stage_entry(globals.stage);
    game.world = PlayerWorld{};
    globals.game_frame = 0;
    globals.stage_frame = 0;
    if (globals.hiscore < globals.score) globals.hiscore = globals.score;
    // 0x449030（Replay 关卡段）：game_rng 种子 := visual_rng 种子低 16 位；视觉 RNG 调用计数清零
    animations.game_rng.seed = animations.visual_rng.seed & 0xffffu;
    animations.visual_rng.calls = 0;
    replay_stage_seed_ = u16(animations.visual_rng.seed & 0xffffu);
    if (game.mode == 1 && replay_file_loaded_) {
        // 回放：本关段备好（+0xd8 = -1），+0x214 仍指上一段——0x448eb0 之前上一段读尽后只回零输入；
        // 种子取段头 +2、全局块整块读回
        for (i32 i = 0; i < i32(replay_file_.stages.size()); ++i) {
            if (replay_file_.stages[size_t(i)].stage != i16(globals.stage)) continue;
            replay_next_stage_index_ = i;
            const ReplayStageSegment& seg = replay_file_.stages[size_t(i)];
            animations.visual_rng.seed = seg.rng_seed;
            animations.game_rng.seed = seg.rng_seed;
            if (seg.header.size() == ReplayFile::kStageHeaderSize)
                load_replay_globals(globals, seg.header.data() + kReplayGlobalsOffset);
            break;
        }
    }
    {
        AnmFile* logo = nullptr;
        std::vector<u8> msg_bytes;
        if (!load_gui_stage_assets(logo, msg_bytes)) return false;
        if (!game.gui.stage_init(logo, *game.entry, std::move(msg_bytes))) { failure = game.gui.error; return false; }
    }
    if (!create_stage()) return false;
    if ((globals.flags & 9u) == 0 && !create_enemies()) return false;
    create_bombs();
    game.gui.bomb = game.bombs.main();
    bind_spellcard();
    register_game_chains();
    game.transition_length = 0x3c;
    game.flags &= ~GameManager::kLoading;
    globals.flags &= ~0xbu;  // 0x42d200 末同样 DAT_004a5bec &= 0xfffffff4
    globals.post_load_frames = 0;
    game.timer.set(0);
    chains.enable(game_update_chain_);
    // 关卡链在过关奖励展示结束后的首帧（0x42dc50 → 0x40add0）才启用：这 120 帧里新关背景静止
    return true;
}

void GameSession::create_bombs() {
    destroy_bombs();
    BombContext context;
    context.animations = &animations;
    context.player = &game.player;
    context.globals = &globals;
    context.enemies = &game.enemies;
    context.bullets = game.bullets.get();
    context.lasers = game.lasers.get();
    context.effects = &effects;
    context.spellcard = &spellcard;
    context.sfx_sink = &sfx_events;
    context.world = &game.world;
    context.gui = &game.gui;
    game.bombs.create(std::move(context));
    game.player.bomb = game.bombs.main();
    game.player.season_release = game.bombs.release();
    game.gui.season_release = game.bombs.release();
    if (game.items_live) game.items->release_bonus = &game.bombs.release()->release_bonus;

    // 0x40d600：更新链 0x18 / 0x19（0x40dd00），绘制链 0x28 / 0x29（0x40da70）
    // 0x40d600 建链时即置启用位（与 Enemy / Spellcard 链等到 0x42dee0 才启用不同），不看换关状态
    bomb_update_chain_ = chains.create([this]() -> ChainResult {
        game.bombs.update_main(supervisor.game_speed);
        harvest_bomb_events();
        return ChainResult::Continue;
    }, 0x18, true);
    release_update_chain_ = chains.create([this]() -> ChainResult {
        game.bombs.update_release(supervisor.game_speed);
        harvest_bomb_events();
        return ChainResult::Continue;
    }, 0x19, true);
    bomb_draw_chain_ = chains.create([this]() -> ChainResult {
        game.bombs.draw_main(ascii, supervisor.game_speed);
        return ChainResult::Continue;
    }, 0x28, false);
    release_draw_chain_ = chains.create([this]() -> ChainResult {
        game.bombs.draw_release(ascii, supervisor.game_speed);
        return ChainResult::Continue;
    }, 0x29, false);
    chains.enable(bomb_update_chain_);
    chains.enable(release_update_chain_);
    chains.enable(bomb_draw_chain_);
    chains.enable(release_draw_chain_);
}

void GameSession::destroy_bombs() {
    for (ChainEntry** entry : {&bomb_update_chain_, &release_update_chain_, &bomb_draw_chain_, &release_draw_chain_}) {
        if (*entry != nullptr) { chains.remove(*entry); *entry = nullptr; }
    }
    if (game.bombs.main() == nullptr) return;
    game.bombs.destroy();
    game.player.bomb = nullptr;
    game.player.season_release = nullptr;
    if (game.items_live) game.items->release_bonus = nullptr;
}

void GameSession::sync_bomb_state() {
    const Bomb* main = game.bombs.main();
    const Bomb* release = game.bombs.release();
    const bool main_active = main != nullptr && main->active == 1;
    const bool release_active = release != nullptr && release->active == 1;
    game.enemies.bomb_active = main_active;
    game.enemies.release_active = release_active;
    if (game.items_live) {
        game.items->bomb_attract = main_active && main->timer.current < 0x3c;
        game.items->release_attract = release_active && release->timer.current < 10000;
    }
}

void GameSession::harvest_bomb_events() {
    BombContext& context = game.bombs.context();
    for (const BombShakeRequest& request : context.shake_requests) {
        if (request.kind == BombShakeRequest::kLinear) {
            create_screen_shake(EnemyManager::ShakeRequest{request.a, request.b, request.c});
        } else {
            create_screen_shake_envelope(request);
        }
    }
    context.shake_requests.clear();
}

// 0x45d150(8, amplitude, attack, sustain, decay) + 0x45d1a0：更新链 0x13（0x45cf10）
void GameSession::create_screen_shake_envelope(const BombShakeRequest& request) {
    auto shake = std::make_shared<ScreenShakeEnvelope>();
    shake->amplitude = request.a;
    shake->attack = request.b;
    shake->sustain = request.c;
    shake->decay = request.d;
    shake->timer.reset();
    ChainEntry* entry = chains.create([this, shake]() -> ChainResult {
        const bool frozen = ((game.flags | (game.flags >> 2)) & 1u) != 0 || (game.flags & 0x72u) != 0;
        return screen_shake_envelope_step(*shake, animations.visual_rng, geometry.window_scale, supervisor.game_speed,
                                          globals.post_load_frames, frozen, screen_shake)
                   ? ChainResult::Continue
                   : ChainResult::Destroy;
    }, 0x13, true);
    entry->on_destroy = [this, entry]() {
        shake_chains_.erase(std::remove(shake_chains_.begin(), shake_chains_.end(), entry), shake_chains_.end());
        chains.remove(entry);
    };
    shake_chains_.push_back(entry);
    chains.enable(entry);
}

// 0x45d150(1, duration, from, to) + 0x45d1a0：更新链 0x13（0x45cd30）、绘制链 0x54；计时结束返回 7 → 0x45d130 自删
void GameSession::create_screen_shake(const EnemyManager::ShakeRequest& request) {
    auto shake = std::make_shared<ScreenShake>();
    shake->duration = request.duration;
    shake->amp_from = request.amp_from;
    shake->amp_to = request.amp_to;
    shake->timer.reset();
    ChainEntry* entry = chains.create([this, shake]() -> ChainResult {
        return screen_shake_step(*shake, animations.game_rng, geometry.window_scale, supervisor.game_speed, screen_shake)
                   ? ChainResult::Continue
                   : ChainResult::Destroy;
    }, 0x13, true);
    entry->on_destroy = [this, entry]() {
        shake_chains_.erase(std::remove(shake_chains_.begin(), shake_chains_.end(), entry), shake_chains_.end());
        chains.remove(entry);
    };
    shake_chains_.push_back(entry);
    chains.enable(entry);
}

void GameSession::clear_screen_shake() {
    for (ChainEntry* shake : shake_chains_) chains.remove(shake);
    shake_chains_.clear();
    screen_shake = ScreenShakeOffset{};
}

// 0x45d150 / 0x45d1a0 及其内联副本（0x40c0d0 / 0x409f90 / 0x4199f0）：更新链 0x13 + 绘制链
//   kind 2 / 3（换关淡出 / 新关淡入）：绘制 0x45c990，优先级 0xa —— 紧跟关卡背景链 3 之后、层 3 绘制链之前，
//     画进场地渲染目标，不改视口
//   kind 0 / 5（结局 / 过关结束）：绘制 0x45c860，优先级 0x54 —— 先把视口拉成整幅后备缓冲再画
//   kind 4：绘制 0x45ccc0（场地矩形），优先级取 param_6，移植侧默认 0xa
//   kind 6 / 9：绘制 0x45cb50；kind 7：绘制 0x45cba0 固定 (128,16)-(512,464)；优先级 0x54
// 0x45d130：更新链自删时连同绘制链一起摘除
void GameSession::create_screen_fade(i32 kind, i32 duration, u32 color) {
    auto fade = std::make_shared<ScreenFade>();
    fade->kind = kind;
    fade->duration = duration;
    fade->timer.reset();
    if (kind == 4) {
        // param_3(+0x1c) = 循环次数；param_4(+0x20) = 起始 alpha<<24 | RGB（API 的 color 为 RGB，起始 alpha 默认 0xff）
        fade->color = 1;
        fade->color_aux = (color & 0xffffffu) | 0xff000000u;
        fade->alpha = 0xff;
    } else {
        fade->color = color;
        // 0x45d1a0：kind 3 / 9 从全不透明开始
        if (kind == 3 || kind == 9) fade->alpha = 0xff;
    }
    screen_fades.push_back(fade);
    const bool fade_in = kind == 0 || kind == 3;
    const bool fade_hold = kind == 9;
    const bool fade_from_aux = kind == 4;
    const bool fade_half = kind == 6 || kind == 7;
    const bool reset_viewport = kind == 0 || kind == 5;
    const i32 draw_priority = (kind == 0 || kind == 5 || kind == 6 || kind == 7 || kind == 9) ? 0x54 : 0x0a;
    ChainEntry* draw = chains.create([this, fade, reset_viewport, kind]() -> ChainResult {
        if (renderer.device == nullptr) return ChainResult::Continue;
        const float width = float(animations.render_target_width), height = float(animations.render_target_height);
        if (reset_viewport) {
            // 0x45c860：出批、SetViewport(0, 0, DAT_004d9d2c, DAT_004d9d30)
            renderer.flush();
            renderer.device->set_viewport(Viewport{0, 0, i32(animations.render_target_width), i32(animations.render_target_height)});
        }
        const u32 packed = (u32(fade->alpha) << 24) | ((kind == 4 ? fade->color_aux : fade->color) & 0xffffffu);
        if (kind == 4) {
            // 0x45ccc0：DAT_004d9d38/3c 起 DAT_004d9d44×DAT_004d9d40（场地 0,0 起 384×448）
            renderer.draw_screen_fill(0.0f, 0.0f, 384.0f, 448.0f, packed);
        } else if (kind == 7) {
            // 0x45cba0：固定矩形 (128,16)-(512,464)
            renderer.draw_screen_fill(128.0f, 16.0f, 512.0f, 464.0f, packed);
        } else {
            renderer.draw_screen_fill(width, height, packed);
        }
        return ChainResult::Continue;
    }, draw_priority, false);
    ChainEntry* entry = chains.create([this, fade, fade_in, fade_hold, fade_from_aux, fade_half]() -> ChainResult {
        bool alive;
        if (fade_hold) {
            alive = screen_fade_hold_step(*fade, globals.post_load_frames, supervisor.game_speed);
        } else if (fade_from_aux) {
            alive = screen_fade_from_aux_step(*fade, globals.post_load_frames, supervisor.game_speed);
        } else if (fade_half) {
            alive = screen_fade_half_step(*fade, supervisor.game_speed);
        } else if (fade_in) {
            alive = screen_fade_in_step(*fade, globals.post_load_frames, supervisor.game_speed);
        } else {
            const bool frozen = ((game.flags | (game.flags >> 2)) & 1u) != 0;
            alive = screen_fade_step(*fade, globals.post_load_frames, frozen, supervisor.game_speed);
        }
        return alive ? ChainResult::Continue : ChainResult::Destroy;
    }, 0x13, true);
    entry->on_destroy = [this, entry, draw, fade]() {
        fade_chains_.erase(std::remove(fade_chains_.begin(), fade_chains_.end(), entry), fade_chains_.end());
        fade_chains_.erase(std::remove(fade_chains_.begin(), fade_chains_.end(), draw), fade_chains_.end());
        screen_fades.erase(std::remove(screen_fades.begin(), screen_fades.end(), fade), screen_fades.end());
        chains.remove(draw);
        chains.remove(entry);
    };
    fade_chains_.push_back(entry);
    fade_chains_.push_back(draw);
    chains.enable(entry);
    chains.enable(draw);
}

void GameSession::accumulate_play_time() {
    // 0x42dbc0：非回放、DAT_004c17c4 不为 -1 / 3 时把距上次计时的 1/100 秒累加进总时长与本机体时长，并重置起点
    const i32 scene = globals.scene_request >= 0 ? globals.scene_request : i32(supervisor.requested);
    if (game.mode != 0 || scene == -1 || scene == 3) return;
    if (wall_clock_seconds < 0) return;
    const double elapsed = wall_clock_seconds - play_clock_origin;
    if (elapsed >= 0.0) {
        const u64 ticks = u64(elapsed * 100.0);
        const u64 total = scores.total_play_time() + ticks;
        scores.stats.play_time[0] = u32(total);
        scores.stats.play_time[1] = u32(total >> 32);
        scores.blocks[globals.shot + globals.shot_aux].play_time += ticks;
    }
    play_clock_origin = wall_clock_seconds;
}

void GameSession::restart_play_clock() {
    if (game.mode != 0 || wall_clock_seconds < 0) return;
    play_clock_origin = wall_clock_seconds;
}

bool GameSession::save_scores() {
    if (score_path.empty()) return false;
    const std::vector<u8> bytes = scores.save_image();
    std::ofstream out(score_path, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(bytes.data()), std::streamsize(bytes.size()));
    out.close();
    if (!out) return false;
    if (on_saved) on_saved();
    return true;
}

void GameSession::leave_game() {
    // 0x42d200：先 0x449a00 写成绩文件，再摘链、释放管理器；关卡背景 ANM 槽回收
    save_scores();
    if (game_update_chain_) { chains.remove(game_update_chain_); game_update_chain_ = nullptr; }
    for (StageManager* stage : {game.stage.get(), game.previous_stage.get()})
        if (stage != nullptr) remove_stage_chains(*stage);
    if (enemy_update_chain_) { chains.remove(enemy_update_chain_); enemy_update_chain_ = nullptr; }
    if (bullet_update_chain_) { chains.remove(bullet_update_chain_); bullet_update_chain_ = nullptr; }
    if (bullet_draw_chain_) { chains.remove(bullet_draw_chain_); bullet_draw_chain_ = nullptr; }
    if (item_update_chain_) { chains.remove(item_update_chain_); item_update_chain_ = nullptr; }
    if (item_draw_chain_) { chains.remove(item_draw_chain_); item_draw_chain_ = nullptr; }
    if (item_draw_season_chain_) { chains.remove(item_draw_season_chain_); item_draw_season_chain_ = nullptr; }
    if (laser_update_chain_) { chains.remove(laser_update_chain_); laser_update_chain_ = nullptr; }
    if (laser_draw_chain_) { chains.remove(laser_draw_chain_); laser_draw_chain_ = nullptr; }
    if (spellcard_update_chain_) { chains.remove(spellcard_update_chain_); spellcard_update_chain_ = nullptr; }
    if (spellcard_draw_chain_) { chains.remove(spellcard_draw_chain_); spellcard_draw_chain_ = nullptr; }
    // 0x449d60：得分飘字随 Item / Laser 一起释放
    if (popup_update_chain_) { chains.remove(popup_update_chain_); popup_update_chain_ = nullptr; }
    if (popup_draw_chain_) { chains.remove(popup_draw_chain_); popup_draw_chain_ = nullptr; }
    game.score_popups_live = false;
    game.player.score_popups = nullptr;
    destroy_bombs();
    game.gui.bomb = nullptr;
    if (gui_update_chain_) { chains.remove(gui_update_chain_); gui_update_chain_ = nullptr; }
    for (ChainEntry*& entry : gui_draw_chains_) {
        if (entry) { chains.remove(entry); entry = nullptr; }
    }
    // 0x43e4c0：GameMenu 随 GameManager 一起销毁
    for (ChainEntry** entry : {&menu_update_chain_, &menu_draw_chain_}) {
        if (*entry) { chains.remove(*entry); *entry = nullptr; }
    }
    if (menu_live) {
        menu.destroy();
        menu_live = false;
    }
    if (game.gui_live) {
        // 0x427a20：对话 VM 标删；关卡 logo ANM 槽 6 回收
        game.gui.shutdown();
        animations.kill_file(Gui::kLogoSlot);
        animations.unload(Gui::kLogoSlot);
    }
    clear_screen_shake();
    for (ChainEntry* fade : fade_chains_) chains.remove(fade);
    fade_chains_.clear();
    screen_fades.clear();
    text_into_texture.clear();
    animations.camera_pos = animations.camera_pos_offset = animations.camera_facing_norm = nullptr;
    for (ChainEntry** entry : {&replay_record_chain_, &replay_playback_chain_, &replay_draw_chain_}) {
        if (*entry) { chains.remove(*entry); *entry = nullptr; }
    }
    if (player_update_chain_) { chains.remove(player_update_chain_); player_update_chain_ = nullptr; }
    if (player_draw_chain_) { chains.remove(player_draw_chain_); player_draw_chain_ = nullptr; }
    // 0x4099a0 末：背景 ANM 槽只在非快速重开（DAT_004a5bec bit0 清）时释放
    for (StageManager* stage : {game.stage.get(), game.previous_stage.get()}) {
        if (stage == nullptr || (globals.flags & 1u) != 0) continue;
        const int slot = 3 + (stage->stage_number & 1);
        animations.kill_file(slot);
        animations.unload(slot);
    }
    stage_camera = StageCamera{};
    if (game.player_live) {
        // 0x441a50 析构：pl0N.anm / pl0Nsub.anm 的 VM 全部标删（0x46f270）并释放槽 9 / 0x1e
        // 标删的 VM 与原版一样等到下一次世界 tick 才回收：这里不能额外 tick，否则刚建的 Loading VM 会多跑几帧
        animations.kill_file(9);
        animations.kill_file(0x1e);
        animations.unload(9);
        animations.unload(0x1e);
    }
    // (DAT_004a5bec & 9) != 0 —— 同一关快速重开 / Extra 续关：EnemyManager 走 0x4185b0 复位而非 0x41b1a0 析构，
    // 留给下一局沿用（0x42cb60 以 0x42ca00 接手）；Loading VM 属 AsciiManager，不随 GameManager 消失
    const bool keep_enemies = (globals.flags & 9u) != 0 && game.enemies_live;
    const u32 loading_vm = game.loading_vm;
    if (keep_enemies) {
        game.enemies.reset();
        EnemyManager retained = std::move(game.enemies);
        game = GameManager{};
        game.enemies = std::move(retained);
        game.enemies_live = true;
    } else {
        game = GameManager{};
    }
    game.loading_vm = loading_vm;
    game_live = false;
    unload_bgm();      // 0x45ed00(3) + DAT_004dc0f0 清空（符卡练习重开 / 回放 / 换关过场除外）
    sound.suspend();   // 0x42e4d0
    // 0x42d200 末：场地清屏色回到黑；同一关快速重开（bit0）时清成全透明黑，让上一帧画面留到重开完成
    compositor.background_color = (globals.flags & 1u) != 0 ? 0u : 0xff000000u;
}

// 0x43e5a0（0x4314a0 之后、0x41b340 之前）：新开局建 GameMenu 并登记更新链 10；换关（next_stage）沿用同一对象
void GameSession::create_menu() {
    if (menu_live) {
        menu.destroy();
        menu_live = false;
    }
    if (menu_draw_chain_) { chains.remove(menu_draw_chain_); menu_draw_chain_ = nullptr; }
    menu = GameMenu{};
    menu.reset_timers();   // 0x43e3f0 末：两只计时复位
    menu.ctx.animations = &animations;
    menu.ctx.globals = &globals;
    menu.ctx.game = &game;
    menu.ctx.gui = &game.gui;
    menu.ctx.player = &game.player;
    menu.ctx.scores = &scores;
    menu.ctx.input = &supervisor.input;
    menu.ctx.text_anm = animations.file(0);
    menu.ctx.game_speed = &supervisor.game_speed;
    menu.ctx.sound_thread_state = &globals.sound_thread_state;
    menu.ctx.config_flags = &config_flags;
    menu.ctx.music = &music;
    menu.ctx.sfx_sink = &sfx_events;
    menu.ctx.sound = &sound;
    if (!replay_store_) {
        auto store = std::make_unique<DiskReplayStore>();
        store->session = this;
        replay_store_ = std::move(store);
    }
    menu.ctx.replays = replay_store_.get();
    // 0x42e760：help.anm → 槽 0x13（原版由装载线程异步完成；这里同步装入）
    menu.ctx.accumulate_play_time = [this]() { accumulate_play_time(); };
    menu.ctx.restart_play_clock = [this]() { restart_play_clock(); };
    menu.ctx.load_help_anm = [this]() -> AnmFile* {
        if (AnmFile* loaded = animations.file(0x13)) return loaded;
        AnmResource* res = nullptr;
        if (!load_anm("help.anm", res)) return nullptr;
        return animations.load(0x13, *res);
    };
    menu_live = true;
    menu_update_chain_ = chains.create([this]() -> ChainResult {
        if (!menu_live || game.chains_suspended) return ChainResult::Continue;
        menu.update();
        harvest_menu_events();
        return ChainResult::Continue;
    }, 10, true);
    chains.enable(menu_update_chain_);
    // 绘制链 0x4a（0x43ef10 → 0x43edc0）建为禁用，关卡首帧 0x42dee0 启用（on_stage_started）
    menu_draw_chain_ = chains.create([this]() -> ChainResult {
        if (menu_live) menu.draw(ascii, compositor);
        return ChainResult::Continue;
    }, 0x4a, false);
}

// GameMenu 未挂帧级音效表时登记到自身队列的音效（0x45e150）并入本帧音效表
void GameSession::harvest_menu_events() {
    if (!menu_live) return;
    for (i32 id : menu.sfx_queue) sfx_events.push(id);
    menu.sfx_queue.clear();
}

// 0x42d200 中段：非（符卡练习且快速重开）且非（回放 / 过关换关）时停曲并清当前曲名
void GameSession::unload_bgm() {
    const u32 f = globals.flags;
    if (((f & 0x30u) != 0x20u || (f & 1u) == 0) && (f & 0x42u) == 0) {
        music.stop_track();
        music.current.clear();
    }
}

i32 GameSession::process_sound() {
    music.step();
    sound.process();
    return music.head_kind();
}

void GameSession::drain_sound() {
    for (int k = 0; k < 64 && process_sound() != 0; ++k) {}
}

// 自机音效已随 Player::play_sfx 直接进入 sfx_events；这里只取走自机子集（harness 的 player.sfx 字段）
void GameSession::harvest_player_sfx() {
    player_sfx_events.insert(player_sfx_events.end(), game.player.sfx_queue.begin(), game.player.sfx_queue.end());
    game.player.sfx_queue.clear();
}

bool GameSession::logic_step() {
    sample_input();
    sfx_events.clear();
    player_sfx_events.clear();
    process_sound();   // 0x43b3d0 开头的 0x45e330：乐曲命令推进一步，上一帧登记的音效在此出声
    music.tick_fade(); // 紧随其后的 0x43dcf0


    // 游戏中登记的 DAT_004c17c4（过关 0x42e150 → 0xc、装载失败 → 2）在下一帧主循环 0x43ce10 处被消费
    if (game_live && globals.scene_request >= 0) {
        supervisor.request(SceneId(globals.scene_request));
        globals.scene_request = -1;
    }
    SceneId leave = SceneId::Invalid, enter = SceneId::Invalid;
    if (supervisor.consume_switch(leave, enter)) {
        // 0x43ce10 case 0xd 需要在 leave_title 前抄出 demo 下标 / 选中的回放路径
        const i32 pending_demo = (leave == SceneId::Title) ? title.pending_demo : -1;
        const std::string pending_replay = (leave == SceneId::Title) ? title.pending_replay_path : std::string{};
        const i32 pending_replay_stage = (leave == SceneId::Title) ? title.replay_stage_choice : 0;
        if (leave == SceneId::Title && enter != SceneId::PracticeFromTitle) leave_title();
        if (enter == SceneId::Title && !enter_title_assets()) return false;
        if (enter == SceneId::NextStage) {
            // 0x43ce10 case 0xc：GameManager 销毁 / 重建后场景仍为 7
            if (!next_stage()) return false;
            supervisor.current = supervisor.requested = SceneId::Game;
        } else if (enter == SceneId::Restart || enter == SceneId::RestartPractice || enter == SceneId::RestartExtra ||
                   enter == SceneId::RestartSpellPractice) {
            // 0x43ce10 case 0xa / 0xb / 0xe / 0x13：0x42d780 整体销毁，关卡回到开局选择（DAT_004a5794），新开局重建
            if (!restart_game(enter, enter == SceneId::RestartPractice ? 1 : 0)) return false;
            supervisor.current = supervisor.requested = SceneId::Game;
        } else if (enter == SceneId::PracticeFromTitle) {
            // 0x43ce10 case 0xd：标题 demo / Replay 选槽 / 练习进游戏，0x42d700(1)
            bool loaded = false;
            if (!pending_replay.empty()) {
                loaded = load_replay_file(pending_replay.c_str());
                if (!loaded) return false;
            } else if (pending_demo >= 0 && !demo_directory.empty()) {
                char path[512];
                std::snprintf(path, sizeof(path), "%s/demo/demo%d.rpy", demo_directory.c_str(), pending_demo + 1);
                if (!load_replay_file(path)) {
                    // artifacts/th16/dat 扁平布局：demo1.rpy 与 th16.dat 同级
                    std::snprintf(path, sizeof(path), "%s/demo%d.rpy", demo_directory.c_str(), pending_demo + 1);
                    if (!load_replay_file(path)) return false;
                }
                loaded = true;
            }
            if (loaded) apply_replay_start(pending_replay.empty() ? -1 : pending_replay_stage);
            if (leave == SceneId::Title) leave_title();
            if (!enter_game(1)) return false;
            supervisor.current = supervisor.requested = SceneId::Game;
        } else if (enter == SceneId::Ending || enter == SceneId::ExtraEnding) {
            if (leave == SceneId::Game) leave_game();
            if (leave == SceneId::Title) leave_title();
            if (!enter_ending(enter == SceneId::ExtraEnding)) return false;
        } else {
            if (leave == SceneId::Ending) leave_ending();
            if (leave == SceneId::Game) leave_game();
            if (enter == SceneId::Exit) {
                // 退出进程：0x43afe0 Supervisor 析构先经 0x449a00 写成绩文件；随后 0x459830 主循环返回处写 th16.cfg
                quit_requested = true;
                save_scores();
                save_config();
            }
            if (enter == SceneId::Game && !enter_game()) return false;
            // 结局回标题：跳过启动幕等待，直接启用标题菜单（0x419450 之后请求 4）
            if (enter == SceneId::Title && leave == SceneId::Ending) {
                if (!activate_title_menu()) return false;
            }
        }
    }

    // 启动幕：绘制链推进 boot_frames；标题线程等待条件（0x44abea / 0x44ac00）
    chains.run_update();
    // 0x45a9f0 族：更新链后 0x417bc0 符卡实时计时 / 0x4262f0 FPS 测速
    if (wall_clock_seconds >= 0) {
        const bool paused = game_live && (game.flags & (GameManager::kLoading | GameManager::kMenuOpen)) != 0;
        frame_timer.tick(wall_clock_seconds, skip_frames, game_live, paused);
        global_fps_ = frame_timer.fps;
        if (global_fps_draw_chain_ != nullptr &&
            (global_fps_draw_chain_->flags & ChainManager::kFlagEnabled) == 0) {
            chains.enable(global_fps_draw_chain_);
        }
    }
    if (game_live) {
        spellcard.wall_clock_seconds = wall_clock_seconds;
        spellcard.tick_realtime();
    }
    // 0x45a8a0：更新链之后 BeginScene → 绘制链 → EndScene
    if (run_draw_chains) draw_frame();
    // 标题线程（0x44abea / 0x44ac00）在帧间醒来：启动幕绘制链把 +0x638 数到 0xb4 后装 title*.anm 并登记更新链，
    // 该链从下一帧的更新起运行
    if (!supervisor.title_menu_live && !game_live && supervisor.title_assets_ready && supervisor.boot_hold_elapsed()) {
        if (!activate_title_menu()) return false;
    }
    // 不跑绘制链时补上关卡绘制链（3：0x409f90 淡入逻辑；6：0x40a410 结束计时）里改动逻辑状态的部分。
    // 顺序同绘制链：两关的 3 号（上一关先登记）在前，再两关的 6 号；链未启用（0x40add0 之前）的关卡跳过
    if (game_live && !run_draw_chains) {
        StageManager* stages[2] = {game.previous_stage.get(), game.stage.get()};
        for (StageManager* stage : stages)
            if (stage != nullptr && stage->background_draw_chain != nullptr &&
                (stage->background_draw_chain->flags & ChainManager::kFlagEnabled) != 0)
                stage->transition_step();
        for (StageManager* stage : stages)
            if (stage != nullptr && stage->foreground_draw_chain != nullptr &&
                (stage->foreground_draw_chain->flags & ChainManager::kFlagEnabled) != 0)
                stage->end_timer_step(supervisor.game_speed);
    }
    return failure.empty();
}

void GameSession::draw_frame() {
    // 0x45a8a0：更新链之前出批并切 UI 相机；绘制前丢弃残批、雾缓存置未知后关雾；绘制后出批、解绑纹理
    renderer.use_camera(compositor.ui_camera);
    renderer.begin_frame();
    chains.run_draw();
    renderer.end_frame();
}

void GameSession::register_layer_draw_chains() {
    // 0x46a3a0：AnmManager 构造时按优先级登记的层绘制链。多数只是 0x46e750(层)；带相机的几条见 ScreenCompositor。
    struct LayerNode { i32 priority; i32 layer; };
    static constexpr LayerNode kPlainLayers[] = {
        {0x05, 0x00}, {0x07, 0x01}, {0x09, 0x02}, {0x0b, 0x04}, {0x0d, 0x05}, {0x10, 0x06}, {0x12, 0x07},
        {0x14, 0x08}, {0x15, 0x09}, {0x16, 0x0a}, {0x18, 0x0b}, {0x1b, 0x0c}, {0x1c, 0x0d}, {0x1f, 0x0e},
        {0x20, 0x0f}, {0x22, 0x10}, {0x24, 0x11}, {0x27, 0x12}, {0x2a, 0x13}, {0x2e, 0x15}, {0x34, 0x16},
        {0x36, 0x17}, {0x3b, 0x19}, {0x3c, 0x25}, {0x3d, 0x1a}, {0x3e, 0x1b}, {0x3f, 0x26}, {0x4d, 0x1d},
        {0x4e, 0x28}, {0x4f, 0x1e}, {0x50, 0x29}, {0x52, 0x1f}, {0x53, 0x2a},
    };
    for (const LayerNode& node : kPlainLayers) {
        chains.enable(chains.create([this, layer = node.layer]() {
            renderer.draw_layer(layer);   // 0x46dd90 族
            return ChainResult::Continue;
        }, node.priority, false));
    }
    // 0x46ddd0（优先级 10）：渲染目标相机复位到正对视口中心并切换、关雾，画层 3
    chains.enable(chains.create([this]() {
        stage_camera.reset_to_viewport_center();
        renderer.use_camera(stage_camera);
        renderer.set_fog(false);
        renderer.draw_layer(3);
        return ChainResult::Continue;
    }, 0x0a, false));
    // 0x46e030（优先级 0x2d）：宽场地相机（抖动偏移 DAT_004c1530）、关 Z 写、ZFUNC ALWAYS，画层 0x14
    chains.enable(chains.create([this]() {
        compositor.wide_field_camera.shift_x = screen_shake.offset_scaled.x;
        compositor.wide_field_camera.shift_y = screen_shake.offset_scaled.y;
        renderer.use_camera(compositor.wide_field_camera);
        renderer.set_depth_write(false);
        renderer.set_depth_compare(touhou::graphics::Compare::Always);
        renderer.draw_layer(0x14);
        return ChainResult::Continue;
    }, 0x2d, false));
    // 0x46e0d0（优先级 0x3a）：UI 相机、关 Z 写、ZFUNC ALWAYS、绘制偏移清零，画层 0x18
    chains.enable(chains.create([this]() {
        renderer.use_camera(compositor.ui_camera);
        renderer.set_depth_write(false);
        renderer.set_depth_compare(touhou::graphics::Compare::Always);
        renderer.set_draw_offset(0.0f, 0.0f);
        renderer.draw_layer(0x18);
        return ChainResult::Continue;
    }, 0x3a, false));
    // 0x46e210（优先级 0x37）：UI 相机、关 Z 写、ZFUNC ALWAYS，画层 0x24
    chains.enable(chains.create([this]() {
        renderer.use_camera(compositor.ui_camera);
        renderer.set_depth_write(false);
        renderer.set_depth_compare(touhou::graphics::Compare::Always);
        renderer.draw_layer(0x24);
        return ChainResult::Continue;
    }, 0x37, false));
    // 0x46e180（0x40）/ 0x46e2b0（0x41）：场地相机画层 0x1c / 0x27，再切回 UI 相机
    for (const LayerNode& node : {LayerNode{0x40, 0x1c}, LayerNode{0x41, 0x27}}) {
        chains.enable(chains.create([this, layer = node.layer]() {
            renderer.use_camera(compositor.field_camera);
            renderer.draw_layer(layer);
            renderer.use_camera(compositor.ui_camera);
            return ChainResult::Continue;
        }, node.priority, false));
    }
}

}  // namespace th16
