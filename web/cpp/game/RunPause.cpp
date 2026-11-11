#include "RunPause.hpp"
#include "GameHud.hpp"

#include <cstdio>
#include <ctime>

namespace th13 {

namespace {
// 0x4bb944 关卡名表（TitleResultEntry kStageNames 同源，0x43de80 榜行 %s 用）。
constexpr const char* kRankStageNames[10] = {"test   ", "Stage 1", "Stage 2", "Stage 3",
                                             "Stage 4", "Stage 5", "Stage 6", "Extra  ",
                                             "Clear  ", "ExClear"};

// 0x43de80 case 0x13/0x1c 榜行短格式（0x4abaf0/0x4abad0）：timestamp 为 0 =
// 从未玩过行。%9ld%d = 榜内 9 位分 + 个位（续关数位，0x43d6b0 写 be7c8）。
// 待录入行名字 = 8 空格（0x43d6b0 写 0x20202020×2；font0 空格推进保对齐）。
std::string rank_row_text(const TitlePlayerDataScore& row, i32 index) {
    char text[128];
    const char* name = row.name.empty() ? "        " : row.name.c_str();
    if (row.timestamp == 0 || row.stage < 0 || row.stage >= 10) {
        std::snprintf(text, sizeof text, "%2d %s %9d%d --/--/-- Stage -",
                      index + 1, name, row.score, row.continues);
        return text;
    }
    const std::time_t clock = std::time_t(row.timestamp);
    const std::tm* date = std::localtime(&clock);
    if (!date) {
        std::snprintf(text, sizeof text, "%2d %s %9d%d --/--/-- Stage -",
                      index + 1, name, row.score, row.continues);
        return text;
    }
    std::snprintf(text, sizeof text, "%2d %s %9d%d %.2d/%.2d/%.2d %s",
                  index + 1, name, row.score, row.continues,
                  date->tm_year % 100, date->tm_mon + 1, date->tm_mday,
                  kRankStageNames[row.stage]);
    return text;
}
}  // namespace

struct RunPause::Services final : PauseMenuServices {
    RunPause& owner;
    explicit Services(RunPause& p) : owner(p) {}
    bool sound(i32 id) override { return owner.sound(id); }
    bool finish_selection(i32 entry) override { return owner.finish_selection(entry); }
    bool save_replay_slot(i32 slot, const std::string& name) override { return owner.save_replay_slot(slot, name); }
    bool change_option(i32 row, i32 delta) override { return owner.change_option(row, delta); }
    bool record_result_name(const std::string& name) override { return owner.record_result_name(name); }
};

RunPause::RunPause(SessionState& p, AnmManager& a, AnmEnvironment& e, RunPausePlatform& h,
                   FrameScheduler& f, i32 bank, i32 text_bank)
    : services(std::make_unique<Services>(*this)), progress(p), animations(a), environment(e),
      platform(h), front(bank), text(text_bank), menu(state, a, *services), frame(state, p, menu, f) {}

RunPause::~RunPause() { frame.enable(false); }

bool RunPause::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Run pause unavailable" : why;
    return false;
}

bool RunPause::sound(i32 id) { return platform.sound(id); }
bool RunPause::save_replay_slot(i32 slot, const std::string& name) {
    return platform.save_replay_slot(slot,name) || fail("Replay save failed");
}
bool RunPause::change_option(i32 row, i32 delta) {
    return platform.change_pause_option(row,delta) || fail("Pause option update failed");
}
bool RunPause::record_result_name(const std::string& name) {
    return platform.record_result_name(name) || fail("Result record failed");
}

