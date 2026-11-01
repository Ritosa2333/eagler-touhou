// Application 主流程（th15 Application.cpp 裁剪：destination 驱动、装载挡板、
// 帧序固定「装载完成 → 场景 controls+scheduler → 显示绘制 → 装载挡板 →
// apply_destination」，docs/plans/ui-base.md §四）。
#include "ApplicationState.hpp"
#include "game/TitleFrame.hpp"
#include "game/VisualBootstrap.hpp"

#include <SDL3/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace th13 { extern bool damage_probe_enabled; }
namespace th13::sdl {

namespace {
// 常驻 ANM bank 表（th13 与 th15 同名同布局，dumps/anm-list.txt 实证）。
constexpr std::pair<const char*, i32> kBanks[7] = {
    {"text.anm", 0}, {"sig.anm", 1}, {"ascii.anm", 2},  {"effect.anm", 8},
    {"title.anm", 16}, {"title_v.anm", 17}, {"help.anm", 19},
};
// front 面板在 begin_run 时由场景资产登记（残机/灵击脚本 30–45 已验证）。
constexpr i32 kFrontBank = 3;
// Stage1 战斗资产 bank：stage = ANIM 名单顺序合并（脚本号全局连续，ECL 302/303
// 引用即此序）、bullet = 弹幕外观表脚本、player = plXX.anm（选人定档）、
// scenery = STD 引用 ANM（st01.std → st01wl.anm）。
constexpr i32 kStageBank = 4, kBulletBank = 5, kPlayerBank = 6, kSceneryBank = 7;
constexpr i32 kSpiritBank = 25; // 00405530: astral.anm.
constexpr i32 kStageLogoBank = 27;
}  // namespace

Application::Application() : state(std::make_unique<ApplicationState>()) {}
Application::~Application() = default;
bool Application::initialize(bool device) { return state->initialize(device); }
bool Application::step(const GameInput& input, float fps, bool focus) {
    return state->step(input, fps, focus);
}
const std::string& Application::error() const { return state->failure; }
const i32* Application::projection() const { return state->projection(); }

bool ApplicationState::fail(const std::string& why) {
    if (failure.empty()) failure = why.empty() ? "Application operation failed" : why;
    return false;
}

bool ApplicationState::read(const std::string& name, std::vector<u8>& bytes, bool required) {
    const auto at = name.find_last_of("/\\");
    const auto base = name.substr(at == std::string::npos ? 0 : at + 1);
    if (base.empty() || base == "." || base == "..") return fail("Invalid game asset name");
    const auto found = resource_cache.find(base);
    if (found != resource_cache.end()) {
        bytes = found->second;
        return true;
    }
    size_t size = 0;
    auto* data = static_cast<u8*>(SDL_LoadFile(("/assets/" + base).c_str(), &size));
    if (!data) return required ? fail("Missing game asset: " + base) : false;
    bytes.assign(data, data + size);
    SDL_free(data);
    if (bytes.size() <= 1024 * 1024 && base.size() >= 4 && base.substr(base.size() - 4) != ".wav" &&
        base.substr(base.size() - 4) != ".anm")
        resource_cache.emplace(base, bytes);
    return true;
}

bool ApplicationState::prepare_platform() {
    if (platform_prepared) return true;
    if (!graphics.initialize()) return fail(graphics.error);
    // 0043a040 seeds both RNGs from timeGetTime's low16 bits.
#ifdef __EMSCRIPTEN__
    // Deterministic same-seed capture support: the Web test harness may set
    // window.__th13_boot_seed before module initialization.  When absent the
    // production path still uses SDL_GetTicks().
    const int override = EM_ASM_INT({ return window.__th13_boot_seed ?? -1; });
    boot_seed = override >= 0 ? u16(override) : u16(SDL_GetTicks());
#else
    boot_seed = u16(SDL_GetTicks());
#endif
    visual.reseed(boot_seed);
    game_random.reseed(boot_seed);
    // 0046b7c4/0046b7ee load ECX=004dc324 before distortion random draws.
    environment.shared_game_rng = &game_random;
    loading_environment.shared_game_rng = &game_random;
    platform_prepared = true;
    return true;
}

// ---- [R3-M] 标题 Replay 菜单平台（TitleReplayPlatform；0x452300 槽位语义）----
#ifdef __EMSCRIPTEN__
// 浏览器文件选择触发（EM_JS 定义于 BrowserRuntime.cpp 文件导入段；页面壳
// th13.html 承接弹窗与拖放）。
extern "C" void th13_browser_replay_request();
#endif

i32 ApplicationState::replay_count() const { return i32(replay_slots.size()); }

const ReplayFile* ApplicationState::replay_entry(i32 index) const {
    if (index < 0 || index >= i32(replay_slots.size())) return nullptr;
    const ReplayCatalogSlot& slot = replay_slots[u32(index)];
    return slot.decoded ? slot.decoded.get() : nullptr;
}

std::string ApplicationState::replay_filename(i32 index) const {
    if (index < 0 || index >= i32(replay_slots.size())) return {};
    return replay_slots[u32(index)].name;
}

bool ApplicationState::request_replay_file() {
#ifdef __EMSCRIPTEN__
    th13_browser_replay_request();
    return true;
#else
    replay_import_error_ = "Replay import needs the browser runtime";
    return false;
#endif
}

std::string ApplicationState::replay_import_error() const { return replay_import_error_; }

bool ApplicationState::start_replay(i32 file_index, i32 stage_index) {
    // 只排队 destination（0x4525b0 case3 写 DAT_004dcc24=0xd 的等价拍）：帧尾
    // apply_destination 消费，菜单 update 内不销毁自身。
    if (file_index < 0 || file_index >= i32(replay_slots.size()) ||
        !replay_slots[u32(file_index)].decoded)
        return fail("Replay start on empty slot: " + std::to_string(file_index));
    if (stage_index < 0) return fail("Replay start with negative stage index");
    pending_replay_file = file_index;
    replay_catalog_index = file_index;
    pending_replay_stage_slot = stage_index;
    pending_destination = i32(Destination::Run);
    return true;
}

bool ApplicationState::import_replay_bytes(const u8* data, size_t size, const std::string& name) {
    auto decoded = std::make_shared<ReplayFile>();
    if (!decoded->decode(data, size)) {
        replay_import_error_ = name + ": " + decoded->error();
        return false;
    }
    // 槽位（0x452300 语义）：th13_NN.rpy（01..25）→ 编号槽 NN-1（同名重导入
    // 覆盖）；其余 → 首个空闲用户槽（25..73；74 为网页导入行保留）。
    i32 slot = -1;
    if (name.size() >= 11 && name.compare(0, 5, "th13_") == 0 &&
        name.compare(7, 4, ".rpy") == 0 &&
        std::all_of(name.begin() + 5, name.begin() + 7,
                    [](char c) { return c >= '0' && c <= '9'; })) {
        const i32 number = (name[5] - '0') * 10 + (name[6] - '0');
        if (number >= 1 && number <= 25) slot = number - 1;
    }
    if (slot < 0) {
        for (i32 user = 25; user < i32(replay_slots.size()) - 1; user++) {
            if (!replay_slots[u32(user)].decoded) {
                slot = user;
                break;
            }
        }
        if (slot < 0) {
            replay_import_error_ = name + ": replay slots full";
            return false;
        }
    }
    ReplayCatalogSlot& target = replay_slots[u32(slot)];
    target.name = name;
    target.bytes.assign(data, data + size);
    target.decoded = std::move(decoded);
    replay_import_error_.clear();
    if (title) title->state.replay_import_hint = slot;  // 菜单 substate2 选中
    return true;
}

bool ApplicationState::prepare_loading() {
    if (!prepare_platform()) return false;
    std::vector<u8> bytes;
    loading_environment.resolution_scale = 1;
    loading_environment.raster_scale = float(graphics.render_scale);
    constexpr std::pair<const char*, i32> loading_banks[3] = {
        {"sig.anm", 1}, {"ascii.anm", 2}, {"text.anm", 0}};
    for (const auto& bank : loading_banks) {
        if (!loading_animations.resource(bank.second) &&
            (!read(bank.first, bytes) ||
             !loading_animations.load(bank.second, bytes.data(), u32(bytes.size())) ||
             !graphics.preload(*loading_animations.resource(bank.second))))
            return fail("Loading artwork: " + loading_animations.error + graphics.error);
    }
    return draw_loading(true);
}

bool ApplicationState::loading_waiting() const { return loading_startup || pending_load != 0; }

bool ApplicationState::draw_loading(bool signature) {
    // 真机 boot 开场：sig.anm script0（自带 script1 右半 spawn，两半 512×480
    // 拼 640 宽）自黑场 10f 淡入，g1..g179 共 179 帧后卸载。**同时**原版在
    // boot f0 就创建 ascii.anm script17 祈祷根（0x439ded/0x42bfa0；G 归因：
    // artifacts/round-16/agent-g-menu/segment-attribution.json）：script17 每
    // 4 帧 op503 生成 script16，每次 12 个变量读 = 24 visual index 单位；
    // 原生签名幕 1..177 共 45 次 = 1080 单位（本文件旧注释「无 ascii 祷告幕」
    // 与原生迹线矛盾，已按证据更正）。180 帧挡板由 advance_loading 计
    // （g181 = 标题任务使能帧），两种 draw_loading 分支都必须建这棵树：
    // signature=true 是生产启动幕，signature=false 是菜单/回放换场挡板。
    graphics.presentation.reset();
    if (!loading_animations.retire(loading_signature) ||
        !loading_animations.retire(loading_prayer))
        return fail(loading_animations.error);
    if (signature) {
        loading_signature = loading_animations.create(1, 0);
        if (!loading_signature) return fail(loading_animations.error);
        // [窗A 修正] sig 右半（script0 自 spawn 的 script1）层 = L26（oracle boot
        // 全程 f0-f178 实测 23/26；dump ins_304(25) 的 +1 偏移，蓝图 §8.3）。
        // 直接落子 VM 层——注意 tests/harness/harness.cpp 的 prepare_loading
        // 镜像需同步此行（该文件非本轮所有权，已向协调方报告）。
        if (auto* boot = loading_animations.registry.find(loading_signature))
            if (auto* right = loading_animations.registry.find_child_script(*boot, 1, 0))
                right->visual.layer = 26;
    }
    // 417370 /439ced create ASCII script17 at (480,392) while loading. The
    // create only primes the root (no RNG draws); the loader's ordinary
    // main+secondary passes drive the op501/op503/op200 loop from there.
    loading_prayer = loading_animations.create(2, 17, -1, 0, {480,392,0});
    if (!loading_prayer) return fail(loading_animations.error);
    loading_startup = signature;
    loading_frames = 0;
    loading_debt = 0;
    return render_loading();
}

bool ApplicationState::render_loading() {
    renderer.invalidate();
    // textureFactor 保持（真机 TFACTOR 跨帧持续的设备态，蓝图 §8.8）。
    const auto factor_kept = graphics.backend.pipeline().textureFactor;
    graphics.backend.pipeline() = touhou::graphics::PipelineState{};
    graphics.backend.pipeline().textureFactor = factor_kept;
    if (!graphics.select_target(nullptr, 0) || !graphics.clear_target(0xff000000, nullptr))
        return fail(graphics.error);
    ScreenViews loading_views{renderer, loading_environment};
    if (!loading_views.camera(DrawCamera::Fullscreen, false)) return fail("Loading camera unavailable");
    // [boot @R] 装载幕期同发 4 条边条（oracle raw f1 起每帧恒定；仅捕获态生效）。
    emit_title_side_strips(loading_animations);
    // sig 幕止于 raw f179（oracle 实测 f180 为空档帧，f181 背景入场）。
    if (loading_frames < 180) {
        const auto* credit = loading_animations.registry.find(loading_signature);
        for (u32 layer = 0; layer < 33; ++layer) {  // th13 层域：主链 0-30 + 副链 31/32
            if (!renderer.draw_layer(loading_animations.registry.layer(layer))) return fail(renderer.error);
            if (credit && layer == u32(credit->visual.layer)) {
                renderer.flush();
                graphics.draw_startup_branding(credit->visual.color);
            }
        }
    }
    renderer.flush();
    graphics.present();
    return true;
}

bool ApplicationState::advance_loading_caption() {
    // 0x46f170 + 0x46f240 over the loading manager: one loader frame. The
    // production startup drives its 180-frame gate with this; the debug replay
    // entry uses the same primitive for the three loader frames the original
    // runs between the ASCII script17 create (f0) and battle frame 0 (f4).
    if (!loading_animations.update(false) || !loading_animations.update(true))
        return fail(loading_animations.error);
    return true;
}

bool ApplicationState::draw_loading_battle() {
    if (!run || !loading_prayer) return true;
    if (!loading_animations.registry.find(loading_prayer)) return true;
    // progress.stage_frame is sampled after this frame's update: battle frame
    // 0 -> 1, battle frame 9 -> 10. Native draws the fading loading sheet for
    // battle frames 0..9 and retires the tree before frame 10 (H evidence:
    // loading-op-ownership.json, last drawn frame 9).
    const i32 battle_frame = progress.stage_frame - 1;
    if (battle_frame < 0 || battle_frame > 10) return true;
    if (battle_frame == 10) {
        // 0x4172e0 -> 0x46fc30: retire the whole caption tree. Retirement only
        // marks the tree; it draws nothing and consumes no RNG.
        if (!loading_animations.retire(loading_prayer))
            return fail(loading_animations.error);
        return true;
    }
    // Battle frame 0: one fore-pool tick (layer 31/32 only) brings the caption
    // VMs to the exact state the native draw logs at frame 0 (alpha 0x9F on
    // the UP strip, 2-triangle strip). The layer26 script17 root is
    // deliberately not ticked: its op200 loop would spawn script16 and draw
    // visual RNG, which the original avoids by interrupting the root at battle
    // start. The script14/15/16 children have no random opcodes left after
    // their initial tick, so this tick adds no visual-RNG draw (b2-boot 300).
    if (battle_frame == 0) {
        if (!loading_animations.update(true)) return fail(loading_animations.error);
    }
    // Native fades the still-live loading sheet with the loader's 30-frame
    // fade-out: the drawn vertex alpha is floor(base * (30 - f) / 30) with
    // base 0x9F at battle frame 0, giving the measured 0x9F,0x99,0x94,0x8F,
    // 0x89,0x84,0x7F,0x79,0x74,0x6F ramp (H loading-op-ownership.json). The
    // port has no equivalent global loader-fade multiplier, so apply it to
    // the fore-pool VM colors for this draw only and restore them afterwards;
    // no Update phase, timer or RNG state is touched.
    struct SavedColor {
        AnmVm* vm;
        u32 color;
        u32 secondary;
    };
    std::vector<SavedColor> saved;
    auto& fore = loading_animations.registry.layer(31);
    saved.reserve(fore.size());
    const u32 fade = u32(30 - battle_frame);
    for (auto* vm : fore) {
        saved.push_back({vm, vm->visual.color, vm->visual.secondary_color});
        vm->visual.color = (vm->visual.color & 0x00ffffffu) |
                           (u32(u8((vm->visual.color >> 24) * fade / 30u)) << 24);
        vm->visual.secondary_color = (vm->visual.secondary_color & 0x00ffffffu) |
                                     (u32(u8((vm->visual.secondary_color >> 24) * fade / 30u)) << 24);
    }
    // Same submission context as AnmDrawSchedule's layer31 entry
    // ({31,71,2,false,true,true,true,false}): full-screen camera, fog off,
    // depth write off/compare always, no presentation offset.
    renderer.invalidate();
    bool drawn = views.camera(DrawCamera::Fullscreen, false);
    if (drawn) {
        graphics.set_fog(false);
        graphics.set_depth_mask(false);
        graphics.set_depth_compare(touhou::graphics::Compare::Always);
        renderer.offset = {};
        drawn = renderer.draw_layer(fore);
    }
    for (const auto& item : saved) {
        item.vm->visual.color = item.color;
        item.vm->visual.secondary_color = item.secondary;
    }
    if (!drawn) return fail(renderer.error.empty() ? "Loading caption camera unavailable"
                                                   : renderer.error);
    return true;
}

bool ApplicationState::advance_loading(double delta) {
    // 启动保留 180 帧原生装载动画；仅推进隔离的装载 ANM 时钟（th15 同款）。
    loading_debt += std::clamp(delta, 0., .1);
    while (loading_debt + 1e-9 >= 1. / 60. && loading_frames < 180) {
        loading_debt = std::max(0., loading_debt - 1. / 60.);
        if (!advance_loading_caption()) return false;
        ++loading_frames;
    }
    if (!render_loading()) return false;
    if (loading_frames >= 180) {
        // 0x4172e0 -> 0x46fc30 retires the caption tree when the harness gate
        // ends (native trace: last prayer spawn at frame 177, none during the
        // 181..480 title idle window). Retire both roots so no loading VM can
        // survive into the title or be ticked by a later scene.
        if (!loading_animations.retire(loading_signature) ||
            !loading_animations.retire(loading_prayer))
            return fail(loading_animations.error);
        loading_startup = false;
        loading_debt = 0;
    }
    return true;
}

bool ApplicationState::initialize(bool device) {
    if (initialized) return fail("Application already initialized");
    if (!prepare_platform()) return false;
    if (!loading_animations.resource(2) && !prepare_loading()) return false;
    // 字体设备失败不终止启动（标题/HUD 文本走 ascii.anm 通道，不依赖字体），
    // 但显式记录：对话日文渲染保持 BLOCKED，随 projection[15] 暴露，不静默。
    if (!fonts.initialize()) fonts_failure = fonts.error;
    // 0043a040 -> 0045ac90 builds DAT_004be8a0 with 256 visual RNG steps.
    // The table is retained by the font/runtime owner; advancing the same
    // visual RNG here preserves the ANM stream before title construction.
    visual_font_noise_table = visual_font_noise(visual);
    if (!audio_device.initialize(device)) return fail(audio_device.error);
    environment.raster_scale = float(graphics.render_scale);
    environment.render_target_has_alpha = true;
    environment.screen_offsets = {32, 16, 320, 16};
    environment.playfield_origin = {32, 16};
    if (!files.initialize()) return fail(files.error);
    // 存档音量 → 设备真值（原版 DAT_004dc892/893；缺档默认 100/80）。
    // 混音实时读两字段（AudioDevice.cpp :287-297 SE 曲线 / :306 BGM 曲线）。
    audio_device.music_volume = files.bgm_volume;
    audio_device.effect_volume = files.effect_volume;
    // MusicRoom 曲目目录（musiccmt.txt 缺席 = 空目录，不阻断启动）。
    load_music_room_catalog();
    connect_record_menus();
    animations.resource_release = [this](const AnmResource& resource) { graphics.unload(resource); };
    std::vector<u8> bytes;
    for (const auto& bank : kBanks) {
        if (!read(bank.first, bytes) ||
            !animations.load(bank.second, bytes.data(), u32(bytes.size())))
            return fail("Application ANM load: " + std::string(bank.first) + ": " + animations.error);
        animations.name_resource(bank.second, bank.first);
        if (bank.second == 2) environment.fallback_sprite_resource = animations.resource(2);
        if (!graphics.preload(*animations.resource(bank.second))) return fail(graphics.error);
    }
    // 对话正文生产接线自检（只有真正可提交时才撤掉 BLOCKED 标记）：
    // text.anm entry0 texture0 必须是 ARGB4444 Blank 且 scripts/sprites 0..3 齐全。
    if (fonts.ready()) {
        const AnmResource* text = animations.resource(0);
        if (!text || text->scripts.size() < 4 || text->sprites.size() < 4 ||
            text->textures.empty() || text->textures[0].format != 5 ||
            text->textures[0].kind == AnmTexture::Kind::External ||
            text->textures[0].kind == AnmTexture::Kind::RenderTarget)
            fonts_failure = "TH13 text.anm texture0 dialogue layout unavailable";
        else
            fonts_failure.clear();
    }
    display = std::make_unique<SceneDisplay>(scheduler, animations, environment, graphics, renderer,
                                             views, captions, 2);
    if (!display->initialize()) return fail(display->error);
    for (u32 index = 0; index < animation_updates.size(); index++) {
        auto& callback = animation_updates[index];
        callback.owner = this;
        callback.enabled = true;
        callback.run = index == 0 ? [](void* p) -> i32 {
                                 return static_cast<ApplicationState*>(p)->animations.update(true) ? 1 : 5;
                             }
                                  : [](void* p) -> i32 {
                                 return static_cast<ApplicationState*>(p)->animations.update(false) ? 1 : 5;
                             };
        if (scheduler.add(callback, FramePass::Update, index == 0 ? 9 : 34) < 0)
            return fail("Application animation callback registration failed");
    }    // Draw gate 2: the loader's ascii script17 fore-pool is still drawn by the
    // native manager for battle frames 0..9 (ascii/loading.png quad+UP with a
    // 10-frame alpha fade, then retired). The port's isolated loading manager
    // has no scene draw callbacks, so submit its layer31 at the layer31
    // schedule slot: after the main manager's layer31 (priority 71) and before
    // the ASCII text space0 pass (priority 78).
    loading_battle_draw.owner = this;
    loading_battle_draw.enabled = true;
    loading_battle_draw.run = [](void* p) -> i32 {
        return static_cast<ApplicationState*>(p)->draw_loading_battle() ? 1 : 5;
    };
    if (scheduler.add(loading_battle_draw, FramePass::Draw, 72) < 0)
        return fail("Loading caption draw callback registration failed");
    initialized = true;
    return begin_title(0);
}

ApplicationState::~ApplicationState() {
    clear_ending_windows();
    title.reset();
    if (ending) {
        ending->teardown();
        ending.reset();
    }
    release_run();
    scheduler.remove(loading_battle_draw);
    for (auto& callback : animation_updates) scheduler.remove(callback);
    display.reset();
    animations.resource_release = nullptr;
}

void ApplicationState::release_run(bool preserve_hud) {
    graphics.presentation.reset();
    pause.reset();
    playfield_targets.reset();
    run.reset();  // ~RunScene 先释放战斗 ANM 子树（外部 VM 归还 registry）
    if (!preserve_hud) {
        run_hud.reset();
        run_player.reset();
    }
    dialogue_text.release(animations);  // ~RunScene 已释放；幂等兜底
    // 对局 bank 卸载（unload 从 banks 摘除并落 retired 队列，供重开再装载）；
    // 常驻 bank（text/ascii/effect）只 retire 活动 VM，资源本体跨场景常驻。
    for (const auto bank : {kFrontBank, kStageBank, kBulletBank, kPlayerBank, kSceneryBank, 26, kStageLogoBank,
                            kSpiritBank})
        if (!preserve_hud || bank != kFrontBank) animations.unload(bank);
    if (!preserve_hud)
        for (const auto bank : {0, 2, 8}) animations.retire_resource(bank);
    // 004172e0's caption slot must not outlive the run. preserve_hud keeps
    // bank 2 loaded, so retire the tree explicitly instead of dropping the
    // handle (an abandoned transition would otherwise keep spawning script16
    // particles in the title/menu).
    animations.retire(transition_prayer);
    transition_prayer = 0;
    // A pending next-stage scenery never materialized (title/quit before the
    // loader gate's transition phase): drop the deferred request so a later
    // stage cannot inherit it.
    transition_scenery_pending = false;
    transition_scenery_bank = -1;
    transition_scenery_std.clear();
    animations.collect_resources();
    for (auto& callback : animation_updates) callback.enabled = true;
    environment.paused = false;
    environment.screen_translation = {};
    environment.background_delta = {};
    environment.screen_offsets = {32, 16, 320, 16};
    environment.playfield_origin = {32, 16};
    views.configure(environment);
    renderer.invalidate();
    // textureFactor 保持（真机 TFACTOR 跨帧持续的设备态，蓝图 §8.8）。
    const auto factor_kept = graphics.backend.pipeline().textureFactor;
    graphics.backend.pipeline() = touhou::graphics::PipelineState{};
    graphics.backend.pipeline().textureFactor = factor_kept;
    if (display) display->detach();
}

bool ApplicationState::preload_scenery_animation(i32 bank, const std::string& name) {
    // STD 引用 ANM 装载（begin_run 场景期专用；release_run 随对局 bank 卸载）。
    std::vector<u8> bytes;
    if (animations.resource(bank)) return true;
    if (!read(name, bytes) || !animations.load(bank, bytes.data(), u32(bytes.size())))
        return fail("Scenery ANM load: " + name + ": " + animations.error);
    animations.name_resource(bank, name);
    if (!graphics.preload(*animations.resource(bank)))
        return fail("Scenery ANM preload: " + name + ": " + graphics.error);
    return true;
}

bool ApplicationState::begin_title(i32 reason) {
    // 0042bfa0 saves before destroying a battle, including Replay. Ending
    // teardown alone (43c1c0 case 10) is not a score save point.
    if (run || ordinary_records_dirty) {
        if (!(run ? files.save_score() : files.save())) return fail(files.error);
        ordinary_records_dirty = false;
    }
    // 4525b0 preserves dc670==2 across Replay playback. Its return opens the
    // same Replay page/row (dc674), including imported User entries.
    if (replay_active && reason == 1) reason = 2;
    // Only a real Practice quit/reselect gets original dc670==4 semantics.
    // Capture replay status before release_run; never repurpose a replay return.
    if (returns_to_practice_selection(progress, reason, replay_active)) reason = 4;
    if (ending) {
        // Ending 场景兜底拆卸（正常路径在 apply_destination 0x10 已做）。
        ending->teardown();
        ending.reset();
    }
    clear_ending_windows();
    // Leaving a run (including a pause opened mid-transition) discards the
    // prepared 0xc stage and any replay restore pointer; otherwise the next
    // normal launch would be mistaken for a replay session.
    stage_transition_active = stage_transition_loading = false;
    stage_startup_age = -1;
    pending_replay_stage = nullptr;
    pending_replay_stage_index = 0;
    title.reset();
    release_run();
    replay_active = false;
    replay_finished = false;
    replay_cursor.reset();
    if (!audio_device.queue_music(2, -1, "th13_00.wav")) return fail(audio_device.error);
    current_destination = i32(Destination::Title);
    title = std::make_unique<TitleScene>(progress, selection, animations, *this, scheduler,
                                         TitleSceneResources{});
    title->state.return_reason = reason;
    if (reason == 2) title->state.replay_index = replay_catalog_index;
    return title->error.empty() || fail(title->error);
}

bool ApplicationState::begin_run() {
    const bool new_run = progress.new_run;  // 关终 0xc 换关为延续对局（非新开）
    const bool preserve_hud = run && !new_run;
    // 0xc continuation keeps the live HUD/banks; a fresh retry (0xa/0xb) is
    // new_run and must take the release_run path even if a transition was
    // pending when the pause menu opened.
    const bool continuing = stage_transition_active && !new_run;
    std::unique_ptr<RunScene> previous_run;
    // 回放会话（0x42b990 恢复序挂点在下方消费 pending_replay_stage，先留存）：
    // 原版回放无 BGM（0x4dc8a8&0x10 时 0x43ae40/0x43af40 的曲命令退化为停曲）。
    const bool replay_session = pending_replay_stage != nullptr;
    if (run) {
        if (!files.save_score()) return fail(files.error);
        ordinary_records_dirty = false;
    }
    title.reset();
    if (continuing) {
        pause.reset();
        previous_run = std::move(run);
        for (i32 bank : {kStageBank, kBulletBank, kSpiritBank, kStageLogoBank}) animations.unload(bank);
        animations.collect_resources();
    } else {
        release_run(preserve_hud);
        next_scenery_bank = 26;
    }
    // 对局数值通道预置（0x42b990:103-110 关首语义子集，DAT_004be7d0/0x4be7dc/0x4be7e4）：
    // 初值表 0x4c0ab0[diff]×100（EXE .data 实读 5000/10000/15000/20000/20000 →
    // 0x4be7e0）；封顶表 0x4c0ac8[diff]×100（100000/250000/500000/500000/500000 →
    // 0x4be7e4，Extra 行 4）。0x42bb8c 起字节实证：db 05 fimul [0x4be7e0]；
    // dd 05 fld qword[0x4ae868]（=100.0）；dc f9 fdiv；d9 5c 24 14/d8 4c 24 14
    // ×100 同值往返；e8 call 0x4971f0（ftol，吃 FPU 浮点——Ghidra 把 0x42b990
    // 读成 float 参数即此误源）→ DAT_004be7dc = 初值 = 0x4c0ab0[diff]×100。
    // Normal 开局点值 10000×100 = 1000000，HUD 显示 /100（0x427910:236
    // 0x403ec0(4be7dc/100-(4be7dc/100)%10)）即屏显 10000 点（stage1-check2.png
    // 实证），非封顶 250000。graze 关首清零（:103）。
    // Six entries at 004c0ab0/004c0ac8, including Overdrive (rank 5).
    static constexpr i32 kPointInitTable[6] = {5000, 10000, 15000, 20000, 20000, 100000};
    static constexpr i32 kPointCapTable[6] = {100000, 250000, 500000, 500000, 500000, 1000000};
    if (!ecl_difficulty_mask(progress.difficulty)) return fail("Invalid run difficulty");
    const u32 difficulty = u32(progress.difficulty);
    progress.max_point_cap = kPointCapTable[difficulty] * 100;
    if (!continuing) {
        progress.max_point = kPointInitTable[difficulty] * 100;
        progress.graze = 0;
    }
    progress.chapter = progress.chapter_frame = 0;
    for (auto& callback : animation_updates) callback.enabled = true;
    if (new_run) {
        static constexpr i32 continues_by_difficulty[] = {3,3,6,9,0,0};
        progress.continue_stock = continues_by_difficulty[difficulty];
        progress.stage = selected_stage;
        progress.starting_stage = selected_stage;
    }
    // 关终 0xc 延续：DAT_004be81c 已由 PostGameFlow 递增，stage 保持；
    // score/残机/灵力（selection_score/player）由 Application 持有、跨关保留
    //（0x42bfa0 的 0xc 分支不清 DAT_004be7c0/资源，与 0xe 续关重置相区分）。
    progress.stage_frame = 0;  // 关首重置（原版 supervisor 重建对局状态）
    progress.scene_flags = 0;
    progress.rate = 1;
    progress.new_run = false;  // 已入对局：下次 begin_run 即换关延续
    replay_active = replay_session;
    const bool cold_replay = pending_replay_stage && !continuing;
    std::vector<u8> bytes;
    if (!animations.resource(kFrontBank) &&
        (!read("front.anm", bytes) ||
         !animations.load(kFrontBank, bytes.data(), u32(bytes.size())) ||
         !graphics.preload(*animations.resource(kFrontBank))))
        return fail("Run front asset: " + animations.error + graphics.error);
    // Stage 场景资产（0x4bb4f0 表按 DAT_004be81c 行取 +0x4 std / +0x8 ecl）：
    // ANIM 名单顺序合并成单一 bank——ECL 302/303/306 的脚本号即该合并表的
    // 全局序号（0x46e0b0/0x46e230 语义）。表行 Stage1-7 = 下标 stage-1
    //（strings 0x4ac28c-0x4ac508 成套 st0N.ecl/.std；表首两行 Extra 段未取）。
    if (progress.stage < 1 || progress.stage > 7)
        return fail("Stage number out of 0x4bb4f0 table range: " +
                     std::to_string(progress.stage));
    char stage_ecl[16];
    std::snprintf(stage_ecl, sizeof stage_ecl, "st%02d.ecl", progress.stage);
    std::vector<u8> ecl;
    if (!read(stage_ecl, ecl)) return fail(std::string("Stage ECL missing: ") + stage_ecl);
    EclResource head;
    if (!head.open(ecl.data(), u32(ecl.size())) || head.animations.empty())
        return fail("Stage ECL ANIM list unavailable");
    AnmResource stage;
    for (const auto& name : head.animations) {
        if (!read(name, bytes)) return fail("Stage ANM missing: " + name);
        if (!stage.append(bytes.data(), u32(bytes.size())))
            return fail("Stage ANM invalid: " + name);
    }
    if (!animations.adopt(kStageBank, std::move(stage)) ||
        !graphics.preload(*animations.resource(kStageBank)))
        return fail("Stage ANM bank: " + animations.error + graphics.error);
    if (!read("bullet.anm", bytes) ||
        !animations.load(kBulletBank, bytes.data(), u32(bytes.size())) ||
        !graphics.preload(*animations.resource(kBulletBank)))
        return fail("Bullet ANM bank: " + animations.error + graphics.error);
    if (!read("astral.anm", bytes) ||
        !animations.load(kSpiritBank, bytes.data(), u32(bytes.size())) ||
        !graphics.preload(*animations.resource(kSpiritBank)))
        return fail("Spirit ANM bank: " + animations.error + graphics.error);
    animations.name_resource(kSpiritBank, "astral.anm");
    const char* player_files[4] = {"pl00.anm", "pl01.anm", "pl02.anm", "pl03.anm"};
    const i32 character = progress.character & 3;
    if (!animations.resource(kPlayerBank) && (!read(player_files[character], bytes) ||
        !animations.load(kPlayerBank, bytes.data(), u32(bytes.size())) ||
        !graphics.preload(*animations.resource(kPlayerBank))))
        return fail("Player ANM bank: " + animations.error + graphics.error);
    // 第 4 员 effect=8（kBanks 表 effect.anm 同值）：ECL 307/308 特效 VM 创建源。
    StagePresentation::Banks banks{kStageBank, kBulletBank, kPlayerBank, 8, kSpiritBank,
                                   kFrontBank};
    // STD 舞台背景资产名（0x4bb4f0 行 +0x4；Stage1 = st01.std → st01wl.anm）。
    char stage_std[16];
    std::snprintf(stage_std, sizeof stage_std, "st%02d.std", progress.stage);
    if (!run_hud)
        run_hud = std::make_unique<GameHud>(animations, progress, selection_player,
                                            selection_score, kFrontBank, 2);
    // 42b990 loads stage-table +0x10's logo separately from ECL ANIM banks.
    // ECL554 creates its script0 at the script's actual trigger frame.
    char stage_logo[20];
    std::snprintf(stage_logo, sizeof stage_logo, "st%02dlogo.anm", progress.stage);
    if (!preload_scenery_animation(kStageLogoBank, stage_logo)) return false;
    run_hud->stage_logo_bank = kStageLogoBank;
    if (!run_player) run_player = std::make_unique<StagePlayerState>();
    run = std::make_unique<RunScene>(progress, selection_player, selection_score, animations,
                                     renderer, environment, *this, scheduler, banks, kFrontBank,
                                     2, kSceneryBank, stage_std, &game_random, run_hud.get(),
                                     run_player.get());
    // 角色主副机体 SHT 选用（0x4abb48 表 + DAT_004be7d4 槽位）：正常开局按
    // 进位选择的原角色选档；th13 副机体不换 SHT（与 th15 shot_type 不同），
    // 只切 progress.character 的高位记录。无进位信息 = 主机体。
    char sht[16];
    std::snprintf(sht, sizeof sht, "pl%02da.sht", character);
    run->battle_sht = sht;  // 移速/弹型源（§7.2；正常开局与回放同源）
    if (cold_replay) {
        pending_replay_stage = nullptr;
        run->restore_after_player_prepare = [this] {
            ReplayRestoreLog log;
            std::string restore_error;
            const ReplayRestoreTarget target{&progress, &selection_player, &selection_score,
                                             &game_random, &visual};
            if (!ReplaySession::restore_stage(replay_file, pending_replay_stage_index,
                    target, log, restore_error, ReplayRestorePhase::Load))
                return fail(restore_error);
            ReplaySession::emit_log(log);
            return true;
        };
    }
    if (previous_run) {
        run->preserve_presentation_from(*previous_run);
        previous_run.reset();
    }
    if (!replay_session) {
        if (new_run && !recorder.start(ReplayRecorder::top_snapshot(progress)))
            return fail("Replay recording initialization failed");
        if (!begin_record_stage()) return false;
    }
    if (!run->initialize(HudEntryContext{0, new_run})) return fail(run->error);
    // 0042c550's activation branch (0x42c881 / 0x42c9fb) -> 004172e0 ->
    // 0046fc30 retires the loading caption tree (root plus spawned
    // script14/15/16 children). The script's label1 is a 10-frame fade, not
    // the teardown; retire the whole tree so it cannot draw into stage 2.
    if (continuing) {
        if (!animations.retire(transition_prayer))
            return fail("Next-stage loading caption release: " + animations.error);
    }
    if (!replay_session) run->record_input = [this](const GameInput& input) {
        if (!recorder.record(input, 0)) fail("Replay input recording failed");
    };
    if (replay_session) {
        int restore_player_first = 1;
        run->playback_input = [this, restore_player=true, restore_player_first]() mutable -> const GameInput* {
            if (replay_finished) return nullptr;

            if(restore_player) {
                std::string error;
                const auto* stage=replay_cursor?replay_cursor->stage():nullptr;
                if(!stage||!ReplaySession::restore_player_start(*stage,run->battle,error)) {
                    fail(error.empty()?"Replay starting stage unavailable":error);return nullptr;
                }
                restore_player=false;
                restore_player_first = 1;
            }
            const u32 recorded = replay_take_input();
            if (replay_finished) return nullptr;
            replay_input.update(recorded);
            if (::th13::damage_probe_enabled)
                std::printf("PB stage=%d f=%d first=%d rec=%u\n", progress.stage,
                            progress.stage_frame, int(restore_player_first), recorded);
            restore_player_first = 0;
            return &replay_input;
        };
    }
    if (!playfield_targets) {
        playfield_targets = std::make_unique<PlayfieldTargets>(scheduler, animations, renderer,
                                                              graphics, views);
        if (!playfield_targets->initialize(0)) return fail(playfield_targets->error);
    }
    pause = std::make_unique<RunPause>(progress, animations, environment, *this, scheduler, kFrontBank);
    if (!pause->prepare()) return fail(pause->error);
    // 关首曲序（stage 表 0x4bb4f0 行 +0xc/+0x10，EXE .data 实读 dump；本任务
    // 核对 artifacts/verification/r3/D）。正常曲 = 行 +0xc（原版经 ECL 指令
    // 0x416a10 播预载槽 0；port 保持关首内联播——p61/62 已过门行为）；Boss 曲
    // = 行 +0x10，按 0x42b990:215 载入预载槽 1（MSG opcode 19 经 0x43ae40(1,·)
    // 播本槽）。回放会话不进音乐（原版 0x4dc8a8&0x10 曲命令退化为停曲）。
    static constexpr const char* kStageMusic[7] = {"th13_01", "th13_04", "th13_07",
                                                   "th13_10", "th13_12", "th13_16",
                                                   "th13_20"};
    static constexpr const char* kStageBossMusic[7] = {"th13_03", "th13_06", "th13_08",
                                                       "th13_11", "th13_15", "th13_17",
                                                       "th13_22"};
    // Replay stages use the same stage/Boss music transitions as a live run.
    // The old port suppressed this branch whenever replay_session was true,
    // leaving replay playback silent after the title handoff.
    if (progress.stage >= 1 && progress.stage <= 7) {
        if (!audio_device.queue_music(2, -1,
                                      std::string(kStageMusic[progress.stage - 1]) + ".wav") ||
            !audio_device.queue_music(1, 1,
                                      std::string(kStageBossMusic[progress.stage - 1]) + ".wav"))
            return fail(audio_device.error);
    }
    // 0x42b990:70-72: entering an ordinary stage marks its played byte, not
    // its clear byte. Record only after successful asset/scene initialization.
    if (records_practice_stage(progress, replay_session) &&
        !files.practice_stage_played(progress.character, progress.difficulty, progress.stage)) {
        const auto previous = files.practice_played;
        files.mark_practice_stage_played(progress.character, progress.difficulty, progress.stage);
        if (!files.save()) {
            files.practice_played = previous;
            return fail(files.error);
        }
    }
    // 0042b990: a fresh GameInf(.74==0), including Spell Practice, records
    // a play start. Stage transitions and replay restoration do not.
    if (new_run && !replay_session && progress.practice_mode == 0) {
        if (!files.record_run_start(progress.character) || !files.save())
            return fail(files.error.empty() ? "Invalid play start record" : files.error);
        ordinary_records_dirty = false;
    }
    return true;
}

bool ApplicationState::begin_record_stage() {
    const auto header = ReplayRecorder::stage_snapshot(progress, selection_player, selection_score,
        game_random.seed(), run_player ? run_player->fixed_x : 0,
        run_player ? run_player->fixed_y : 51200, run_player && run_player->focus);
    return recorder.begin_stage(header) || fail("Replay stage recording failed");
}

bool ApplicationState::prepare_next_stage() {
    if (!run) return fail("Stage transition has no run");
    // 42b990's DAT_004dcc30==0 continuation catches up the record before
    // Replay restoration/loading. The loading gate skips HUD convergence.
    if (progress.high_score < selection_score.score)
        progress.high_score = selection_score.score;
    stage_transition_loading = false;
    stage_transition_active = true;
    stage_startup_age = -1;
    // 0xc 换关门（0x42bfa0）：原版关终 0x42cd40 → 0x42b6d0 在 input 12184
    // 置自机隐藏位、对 option 根 pending=3 并清隐藏计数 player+0x1469c；
    // 本 port 在门首（input 12185）下发同一 label3（一帧代理成立条件见
    // StagePresentation::hide_player_options 注释）。0x442960 隐藏支路在
    // 门内每帧计数，第 30 帧（计数 29，input 12214）对 option 根 pending=1。
    transition_gate_active = true;
    transition_gate_frames = 0;
    progress.scene_flags |= 4u;
    if (!files.save_score()) return fail(files.error);
    run_hud->prepare_stage_assets();
    // 0042bfa0's 0xc branch (0x42bffd/0x42c02e/0x42c061) calls 00417370
    // before 00418b50/00438520 retire the old managers. 00417370 ->
    // 0046f820 creates ASCII script17 in the main ANM manager; the create
    // only performs the root's initial tick, and the scheduler's animation
    // passes (priorities 9/34) advance it later in this same frame.
    if (!transition_prayer) {
        transition_prayer = animations.create(2, 17, -1, 0, {480,392,0});
        if (!transition_prayer) return fail("Next-stage loading caption: " + animations.error);
    }
    char name[16]; std::snprintf(name, sizeof name, "st%02d.std", progress.stage);
    const i32 bank = next_scenery_bank;
    next_scenery_bank = bank == 7 ? 26 : 7;
    // 0x42bfa0's 0xc branch retires the old battle managers on the door frame
    // (0x418b50/0x414180 enemies, 0x412b00 bullets, 0x438520 items) but the
    // next-stage STD scenery is built later by the loader gate. Stage 3's
    // scenery ANM scripts read one visual RNG value at their first primitive
    // tick (st03wl scripts 11-16 = 12 index units; probe:
    // artifacts/round-16/agent-b-loading/probe/scenery_probe). The original
    // consumes those draws at the gate's fade phase, not at the door: native
    // continuous th13_01 shows the door frame (input 27673) carrying only the
    // caption's 24-unit burst, with the extra 6 draws appearing at input
    // 27793 when start_transition fires. Keep the door teardown here and
    // defer the scenery load/initialize (and its RNG consumption) to the
    // ordinary start_transition trigger in step(). RunScene::prepare_transition
    // still owns both halves; this re-uses its teardown via the same public
    // state it mutates.
    for (auto& enemy : run->battle.sim.enemies)
        if (run->battle.sim.retire_sink) run->battle.sim.retire_sink(enemy->serial);
    run->battle.sim.enemies.clear();
    run->battle.sim.bullets.clear();
    run->battle.item_manager.reset_items();
    // 00438520 destroys DAT_004c22a4, the same manager whose +0x14 pool
    // 00438ec0 spawns spirits into; reset_items covers the item pools only.
    for (auto& spirit : run->battle.item_manager.spirits()) spirit = Spirit{};
    run->presentation->release_stage();
    run->transition_wait = true;
    transition_scenery_pending = true;
    transition_scenery_bank = bank;
    transition_scenery_std = name;
    // 0x42b6d0 的 option 根 label3 中断（见上方门首注释）：句柄保留，
    // deferred pending=3 使本帧主链 tick 不前进计时器，卫星出生顺延一帧。
    // 0x42b6d0 已在 clear 帧（msg_stage_clear）下发；门只在尚未下发时兜底。
    if (!run->presentation->options_hidden() &&
        !run->presentation->hide_player_options())
        return fail(run->presentation->error());
    if (replay_active) {
        const i32 index = ReplaySession::stage_index(replay_file, progress.stage);
        if (index < 0) return fail("Replay next stage missing");
        pending_replay_stage_index = u32(index);
        pending_replay_stage = &replay_file.stages[u32(index)];
        // 0x449070（装载器 0042b990 回放分支，DAT_004be830&2）在装载帧的
        // 动画抽取之后触达：连续参考 frame 12185 先做 24 index 单位旧血缘
        // 抽取，随后 gs/vs 重打 stamp、gi 清零。恢复延迟到本帧 step 尾部。
        pending_transition_restore = true;
    } else {
        static constexpr i32 point_init[] = {5000,10000,15000,20000,20000,100000};
        progress.max_point = point_init[progress.difficulty] * 100;
        progress.graze = 0;
        // 448ec0 creates the next recording slot at activation; the old slot
        // continues collecting inputs throughout the HUD/startup gates.
    }
    return true;
}

bool ApplicationState::activate_next_stage() {
    if (replay_active) {
        const ReplayStage& stage = replay_file.stages[pending_replay_stage_index];
        replay_cursor = std::make_unique<ReplayInputCursor>(stage);
        replay_consumed = 0;
        replay_input = {};
        replay_finished = false;
        // The header was restored during loading; do not restore RNG again.
    }
    progress.new_run = false;
    if (!begin_run()) return false;
    pending_replay_stage = nullptr;
    stage_transition_active = false;
    stage_startup_age = -1;
    transition_gate_active = false;
    transition_gate_frames = 0;
    return true;
}

bool ApplicationState::begin_replay_run(const u8* data, size_t size, u32 stage_index) {
    // 回放会话（回放侧 0x448200 的对局直入版）：标题流程全部跳过，难度/角色/
    // 关卡按录像头配置。原版回放侧即真实语义：不补命、不终局记账豁免——
    // 死亡/掉落按对局本体走（harness 观察口径此前靠 8 命底板维持，现按
    // 录像头恢复初值，资格对拍交给 replay 消费端）。
    if (!replay_file.decode(data, size)) return fail("Replay decode: " + replay_file.error());
    if (stage_index >= replay_file.stages.size())
        return fail("Replay stage index out of range");
    return begin_replay_stage(stage_index);
}

bool ApplicationState::begin_replay_stage(u32 stage_index, bool stage_transition) {
    if (stage_index >= replay_file.stages.size())
        return fail("Replay stage index out of range");
    const ReplayStage& stage = replay_file.stages[stage_index];
    replay_cursor = std::make_unique<ReplayInputCursor>(stage);
    replay_consumed = 0;
    replay_input = {};
    replay_finished = false;
    selected_stage = stage.number > 0 ? stage.number : 1;
    progress.difficulty = replay_file.rank;      // 顶层头 +0x64（0x447900:199 先写序）
    progress.character = replay_file.character & 3;  // 顶层头 +0x5C（:197；最终以 stage 头为准）
    // 004525b0: the replay header chooses the mode before stage restoration.
    const bool spell_replay = replay_file.top_header.size() > 10 &&
                              (replay_file.top_header[10] & 2u) != 0;
    progress.run_mode_flags = (progress.run_mode_flags & ~0x30u) |
                              (spell_replay ? 0x20u : 0u);
    // 4525b0 requests scene0xd;43c1c0 constructs42c490(1). Replay uses
    // GameInf+74's non-recording path, including the immediate clear menu.
    progress.practice_mode = 1;
    progress.selected_spell_id = -1;
    if (spell_replay && replay_file.top_header.size() >= 0x74)
        std::memcpy(&progress.selected_spell_id, replay_file.top_header.data() + 0x70, 4);
    // 对局初值：HUD 域清零（对应 0x447900:52-59 新 0xC4 块 memset——22 dword
    // 中没有的 HUD 字段 deaths/mode_flags 归零），随后 begin_run 内的关首
    // 表重置之后按 0x449070 恢复 stage 头 22 dword + 种子（挂点见 begin_run）。
    // 8 命观察底板已撤——回放不再豁免死亡终局。
    selection_player = HudPlayerState{};
    selection_score = HudScoreState{};
    pending_replay_stage = &stage;  // begin_run 恢复挂点（0x42b990 恢复序）
    pending_replay_stage_index = stage_index;
    // 0042bfa0's0xc continuation retains the HUD; title launches and replay
    // restarts still take the fresh-run lifetime path.
    progress.new_run = !stage_transition;
    loading_startup = false;
    loading_debt = 0;
    pending_load = 0;
    pending_destination = -1;
    if (!begin_run()) return false;
    replay_active = true;
    current_destination = i32(Destination::Run);
    return true;
}

u32 ApplicationState::replay_take_input() {
    // 输入流帧号即游戏帧（golden README「时间轴对齐」）：每游戏帧恰取一次；
    // 越界/EOF 后零态（0x448200:14-19 同源）。
    if (!replay_active || replay_finished || !replay_cursor || !replay_cursor->valid()) return 0;
    const ReplayFrame frame = replay_cursor->next();
    if (frame.eof()) {
        replay_finished = true;
        if (!pause || !pause->open(PauseEntrance::PracticeEnd))
            fail(pause ? pause->error : "Replay end has no overlay");
        return 0;
    }
    ++replay_consumed;
    return frame.state;
}

bool ApplicationState::apply_destination() {
    if (pending_destination < 0) return true;
    const auto next = pending_destination;
    pending_destination = -1;
    if (next == i32(Destination::Run)) {
        current_destination = next;
        // [R3-M] 标题 Replay 启动（0x4525b0 case3 → destination 0xd）：消费
        // start_replay 排队的槽位，走生产解码 begin_replay_run（初值/seed 恢复
        // 由 ReplaySession::restore_stage 承担）。正常开局仍走 begin_run。
        if (pending_replay_file >= 0) {
            const i32 file_index = pending_replay_file;
            const i32 stage_index = pending_replay_stage_slot;
            pending_replay_file = pending_replay_stage_slot = -1;
            if (file_index < 0 || file_index >= i32(replay_slots.size()) ||
                !replay_slots[u32(file_index)].decoded)
                return fail("Replay launch slot unavailable: " + std::to_string(file_index));
            const ReplayCatalogSlot& slot = replay_slots[u32(file_index)];
            return begin_replay_run(slot.bytes.data(), slot.bytes.size(), u32(stage_index));
        }
        if (replay_active) return begin_replay_stage(pending_replay_stage_index);
        return begin_run();
    }
    if (next == i32(Destination::NextStage)) {
        current_destination = i32(Destination::Run);
        stage_transition_loading = true;
        return true;
    }
    if (next == i32(Destination::Ending)) {
        // 0x42cd40 records no-continue clears; replay sessions must not persist records.
        if (!replay_active) {
            if (progress.continues == 0)
                files.set_cleared(progress.character & 3,
                                  std::clamp(progress.difficulty, 0, kSaveDifficulties - 1));
            if (progress.difficulty > 0 && progress.continues == 0)
                files.set_extra_unlocked(progress.character & 3);
        }
        // 0042c500 -> 0042bfa0 -> 00449e50 precedes 00416130's new ending bits.
        if (!files.save_score()) return fail(files.error);
        ordinary_records_dirty = false;
        // 0x416570 → 0x416130：Ending 场景（结局索引/落档位/e##.msg/滚屏）。
        current_destination = next;
        release_run();
        ending = std::make_unique<EndingScene>(progress, *this);
        if (!ending->initialize()) return fail(ending->error);
        return initialize_ending_text();
    }
    if (next == i32(Destination::AfterEnding)) {
        // 0x10：Ending 脚本 END（0x416610 置 0x4dcc24=0x10）→ 0x43c1c0
        // case0x10：0x4165f0 → 0x4163b0 拆卸（ANM 槽 20-23/msg/任务）→
        // 目的地 4 + DAT_004dc670=3（标题直入菜单态 10，跳过常驻入场）。
        // begin_title 变体 3 与原版 dc670=3 对齐（现有 reason 3 的 GameOver
        // 语义同走直入菜单路径，行为面等价）。
        if (ending) {
            ending->teardown();
            ending.reset();
        }
        return begin_title(3);
    }
    if (next == i32(Destination::Title) || next == i32(Destination::TitleFromGameOver) ||
        next == i32(Destination::TitleFromReplay)) {
        return begin_title(next == i32(Destination::TitleFromGameOver) ? 3 : 1);
    }
    if (next == i32(Destination::Quit)) {
        return quit();
    }
    return fail("Unrecognized application destination: " + std::to_string(next));
}

bool ApplicationState::quit() {
    if (exiting) return true;
    // 0043a390 -> 004399e0 saves the shared score, including ending bits.
    if (!files.save_score()) return fail(files.error);
    ordinary_records_dirty = false;
    exiting = true;
    return true;
}

bool ApplicationState::complete_loading() {
    if (pending_load) {
        const unsigned task = pending_load;
        pending_load = 0;
        if (task == 1) {
            if (!apply_destination()) return false;
        }
    }
    return true;
}

bool ApplicationState::step(const GameInput& input, float fps, bool lost_focus) {
    if (!initialized || exiting || !failure.empty()) return false;
    if (!complete_loading()) return false;
    bool loaded_transition = false;
    if (stage_transition_loading) {
        if (!prepare_next_stage()) return false;
        loaded_transition = true;
    }
    // 门内第 30 帧（门首=0，input 12214）：0x442960 隐藏支路 counter>0x1d，
    // 对每个 option 根（0xa368/0xa36c 两组）置 pending=1 并清槽；每帧
    // counter>0x1d 都会重入，故用 >= 且 interrupt_player_options 幂等。
    // input 12210 的卫星已出生并绘制，12214 的出生被拦下，根按
    // label1(t=4) → op1(t=8) 于 input 12218 回收。
    // 0x442960 属原生 player update：environment.paused 或 scene_flags&0x70
    // 的覆盖层帧不推进 player+0x1469c、也不触发 interrupt。门计数与触发
    // 条件统一走 transition_gate_player_update_runs（原生可测函数），避免
    // 门内暂停 >=30 帧让 port 在暂停中提前清掉 option 树。
    const bool gate_player_update =
        transition_gate_player_update_runs(environment.paused, progress.scene_flags);
    if (transition_gate_active && stage_transition_active && !loaded_transition &&
        transition_gate_should_interrupt(transition_gate_frames, environment.paused,
                                         progress.scene_flags) &&
        run && run->presentation) {
        if (!run->presentation->interrupt_player_options())
            return fail(run->presentation->error());
    }
    if (stage_transition_active && !loaded_transition &&
        !environment.paused && !(progress.scene_flags & 0x70u)) {
        progress.scene_flags &= ~4u;
        if (stage_startup_age < 0) {
            if (!(run->hud.flags & 0x100u) || run->hud.clear_age.current >= 120) {
                // Loader gate reached its transition phase: build the next STD
                // scenery here (ordinary start_transition trigger), matching
                // the original where the scenery ANM first ticks land on this
                // frame rather than the 0042bfa0 door (see prepare_next_stage).
                if (transition_scenery_pending) {
                    if (!run->prepare_transition(transition_scenery_bank,
                                                 transition_scenery_std))
                        return fail(run->error);
                    transition_scenery_pending = false;
                }
                if (!run->start_transition()) return false;
                stage_startup_age = 0;
            }
        } else if (stage_startup_age == 30) {
            // [妖梦 S2 相位取证] age==30 与原生同拍：ACT 帧打印（e24）实证
            // activate_next_stage 发生在 step=12343（boot 偏移 8 → input
            // 12335，S2 首帧），S2 两个 option 根首次 walk 入口 timer=1
            // （e18/e27 chain-walk label0），与原生一致。曾试 age==31 延迟
            // 一帧（e19/e21），反而把首次 walk 推到 timer=2（相位错位），
            // 已回滚；26792 的 −1 漂移发生在 S2 中段（12335..26695），
            // 与本激活帧无关。
            if (!activate_next_stage()) return false;
        }
        if (stage_transition_active && stage_startup_age >= 0) ++stage_startup_age;
    }
    // 门内帧计数（prepare_next_stage 当帧=0）：只在原生 player update 会跑
    // 的帧推进；暂停/覆盖层帧冻结计数。activate_next_stage 清零。
    if (transition_gate_active && stage_transition_active && gate_player_update)
        ++transition_gate_frames;
    if (title) {
        title->controls({input.held, input.pressed, input.repeated, 0});
        if (scheduler.update() < 0)
            return fail(title->error.empty() ? title->frame.error.empty() ? animations.error
                                                                           : title->frame.error
                                                : title->error);
    } else if (run) {
        if (pause) {
            if (pause->active()) {
                pause->controls({input.pressed, input.repeated, lost_focus});
            } else if (input.pressed & 0x100) {  // Esc：暂停覆盖层
                if (!pause->open(PauseEntrance::Pause)) return fail(pause->error);
            } else if (lost_focus) {
                if (!pause->open(PauseEntrance::Pause)) return fail(pause->error);
            }
        }
        const i32 previous_stage_frame = progress.stage_frame;
        run->controls(input);  // 战斗输入（Z 射击/X 炸弹/方向）：唯一实例直通 StageBattle
        if (scheduler.update() < 0)
            return fail(run->error.empty() ? animations.error : run->error);
        // 0042c550: every active non-Replay simulation tick, any play mode.
        if (!replay_active && progress.stage_frame != previous_stage_frame) {
            if (!files.record_play_frame(progress.character))
                return fail("Invalid play time record");
            ordinary_records_dirty = true;
        }
    } else if (ending) {
        // Ending 场景帧（0x416610 → 0x416a10）：物理输入驱动对话推进/快进
        //（0x4e49f0/0x4e49fc/0x4e4b04 语义见 EndingScene）；动画调度器照常
        // 推进（op7/op8 cue 的 ANM VM 属同一 registry）。
        if (!ending->update(input)) return fail(ending->error);
        if (scheduler.update() < 0) return fail(animations.error);
        if (!graphics.select_target(nullptr, 0) ||
            !graphics.clear_target(0xff000000, nullptr))
            return fail(graphics.error);
    } else {
        return fail("Application has no active scene");
    }
    // 换关装载帧的 0x449070 恢复点：本帧全部场景/动画更新（含旧血缘视觉
    // 抽取）已完成、下一帧消费之前，执行种子/计数/22 dword 恢复。
    if (pending_transition_restore) {
        pending_transition_restore = false;
        ReplayRestoreLog log;
        std::string error;
        if (!ReplaySession::restore_stage(replay_file, pending_replay_stage_index,
            {&progress, &selection_player, &selection_score, &game_random, &visual}, log, error))
            return fail(error);
    }
    ending_fades.erase(std::remove_if(ending_fades.begin(), ending_fades.end(),
        [this](const auto& fade) { return !scheduler.contains(fade->update); }), ending_fades.end());
    audio_device.update();
    if (!audio_device.error.empty()) return fail(audio_device.error);
    frames++;
    if (run) {
        HudTextStyle style;
        style.font = 1;
        style.color = fps < 30 ? 0xff5050ffu : fps < 40 ? 0xffa0a0ffu : 0xffffffffu;
        char text[32];
        std::snprintf(text, sizeof text, "%2.1ffps", double(fps) + .05);
        if (!captions.enqueue({{588,470,0}, style, text})) return fail(captions.error);
    }
    if (pause && !pause->draw_overlay(static_cast<RunSceneServices&>(*this))) return fail(pause->error);
    if (!display->draw()) return fail(display->error);
    if (pending_destination == i32(Destination::Run) || pending_destination == i32(Destination::Ending)) {
        if (!draw_loading(false)) return false;
        pending_load = 1;
        return true;
    }
    return apply_destination() && (graphics.error.empty() || fail(graphics.error));
}

const i32* ApplicationState::projection() {
    projected.fill(0);
    projected[0] = title ? 0 : run ? 1 : ending ? 2 : -1;
    projected[1] = current_destination;
    projected[2] = progress.stage;
    projected[3] = progress.difficulty;
    projected[4] = progress.character;
    projected[5] = progress.stage_frame;
    projected[6] = i32(progress.scene_flags);
    projected[7] = i32(frames);
    if (title) {
        projected[8] = i32(title->state.screen);
        projected[9] = title->state.substate;
        projected[10] = title->state.menu.cursor;
        projected[11] = title->state.menu.count;
        projected[12] = i32(selection_player.mode_flags);
    } else if (run) {
        projected[8] = selection_score.score;
        projected[9] = selection_player.extra_lives;
        projected[10] = selection_player.bombs;
        projected[11] = selection_player.power;
        projected[12] = i32(selection_player.mode_flags);
        projected[13] = selection_player.deaths;
        projected[14] = pause ? i32(pause->state.screen) : 0;
    } else if (ending) {
        // Ending 场景观测面：结局索引（0x4bba20 序）/滚屏行/帧计数。
        projected[8] = ending->ending_index();
        projected[9] = ending->line();
        projected[10] = ending->frames();
        projected[11] = i32(ending->scene_flags());
    }
    // 字体设备显式失败面（非零 = 对话日文渲染 BLOCKED；见 initialize 注释）。
    projected[15] = fonts_failure.empty() ? 0 : 1;
    return projected.data();
}

// ---- EndingPlatform（0x416570 场景宿主面）----
bool ApplicationState::fade_out_music(i32 frames) {
    // op11 0x43af70(3.0f) → 生产音乐命令 5（淡出 value*60 帧）。
    return audio_device.queue_music(5, frames / 60, "") || fail(audio_device.error);
}

bool ApplicationState::record_music_unlocked(i32 index) {
    if (index < 0 || index >= 32) return fail("Ending music index out of range");
    files.set_music_heard(index);
    return files.save() || fail(files.error);
}

bool ApplicationState::advance_sound() {
    // op5/6 计声放行 0x460bc0(0)：同 MsgPlayer opcode 11 的 voice 0 约定。
    return sound(0);
}

bool ApplicationState::load_ending_anm(i32 slot, const std::string& name) {
    // op7：e##.anm/staff.anm 装载到 ANM 槽 0x14+idx（20..23；0x417230 回调
    // 0x46df40 的同步等价）。资产经统一 read 通道（/assets + 缓存）。
    std::vector<u8> bytes;
    if (!read(name, bytes)) return false;
    if (animations.resource(slot)) animations.unload(slot);
    if (!animations.load(slot, bytes.data(), u32(bytes.size())))
        return fail("Ending ANM load: " + name + ": " + animations.error);
    animations.name_resource(slot, name);
    if (!graphics.preload(*animations.resource(slot)))
        return fail("Ending ANM preload: " + name + ": " + graphics.error);
    return true;
}

bool ApplicationState::cue_ending_anm(i32 vm_slot, i32 bank_slot, i32 script) {
    if (vm_slot < 0 || vm_slot >= i32(ending_anm_vms.size()))
        return fail("Ending ANM VM slot out of range");
    u32& vm = ending_anm_vms[u32(vm_slot)];
    if (!animations.retire(vm)) return fail(animations.error);
    vm = animations.create(bank_slot, script, -1, 0);
    if (!vm) return fail("Ending ANM cue failed: " + animations.error);
    return true;
}

void ApplicationState::release_ending_anm(i32 slot) {
    // 0x4163b0：ANM 管理器槽 20-23 反装载（0x46e620）。
    animations.unload(slot);
    if (slot == 0x17) {
        for (u32& vm : ending_anm_vms) animations.retire(vm);
    }
}

bool ApplicationState::initialize_ending_text() {
    for (u32 row = 0; row < ending_text_vms.size(); ++row) {
        if (animations.registry.find(ending_text_vms[row])) continue;
        ending_text_vms[row] = animations.create(0, i32(68 + row), -1, 0);
        if (!ending_text_vms[row]) {
            release_ending_page();
            return fail("Ending text VM creation: " + animations.error);
        }
        AnmVm* vm = animations.registry.find(ending_text_vms[row]);
        if (!vm) {
            release_ending_page();
            return fail("Ending text VM unavailable");
        }
        vm->visual.render_flags |= 0x1000;
    }
    return true;
}

void ApplicationState::release_ending_page() {
    for (u32& vm : ending_text_vms) animations.retire(vm);
}

bool ApplicationState::ending_page_reset() {
    if (!initialize_ending_text()) return false;
    for (u32 row = 0; row < ending_text_vms.size(); ++row)
        if (!ending_page_line(i32(row), 0x00ffffff, " ")) return false;
    return ending_page_clear();
}

bool ApplicationState::ending_page_line(i32 row, u32 color, const std::string& cp932) {
    if (row < 0 || row >= i32(ending_text_vms.size()))
        return fail("Ending text row out of range");
    AnmVm* vm = animations.registry.find(ending_text_vms[u32(row)]);
    if (!vm || !vm->resource || !vm->sprite)
        return fail("Ending text VM unavailable");
    const AnmSprite& sprite = *vm->sprite;
    const u32 handle = graphics.texture(*vm->resource, sprite.texture);
    TextureImage* target = handle ? graphics.pixels(handle) : nullptr;
    if (!target || target->format != touhou::graphics::PixelFormat::Argb4444 ||
        target->pitch < target->width * 2 ||
        target->pixels.size() < size_t(target->pitch) * target->height ||
        sprite.x < 0 || sprite.y < 0 || sprite.width != 512 || sprite.height != 20 ||
        sprite.x + sprite.width > float(target->width) ||
        sprite.y + sprite.height > float(target->height))
        return fail("Ending text texture unavailable");
    TextureImage scratch;
    scratch.width = 1024;
    scratch.height = 40;
    scratch.pitch = 2048;
    scratch.packed_format = 5;
    scratch.format = touhou::graphics::PixelFormat::Argb4444;
    scratch.pixels.assign(size_t(scratch.pitch) * scratch.height, 0);
    if (!fonts.draw_text(scratch, 0, 2, 0, color, cp932))
        return fail(fonts.error);
    const u32 x = u32(sprite.x), y = u32(sprite.y);
    for (u32 py = 0; py < 20; ++py) {
        u8* destination = target->pixels.data() + size_t(y + py) * target->pitch + size_t(x) * 2;
        const float center_y = (float(py) + .5f) * 1.8f - .5f;
        for (u32 px = 0; px < 512; ++px) {
            const float center_x = (float(px) + .5f) * 2.f - .5f;
            float sum[4] = {}, total = 0;
            for (i32 sy = i32(std::ceil(center_y - 1.8f));
                 sy <= i32(std::floor(center_y + 1.8f)); ++sy) {
                const float wy = std::max(0.f, 1.f - std::fabs(float(sy) - center_y) / 1.8f);
                for (i32 sx = i32(std::ceil(center_x - 2.f));
                     sx <= i32(std::floor(center_x + 2.f)); ++sx) {
                    const float weight = wy * std::max(0.f, 1.f - std::fabs(float(sx) - center_x) / 2.f);
                    total += weight;
                    if (sx < 0 || sx >= 1024 || sy < 0 || sy >= 36) continue;
                    u16 pixel = 0;
                    std::memcpy(&pixel, scratch.pixels.data() + size_t(sy) * scratch.pitch + size_t(sx) * 2, 2);
                    for (u32 c = 0; c < 4; ++c) sum[c] += float((pixel >> (12 - c * 4)) & 15u) * weight;
                }
            }
            u16 pixel = 0;
            for (float component : sum)
                pixel = u16((pixel << 4) | u16(std::clamp(i32(std::floor(component / total + .5f)), 0, 15)));
            std::memcpy(destination + px * 2, &pixel, 2);
        }
    }
    graphics.changed(handle, {x, y, x + 512, y + 20});
    return animations.interrupt(ending_text_vms[u32(row)], 2, false) || fail(animations.error);
}

bool ApplicationState::ending_page_clear() {
    for (u32 vm : ending_text_vms)
        if (!animations.interrupt(vm, 3, false)) return fail(animations.error);
    return true;
}

bool ApplicationState::request_stage6_fade() {
    return open_game_fade(false, 200, false);
}

bool ApplicationState::open_game_fade(bool entering, i32 duration, bool playfield_only) {
    if (!open_ending_window(entering ? 0 : 5, duration)) return false;
    auto& fade = *ending_fades.back();
    fade.playfield_only = playfield_only;
    if (playfield_only) {
        const auto draw_function = fade.draw.run;
        scheduler.remove(fade.draw);
        fade.draw.run = draw_function;
        if (scheduler.add(fade.draw, FramePass::Draw, 11) < 0) return fail("Game fade registration failed");
    }
    return true;
}


bool ApplicationState::open_ending_window(i32 kind, i32 duration) {
    if ((kind != 0 && kind != 5) || duration < 0)
        return fail("Ending fade parameters invalid");
    auto fade = std::make_unique<EndingFade>();
    fade->owner = this;
    fade->kind = kind;
    fade->duration = duration;
    fade->alpha = kind == 0 ? 255 : 0;
    auto* item = fade.get();
    item->update.owner = item;
    item->update.enabled = true;
    item->update.run = [](void* owner) -> i32 {
        auto& fade = *static_cast<EndingFade*>(owner);
        if (fade.owner->environment.paused) return i32(FrameAction::Continue);
        if (fade.kind == 5) {
            if (fade.duration)
                fade.alpha = fade.time >= fade.duration ? 255 :
                    i32(double(fade.time) * 255.0 / double(fade.duration));
            if (fade.time >= fade.duration + 2) {
                fade.owner->scheduler.remove(fade.draw);
                return i32(FrameAction::Remove);
            }
        } else {
            if (fade.duration)
                fade.alpha = std::max(0, i32(255.0 - double(fade.time) * 255.0 /
                                                           double(fade.duration)));
            if (fade.time >= fade.duration) {
                fade.owner->scheduler.remove(fade.draw);
                return i32(FrameAction::Remove);
            }
        }
        // 45f430 holds its timer on GameInf bits0/2; 45ef50 does not.
        if (fade.kind != 5 || !(fade.owner->progress.scene_flags & 5u)) ++fade.time;
        return i32(FrameAction::Continue);
    };
    item->draw.owner = item;
    item->draw.enabled = true;
    item->draw.run = [](void* owner) -> i32 {
        auto& fade = *static_cast<EndingFade*>(owner);
        auto& app = *fade.owner;
        app.renderer.invalidate();
        app.renderer.set_viewport({0, 0, 640, 480, 0, 1});
        auto& pipeline = app.graphics.pipeline();
        pipeline.blend = true;
        pipeline.sourceBlend = touhou::graphics::BlendFactor::SourceAlpha;
        pipeline.destinationBlend = touhou::graphics::BlendFactor::InverseSourceAlpha;
        pipeline.blendEquation = touhou::graphics::BlendEquation::Add;
        pipeline.depthWrite = false;
        pipeline.color.operation = pipeline.alpha.operation = touhou::graphics::ColorOperation::First;
        pipeline.color.first = pipeline.alpha.first = {touhou::graphics::ArgumentSource::Diffuse};
        app.graphics.set_layout(touhou::graphics::VertexLayout::ScreenColor);
        struct Vertex { Vec3 position; float reciprocal_w; u32 color; };
        const u32 color = u32(fade.alpha) << 24;
        const float x0 = fade.playfield_only ? 128.f : 0.f;
        const float x1 = fade.playfield_only ? 512.f : 640.f;
        const float y0 = fade.playfield_only ? 16.f : 0.f;
        const float y1 = fade.playfield_only ? 464.f : 480.f;
        const Vertex quad[] = {{{x0, y0, 0}, 1, color}, {{x1, y0, 0}, 1, color},
                               {{x0, y1, 0}, 1, color}, {{x1, y1, 0}, 1, color}};
        app.graphics.primitives(touhou::graphics::Topology::Strip, 2, quad, sizeof(Vertex));
        app.graphics.flush();
        pipeline.color.operation = pipeline.alpha.operation = touhou::graphics::ColorOperation::Multiply;
        pipeline.color.first = pipeline.alpha.first = {touhou::graphics::ArgumentSource::Texture};
        pipeline.color.second = pipeline.alpha.second = {touhou::graphics::ArgumentSource::Diffuse};
        return i32(FrameAction::Continue);
    };
    if (scheduler.add(item->update, FramePass::Update, 16) < 0 ||
        scheduler.add(item->draw, FramePass::Draw, 75) < 0) {
        scheduler.remove(item->update);
        scheduler.remove(item->draw);
        return fail("Ending fade registration failed");
    }
    ending_fades.push_back(std::move(fade));
    return true;
}

void ApplicationState::clear_ending_windows() {
    for (auto& fade : ending_fades) {
        scheduler.remove(fade->update);
        scheduler.remove(fade->draw);
    }
    ending_fades.clear();
}

}  // namespace th13::sdl
