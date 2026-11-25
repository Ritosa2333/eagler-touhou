#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>
#include "locale.hpp"

namespace soew {

enum class EndingBranch { Bad, Good, Extra };

struct EndingRun {
    unsigned continues_used = 0;
    unsigned shot_type = 0; // Original values 0/1/2 (A/B/C).
    unsigned rank = 1;      // Original values 0–3, or 4 for Extra.
    std::uint32_t score = 0; // Final resident score, not the live internal score.
    int start_lives = 2;     // Resident value, excluding the on-screen player.
    int start_bombs = 3;
    int skill = 0;          // Raw skill; the verdict applies its own display clamp.
    bool extra_unlocked = false; // Result of the host's saved clear check.
};

// Snapshot these run totals before resetting the stage or player state.
// ReC98 th02/main/boss/b5_.cpp:5–22 (the resident stores a signed 16-bit int).
constexpr int ending_calculate_skill(unsigned rank, unsigned continues_used, std::uint8_t total_misses,
                                     std::uint8_t total_bombs_used, int item_skill) {
    const int miss_and_bombs = std::max(0, 50 - 3 * int(total_misses) - int(total_bombs_used));
    const auto skill = static_cast<std::uint16_t>(
        rank * 20u + 10u - continues_used * 3u + miss_and_bombs + std::clamp(item_skill, 0, 25));
    return skill < 0x8000u ? int(skill) : int(skill) - 0x10000;
}

// ReC98 th02_main.asm:21300–21304 and th02/maine_03.cpp:27.
constexpr std::uint32_t ending_resident_score(std::uint32_t live_score, unsigned continues_used) {
    return live_score * 10u + static_cast<std::uint16_t>(continues_used);
}

// ReC98 th02/formats/end.hpp: 44 display bytes plus a two-byte record terminator.
// The original renderer types pairs of bytes, not Unicode code points.
struct EndingText {
    static constexpr std::size_t line_bytes = 44, record_bytes = 46;
    using Line = std::array<std::byte, line_bytes>;
    std::vector<Line> lines;

