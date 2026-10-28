// TitleReplayMenu 实现：0x4525b0（replay-create）/0x452c30（replay-draw）的
// 移植。数值证据（.data/.rdata 实读，详见 artifacts/verification/r3/M）：
//   格式串 0x4c0d8c 表 = [0x4ac710,0x4ac6d4,0x4ac6a0,0x4ac668]、0x4c0d9c =
//   0x4ac62c（SpellPr 变体）；名字表 0x4bb904（角色 4 项）、0x4bb914（难度）、
//   0x4bb944（关卡 1..7）、0x4bb96c（路线）；坐标 58/80/18 与 220/128；
//   顶层头布局：+0x00 名字(12B)、+0x0A 特征位（bit1=符卡练习）、+0x0C 保存
//   时刻(time64)、+0x14 最终分、+0x54 慢速、+0x68 路线、+0x6C 续关数、
//   +0x70 练习关；stage 头 +0x1C 起始分、+0x24 续关数（0x449070/0x447900 恢复
//   源同偏移）。选关行 k 显示 = 关 k+1 的起始分（= k 关终了分近似），k≥6 或
//   下关缺 = 顶层最终分（0x452eab-0x452ec9 分支实读）。
#include "TitleReplayMenu.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <cstring>

namespace th13 {

namespace {
// 页行数/页数（0x4525f0 +0x30=0x19、+0x1e0=3；行距 18 = 0x4aeb30 实读）。
constexpr i32 kRowsPerPage = 25;
constexpr i32 kStageRows = 7;

// 名字表（原版指针表实读；尾随空格是原版列对齐的一部分，原样保留）。
constexpr const char* kCharacterNames[4] = {"Reimu  ", "Marisa ", "Sanae  ", "Youmu  "};  // 0x4bb904
constexpr const char* kDifficultyNames[6] = {"Easy   ", "Normal ", "Hard   ", "Lunatic",
                                             "Extra  ", "O.D.   "};                          // 0x4bb914
constexpr const char* kStageNames[kStageRows + 1] = {nullptr, "Stage 1", "Stage 2", "Stage 3",
                                                     "Stage 4", "Stage 5", "Stage 6",
                                                     "Extra  "};                              // 0x4bb944
constexpr const char* kRouteNames[10] = {"tst", "St1", "St2", "St3", "St4",
                                         "St5", "St6", "Ex ", "All", "ExA"};                  // 0x4bb96c

// 选中/未选色（0x452f95：-(row!=cursor)&0xff808180 - 0x100 的两分支常量）。
constexpr u32 kColorSelected = 0xffffff00u;
constexpr u32 kColorUnselected = 0xff808080u;
constexpr u32 kColorHeader = 0xffffffffu;  // 选关头行（寄存器复位值 0xffffffff）

u32 read_u32(const u8* p) { return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24; }
i32 read_i32(const u8* p) { return i32(read_u32(p)); }
i64 read_i64(const u8* p) {
    return i64(u64(read_u32(p)) | u64(read_u32(p + 4)) << 32);
}

// 顶层头 +0x0C 保存时刻 → 本地历（原版 _localtime64，0x452fbc 调用点）。
bool save_date(const ReplayFile& file, i32& year2, i32& month, i32& day, i32& hour,
               i32& minute) {
    if (file.top_header.size() < 0x14) return false;
    const std::time_t stamp = std::time_t(read_i64(file.top_header.data() + 0x0C));
    std::tm broken{};
#if defined(_WIN32)
    if (localtime_s(&broken, &stamp) != 0) return false;
#else
    if (localtime_r(&stamp, &broken) == nullptr) return false;
#endif
    year2 = broken.tm_year % 100;  // 原版 push tm_year%100（0x45301e idiv 0x64）
    month = broken.tm_mon + 1;     // +1（0x453016 inc）
    day = broken.tm_mday;
    hour = broken.tm_hour;
    minute = broken.tm_min;
    return true;
}

std::string format(const char* pattern, ...) {
    char text[256];
    va_list args;
    va_start(args, pattern);
    std::vsnprintf(text, sizeof text, pattern, args);
    va_end(args);
    return std::string(text);
}

// 页 >0 行首文件标识（原版 dword 读 entry+0x227 = 文件名第 7..10 字节：
// th13_udXXXX.rpy → "XXXX"；00452c30 0x4e5368 写入点）。
std::string file_identifier(const std::string& name) {
    if (name.size() >= 11) return name.substr(7, 4);
    return name;
}

// 行首 No. 编号 = 扁平序 + 1（0x453020 inc ebx）。
std::string number_label(i32 index) { return format("%.2d", index + 1); }

// 0x4ac6d4/0x4ac668 公共尾（原版串尾 "%%" 经 printf 渲染为 "%"，此处存渲染形）。
constexpr const char* kEmptyListRow =
    " -------- --/--/-- --:-- ------- ------- --- ---%";

const std::string kEmptyListSuffix = kEmptyListRow;  // 字符串拼接用（渲染形）
}  // namespace

bool TitleReplayMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool TitleReplayMenu::sound(i32 id) {
    return play_sound ? play_sound(id) || fail("Replay sound request failed")
                      : fail("Replay sound service unavailable");
}

bool TitleReplayMenu::present(u32 handle) const { return handle && animations.registry.find(handle); }

bool TitleReplayMenu::spell_practice(const ReplayFile& file) {
    // 顶层头 +0xA bit1（0x452cde/0x452fd4 test [esi+0xa],2；0x447900:67 录制侧
    // 置位源 = DAT_004be830 &0x30==0x20）。
    return file.top_header.size() > 0x0A && (file.top_header[0x0A] & 2) != 0;
}

const ReplayFile* TitleReplayMenu::selected_file() const {
    return replay_host.replay_entry(selected_index);
}

const ReplayStage* TitleReplayMenu::stage_block(const ReplayFile& file, i32 number) const {
    // 0x448c40:75 槽位 = stage 头编号索引（+0xd8 + num*0x28），不是链序。
    for (const ReplayStage& stage : file.stages)
        if (stage.number == u16(number)) return &stage;
    return nullptr;
}

i32 TitleReplayMenu::stage_chain_index(const ReplayFile& file) const {
    // start_replay 的 stage_index = stage 块链序（begin_replay_run 消费）。
    if (spell_practice(file)) return 0;  // 符卡练习录像：首块即该符卡（原版
                                         // +0x5aa4=0 → be81c=1 直开）。
    const i32 number = selected_stage_row + 1;
    for (u32 i = 0; i < file.stages.size(); i++)
        if (file.stages[i].number == u16(number)) return i32(i);
    return -1;
}

// case0（0x4525cf-0x45274f）：光标自扁平序恢复 + 建粒子根/徽记/背景根缺位补建。
bool TitleReplayMenu::open() {
    const i32 slots = replay_host.replay_count();
    if (slots <= 0) return fail("Replay catalog unavailable");
    pages = std::max(1, slots / kRowsPerPage);
    if (state.replay_index < 0 || state.replay_index >= slots) state.replay_index = 0;
    state.menu.wrapping = true;
    state.menu.count = kRowsPerPage;
    state.menu.select(state.replay_index % kRowsPerPage);   // +0x30=0x19、cursor=%25
    page.wrapping = true;
    page.count = pages;
    page.select(state.replay_index / kRowsPerPage);          // +0x1e0=3、page=/25
    // ascii.anm s19 粒子根（0x452647-0x4526ce：仅缺位建，+0x690 持有；
    // 脚本自带 L22/持续 spawn s18，无平移）。
    if (!state.replay_ascii_root) {
        state.replay_ascii_root =
            animations.create(resources.ascii, resources.replay_particle_root, -1, 0);
        if (!state.replay_ascii_root) return fail(animations.error);
    }
    // title.anm s104 徽记（0x4526d5 edi=0x68 → 0x44bb80 → 槽 +0x468）。
    if (!present(state.handles[resources.replay_badge])) {
        state.handles[resources.replay_badge] =
            animations.create(resources.title, resources.replay_badge, -1, 0);
        if (!state.handles[resources.replay_badge]) return fail(animations.error);
    }
    // 背景根缺位重建 + label3（0x452737-0x45274a：script89 + interrupt 3）。
    if (!present(state.handles[resources.background_root])) {
        state.handles[resources.background_root] =
            animations.create(resources.title, resources.background_root, -1, 0);
        if (!state.handles[resources.background_root]) return fail(animations.error);
        if (!animations.interrupt(state.handles[resources.background_root], 3, false))
            return fail(animations.error);
    }
    return true;
}

void TitleReplayMenu::consume_import_hint() {
    // 网页导入完成 → 落到新槽并选中（宿主经 TitleState.replay_import_hint 提示）。
    if (state.replay_import_hint < 0) return;
    const i32 slot = state.replay_import_hint;
    state.replay_import_hint = -1;
    if (slot < 0 || slot >= replay_host.replay_count()) return;
    state.replay_index = slot;
    page.select(slot / kRowsPerPage);
    state.menu.select(slot % kRowsPerPage);
}

// case2（0x452810-0x4529ea）：文件列表交互。
bool TitleReplayMenu::confirm_file() {
    const i32 slots = replay_host.replay_count();
    selected_index = page.cursor * kRowsPerPage + state.menu.cursor;
    state.replay_index = selected_index;
    if (selected_index == slots - 1) {
        // 末行 = 网页「导入本地 .rpy」入口：触发文件选择（异步，结果经
        // replay_import_hint/错误字段落回）。请求失败（如非浏览器宿主）=
        // 空槽同族无效操作：菜单保持可操作，错误文案由 Import 行逐帧展示
        // （replay_import_error），不整帧 fail。
        if (!replay_host.request_replay_file()) return true;
        return sound(7);
    }
    const ReplayFile* file = replay_host.replay_entry(selected_index);
    if (!file) return true;  // 空槽：Z 无效（0x452914 entry!=0 门）
    if (!sound(7)) return false;
    state.menu.push();
    state.menu.wrapping = true;
    state.menu.count = kStageRows;  // +0x30 = 7（0x45293a）
    for (i32 row = 0; row < kStageRows; row++) {
        // 缺关槽禁用（0x452916-0x452929 → 0x4417c0 disabled 追加）。
        if (!stage_block(*file, row + 1) && !state.menu.disable(row))
            return fail(state.menu.error);
    }
    state.menu.select(0);  // +0x45296x FUN_00415030(0) + 前后 ±1 步进落启用行
    if (spell_practice(*file)) {
        // 符卡练习录像：跳过选关直进 case3（0x4529d0 → LAB_004529ea）。
        selected_stage_row = 0;
        if (!sound(50)) return false;
        state.change_substate(3);
        return true;
    }
    state.change_substate(4);  // 0x45290d FUN_0044bb20(4)
    return true;
}

bool TitleReplayMenu::update(u32 pressed, u32 repeated) {
    switch (state.substate) {
    case 0:
        if (!open()) return false;
        state.change_substate(1);
        break;
    case 1:
        // >6 帧进列表（0x4525cb case1 age>6 → FUN_0044bb20(2)）。
        if (state.age.current > 6) state.change_substate(2);
        break;
    case 2: {
        consume_import_hint();
        state.menu.previous = state.menu.cursor;
        const i32 page_previous = page.cursor;
        if ((pressed | repeated) & 0x10) state.menu.move(-1);  // ↑（0x4e49fc 位布局）
        if ((pressed | repeated) & 0x20) state.menu.move(1);   // ↓
        if (!state.menu.error.empty()) return fail(state.menu.error);
        if ((pressed | repeated) & 0x40) page.move(-1);        // ← 换页
        if ((pressed | repeated) & 0x80) page.move(1);         // →
        if (!page.error.empty()) return fail(page.error);
        if (state.menu.previous != state.menu.cursor || page_previous != page.cursor) {
            if (!sound(10)) return false;  // 0x45285x FUN_00460bc0(…,10)
        }
        if (pressed & 0x102) {  // X/Esc：→ case5 收面（0x452896 FUN_0044bb20(5)、音 9）
            if (!sound(9)) return false;
            state.change_substate(5);
            return true;
        }
        if (pressed & 0x80001) {  // Z：确认
            if (!confirm_file()) return false;
        }
        break;
    }
    case 4:
        // >14 帧收输入（0x452a76 case4 age>0xe）。
        if (state.age.current > 14) {
            state.menu.previous = state.menu.cursor;
            if ((pressed | repeated) & 0x10) state.menu.move(-1);
            if ((pressed | repeated) & 0x20) state.menu.move(1);
            if (!state.menu.error.empty()) return fail(state.menu.error);
            if (state.menu.previous != state.menu.cursor) {
                if (!sound(10)) return false;
            }
            if (pressed & 0x102) {  // X：回列表（0x452a9d：pop、+0x30=0x19、case2、音 9）
                if (!sound(9)) return false;
                state.menu.pop();
                state.menu.count = kRowsPerPage;
                state.change_substate(2);
                return true;
            }
            if (pressed & 0x80001) {  // Z：确认关卡 → case3（LAB_004529ea、音 50）
                selected_stage_row = state.menu.cursor;
                if (!sound(50)) return false;
                state.change_substate(3);
            }
        }
        break;
    case 3:
        // >31 帧排队 destination 0xd（0x452a70 case3 age>0x1f && 枚举完成；
        // 原版此拍写 DAT_004dc674 = 选中扁平序，同拍持久化）。
        if (!launch_queued && state.age.current > 31) {
            const ReplayFile* file = selected_file();
            if (!file) return fail("Replay launch: selected entry unavailable");
            const i32 chain = stage_chain_index(*file);
            if (chain < 0)
                return fail("Replay launch: stage block missing for row " +
                            std::to_string(selected_stage_row + 1));
            state.replay_index = selected_index;
            if (!replay_host.start_replay(selected_index, chain))
                return fail("Replay launch: host rejected start request");
            launch_queued = true;  // 排队即止：宿主帧尾消费，本菜单不销毁自身
        }
        break;
    case 5:
        // 收面（0x452b04 case5 age>5：释放目录、label1 退场徽记/粒子根、回主菜单）。
        if (state.age.current > 5) {
            if (state.handles[resources.replay_badge]) {
                animations.retire(state.handles[resources.replay_badge]);
                state.handles[resources.replay_badge] = 0;
            }
            if (state.replay_ascii_root) {
                animations.retire(state.replay_ascii_root);
                state.replay_ascii_root = 0;
            }
            state.change_screen(TitleScreen::Main);
            state.menu.pop();
        }
        break;
    default:
        break;
    }
    return true;
}

// ---- 绘制侧（0x452c30）：文件行文本。 ----
bool TitleReplayMenu::file_row_text(i32 index, std::string& out) {
    const i32 slots = replay_host.replay_count();
    const i32 page_index = index / kRowsPerPage;
    const ReplayFile* file = replay_host.replay_entry(index);
    if (index == slots - 1) {
        // 末行 = 网页导入入口（样式沿用原版列表行；错误文案即时显示）。
        const std::string reason = replay_host.replay_import_error();
        out = reason.empty() ? "Import " + kEmptyListSuffix : "Import  " + reason;
        return true;
    }
    if (!file) {
        // 空槽：页 0 = "No.%.2d -------- …"（0x4ac6d4）、页 >0 = "User  -------- …"
        //（0x4ac668；0x4530fe 表 [(page!=0)*2+1]）。
        out = page_index == 0 ? "No." + number_label(index) + kEmptyListSuffix
                              : "User " + kEmptyListSuffix;
        return true;
    }
    i32 year2 = 0, month = 0, day = 0, hour = 0, minute = 0;
    const bool dated = save_date(*file, year2, month, day, hour, minute);
    if (!dated) return fail("Replay save date unavailable: " + replay_host.replay_filename(index));
    const u8* top = file->top_header.data();
    const i32 character = std::clamp(read_i32(top + 0x5C) + read_i32(top + 0x60), 0, 3);
    const i32 difficulty = std::clamp(read_i32(top + 0x64), 0, 5);
    const i32 route = std::clamp(read_i32(top + 0x68), 0, 9);
    const double slow = double(file->slow_rate);
    if (page_index == 0) {
        if (spell_practice(*file)) {
            // 0x4ac62c："No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s SpellPr %3d %2.1f%%"
            const i32 practice = read_i32(top + 0x70) + 1;  // 0x453038 inc（不截断）
            out = format("No.%s %s %.2d/%.2d/%.2d %.2d:%.2d %s SpellPr %3d %2.1f%%",
                         number_label(index).c_str(), file->player_name.c_str(), year2, month,
                         day, hour, minute, kCharacterNames[character], practice, slow);
        } else {
            // 0x4ac710："No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%"
            out = format("No.%s %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",
                         number_label(index).c_str(), file->player_name.c_str(), year2, month,
                         day, hour, minute, kCharacterNames[character],
                         kDifficultyNames[difficulty], kRouteNames[route], slow);
        }
    } else {
        // 0x4ac6a0："%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%"（首 %s =
        // 文件名字节 7..10，0x4e5368 全局）。
        out = format("%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",
                     file_identifier(replay_host.replay_filename(index)).c_str(),
                     file->player_name.c_str(), year2, month, day, hour, minute,
                     kCharacterNames[character], kDifficultyNames[difficulty],
                     kRouteNames[route], slow);
    }
    return true;
}

// 选关行（0x452e26-0x452f06）：行 k = 关 k+1；本关块缺失 = "---------"
//（0x452e87），否则 = 下关起始分（k<6 且下关块在，0x452eab-0x452ec1）或
// 顶层最终分 + 续关数（0x452ec3-0x452ec9）。
bool TitleReplayMenu::stage_row_text(const ReplayFile& file, i32 row, std::string& out) {
    const i32 number = row + 1;
    if (number < 1 || number > kStageRows) return fail("Replay stage row out of range");
    if (!stage_block(file, number)) {
        out = format("%s  ---------", kStageNames[number]);
        return true;
    }
    const ReplayStage* next = number < 6 ? stage_block(file, number + 1) : nullptr;
    const u8* top = file.top_header.data();
    i32 score = 0, extra = 0;
    if (next) {
        score = next->score;  // 下关起始分（stage 头 +0x1C）
        extra = next->header.size() > 0x28 ? read_i32(next->header.data() + 0x24) : 0;
    } else {
        if (file.top_header.size() < 0x74) return fail("Replay top header unavailable");
        score = read_i32(top + 0x14);              // 最终分
        extra = read_i32(top + 0x6C);              // 续关数
    }
    out = format("%s  %.8d%d", kStageNames[number], score, extra);  // 0x4ac818
    return true;
}

bool TitleReplayMenu::draw_texts(HudDrawServices& host) {
    if (i32(state.screen) != i32(TitleScreen::Replay)) return true;
    if (state.substate != 2 && state.substate != 4) return true;  // 0x452c30 值域
    HudTextStyle style;  // 队列默认：font0、左对齐、scale 1
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return host.hud_text({{x, y, 0}, style, text});
    };
    if (state.substate == 2) {
        // 文件列表：origin (58,80)、行距 15（0x452f30 x=0x4aeb54=58f、
        // 0x452f49 y=0x4aeb28=80f、0x45312e fadd QWORD 0x4aeb20=15.0——
        // 25 行末行 y=80+24*15=440，恰收进 480 高度；早先「行距 18」是
        // 0x4aeb30 误读，该 qword 实为选关行距）。
        for (i32 r = 0; r < kRowsPerPage; r++) {
            const i32 index = page.cursor * kRowsPerPage + r;
            if (index >= replay_host.replay_count()) break;
            std::string text;
            if (!file_row_text(index, text)) return false;
            const u32 color = r == state.menu.cursor ? kColorSelected : kColorUnselected;
            if (!put(58, 80 + 15 * r, color, text)) return false;
        }
        return true;
    }
    // 选关（substate 4）：头行 x=58（0x4aeb54），前 10 帧 y = 80 + (选中行
    // %25)*15*(10-age_f)/10（0x452c81-0x452caf：[+0x2bc] float age、
    // [+0x5aa0] 选中扁平序 %25*15、除 QWORD 0x4aeb10=10.0、加 0x4aeb88=80.0；
    // 0x452c7e jge age>=10 定格 80）——选中行自列表位线性滑至顶部；
    // 关卡行 (220,128+18k)（0x4aeb84=220f、0x4ae9e8=128f、0x452eec fadd
    // QWORD 0x4aeb30=18.0）自 age>9 起（0x452e19 jl → 0x452f06）。
    const ReplayFile* file = selected_file();
    if (file) {
        float header_y = 80;
        if (state.age.current < 10)
            header_y = 80 + float(selected_index % kRowsPerPage) * 15 *
                               (10 - state.age.fractional) / 10;
        std::string text;
        if (!file_row_text(selected_index, text)) return false;
        if (!put(58, header_y, kColorHeader, text)) return false;
    }
    if (state.age.current > 9 && file) {
        for (i32 k = 0; k < kStageRows; k++) {
            std::string text;
            if (!stage_row_text(*file, k, text)) return false;
            const u32 color = k == state.menu.cursor ? kColorSelected : kColorUnselected;
            if (!put(220, 128 + 18 * k, color, text)) return false;
        }
    }
    return true;
}

}  // namespace th13
