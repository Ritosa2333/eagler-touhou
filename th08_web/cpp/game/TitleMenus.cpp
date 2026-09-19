// TH08 title/start/options/controller/difficulty/character/practice menus.
// Adapted from GensokyoClub/th08 under MIT; see cpp/licenses.
#include "TitleMenus.hpp"
#include "TitleText.hpp"
#include "TitleSpellText.hpp"
#include "Localization.hpp"
namespace th08 {
#define TITLE_SPRITE_OPTION_START 10

#define TITLE_SPRITE_OPTION_PLAYER_START 20
#define TITLE_SPRITE_OPTION_PLAYER_END 26

#define TITLE_SPRITE_OPTION_GRAPHICS_MODE_START 27
#define TITLE_SPRITE_OPTION_GRAPHICS_MODE_END 28

#define TITLE_SPRITE_OPTION_MUSIC_MODE_START 29
#define TITLE_SPRITE_OPTION_MUSIC_MODE_END 31

#define TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3 32
#define TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2 33
#define TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT1 34

#define TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3 36
#define TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2 37
#define TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT1 38

#define TITLE_SPRITE_OPTION_WINDOWED_START 40
#define TITLE_SPRITE_OPTION_WINDOWED_END 41

#define TITLE_SPRITE_OPTION_SLOWMODE_START 42
#define TITLE_SPRITE_OPTION_SLOWMODE_END 43

#define TITLE_SPRITE_KEYCONFIG_SLOWSHOT_START 75
#define TITLE_SPRITE_KEYCONFIG_SLOWSHOT_END 76

#define TITLE_SPRITE_CHARACTER_START 111
#define TITLE_SPRITE_CHARACTER_END 130
#define TITLE_SPRITE_DIFFICULTY_START 131
#define TITLE_SPRITE_DIFFICULTY_EXTRA (TITLE_SPRITE_DIFFICULTY_START + 4)



enum
{
    TITLE_MENU_ITEM_START_START = 0,
    TITLE_MENU_ITEM_START_EXTRA_START = 1,
    TITLE_MENU_ITEM_START_SPELL_PRACTICE = 2,
    TITLE_MENU_ITEM_START_PRACTICE_START = 3,
    TITLE_MENU_ITEM_START_REPLAY = 4,
    TITLE_MENU_ITEM_START_RESULT = 5,
    TITLE_MENU_ITEM_START_MUSIC_ROOM = 6,
    TITLE_MENU_ITEM_START_OPTION = 7,
    TITLE_MENU_ITEM_START_QUIT = 8,
    TITLE_MENU_ITEM_START_NUM_ITEMS
};

enum
{
    TITLE_MENU_ITEM_OPTION_PLAYER = 0,
    TITLE_MENU_ITEM_OPTION_GRAPHIC = 1,
    TITLE_MENU_ITEM_OPTION_BGM = 2,
    TITLE_MENU_ITEM_OPTION_VOL = 3,
    TITLE_MENU_ITEM_OPTION_SE_VOL = 4,
    TITLE_MENU_ITEM_OPTION_MODE = 5,
    TITLE_MENU_ITEM_OPTION_SLOWMODE = 6,
    TITLE_MENU_ITEM_OPTION_RESET = 7,
    TITLE_MENU_ITEM_OPTION_KEYCONFIG = 8,
    TITLE_MENU_ITEM_OPTION_EXIT = 9,
    TITLE_MENU_ITEM_OPTION_NUM_ITEMS
};

enum
{
    TITLE_MENU_ITEM_KEYCONFIG_SHOT = 0,
    TITLE_MENU_ITEM_KEYCONFIG_BOMB = 1,
    TITLE_MENU_ITEM_KEYCONFIG_SLOW = 2,
    TITLE_MENU_ITEM_KEYCONFIG_SKIP = 3,
    TITLE_MENU_ITEM_KEYCONFIG_PAUSE = 4,
    TITLE_MENU_ITEM_KEYCONFIG_UP = 5,
    TITLE_MENU_ITEM_KEYCONFIG_DOWN = 6,
    TITLE_MENU_ITEM_KEYCONFIG_LEFT = 7,
    TITLE_MENU_ITEM_KEYCONFIG_RIGHT = 8,
    TITLE_MENU_ITEM_KEYCONFIG_SHOTSLOW = 9,
    TITLE_MENU_ITEM_KEYCONFIG_RESET = 10,
    TITLE_MENU_ITEM_KEYCONFIG_QUIT = 11,
};

i32 g_TitleCharacterSpriteIndices[12][4] = {
    /* Team character sprites */
    {
        0x77,
        0x6f,
        0x70,
        -1,
    },
    {
        0x78,
        0x72,
        0x71,
        -1,
    },
    {
        0x79,
        0x73,
        0x74,
        -1,
    },
    {
        0x7a,
        0x76,
        0x75,
        -1,
    },

    /* Solo character sprites */
    {0x7b, 0x6f, -1, 0x70},
    {0x7c, 0x70, -1, 0x6f},
    {0x7d, 0x72, -1, 0x71},
    {0x7e, 0x71, -1, 0x72},
    {0x7f, 0x73, -1, 0x74},
    {0x80, 0x74, -1, 0x73},
    {0x81, 0x76, -1, 0x75},
    {0x82, 0x75, -1, 0x76},
};


#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#define ARRAY_SIZE_SIGNED(a) i32(ARRAY_SIZE(a))
#define WAS_PRESSED(mask) ((context.input.current&(mask))&&((context.input.current&(mask))!=(context.input.previous&(mask))))
#define WAS_PRESSED_SCROLLING(mask) (WAS_PRESSED(mask)||((context.input.current&(mask))&&context.input.scrolling))
#define IS_STAGE_CLEARED(info,stage) ((info)&(1u<<(stage)))
#define ZUN_BIT(n) (1u<<(n))
constexpr i32 TRUE=1,FALSE=0,ZUN_SUCCESS=0,OFF=0,WAV=1,MIDI=2,EASY=0,NORMAL=1,HARD=2,EXTRA=4,STAGE1=0,STAGE4A=3,STAGE4B=4,STAGE6B=7;
constexpr i32 TH_BUTTON_SHOOT=1,TH_BUTTON_BOMB=2,TH_BUTTON_MENU=8,TH_BUTTON_UP=16,TH_BUTTON_DOWN=32,TH_BUTTON_LEFT=64,TH_BUTTON_RIGHT=128,TH_BUTTON_ENTER=4096;
constexpr i32 SOUND_SELECT=10,SOUND_BACK=11,SOUND_MOVE_MENU=12,SOUND_TIMEOUT=29;
constexpr i32 SupervisorState_ResultScreen=5,SupervisorState_GameManager=2,SupervisorState_MusicRoom=8,SupervisorState_ExitGame=-1;
constexpr i32 CHAIN_CALLBACK_RESULT_CONTINUE=1,CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB=0,CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN=2,CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR=5;
constexpr i32 TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE=15,SHOT_ALL=12,COLOR_TEXT_WHITE=0xffffff,SOUND_INVALID_ACTION=41,STAGE_LAST_WORD=9;
constexpr i32 STAGE2=1,STAGE3=2,STAGE5=5,STAGE6A=6,EXTRASTAGE=8;
enum {SPELLCARD_LAST_WORD_START=205,SPELLCARD_LW_WRIGGLE=205,SPELLCARD_LW_MYSTIA,SPELLCARD_LW_KEINE,SPELLCARD_LW_REISEN,SPELLCARD_LW_EIRIN,SPELLCARD_LW_KAGUYA,SPELLCARD_LW_MOKOU,SPELLCARD_LW_TEWI,SPELLCARD_LW_KEINEEX,SPELLCARD_LW_REIMU,SPELLCARD_LW_MARISA};
static Vec3 Float3(float x,float y,float z){return {x,y,z};}
i32 TitleMenus::OnUpdateStartMenu()
{
    i32 i;
    i32 fileSize;

    switch (state.currentScreenState)
    {
    case TitleCurrentScreenState_Init:
        if (state.stateTimer2 == 0)
        {
            if (state.previousScreen == TitleCurrentScreen_StartMenu &&
                context.supervisor_previous != SupervisorState_ResultScreen)
            {
                actions.play_music(8, 0);
            }

            if (state.previousScreen == TitleCurrentScreen_StartMenu ||
                state.previousScreen == TitleCurrentScreen_DifficultySelect ||
                state.previousScreen == TitleCurrentScreen_Replay ||
                state.previousScreen == TitleCurrentScreen_DifficultySelectPractice ||
                state.previousScreen == TitleCurrentScreen_DifficultySelectExtra ||
                state.previousScreen == TitleCurrentScreen_CharacterSelectSpell ||
                state.previousScreen == TitleCurrentScreen_SpellStageSelect)
            {
                if (actions.load_surface(0, "title/title00.png") != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
                }
            }

            if (state.vmCount == 0)
            {
                state.vmCount = 142;
                // Most likely SprtInf, but it's inlined so we can't know for sure.
                state.vms = new AnmVm[state.vmCount];
                ExecuteAnmIdxArray(state.vms, 0, state.vmCount);
            }

            SetInterruptArray(state.vms, state.vmCount, 2);

            if (context.IsReplay())
            {
                this->ChangeCurrentScreen(TitleCurrentScreen_Replay);
                SetInterruptArray(state.vms, state.vmCount, 13);
                state.currentHelpTextVm->SetInterrupt(2);
                context.SetIsReplayWeird(FALSE);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            }

            if (state.practiceState != 0)
            {
                if (state.practiceState == 2)
                {
                    context.flags.isSpellPractice = TRUE;
                }

                this->ChangeCurrentScreen(context.IsSpellPractice()
                                              ? TitleCurrentScreen_CharacterSelectSpell
                                              : TitleCurrentScreen_DifficultySelectPractice);

                SetInterruptArray(state.vms, state.vmCount, 5);
                state.currentHelpTextVm->SetInterrupt(2);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            }

            /* Set each menu item's sprite. */
            for (i = 0; i < TITLE_MENU_ITEM_START_NUM_ITEMS; i++)
            {
                state.titleAnm->SetSprite(&state.vms[1 + i], state.vms[1 + i].baseSpriteIndex + 1);
            }

            /* Mark the selected menu item. */
            state.titleAnm->SetSprite(&state.vms[1 + state.cursor], state.vms[1 + state.cursor].baseSpriteIndex);

            /* Mark the "Spell Practice" button as grayed out. */
            if (!context.IsSpellPracticeUnlocked())
            {
                state.vms[3].color1.d3dColor = 0xff404040;
            }

            /* Mark the "Extra Start" button as grayed out. */
            if (!context.IsExtraUnlocked())
            {
                state.vms[2].color1.d3dColor = 0xff404040;
            }
        }

        if (state.stateTimer2 < ARRAY_SIZE(g_StartMenuHelpText))
        {
            DrawTextCentered(&state.helpTextVms[state.stateTimer2], 0xfff0e0, 0x300000,
                                           Localization::StringById(g_StartMenuHelpTextIds[state.stateTimer2],
                                                                    g_StartMenuHelpText[state.stateTimer2]));
            state.stateTimer2=wrapping_add(state.stateTimer2,1);

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }

        state.stateTimer2 = 0;
        state.stateTimer = 0;
        state.cursor2 = -1;
        state.currentScreenState = TitleCurrentScreenState_Ready;
        state.startMenuIdleFrames = 0;
    case TitleCurrentScreenState_Ready:
        i = this->MoveCursorVertical(9);
        if (i != 0)
        {
            /* ... Just why, ZUN */
        back:
            if (!context.IsSpellPracticeUnlocked())
            {
                if (state.cursor == TITLE_MENU_ITEM_START_SPELL_PRACTICE)
                {
                    state.cursor += i;
                    goto back;
                }
            }

            if (!context.IsExtraUnlocked())
            {
                if (state.cursor == TITLE_MENU_ITEM_START_EXTRA_START)
                {
                    state.cursor += i;
                    goto back;
                }
            }

            /* Set each menu item's sprite. */
            for (i = 0; i < TITLE_MENU_ITEM_START_NUM_ITEMS; i++)
            {
                state.titleAnm->SetSprite(&state.vms[1 + i], state.vms[1 + i].baseSpriteIndex + 1);
            }

            /* Mark the selected menu item. */
            state.titleAnm->SetSprite(&state.vms[1 + state.cursor], state.vms[1 + state.cursor].baseSpriteIndex);

            /* Mark the "Spell Practice" button as grayed out. */
            if (!context.IsSpellPracticeUnlocked())
            {
                state.vms[3].color1.d3dColor = 0xff404040;
            }

            /* Mark the "Extra Start" button as grayed out. */
            if (!context.IsExtraUnlocked())
            {
                state.vms[2].color1.d3dColor = 0xff404040;
            }
        }

        state.startMenuIdleFrames=wrapping_add(state.startMenuIdleFrames,1);
        if (context.input.current != 0)
        {
            state.startMenuIdleFrames = 0;
        }

        if (state.startMenuIdleFrames > 1500)
        {
            context.currentDemoReplay++;
            context.currentDemoReplay %= ARRAY_SIZE_SIGNED(g_DemoReplayFiles);
            strcpy(context.replayFilename, g_DemoReplayFiles[context.currentDemoReplay]);

            if(start_demo())return 0;
            state.startMenuIdleFrames=0;
        }

        if (state.cursor2 != state.cursor)
        {
            state.currentHelpTextVm = state.helpTextVms + state.cursor;
            state.currentHelpTextVm->SetInterrupt(1);
        }

        state.cursor2 = state.cursor;

        if (state.stateTimer2 < 10)
        {
            break;
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            actions.sound(SOUND_SELECT, 0);
            actions.process_sounds();

            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_START_START:
                context.flags.isPracticeMode = FALSE;
                context.flags.isSpellPractice = FALSE;
                state.cursor = config.difficulty;
                if (state.cursor >= EXTRA)
                {
                    state.cursor = HARD;
                }

                this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelect);
                SetInterruptArray(state.vms, state.vmCount, 5);
                state.currentHelpTextVm->SetInterrupt(2);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            case TITLE_MENU_ITEM_START_PRACTICE_START:
                context.flags.isPracticeMode = TRUE;
                context.flags.isSpellPractice = FALSE;

                state.cursor = config.difficulty;
                if (state.cursor >= EXTRA)
                {
                    state.cursor = HARD;
                }

                this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelectPractice);
                SetInterruptArray(state.vms, state.vmCount, 5);

                state.currentHelpTextVm->SetInterrupt(2);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            case TITLE_MENU_ITEM_START_EXTRA_START:
                if (context.IsExtraUnlocked())
                {
                    context.flags.isPracticeMode = FALSE;
                    context.flags.isSpellPractice = FALSE;

                    state.cursor = 0;
                    this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelectExtra);

                    SetInterruptArray(state.vms, state.vmCount, 5);
                    state.currentHelpTextVm->SetInterrupt(2);

                    return CHAIN_CALLBACK_RESULT_CONTINUE;
                }
            case TITLE_MENU_ITEM_START_SPELL_PRACTICE:
                if (context.IsSpellPracticeUnlocked())
                {
                    context.flags.isPracticeMode = TRUE;
                    context.flags.isSpellPractice = TRUE;

                    state.cursor = context.character;
                    this->ChangeCurrentScreen(TitleCurrentScreen_SpellStageSelect);

                    SetInterruptArray(state.vms, state.vmCount, 5);
                    state.currentHelpTextVm->SetInterrupt(2);

                    return CHAIN_CALLBACK_RESULT_CONTINUE;
                }
            case TITLE_MENU_ITEM_START_REPLAY:
                context.flags.isPracticeMode = FALSE;
                context.flags.isSpellPractice = FALSE;