    static EndingText decode(std::span<const std::byte> file, std::string_view asset) {
        const std::size_t count = asset == "END1.TXT" ? 51 :
            asset == "END2.TXT" ? 94 : asset == "END3.TXT" ? 12 : 0;
        if (!count || file.size() != count * record_bytes) {
            throw std::runtime_error("Invalid TH02 ending text asset or length");
        }
        EndingText result;
        result.lines.resize(count);
        for (std::size_t i = 0; i < count; ++i) {
            const auto record = file.subspan(i * record_bytes, record_bytes);
            if (record[44] != std::byte{13} || record[45] != std::byte{10}) {
                throw std::runtime_error("Invalid TH02 ending text line terminator");
            }
            std::copy_n(record.begin(), line_bytes, result.lines[i].begin());
        }
        return result;
    }
};

enum class EndingInk { Yellow = 6, Green = 9, White = 15 };
enum class EndingFade { BlackIn, BlackOut, WhiteIn, WhiteOut };
enum class EndingStaffText { Title, Program, Graphics, Music, TestPlayers };
enum class EndingEventKind {
    LoadText, PlaySong, LoadFont, LoadPicture, ShowQuarter, FullPicture,
    ApplyPicturePalette, LoadPalette, SetTone, Fade, WaitFrames, WaitMeasure,
    TypeLine, ClearLine, ScrollMarisa, SlideToStaff,
    StaffTitleCels, StaffVersion, StaffTitleDown, StaffText, StaffRotate,
    VerdictBackdrop, VerdictRow, VerdictSkillValue, WaitKey, ClearScreen,
    RegisterScore
};

struct EndingEvent {
    EndingEventKind kind;
    std::string_view asset{};
    int a = 0, b = 0, c = 0;
    EndingInk ink = EndingInk::White;
    bool fast_and_clear = false;
    friend bool operator==(const EndingEvent&, const EndingEvent&) = default;
};
// Event operands:
// LoadPicture: a=apply embedded palette now (0 defers or leaves unchanged).
// ShowQuarter: a=quarter (0,1 above 2,3), b=destination X, top always 100.
// Fade: a=EndingFade, b=original master.lib speed argument (not total frames).
// TypeLine: a=0-based record, b=frames/pair, asset=active text file.
// WaitMeasure: a=total played measures, b=frames if no sound driver is active.
// ScrollMarisa: a=incoming quarter, b=pixels/step, c=steps (1 frame each).
// SlideToStaff: a=start X, b=pixels/step left, c=steps (1 frame each).
// StaffTitleCels: a=cel count, b=frames/cel, c=BFNT segments/cel.
// StaffTitleDown: a=initial title Y, b=pixels/step, c=steps (1 frame each).
// StaffText: a=EndingStaffText, b=frames per pair for Title (0: immediate).
// StaffRotate: a=quarter, b=angle speed and c=initial angle (8-bit angle units).
// It runs 20 rotation frames, blits the quarter, waits 4, and restores tone 100.
// VerdictRow / VerdictSkillValue: a=row (mask reveal: 3 masks, 10 frames each).
// WaitKey is the original release-then-press gate; not a text advance shortcut.

// Original hardcoded staff/verdict labels, transcoded from th02/shiftjis/end.hpp.
struct EndingStaffLine { std::u8string_view text; int x, y; };
inline constexpr std::u8string_view ending_version = u8"ver 1.00";
inline constexpr std::array<std::u8string_view, 9> ending_verdict_labels = {
    u8"Ｔｈａｎｋ　ｙｏｕ　ｆｏｒ　ｐｌａｙｉｎｇ", u8"　最終得点", u8"コンティニュー", u8"　 ランク",
    u8"プレイヤー初期", u8" 霊撃初期数", u8"あなたの腕前", u8"", u8"１９９７　ＺＵＮ  (Amusement Makers)"
};
inline constexpr std::array<std::string_view, 4> ending_verdict_rank = {
    "  EASY ", " NORMAL", "  HARD ", "LUNATIC"
};
inline constexpr std::array<std::u8string_view, 9> ending_verdict_labels_en = {
    u8"Thank you for playing", u8"Final score", u8"Continues", u8"Difficulty",
    u8"Starting players", u8"Starting bombs", u8"Skill rating", u8"", u8"１９９７　ＺＵＮ  (Amusement Makers)"
};
inline constexpr std::array<std::u8string_view, 9> ending_verdict_labels_hans = {
    u8"感谢游玩", u8"最终得分", u8"续关次数", u8"难度", u8"初始残机", u8"初始灵击", u8"技术评价", u8"", u8"１９９７　ＺＵＮ  (Amusement Makers)"
};
inline constexpr std::array<std::u8string_view, 9> ending_verdict_labels_hant = {
    u8"感謝遊玩", u8"最終得分", u8"續關次數", u8"難度", u8"初始殘機", u8"初始靈擊", u8"技術評價", u8"", u8"１９９７　ＺＵＮ  (Amusement Makers)"
};
inline constexpr std::array<std::u8string_view,4> ending_verdict_rank_hans={u8"简单",u8"普通",u8"困难",u8"疯狂"};
inline constexpr std::array<std::u8string_view,4> ending_verdict_rank_hant={u8"簡單",u8"普通",u8"困難",u8"瘋狂"};
inline std::span<const EndingStaffLine> ending_staff_lines(EndingStaffText section, Language language=Language::Japanese) {
    if(is_chinese(language)) {
        const bool hans=language==Language::ChineseSimplified;
        static constexpr EndingStaffLine title_hans[]={{u8"东方封魔录制作组名单",432,192}};
        static constexpr EndingStaffLine title_hant[]={{u8"東方封魔錄製作組名單",432,192}};
        static constexpr EndingStaffLine program_hans[]={{u8"程序：ZUN",416,192}};
        static constexpr EndingStaffLine program_hant[]={{u8"程式：ZUN",416,192}};
        static constexpr EndingStaffLine graphics_hans[]={{u8"美术：ZUN",416,192},{u8"高附日向",416,224},{u8"（全通关插画）",416,240}};
        static constexpr EndingStaffLine graphics_hant[]={{u8"美術：ZUN",416,192},{u8"高附日向",416,224},{u8"（全通關插畫）",416,240}};
        static constexpr EndingStaffLine music_hans[]={{u8"音乐：ZUN",432,192}};
        static constexpr EndingStaffLine music_hant[]={{u8"音樂：ZUN",432,192}};
        static constexpr EndingStaffLine testers_hans[]={{u8"测试玩家",416,192},{u8"かおる",440,224},{u8"C-W",440,248},{u8"山田　恵一郎",440,272},{u8"及其他各位",440,296}};
        static constexpr EndingStaffLine testers_hant[]={{u8"測試玩家",416,192},{u8"かおる",440,224},{u8"C-W",440,248},{u8"山田　恵一郎",440,272},{u8"及其他各位",440,296}};
        switch(section) {
        case EndingStaffText::Title:return hans?title_hans:title_hant;
        case EndingStaffText::Program:return hans?program_hans:program_hant;
        case EndingStaffText::Graphics:return hans?graphics_hans:graphics_hant;
        case EndingStaffText::Music:return hans?music_hans:music_hant;
        case EndingStaffText::TestPlayers:return hans?testers_hans:testers_hant;
        }
    }
    if(language==Language::English){
        // The title keeps ten full-width glyphs and the original reveal schedule.
        static constexpr EndingStaffLine title[] = {{u8"ＳＴＡＦＦ　ＲＯＬＬ", 432, 192}};
        static constexpr EndingStaffLine program[] = {{u8"Programming: ZUN", 416, 192}};
        static constexpr EndingStaffLine graphics[] = {{u8"Graphics: ZUN", 416, 192}, {u8"高附日向", 416, 224}, {u8"(All-clear illustration)", 416, 240}};
        static constexpr EndingStaffLine music[] = {{u8"Music: ZUN", 432, 192}};
        static constexpr EndingStaffLine testers[] = {{u8"Test players", 416, 192}, {u8"かおる", 440, 224}, {u8"C-W", 440, 248}, {u8"山田　恵一郎", 440, 272}, {u8"and many others", 440, 296}};
        switch(section){
        case EndingStaffText::Title: return title;
        case EndingStaffText::Program: return program;
        case EndingStaffText::Graphics: return graphics;
        case EndingStaffText::Music: return music;
        case EndingStaffText::TestPlayers: return testers;
        }
    }
    static constexpr EndingStaffLine title[] = {{u8"封魔録　　ＳＴＡＦＦ", 432, 192}};
    static constexpr EndingStaffLine program[] = {{u8"プログラム　　　ＺＵＮ", 416, 192}};
    static constexpr EndingStaffLine graphics[] = {
        {u8"グラフィック　ＺＵＮ", 416, 192}, {u8"　　　　　　　高附日向", 416, 224},
        {u8"　　　　（オールクリア画）", 416, 240}
    };
    static constexpr EndingStaffLine music[] = {{u8"ＭＵＳＩＣ　　ＺＵＮ", 432, 192}};
    static constexpr EndingStaffLine testers[] = {
        {u8"ＴＥＳＴ　ＰＬＡＹＥＲ", 416, 192}, {u8"　　　　かおる", 416, 224},
        {u8"　　　　Ｃ－Ｗ", 416, 248}, {u8"　　 山田　恵一郎", 416, 272},
        {u8" その他大勢のみなさん", 416, 296}
    };
    switch (section) {
    case EndingStaffText::Title: return title;
    case EndingStaffText::Program: return program;
    case EndingStaffText::Graphics: return graphics;
    case EndingStaffText::Music: return music;
    case EndingStaffText::TestPlayers: return testers;
    }
    return {};
}

struct EndingVerdict {
    std::uint32_t score;
    unsigned continues_used, rank;
    int initial_players, initial_bombs, skill, title_line;
};

constexpr EndingVerdict ending_verdict(const EndingRun& run) {
    const int skill = std::clamp(run.skill, 0, 100);
    const int title = skill == 100 ? 0 : skill == 77 ? 3 :
        skill >= 70 && skill < 80 ? 4 : (100 - (skill / 10) * 10) / 10 + (skill < 70);
    return {run.score, run.continues_used, run.rank, run.start_lives + 1, run.start_bombs, skill, title};
}

struct EndingScript {
    EndingBranch branch;
    EndingVerdict verdict;
    std::vector<EndingEvent> events;
    struct StaffRange {
        std::size_t begin = 0, end = 0;
        [[nodiscard]] bool contains(std::size_t index) const { return begin <= index && index < end; }
    } staff; // Empty for Extra; the verdict is not a Staff event.
};

// Original event order from ReC98 th02/end/end.cpp:379–1118 and
// th02_maine.asm:132–196, 252–286. No synthetic credits or narration.
inline EndingScript make_ending_script(const EndingRun& run) {
    if (run.shot_type > 2 || run.rank > 4) throw std::invalid_argument("Invalid TH02 ending route");
    const auto branch = run.rank == 4 ? EndingBranch::Extra :
        run.continues_used ? EndingBranch::Bad : EndingBranch::Good;
    EndingScript script{branch, ending_verdict(run), {}, {}};
    using K = EndingEventKind;
    using F = EndingFade;
    using I = EndingInk;
    using T = EndingStaffText;
    const auto emit = [&](K kind, std::string_view asset = {}, int a = 0, int b = 0, int c = 0) {
        script.events.push_back({kind, asset, a, b, c});
    };
    const auto wait = [&](int frames) { emit(K::WaitFrames, {}, frames); };
    const auto fade = [&](F type, int speed) { emit(K::Fade, {}, int(type), speed); };
    const auto measure = [&](int n) { emit(K::WaitMeasure, {}, n, 100); };
    const auto extra_card = [&](std::string_view final) {
        emit(K::FullPicture, "ALL.PI");
        fade(F::BlackIn, 2);
        wait(150);
        emit(K::FullPicture, final);
        emit(K::WaitKey);
        fade(F::BlackOut, 5);
    };
    emit(K::LoadFont, "MIKOFT.BFT");
    emit(K::LoadFont, "ENDFT.BFT");
    wait(100);
    if (branch == EndingBranch::Extra) {
        if (run.extra_unlocked) {
            emit(K::SetTone, {}, 0);
            extra_card("EXTRA.PI");
        }
    } else {
        const auto file = branch == EndingBranch::Bad ? "END1.TXT" : "END2.TXT";
        const auto lines = [&](int first, int last, I color = I::White, int speed = 6, bool auto_clear = true) {
            for (int i = first; i <= last; ++i) {
                script.events.push_back({K::TypeLine, file, i, speed, 0, color, auto_clear});
            }
        };
        const auto picture = [&](std::string_view asset, bool apply = true) {
            emit(K::LoadPicture, asset, apply ? 1 : 0);
        };
        const auto show = [&](int quarter) { emit(K::ShowQuarter, {}, quarter, 160); };
        const auto palette = [&](std::string_view asset) { emit(K::LoadPalette, asset); };
        emit(K::LoadText, file);
        emit(K::PlaySong, "END1.M");
        emit(K::SetTone, {}, 0);
        picture("ED01.PI");
        fade(F::BlackIn, 2);
        wait(40);
        show(0);
        lines(0, 0);
        wait(20);
        fade(F::BlackOut, 1);

        if (branch == EndingBranch::Bad) {
            show(1);
            fade(F::BlackIn, 1);
            lines(1, 3);
            show(2);
            lines(4, 5);
            lines(6, 6, I::Yellow);
            lines(7, 9);
            lines(10, 10, I::Yellow);
            lines(11, 11);
            wait(20);
            lines(12, 12);
            emit(K::ScrollMarisa, {}, 3, 2, 100);
            lines(13, 13);
            lines(14, 15, I::Yellow);
            lines(16, 17);
            lines(18, 20, I::Yellow);
            lines(21, 21);
            lines(22, 22, I::Yellow);
            lines(23, 23);
            picture("ED02.PI"); // The source changes palettes before blacking out.
            fade(F::BlackOut, 2);
            show(std::array{0, 2, 3}[run.shot_type]);
            fade(F::BlackIn, 2);
            lines(24, 24, I::Yellow);
            lines(25, 26);
            switch (run.shot_type) {
            case 0:
                lines(27, 28, I::Yellow);
                lines(29, 30);
                show(1);
                lines(31, 31, I::Yellow);
                lines(32, 32, I::White, 12);
                lines(33, 33, I::White, 9, false);
                break;
            case 1:
                lines(34, 34, I::Yellow);
                lines(35, 35);
                lines(36, 36, I::Yellow);
                lines(37, 38);
                lines(39, 39, I::Yellow);
                lines(40, 40, I::White, 12);
                lines(41, 41, I::White, 9, false);
                break;
            case 2:
                lines(42, 42, I::Yellow);
                lines(43, 43);
                lines(44, 45, I::Yellow);
                lines(46, 47);
                lines(48, 48, I::Yellow);
                lines(49, 49, I::White, 12);
                lines(50, 50, I::White, 9, false);
                break;
            }
        } else {
            picture("ED03.PI");
            show(0);
            fade(F::BlackIn, 1);
            lines(1, 3);
            show(1);
            lines(4, 9);
            show(2);
            lines(10, 11);
            show(3);
            lines(12, 13);
            palette("ED03A.RGB");
            lines(14, 15, I::Green);
            fade(F::BlackOut, 2);
            picture("ED04.PI");
            show(0);
            fade(F::BlackIn, 2);
            lines(16, 16);
            lines(17, 17, I::Green);
            lines(18, 19);
            show(1);
            lines(20, 21, I::Green);
            lines(22, 22);
            lines(23, 24, I::Green);
            lines(25, 25);
            lines(26, 26, I::Green);
            wait(10);
            switch (run.shot_type) {
            case 0:
                lines(27, 27, I::Green);
                wait(30);
                show(2);
                lines(28, 28);
                show(3);
                lines(29, 29);
                lines(30, 31, I::Green);
                lines(32, 36);
                lines(37, 38, I::Green);
                lines(39, 39);
                break;
            case 1:
                lines(48, 49, I::Green);
                lines(50, 50);
                lines(51, 51, I::Green);
                wait(30);
                show(2);
                lines(52, 52);
                show(3);
                lines(53, 53);
                lines(54, 54, I::Green);
                lines(55, 55);
                lines(56, 56, I::Green);
                lines(57, 57);
                lines(58, 61, I::Green);
                lines(62, 66);
                break;
            case 2:
                lines(71, 71, I::Green);
                lines(72, 72);
                lines(73, 73, I::Green);
                wait(30);
                show(2);
                lines(74, 74);
                show(3);
                lines(75, 75);
                lines(76, 76, I::Green);
                lines(77, 77);
                lines(78, 78, I::Green);
                lines(79, 79);
                lines(80, 80, I::Green);
                lines(81, 81);
                lines(82, 82, I::Green);
                lines(83, 83);
                break;
            }
            fade(F::BlackOut, 2);
            picture("ED05.PI");
            show(int(run.shot_type));
            fade(F::BlackIn, 2);
            switch (run.shot_type) {
            case 0:
                lines(40, 46);
                lines(47, 47, I::White, 12, false);
                break;
            case 1:
                lines(67, 68);
                lines(69, 69, I::White, 12);
                lines(70, 70, I::White, 12, false);
                break;
            case 2:
                lines(84, 92);
                lines(93, 93, I::White, 12, false);
                break;
            }
        }

        // The last ending line is held through the first part of the transition.
        emit(K::LoadText, "END3.TXT");
        wait(30);
        fade(F::WhiteOut, 1);
        script.staff.begin = script.events.size();
        emit(K::PlaySong, "ENDING.M");
        emit(K::ClearLine);
        fade(F::WhiteIn, 4);
        measure(5);
        emit(K::SlideToStaff, {}, 160, 4, 31); // Original stops at X=36, not 32.
        measure(6);
        emit(K::StaffTitleCels, "ENDFT.BFT", 11, 4, 5);
        emit(K::StaffVersion);
        measure(8);
        emit(K::StaffTitleDown, {}, 184, 4, 46);
        measure(9);
        emit(K::StaffText, {}, int(T::Title), 12);
        measure(13);

        const auto staff = [&](int quarter, int speed, int angle) {
            emit(K::StaffRotate, {}, quarter, speed, angle);
        };
        const auto staff_text = [&](T section) { emit(K::StaffText, {}, int(section)); };
        picture("ED06.PI");
        staff(0, 0x04, 0x29);
        staff_text(T::Program);
        measure(17);
        palette("ED06B.RGB");
        staff(2, -0x04, 0x29);
        measure(21);
        emit(K::ShowQuarter, {}, 3, 32);
        palette("ED06C.RGB");
        staff(3, 0x04, 0x29);
        picture("ED07.PI", false);
        measure(25);
        staff_text(T::Graphics);
        palette("ED07A.RGB");
        staff(0, -0x04, 0x29);
        measure(29);
        palette("ED07B.RGB");
        staff(1, 0x08, -0x17);
        measure(33);
        emit(K::ShowQuarter, {}, 2, 32);
        staff(2, -0x08, -0x17);
        measure(37);
        staff_text(T::Music);
        palette("ED07B.RGB");
        staff(3, 0x08, -0x17);
        picture("ED08.PI", false);
        measure(41);
        emit(K::ApplyPicturePalette);
        staff(0, -0x08, -0x17);
        measure(45);
        palette("ED08A.RGB");
        staff(1, 0x0C, 0x29);
        measure(49);
        staff_text(T::TestPlayers);
        palette("ED08B.RGB");
        staff(2, -0x0C, 0x29);
        measure(53);
        palette("ED08C.RGB");
        staff(3, 0x0C, 0x29);
        measure(57);
        fade(F::BlackOut, 4);

        script.staff.end = script.events.size();
        emit(K::VerdictBackdrop, "ED09.PI");
        fade(F::BlackIn, 4);
        wait(100);
        constexpr std::array row_waits{200, 100, 100, 100, 100, 150, 120};
        for (int row = 0; row < int(row_waits.size()); ++row) {
            emit(K::VerdictRow, {}, row);
            wait(row_waits[row]);
        }
        emit(K::VerdictSkillValue, "END3.TXT", 6, script.verdict.title_line);
        wait(200);
        emit(K::VerdictRow, {}, 8);
        emit(K::WaitKey);
        fade(F::BlackOut, 5);
        if (run.extra_unlocked && run.continues_used == 0) extra_card("BUT.PI");
        emit(K::ClearScreen);
    }
    emit(K::SetTone, {}, 50);
    emit(K::RegisterScore);
    emit(K::SetTone, {}, 0);
    return script;
}

} // namespace soew
