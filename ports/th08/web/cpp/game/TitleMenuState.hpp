#pragma once

namespace th08 {
enum TitleCurrentScreen
{
    TitleCurrentScreen_StartMenu = 0,
    TitleCurrentScreen_Option = 2,
    TitleCurrentScreen_KeyConfig = 3,
    TitleCurrentScreen_DifficultySelect = 4,
    TitleCurrentScreen_CharacterSelect = 5,
    TitleCurrentScreen_ShotSelect = 6, /* Leftover from PCB */
    TitleCurrentScreen_Replay = 7,
    TitleCurrentScreen_DifficultySelectPractice = 8,
    TitleCurrentScreen_CharacterSelectPractice = 9,
    TitleCurrentScreen_ShotSelectPractice = 10, /* Leftover from PCB */
    TitleCurrentScreen_PracticeStageSelect = 11,
    TitleCurrentScreen_DifficultySelectExtra = 12,
    TitleCurrentScreen_CharacterSelectExtra = 13,
    TitleCurrentScreen_ShotSelectExtra = 14,      /* Leftover from PCB */
    TitleCurrentScreen_CharacterSelectSpell = 15, /* Seems to be an earlier feature, the current scene
                                                   * is never set to this value, but it checks against
                                                   * this value.
                                                   */
    TitleCurrentScreen_SpellStageSelect = 16,
    TitleCurrentScreen_SpellCardSelect = 17,
};

enum TitleScreenState
{
    TitleScreenState_Ready = 0,
    TitleScreenState_Loading = 1,
    TitleScreenState_Close = 2,
};

enum TitleCurrentScreenState
{
    TitleCurrentScreenState_Init = 0,
    TitleCurrentScreenState_Ready = 1,
    TitleCurrentScreenState_Exit = 2,
    TitleCurrentScreenState_Changing = 3,
};

// The start screen becomes confirmable after its ten-tick startup delay.
// Other screens must be explicitly listed when their confirmation gate opens.
constexpr bool title_confirmation_ready(int currentScreen, int currentScreenState, int stateTimer2) noexcept
{
    if (currentScreenState != TitleCurrentScreenState_Ready) return false;
    switch (currentScreen) {
    case TitleCurrentScreen_StartMenu:
        return stateTimer2 >= 10;
    case TitleCurrentScreen_DifficultySelect:
        return true;
    default:
        return false;
    }
}
}