bool RunPause::draw_overlay(HudDrawServices& sink) {
    if (state.screen == PauseScreen::Inactive) return true;
    // 覆盖层文本面 = 原版 0x43de80 逐帧文本分发（AsciiInf 队列）：
    // 全 pass shadow=1（入口 +0x19174=1），其余样式 = 0x4038a0 构造默认
    // （font0 ch-32 全覆盖、左对齐、scale1、白色）。
    HudTextStyle style;
    style.shadow = true;
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return sink.hud_text({{x, y, 0}, style, text}) ||
               fail("Pause overlay text failed");
    };
    if (state.phase == 18) {
        TitlePlayerDataScore rows[10];
        const i32 rank = platform.result_ranking(rows);
        if (state.name_is_result) {
            if (rank < 0) return fail("Result rank unavailable for name entry");
            // 0x43de80 case 0x13：头部 (48,64) → 名字行 (75,96+rank*18)+黄 _ →
            // 榜 10 行 (48,96+18i) 高亮=rank 黄 0xffffff00、余 0xff808180
            //（0x43e1bd：y = rank*18.0(0x4aeb30) + 96.0(0x4aecc0)，与榜行同基）。
            if (!put(48, 64, 0xffffffffu, "            Score Ranking!!")) return false;
            for (i32 i = 0; i < 10; ++i)
                if (!put(48, 96.f + 18.f * i,
                         i == rank ? 0xffffff00u : 0xff808180u,
                         rank_row_text(rows[i], i)))
                    return false;
            const float name_y = 96.f + 18.f * rank;
            if (!put(75, name_y, 0xffffffffu, state.replay_name)) return false;
            // 0x43dd10：黄 _ 紧随名字（字距 9px=font0 默认 spacing；8 字时 -9 回退）。
            float underscore_x = 75.f + 9.f * float(state.replay_name.size());
            if (state.replay_name.size() >= 8) underscore_x -= 9.f;
            if (!put(underscore_x, name_y, 0xffffff00u, "_")) return false;
        } else {
            // 0x43de80 case 10/0x12/0x1b（回放保存名前录入）：名字行定格
            // (102,224)（原版 age<10 自选中槽行 y=slot*15 滑入，属插值细节）。
            if (!put(102, 224, 0xffffffffu, state.replay_name)) return false;
            float underscore_x = 102.f + 9.f * float(state.replay_name.size());
            if (state.replay_name.size() >= 8) underscore_x -= 9.f;
            if (!put(underscore_x, 224, 0xffffff00u, "_")) return false;
            // "No.%02d ..." 选中槽行（(48,224)）需要逐槽回放元数据，未接线，
            // 保持省略（不占位伪造）。
        }
        // 91 字符网格 (112,320)，列步 18 行步 16；SPACE/DEL/END 槽画 font0
        // 专用字形（原版推字符 0x81/0x7f/0x80 → sprite 95/96/97）；
        // 光标字符黄 0xffffff00、余 0xff808180（0x43dd10 主循环）。
        for (i32 i = 0; i < 91; ++i) {
            const u8 ch = i < 0x58 ? u8(kResultNameCharacters[i])
                        : i == 0x58 ? u8(0x81) : i == 0x59 ? u8(0x7f) : u8(0x80);
            const float x = 112.f + 18.f * float(i % 13);
            const float y = 320.f + 16.f * float(i / 13);
            if (!put(x, y, i == state.name_cursor ? 0xffffff00u : 0xff808180u,
                     std::string(1, char(ch))))
                return false;
        }
        return true;
    }
    if (state.phase == 17) {
        // 回放选槽：原版 = 25 行 "No.%02d ..." 榜单（0x43de80 case 9，
        // (48,64+15i) 高亮黄）。逐槽回放元数据未接线，占位保持（font0）。
        const std::string text =
            "REPLAY SLOT  " + std::to_string(state.replay_slot) + " / 25";
        return put(150, 180, 0xffffffffu, text);
    }
    if (state.phase == 19) {
        const std::string text =
            "OPTIONS  " + std::string(state.option_row ? "SE " : "BGM ") +
            std::to_string(platform.pause_option_value(state.option_row));
        return put(150, 180, 0xffffffffu, text);
    }
    // 0x43de80 case 0xf：GameOver 续关菜单 Credit 计数 (184,448)，白。
    if (state.screen == PauseScreen::GameOver && state.phase == 6)
        return put(184, 448, 0xffffffffu,
                   "Credit " + std::to_string(progress.continue_stock));
    return true;
}

