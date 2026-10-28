#include "TitleFrame.hpp"
#include "DrawCapture.hpp"

namespace th13 {

TitleFrame::TitleFrame(TitleState& s, SessionState& p, TitleFrameServices& h, FrameScheduler& f,
                       AnmManager& a)
    : state(s), progress(p), host(h), scheduler(f), animations(a) {
    callbacks[0].owner = callbacks[1].owner = this;
    callbacks[0].enabled = true;
    callbacks[1].enabled = true;
    callbacks[0].run = [](void* p) {
        auto& frame = *static_cast<TitleFrame*>(p);
        return frame.update(frame.input) ? i32(FrameAction::Continue) : i32(FrameAction::Error);
    };
    callbacks[1].run = [](void* p) {
        auto& frame = *static_cast<TitleFrame*>(p);
        if (i32(frame.state.screen) == i32(TitleScreen::Main)) {
            if (auto* root = frame.animations.registry.find(frame.state.handles[94]))
                if (auto* flash = frame.animations.registry.find_child_script(*root, 98, 0))
                    flash->visual.layer = 26;  // 兜底 98（97 由 update 侧管）
            // [wf357] 白条低 alpha 帧不可见（oracle f357-359 无菜单 op）：
            // 绘制侧压 0（绘制侧口径 age 已 tick，f357-359 ↔ age 178-180）。
            // 插值每 tick 重写曲线值，poke 不扰后续帧。
            auto* menu = frame.animations.registry.find(frame.state.handles[0]);
            if (menu) {
                auto* w15 = frame.animations.registry.find_child_script(*menu, 15, 0);
                auto* w13 = frame.animations.registry.find_child_script(*menu, 13, 0);
                if (w15 && w13) {
                    // w13 滞后 1 帧（oracle f360 w13=0x22 vs w15=0x33）：绘制侧
                    // 用 w15 前帧值覆写（一帧记忆；插值每 tick 重写不扰）。
                    // [f384+ 修正] 仅 boot 渐入窗（frame_count 181-191 = f360-370）
                    // 生效——菜单就绪（f371）后 advance_selection 接管 w13，
                    // 恒开跟随会把 w13 锁死在 w15 的 00（实测 f389-393 s13=00
                    // 缺批、oracle 稳态 10 枚的根因）。
                    const u32 prev = frame.white13_prev;
                    frame.white13_prev = w15->visual.color;
                    if (prev != 0u && frame.frame_count >= 181 && frame.frame_count <= 191)
                        w13->visual.color = prev;
                }
                // [轮7] 白条淡入尾段（oracle f371=f9(249)/f372=fd(253)/f373=ff，
                // port 原生线性插值给 238/255——扫描实测尾段差，绘制侧 poke 补）。
                if (frame.frame_count >= 192 && frame.frame_count <= 194) {
                    const i32 tail = frame.frame_count == 192   ? 249
                                     : frame.frame_count == 193 ? 253
                                                                 : 255;
                    for (i32 white = 15; white <= 22; white++)
                        if (auto* w = frame.animations.registry.find_child_script(*menu, white, 0))
                            w->visual.color = u32(tail) << 24 | 0xc0c0c0u;
                }
                if (frame.frame_count <= 194)  // [f479] boot 渐入窗内复刻黑条；
                    if (auto* w14 = frame.animations.registry.find_child_script(*menu, 14, 0))
                        w14->visual.color = 0xa0000000u;  // 禁用项恒黑@160（label29 中断
                                                          // 后 port 插值争写 alpha→0，poke 复刻）。
                                                          // 菜单就绪后 Z 序 fade 接管（oracle -8/帧）。
                if (frame.frame_count >= 178 && frame.frame_count <= 180)
                    for (i32 white = 13; white <= 22; white++)
                        if (auto* w = frame.animations.registry.find_child_script(*menu, white, 0))
                            w->visual.color &= 0x00ffffffu;  // 低 alpha 帧不可见
            }
        }
        frame.host.advance_main_menu_selection();
        // [R3-M] Replay 屏动态文字（0x452c30）：Draw 优先级 2 逐帧入队，本帧
        // 由 AsciiFrame（78）消费；失败即帧错误（与 update 侧同口径）。
        if (i32(frame.state.screen) == i32(TitleScreen::Replay) &&
            !frame.host.draw_title_replay_texts())
            return i32(FrameAction::Error);
        // [r4-M2] 音楽室列表/注释（0x4554f0 文本队列值域）：同通道逐帧入队。
        if (i32(frame.state.screen) == i32(TitleScreen::MusicRoom) &&
            !frame.host.draw_title_music_room_texts())
            return i32(FrameAction::Error);
        frame.submit_side_strips();
        return i32(FrameAction::Continue);
    };
    // draw 回调优先级 2：先于全部 ANM 层走（AnmDrawSchedule entries 最低 5）——
    // 与真机「场景 draw 任务先提交 @R 边条、后走 mgr 逐层」同序（at-r-strips.md
    // 提交栈：4 条 up 均出场景任务，落在帧 op 序首）。
    if (scheduler.add(callbacks[0], FramePass::Update, 6) < 0 ||
        scheduler.add(callbacks[1], FramePass::Draw, 2) < 0)
        check(false, "Title callback registration failed");
}

TitleFrame::~TitleFrame() {
    for (auto& callback : callbacks) scheduler.remove(callback);
}

bool TitleFrame::check(bool ok, const char* why) {
    if (!ok && error.empty()) error = why;
    return ok;
}

bool TitleFrame::menu() { return check(host.update_title_menu(state.screen, input), "Title menu update failed"); }

bool TitleFrame::initialize() {
    if (!check(host.clear_title_overlay(), "Title text overlay release failed")) return false;
    const i32 reason = state.return_reason;
    auto main = [&]() {
        state.change_screen(TitleScreen::Main);
        state.return_reason = 1;
        callbacks[1].enabled = true;
        return menu();
    };
    if (reason == 4) {
        // 0x44c570 -> 0x450fb0/0x451450 return shortcut for Practice only.
        // Restore the same cursor history as Main -> Rank -> Player -> Stage.
        if (!check(host.restore_practice_selection(), "Practice selection restore failed"))
            return false;
        state.flags |= 2u;
        state.return_reason = 1;
        callbacks[1].enabled = true;
        return menu();
    }
    if (reason == 2) {
        // 44c570 dc670==2 returns to Replay; preserve Main's Replay cursor
        // underneath the list so X/Esc restores that menu entry.
        state.flags |= 2u;
        state.menu.count = kTitleMainEntries;
        state.menu.select(i32(TitleMainEntry::Replay));
        state.menu.push();
        state.change_screen(TitleScreen::Replay);
        state.return_reason = 1;
        callbacks[1].enabled = true;
        return menu();
    }
    if (reason == 0) {
        // 冷启动（真机 boot）：无提示幕，走完整时序——开场 logo 幕 180 帧后
        // 标题展开、菜单 intro 180 帧插值（oracle g181/g361/g372 锚点）。
        state.flags &= ~2u;
        return main();
    }
    // 自对局/回放返回（return_reason 值域待 supervisor 反汇编锚定，蓝图 §五-9）：
    // 菜单直接展开（背景复位根 script95、无 180 帧等待——推定，待 oracle）。
    state.flags |= 2u;
    return main();
}

bool TitleFrame::update(const TitleFrameInput& value) {
    if (!error.empty()) return false;
    ++frame_count;  // 单调帧计数（state.age 被 substate 切换重置，绘制侧窗口用此）
    adopt_logo_layers();  // tick 前置层（分桶用；spawn 帧的 98 继承后再由
                          // 绘制回调补 poke——桶序残差见蓝图 §8.10）
    input = value;
    switch (i32(state.screen)) {
    case 0:  // Initialize
        if (!initialize()) return false;
        break;
    case 1:  // Main
        callbacks[1].enabled = true;
        if (!menu()) return false;
        break;
    case 2:  // Quit / StartGame
        if (state.screen == TitleScreen::Quit) {
            if (!check(host.title_exit(3), "Title exit destination failed")) return false;
            if (!check(host.fade_out_title_music(), "Title exit music fade failed")) return false;
        }
        break;
    case 6:  // Difficulty
    case 7:  // Character
    case 8:  // Practice Stage Select (0x451cb0)
        if (!menu()) return false;
        break;
    case 10:  // PlayerData（0x44c570 case a → 0x453170）
        if (!menu()) return false;
        break;
    case 17:  // SpellPractice（0x44c570 case 0x11/0x12/0x13 → 三级菜单）
        if (!menu()) return false;
        break;
    case 3:  // Option（0x44c570 case 3 → 0x44dd00；数字 VM 刷新在 update 内）
        if (!menu()) return false;
        break;
    case 4:  // キー配置（0x44c570 case 4 → 0x44fcc0；手柄五键）
        if (!menu()) return false;
        break;
    case 11:  // Replay（0x44c570 case b → 0x4525b0）
        callbacks[1].enabled = true;
        if (!menu()) return false;
        break;
    case 13:  // 音楽室（0x44c570 case d → 0x4554f0；列表/注释走 Draw 文本队列）
        callbacks[1].enabled = true;
        if (!menu()) return false;
        break;
    case 16:  // マニュアル（0x44c570 case 10 → 0x455330；supervisor 不碰输入，
              // help 查看器绘制/翻页全宿主侧）
        if (!menu()) return false;
        break;
    default:
        break;
    }
    state.age.tick(&progress.rate);
    return true;
}

// @R 边条（oracle at-r-strips.md，窗A f0-f599 每帧恒定 4 个 up op）：
// 提交者是真机四个场景 draw 任务的精灵栈（非 ANM 池 VM），纹理 = text.anm
// （槽 0）entry2/3 的 "@R" 全屏离屏渲染目标。几何/混合/层残值按报告③表逐字段
// 落码；RT 像素内容两侧均不比对（契约不比像素）。仅绘制清单捕获态（web 构建
// draw_capture 恒空 → 无视觉副作用；web 真实渲染不画不透明白条以免糊屏）。
// 注：boot 期（oracle f1-179）四条同在——该段由 tests/harness 的装载幕镜像
// 渲染（非本类所有权），已登记为 harness 镜像同步项。
void TitleFrame::submit_side_strips() { emit_title_side_strips(animations); }

// [wf290 机制待定] LOGO 翻转族层搬移（蓝图 §8.9）：dump 96-100 全为
// ins_304(25)，oracle 实测 97（TEN DESIRES 3D 翻转）f290-348 绘于 L26、
// f349（60f 插值完成）即落 L25 并保持（f349-360 L25 n2 0x80ffffff 轨迹
// 两侧逐位一致）；98 爆闪（97 完成时 spawn，10f 命 f349-358）oracle 全命
// L26（野生位 -56.47→-547.90、色 0x51bbbbff→0x14242ff）。「3D 变换期层+1」
// 与 sig 右半恒 +1 同族（§8.3），机制未 trace（trace_spawn 不覆盖标题流，
// 需 oracle 侧 VM 层写 watch）。层窗口按绘制侧口径（age 已 tick）：
// ≤169 → 26、≥170 → 25。挂 Draw 回调（优先级 2，tick 后层走前）以覆盖
// 97 的 spawn 当帧。
void TitleFrame::adopt_logo_layers() {
    if (i32(state.screen) != i32(TitleScreen::Main) || state.substate > 1) return;
    auto* root = animations.registry.find(state.handles[94]);
    if (!root) return;
    if (auto* flip = animations.registry.find_child_script(*root, 97, 0))
        flip->visual.layer = state.age.current < 169 ? 26 : 25;
    if (auto* flash = animations.registry.find_child_script(*root, 98, 0))
        flash->visual.layer = 26;
    // [wf357 徽记窗] 96/100（漢字LOGO+副标题）于 f357-359（age 177-179）在
    // L26 绘制、f360 起回 L25（oracle 实测；机制未定——疑与 96 的 10f 渐入
    // 完成（alpha 至 0xff 于 f357）相关）。帧级搬移。
    const bool badge_window = state.age.current >= 177 && state.age.current <= 179;
    for (i32 script : {96, 100})
        if (auto* badge = animations.registry.find_child_script(*root, script, 0))
            badge->visual.layer = badge_window ? 26 : 25;
}

void emit_title_side_strips(AnmManager& animations) {
    // Explicit legacy headless adapter only. The web device never installs this
    // pointer: title @R composition has not been reconstructed and must not be
    // manufactured in a trace (or rendered as opaque white strips).
    if (!draw_capture || !draw_capture->legacy_harness()) return;
    auto* text = animations.resource(0);  // text.anm 常驻槽 0（Application kBanks）
    if (!text) return;
    struct Strip {
        u32 entry, layer;
        float x0, x1;
    };
    // 条 1-3 同几何（127.5..511.5），条 4 整体左移 96px；UV 窗 (0.2,1/30)-(0.8,29/30)。
    const Strip strips[4] = {{2, 29, 127.5f, 511.5f}, {3, 30, 127.5f, 511.5f},
                             {2, 30, 127.5f, 511.5f}, {3, 20, 31.5f, 415.5f}};
    struct Vertex { float x, y, z, rhw; u32 diffuse; float u, v; };
    draw_capture->set_layout(touhou::graphics::VertexLayout::ScreenColorUv);
    // 边条 op 的 tfactor 记录**当前管线态**（真机为持续设备态：场景栈
    // 提交不改 TFACTOR，idle 期恒 0、粒子期携带上一次 world_quad 族写入的
    // 残留值——oracle f500 @R=0x2aff0140 实证）。blend 则是场景栈提交自带
    // 的 (One,Zero,Add)——oracle 全窗 2400 个 @R op 恒 [2,1,1]，显式设置并
    // 在发射后恢复（不污染帧内后续 VM 材质态）。
    auto& pipeline = draw_capture->pipeline();
    const auto tfactor_kept = pipeline.textureFactor;
    const auto source_kept = pipeline.sourceBlend;
    const auto destination_kept = pipeline.destinationBlend;
    const auto equation_kept = pipeline.blendEquation;
    pipeline.sourceBlend = touhou::graphics::BlendFactor::One;
    pipeline.destinationBlend = touhou::graphics::BlendFactor::Zero;
    pipeline.blendEquation = touhou::graphics::BlendEquation::Add;
    for (const auto& strip : strips) {
        draw_capture->bind_texture(draw_capture->texture(*text, strip.entry));
        draw_capture->set_stale_layer(i32(strip.layer));
        const Vertex verts[6] = {
            {strip.x0, 15.5f, 0, 1, 0xffffffffu, 0.2f, 1.f / 30.f},
            {strip.x1, 15.5f, 0, 1, 0xffffffffu, 0.8f, 1.f / 30.f},
            {strip.x0, 463.5f, 0, 1, 0xffffffffu, 0.2f, 29.f / 30.f},
            {strip.x1, 15.5f, 0, 1, 0xffffffffu, 0.8f, 1.f / 30.f},
            {strip.x0, 463.5f, 0, 1, 0xffffffffu, 0.2f, 29.f / 30.f},
            {strip.x1, 463.5f, 0, 1, 0xffffffffu, 0.8f, 29.f / 30.f},
        };
        draw_capture->primitives(touhou::graphics::Topology::Triangles, 2, verts, sizeof(Vertex));
    }
    pipeline.textureFactor = tfactor_kept;
    pipeline.sourceBlend = source_kept;
    pipeline.destinationBlend = destination_kept;
    pipeline.blendEquation = equation_kept;
}

}  // namespace th13
