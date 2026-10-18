// Result menu logic adapted from GensokyoClub/th08 (MIT); see cpp/licenses.

// Generated base: scripts/cpp/import-result-controls.py. Native comparisons validate it.

#include "ResultScreen.hpp"

#include "ResultConstants.hpp"

namespace th08 {

#define ARRAY_SIZE_SIGNED(a) i32(sizeof(a)/sizeof((a)[0]))

#define WAS_PRESSED(mask) context.input.pressed(mask)

#define WAS_PRESSED_SCROLLING(mask) (WAS_PRESSED(mask)||((context.input.current&(mask))&&context.input.scrolling))

#define IS_PRESSED(mask) (context.input.current&(mask))

constexpr i32 MAX_DIFFICULTIES=5,SHOT_ALL=12,SPELLCARD_COUNT_SPELLCARDS=222,ZUN_SUCCESS=0;

constexpr i32 SOUND_SELECT=10,SOUND_BACK=11,SOUND_MOVE_MENU=12,SOUND_1UP=28;

constexpr u16 TH_BUTTON_SELECTMENU=4097,TH_BUTTON_RETURNMENU=10,TH_BUTTON_FOCUS=4,TH_BUTTON_SKIP=256,TH_BUTTON_RIGHT=128,TH_BUTTON_LEFT=64,TH_BUTTON_D=8192,TH_BUTTON_Q=512,TH_BUTTON_WRONG_CHEATCODE=0x160b;

i32 ResultScreen::HandleCategorySelectScreen()
{
    AnmVm *vm;
    i32 i;

    switch (state.menuDepth)
    {
    case 0:
        if (state.frameTimer2 == 0)
        {
            vm = state.spriteVms;

            for (i = 0; i < ARRAY_SIZE_SIGNED(state.spriteVms); i++, vm++)
            {
                vm->pendingInterrupt = RESULT_INTERRUPT_HIDE;
            }

            for (i = RESULT_SCRIPT_CATEGORY_HIGHSCORE; i <= RESULT_SCRIPT_CATEGORY_BACK_TO_TITLE; i++)
            {
                state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_APPEAR;
                execute(&state.spriteVms[i]);
                if (i == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (state.frameTimer2 < 20)
        {
            break;
        }
        state.menuDepth++;
        state.frameTimer2 = 0;
    case 1:
        i = move_cursor( 4);
        if (i != 0)
        {
            for (i = RESULT_SCRIPT_CATEGORY_HIGHSCORE; i <= RESULT_SCRIPT_CATEGORY_BACK_TO_TITLE; i++)
            {
                if (i == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }
        if (WAS_PRESSED(TH_BUTTON_D))
        {
            actions.export_records();
        }
        if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {
            if (state.cursor == 3)
            {
                goto exit;
            }

            state.cursor = 3;

            actions.sound(SOUND_BACK, 0);

            for (i = RESULT_SCRIPT_CATEGORY_HIGHSCORE; i <= RESULT_SCRIPT_CATEGORY_BACK_TO_TITLE; i++)
            {
                if (i == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            // Useless assignment?
            vm = state.spriteVms;

            switch (state.cursor)
            {
            case 0:
                set_state(RESULT_SCREEN_STATE_BEST_SCORES_CHOOSING_DIFFICULTY);
                for (i = RESULT_SCRIPT_CATEGORY_HIGHSCORE; i <= RESULT_SCRIPT_CATEGORY_BACK_TO_TITLE; i++)
                {
                    if (i == state.cursor)
                    {
                        state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_CHOSEN;
                    }
                    else
                    {
                        state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_HIDE;
                    }
                }
                break;
            case 1:
                set_state(RESULT_SCREEN_STATE_SPELLCARDS_CHOOSING_DIFFICULTY);
                for (i = RESULT_SCRIPT_CATEGORY_HIGHSCORE; i <= RESULT_SCRIPT_CATEGORY_BACK_TO_TITLE; i++)
                {
                    if (i == state.cursor)
                    {
                        state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_CHOSEN;
                    }
                    else
                    {
                        state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_HIDE;
                    }
                }
                break;
            case 2:
                for (i = RESULT_SCRIPT_CATEGORY_HIGHSCORE; i <= RESULT_SCRIPT_CATEGORY_BACK_TO_TITLE; i++)
                {
                    if (i == state.cursor)
                    {
                        state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_CHOSEN;
                    }
                    else
                    {
                        state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_HIDE;
                    }
                }
                set_state(RESULT_SCREEN_STATE_OTHER_STATS_SCREEN_INIT);
                break;
            case 3:
            exit:
                set_state(RESULT_SCREEN_STATE_EXITING);
                actions.sound(SOUND_BACK, 0);
                return 1;
            }

            actions.sound(SOUND_SELECT, 0);

            return 1;
        }
    }

    state.frameTimer2++;

    return 0;
}

i32 ResultScreen::HandleHighScoreDifficultySelect()
{
    AnmVm *vm;
    i32 i;
    i32 i2;
    i32 j;

    switch (state.menuDepth)
    {
    case 0:
        if (state.frameTimer2 == 0)
        {
            state.cursor = state.selectedDifficulty;

            for (i = RESULT_SCRIPT_HIGHSCORE_DIFFICULTY_EASY; i <= RESULT_SCRIPT_HIGHSCORE_DIFFICULTY_EXTRA; i++)
            {
                state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_APPEAR;
                execute(&state.spriteVms[i]);
                if ((i - RESULT_SCRIPT_HIGHSCORE_DIFFICULTY_EASY) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (state.frameTimer2 < 6)
        {
            break;
        }
        state.menuDepth++;
        state.frameTimer2 = 0;
    case 1:
        i = move_cursor( MAX_DIFFICULTIES);
        if (i != 0)
        {
            for (i = RESULT_SCRIPT_HIGHSCORE_DIFFICULTY_EASY; i <= RESULT_SCRIPT_HIGHSCORE_DIFFICULTY_EXTRA; i++)
            {
                if ((i - RESULT_SCRIPT_HIGHSCORE_DIFFICULTY_EASY) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {
            set_state(RESULT_SCREEN_STATE_CHOOSING_CATEGORY);
            actions.sound(SOUND_BACK, 0);
            state.selectedDifficulty = state.cursor;
            state.cursor = 0;
            return 1;
        }

        if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            // Another useless assignment?
            vm = state.spriteVms;

            for (i = RESULT_SCRIPT_HIGHSCORE_DIFFICULTY_EASY; i <= RESULT_SCRIPT_HIGHSCORE_DIFFICULTY_EXTRA; i++)
            {
                if ((i - RESULT_SCRIPT_HIGHSCORE_DIFFICULTY_EASY) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_CHOSEN;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_HIDE;
                }
            }

            state.selectedDifficulty = state.cursor;
            set_state(RESULT_SCREEN_STATE_BEST_SCORES_CHOOSING_CHARACTER);
            state.cursor = 0;
            actions.sound(SOUND_SELECT, 0);

            return 1;
        }

        break;
    }

    if (IS_PRESSED(TH_BUTTON_FOCUS) || IS_PRESSED(TH_BUTTON_SKIP))
    {
        if (state.cheatCodeStep < 4)
        {
            if (WAS_PRESSED(TH_BUTTON_RIGHT))
            {
                state.cheatCodeStep++;
            }
            else if (WAS_PRESSED(TH_BUTTON_WRONG_CHEATCODE))
            {
                state.cheatCodeStep = 0;
            }
        }
        else if (state.cheatCodeStep < 5)
        {
            if (WAS_PRESSED(TH_BUTTON_LEFT))
            {
                state.cheatCodeStep++;
            }
            else if (WAS_PRESSED(TH_BUTTON_WRONG_CHEATCODE))
            {
                state.cheatCodeStep = 0;
            }
        }
        else if (state.cheatCodeStep < 7)
        {
            if (WAS_PRESSED(TH_BUTTON_D))
            {
                state.cheatCodeStep++;
            }
            else if (WAS_PRESSED(TH_BUTTON_WRONG_CHEATCODE))
            {
                state.cheatCodeStep = 0;
            }
        }
        else if (state.cheatCodeStep < 10)
        {
            if (WAS_PRESSED(TH_BUTTON_Q))
            {
                state.cheatCodeStep++;
            }
            else if (WAS_PRESSED(TH_BUTTON_WRONG_CHEATCODE))
            {
                state.cheatCodeStep = 0;
            }
        }
        else
        {
            for (i2 = 0; i2 < SHOT_ALL + 1; i2++)
            {
                for (j = 0; j < MAX_DIFFICULTIES; j++)
                {
                    session.clears[i2].without_retries[j] |= 0xffff;
                    session.clears[i2].with_retries[j] |= 0xffff;
                }
            }

            for (i2 = 0; i2 < SPELLCARD_COUNT_SPELLCARDS; i2++)
            {
                for (j = 0; j < SHOT_ALL + 1; j++)
                {
                    session.records[i2].game.attempts[j]++;
                }
            }

            state.cheatCodeStep = 0;

            actions.sound(SOUND_1UP, 0);
        }
    }
    else
    {
        state.cheatCodeStep = 0;
    }

    state.frameTimer2++;

    return 0;
}

i32 ResultScreen::HandleHighScoreCharacterSelect()
{
    AnmVm *vm;
    i32 i;

    switch (state.menuDepth)
    {
    case 0:
        if (state.frameTimer2 == 0)
        {
            state.cursor = state.selectedHighScoreCharacter;

            for (i = RESULT_SCRIPT_HIGHSCORE_CHARACTER_REIMU_YUKARI; i <= RESULT_SCRIPT_HIGHSCORE_CHARACTER_YUYUKO; i++)
            {
                state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_APPEAR;
                execute(&state.spriteVms[i]);
                if ((i - RESULT_SCRIPT_HIGHSCORE_CHARACTER_REIMU_YUKARI) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (state.frameTimer2 < 6)
        {
            break;
        }
        state.menuDepth++;
        state.frameTimer2 = 0;
    case 1:
        i = move_cursor( SHOT_ALL);
        if (i != 0)
        {
            for (i = RESULT_SCRIPT_HIGHSCORE_CHARACTER_REIMU_YUKARI; i <= RESULT_SCRIPT_HIGHSCORE_CHARACTER_YUYUKO; i++)
            {
                if ((i - RESULT_SCRIPT_HIGHSCORE_CHARACTER_REIMU_YUKARI) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {
            set_state(RESULT_SCREEN_STATE_BEST_SCORES_CHOOSING_DIFFICULTY);
            actions.sound(SOUND_BACK, 0);
            state.selectedHighScoreCharacter = state.cursor;

            for (i = RESULT_SCRIPT_HIGHSCORE_CHARACTER_REIMU_YUKARI; i <= RESULT_SCRIPT_HIGHSCORE_CHARACTER_YUYUKO; i++)
            {
                state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_HIDE;
            }

            return 1;
        }

        if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            // I'm pretty sure ZUN copy pasted the code for each function and
            // didn't check.
            vm = state.spriteVms;

            for (i = RESULT_SCRIPT_HIGHSCORE_CHARACTER_REIMU_YUKARI; i <= RESULT_SCRIPT_HIGHSCORE_CHARACTER_YUYUKO; i++)
            {
                if ((i - RESULT_SCRIPT_HIGHSCORE_CHARACTER_REIMU_YUKARI) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_CHOSEN;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_HIDE;
                }
            }

            state.spriteVms[RESULT_SCRIPT_LISTING].pendingInterrupt = RESULT_INTERRUPT_LISTING_APPEAR;
            state.selectedHighScoreCharacter = -1;
            set_state(RESULT_SCREEN_STATE_BEST_SCORES);
            actions.sound(SOUND_SELECT, 0);

            return 1;
        }

        break;
    }

    state.frameTimer2++;

    return 0;
}

i32 ResultScreen::HandleHighScoreScreen()
{
    i32 oldCursor;

    if (state.selectedHighScoreCharacter != state.cursor && state.frameTimer == 10)
    {
        state.selectedHighScoreCharacter = state.cursor;
    }

    if (state.frameTimer < 6)
    {
        return 0;
    }

    oldCursor = state.cursor;

    if (move_horizontal( SHOT_ALL) != 0)
    {
        state.frameTimer = 0;
        state.spriteVms[RESULT_SCRIPT_LISTING].pendingInterrupt =
            state.selectedDifficulty + RESULT_INTERRUPT_LISTING_APPEAR;
        state.spriteVms[oldCursor + RESULT_SCRIPT_HIGHSCORE_CHARACTER_REIMU_YUKARI].pendingInterrupt =
            RESULT_INTERRUPT_CHARACTER_DISAPPEAR;
        state.spriteVms[state.cursor + RESULT_SCRIPT_HIGHSCORE_CHARACTER_REIMU_YUKARI].pendingInterrupt =
            RESULT_INTERRUPT_CHARACTER_APPEAR;
    }

    if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
    {
        state.selectedHighScoreCharacter = state.cursor;
        set_state(RESULT_SCREEN_STATE_BEST_SCORES_CHOOSING_CHARACTER);
        state.spriteVms[RESULT_SCRIPT_LISTING].pendingInterrupt = RESULT_INTERRUPT_HIDE;
        actions.sound(SOUND_BACK, 0);

        return 1;
    }

    state.frameTimer2++;

    return 0;
}

i32 ResultScreen::HandleSpellCardDifficultySelect()
{
    AnmVm *vm;
    i32 i;

    switch (state.menuDepth)
    {
    case 0:
        if (state.frameTimer2 == 0)
        {
            state.cursor = state.selectedSpellcardDifficulty;

            for (i = RESULT_SCRIPT_SPELLCARD_DIFFICULTY_EASY; i <= RESULT_SCRIPT_SPELLCARD_DIFFICULTY_ALL; i++)
            {
                state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_APPEAR;
                execute(&state.spriteVms[i]);
                if ((i - RESULT_SCRIPT_SPELLCARD_DIFFICULTY_EASY) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (state.frameTimer2 < 6)
        {
            break;
        }
        state.menuDepth++;
        state.frameTimer2 = 0;
    case 1:
        i = move_cursor( MAX_DIFFICULTIES + 1);
        if (i != 0)
        {
            for (i = RESULT_SCRIPT_SPELLCARD_DIFFICULTY_EASY; i <= RESULT_SCRIPT_SPELLCARD_DIFFICULTY_ALL; i++)
            {
                if ((i - RESULT_SCRIPT_SPELLCARD_DIFFICULTY_EASY) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {
            set_state(RESULT_SCREEN_STATE_CHOOSING_CATEGORY);
            actions.sound(SOUND_BACK, 0);
            state.selectedSpellcardDifficulty = state.cursor;
            state.cursor = 1;

            return 1;
        }

        if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            vm = state.spriteVms;

            for (i = RESULT_SCRIPT_SPELLCARD_DIFFICULTY_EASY; i <= RESULT_SCRIPT_SPELLCARD_DIFFICULTY_ALL; i++)
            {
                if ((i - RESULT_SCRIPT_SPELLCARD_DIFFICULTY_EASY) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_CHOSEN;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_HIDE;
                }
            }

            state.selectedSpellcardDifficulty = state.cursor;
            set_state(RESULT_SCREEN_STATE_SPELLCARDS_CHOOSING_CHARACTER);
            actions.sound(SOUND_SELECT, 0);

            return 1;
        }

        break;
    }

    state.frameTimer2++;

    return 0;
}

i32 ResultScreen::HandleSpellCardCharacterSelect()
{
    AnmVm *vm;
    i32 i;

    switch (state.menuDepth)
    {
    case 0:
        if (state.frameTimer2 == 0)
        {
            state.cursor = state.shotTypeCursor;

            for (i = RESULT_SCRIPT_SPELLCARD_CHARACTER_REIMU_YUKARI; i <= RESULT_SCRIPT_SPELLCARD_CHARACTER_ALL; i++)
            {
                state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_APPEAR;
                execute(&state.spriteVms[i]);
                if ((i - RESULT_SCRIPT_SPELLCARD_CHARACTER_REIMU_YUKARI) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (state.frameTimer2 < 6)
        {
            break;
        }
        state.menuDepth++;
        state.frameTimer2 = 0;
    case 1:
        i = move_cursor( SHOT_ALL + 1);
        if (i != 0)
        {
            for (i = RESULT_SCRIPT_SPELLCARD_CHARACTER_REIMU_YUKARI; i <= RESULT_SCRIPT_SPELLCARD_CHARACTER_ALL; i++)
            {
                if ((i - RESULT_SCRIPT_SPELLCARD_CHARACTER_REIMU_YUKARI) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_SELECTED;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_NOT_SELECTED;
                }
            }
        }

        if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {
            set_state(RESULT_SCREEN_STATE_SPELLCARDS_CHOOSING_DIFFICULTY);
            actions.sound(SOUND_BACK, 0);
            state.shotTypeCursor = state.cursor;

            for (i = RESULT_SCRIPT_SPELLCARD_CHARACTER_REIMU_YUKARI; i <= RESULT_SCRIPT_SPELLCARD_CHARACTER_ALL; i++)
            {
                state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_HIDE;
            }

            return 1;
        }

        if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            vm = state.spriteVms;

            for (i = RESULT_SCRIPT_SPELLCARD_CHARACTER_REIMU_YUKARI; i <= RESULT_SCRIPT_SPELLCARD_CHARACTER_ALL; i++)
            {
                if ((i - RESULT_SCRIPT_SPELLCARD_CHARACTER_REIMU_YUKARI) == state.cursor)
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_SPRITE_CHOSEN;
                }
                else
                {
                    state.spriteVms[i].pendingInterrupt = RESULT_INTERRUPT_HIDE;
                }
            }

            state.shotTypeCursor = state.cursor;
            set_state(RESULT_SCREEN_STATE_SPELLCARDS);

            actions.sound(SOUND_SELECT, 0);

            state.spriteVms[RESULT_SCRIPT_LISTING].pendingInterrupt = RESULT_INTERRUPT_LISTING_APPEAR;
            state.cursor = 0;
            state.spellcardPage = -1;

            return 1;
        }

        break;
    }

    state.frameTimer2++;

    return 0;
}

i32 ResultScreen::CheckConfirmButton()
{
    AnmVm *vm;

    switch (state.currentState)
    {
    case RESULT_SCREEN_STATE_STATS_SCREEN:
        if (state.frameTimer <= 30)
        {
            vm = &state.spriteVms[RESULT_SCRIPT_PLAYER_RESULTS];
            vm->pendingInterrupt = RESULT_INTERRUPT_PLAYER_RESULTS_SHOW;
        }
        if (state.frameTimer >= 90 && WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            vm = &state.spriteVms[RESULT_SCRIPT_PLAYER_RESULTS];
            vm->pendingInterrupt = RESULT_INTERRUPT_EXITING;
            state.frameTimer = 0;
            state.currentState = RESULT_SCREEN_STATE_STATS_TO_SAVE_TRANSITION;
        }
        break;
    case RESULT_SCREEN_STATE_STATS_TO_SAVE_TRANSITION:
        if (state.frameTimer >= 30)
        {
            state.frameTimer = 9;
            state.currentState = RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION;
        }
        break;
    }

    return ZUN_SUCCESS;
}

}