bool RunPause::finish_selection(i32 entry) {
    // 43e780 phase4 only calls43e700 (resume) for entry0. Title/restart
    // keep the run frozen until the host switches scenes, including the
    // remaining callbacks in this update.
    if (entry == 2) return platform.save_replay() || fail("Replay save request failed");
    if (entry == 3) return platform.open_pause_options() || fail("Pause options request failed");
    if (!animations.retire(state.menu_animation) ||
        !animations.retire(state.snapshot_animation)) return fail(animations.error);
    animations.clear_captures();
    state.menu_animation = 0;
    switch (entry) {
    case 0:
        if (state.screen == PauseScreen::GameOver && !platform.continue_run())
            return fail("Continue request failed");
        progress.rate = state.saved_rate;
        progress.scene_flags &= ~0x10u;
        environment.paused = false;
        // 0x43e700:0x43e749-0x43e753：恢复时对话对象存在 → 0x429f10 置回
        // VM+0x18 bit1（0x429f10 唯一非结算调用者）。
        if (!platform.dialogue_visible(true)) return fail("Pause dialogue show failed");
        return platform.resume_music() || fail("Pause music resume failed");
    case 1:
        return platform.return_to_title() || fail("Pause title destination failed");
    case 4:
        return platform.restart_run() || fail("Pause restart destination failed");
    default:
        return fail("Pause selection outside original range");
    }
}

bool RunPause::prepare() {
    if (!error.empty() || !frame.error.empty()) return fail(frame.error);
    if (!animations.resource(front)) return fail("Run pause front resource unavailable");
    if (!animations.resource(text)) return fail("Run pause text resource unavailable");
    frame.enable(true);
    return true;
}