                this->ChangeCurrentScreen(TitleCurrentScreen_Replay);

                SetInterruptArray(state.vms, state.vmCount, 13);
                state.currentHelpTextVm->SetInterrupt(2);

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            case TITLE_MENU_ITEM_START_MUSIC_ROOM:
                context.supervisor_state = SupervisorState_MusicRoom;
                state.currentHelpTextVm->SetInterrupt(2);
                return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
            case TITLE_MENU_ITEM_START_RESULT:
                context.supervisor_state = SupervisorState_ResultScreen;
                state.currentHelpTextVm->SetInterrupt(2);
                return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
            case TITLE_MENU_ITEM_START_OPTION:
                /* ??? */
                state.currentScreenState = TitleCurrentScreenState_Init;
                state.cursor = 0;
                state.stateTimer2 = 0;
                state.stateTimer = 0;
                state.currentScreenState = TitleCurrentScreenState_Changing;
                state.stateTimer = 0;
                this->OnUpdateOptions();
                state.cursor = 0;
                break;
            case TITLE_MENU_ITEM_START_QUIT:
                state.currentScreenState = TitleCurrentScreenState_Exit;
                state.stateTimer = 0;
                SetInterruptArray(state.vms, state.vmCount, 1);
                if (config.music == MIDI)
                {
                    actions.midi_reset();
                }
                break;
            }
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            state.titleAnm->SetSprite(&state.vms[state.cursor + 1], state.vms[state.cursor + 1].baseSpriteIndex + 1);
            state.cursor = TITLE_MENU_ITEM_START_QUIT;
            state.titleAnm->SetSprite(&state.vms[state.cursor + 1], state.vms[state.cursor + 1].baseSpriteIndex);
            actions.sound(SOUND_BACK, 0);
            actions.process_sounds();
        }
        break;
    case TitleCurrentScreenState_Exit:
        if (state.stateTimer >= 60)
        {
            delete[] state.vms; state.vms=nullptr;
            // Yes, state.vms is set to NULL twice.
            state.vms = NULL;

            state.vmCount = 0;
            state.stateTimer2 = 0;

            context.supervisor_state = SupervisorState_ExitGame;

            return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
        }
        break;
    case TitleCurrentScreenState_Changing:
        if (state.stateTimer >= 30)
        {
            this->ChangeCurrentScreen(TitleCurrentScreen_Option);
            state.cursor = 0;
            state.currentGameConfig = config;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
        break;
    }

    state.idleFrames=wrapping_add(state.idleFrames,1);
    state.stateTimer=wrapping_add(state.stateTimer,1);
    state.stateTimer2=wrapping_add(state.stateTimer2,1);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 TitleMenus::OnUpdateOptions()
{
    i32 i;

    switch (state.currentScreenState)
    {
    case TitleCurrentScreenState_Init:
        if (state.stateTimer2 == 0)
        {
            SetInterruptArray(state.vms, state.vmCount, 3);

            for (i = 0; i < TITLE_MENU_ITEM_OPTION_NUM_ITEMS; i++)
            {
                state.titleAnm->SetSprite(&state.vms[i + TITLE_SPRITE_OPTION_START],
                                          state.vms[i + TITLE_SPRITE_OPTION_START].baseSpriteIndex + 1);
            }
            state.titleAnm->SetSprite(&state.vms[state.cursor + TITLE_SPRITE_OPTION_START],
                                      state.vms[state.cursor + TITLE_SPRITE_OPTION_START].baseSpriteIndex);
            state.currentScreenState = TitleCurrentScreenState_Init;
            state.stateTimer = 0;
            state.cursor2 = -1;
        }
        state.currentScreenState = TitleCurrentScreenState_Ready;

        for (i = 0; i < ARRAY_SIZE(g_OptionsHelpText); i++)
        {
            DrawTextCentered(&state.helpTextVms[i], 0xfff0e0, 0x300000, Localization::StringById(g_OptionsHelpTextIds[i], g_OptionsHelpText[i]));
        }
    case TitleCurrentScreenState_Ready:
        if (this->MoveCursorVertical(10) != 0)
        {
            for (i = 0; i < TITLE_MENU_ITEM_OPTION_NUM_ITEMS; i++)
            {
                state.titleAnm->SetSprite(&state.vms[i + TITLE_SPRITE_OPTION_START],
                                          state.vms[i + TITLE_SPRITE_OPTION_START].baseSpriteIndex + 1);
            }
            state.titleAnm->SetSprite(&state.vms[state.cursor + TITLE_SPRITE_OPTION_START],
                                      state.vms[state.cursor + TITLE_SPRITE_OPTION_START].baseSpriteIndex);
        }

        if (state.cursor2 != state.cursor)
        {
            state.currentHelpTextVm = &state.helpTextVms[state.cursor];
            state.currentHelpTextVm->SetInterrupt(1);
        }

        state.cursor2 = state.cursor;

        /* Player life option */

        for (i = TITLE_SPRITE_OPTION_PLAYER_START; i <= TITLE_SPRITE_OPTION_PLAYER_END; i++)
        {
            state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex + 1);
        }

        if (context.play.counts[6].total < 30)
        {
            state.vms[25].flag1 = FALSE;
        }

        if (context.play.counts[6].total < 60)
        {
            state.vms[26].flag1 = FALSE;
        }

        i = TITLE_SPRITE_OPTION_PLAYER_START + config.lives;
        state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex);

        /* Graphics mode */

        for (i = TITLE_SPRITE_OPTION_GRAPHICS_MODE_START; i <= TITLE_SPRITE_OPTION_GRAPHICS_MODE_END; i++)
        {
            state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_OPTION_GRAPHICS_MODE_START + config.color16;
        state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex);

        /* Music mode */

        for (i = TITLE_SPRITE_OPTION_MUSIC_MODE_START; i <= TITLE_SPRITE_OPTION_MUSIC_MODE_END; i++)
        {
            state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_OPTION_MUSIC_MODE_START + config.music;
        state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex);

        /* Music volume */

        /* 3rd digit */
        if (config.music_volume >= 100)
        {
            state.titleAnm->SetSprite(&state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3],
                                      state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3].baseSpriteIndex);
            state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3].color1.a = 255;
        }
        else
        {
            state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT3].color1.a = 0;
        }

        /* 2nd digit */
        if (config.music_volume >= 10)
        {
            state.titleAnm->SetSprite(&state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2],
                                      state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2].baseSpriteIndex +
                                          ((config.music_volume / 10) % 10) * 2);
            state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2].color1.a = 255;
        }
        else
        {
            state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT2].color1.a = 0;
        }

        state.titleAnm->SetSprite(&state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT1],
                                  state.vms[TITLE_SPRITE_OPTION_MUSIC_VOLUME_DIGIT1].baseSpriteIndex +
                                      ((config.music_volume) % 10) * 2);

        state.titleAnm->SetSprite(&state.vms[35], state.vms[35].baseSpriteIndex);

        /* SFX volume */

        /* 3rd digit */
        if (config.sound_volume >= 100)
        {
            state.titleAnm->SetSprite(&state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3],
                                      state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3].baseSpriteIndex);
            state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3].color1.a = 255;
        }
        else
        {
            state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT3].color1.a = 0;
        }

        /* 2nd digit */
        if (config.sound_volume >= 10)
        {
            state.titleAnm->SetSprite(&state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2],
                                      state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2].baseSpriteIndex +
                                          ((config.sound_volume / 10) % 10) * 2);
            state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2].color1.a = 255;
        }
        else
        {
            state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT2].color1.a = 0;
        }

        state.titleAnm->SetSprite(&state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT1],
                                  state.vms[TITLE_SPRITE_OPTION_SFX_VOLUME_DIGIT1].baseSpriteIndex +
                                      ((config.sound_volume % 10) * 2));

        /* Display mode */

        state.titleAnm->SetSprite(&state.vms[39], state.vms[39].baseSpriteIndex);
        for (i = TITLE_SPRITE_OPTION_WINDOWED_START; i <= TITLE_SPRITE_OPTION_WINDOWED_END; i++)
        {
            state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_OPTION_WINDOWED_START + config.windowed;
        state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex);

        /* Slow mode */

        for (i = TITLE_SPRITE_OPTION_SLOWMODE_START; i <= TITLE_SPRITE_OPTION_SLOWMODE_END; i++)
        {
            state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_OPTION_SLOWMODE_START + config.slow_mode;
        state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex);

        if (state.stateTimer2 < 4)
        {
            break;
        }

        if (WAS_PRESSED_SCROLLING(TH_BUTTON_LEFT))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_OPTION_PLAYER: /* Player */
                i = 7;
                if (context.play.counts[6].total < 30)
                {
                    i--;
                }
                if (context.play.counts[6].total < 60)
                {
                    i--;
                }
                if (config.lives == 0)
                {
                    config.lives = i - 1;
                }
                else
                {
                    config.lives--;
                }
                break;
            case TITLE_MENU_ITEM_OPTION_GRAPHIC:
                if (config.color16 == 0)
                {
                    config.color16 = 1;
                }
                else
                {
                    config.color16--;
                }
                break;
            case TITLE_MENU_ITEM_OPTION_BGM:
                actions.stop_audio();
                if (config.music == MIDI)
                {
                    actions.midi_reset();
                }
                if (config.music == OFF)
                {
                    config.music = MIDI;
                }
                else
                {
                    config.music--;
                }
                actions.load_music(8, "bgm/th08_01.mid");
                actions.play_music(8, 0);
                break;
            case TITLE_MENU_ITEM_OPTION_MODE:
                if (config.windowed == 0)
                {
                    config.windowed = 1;
                }
                else
                {
                    config.windowed--;
                }
                break;
            case TITLE_MENU_ITEM_OPTION_SLOWMODE:
                if (config.slow_mode == 0)
                {
                    config.slow_mode = 1;
                }
                else
                {
                    config.slow_mode--;
                }
                break;
            default:
                goto left_button_check;
            }

            actions.sound(SOUND_MOVE_MENU, 0);
            actions.process_sounds();
        }
    left_button_check:

        if (WAS_PRESSED(TH_BUTTON_LEFT))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_OPTION_VOL:
                config.music_volume -= 4;
                if (config.music_volume < 0)
                {
                    config.music_volume = 0;
                }
                actions.apply_volume();
                break;
            case TITLE_MENU_ITEM_OPTION_SE_VOL:
                config.sound_volume -= 4;
                if (config.sound_volume < 0)
                {
                    config.sound_volume = 0;
                }
                actions.apply_volume();
                break;
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RIGHT))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_OPTION_VOL:
                config.music_volume += 4;
                if (config.music_volume > 100)
                {
                    config.music_volume = 100;
                }
                actions.apply_volume();
                break;
            case TITLE_MENU_ITEM_OPTION_SE_VOL:
                config.sound_volume += 4;
                if (config.sound_volume > 100)
                {
                    config.sound_volume = 100;
                }
                actions.apply_volume();
                break;
            }
        }

        /* If you press left for more than 30 frames, counter goes brrr */
        if (WAS_PRESSED(TH_BUTTON_LEFT) || context.input.held >= 30 && (context.input.current & TH_BUTTON_LEFT))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_OPTION_VOL:
                if (config.music_volume > 0)
                {
                    config.music_volume--;
                }
                actions.apply_volume();
                break;
            case TITLE_MENU_ITEM_OPTION_SE_VOL:
                if (config.sound_volume > 0)
                {
                    config.sound_volume--;
                }
                actions.apply_volume();
                break;
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RIGHT) || context.input.held >= 30 && (context.input.current & TH_BUTTON_RIGHT))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_OPTION_VOL:
                if (config.music_volume < 100)
                {
                    config.music_volume++;
                }
                actions.apply_volume();
                break;
            case TITLE_MENU_ITEM_OPTION_SE_VOL:
                if (config.sound_volume < 100)
                {
                    config.sound_volume++;
                }
                actions.apply_volume();
                break;
            }
        }

        context.bgmVolume = config.music_volume;
        context.sfxVolume = config.sound_volume;

        if (WAS_PRESSED_SCROLLING(TH_BUTTON_RIGHT))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_OPTION_PLAYER:
                i = 7;
                if (context.play.counts[6].total < 30)
                {
                    i--;
                }
                if (context.play.counts[6].total < 60)
                {
                    i--;
                }
                if (config.lives >= i - 1)
                {
                    config.lives = 0;
                }
                else
                {
                    config.lives++;
                }
                break;
            case TITLE_MENU_ITEM_OPTION_GRAPHIC:
                if (config.color16 >= 1)
                {
                    config.color16 = 0;
                }
                else
                {
                    config.color16++;
                }
                break;
            case TITLE_MENU_ITEM_OPTION_BGM:
                actions.stop_audio();
                if (config.music >= MIDI)
                {
                    config.music = OFF;
                }
                else
                {
                    config.music++;
                }
                actions.load_music(8, "bgm/th08_01.mid");
                actions.play_music(8, 0);
                break;
            case TITLE_MENU_ITEM_OPTION_MODE:
                if (config.windowed >= 1)
                {
                    config.windowed = 0;
                }
                else
                {
                    config.windowed++;
                }
                break;
            case TITLE_MENU_ITEM_OPTION_SLOWMODE:
                if (config.slow_mode >= 1)
                {
                    config.slow_mode = 0;
                }
                else
                {
                    config.slow_mode++;
                }
                break;
            default:
                goto sfx_play;
            }

            actions.sound(SOUND_MOVE_MENU, 0);
            actions.process_sounds();
        }

    sfx_play:
        switch (state.cursor)
        {
        case TITLE_MENU_ITEM_OPTION_VOL:
        case TITLE_MENU_ITEM_OPTION_SE_VOL:
            if ((state.stateTimer2 % 50) == 0)
            {
                actions.sound(SOUND_TIMEOUT, 0);
            }
            break;
        default:
            break;
        }

        if (context.input.current != 0)
        {
            state.idleFrames = 0;
        }

        if (state.idleFrames >= 3600)
        {
            goto exit_options;
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_OPTION_RESET:
                config.lives = 2;
                config.bombs = 3;
                config.music = WAV;
                config.sounds = 1;
                config.slow_mode = 0;

                actions.sound(SOUND_SELECT, 0);
                actions.process_sounds();
                break;
            case TITLE_MENU_ITEM_OPTION_KEYCONFIG:
                state.cursor = TITLE_MENU_ITEM_KEYCONFIG_SHOT;
                this->ChangeCurrentScreen(TitleCurrentScreen_KeyConfig);
                actions.sound(SOUND_SELECT, 0);
                actions.process_sounds();
                return CHAIN_CALLBACK_RESULT_CONTINUE;
            case TITLE_MENU_ITEM_OPTION_EXIT:
            exit_options:
                state.cursor = TITLE_MENU_ITEM_START_OPTION;
                this->ChangeCurrentScreen(TitleCurrentScreen_StartMenu);
                actions.sound(SOUND_BACK, 0);
                actions.process_sounds();

                if (state.currentGameConfig.color16 != config.color16 ||
                    state.currentGameConfig.windowed != config.windowed)
                {
                    return CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR;
                }
                else
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE;
                }
            }
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            if (state.cursor == TITLE_MENU_ITEM_OPTION_EXIT)
            {
                goto exit_options;
            }

            state.titleAnm->SetSprite(&state.vms[state.cursor + TITLE_SPRITE_OPTION_START],
                                      state.vms[state.cursor + TITLE_SPRITE_OPTION_START].baseSpriteIndex + 1);
            state.cursor = TITLE_MENU_ITEM_OPTION_EXIT;
            state.titleAnm->SetSprite(&state.vms[state.cursor + TITLE_SPRITE_OPTION_START],
                                      state.vms[state.cursor + TITLE_SPRITE_OPTION_START].baseSpriteIndex);

            actions.sound(SOUND_BACK, 0);
            actions.process_sounds();
        }
    }

    state.idleFrames=wrapping_add(state.idleFrames,1);
    state.stateTimer=wrapping_add(state.stateTimer,1);
    state.stateTimer2=wrapping_add(state.stateTimer2,1);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 TitleMenus::OnUpdateKeyConfig()
{
    AnmVm *vmPair;
    i32 i;
    u8 *controllerState;
    i16 keyToChange;

    switch (state.currentScreenState)
    {
    case TitleCurrentScreenState_Init:
        if (state.stateTimer2 == 0)
        {
            SetInterruptArray(state.vms, state.vmCount, 4);

            for (i = 0; i < 12; i++)
            {
                state.titleAnm->SetSprite(&state.vms[i + 45], state.vms[i + 45].baseSpriteIndex + 1);
            }

            state.titleAnm->SetSprite(&state.vms[state.cursor + 45], state.vms[state.cursor + 45].baseSpriteIndex);
            state.currentScreenState = TitleCurrentScreenState_Init;
            state.stateTimer = 0;

            std::memcpy(&state.controllerMapping,config.controller,sizeof(state.controllerMapping));

            config.controller[4] = -1;
            config.controller[5] = -1;

            /* Yes, ZUN really did write this. */
            vmPair = &state.vms[57];
            this->SetKeyNumberSprite(vmPair, state.controllerMapping.shotButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, state.controllerMapping.bombButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, state.controllerMapping.focusButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, state.controllerMapping.skipButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, state.controllerMapping.menuButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, state.controllerMapping.upButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, state.controllerMapping.downButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, state.controllerMapping.leftButton);
            vmPair += 2;
            this->SetKeyNumberSprite(vmPair, state.controllerMapping.rightButton);

            state.cursor2 = -1;
        }

        state.currentScreenState = TitleCurrentScreenState_Ready;

        for (i = 0; i < ARRAY_SIZE(g_KeyConfigHelpText); i++)
        {
            DrawTextCentered(&state.helpTextVms[i], 0xfff0e0, 0x300000, Localization::StringById(g_KeyConfigHelpTextIds[i], g_KeyConfigHelpText[i]));
        }

    case TitleCurrentScreenState_Ready:
        if (this->MoveCursorVertical(12) != 0)
        {
            for (i = 0; i < 12; i++)
            {
                state.titleAnm->SetSprite(&state.vms[i + 45], state.vms[i + 45].baseSpriteIndex + 1);
            }

            state.titleAnm->SetSprite(&state.vms[state.cursor + 45], state.vms[state.cursor + 45].baseSpriteIndex);
        }
        if (state.cursor2 != state.cursor)
        {
            state.currentHelpTextVm = &state.helpTextVms[state.cursor];
            state.currentHelpTextVm->SetInterrupt(1);
        }
        state.cursor2 = state.cursor;

        vmPair = &state.vms[57];
        this->SetKeyNumberSprite(vmPair, state.controllerMapping.shotButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, state.controllerMapping.bombButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, state.controllerMapping.focusButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, state.controllerMapping.skipButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, state.controllerMapping.menuButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, state.controllerMapping.upButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, state.controllerMapping.downButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, state.controllerMapping.leftButton);
        vmPair += 2;
        this->SetKeyNumberSprite(vmPair, state.controllerMapping.rightButton);

        for (i = TITLE_SPRITE_KEYCONFIG_SLOWSHOT_START; i <= TITLE_SPRITE_KEYCONFIG_SLOWSHOT_END; i++)
        {
            state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex + 1);
        }

        i = TITLE_SPRITE_KEYCONFIG_SLOWSHOT_START + config.shot_slow;
        state.titleAnm->SetSprite(&state.vms[i], state.vms[i].baseSpriteIndex);

        controllerState = context.controller_state;

        for (keyToChange = 0; keyToChange < 32; keyToChange++)
        {
            if ((controllerState[keyToChange] & TH_BUTTON_RIGHT) != 0)
            {
                break;
            }
        }

        if (keyToChange < 32 && context.lastKeyChanged != keyToChange)
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_KEYCONFIG_SHOT:
                this->SetKeyConfigKey(keyToChange, state.controllerMapping.shotButton, 1);
                state.controllerMapping.shotButton = keyToChange;
                break;
            case TITLE_MENU_ITEM_KEYCONFIG_BOMB:
                this->SetKeyConfigKey(keyToChange, state.controllerMapping.bombButton, 0);
                state.controllerMapping.bombButton = keyToChange;
                break;
            case TITLE_MENU_ITEM_KEYCONFIG_SLOW:
                this->SetKeyConfigKey(keyToChange, state.controllerMapping.focusButton, 1);
                state.controllerMapping.focusButton = keyToChange;
                break;
            case TITLE_MENU_ITEM_KEYCONFIG_PAUSE:
                this->SetKeyConfigKey(keyToChange, state.controllerMapping.menuButton, 0);
                state.controllerMapping.menuButton = keyToChange;
                break;
            case TITLE_MENU_ITEM_KEYCONFIG_UP:
                this->SetKeyConfigKey(keyToChange, state.controllerMapping.upButton, 0);
                state.controllerMapping.upButton = keyToChange;
                break;
            case TITLE_MENU_ITEM_KEYCONFIG_DOWN:
                this->SetKeyConfigKey(keyToChange, state.controllerMapping.downButton, 0);
                state.controllerMapping.downButton = keyToChange;
                break;
            case TITLE_MENU_ITEM_KEYCONFIG_LEFT:
                this->SetKeyConfigKey(keyToChange, state.controllerMapping.leftButton, 0);
                state.controllerMapping.leftButton = keyToChange;
                break;
            case TITLE_MENU_ITEM_KEYCONFIG_RIGHT:
                this->SetKeyConfigKey(keyToChange, state.controllerMapping.rightButton, 0);
                state.controllerMapping.rightButton = keyToChange;
                break;
            case TITLE_MENU_ITEM_KEYCONFIG_SKIP:
                this->SetKeyConfigKey(keyToChange, state.controllerMapping.skipButton, 0);
                state.controllerMapping.skipButton = keyToChange;
                break;
            default:
                goto out;
            }

            actions.sound(SOUND_SELECT, 0);
            actions.process_sounds();
        }

    out:

        context.lastKeyChanged = keyToChange;

        if (WAS_PRESSED(TH_BUTTON_LEFT))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_KEYCONFIG_SHOTSLOW:
                config.shot_slow = 1 - config.shot_slow;
                break;
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RIGHT))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_KEYCONFIG_SHOTSLOW:
                config.shot_slow = 1 - config.shot_slow;
                break;
            }
        }

        if (context.input.current != 0)
        {
            state.idleFrames = 0;
        }

        if (state.idleFrames >= 3600)
        {
            goto exit_keyconfig;
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            switch (state.cursor)
            {
            case TITLE_MENU_ITEM_KEYCONFIG_RESET:
                actions.sound(SOUND_SELECT, 0);
                actions.process_sounds();

                state.controllerMapping = g_ControllerMapping;
                config.shot_slow = TRUE;
                break;
            case TITLE_MENU_ITEM_KEYCONFIG_QUIT:
            exit_keyconfig:
                actions.sound(SOUND_BACK, 0);
                actions.process_sounds();

                this->ChangeCurrentScreen(TitleCurrentScreen_Option);

                std::memcpy(config.controller,&state.controllerMapping,sizeof(state.controllerMapping));
                state.cursor = TITLE_MENU_ITEM_OPTION_KEYCONFIG;
                return CHAIN_CALLBACK_RESULT_CONTINUE;
            }
        }

        break;
    }

    state.idleFrames=wrapping_add(state.idleFrames,1);
    state.stateTimer=wrapping_add(state.stateTimer,1);
    state.stateTimer2=wrapping_add(state.stateTimer2,1);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 TitleMenus::OnUpdateDifficultySelect()
{
    int i;
    TitleCurrentScreen oldScreen;
    int menuLength;

    switch (state.currentScreenState)
    {
    case TitleCurrentScreenState_Init:
        if (state.stateTimer2 == 0)
        {
            if (state.previousScreen != TitleCurrentScreen_CharacterSelect &&
                state.previousScreen != TitleCurrentScreen_CharacterSelectPractice &&
                state.previousScreen != TitleCurrentScreen_CharacterSelectExtra)
            {
                if (actions.load_surface(0, "title/select00.png") != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
                }
            }

            state.cursor = config.difficulty;

            if (state.currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                SetInterruptArray(state.vms, state.vmCount, 7);
            }
            else
            {
                /* Hold over from PCB because IN doesn't have Phantasm */
                if (!context.IsPhantasmUnlocked())
                {
                    SetInterruptArray(state.vms, state.vmCount, 12);
                    state.cursor = 4;
                }
            }

            if (state.currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                if (state.cursor >= EXTRA)
                {
                    state.cursor = NORMAL;
                }

                for (i = 0; i < 4; i++)
                {
                    state.titleAnm->SetSprite(&state.vms[i + TITLE_SPRITE_DIFFICULTY_START],
                                              state.vms[i + TITLE_SPRITE_DIFFICULTY_START].baseSpriteIndex + 1);
                    state.vms[i + TITLE_SPRITE_DIFFICULTY_START].SetInterrupt(25);
                }

                state.titleAnm->SetSprite(&state.vms[state.cursor + TITLE_SPRITE_DIFFICULTY_START],
                                          state.vms[state.cursor + TITLE_SPRITE_DIFFICULTY_START].baseSpriteIndex);
                state.vms[state.cursor + TITLE_SPRITE_DIFFICULTY_START].SetInterrupt(24);
            }
            else
            {
                state.cursor -= 4;
                if (state.cursor < 0)
                {
                    state.cursor = 0;
                }
                state.titleAnm->SetSprite(&state.vms[TITLE_SPRITE_DIFFICULTY_EXTRA],
                                          state.vms[TITLE_SPRITE_DIFFICULTY_EXTRA].baseSpriteIndex);
            }

            state.currentScreenState = TitleCurrentScreenState_Init;
            state.stateTimer = 0;
            state.currentHelpTextVm = NULL;
        }

        if (state.practiceState != 0)
        {
            this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);
            state.cursor = 0;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }

        if (state.stateTimer2 == 8)
        {
            state.currentScreenState = TitleCurrentScreenState_Ready;
        }
        break;
    case TitleCurrentScreenState_Ready:
        menuLength = (state.currentScreen != TitleCurrentScreen_DifficultySelectExtra) ? 4 : 1;

        if (this->MoveCursorVertical(menuLength) != 0 &&
            state.currentScreen != TitleCurrentScreen_DifficultySelectExtra)
        {
            for (i = 0; i < 4; i++)
            {
                state.titleAnm->SetSprite(&state.vms[i + TITLE_SPRITE_DIFFICULTY_START],
                                          state.vms[i + TITLE_SPRITE_DIFFICULTY_START].baseSpriteIndex + 1);
                state.vms[i + TITLE_SPRITE_DIFFICULTY_START].SetInterrupt(25);
            }

            state.titleAnm->SetSprite(&state.vms[state.cursor + TITLE_SPRITE_DIFFICULTY_START],
                                      state.vms[state.cursor + TITLE_SPRITE_DIFFICULTY_START].baseSpriteIndex);
            state.vms[state.cursor + TITLE_SPRITE_DIFFICULTY_START].SetInterrupt(24);
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            if (state.currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                config.difficulty = state.cursor;
            }
            else
            {
                config.difficulty = state.cursor + EXTRA;
            }

            actions.sound(SOUND_SELECT, 0);
            actions.process_sounds();

            if (state.currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                if (!context.IsPracticeMode())
                {
                    this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelect);
                }
                else
                {
                    this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);
                }
            }
            else
            {
                this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectExtra);
            }

            state.cursor = 0;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            if (state.currentScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                config.difficulty = state.cursor;
            }
            else
            {
                config.difficulty = state.cursor + 4;
            }
            actions.sound(SOUND_BACK, 0);
            actions.process_sounds();

            state.currentScreenState = TitleCurrentScreenState_Changing;
            state.stateTimer = 0;

            SetInterruptArray(state.vms, state.vmCount, 6);
        }
        break;
    case TitleCurrentScreenState_Changing:
        if (state.stateTimer >= 20)
        {
            /* ?! but what does TitleMenus::previousScreen store? */
            oldScreen = state.currentScreen;

            this->ChangeCurrentScreen(TitleCurrentScreen_StartMenu);

            if (oldScreen != TitleCurrentScreen_DifficultySelectExtra)
            {
                if (!context.IsPracticeMode())
                {
                    state.cursor = TITLE_MENU_ITEM_START_START;
                }
                else
                {
                    state.cursor = TITLE_MENU_ITEM_START_PRACTICE_START;
                }
            }
            else
            {
                state.cursor = TITLE_MENU_ITEM_START_EXTRA_START;
            }

            context.flags.isPracticeMode = FALSE;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
        break;
    }

    state.stateTimer=wrapping_add(state.stateTimer,1);
    state.idleFrames=wrapping_add(state.idleFrames,1);
    state.stateTimer2=wrapping_add(state.stateTimer2,1);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 TitleMenus::OnUpdateCharacterSelect()
{
    i32 menuLength;
    i32 vmIdx1;
    i32 vmIdx2;
    i32 i1;
    i32 i2;
    i32 cursorMovement;
    TitleCurrentScreen oldScreen;

    switch (state.currentScreenState)
    {
    case TitleCurrentScreenState_Init:
        if (state.stateTimer2 == 0)
        {
            state.cursor = context.character;
            SetInterruptArray(state.vms, state.vmCount, 8);
            menuLength = context.IsExtraUnlockedWithAllTeams() ? 12 : 4;
            if (state.currentScreen == TitleCurrentScreen_CharacterSelectSpell)
            {
                /* This is all dead code, by the way. */
                if (actions.load_surface(0, "title/select00.png") != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
                }
                while (!context.IsSpellPracticeUnlockedForCharacter(state.cursor))
                {
                    state.cursor++;
                    if (state.cursor >= menuLength)
                    {
                        state.cursor -= menuLength;
                    }
                }
            }
            else
            {
                /* Highlight the selected difficulty. */
                state.vms[TITLE_SPRITE_DIFFICULTY_START + config.difficulty].SetInterrupt(9);
                if (state.currentScreen == TitleCurrentScreen_CharacterSelectExtra)
                {
                    while (!context.IsExtraUnlockedForCharacter(state.cursor))
                    {
                        state.cursor++;
                        if (state.cursor >= menuLength)
                        {
                            state.cursor -= menuLength;
                        }
                    }
                }
            }

            for (vmIdx1 = TITLE_SPRITE_CHARACTER_START; vmIdx1 <= TITLE_SPRITE_CHARACTER_END; vmIdx1++)
            {
                state.vms[vmIdx1].flag1 = FALSE;
                state.vms[vmIdx1].SetInterrupt(8);
                for (i1 = 0; i1 < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i1++)
                {
                    if (g_TitleCharacterSpriteIndices[state.cursor][i1] == vmIdx1)
                    {
                        state.vms[vmIdx1].flag1 = TRUE;
                        state.vms[vmIdx1].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[state.cursor][i1] == vmIdx1)
                {
                    state.vms[vmIdx1].flag1 = TRUE;
                    state.vms[vmIdx1].SetInterrupt(23);
                }
            }

            state.currentScreenState = TitleCurrentScreenState_Init;
            state.stateTimer = 0;
        }

        if (state.practiceState != 0)
        {
            if (state.currentScreen != TitleCurrentScreen_CharacterSelectSpell)
            {
                this->ChangeCurrentScreen(TitleCurrentScreen_PracticeStageSelect);
            }
            else
            {
                this->ChangeCurrentScreen(TitleCurrentScreen_SpellStageSelect);
            }

            state.cursor = context.currentStage;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }

        if (state.stateTimer2 == 8)
        {
            state.currentScreenState = TitleCurrentScreenState_Ready;
        }
        break;
    case TitleCurrentScreenState_Ready:
        menuLength = context.IsExtraUnlockedWithAllTeams() ? 12 : 4;
        if (state.cursor >= menuLength)
        {
            state.cursor = 0;
        }

        cursorMovement = this->MoveCursorHorizontal(menuLength);
        if (cursorMovement != 0)
        {
            if (state.currentScreen == TitleCurrentScreen_CharacterSelectSpell)
            {
                while (!context.IsSpellPracticeUnlockedForCharacter(state.cursor))
                {
                    state.cursor += cursorMovement;

                    if (state.cursor >= menuLength)
                    {
                        state.cursor -= menuLength;
                    }
                    if (state.cursor < 0)
                    {
                        state.cursor += menuLength;
                    }
                }
            }
            else if (state.currentScreen == TitleCurrentScreen_CharacterSelectExtra)
            {
                while (!context.IsExtraUnlockedForCharacter(state.cursor))
                {
                    state.cursor += cursorMovement;

                    if (state.cursor >= menuLength)
                    {
                        state.cursor -= menuLength;
                    }
                    if (state.cursor < 0)
                    {
                        state.cursor += menuLength;
                    }
                }
            }
            for (vmIdx2 = TITLE_SPRITE_CHARACTER_START; vmIdx2 <= TITLE_SPRITE_CHARACTER_END; vmIdx2++)
            {
                state.vms[vmIdx2].flag1 = TRUE;
                state.vms[vmIdx2].SetInterrupt(8);
                for (i2 = 0; i2 < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i2++)
                {
                    if (g_TitleCharacterSpriteIndices[state.cursor][i2] == vmIdx2)
                    {
                        state.vms[vmIdx2].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[state.cursor][i2] == vmIdx2)
                {
                    state.vms[vmIdx2].SetInterrupt(23);
                }
            }
        }
        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            context.character = state.cursor;
            context.shotType = 0;

            actions.sound(SOUND_SELECT, 0);
            actions.process_sounds();

            // The practice stage-select confirm owns this assignment for a
            // vanilla run, but thprac's practice menu intercepts that screen,
            // and the practice character select returns early below. Set the
            // chosen difficulty here so a thprac run does not inherit a stale
            // title difficulty.
            context.difficulty = config.difficulty;
            if (state.currentScreen == TitleCurrentScreen_CharacterSelectPractice)
            {
                state.cursor = context.currentStage;
                this->ChangeCurrentScreen(TitleCurrentScreen_PracticeStageSelect);
                return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
            }
            else if (state.currentScreen == TitleCurrentScreen_CharacterSelectSpell)
            {
                state.cursor = context.currentStage;
                this->ChangeCurrentScreen(TitleCurrentScreen_SpellStageSelect);
                return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
            }

            context.difficulty = config.difficulty;
            if (state.currentScreen != TitleCurrentScreen_CharacterSelectExtra)
            {
                context.currentStage = STAGE1;
            }
            else
            {
                context.currentStage = context.difficulty + EXTRA;
            }

            context.supervisor_state = SupervisorState_GameManager;
            context.SetIsReplayWeird(FALSE);
            actions.stop_audio();

            return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            actions.sound(SOUND_BACK, 0);
            actions.process_sounds();

            context.character = state.cursor;

            if (state.currentScreen == TitleCurrentScreen_CharacterSelectSpell)
            {
                actions.sound(SOUND_BACK, 0);
                actions.process_sounds();
                state.currentScreenState = TitleCurrentScreenState_Changing;
                state.stateTimer = 0;

                SetInterruptArray(state.vms, state.vmCount, 6);

                break;
            }
            else
            {
                if (state.currentScreen != TitleCurrentScreen_CharacterSelectExtra)
                {
                    if (!context.IsPracticeMode())
                    {
                        this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelect);
                    }
                    else
                    {
                        this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelectPractice);
                    }
                }
                else
                {
                    this->ChangeCurrentScreen(TitleCurrentScreen_DifficultySelectExtra);
                }

                state.cursor = 0;

                return CHAIN_CALLBACK_RESULT_CONTINUE;
            }
        }
        break;
    case TitleCurrentScreenState_Changing:
        if (state.stateTimer >= 30)
        {
            oldScreen = state.currentScreen;
            this->ChangeCurrentScreen(TitleCurrentScreen_StartMenu);
            state.cursor = TITLE_MENU_ITEM_START_SPELL_PRACTICE;

            context.flags.isPracticeMode = FALSE;

            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
        break;
    }

    state.idleFrames=wrapping_add(state.idleFrames,1);
    state.stateTimer=wrapping_add(state.stateTimer,1);
    state.stateTimer2=wrapping_add(state.stateTimer2,1);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 TitleMenus::OnUpdatePracticeStageSelect()
{
    u16 clearInfo;
    i32 i;
    i32 vmIdx;
    i32 cursorMovement;

    switch (state.currentScreenState)
    {
    case TitleCurrentScreenState_Init:
        if (state.stateTimer2 == 0)
        {
            SetInterruptArray(state.vms, state.vmCount, 18);
            state.practiceState = 0;
            for (vmIdx = TITLE_SPRITE_CHARACTER_START; vmIdx <= TITLE_SPRITE_CHARACTER_END; vmIdx++)
            {
                state.vms[vmIdx].flag1 = FALSE;
                state.vms[vmIdx].SetInterrupt(8);
                for (i = 1; i < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i++)
                {
                    if (g_TitleCharacterSpriteIndices[context.character][i] == vmIdx)
                    {
                        state.vms[vmIdx].flag1 = TRUE;
                        state.vms[vmIdx].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[context.character][i] == vmIdx)
                {
                    state.vms[vmIdx].flag1 = TRUE;
                    state.vms[vmIdx].SetInterrupt(23);
                }
            }
            state.currentScreenState = TitleCurrentScreenState_Init;
            state.stateTimer = 0;
            context.flags.isPracticeMode = TRUE;
        }
        if (state.stateTimer2 == 8)
        {
            state.currentScreenState = TitleCurrentScreenState_Ready;
        }
        break;
    case TitleCurrentScreenState_Ready:
        clearInfo = context.clears[context.character]
                        .with_retries[config.difficulty];

        /* Make Stage 1 selectable in practice */
        if (clearInfo == 0)
        {
            clearInfo = 1;
        }

        /* Make Stage 4A and Stage 4B selectable when Final B is cleared */
        if (IS_STAGE_CLEARED(clearInfo, STAGE6B))
        {
            clearInfo |= ZUN_BIT(STAGE4A) | ZUN_BIT(STAGE4B);
        }

        if (!IS_STAGE_CLEARED(clearInfo, state.cursor))
        {
            state.cursor = 0;
        }

        cursorMovement = this->MoveCursorVertical(8);

        if (cursorMovement != 0)
        {
            while (!IS_STAGE_CLEARED(clearInfo, state.cursor))
            {
                state.cursor += cursorMovement;
                if (state.cursor >= 8)
                {
                    state.cursor = 0;
                }
                if (state.cursor < 0)
                {
                    state.cursor = 7;
                }
            }
        }

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            actions.sound(SOUND_SELECT, 0);
            context.difficulty = config.difficulty;
            context.currentStage = state.cursor;

            context.supervisor_state = SupervisorState_GameManager;

            context.SetIsReplayWeird(FALSE);
            actions.stop_audio();

            return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            actions.sound(SOUND_BACK, 0);

            state.cursor = context.character;
            this->ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);

            return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
        }
        break;
    }

    state.idleFrames=wrapping_add(state.idleFrames,1);
    state.stateTimer=wrapping_add(state.stateTimer,1);
    state.stateTimer2=wrapping_add(state.stateTimer2,1);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 TitleMenus::OnUpdateSpellStageSelect()
{
    i32 menuLength1;
    i32 menuLength2;
    i32 vmIdx1;
    i32 vmIdx2;
    i32 i1;
    i32 i2;
    i32 oldCursorPos;
    i32 horizontalCursorMovement;
    TitleCurrentScreen oldScreen;

    switch (state.currentScreenState)
    {
    case TitleCurrentScreenState_Init:
        if (state.stateTimer2 == 0)
        {
            SetInterruptArray(state.vms, state.vmCount, 18);

            state.vms[139].SetInterrupt(27);
            state.vms[138].SetInterrupt(27);

            if (state.previousScreen != TitleCurrentScreen_SpellCardSelect)
            {
                if (actions.load_surface(0, "title/select00.png") != ZUN_SUCCESS)
                {
                    return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
                }
            }

            state.cursor = context.character;

            menuLength1 = context.IsExtraUnlockedWithAllTeams() ? 12 : 4;
            while (!context.IsSpellPracticeUnlockedForCharacter(state.cursor))
            {
                state.cursor++;
                if (state.cursor >= menuLength1)
                {
                    state.cursor -= menuLength1;
                }
            }

            context.character = state.cursor;

            state.percentageCapturedSpellPracticePerShot = 0.0f;
            state.percentageCapturedInGamePerShot = 0.0f;
            state.percentageCapturedSpellPractice = 0.0f;
            state.percentageCapturedInGame = 0.0f;

            state.cursor = context.currentStage;

            for (vmIdx1 = TITLE_SPRITE_CHARACTER_START; vmIdx1 <= TITLE_SPRITE_CHARACTER_END; vmIdx1++)
            {
                state.vms[vmIdx1].flag1 = FALSE;
                state.vms[vmIdx1].SetInterrupt(8);
                for (i1 = 1; i1 < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i1++)
                {
                    if (g_TitleCharacterSpriteIndices[context.character][i1] == vmIdx1)
                    {
                        state.vms[vmIdx1].flag1 = TRUE;
                        state.vms[vmIdx1].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[context.character][i1] == vmIdx1)
                {
                    state.vms[vmIdx1].flag1 = TRUE;
                    state.vms[vmIdx1].SetInterrupt(23);
                }
            }

            InitializeAndSetSprite(state.resultTextAnm,&state.spellCardNameVms[0], 2);
            state.spellCardNameVms[0].pos = Float3(0, 0, 0);
            state.spellCardNameVms[0].anchor = 3;
            state.spellCardNameVms[0].fontWidth = 15;
            state.spellCardNameVms[0].fontHeight = 15;
            state.spellCardNameVms[0].color1.a = 255;
            state.spellCardNameVms[0].color1.r = 255;
            state.spellCardNameVms[0].color1.g = 255;
            state.spellCardNameVms[0].color1.b = 255;
            DrawTextLeft(&state.spellCardNameVms[0], COLOR_TEXT_WHITE, 0, Localization::StringById("th08 Spell Practice Description", TH_TITLE_SPELL_STAGE_INFO));

            InitializeAndSetSprite(state.resultTextAnm,&state.spellCardNameVms[1], 3);
            state.spellCardNameVms[1].pos = Float3(0, 0, 0);
            state.spellCardNameVms[1].anchor = 3;
            state.spellCardNameVms[1].fontWidth = 15;
            state.spellCardNameVms[1].fontHeight = 15;
            state.spellCardNameVms[1].color1.a = 255;
            state.spellCardNameVms[1].color1.r = 255;
            state.spellCardNameVms[1].color1.g = 255;
            state.spellCardNameVms[1].color1.b = 255;
            DrawTextLeft(&state.spellCardNameVms[1], COLOR_TEXT_WHITE, 0,
                                       Localization::StringById("th08 Spell Practice Percentages",
                                                                TH_TITLE_SPELL_CAPTURE_PERCENTAGE));

            /* ZUN bug: possible copy paste mistake? */
            InitializeAndSetSprite(state.titleAnm,&state.spellCardNameVms[2], 144);
            state.spellCardNameVms[1].anchor = 3;
            state.spellCardNameVms[1].color1.a = 255;
            state.spellCardNameVms[1].color1.r = 255;
            state.spellCardNameVms[1].color1.g = 255;
            state.spellCardNameVms[1].color1.b = 255;

            state.currentScreenState = TitleCurrentScreenState_Init;
            state.stateTimer = 0;

            context.flags.isPracticeMode = TRUE;
            context.flags.isSpellPractice = TRUE;
        }

        if (state.practiceState != 0)
        {
            state.cursor = 0;
            this->ChangeCurrentScreen(TitleCurrentScreen_SpellCardSelect);
            if (context.currentSpellCardNumber >= SPELLCARD_LAST_WORD_START)
            {
                context.currentStage = STAGE_LAST_WORD;
            }
            return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
        }

        if (state.stateTimer2 == 8)
        {
            state.currentScreenState = TitleCurrentScreenState_Ready;
        }
        break;
    case TitleCurrentScreenState_Ready:
        this->MoveCursorVertical(10);
        oldCursorPos = state.cursor;
        state.cursor = context.character;

        menuLength2 = context.IsExtraUnlockedWithAllTeams() ? 12 : 4;

        horizontalCursorMovement = this->MoveCursorHorizontal(menuLength2);
        if (horizontalCursorMovement != 0)
        {
            while (!context.IsSpellPracticeUnlockedForCharacter(state.cursor))
            {
                state.cursor += horizontalCursorMovement;
                if (state.cursor >= menuLength2)
                {
                    state.cursor -= menuLength2;
                }
                if (state.cursor < 0)
                {
                    state.cursor += menuLength2;
                }
            }

            context.character = state.cursor;

            for (vmIdx2 = TITLE_SPRITE_CHARACTER_START; vmIdx2 <= TITLE_SPRITE_CHARACTER_END; vmIdx2++)
            {
                state.vms[vmIdx2].flag1 = FALSE;
                state.vms[vmIdx2].SetInterrupt(8);
                for (i2 = 1; i2 < ARRAY_SIZE(g_TitleCharacterSpriteIndices[0]) - 1; i2++)
                {
                    if (g_TitleCharacterSpriteIndices[state.cursor][i2] == vmIdx2)
                    {
                        state.vms[vmIdx2].flag1 = TRUE;
                        state.vms[vmIdx2].SetInterrupt(9);
                    }
                }
                if (g_TitleCharacterSpriteIndices[state.cursor][i2] == vmIdx2)
                {
                    state.vms[vmIdx2].flag1 = TRUE;
                    state.vms[vmIdx2].SetInterrupt(23);
                }
            }

            state.stateTimer2 = 0;
        }

        state.cursor = oldCursorPos;

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            actions.sound(SOUND_SELECT, 0);

            context.currentStage = state.cursor;

            /* ??? */
            actions.sound(SOUND_SELECT, 0);
            actions.process_sounds();

            state.cursor = 0;

            this->ChangeCurrentScreen(TitleCurrentScreen_SpellCardSelect);

            return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            actions.sound(SOUND_BACK, 0);
            actions.process_sounds();

            state.currentScreenState = TitleCurrentScreenState_Changing;
            state.stateTimer = 0;

            context.currentStage = state.cursor;
            SetInterruptArray(state.vms, state.vmCount, 6);
            break;
        }
        break;
    case TitleCurrentScreenState_Changing:
        if (state.stateTimer >= 20)
        {
            oldScreen = state.currentScreen;
            this->ChangeCurrentScreen(TitleCurrentScreen_StartMenu);
            state.cursor = 2;
            context.flags.isPracticeMode = FALSE;
            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
        break;
    }

    state.idleFrames=wrapping_add(state.idleFrames,1);
    state.stateTimer=wrapping_add(state.stateTimer,1);
    state.stateTimer2=wrapping_add(state.stateTimer2,1);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 TitleMenus::OnUpdateSpellCardSelect()
{
    i32 spellCardNumber;
    i32 spellCardNumber3;
    i32 spellCardNumber2;
    i32 i;
    i32 i2;
    i32 oldCursor;
    i32 oldPageIdx;

    switch (state.currentScreenState)
    {
    case TitleCurrentScreenState_Init:
        if (state.stateTimer2 == 0)
        {
            SetInterruptArray(state.vms, state.vmCount, 26);

            state.practiceState = 0;
            state.currentScreenState = TitleCurrentScreenState_Init;
            state.stateTimer = 0;

            context.flags.isPracticeMode = TRUE;
            context.flags.isSpellPractice = TRUE;

            state.currentNumberOfSpellCards = spell_stage_counts[context.currentStage];

            this->UnlockLastWordSpellCards();

            state.cursor = 0;
            for (i = 0; i < state.currentNumberOfSpellCards; i++)
            {
                if (spells_by_stage[context.currentStage][i] == context.currentSpellCardNumber)
                {
                    state.cursor = i;
                    break;
                }
            }

            state.currentPageSpellCardSelect = state.cursor / TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;

            for (i = 0; i < TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE; i++)
            {
                if ((i + state.currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE) >=
                    state.currentNumberOfSpellCards)
                {
                    break;
                }

                spellCardNumber = spells_by_stage[context.currentStage]
                                                            [i + state.currentPageSpellCardSelect *
                                                                     TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE];

                InitializeAndSetSprite(state.resultTextAnm,&state.spellCardNameVms[i], i + 2);
                state.spellCardNameVms[i].pos = Float3(0, 0, 0);
                state.spellCardNameVms[i].anchor = 3;
                /* Copy paste mistake? */
                state.spellCardNameVms[0].fontWidth = 15;
                state.spellCardNameVms[i].fontHeight = 15;

                if (context.spells[spellCardNumber].game.attempts[SHOT_ALL] == 0 &&
                    context.spells[spellCardNumber].practice.attempts[SHOT_ALL] == 0)
                {
                    if (spell_difficulty(spellCardNumber) <= EXTRA ||
                        !context.IsLastWordSpellCardAttempted(spellCardNumber))
                    {
                        DrawTextLeft(&state.spellCardNameVms[i], COLOR_TEXT_WHITE, 0,
                                                   Localization::StringById("th08_????????", TH_TITLE_SPELLCARD_NOT_UNLOCKED));
                    }
                    else
                    {
                        DrawTextLeft(&state.spellCardNameVms[i], COLOR_TEXT_WHITE, 0,
                                                   TH_TITLE_SPELLCARD_AVAILABLE);
                    }
                }
                else
                {
                    DrawTextLeft(&state.spellCardNameVms[i], COLOR_TEXT_WHITE, 0,
                                               Localization::SpellName(u32(spellCardNumber), context.spells[spellCardNumber].name));
                }

                state.spellCardNameVms[i].color1.a = 255;
                state.spellCardNameVms[i].color1.r = 96;
                state.spellCardNameVms[i].color1.g = 96;
                state.spellCardNameVms[i].color1.b = 96;
            }

            i = TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;
            InitializeAndSetSprite(state.resultTextAnm,&state.spellCardNameVms[i], i + 2);
            state.spellCardNameVms[i].pos = Float3(0, 0, 0);
            state.spellCardNameVms[i].anchor = 3;
            state.spellCardNameVms[i].fontWidth = 15;
            state.spellCardNameVms[i].fontHeight = 15;

            DrawTextLeft(&state.spellCardNameVms[i], COLOR_TEXT_WHITE, 0, Localization::StringById("th08 Spell Practice List Description", TH_TITLE_SPELL_CARD_INFO));

            state.spellCardNameVms[i].color1.a = 255;
            state.spellCardNameVms[i].color1.r = 255;
            state.spellCardNameVms[i].color1.g = 255;
            state.spellCardNameVms[i].color1.b = 255;

            i = state.cursor - (state.currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE);
            state.spellCardNameVms[i].color1.r = 255;
            state.spellCardNameVms[i].color1.g = 255;
            state.spellCardNameVms[i].color1.b = 255;

            for (i = 0; i < 7; i++)
            {
                InitializeAndSetSprite(context.textAnm,&state.spellCardInfoVms[i], i + 21);

                if (i < 4)
                {
                    state.spellCardInfoVms[i].pos = Float3(64.0f, (i * 16) + 344.0f, 0.0f);
                }
                else
                {
                    state.spellCardInfoVms[i].pos = Float3(64.0f, (i * 16) + 344.0f + 8.0f, 0.0f);
                }

                state.spellCardInfoVms[i].anchor = 3;
                state.spellCardInfoVms[i].fontWidth = 15;
                state.spellCardInfoVms[i].fontHeight = 15;
                state.spellCardInfoVms[i].color1.d3dColor = COLOR_WHITE;
            }

            this->FormatSpellCardInfo();
            state.unk0xc29c = 0;
        }

        if (state.stateTimer2 == 8)
        {
            state.currentScreenState = TitleCurrentScreenState_Ready;
        }
        break;
    case TitleCurrentScreenState_Ready:
        oldPageIdx = state.currentPageSpellCardSelect;
        oldCursor = state.cursor;

        if (state.currentNumberOfSpellCards > TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE)
        {
            if (WAS_PRESSED_SCROLLING(TH_BUTTON_LEFT))
            {
                state.cursor -= TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;

                actions.sound(SOUND_MOVE_MENU, 0);
                actions.process_sounds();

                if (state.cursor < 0)
                {
                    state.cursor = state.currentNumberOfSpellCards - 1;
                }
                if (state.cursor >= state.currentNumberOfSpellCards)
                {
                    state.cursor = 0;
                }
            }
            if (WAS_PRESSED_SCROLLING(TH_BUTTON_RIGHT))
            {
                actions.sound(SOUND_MOVE_MENU, 0);

                if (state.currentNumberOfSpellCards - state.cursor <=
                    state.currentNumberOfSpellCards % TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE)
                {
                    state.cursor %= TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;
                }
                else
                {
                    state.cursor += TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;
                    if (state.cursor < 0)
                    {
                        state.cursor = state.currentNumberOfSpellCards - 1;
                    }
                    if (state.cursor >= state.currentNumberOfSpellCards)
                    {
                        state.cursor = state.currentNumberOfSpellCards - 1;
                    }
                }
            }
        }

        this->MoveCursorVertical(state.currentNumberOfSpellCards);
        state.currentPageSpellCardSelect = state.cursor / TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE;

        if (oldPageIdx != state.currentPageSpellCardSelect)
        {
            for (i2 = 0; i2 < TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE; i2++)
            {
                if ((i2 + state.currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE) >=
                    state.currentNumberOfSpellCards)
                {
                    break;
                }

                InitializeAndSetSprite(state.resultTextAnm,&state.spellCardNameVms[i2], i2 + 2);
                state.spellCardNameVms[i2].pos = Float3(0, 0, 0);
                state.spellCardNameVms[i2].anchor = 3;
                /* Similar copy paste mistake as before? */
                state.spellCardInfoVms[0].fontWidth = 15;
                state.spellCardNameVms[i2].fontHeight = 15;

                spellCardNumber2 = spells_by_stage[context.currentStage]
                                                             [i2 + state.currentPageSpellCardSelect *
                                                                       TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE];

                /* Why does ZUN use this helper method here, and in the initialization , use direct access? */
                if (context.HasSpellCardBeenEncountered(spellCardNumber2, SHOT_ALL))
                {
                    DrawTextLeft(&state.spellCardNameVms[i2], COLOR_TEXT_WHITE, 0,
                                               Localization::SpellName(u32(spellCardNumber2), context.spells[spellCardNumber2].name));
                }
                else
                {
                    if (spell_difficulty(spellCardNumber2) <= EXTRA ||
                        !context.IsLastWordSpellCardAttempted(spellCardNumber2))
                    {
                        DrawTextLeft(&state.spellCardNameVms[i2], COLOR_TEXT_WHITE, 0,
                                                   Localization::StringById("th08_????????", TH_TITLE_SPELLCARD_NOT_UNLOCKED));
                    }
                    else
                    {
                        DrawTextLeft(&state.spellCardNameVms[i2], COLOR_TEXT_WHITE, 0,
                                                   TH_TITLE_SPELLCARD_AVAILABLE);
                    }
                }

                state.spellCardNameVms[i2].color1.a = 255;
                state.spellCardNameVms[i2].color1.r = 96;
                state.spellCardNameVms[i2].color1.g = 96;
                state.spellCardNameVms[i2].color1.b = 96;
            }
        }

        if (oldCursor != state.cursor)
        {
            for (i2 = 0; i2 < TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE; i2++)
            {
                state.spellCardNameVms[i2].color1.r = 96;
                state.spellCardNameVms[i2].color1.g = 96;
                state.spellCardNameVms[i2].color1.b = 96;
            }

            i2 = state.cursor - (state.currentPageSpellCardSelect * TITLE_SPELL_CARD_SPELLCARDS_PER_PAGE);
            state.spellCardNameVms[i2].color1.r = 255;
            state.spellCardNameVms[i2].color1.g = 255;
            state.spellCardNameVms[i2].color1.b = 255;

            for (i2 = 0; i2 < 7; i2++)
            {
                state.spellCardInfoVms[i2].color1.a = 0;
            }

            state.unk0xc29c = 21;
        }

        this->FormatSpellCardInfo();

        if (WAS_PRESSED(TH_BUTTON_SHOOT | TH_BUTTON_ENTER))
        {
            spellCardNumber3 = spells_by_stage[context.currentStage][state.cursor];
            if (context.spells[spellCardNumber3].game.attempts[SHOT_ALL] != 0 ||
                context.spells[spellCardNumber3].practice.attempts[SHOT_ALL] != 0 ||
                (spellCardNumber3 >= SPELLCARD_LAST_WORD_START &&
                 context.IsLastWordSpellCardAttempted(spellCardNumber3)))
            {
                actions.sound(SOUND_SELECT, 0);

                context.flags.isSpellPractice = TRUE;
                context.currentSpellCardNumber =
                    spells_by_stage[context.currentStage][state.cursor];

                context.difficulty = spell_difficulty(context.currentSpellCardNumber);

                if (context.difficulty > EXTRA)
                {
                    /* Set the correct difficulty for each Last Word spell card. */
                    switch (context.currentSpellCardNumber)
                    {
                    case SPELLCARD_LW_WRIGGLE:
                        context.currentStage = STAGE1;
                        break;
                    case SPELLCARD_LW_MYSTIA:
                        context.currentStage = STAGE2;
                        break;
                    case SPELLCARD_LW_KEINE:
                        context.currentStage = STAGE3;
                        break;
                    case SPELLCARD_LW_REISEN:
                        context.currentStage = STAGE5;
                        break;
                    case SPELLCARD_LW_EIRIN:
                        context.currentStage = STAGE6A;
                        break;
                    case SPELLCARD_LW_KAGUYA:
                        context.currentStage = STAGE6B;
                        break;
                    case SPELLCARD_LW_MOKOU:
                        context.currentStage = EXTRASTAGE;
                        break;
                    case SPELLCARD_LW_TEWI:
                        context.currentStage = STAGE5;
                        break;
                    case SPELLCARD_LW_KEINEEX:
                        context.currentStage = EXTRASTAGE;
                        break;
                    case SPELLCARD_LW_REIMU:
                        context.currentStage = STAGE4A;
                        break;
                    case SPELLCARD_LW_MARISA:
                        context.currentStage = STAGE4B;
                        break;
                    default: /* ... everyone else */
                        context.currentStage = STAGE4A;
                        break;
                    }

                    context.difficulty = NORMAL;
                }

                context.supervisor_state = SupervisorState_GameManager;
                context.SetIsReplayWeird(FALSE);

                actions.stop_audio();

                return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
            }
            else
            {
                actions.sound(SOUND_INVALID_ACTION, 0);
            }
        }

        if (WAS_PRESSED(TH_BUTTON_BOMB | TH_BUTTON_MENU))
        {
            context.currentSpellCardNumber = spells_by_stage[context.currentStage][state.cursor];

            actions.sound(SOUND_BACK, 0);
            state.cursor = context.currentStage;

            this->ChangeCurrentScreen(TitleCurrentScreen_SpellStageSelect);

            return CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN;
        }

        break;
    }

    state.idleFrames=wrapping_add(state.idleFrames,1);
    state.stateTimer=wrapping_add(state.stateTimer,1);
    state.stateTimer2=wrapping_add(state.stateTimer2,1);

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 TitleMenus::MoveCursorVertical(i32 menuLength)
{
    if (menuLength == 0)
    {
        return 0;
    }

    if (WAS_PRESSED_SCROLLING(TH_BUTTON_UP))
    {
        state.cursor--;
        actions.sound(SOUND_MOVE_MENU, 0);
        if (state.cursor < 0)
        {
            state.cursor = menuLength - 1;
        }
        if (state.cursor >= menuLength)
        {
            state.cursor = 0;
        }

        return -1;
    }

    if (WAS_PRESSED_SCROLLING(TH_BUTTON_DOWN))
    {
        state.cursor++;
        actions.sound(SOUND_MOVE_MENU, 0);
        if (state.cursor < 0)
        {
            state.cursor = menuLength - 1;
        }
        if (state.cursor >= menuLength)
        {
            state.cursor = 0;
        }

        return 1;
    }

    return 0;
}

/* Why is this function slightly better written than MoveCursorVertical? */
i32 TitleMenus::MoveCursorHorizontal(i32 menuLength)
{
    if (menuLength == 0)
    {
        return 0;
    }

    if (WAS_PRESSED_SCROLLING(TH_BUTTON_LEFT))
    {
        state.cursor--;

        if (state.cursor < 0)
        {
            state.cursor = state.cursor + menuLength;
        }

        actions.sound(SOUND_MOVE_MENU, 0);
        return -1;
    }

    if (WAS_PRESSED_SCROLLING(TH_BUTTON_RIGHT))
    {
        state.cursor++;

        if (state.cursor >= menuLength)
        {
            state.cursor = state.cursor - menuLength;
        }

        actions.sound(SOUND_MOVE_MENU, 0);
        return 1;
    }

    return 0;
}


}
