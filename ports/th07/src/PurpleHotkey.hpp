// Generated from purple THPrac (MIT), TH07 source sha256 b5be6dc698f574992ab49696c5c0d8ad756af93bf0760be1504ee9e958bee1b0.
#pragma once
#include <imgui.h>
#include <string>
namespace PracticeRuntime {
        inline bool PurpleHotkeyWidget(const char* mKeyText, const char* text, bool mStatus)
        {
            std::string realText;
            if (mStatus) {
                realText = std::string("[") + mKeyText + ": " + text + "]";
                ImGui::PushStyleColor(ImGuiCol_Text, { 0.0f, 1.0f, 0.0f, 1.0f });
            } else {
                realText = std::string(mKeyText) + ": " + text;
            }

            auto cursor = ImGui::GetCursorPos();
            ImGui::TextUnformatted(realText.c_str());
            ImGui::SetCursorPos(cursor);

            if (mStatus)
                ImGui::PopStyleColor();

            if (ImGui::InvisibleButton(mKeyText, ImGui::CalcTextSize(realText.c_str())))
                return true;
            else
                return false;
        }
inline void DrawPurpleHotkey(const char* key, const char* text, bool& status) {
    if (PurpleHotkeyWidget(key, text, status)) status = !status;
}
inline constexpr const char* purple_THPRAC_FAST_RETRY[3]{"练习模式快速re","fast retry for prac mode","fast retry for prac mode"};
inline constexpr const char* purple_THPRAC_FAST_RETRY_DESC2[3]{"快捷键: shift+F","hotkey shift+F","hotkey shift+F"};
inline constexpr const char* purple_THPRAC_AUTO_SHOOT[3]{"自动射击","auto shoot","auto shoot"};
inline constexpr const char* purple_THPRAC_INGAMEINFO_07_CHERRY[3]{"Cherry:","Cherry:","Cherry:"};
inline constexpr const char* purple_THPRAC_INGAMEINFO_07_CHERRY_PLUS[3]{"CherryP:","CherryP:","CherryP:"};
inline constexpr const char* purple_THPRAC_INGAMEINFO_07_CHERRY_MAX[3]{"CherryM:","CherryM:","CherryM:"};
inline constexpr const char* purple_THPRAC_INGAMEINFO[3]{"更多游戏信息","in game info","in game info"};
inline constexpr const char* purple_TH_ENEMY_MUTEKI[3]{"敌人无敌","Enemy Invincible","敵無敵"};
inline constexpr const char* purple_TH_ADV_DISABLE_X_KEY[3]{"禁用X键","disable X key","disable X key"};
inline constexpr const char* purple_TH_ADV_DISABLE_SHIFT_KEY[3]{"禁用Shift键","disable shift key","disable shift key"};
inline constexpr const char* purple_TH_ADV_DISABLE_Z_KEY[3]{"禁用Z键","disable Z key","disable Z key"};
inline constexpr const char* purple_TH_ADV_DISABLE_C_KEY_SAMETIME[3]{"禁用X同时禁用C键","disable C key at the same time when disable X","disable C key at the same time when disable X"};
inline constexpr const char* purple_TH_ADV_FORCE_SHIFT_KEY[3]{"始终按住Shift键","force press shift key","force press shift key"};
inline constexpr const char* purple_THPRAC_INFLIVES_MAP[3]{"将锁残修改为不疮","map inf.lives to no continue","map inf.lives to no continue"};
inline constexpr const char* purple_TH_ENABLE_LOCK_TIMER[3]{"启用锁时计时","enable lock timer","enable lock timer"};
inline constexpr const char* purple_THPRAC_SHOW_BULLET_HITBOX[3]{"显示弹幕判定(仅练习/rep)","show bullet hitbox(only prac/rep)","show bullet hitbox(only prac/rep)"};
inline constexpr const char* purple_TH_ONE_KEY_DIE[3]{"一键疮痍","one key DIE","one key DIE"};
inline constexpr const char* purple_TH_ONE_KEY_DIE_DESC[3]{"为了在某些作品的练习模式中快速存rep","for rep storage in practice mode","for rep storage in practice mode"};
inline constexpr const char* purple_THPRAC_KB_OPEN[3]{"打开按键显示","open key monitor","open key monitor"};
inline constexpr const char* purple_THPRAC_KB_RECORD_START[3]{"开始记录APS","start to record APS","start to record APS"};
inline constexpr const char* purple_THPRAC_KB_RECORD_STOP[3]{"停止记录APS","stop recording","stop recording"};
inline constexpr const char* purple_THPRAC_KB_OUTPUT[3]{"导出到游戏文件夹","output to game folder","output to game folder"};
inline constexpr const char* purple_THPRAC_TH07_ALWAYS_SAVE_SCORE_SPELL_HIST[3]{"妖妖梦始终保存存档","always save score/spell history in PCB","always save score/spell history in PCB"};
inline constexpr const char* purple_THPRAC_TH07_ALWAYS_SAVE_SCORE_SPELL_HIST_DESC[3]{"例如练习模式re时","e.g. retry in practice","e.g. retry in practice"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_TEST[3]{"反应力测试","reaction test","reaction test"};
inline constexpr const char* purple_THPRAC_BACK[3]{"返回","Back","戻る"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_TEST_DOWN[3]{"按下反应测试(等待变色后按下方向键)","keydown test(wait for color changed then press arrow key)","keydown test(wait for color changed then press arrow key)"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_TEST_UP[3]{"弹起反应测试(按下方向键后,等待变色时弹起)","keyup test(press arrow key until color changes)","keyup test(press arrow key until color changes)"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_SHOW_PROC_BAR[3]{"显示等待时间进度条","show wait time progress bar","show wait time progress bar"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_TEST_TIME[3]{"测试次数","times for test","times for test"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_TEST_BEGIN[3]{"开始测试(快捷键z/shift)","start(hotkey z/shift)","start(hotkey z/shift)"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_TEST_TOO_EARLY[3]{"按太快了","too early","too early"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_TEST_NEXT_TEST[3]{"再测一次(快捷键z/shift)","next test(hotkey z/shift)","next test(hotkey z/shift)"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_TEST_RESULT[3]{"结果","result","result"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_MODE[3]{"模式","mode","mode"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_MODE_DOWN[3]{"按下测试","press time test","press time test"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_MODE_UP[3]{"弹起测试","release time test","release time test"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_MODE_PROGRESSBAR[3]{"显示进度条","with progress bar","with progress bar"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_MODE_NORMAL[3]{"不显示进度条","no progress bar","no progress bar"};
inline constexpr const char* purple_THPRAC_TOOLS_REACTION_TEST_RESULT_AVG[3]{"结果(平均值)","result(avg)","result(avg)"};
}