bool RunPause::open(PauseEntrance entrance) {
    if (!error.empty()) return false;
    PauseState previous_state = state;
    const float previous_rate = progress.rate;
    const u32 previous_scene_flags = progress.scene_flags;
    const bool previous_paused = environment.paused;
    bool activated = false;
    auto rollback = [&]() {
        // 43e3f0 sets the freeze only as part of successful overlay activation.
        // A host resource/service failure must not leave a ghost pause behind.
        animations.retire(state.menu_animation);
        animations.retire(state.snapshot_animation);
        animations.clear_captures();
        state = previous_state;
        progress.rate = previous_rate;
        progress.scene_flags = previous_scene_flags;
        environment.paused = previous_paused;
        if (activated && !platform.resume_music())
            error = "Pause activation rollback music resume failed";
    };
    // 激活序列（th15 PauseActivation 裁剪）：菜单视觉 → 音频挂起 → 表现冻结。
    state.select(entrance == PauseEntrance::GameOver ? PauseScreen::GameOver
                 : entrance == PauseEntrance::PracticeEnd ? PauseScreen::PracticeEnd
                 : entrance == PauseEntrance::ExtraEnd ? PauseScreen::ExtraEnd
                 : entrance == PauseEntrance::BGameOver ? PauseScreen::BGameOver
                 : PauseScreen::Pause);
    progress.scene_flags |= 0x10;
    state.front_bank = front;
    state.practice = progress.practice_mode != 0;
    state.can_continue = progress.continue_stock > 0;
    // 0x440960's 0x17/0x18 menus always have three items; the 43fa90/43e780
    // "hide replay save after a continue" gate belongs to the A menus only.
    const bool results_menu = entrance == PauseEntrance::ExtraEnd ||
                              entrance == PauseEntrance::BGameOver;
    state.can_save_replay = platform.can_save_replay() &&
                            (results_menu || progress.continues == 0);
    state.can_open_options = platform.can_open_pause_options();
    state.result_needs_name = (entrance == PauseEntrance::ExtraEnd || entrance == PauseEntrance::GameOver)
                              && platform.result_needs_name();
    if (entrance == PauseEntrance::GameOver || entrance == PauseEntrance::PracticeEnd)
        state.select_phase(0);
    if (!animations.retire(state.menu_animation) ||
        !animations.retire(state.snapshot_animation)) {
        rollback();
        return fail(animations.error);
    }
    // overlay 菜单 = create(ordering 4) + 清 0xc000 渲染位（th15 create_overlay 同义）。
    state.snapshot_animation = animations.create(text, kPauseBackdropScript, -1, 4);
    if (!state.snapshot_animation) {
        rollback();
        return fail(animations.error);
    }
    // 0x43e3f0:0x43e492-0x43e4d0：backdrop VM 建好后立即调 0x4418b0，把
    // 战场 (32,16,384,448) 点采样进该 VM 当前精灵的纹理矩形（text.anm
    // entry1 Blank，sprite76=(0,0,128,128)）。0x45bf10 在 Present 前消费；
    // port 由 SceneDisplay::before_present → ApplicationState::flush_captures
    // 执行（见 AnmManager::queue_capture）。缺此截取时 script76-79 的加色
    // 淡入采样到全零纹理，暂停入场黑幕整段不可见（BUG 7）。
    // 截取请求是表现增强，不是暂停激活的必要条件：合成夹具/无精灵 backdrop
    // 下 0x4418b0 也只是空槽 no-op，不得因此让 open 失败（旧测试与无资源
    // 宿主必须仍能进入暂停）。
    if (auto* backdrop = animations.registry.find(state.snapshot_animation))
        animations.queue_capture(*backdrop);
    const i32 script = entrance == PauseEntrance::GameOver ? kGameOverMenuScript
                       : entrance == PauseEntrance::ExtraEnd ? kResultsMenuScript
                       : entrance == PauseEntrance::BGameOver ? kResultsMenuScript
                       : entrance == PauseEntrance::PracticeEnd ? kPracticeEndMenuScript
                       : state.practice ? kPracticePauseMenuScript : kPauseMenuScript;
    // 0x43fa90 态 0xe：GameOver 进榜（+0x1f8==0 → 态 0x13 名前录入）不建
    // 菜单脚本 142；录入完成 END → LAB_00440520 才建 142 进态 0xf。未进榜
    // 走原态 0xf 分支立即建。移植侧 phase18 END（PauseMenu case 18）补建。
    if (!(entrance == PauseEntrance::GameOver && state.result_needs_name)) {
        state.menu_animation = animations.create(front, script, -1, 4);
        if (auto* vm = animations.registry.find(state.menu_animation))
            vm->visual.render_flags &= ~0xc000u;
        if (!state.menu_animation) {
            rollback();
            return fail(animations.error);
        }
    }
    state.saved_rate = progress.rate;
    progress.rate = 1;
    environment.paused = true;
    if (!platform.suspend_music()) {
        rollback();
        return fail("Pause music suspend failed");
    }
    activated = true;
    // 0043e3f0: pause activation calls 0x460bc0(voice 0x0e) after building
    // the overlay and before the menu animation advances.
    if (entrance == PauseEntrance::Pause && !platform.sound(0x0e)) {
        rollback();
        return fail("Pause entry sound failed");
    }
    // 延迟建菜单（GameOver 进榜名前录入）时无菜单 VM 可中断。
    if (state.menu_animation && !animations.interrupt(state.menu_animation, 3)) {
        rollback();
        return fail(animations.error);
    }
    // 0x43e3f0 激活末尾：对话对象存在（ctx+0x198 != 0）→ 0x429d00 清对话
    // 9 槽 VM+0x18 bit1（隐藏，不销毁）。仅普通 Esc 暂停；GameOver/结算
    // 激活路径（0x43e5a0 族）不调用 0x429d00。
    if (entrance == PauseEntrance::Pause && !platform.dialogue_visible(false)) {
        rollback();
        platform.dialogue_visible(true);  // 回滚不留隐藏残态
        return fail("Pause dialogue hide failed");
    }
    return true;
}

}  // namespace th13
