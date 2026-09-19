#include "Gui.hpp"

#if defined(TH_DEV_TOOLS) && defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

#include <cstdio>
#include <cstring>

#include "AnmIdx.hpp"
#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "BulletManager.hpp"
#include "Chain.hpp"
#include "Controller.hpp"
#include "EnemyManager.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "ItemManager.hpp"
#include "Localization.hpp"
#include "Player.hpp"
#include "PracticeRuntime.hpp"
#include "SoundPlayer.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"
#include "ZunResult.hpp"
#include "dxutil.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif

u32 g_SpellcardTimeColors[4] = {
    0xa0d0ff,
    0xa080ff,
    0xe080c0,
    0xff4040,
};

Gui g_Gui;

ChainElem g_GuiCalcChain;

ChainElem g_GuiDrawChain;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
namespace
{
// Countdown warning audio is presentation state, not rollback state. Track the
// currently presented spellcard and a bit for each audible second (0..9). A
// rollback may move the timer back above 10 or revisit 9/8/etc., but it must not
// re-arm a second which this spellcard already voiced.
struct SpellcardCountdownAudioState
{
    bool initialized = false;
    i32 phaseSpellcardId = -1;
    i32 lastPresentedSecond = -1;
    u16 soundedSeconds = 0;
};

SpellcardCountdownAudioState g_SpellcardCountdownAudio;

void ResetSpellcardCountdownAudio()
{
    g_SpellcardCountdownAudio = {};
    g_SpellcardCountdownAudio.phaseSpellcardId = -1;
    g_SpellcardCountdownAudio.lastPresentedSecond = -1;
}

bool ShouldPlaySpellcardCountdownWarning(i32 secondsRemaining,
                                         i32 rollbackLastSecondsRemaining)
{
    if (!MultiplayerGameplay::IsMultiplayer())
        return secondsRemaining < 10 &&
               rollbackLastSecondsRemaining != secondsRemaining;

    // The vanilla warning belongs to the whole Boss timer, including non-spell
    // phases. Use the active spell id when there is one, otherwise -1. A real
    // phase restart also makes the displayed timer jump upward by many seconds;
    // TH07 rollback is capped at 12 frames (< 1 second), so an increase of more
    // than one displayed second cannot be caused by rollback alone.
    const i32 phaseSpellcardId = g_EnemyManager.spellcardInfo.isActive != 0
                                     ? g_EnemyManager.spellcardInfo.spellcardIdx
                                     : -1;
    const bool newPhase =
        !g_SpellcardCountdownAudio.initialized ||
        g_SpellcardCountdownAudio.phaseSpellcardId != phaseSpellcardId ||
        (g_SpellcardCountdownAudio.lastPresentedSecond >= 0 &&
         secondsRemaining > g_SpellcardCountdownAudio.lastPresentedSecond + 1);
    if (newPhase)
    {
        g_SpellcardCountdownAudio.initialized = true;
        g_SpellcardCountdownAudio.phaseSpellcardId = phaseSpellcardId;
        g_SpellcardCountdownAudio.soundedSeconds = 0;
    }
    g_SpellcardCountdownAudio.lastPresentedSecond = secondsRemaining;

    if (secondsRemaining < 0 || secondsRemaining >= 10)
        return false;

    const u16 bit = static_cast<u16>(1u << secondsRemaining);
    if ((g_SpellcardCountdownAudio.soundedSeconds & bit) != 0)
        return false;
    g_SpellcardCountdownAudio.soundedSeconds |= bit;
    return true;
}

const char *GetPlayerFacePath(u8 playerId)
{
    switch (MultiplayerGameplay::GetPlayerCharacter(playerId))
    {
    case CHAR_REIMU: return "data/face_rm00.anm";
    case CHAR_MARISA: return "data/face_mr00.anm";
    case CHAR_SAKUYA: return "data/face_sk00.anm";
    default: return "data/face_rm00.anm";
    }
}

const char *GetMultiplayerHudLoadoutName(u8 playerId)
{
    static const char *names[6] = {
        "ReimuA", "ReimuB", "MarisaA", "MarisaB", "SakuyaA", "SakuyaB",
    };
    const i32 index = MultiplayerGameplay::GetPlayerCharacter(playerId) * 2 +
                      MultiplayerGameplay::GetPlayerShot(playerId);
    return index >= 0 && index < 6 ? names[index] : "Unknown";
}
} // namespace

#if defined(TH_DEV_TOOLS) && defined(__EMSCRIPTEN__) && defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
extern "C" EMSCRIPTEN_KEEPALIVE int TouhouDebugCountdownAudioStep(
    i32 secondsRemaining, i32 rollbackLastSecondsRemaining, i32 phaseSpellcardId,
    i32 spellActive, i32 reset)
{
    if (reset)
        ResetSpellcardCountdownAudio();
    const auto oldActive = g_EnemyManager.spellcardInfo.isActive;
    const auto oldIdx = g_EnemyManager.spellcardInfo.spellcardIdx;
    g_EnemyManager.spellcardInfo.isActive = spellActive ? 1u : 0u;
    g_EnemyManager.spellcardInfo.spellcardIdx = phaseSpellcardId;
    const bool play = ShouldPlaySpellcardCountdownWarning(
        secondsRemaining, rollbackLastSecondsRemaining);
    g_EnemyManager.spellcardInfo.isActive = oldActive;
    g_EnemyManager.spellcardInfo.spellcardIdx = oldIdx;
    if (play)
        g_SoundPlayer.PlaySoundByIdx(SOUND_LOW_SPELL_TIME_WARNING, 0);
    return play ? 1 : 0;
}
#endif
#endif

i32 Gui::IsStageFinished()
{
    return this->impl->stageClearBg.activeSpriteIdx >= 0 && this->impl->stageClearBg.isStopped;
}

void Gui::EndPlayerSpellcard()
{
    this->impl->bombSpellcardName.pendingInterrupt = 1;
    this->impl->bombSpellcardNameBg.SetInterrupt(2);
}

void Gui::EndEnemySpellcard()
{
    this->impl->enemySpellcardName.pendingInterrupt = 1;
    this->impl->enemySpellcardNameBg.SetInterrupt(2);
    this->impl->spellcardBonusIndicator.SetInterrupt(2);
}

void Gui::ClearActiveSprites()
{
    this->impl->stageClearBg.activeSpriteIdx = -1;
    this->impl->loadingSprite.activeSpriteIdx = -1;
    this->impl->stageTransitionSnapshotVm.activeSpriteIdx = -1;
    this->impl->activeTransitionQuads = 0;
}

i32 Gui::IsDialogueSkippable()
{
    return this->impl->msg.dialogueSkippable;
}

void Gui::ShowBonusScore(i32 score)
{
    this->impl->bonusScore.prevPos = this->impl->bonusScore.pos = ZunVec3(416.0f, 48.0f, 0.0f);
    this->impl->bonusScore.displayArg = GUI_DISPLAY_SHOWN;
    this->impl->bonusScore.timer = 0;
    this->impl->bonusScore.fmtArg = score;
    g_Supervisor.renderSkipFrames = 2;
}

void Gui::ShowStatusPopup(i32 fmtArg, i32 popupType)
{
    this->impl->statusPopup.prevPos = this->impl->statusPopup.pos = ZunVec3(416.0f, 168.0f, 0.0f);
    this->impl->statusPopup.displayArg = popupType;
    this->impl->statusPopup.timer = 0;
    this->impl->statusPopup.fmtArg = fmtArg;
    g_Supervisor.renderSkipFrames = 2;
}

void Gui::ShowSpellcardBonus(i32 fmtArg)
{
    this->impl->spellCardBonus.prevPos = this->impl->spellCardBonus.pos =
        ZunVec3(224.0f, 16.0f, 0.0f);
    this->impl->spellCardBonus.displayArg = GUI_DISPLAY_SHOWN;
    this->impl->spellCardBonus.timer = 0;
    this->impl->spellCardBonus.fmtArg = fmtArg;
    g_Supervisor.renderSkipFrames = 2;
}

void Gui::CopyEnemyNameTexture(i32 spriteIdx)
{
    SDL_Rect dstRect;
    SDL_Rect enemyNameRect;

    dstRect.x = g_AnmManager->GetSprite(ANM_SPRITE_FRONT_ENEMY_NAME_FRAME)->startPixelInclusive.x;
    dstRect.y = g_AnmManager->GetSprite(ANM_SPRITE_FRONT_ENEMY_NAME_FRAME)->startPixelInclusive.y;
    dstRect.w =
        g_AnmManager->GetSprite(ANM_SPRITE_FRONT_ENEMY_NAME_FRAME)->endPixelInclusive.x - dstRect.x;
    dstRect.h =
        g_AnmManager->GetSprite(ANM_SPRITE_FRONT_ENEMY_NAME_FRAME)->endPixelInclusive.y - dstRect.y;
    enemyNameRect.x = g_AnmManager->sprites[spriteIdx].startPixelInclusive.x;
    enemyNameRect.y = g_AnmManager->sprites[spriteIdx].startPixelInclusive.y;
    enemyNameRect.w = g_AnmManager->sprites[spriteIdx].endPixelInclusive.x - enemyNameRect.x;
    enemyNameRect.h = g_AnmManager->sprites[spriteIdx].endPixelInclusive.y - enemyNameRect.y;
    g_AnmManager->CopyTexture(21, 22, &dstRect, &enemyNameRect);
}

u32 Gui::OnUpdate(Gui *arg)
{
    if (g_GameManager.isTimeStopped)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }

    if (arg->impl->transitionToScoreScreen)
    {
        g_Supervisor.curState = 3;
        arg->impl->transitionToScoreScreen = 0;
    }
    arg->UpdateGui();
    arg->impl->RunMsg();
    arg->frameCounter = arg->frameCounter + 1;
    // th07_bgm_st6_1: direct advanced-section starts select/play their BGM in
    // GameManager::AddedCallback, so the vanilla delayed Stage6 road start
    // must not fire again at frame 300.
    if (!PracticeRuntime::AdvancedSectionActive() &&
        g_GameManager.currentStage == 6 && arg->frameCounter == 300)
    {
        g_Supervisor.PlayLoadedAudio(0);
    }
    if (IS_PRESSED_RAW(TH_BUTTON_SKIP) && g_Supervisor.renderSkipFrames < 8)
    {
        g_Supervisor.renderSkipFrames = 8;
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

u32 Gui::OnDraw(Gui *arg)
{
    char fmtArg[32];
    ZunVec3 stringPos;

    g_AnmManager->offset.y = 0.0f;
    g_AnmManager->offset.x = 0.0f;
    if (arg->impl->finishedStage)
    {
        stringPos.x = 144.0f;
        stringPos.y = 128.0f;
        stringPos.z = 0.0f;
        if (Localization::Active())
        {
            // base_tsa/th07.v1.00b stage_result_align calls strings_lookup()
            // for the Clear row format, measures its byte length, multiplies
            // by the fixed 8px sprite-ASCII advance, and writes
            //   stringPos.x = 200 - translatedLength * 8.
            // This moves the whole Stage Result block, not just one row.
            const char *clearFormat = Localization::AsciiString("Clear  = %8d");
            stringPos.x = 200.0f - static_cast<f32>(std::strlen(clearFormat) * 8u);
#ifdef TH_DEV_TOOLS
            static bool loggedLocalizedStageResultAlign = false;
            if (!loggedLocalizedStageResultAlign)
            {
                SDL_Log("th07 thcrap stage result align: localization=1 basis=%s bytes=%zu x=%.3f",
                        clearFormat, std::strlen(clearFormat), stringPos.x);
                loggedLocalizedStageResultAlign = true;
            }
#endif
        }
#ifdef TH_DEV_TOOLS
        else
        {
            static bool loggedJapaneseStageResultAlign = false;
            if (!loggedJapaneseStageResultAlign)
            {
                SDL_Log("th07 thcrap stage result align: localization=0 basis=Clear  = %%8d bytes=12 x=%.3f",
                        stringPos.x);
                loggedJapaneseStageResultAlign = true;
            }
        }
#endif
        g_AsciiManager.color = 0xffffff40;
        if (g_GameManager.currentStage < 6)
        {
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Stage Clear");
        }
        else
        {
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "All Clear!");
        }
        stringPos.y = stringPos.y + 32.0f;
        g_AsciiManager.color = 0xffffffff;
        AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Clear  = %8d",
                                    g_GameManager.currentStage * 1000000);
        stringPos.y = stringPos.y + 16.0f;
        g_AsciiManager.color = 0xffe0e0ff;
        AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Point  = %8d",
                                    arg->impl->clearPointItems * 50000);
        stringPos.y = stringPos.y + 16.0f;
        g_AsciiManager.color = 0xffd0d0ff;
        AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Graze  = %8d",
                                    arg->impl->clearGraze * 500);
        stringPos.y = stringPos.y + 16.0f;
        g_AsciiManager.color = 0xffd0d0ff;
        AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Cherry = %8d",
                                    arg->impl->clearCherryMax * 10);
        if (g_GameManager.currentStage >= 7 ||
            (g_GameManager.currentStage == 6 &&
             (!g_GameManager.practice || PracticeRuntime::AdvancedAllClearBonus()) &&
             (!g_GameManager.replay || g_ReplayManager->data->stageReplayData[4] != NULL)))
        {
            stringPos.y = stringPos.y + 16.0f;
            g_AsciiManager.color = 0xffffff80;
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Player =%9d",
                                        (i32)g_GameManager.globals->livesRemaining * 20000000);
            stringPos.y = stringPos.y + 16.0f;
            g_AsciiManager.color = 0xffffff80;
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Bomb   = %8d",
                                        (i32)g_GameManager.globals->bombsRemaining * 4000000);
        }
        stringPos.y = stringPos.y + 32.0f;
        switch (g_GameManager.difficulty)
        {
        case DIFF_EASY:
            g_AsciiManager.color = 0xffff8080;
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Easy Rank    *0.5");
            break;
        case DIFF_NORMAL:
            g_AsciiManager.color = 0xffff8080;
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Normal Rank  *1.0");
            break;
        case DIFF_HARD:
            g_AsciiManager.color = 0xffff8080;
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Hard Rank    *1.2");
            break;
        case DIFF_LUNATIC:
            g_AsciiManager.color = 0xffff8080;
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Lunatic Rank *1.5");
            break;
        case DIFF_EXTRA:
            g_AsciiManager.color = 0xffff8080;
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Extra Rank   *2.0");
            break;
        case DIFF_PHANTASM:
            g_AsciiManager.color = 0xffff8080;
            AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Phantasm Rank*2.0");
            break;
        }
        stringPos.y = stringPos.y + 16.0f;
        if (g_GameManager.difficulty < 4 && !g_GameManager.practice)
        {
            switch (g_GameManager.defaultCfg->lifeCount)
            {
            case 3:
                g_AsciiManager.color = 0xffff8080;
                AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Player Penalty*0.5");
                stringPos.y = stringPos.y + 16.0f;
                break;
            case 4:
                g_AsciiManager.color = 0xffff8080;
                AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Player Penalty*0.2");
                stringPos.y = stringPos.y + 16.0f;
                break;
            }
        }
        g_AsciiManager.color = 0xffffffff;
        AsciiManager::AddFormatText(&g_AsciiManager, &stringPos, "Total = %8d0",
                                    arg->impl->stageClearBonus);
        g_AsciiManager.color = 0xffffffff;
    }
    arg->impl->DrawDialogue();
    arg->DrawStageElements();
    arg->DrawGameScene();
    g_AsciiManager.isGui = TRUE;

    ZunVec3 drawPos;
    if (arg->impl->bonusScore.displayArg != GUI_DISPLAY_HIDDEN)
    {
        drawPos = arg->impl->bonusScore.prevPos.Lerp(arg->impl->bonusScore.pos, g_RenderAlpha);
        g_AsciiManager.color = 0xffffff80;
        AsciiManager::AddFormatText(&g_AsciiManager, &drawPos, "BONUS %8d",
                                    arg->impl->bonusScore.fmtArg);
        g_AsciiManager.color = 0xffffffff;
    }

    drawPos = arg->impl->statusPopup.prevPos.Lerp(arg->impl->statusPopup.pos, g_RenderAlpha);
    switch (arg->impl->statusPopup.displayArg)
    {
    case GUI_DISPLAY_FULL_POWER:
        g_AsciiManager.color = 0xffc0b0ff;
        AsciiManager::AddFormatText(&g_AsciiManager, &drawPos, "Full Power Mode!");
        g_AsciiManager.color = 0xffffffff;
        break;
    case GUI_DISPLAY_BORDER:
        g_AsciiManager.scale.x = 0.9f;
        g_AsciiManager.scale.y = 1.0f;
        g_AsciiManager.fontSpacing = 11;
        g_AsciiManager.color = 0xffe0b0ff;
        AsciiManager::AddFormatText(&g_AsciiManager, &drawPos, "Supernatural Border!!");
        g_AsciiManager.color = 0xffffffff;
        g_AsciiManager.scale.x = 1.0f;
        g_AsciiManager.scale.y = 1.0f;
        g_AsciiManager.fontSpacing = 14;
        break;
    case GUI_DISPLAY_CHERRY_MAX:
        g_AsciiManager.color = 0xffc0b0ff;
        AsciiManager::AddFormatText(&g_AsciiManager, &drawPos, "CherryPoint Max!");
        g_AsciiManager.color = 0xffffffff;
        break;
    case GUI_DISPLAY_BORDER_BONUS:
        g_AsciiManager.scale.x = 0.9f;
        g_AsciiManager.scale.y = 1.0f;
        g_AsciiManager.fontSpacing = 11;
        g_AsciiManager.color = 0xffe0b0ff;
        AsciiManager::AddFormatText(&g_AsciiManager, &drawPos, "Border Bonus %7d",
                                    arg->impl->statusPopup.fmtArg);
        g_AsciiManager.color = 0xffffffff;
        g_AsciiManager.scale.x = 1.0f;
        g_AsciiManager.scale.y = 1.0f;
        g_AsciiManager.fontSpacing = 14;
        break;
    }
    if (arg->impl->spellCardBonus.displayArg != GUI_DISPLAY_HIDDEN)
    {
        g_AsciiManager.color = 0xffff0000;
        arg->impl->spellCardBonus.pos.x =
            (384.0f - strlen("Spell Card Bonus!") * 16.0f) / 2.0f + 32.0f;
        arg->impl->spellCardBonus.pos.y = 80.0f;
        AsciiManager::AddFormatText(&g_AsciiManager, &arg->impl->spellCardBonus.pos,
                                    "Spell Card Bonus!");
        arg->impl->spellCardBonus.pos.y = arg->impl->spellCardBonus.pos.y + 16.0f;
        sprintf(fmtArg, "+%d", arg->impl->spellCardBonus.fmtArg);
        arg->impl->spellCardBonus.pos.x = (384.0f - strlen(fmtArg) * 32.0f) / 2.0f + 32.0f;
        g_AsciiManager.scale.x = 2.0f;
        g_AsciiManager.scale.y = 2.0f;
        g_AsciiManager.color = 0xffff8080;
        g_AsciiManager.AddString(&arg->impl->spellCardBonus.pos, fmtArg);
        g_AsciiManager.scale.x = 1.0f;
        g_AsciiManager.scale.y = 1.0f;
        g_AsciiManager.color = 0xffffffff;
    }
    g_AsciiManager.isGui = 0;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

void Gui::ShowBombNamePortrait(i32 sprite, const char *name)
{
    i32 portraitScript = 1185;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (sprite >= ANM_OFFSET_FACE2 && sprite < ANM_OFFSET_FRONT)
        portraitScript += ANM_OFFSET_FACE2 - ANM_OFFSET_FACE;
    else if (sprite >= ANM_OFFSET_FACE3)
        portraitScript += ANM_OFFSET_FACE3 - ANM_OFFSET_FACE;

    // A missing secondary face must not turn a cosmetic cut-in into an array
    // underflow. The normal P1 face is already loaded at this point.
    if (sprite < 0 || sprite >= (i32)(sizeof(g_AnmManager->sprites) /
                                      sizeof(g_AnmManager->sprites[0])) ||
        g_AnmManager->sprites[sprite].sourceFileIndex < 0)
    {
        portraitScript = 1185;
        sprite = ANM_OFFSET_FACE + 1;
    }
#endif
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->bombSpellcardPortrait,
                                             portraitScript);
    g_AnmManager->SetActiveSprite(&this->impl->bombSpellcardPortrait, sprite);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->bombSpellcardDecorLeft, 1188);
    g_AnmManager->SetActiveSprite(&this->impl->bombSpellcardDecorLeft, 1196);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->bombSpellcardDecorRight, 1190);
    g_AnmManager->SetActiveSprite(&this->impl->bombSpellcardDecorRight, 1196);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->bombSpellcardName, 1796);
    AnmManager::DrawVmTextFmt(g_AnmManager, &this->impl->bombSpellcardName, 0xf0f0ff, 0, name);
    this->bombNameBarLength = (f32)(u32)(strlen(name) * 15) / 2.0f + 16.0f;
    this->impl->bombSpellcardNameBg.SetInterrupt(1);
    g_SoundPlayer.PlaySoundByIdx(SOUND_BOMB, 0);
    g_Supervisor.renderSkipFrames = 2;
}

void Gui::ShowSpellcard(i32 spellcardSprite, const char *spellcardName)
{
    if (spellcardSprite >= 0)
    {
        g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->enemySpellcardPortrait,
                                                ANM_SCRIPT_FACE_ENEMY_SPELLCARD_PORTRAIT);
        g_AnmManager->SetActiveSprite(&this->impl->enemySpellcardPortrait,
                                      spellcardSprite +
                                          ANM_SPRITE_FACE_STAGE_ENEMY_SPELLCARD_PORTRAIT_ARRAY);
        if (this->impl->enemySpellcardPortrait.sprite->widthPx > 256.0f)
        {
            this->impl->enemySpellcardPortrait.offset.x = -288.0f;
        }
        else
        {
            if (this->impl->enemySpellcardPortrait.sprite->widthPx > 128.0f)
            {
                this->impl->enemySpellcardPortrait.offset.x = -112.0f;
            }
            else
            {
                this->impl->enemySpellcardPortrait.offset.x = 0.0f;
            }
        }
    }
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->enemySpellcardDecorHorizontalUp,
                                            ANM_SCRIPT_FACE_SPELLCARD_DECOR_UP);
    g_AnmManager->SetActiveSprite(&this->impl->enemySpellcardDecorHorizontalUp,
                                  ANM_SPRITE_FACE_SPELLCARD_DECOR);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->enemySpellcardDecorHorizontalDown,
                                            ANM_SCRIPT_FACE_SPELLCARD_DECOR_DOWN);
    g_AnmManager->SetActiveSprite(&this->impl->enemySpellcardDecorHorizontalDown,
                                  ANM_SPRITE_FACE_SPELLCARD_DECOR);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->enemySpellcardName,
                                            ANM_SCRIPT_TEXT_ENEMY_SPELLCARD_NAME);
    g_AnmManager->DrawStringFormat(&this->impl->enemySpellcardName, 0xfff0f0, 0, spellcardName);
    this->spellcardBarLength = (f32)(u32)(strlen(spellcardName) * 15) / 2.0f + 16.0f;
    this->impl->enemySpellcardNameBg.SetInterrupt(1);
    this->impl->spellcardBonusIndicator.SetInterrupt(1);
    g_SoundPlayer.PlaySoundByIdx(SOUND_BOMB, 0);
    g_Supervisor.renderSkipFrames = 2;
}

ZunResult Gui::ActualAddedCallback()
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    ResetSpellcardCountdownAudio();
#endif
    i32 k;
    i32 j;
    i32 i;

    this->frameCounter = 0;
    if (IsInitialStageLoad())
    {
        memset(this->impl, 0, sizeof(GuiImpl));

        if (g_AnmManager->LoadAnms(ANM_FILE_FRONT, "data/front.anm", ANM_OFFSET_FRONT) !=
            ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        ClearActiveSprites();
        switch (g_GameManager.character)
        {
        case CHAR_REIMU:
            if (g_AnmManager->LoadAnms(ANM_FILE_FACE, "data/face_rm00.anm", ANM_OFFSET_FACE) !=
                ZUN_SUCCESS)
            {
                return ZUN_ERROR;
            }
            if (g_AnmManager->LoadAnms(ANM_FILE_LOADING, "data/loading.anm", ANM_OFFSET_LOADING) !=
                ZUN_SUCCESS)
            {
                return ZUN_ERROR;
            }
            break;
        case CHAR_MARISA:
            if (g_AnmManager->LoadAnms(ANM_FILE_FACE, "data/face_mr00.anm", ANM_OFFSET_FACE) !=
                ZUN_SUCCESS)
            {
                return ZUN_ERROR;
            }
            if (g_AnmManager->LoadAnms(ANM_FILE_LOADING, "data/loading2.anm", ANM_OFFSET_LOADING) !=
                ZUN_SUCCESS)
            {
                return ZUN_ERROR;
            }
            break;
        case CHAR_SAKUYA:
            if (g_AnmManager->LoadAnms(ANM_FILE_FACE, "data/face_sk00.anm", ANM_OFFSET_FACE) !=
                ZUN_SUCCESS)
            {
                return ZUN_ERROR;
            }
            if (g_AnmManager->LoadAnms(ANM_FILE_LOADING, "data/loading3.anm", ANM_OFFSET_LOADING) !=
                ZUN_SUCCESS)
            {
                return ZUN_ERROR;
            }
            break;
        }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        // Secondary player face ANMs are GUI-lifetime resources. Loading
        // them from OnDraw during the stage-clear screen repeatedly released
        // and rebound the same raw ANM data until LoadAnm observed corrupted
        // string offsets. Match the upstream multiplayer ownership: load once
        // when the GUI is first registered and preserve them across stages.
        if (MultiplayerGameplay::IsMultiplayer())
        {
            if (MultiplayerGameplay::GetPlayerCount() >= 2 &&
                g_AnmManager->LoadAnms(ANM_FILE_FACE2, GetPlayerFacePath(1),
                                       ANM_OFFSET_FACE2) != ZUN_SUCCESS)
                return ZUN_ERROR;
            if (MultiplayerGameplay::GetPlayerCount() >= 3 &&
                g_AnmManager->LoadAnms(ANM_FILE_FACE3, GetPlayerFacePath(2),
                                       ANM_OFFSET_FACE3) != ZUN_SUCCESS)
                return ZUN_ERROR;
        }
#endif
    }
    else
    {
        ClearActiveSprites();
        g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->stageTransitionSnapshotVm,
                                                ANM_SCRIPT_CAPTURE_TRANSITION_SNAPSHOT);
        this->impl->stageTransitionSnapshotVm.pendingInterrupt = 1;
        g_AnmManager->CreateScreenshotTexture(
            this->impl->stageTransitionSnapshotVm.sprite->startPixelInclusive.x,
            this->impl->stageTransitionSnapshotVm.sprite->startPixelInclusive.y,
            this->impl->stageTransitionSnapshotVm.sprite->widthPx,
            this->impl->stageTransitionSnapshotVm.sprite->heightPx);
        g_AnmManager->TakeScreenshotIfRequested();
        for (i = 0; i < TRANSITION_QUAD_ROWS; i++)
        {
            for (j = 0; j < TRANSITION_QUAD_COLS; j++)
            {
                g_AnmManager->SetAnmIdxAndExecuteScript(
                    &this->impl->transitionQuads[i * TRANSITION_QUAD_COLS + j],
                    (i + j & 1) + ANM_SCRIPT_CAPTURE_TRANSITION_QUAD_ARRAY);
                this->impl->transitionQuads[i * TRANSITION_QUAD_COLS + j].intVars2[0] = i + j * 2;
                this->impl->transitionQuads[i * TRANSITION_QUAD_COLS + j].pos.x =
                    (f32)j * 32.0f - 0.5f + 16.0f;
                this->impl->transitionQuads[i * TRANSITION_QUAD_COLS + j].pos.y =
                    (f32)i * 32.0f - 0.5f + 16.0f;
                this->impl->transitionQuads[i * TRANSITION_QUAD_COLS + j].pos.z = 0.0f;
                this->impl->transitionQuads[i * TRANSITION_QUAD_COLS + j].uvScrollPos.x =
                    (f32)j * 32.0f / 512.0f;
                this->impl->transitionQuads[i * TRANSITION_QUAD_COLS + j].uvScrollPos.y =
                    (f32)i * 32.0f / 512.0f;
            }
        }
        this->impl->activeTransitionQuads = ARRAY_SIZE_SIGNED(this->impl->transitionQuads);
    }
    switch (g_GameManager.currentStage)
    {
    case STAGE1:
        CopyEnemyNameTexture(ENEMY_NAME_CIRNO_MBOSS_ST1);
        if (g_AnmManager->LoadAnms(ANM_FILE_FACE_STAGE, "data/face_01_00.anm",
                                   ANM_OFFSET_FACE_STAGE) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (g_AnmManager->LoadAnms(ANM_FILE_STAGE_TEXT, "data/std1txt.anm",
                                   ANM_OFFSET_STAGE_TEXT) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (LoadMsg("data/msg1.dat") != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        break;
    case STAGE2:
        CopyEnemyNameTexture(ENEMY_NAME_CHEN_MBOSS_ST2);
        if (g_AnmManager->LoadAnms(ANM_FILE_FACE_STAGE, "data/face_02_00.anm",
                                   ANM_OFFSET_FACE_STAGE) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (g_AnmManager->LoadAnms(ANM_FILE_STAGE_TEXT, "data/std2txt.anm",
                                   ANM_OFFSET_STAGE_TEXT) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (LoadMsg("data/msg2.dat") != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        break;
    case STAGE3:
        CopyEnemyNameTexture(ENEMY_NAME_ALICE_MBOSS_ST3);
        if (g_AnmManager->LoadAnms(ANM_FILE_FACE_STAGE, "data/face_03_00.anm",
                                   ANM_OFFSET_FACE_STAGE) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (g_AnmManager->LoadAnms(ANM_FILE_STAGE_TEXT, "data/std3txt.anm",
                                   ANM_OFFSET_STAGE_TEXT) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (LoadMsg("data/msg3.dat") != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        break;
    case STAGE4:
        CopyEnemyNameTexture(ENEMY_NAME_LILY_MBOSS_ST4);
        if (g_AnmManager->LoadAnms(ANM_FILE_FACE_STAGE, "data/face_04_00.anm",
                                   ANM_OFFSET_FACE_STAGE) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (g_AnmManager->LoadAnms(ANM_FILE_STAGE_TEXT, "data/std4txt.anm",
                                   ANM_OFFSET_STAGE_TEXT) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (LoadMsg("data/msg4.dat") != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        break;
    case STAGE5:
        CopyEnemyNameTexture(ENEMY_NAME_YOUMU_MBOSS_ST5);
        if (g_AnmManager->LoadAnms(ANM_FILE_FACE_STAGE, "data/face_05_00.anm",
                                   ANM_OFFSET_FACE_STAGE) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (g_AnmManager->LoadAnms(ANM_FILE_STAGE_TEXT, "data/std5txt.anm",
                                   ANM_OFFSET_STAGE_TEXT) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (LoadMsg("data/msg5.dat") != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        break;
    case STAGE6:
        CopyEnemyNameTexture(ENEMY_NAME_YOUMU_MBOSS_ST6);
        if (g_AnmManager->LoadAnms(ANM_FILE_FACE_STAGE, "data/face_06_00.anm",
                                   ANM_OFFSET_FACE_STAGE) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (g_AnmManager->LoadAnms(ANM_FILE_STAGE_TEXT, "data/std6txt.anm",
                                   ANM_OFFSET_STAGE_TEXT) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (LoadMsg("data/msg6.dat") != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        break;
    case EXTRASTAGE:
        CopyEnemyNameTexture(ENEMY_NAME_CHEN_MBOSS_EX);
        if (g_AnmManager->LoadAnms(ANM_FILE_FACE_STAGE, "data/face_07_00.anm",
                                   ANM_OFFSET_FACE_STAGE) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (g_AnmManager->LoadAnms(ANM_FILE_STAGE_TEXT, "data/std7txt.anm",
                                   ANM_OFFSET_STAGE_TEXT) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (LoadMsg("data/msg7.dat") != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        break;
    case PHANTASMSTAGE:
        CopyEnemyNameTexture(ENEMY_NAME_RAN_MBOSS_PH);
        if (g_AnmManager->LoadAnms(ANM_FILE_FACE_STAGE, "data/face_08_00.anm",
                                   ANM_OFFSET_FACE_STAGE) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (g_AnmManager->LoadAnms(ANM_FILE_STAGE_TEXT, "data/std8txt.anm",
                                   ANM_OFFSET_STAGE_TEXT) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        if (LoadMsg("data/msg8.dat") != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
        break;
    default:
        return ZUN_ERROR;
    }
    if (g_Supervisor.curState == 3 || g_Supervisor.curState == 11 || g_Supervisor.curState == 12
            ? 0
            : 1)
    {
        for (k = 0; k < 33; k++)
        {
            g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->vms0[k], k + 1536);
        }
    }
    this->bossPresent = 0;
    this->impl->bossHealthBarState = 0;
    this->bossHealthBar = 0.0f;
    this->bossHealthBarEased = 0.0f;
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->bombSpellcardPortrait, 1185);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->enemySpellcardPortrait, 1187);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->bombSpellcardName, 1796);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->enemySpellcardName, 1797);
    // th07_disable_title @ 0x42956b skips this exact three-argument call
    // (the hook compensates Esp by 0x0c) for custom section/frame starts.
    // These five VMs own the stage/music-title presentation; skipping their
    // initialization matches upstream without suppressing unrelated HUD VMs.
    if (!PracticeRuntime::SuppressStageIntroTitles())
        g_AnmManager->ExecuteVmsAnms(this->impl->stageTextVm, 2048, 5);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->bombSpellcardNameBg, 1);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->enemySpellcardNameBg, 0);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->spellcardBonusIndicator, 2);
    g_AnmManager->SetAnmIdxAndExecuteScript(&this->impl->captureBonusVm, 3);
    this->impl->bombSpellcardPortrait.currentInstruction = NULL;
    this->impl->bombSpellcardDecorLeft.currentInstruction = NULL;
    this->impl->bombSpellcardDecorRight.currentInstruction = NULL;
    this->impl->bombSpellcardName.currentInstruction = NULL;
    this->impl->enemySpellcardPortrait.currentInstruction = NULL;
    this->impl->enemySpellcardDecorHorizontalUp.currentInstruction = NULL;
    this->impl->enemySpellcardDecorHorizontalDown.currentInstruction = NULL;
    this->impl->enemySpellcardName.currentInstruction = NULL;
    this->impl->bombSpellcardPortrait.visible = 0;
    this->impl->bombSpellcardDecorLeft.visible = 0;
    this->impl->bombSpellcardDecorRight.visible = 0;
    this->impl->bombSpellcardName.visible = 0;
    this->impl->enemySpellcardPortrait.visible = 0;
    this->impl->enemySpellcardDecorHorizontalUp.visible = 0;
    this->impl->enemySpellcardDecorHorizontalDown.visible = 0;
    this->impl->enemySpellcardName.visible = 0;
    this->impl->bombSpellcardName.fontWidth = 15;
    this->impl->bombSpellcardName.fontHeight = 15;
    this->impl->enemySpellcardName.fontWidth = 15;
    this->impl->enemySpellcardName.fontHeight = 15;
    this->impl->msg.currentMsgIdx = -1;
    this->impl->finishedStage = 0;
    this->impl->bonusScore.displayArg = GUI_DISPLAY_HIDDEN;
    this->impl->statusPopup.displayArg = GUI_DISPLAY_HIDDEN;
    this->impl->spellCardBonus.displayArg = GUI_DISPLAY_HIDDEN;
    this->lifeDisplayUpdateFrames = 2;
    this->bombDisplayUpdateFrames = 2;
    this->grazeDisplayUpdateFrames = 2;
    this->pointDisplayUpdateFrames = 2;
    this->powerDisplayUpdateFrames = 2;
    g_Supervisor.renderSkipFrames = 16;
    return ZUN_SUCCESS;
}

ZunResult Gui::LoadMsg(const char *filename)
{
    FreeMsgFile();
    this->impl->msg.msgFile = (MsgRawHeader *)FileSystem::OpenFile(filename, 0);
    if (!this->impl->msg.msgFile)
    {
        g_GameErrorContext.Log("error : メッセージファイル %s が読み込めませんでした\n", filename);
        return ZUN_ERROR;
    }

    this->impl->msg.currentMsgIdx = -1;
    this->impl->msg.curInstr = NULL;

    return ZUN_SUCCESS;
}

void Gui::FreeMsgFile()
{
    SAFE_FREE(this->impl->msg.msgFile);
}

void Gui::MsgRead(i32 msgIdx)
{
    this->impl->MsgRead(msgIdx);
}

void GuiImpl::MsgRead(i32 msgIdx)
{
    MsgRawHeader *tmpMsgFile;

    if (this->msg.msgFile->numInstrs <= msgIdx)
    {
        return;
    }

    tmpMsgFile = this->msg.msgFile;
    memset(&this->msg, 0, sizeof(GuiMsgVm));
    this->msg.currentMsgIdx = msgIdx;
    this->msg.msgFile = tmpMsgFile;
    this->msg.curInstr = (MsgRawInstr *)((u8 *)tmpMsgFile + tmpMsgFile->offsets[msgIdx]);
    this->msg.dialogueLines[0].anmFileIdx = -1;
    this->msg.dialogueLines[1].anmFileIdx = -1;
    this->msg.fontSize = 15;
    this->msg.textColorsA[0] = 0xe8f0ff;
    this->msg.textColorsA[1] = 0xffe8f0;
    this->msg.textColorsB[0] = 0;
    this->msg.textColorsB[1] = 0;
    this->msg.dialogueSkippable = 1;
    g_BulletManager.RemoveAllBullets(1);
    g_EnemyManager.RemoveAllEnemies(0, 0);
    g_ItemManager.RemoveAllItems();
    if (msgIdx % 10 == 0)
    {
        switch (g_GameManager.currentStage)
        {
        case STAGE1:
            Gui::CopyEnemyNameTexture(ENEMY_NAME_LETTY_BOSS_ST1);
            break;
        case STAGE2:
            Gui::CopyEnemyNameTexture(ENEMY_NAME_CHEN_BOSS_ST2);
            break;
        case STAGE3:
            Gui::CopyEnemyNameTexture(ENEMY_NAME_ALICE_BOSS_ST3);
            break;
        case STAGE4:
            Gui::CopyEnemyNameTexture(ENEMY_NAME_PRISMRIVER_BOSS_ST4);
            break;
        case STAGE5:
            Gui::CopyEnemyNameTexture(ENEMY_NAME_YOUMU_BOSS_ST5);
            break;
        case STAGE6:
            Gui::CopyEnemyNameTexture(ENEMY_NAME_YUYUKO_BOSS_ST6);
            g_Stage.spellcardVmsIdx = 2;
            g_BulletManager.itemType = ITEM_STAR;
            break;
        case EXTRASTAGE:
            Gui::CopyEnemyNameTexture(ENEMY_NAME_RAN_BOSS_EX);
            g_Stage.spellcardVmsIdx = 1;
            g_Stage.numSpellcardVms = 2;
            g_BulletManager.itemType = ITEM_STAR;
            break;
        case PHANTASMSTAGE:
            Gui::CopyEnemyNameTexture(ENEMY_NAME_YUKARI_BOSS_PH);
            g_Stage.spellcardVmsIdx = 2;
            g_BulletManager.itemType = ITEM_STAR;
        }
    }
}

ZunResult GuiImpl::RunMsg()
{
    MsgRawInstrArgs *args;

    if (this->msg.currentMsgIdx < 0)
    {
        return ZUN_ERROR;
    }
    if (this->msg.ignoreWaitCounter > 0)
    {
        this->msg.ignoreWaitCounter--;
    }
    if (this->msg.dialogueSkippable && IS_PRESSED_GAME(TH_BUTTON_SKIP))
    {
        this->msg.timer = (u32)this->msg.curInstr->time;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            if (IsPlayerSlotActive(playerId) &&
                g_Players[playerId].hasBorder != BORDER_NONE)
            {
                // Shared-border propagation inside Player clears every
                // participant, so one call is sufficient and deterministic.
                g_Players[playerId].BreakBorderNaturally();
                break;
            }
        }
    }
    else
#endif
    if (g_Player.hasBorder != BORDER_NONE)
    {
        g_Player.BreakBorderNaturally();
    }
    if (g_Player.playerState != PLAYER_STATE_DEAD)
    {
        g_ItemManager.RemoveAllItems();
    }
    while (this->msg.timer >= this->msg.curInstr->time)
    {
        switch (this->msg.curInstr->opcode)
        {
        case MSG_DELETE:
            this->msg.currentMsgIdx = -1;
            return ZUN_ERROR;
        case MSG_SHOW_PORTRAIT:
            args = &this->msg.curInstr->args;
            g_AnmManager->SetAnmIdxAndExecuteScript(
                &this->msg.portraits[args->portrait.portraitIdx],
                args->portrait.anmScriptIdx + (args->portrait.portraitIdx != 0
                                                   ? ANM_SCRIPT_FACE_PORTRAIT_SHOW_RIGHT
                                                   : ANM_SCRIPT_FACE_PORTRAIT_SHOW_LEFT));
            if (this->msg.portraits[args->portrait.portraitIdx].sprite->widthPx > 128.0f)
            {
                this->msg.portraits[args->portrait.portraitIdx].offset.x = -112.0f;
            }
            else
            {
                this->msg.portraits[args->portrait.portraitIdx].offset.x = 0.0f;
            }
            break;
        case MSG_CHANGE_FACE:
            args = &this->msg.curInstr->args;
            g_AnmManager->SetActiveSprite(&this->msg.portraits[args->portrait.portraitIdx],
                                          args->portrait.anmScriptIdx +
                                              (args->portrait.portraitIdx == 0
                                                   ? ANM_SPRITE_FACE_PORTRAIT_ARRAY
                                                   : ANM_SPRITE_FACE_STAGE_ENEMY_PORTRAIT_ARRAY));
            if (this->msg.portraits[args->portrait.portraitIdx].sprite->widthPx > 256.0f)
            {
                this->msg.portraits[args->portrait.portraitIdx].offset.x = -208.0f;
                this->msg.portraits[args->portrait.portraitIdx].offset.y = -50.0f;
            }
            else if (this->msg.portraits[args->portrait.portraitIdx].sprite->widthPx > 128.0f)
            {
                this->msg.portraits[args->portrait.portraitIdx].offset.x = -80.0f;
            }
            else
            {
                this->msg.portraits[args->portrait.portraitIdx].offset.x = 0.0f;
            }
            break;
        case MSG_DIALOGUE:
            args = &this->msg.curInstr->args;
            if (args->dialogue.textLine == 0 && this->msg.dialogueLines[1].anmFileIdx >= 0)
            {
                AnmManager::DrawVmTextFmt(g_AnmManager, this->msg.dialogueLines + 1,
                                          this->msg.textColorsA[args->dialogue.textColor],
                                          this->msg.textColorsB[args->dialogue.textColor], " ");
            }
            g_AnmManager->SetAnmIdxAndExecuteScript(
                &this->msg.dialogueLines[args->dialogue.textLine],
                args->dialogue.textLine + ANM_SCRIPT_TEXT_DIALOGUE);
            this->msg.dialogueLines[args->dialogue.textLine].fontHeight = (u8)this->msg.fontSize;
            this->msg.dialogueLines[args->dialogue.textLine].fontWidth =
                this->msg.dialogueLines[args->dialogue.textLine].fontHeight;
            AnmManager::DrawVmTextFmt(
                g_AnmManager, &this->msg.dialogueLines[args->dialogue.textLine],
                this->msg.textColorsA[args->dialogue.textColor],
                this->msg.textColorsB[args->dialogue.textColor], args->dialogue.text);
            this->msg.framesElapsedDuringPause = 0;
            break;
        case MSG_PAUSE:
            if (this->msg.dialogueSkippable == 0 || !IS_PRESSED_GAME(TH_BUTTON_SKIP))
            {
                if (!WAS_PRESSED_GAME(TH_BUTTON_SHOOT) || this->msg.framesElapsedDuringPause < 12)
                {
                    if (this->msg.framesElapsedDuringPause >=
                        this->msg.curInstr->args.pause.duration)
                    {
                        break;
                    }
                    this->msg.framesElapsedDuringPause++;
                    goto SKIP_TIME_INCREMENT;
                }
            }
            break;
        case MSG_SWITCH:
            args = &this->msg.curInstr->args;
            if (args->msgSwitch.unkIdx < 2)
            {
                this->msg.portraits[args->msgSwitch.unkIdx].pendingInterrupt =
                    args->msgSwitch.interrupt;
            }
            else
            {
                this->msg.dialogueLines[args->msgSwitch.unkIdx - 2].pendingInterrupt =
                    args->msgSwitch.interrupt;
            }
            break;
        case MSG_APPEAR_ENEMY:
            this->msg.ignoreWaitCounter++;
            break;
        case MSG_MUSIC:
            if (g_GameManager.currentStage != STAGE6)
            {
                g_AnmManager->SetAnmIdxAndExecuteScript(&this->stageTextVm[0],
                                                        ANM_SCRIPT_STAGE_TEXT_MUSIC);
            }
            else
            {
                g_AnmManager->SetAnmIdxAndExecuteScript(&this->stageTextVm[0],
                                                        ANM_SCRIPT_STAGE_TEXT_MUSIC_ST6);
            }
            g_AnmManager->SetActiveSprite(this->stageTextVm,
                                          this->msg.curInstr->args.music.musicIdx +
                                              ANM_SPRITE_STAGE_TEXT_MUSIC);
            if (g_Supervisor.PlayLoadedAudio(this->msg.curInstr->args.music.musicIdx) !=
                ZUN_SUCCESS)
            {
                g_Supervisor.PlayAudio(
                    g_Stage.stdData->bgmPaths[this->msg.curInstr->args.music.musicIdx]);
            }
            break;
        case MSG_TEXT_INTRODUCE:
        {
            args = &this->msg.curInstr->args;
#ifdef TH_DEV_TOOLS
            Supervisor::DebugPrint("th07 thcrap msg intro: stage=%d msg=%d line=%d text=%s\n",
                                   g_GameManager.currentStage, this->msg.currentMsgIdx,
                                   args->dialogue.textLine, args->dialogue.text);
#endif
            g_AnmManager->SetAnmIdxAndExecuteScript(&this->msg.introLines[args->dialogue.textLine],
                                                    args->dialogue.textLine + 1794);
            const bool bossImageApplied =
                args->dialogue.textLine == 0
                    ? Localization::ApplyBossTitleImage(
                          this->msg.introLines + args->dialogue.textLine,
                          static_cast<std::uint32_t>(g_GameManager.currentStage),
                          this->msg.portraits[1].activeSpriteIdx)
                    : Localization::ApplyBossNameImage(
                          this->msg.introLines + args->dialogue.textLine,
                          static_cast<std::uint32_t>(g_GameManager.currentStage),
                          this->msg.portraits[1].activeSpriteIdx);
            if (!bossImageApplied)
            {
                g_AnmManager->DrawStringFormat(this->msg.introLines + args->dialogue.textLine,
                                               this->msg.textColorsA[args->dialogue.textColor],
                                               this->msg.textColorsB[args->dialogue.textColor],
                                               args->dialogue.text);
            }
            this->msg.framesElapsedDuringPause = 0;
            break;
        }
        case MSG_STAGERESULTS:
            this->clearPower = g_GameManager.globals->currentPower;
            this->clearPointItems = g_GameManager.globals->pointItemsCollectedThisStage;
            this->clearCherryMax = g_GameManager.cherryMax - g_GameManager.globals->cherryStart;
            this->clearGraze = g_GameManager.globals->grazeInStage;
            this->finishedStage = 1;
            if (g_GameManager.currentStage < STAGE6)
            {
                g_AnmManager->SetAnmIdxAndExecuteScript(&this->stageClearBg,
                                                        ANM_SCRIPT_LOADING_STAGE_CLEAR_BG);
                g_AnmManager->SetAnmIdxAndExecuteScript(&this->stageTransitionSnapshotVm,
                                                        ANM_SCRIPT_CAPTURE_TRANSITION_SNAPSHOT);
                g_AnmManager->CreateScreenshotTexture(
                    this->stageTransitionSnapshotVm.sprite->startPixelInclusive.x,
                    this->stageTransitionSnapshotVm.sprite->startPixelInclusive.y,
                    this->stageTransitionSnapshotVm.sprite->widthPx,
                    this->stageTransitionSnapshotVm.sprite->heightPx);
                g_AnmManager->TakeScreenshotIfRequested();
            }
            else
            {
                g_GameManager.globals->extendsFromPointItems = -1;
            }
            break;
        case MSG_FREEZE:
            goto SKIP_TIME_INCREMENT;
        case MSG_FADEOUT_MUSIC:
            g_Supervisor.FadeOutMusic(4.0f);
            break;
        case MSG_FADE_IN_EFFECT:
            ScreenEffect::RegisterChain(SCREEN_EFFECT_FADE_IN_FULLSCREEN, 0x192, 0xffffff, 0, 0);
            g_Supervisor.renderSkipFrames = 0x192;
            break;
        case MSG_NEXT_LEVEL:
            g_Supervisor.checkTiming = FALSE;
            g_GameManager.globals->guiScore = g_GameManager.globals->score;
            if (g_GameManager.practice)
            {
                g_GameManager.globals->guiScore = g_GameManager.globals->score;
                g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROM_GAME;
                goto SKIP_TIME_INCREMENT;
            }

            if (g_GameManager.currentStage < STAGE6)
            {
                if (g_GameManager.replay &&
                    !g_ReplayManager->StageReplayExists(g_GameManager.currentStage))
                {
                    g_Supervisor.curState = SUPERVISOR_STATE_REPLAY_END;
                    goto SKIP_TIME_INCREMENT;
                }

                g_AnmManager->InitializeAndSetActiveSprite(&this->loadingSprite,
                                                           ANM_SPRITE_ASCII_LOADING);
                this->transitionToScoreScreen = TRUE;
                this->msg.currentMsgIdx = -2;
            }
            else if (!g_GameManager.replay)
            {
                if (g_GameManager.difficulty >= DIFF_EXTRA)
                {
                    if (g_GameManager.difficulty == DIFF_EXTRA)
                    {
                        g_GameManager.clrd[g_GameManager.shotTypeAndCharacter]
                            .difficultyClearedWithRetries[g_GameManager.difficulty] = 99;
                    }
                    g_GameManager.plst.playDataByDifficulty[g_GameManager.difficulty]
                        .noContinueClearCount++;
                    g_GameManager.finished = 1;
                    g_GameManager.globals->guiScore = g_GameManager.globals->score;
                    g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROM_GAME;
                    goto SKIP_TIME_INCREMENT;
                }
                else
                {
                    g_GameManager.finished = 1;
                    g_GameManager.globals->guiScore = g_GameManager.globals->score;
                    g_Supervisor.curState = SUPERVISOR_STATE_ENDING;
                    goto SKIP_TIME_INCREMENT;
                }
            }
            else
            {
                if (g_GameManager.currentStage == PHANTASMSTAGE &&
                    g_GameManager.globals->score != g_ReplayManager->data->data.score)
                {
                    ReplayManager::SaveReplay2(g_GameManager.replayFilename);
                }
                g_Supervisor.curState = SUPERVISOR_STATE_REPLAY_END;
            }
            goto SKIP_TIME_INCREMENT;
        case MSG_ALLOW_SKIP:
            this->msg.dialogueSkippable = *(u8 *)&this->msg.curInstr->args;
            break;
        }
        this->msg.curInstr =
            (MsgRawInstr *)((u8 *)&this->msg.curInstr->args + this->msg.curInstr->argsize);
    }
    this->msg.timer.NextTick();
SKIP_TIME_INCREMENT:
    g_AnmManager->ExecuteScript(&this->msg.portraits[0]);
    g_AnmManager->ExecuteScript(&this->msg.portraits[1]);
    g_AnmManager->ExecuteScript(&this->msg.dialogueLines[0]);
    g_AnmManager->ExecuteScript(&this->msg.dialogueLines[1]);
    g_AnmManager->ExecuteScript(&this->msg.introLines[0]);
    g_AnmManager->ExecuteScript(&this->msg.introLines[1]);
    if (this->msg.timer < 60 && this->msg.dialogueSkippable && IS_PRESSED_GAME(TH_BUTTON_SKIP))
    {
        this->msg.timer = 60;
    }

    return ZUN_SUCCESS;
}

ZunResult GuiImpl::DrawDialogue()
{
    ZunVec3 oldPos;
    f32 height;

    if (this->msg.currentMsgIdx < 0)
    {
        return ZUN_ERROR;
    }
    if (g_GameManager.currentStage == STAGE6 &&
        (this->msg.currentMsgIdx == 1 || this->msg.currentMsgIdx == 11))
    {
        return ZUN_SUCCESS;
    }

    if (this->msg.timer < 60)
    {
        height = this->msg.timer.AsFloat() * 48.0f / 60.0f;
    }
    else
    {
        height = 48.0f;
    }

    VertexDiffuseXyzrhw dialogueBg[4];
    dialogueBg[0].pos = ZunVec3(g_GameManager.arcadeRegionTopLeftPos.x + 16.0f, 384.0f, 0.0f);
    dialogueBg[1].pos =
        ZunVec3(g_GameManager.arcadeRegionTopLeftPos.x + 384.0f - 16.0f, 384.0f, 0.0f);
    dialogueBg[2].pos =
        ZunVec3(g_GameManager.arcadeRegionTopLeftPos.x + 16.0f, 384.0f + height, 0.0f);
    dialogueBg[3].pos =
        ZunVec3(g_GameManager.arcadeRegionTopLeftPos.x + 384.0f - 16.0f, 384.0f + height, 0.0f);
    dialogueBg[0].diffuse.color = dialogueBg[1].diffuse.color = 0xd0000000;
    dialogueBg[2].diffuse.color = dialogueBg[3].diffuse.color = 0x90000000;
    dialogueBg[0].w = dialogueBg[1].w = dialogueBg[2].w = dialogueBg[3].w = 1.0f;
    g_AnmManager->DrawInterpNoRotation(&this->msg.portraits[0]);
    oldPos = this->msg.portraits[1].pos;
    ZunVec3 drawPos =
        this->msg.portraits[1].prevPos.Lerp(this->msg.portraits[1].pos, g_RenderAlpha);
    this->msg.portraits[1].pos = drawPos + this->msg.portraits[1].offset;
    g_AnmManager->DrawNoRotation(&this->msg.portraits[1]);
    this->msg.portraits[1].pos = oldPos;
    g_AnmManager->Flush();
    g_Supervisor.gfxDevice->SetColorOp(COMPONENT_RGB, COLOR_OP_DISABLE);
    g_Supervisor.gfxDevice->SetColorOp(COMPONENT_ALPHA, COLOR_OP_DISABLE);
    if (!g_Supervisor.cfg.disableZBuffer)
    {
        g_Supervisor.gfxDevice->SetDepthMask(false);
    }
    g_Supervisor.gfxDevice->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, dialogueBg,
                                            sizeof(VertexDiffuseXyzrhw));
    g_AnmManager->SetVertexShader(255);
    g_AnmManager->SetColorOp(255);
    g_AnmManager->SetBlendMode(255);
    g_AnmManager->SetZWriteDisable(255);
    g_Supervisor.gfxDevice->SetColorOp(COMPONENT_RGB, COLOR_OP_MODULATE);
    g_Supervisor.gfxDevice->SetColorOp(COMPONENT_ALPHA, COLOR_OP_MODULATE);
    g_AnmManager->DrawInterpNoRotation(&this->msg.dialogueLines[0]);
    g_AnmManager->DrawInterpNoRotation(&this->msg.dialogueLines[1]);
    if (Localization::Active())
    {
        // base_tsa/th07 swaps the original two immediate VM offsets at
        // Rx2acfc/Rx2ad13: Name (introLines[1]) must render before Title
        // (introLines[0]) so the translated boss-title layers composite in
        // the same order as thcrap.
        g_AnmManager->DrawInterpNoRotation(&this->msg.introLines[1]);
        g_AnmManager->DrawInterpNoRotation(&this->msg.introLines[0]);
#ifdef TH_DEV_TOOLS
        static bool loggedLocalizedBossIntroOrder = false;
        if (!loggedLocalizedBossIntroOrder &&
            (this->msg.introLines[0].visible || this->msg.introLines[1].visible))
        {
            SDL_Log("th07 thcrap boss intro draw order: localization=1 order=1,0");
            loggedLocalizedBossIntroOrder = true;
        }
#endif
    }
    else
    {
        g_AnmManager->DrawInterpNoRotation(&this->msg.introLines[0]);
        g_AnmManager->DrawInterpNoRotation(&this->msg.introLines[1]);
#ifdef TH_DEV_TOOLS
        static bool loggedJapaneseBossIntroOrder = false;
        if (!loggedJapaneseBossIntroOrder &&
            (this->msg.introLines[0].visible || this->msg.introLines[1].visible))
        {
            SDL_Log("th07 thcrap boss intro draw order: localization=0 order=0,1");
            loggedJapaneseBossIntroOrder = true;
        }
#endif
    }
    return ZUN_SUCCESS;
}

i32 Gui::MsgWait()
{
    if (!this->impl)
    {
        return 0;
    }

    if (this->impl->msg.ignoreWaitCounter > 0)
    {
        return 0;
    }

    return this->impl->msg.currentMsgIdx >= 0;
}

i32 Gui::IsWaitingForPlayerAdvance()
{
    return this->impl != nullptr && this->impl->msg.currentMsgIdx >= 0 &&
           this->impl->msg.curInstr != nullptr && this->impl->msg.curInstr->opcode == MSG_PAUSE;
}

i32 Gui::HasCurrentMsgIdx()
{
    if (!this->impl)
    {
        return 0;
    }

    return this->impl->msg.currentMsgIdx >= 0 || this->impl->msg.currentMsgIdx == -2;
}

void Gui::UpdateGui()
{
    i32 scoreBonus;
    i32 i;
    i32 activeTransitionQuads;

    this->UpdatePrev();

    if (this->impl->msg.currentMsgIdx < 0)
    {
        if (this->bossPresent)
        {
            if (this->impl->bossHealthBarState == 0)
            {
                this->impl->vms0[11].SetInterrupt(1);
                this->impl->bossHealthBarState = 1;
                this->bossHealthBarAlpha = 0;
            }
            else
            {
                if (this->impl->vms0[11].isStopped)
                {
                    this->impl->bossHealthBarState = 2;
                }
                if (this->bossHealthBarAlpha < 252)
                {
                    this->bossHealthBarAlpha += 4;
                }
                else
                {
                    this->bossHealthBarAlpha = 255;
                }
            }
        }
        else if (this->impl->bossHealthBarState != 0)
        {
            if (this->impl->bossHealthBarState <= 2)
            {
                this->impl->vms0[11].SetInterrupt(2);
                this->impl->bossHealthBarState = 3;
            }
            if (this->bossHealthBarAlpha > 0)
            {
                this->bossHealthBarAlpha -= 4;
            }
            else
            {
                this->bossHealthBarAlpha = 0;
            }
            if (this->impl->vms0[11].isStopped)
            {
                this->impl->bossHealthBarState = 0;
                this->bossHealthBarEased = 0.0f;
                this->bossHealthBarAlpha = 0;
            }
        }

        if (this->impl->bossHealthBarState >= 2)
        {
            if (this->bossHealthBar > this->bossHealthBarEased)
            {
                this->bossHealthBarEased += 0.01f;
                if (this->bossHealthBar < this->bossHealthBarEased)
                {
                    this->bossHealthBarEased = this->bossHealthBar;
                }
            }
            else if (this->bossHealthBar < this->bossHealthBarEased)
            {
                this->bossHealthBarEased -= 0.02f;
                if (this->bossHealthBar > this->bossHealthBarEased)
                {
                    this->bossHealthBarEased = this->bossHealthBar;
                }
            }
        }
    }
    g_AnmManager->ExecuteScripts(this->impl->vms0, ARRAY_SIZE_SIGNED(this->impl->vms0));
    g_AnmManager->ExecuteScripts(this->impl->stageTextVm,
                                 ARRAY_SIZE_SIGNED(this->impl->stageTextVm));
    g_AnmManager->ExecuteScript(&this->impl->bombSpellcardPortrait);
    g_AnmManager->ExecuteScript(&this->impl->bombSpellcardDecorLeft);
    g_AnmManager->ExecuteScript(&this->impl->bombSpellcardDecorRight);
    g_AnmManager->ExecuteScript(&this->impl->bombSpellcardName);
    g_AnmManager->ExecuteScript(&this->impl->enemySpellcardPortrait);
    g_AnmManager->ExecuteScript(&this->impl->enemySpellcardDecorHorizontalUp);
    g_AnmManager->ExecuteScript(&this->impl->enemySpellcardDecorHorizontalDown);
    g_AnmManager->ExecuteScript(&this->impl->enemySpellcardName);
    g_AnmManager->ExecuteScript(&this->impl->bombSpellcardNameBg);
    g_AnmManager->ExecuteScript(&this->impl->enemySpellcardNameBg);
    g_AnmManager->ExecuteScript(&this->impl->spellcardBonusIndicator);
    if (this->impl->stageClearBg.activeSpriteIdx >= 0)
    {
        if (g_AnmManager->ExecuteScript(&this->impl->stageClearBg))
        {
            this->impl->stageClearBg.activeSpriteIdx = -1;
        }
        if (g_AnmManager->ExecuteScript(&this->impl->stageTransitionSnapshotVm) != 0)
        {
            this->impl->stageTransitionSnapshotVm.activeSpriteIdx = -1;
        }
    }
    if (this->impl->activeTransitionQuads != 0)
    {
        activeTransitionQuads = ARRAY_SIZE_SIGNED(this->impl->transitionQuads);
        for (i = 0; i < ARRAY_SIZE_SIGNED(this->impl->transitionQuads); i++)
        {
            if (g_AnmManager->ExecuteScript(this->impl->transitionQuads + i))
            {
                activeTransitionQuads--;
            }
        }
        this->impl->activeTransitionQuads = activeTransitionQuads;
    }
    if (this->impl->bonusScore.displayArg != GUI_DISPLAY_HIDDEN)
    {
        this->impl->bonusScore.prevPos = this->impl->bonusScore.pos;
        if (this->impl->bonusScore.timer < 30)
        {
            this->impl->bonusScore.pos.x =
                this->impl->bonusScore.timer.AsFloat() * -312.0f / 30.0f + 416.0f;
        }
        else
        {
            this->impl->bonusScore.pos.x = 104.0f;
        }
        if (this->impl->bonusScore.timer >= 250)
        {
            this->impl->bonusScore.displayArg = GUI_DISPLAY_HIDDEN;
        }
        ++this->impl->bonusScore.timer;
    }
    if (this->impl->statusPopup.displayArg != GUI_DISPLAY_HIDDEN)
    {
        this->impl->statusPopup.prevPos = this->impl->statusPopup.pos;
        if (this->impl->statusPopup.timer < 30)
        {
            this->impl->statusPopup.pos.x =
                this->impl->statusPopup.timer.AsFloat() * -312.0f / 30.0f + 416.0f;
        }
        else
        {
            this->impl->statusPopup.pos.x = 104.0f;
        }
        if (this->impl->statusPopup.timer >= 180)
        {
            this->impl->statusPopup.displayArg = GUI_DISPLAY_HIDDEN;
        }
        ++this->impl->statusPopup.timer;
    }
    if (this->impl->spellCardBonus.displayArg != GUI_DISPLAY_HIDDEN)
    {
        if (this->impl->spellCardBonus.timer >= 280)
        {
            this->impl->spellCardBonus.displayArg = GUI_DISPLAY_HIDDEN;
        }
        ++this->impl->spellCardBonus.timer;
    }
    if (this->impl->finishedStage == 1)
    {
        scoreBonus = 0;
        scoreBonus += g_GameManager.currentStage * 100000;
        scoreBonus += this->impl->clearGraze * 50;
        scoreBonus += this->impl->clearPointItems * 5000;
        scoreBonus += this->impl->clearCherryMax;
        if (g_GameManager.currentStage >= 7 ||
            (g_GameManager.currentStage == 6 &&
             (!g_GameManager.practice || PracticeRuntime::AdvancedAllClearBonus()) &&
             (!g_GameManager.replay || g_ReplayManager->data->stageReplayData[4])))
        {
            scoreBonus += (i32)g_GameManager.globals->livesRemaining * 2000000;
            scoreBonus += (i32)g_GameManager.globals->bombsRemaining * 400000;
        }
        switch (g_GameManager.difficulty)
        {
        case DIFF_EASY:
            scoreBonus /= 2;
            break;
        case DIFF_HARD:
            scoreBonus = scoreBonus * 12 / 10;
            break;
        case DIFF_LUNATIC:
            scoreBonus = scoreBonus * 15 / 10;
            break;
        case DIFF_EXTRA:
            scoreBonus <<= 1;
            break;
        case DIFF_PHANTASM:
            scoreBonus <<= 1;
            break;
        }

        switch (g_GameManager.defaultCfg->lifeCount)
        {
        case 3:
            scoreBonus = scoreBonus * 5 / 10;
            break;
        case 4:
            scoreBonus = (scoreBonus << 1) / 10;
            break;
        }
        this->impl->stageClearBonus = scoreBonus;

        g_GameManager.AddScore(scoreBonus * 10);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        // These are per-stage contribution diagnostics in final sbrik. Keep
        // their lifetime deterministic even when this endpoint hides the
        // optional presentation-only readout.
        for (i32 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            g_cherryMaxGrazeGrowth[playerId] = 0;
            g_cherryMaxBreakGrowth[playerId] = 0;
        }
#endif
        this->impl->finishedStage++;
    }
    this->lifeDisplayUpdateFrames = 2;
    this->bombDisplayUpdateFrames = 2;
    this->grazeDisplayUpdateFrames = 2;
    this->pointDisplayUpdateFrames = 2;
    this->powerDisplayUpdateFrames = 2;
    if (this->lifeDisplayUpdateFrames)
    {
        this->lifeDisplayUpdateFrames--;
    }
    if (this->powerDisplayUpdateFrames)
    {
        this->powerDisplayUpdateFrames--;
    }
    if (this->bombDisplayUpdateFrames)
    {
        this->bombDisplayUpdateFrames--;
    }
    if (this->grazeDisplayUpdateFrames)
    {
        this->grazeDisplayUpdateFrames--;
    }
}

void Gui::DrawGameScene()
{
    ZunVec3 textDrawPos;
    AnmVm *vm;
    i32 i;
    f32 x;
    f32 y;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const bool compactMultiplayerHud = MultiplayerGameplay::IsMultiplayer();
    const f32 multiplayerHudBaseY = 76.0f;
    const f32 multiplayerHudOffsetX = 8.0f;
    const f32 multiplayerScoreBaseY = 34.0f;
    const f32 resourceIconStep = 11.0f;
    i32 playerId;
    i32 resourceCount;
    f32 blockY;
    AnmVm rowBackgroundVm;
    AnmVm playerLabelVm;
    AnmVm bombLabelVm;
    AnmVm powerLabelVm;
    Float2 savedIconScale;
    u32 contributionKills;
    u32 contributionDamage;
#else
    const bool compactMultiplayerHud = false;
#endif

    g_AnmManager->Flush();
    g_Supervisor.viewport.x = 0;
    g_Supervisor.viewport.y = 0;
    g_Supervisor.viewport.width = GAME_WINDOW_WIDTH;
    g_Supervisor.viewport.height = GAME_WINDOW_HEIGHT;
    g_Supervisor.gfxDevice->SetViewport(g_Supervisor.viewport);
    vm = &this->impl->vms0[12];
    if (g_Supervisor.cfg.redrawEveryFrame || vm->currentInstruction ||
        g_GameManager.isInPauseMenu || g_GameManager.isInRetryMenu ||
        g_Supervisor.renderSkipFrames != 0)
    {
        for (y = 0.0f; y < 464.0f; y = y + 32.0f)
        {
            vm->pos = ZunVec3(0.0f, y, 0.49f);
            g_AnmManager->DrawNoRotation(vm);
        }
        for (x = 416.0f; x < 624.0f; x = x + 32.0f)
        {
            for (y = 16.0f; y < 464.0f; y = y + 32.0f)
            {
                vm->pos = ZunVec3(x, y, 0.49f);
                g_AnmManager->DrawNoRotation(vm);
            }
        }
        vm = &this->impl->vms0[13];
        for (x = 0.0f; x < 624.0f; x = x + 128.0f)
        {
            vm->pos = ZunVec3(x, 0.0f, 0.49f);
            g_AnmManager->DrawNoRotation(vm);
            vm->pos = ZunVec3(x, 464.0f, 0.49f);
            g_AnmManager->DrawNoRotation(vm);
        }
        g_AnmManager->DrawNoRotation(this->impl->vms0);
        g_AnmManager->Draw(this->impl->vms0 + 1);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (compactMultiplayerHud)
        {
            this->impl->vms0[2].pos.y -= 14.0f;
            this->impl->vms0[3].pos.y -= 14.0f;
        }
#endif
        g_AnmManager->DrawNoRotation(this->impl->vms0 + 2);
        g_AnmManager->DrawNoRotation(this->impl->vms0 + 3);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (compactMultiplayerHud)
        {
            this->impl->vms0[2].pos.y += 14.0f;
            this->impl->vms0[3].pos.y += 14.0f;
        }
        else
#endif
        {
            g_AnmManager->DrawNoRotation(this->impl->vms0 + 4);
            g_AnmManager->DrawNoRotation(this->impl->vms0 + 5);
            g_AnmManager->DrawNoRotation(this->impl->vms0 + 6);
            g_AnmManager->DrawNoRotation(this->impl->vms0 + 7);
            g_AnmManager->DrawNoRotation(this->impl->vms0 + 8);
        }
        this->lifeDisplayUpdateFrames = 2;
        this->bombDisplayUpdateFrames = 2;
        this->grazeDisplayUpdateFrames = 2;
        this->pointDisplayUpdateFrames = 2;
        this->powerDisplayUpdateFrames = 2;
    }
    if (!g_Supervisor.cfg.disableItemDrawAroundPlayfield)
    {
        vm = &this->impl->vms0[13];
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (compactMultiplayerHud)
        {
            x = 512.0f;
            vm->pos = ZunVec3(x, multiplayerScoreBaseY, 0.49f);
            g_AnmManager->DrawNoRotation(vm);
            vm->pos = ZunVec3(x, multiplayerScoreBaseY + 16.0f, 0.49f);
            g_AnmManager->DrawNoRotation(vm);

            // Compact multiplayer rows are cleared every draw. This keeps
            // independent life/bomb/power values from leaving stale glyphs
            // when one player's resource count shrinks.
            for (playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
            {
                blockY = multiplayerHudBaseY + 48.0f * playerId;
                for (x = 416.0f; x < 528.0f; x += 16.0f)
                {
                    vm->pos = ZunVec3(x, blockY, 0.48f);
                    g_AnmManager->DrawNoRotation(vm);
                    vm->pos = ZunVec3(x, blockY + 12.0f, 0.48f);
                    g_AnmManager->DrawNoRotation(vm);
                    vm->pos = ZunVec3(x, blockY + 24.0f, 0.48f);
                    g_AnmManager->DrawNoRotation(vm);
                }
            }
        }
        else
#endif
        {
        x = 496.0f;
        vm->pos = ZunVec3(x, 48.0f, 0.49f);
        g_AnmManager->DrawNoRotation(vm);
        vm->pos = ZunVec3(x, 64.0f, 0.49f);
        g_AnmManager->DrawNoRotation(vm);
        if (this->lifeDisplayUpdateFrames)
        {
            vm->pos = ZunVec3(x, 96.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        if (this->bombDisplayUpdateFrames)
        {
            vm->pos = ZunVec3(x, 112.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        if (this->powerDisplayUpdateFrames)
        {
            vm->pos = ZunVec3(x, 144.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        if (this->grazeDisplayUpdateFrames)
        {
            vm->pos = ZunVec3(x, 160.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        if (this->pointDisplayUpdateFrames)
        {
            vm->pos = ZunVec3(x, 176.0f, 0.48f);
            g_AnmManager->DrawNoRotation(vm);
        }
        }
        vm->pos = ZunVec3(512.0f, 464.0f, 0.48f);
        g_AnmManager->DrawNoRotation(vm);
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (compactMultiplayerHud &&
        !g_Supervisor.cfg.disableItemDrawAroundPlayfield &&
        (this->grazeDisplayUpdateFrames || this->pointDisplayUpdateFrames))
    {
        // The lower Graze/Point value area needs clearing without erasing the
        // adjacent logo. Match the upstream half-width tile and publish the
        // temporary scale to Eagler's interpolation state as well.
        g_AnmManager->Flush();
        rowBackgroundVm = this->impl->vms0[13];
        rowBackgroundVm.scale.x *= 0.5f;
        rowBackgroundVm.prevScale = rowBackgroundVm.scale;
        if (this->grazeDisplayUpdateFrames)
        {
            rowBackgroundVm.pos = ZunVec3(480.0f, 224.0f, 0.48f);
            g_AnmManager->DrawNoRotation(&rowBackgroundVm);
        }
        if (this->pointDisplayUpdateFrames)
        {
            rowBackgroundVm.pos = ZunVec3(480.0f, 240.0f, 0.48f);
            g_AnmManager->DrawNoRotation(&rowBackgroundVm);
        }
        g_AnmManager->Flush();
    }
    if (compactMultiplayerHud)
    {
        playerLabelVm = this->impl->vms0[4];
        bombLabelVm = this->impl->vms0[5];
        powerLabelVm = this->impl->vms0[6];
        playerLabelVm.scale = Float2{0.80f, 0.80f};
        bombLabelVm.scale = Float2{0.80f, 0.80f};
        powerLabelVm.scale = Float2{0.80f, 0.80f};
        playerLabelVm.prevScale = playerLabelVm.scale;
        bombLabelVm.prevScale = bombLabelVm.scale;
        powerLabelVm.prevScale = powerLabelVm.scale;

        for (playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            if (!IsPlayerSlotActive((u8)playerId) ||
                MultiplayerGameplay::IsPlayerTemporarilyAbsent((u8)playerId))
                continue;
            blockY = multiplayerHudBaseY + 48.0f * playerId;
            playerLabelVm.pos = ZunVec3(481.0f + multiplayerHudOffsetX, blockY, 0.47f);
            bombLabelVm.pos = ZunVec3(481.0f + multiplayerHudOffsetX, blockY + 12.0f, 0.47f);
            powerLabelVm.pos = ZunVec3(481.0f + multiplayerHudOffsetX, blockY + 24.0f, 0.47f);
            g_AnmManager->DrawNoRotation(&playerLabelVm);
            g_AnmManager->DrawNoRotation(&bombLabelVm);
            g_AnmManager->DrawNoRotation(&powerLabelVm);
        }

        // Graze/Point labels stay below the three player rows.
        this->impl->vms0[7].pos.y += 60.0f;
        this->impl->vms0[8].pos.y += 60.0f;
        g_AnmManager->DrawNoRotation(this->impl->vms0 + 7);
        g_AnmManager->DrawNoRotation(this->impl->vms0 + 8);
        this->impl->vms0[7].pos.y -= 60.0f;
        this->impl->vms0[8].pos.y -= 60.0f;

        vm = &this->impl->vms0[9];
        savedIconScale = vm->scale;
        const Float2 savedIconPrevScale = vm->prevScale;
        vm->scale = Float2{0.65f, 0.65f};
        vm->prevScale = vm->scale;
        for (playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            if (!IsPlayerSlotActive((u8)playerId) ||
                MultiplayerGameplay::IsPlayerTemporarilyAbsent((u8)playerId))
                continue;
            resourceCount = GetPlayerLives((u8)playerId);
            blockY = multiplayerHudBaseY + 48.0f * playerId;
            for (i = 0, x = 532.0f + multiplayerHudOffsetX;
                 i < resourceCount; ++i, x += resourceIconStep)
            {
                vm->pos = ZunVec3(x, blockY, 0.46f);
                g_AnmManager->DrawNoRotation(vm);
            }
        }
        vm->scale = savedIconScale;
        vm->prevScale = savedIconPrevScale;

        vm = &this->impl->vms0[10];
        savedIconScale = vm->scale;
        const Float2 savedBombIconPrevScale = vm->prevScale;
        vm->scale = Float2{0.65f, 0.65f};
        vm->prevScale = vm->scale;
        for (playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            if (!IsPlayerSlotActive((u8)playerId) ||
                MultiplayerGameplay::IsPlayerTemporarilyAbsent((u8)playerId))
                continue;
            resourceCount = GetPlayerBombs((u8)playerId);
            blockY = multiplayerHudBaseY + 12.0f + 48.0f * playerId;
            for (i = 0, x = 532.0f + multiplayerHudOffsetX;
                 i < resourceCount; ++i, x += resourceIconStep)
            {
                vm->pos = ZunVec3(x, blockY, 0.46f);
                g_AnmManager->DrawNoRotation(vm);
            }
        }
        vm->scale = savedIconScale;
        vm->prevScale = savedBombIconPrevScale;
    }
    else
#endif
    if (this->lifeDisplayUpdateFrames)
    {
        vm = &this->impl->vms0[9];
        for (i = 0, x = 496.0f; i < (i32)g_GameManager.globals->livesRemaining; i++, x += 16.0f)
        {
            vm->pos = ZunVec3(x, 96.0f, 0.46f);
            g_AnmManager->DrawNoRotation(vm);
        }
    }
    if (!compactMultiplayerHud && this->bombDisplayUpdateFrames)
    {
        vm = &this->impl->vms0[10];
        for (i = 0, x = 496.0f; i < (i32)g_GameManager.globals->bombsRemaining; i++, x += 16.0f)
        {
            vm->pos = ZunVec3(x, 112.0f, 0.46f);
            g_AnmManager->DrawNoRotation(vm);
        }
    }
    vm = &this->impl->vms0[13];
    for (x = 32.0f; x < 368.0f; x = x + 128.0f)
    {
        vm->pos = ZunVec3(x, 464.0f, 0.49f);
        g_AnmManager->DrawNoRotation(vm);
    }
    textDrawPos.x = 496.0f;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    textDrawPos.y = compactMultiplayerHud ? multiplayerScoreBaseY + 16.0f : 64.0f;
#else
    textDrawPos.y = 64.0f;
#endif
    textDrawPos.z = 0.0f;
    if (g_GameManager.globals->guiScore < 100000000)
    {
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%.8d",
                                    g_GameManager.globals->guiScore);
        textDrawPos.x = textDrawPos.x + 112.0f;
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%1d",
                                    (u32)g_GameManager.globals->numRetries);
    }
    else
    {
        g_AsciiManager.scale.x = 0.9f;
        g_AsciiManager.scale.y = 1.0f;
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%.9d",
                                    g_GameManager.globals->guiScore);
        textDrawPos.x = textDrawPos.x + 113.399994f;
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%1d",
                                    (u32)g_GameManager.globals->numRetries);
        g_AsciiManager.scale.x = 1.0f;
        g_AsciiManager.scale.y = 1.0f;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    textDrawPos = ZunVec3(496.0f,
                          compactMultiplayerHud ? multiplayerScoreBaseY : 48.0f,
                          0.0f);
#else
    textDrawPos = ZunVec3(496.0f, 48.0f, 0.0f);
#endif
    if (g_GameManager.globals->highScore < 100000000)
    {
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%.8d",
                                    g_GameManager.globals->highScore);
        textDrawPos.x = textDrawPos.x + 112.0f;
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%1d",
                                    (u32)g_GameManager.globals->highScoreNumContinues);
    }
    else
    {
        g_AsciiManager.scale.x = 0.9f;
        g_AsciiManager.scale.y = 1.0f;
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%.9d",
                                    g_GameManager.globals->highScore);
        textDrawPos.x = textDrawPos.x + 113.399994f;
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%1d",
                                    (u32)g_GameManager.globals->highScoreNumContinues);
        g_AsciiManager.scale.x = 1.0f;
        g_AsciiManager.scale.y = 1.0f;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (compactMultiplayerHud)
    {
        const Float2 savedScale = g_AsciiManager.scale;
        const u32 savedColor = g_AsciiManager.color;
        const i32 savedGui = g_AsciiManager.isGui;
        const i32 savedSelected = g_AsciiManager.isSelected;
        g_AsciiManager.isGui = 0;
        g_AsciiManager.isSelected = 0;

        for (playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            if (!IsPlayerSlotActive((u8)playerId))
                continue;

            blockY = multiplayerHudBaseY + 48.0f * playerId;
            g_AsciiManager.scale = Float2{0.55f, 0.55f};
            g_AsciiManager.color = 0xffffffff;
            textDrawPos = ZunVec3(424.0f + multiplayerHudOffsetX, blockY, 0.0f);
            AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "P%d", playerId + 1);

            g_AsciiManager.scale = Float2{0.50f, 0.50f};
            g_AsciiManager.color = 0xff80c0ff;
            textDrawPos = ZunVec3(424.0f + multiplayerHudOffsetX,
                                  blockY + 14.0f, 0.0f);
            g_AsciiManager.AddString(&textDrawPos,
                                     GetMultiplayerHudLoadoutName((u8)playerId));

            if (MultiplayerGameplay::ShouldShowContributionStats())
            {
                contributionKills = GetPlayerEnemiesDefeated((u8)playerId);
                contributionDamage = GetPlayerDamageDealt((u8)playerId);
                g_AsciiManager.scale = Float2{0.42f, 0.42f};
                g_AsciiManager.color = 0xffa0d8ff;
                textDrawPos = ZunVec3(424.0f + multiplayerHudOffsetX,
                                      blockY + 27.0f, 0.0f);
                AsciiManager::AddFormatText(
                    &g_AsciiManager, &textDrawPos, "K:%u D:%u",
                    contributionKills, contributionDamage);
            }

            if (MultiplayerGameplay::IsPlayerTemporarilyAbsent((u8)playerId))
            {
                g_AsciiManager.scale = Float2{0.50f, 0.50f};
                g_AsciiManager.color = 0xffffc080;
                textDrawPos = ZunVec3(486.0f + multiplayerHudOffsetX,
                                      blockY + 12.0f, 0.0f);
                g_AsciiManager.AddString(&textDrawPos, "AWAY");
            }
        }

        g_AsciiManager.scale = savedScale;
        g_AsciiManager.color = savedColor;
        g_AsciiManager.isGui = savedGui;
        g_AsciiManager.isSelected = savedSelected;
    }
#endif
    if (this->grazeDisplayUpdateFrames || g_Supervisor.cfg.disableItemDrawAroundPlayfield)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const Float2 savedGrazeScale = g_AsciiManager.scale;
        if (compactMultiplayerHud)
            g_AsciiManager.scale = Float2{0.70f, 0.70f};
        textDrawPos = ZunVec3(compactMultiplayerHud ? 488.0f : 496.0f,
                              compactMultiplayerHud ? 224.0f : 160.0f, 0.0f);
#else
        textDrawPos = ZunVec3(496.0f, 160.0f, 0.0f);
#endif
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%d",
                                    g_GameManager.globals->grazeInTotal);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (compactMultiplayerHud)
            g_AsciiManager.scale = savedGrazeScale;
#endif
    }
    if (this->pointDisplayUpdateFrames || g_Supervisor.cfg.disableItemDrawAroundPlayfield)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const Float2 savedPointScale = g_AsciiManager.scale;
        if (compactMultiplayerHud)
            g_AsciiManager.scale = Float2{0.70f, 0.70f};
        textDrawPos = ZunVec3(compactMultiplayerHud ? 488.0f : 496.0f,
                              compactMultiplayerHud ? 240.0f : 176.0f, 0.0f);
#else
        textDrawPos = ZunVec3(496.0f, 176.0f, 0.0f);
#endif
        AsciiManager::AddFormatText(&g_AsciiManager, &textDrawPos, "%d/%d",
                                    g_GameManager.globals->pointItemsCollectedForExtend,
                                    g_GameManager.globals->nextNeededPointItemsForExtend);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (compactMultiplayerHud)
            g_AsciiManager.scale = savedPointScale;
#endif
    }
    g_AnmManager->Flush();
    if (this->powerDisplayUpdateFrames || compactMultiplayerHud ||
        g_Supervisor.cfg.disableItemDrawAroundPlayfield)
    {
        VertexDiffuseXyzrhw powerBarVerts[4];
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (compactMultiplayerHud)
        {
            const Float2 savedScale = g_AsciiManager.scale;
            const u32 savedColor = g_AsciiManager.color;
            const i32 savedGui = g_AsciiManager.isGui;
            const i32 savedSelected = g_AsciiManager.isSelected;
            g_AsciiManager.scale = Float2{0.70f, 0.70f};
            g_AsciiManager.color = 0xffffffff;
            g_AsciiManager.isGui = 0;
            g_AsciiManager.isSelected = 0;

            for (playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
            {
                if (!IsPlayerSlotActive((u8)playerId) ||
                    MultiplayerGameplay::IsPlayerTemporarilyAbsent((u8)playerId))
                    continue;

                const i32 playerPower = GetPlayerPower((u8)playerId);
                const f32 powerBarLeft = 532.0f + multiplayerHudOffsetX;
                const f32 powerBarTop = multiplayerHudBaseY + 26.0f + 48.0f * playerId;
                const f32 powerBarBottom = multiplayerHudBaseY + 36.0f + 48.0f * playerId;
                const f32 powerBarRight = powerBarLeft + (f32)playerPower * 0.25f;

                if (playerPower > 0)
                {
                    powerBarVerts[0].pos = ZunVec3(powerBarLeft, powerBarTop, 0.1f);
                    powerBarVerts[1].pos = ZunVec3(powerBarRight, powerBarTop, 0.1f);
                    powerBarVerts[2].pos = ZunVec3(powerBarLeft, powerBarBottom, 0.1f);
                    powerBarVerts[3].pos = ZunVec3(powerBarRight, powerBarBottom, 0.1f);
                    powerBarVerts[0].diffuse.color = powerBarVerts[2].diffuse.color = 0xe0e0e0ff;
                    powerBarVerts[1].diffuse.color = powerBarVerts[3].diffuse.color = 0x80e0e0ff;
                    powerBarVerts[0].w = powerBarVerts[1].w =
                        powerBarVerts[2].w = powerBarVerts[3].w = 1.0f;
                    g_Supervisor.gfxDevice->SetColorOp(COMPONENT_RGB, COLOR_OP_DISABLE);
                    g_Supervisor.gfxDevice->SetColorOp(COMPONENT_ALPHA, COLOR_OP_DISABLE);
                    if (!g_Supervisor.cfg.disableZBuffer)
                        g_Supervisor.gfxDevice->SetDepthMask(false);
                    g_Supervisor.gfxDevice->DrawPrimitiveUP(
                        PRIM_TRIANGLE_STRIP, 2, &powerBarVerts,
                        sizeof(VertexDiffuseXyzrhw));
                    g_AnmManager->SetVertexShader(255);
                    g_AnmManager->SetColorOp(255);
                    g_AnmManager->SetBlendMode(255);
                    g_AnmManager->SetZWriteDisable(255);
                    g_Supervisor.gfxDevice->SetColorOp(COMPONENT_RGB, COLOR_OP_MODULATE);
                    g_Supervisor.gfxDevice->SetColorOp(COMPONENT_ALPHA, COLOR_OP_MODULATE);
                }

                ZunVec3 pos(powerBarLeft, powerBarTop, 0.0f);
                if (playerPower < 128)
                    AsciiManager::AddFormatText(&g_AsciiManager, &pos, "%d", playerPower);
                else
                    g_AsciiManager.AddString(&pos, "MAX");
            }

            g_AsciiManager.scale = savedScale;
            g_AsciiManager.color = savedColor;
            g_AsciiManager.isGui = savedGui;
            g_AsciiManager.isSelected = savedSelected;
        }
        else
#endif
        {
        if (0 < (i32)g_GameManager.globals->currentPower)
        {
            powerBarVerts[0].pos = ZunVec3(496.0f, 144.0f, 0.1f);
            powerBarVerts[1].pos = ZunVec3(
                (f32)((i32)g_GameManager.globals->currentPower + 0x1f0) + 0.0f, 144.0f, 0.1f);
            powerBarVerts[2].pos = ZunVec3(496.0f, 160.0f, 0.1f);
            powerBarVerts[3].pos = ZunVec3(
                (f32)((i32)g_GameManager.globals->currentPower + 0x1f0) + 0.0f, 160.0f, 0.1f);
            powerBarVerts[0].diffuse.color = powerBarVerts[2].diffuse.color = 0xe0e0e0ff;
            powerBarVerts[1].diffuse.color = powerBarVerts[3].diffuse.color = 0x80e0e0ff;

            powerBarVerts[0].w = powerBarVerts[1].w = powerBarVerts[2].w = powerBarVerts[3].w =
                1.0f;
            g_Supervisor.gfxDevice->SetColorOp(COMPONENT_RGB, COLOR_OP_DISABLE);
            g_Supervisor.gfxDevice->SetColorOp(COMPONENT_ALPHA, COLOR_OP_DISABLE);
            if (!g_Supervisor.cfg.disableZBuffer)
            {
                g_Supervisor.gfxDevice->SetDepthMask(false);
            }
            g_Supervisor.gfxDevice->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &powerBarVerts,
                                                    sizeof(VertexDiffuseXyzrhw));
            g_AnmManager->SetVertexShader(255);
            g_AnmManager->SetColorOp(255);
            g_AnmManager->SetBlendMode(255);
            g_AnmManager->SetZWriteDisable(255);
            g_Supervisor.gfxDevice->SetColorOp(COMPONENT_RGB, COLOR_OP_MODULATE);
            g_Supervisor.gfxDevice->SetColorOp(COMPONENT_ALPHA, COLOR_OP_MODULATE);
        }

        ZunVec3 pos;
        if ((i32)g_GameManager.globals->currentPower < 128)
        {
            pos = ZunVec3(496.0f, 144.0f, 0.0f);
            AsciiManager::AddFormatText(&g_AsciiManager, &pos, "%d",
                                        (i32)g_GameManager.globals->currentPower);
        }
        else
        {
            pos = ZunVec3(496.0f, 144.0f, 0.0f);
            AsciiManager::AddFormatText(&g_AsciiManager, &pos, "MAX");
        }
        }
    }
}

void Gui::DrawStageElements()
{
    ZunVec3 timerPos;
    i32 markerGap;
    u32 timeColor;
    f32 segmentEndHealth;
    i32 j;
    i32 secondsRemaining;
    ZunRect healthBarRect;
    u32 color1;
    u32 color2;
    ZunBool leadingZeroSkipped;
    i32 digitDivisor;
    i32 digit;
    Catk *catk;
    i32 remainingBonus;
    ZunVec3 oldPos;
    i32 i;

    for (i = 0; i < 5; i++)
    {
        g_AnmManager->DrawInterp(&this->impl->stageTextVm[i]);
    }
    if (this->impl->enemySpellcardPortrait.visible)
    {
        oldPos = this->impl->enemySpellcardPortrait.pos;
        ZunVec3 drawPos = this->impl->enemySpellcardPortrait.prevPos.Lerp(
            this->impl->enemySpellcardPortrait.pos, g_RenderAlpha);
        this->impl->enemySpellcardPortrait.pos =
            drawPos + this->impl->enemySpellcardPortrait.offset;
        g_AnmManager->DrawNoRotation(&this->impl->enemySpellcardPortrait);
        this->impl->enemySpellcardPortrait.pos = oldPos;
        g_AnmManager->DrawInterpNoRotation(&this->impl->enemySpellcardDecorHorizontalUp);
        g_AnmManager->DrawInterp(&this->impl->enemySpellcardDecorHorizontalDown);
    }
    if (this->impl->bombSpellcardName.visible)
    {
        this->impl->bombSpellcardNameBg.pos = this->impl->bombSpellcardName.pos;
        g_AnmManager->DrawInterpNoRotation(&this->impl->bombSpellcardNameBg);
        g_AnmManager->DrawInterp(&this->impl->bombSpellcardName);
    }
    if (this->impl->enemySpellcardName.visible)
    {
        this->impl->enemySpellcardNameBg.pos = this->impl->enemySpellcardName.pos;
        g_AnmManager->DrawInterpNoRotation(&this->impl->enemySpellcardNameBg);
        g_AnmManager->DrawInterp(&this->impl->enemySpellcardName);
        g_AnmManager->DrawNoRotation(&this->impl->spellcardBonusIndicator);
        remainingBonus = g_EnemyManager.spellcardInfo.captureScore +
                         g_EnemyManager.spellcardInfo.grazeBonusScore;
        const bool fixSpellBonusDisplay = PracticeRuntime::AdvancedFixSpellBonusDisplay();
        digitDivisor = fixSpellBonusDisplay ? 100000000 : 10000000;
        leadingZeroSkipped = 0;
        catk = &g_GameManager.catk[g_EnemyManager.spellcardInfo.spellcardIdx];
        if (!g_EnemyManager.spellcardInfo.isCapturing)
        {
            remainingBonus = 0;
        }
        this->impl->captureBonusVm.pos = this->impl->spellcardBonusIndicator.pos;
        this->impl->captureBonusVm.pos.x -= 40.0f;
        if (fixSpellBonusDisplay)
            this->impl->captureBonusVm.pos.x -= 7.0f;
        for (i = 0; i < (fixSpellBonusDisplay ? 9 : 8); i++)
        {
            digit = remainingBonus / digitDivisor;
            if (digit != 0)
            {
                leadingZeroSkipped = TRUE;
            }
            if (leadingZeroSkipped || digitDivisor == 1)
            {
                this->impl->captureBonusVm.sprite =
                    g_AnmManager->GetSprite(digit + ANM_SPRITE_ASCII_DIGITS);
                g_AnmManager->DrawNoRotation(&this->impl->captureBonusVm);
            }
            this->impl->captureBonusVm.pos.x += 7.0f;
            remainingBonus %= digitDivisor;
            digitDivisor /= 10;
        }
        digit = catk->numSuccessesPerShot[g_GameManager.shotTypeAndCharacter];
        if (99 < digit)
        {
            digit = 99;
        }
        this->impl->captureBonusVm.pos.x += 36.0f;
        if (digit / 10 != 0)
        {
            this->impl->captureBonusVm.sprite =
                g_AnmManager->GetSprite(digit / 10 + ANM_SPRITE_ASCII_DIGITS);
            g_AnmManager->DrawNoRotation(&this->impl->captureBonusVm);
        }
        this->impl->captureBonusVm.pos.x += 7.0f;
        this->impl->captureBonusVm.sprite =
            g_AnmManager->GetSprite(digit % 10 + ANM_SPRITE_ASCII_DIGITS);
        g_AnmManager->DrawNoRotation(&this->impl->captureBonusVm);

        digit = catk->numAttemptsPerShot[g_GameManager.shotTypeAndCharacter];
        if (99 < digit)
        {
            digit = 99;
        }
        this->impl->captureBonusVm.pos.x += 14.0f;
        if (digit / 10 != 0)
        {
            this->impl->captureBonusVm.sprite =
                g_AnmManager->GetSprite(digit / 10 + ANM_SPRITE_ASCII_DIGITS);
            g_AnmManager->DrawNoRotation(&this->impl->captureBonusVm);
        }
        this->impl->captureBonusVm.pos.x += 7.0f;
        this->impl->captureBonusVm.sprite =
            g_AnmManager->GetSprite(digit % 10 + ANM_SPRITE_ASCII_DIGITS);
        g_AnmManager->DrawNoRotation(&this->impl->captureBonusVm);
    }
    if (this->impl->stageClearBg.activeSpriteIdx >= 0)
    {
        g_AnmManager->DrawInterpNoRotation(&this->impl->stageClearBg);
        g_AnmManager->DrawInterpNoRotation(&this->impl->stageTransitionSnapshotVm);
        if (this->impl->loadingSprite.activeSpriteIdx >= 0)
        {
            this->impl->loadingSprite.pos = ZunVec3(304.0f, 448.0f, 0.0f);
            g_AnmManager->DrawInterpNoRotation(&this->impl->loadingSprite);
        }
    }
    if (this->impl->activeTransitionQuads != 0)
    {
        for (i = 0; i < ARRAY_SIZE_SIGNED(this->impl->transitionQuads); i++)
        {
            g_AnmManager->DrawProjected(&this->impl->transitionQuads[i]);
            g_AnmManager->SetSprite(NULL);
        }
    }

    u32 interpBossHealthBarAlpha =
        (u32)utils::Lerp(this->prevBossHealthBarAlpha, this->bossHealthBarAlpha, g_RenderAlpha);
    f32 interpBossHealthBarEased =
        utils::Lerp(this->prevBossHealthBarEased, this->bossHealthBarEased, g_RenderAlpha);
    if (this->impl->msg.currentMsgIdx < 0 && this->bossPresent + this->impl->bossHealthBarState > 0)
    {
        healthBarRect.left = 64.0f;
        healthBarRect.top = 19.0f;
        healthBarRect.right = interpBossHealthBarEased * 320.0f + 64.0f;
        healthBarRect.bottom = 23.0f;
        color1 = interpBossHealthBarAlpha << 24 | 0xffffff;
        color2 = interpBossHealthBarAlpha << 24 | 0x202060;
        timerPos.x = 48.0f;
        timerPos.y = 16.0f;
        timerPos.z = 0.0f;
        ScreenEffect::DrawColoredQuad(&healthBarRect, color1, color1, color2, color2);
        for (j = 0; j < ARRAY_SIZE_SIGNED(this->bossHealth); j++)
        {
            f32 interpBossHealthEased =
                utils::Lerp(this->prevBossHealthEased[j], this->bossHealthEased[j], g_RenderAlpha);
            if (this->bossHealth[j] == 0.0f)
            {
                continue;
            }
            if (interpBossHealthEased >= interpBossHealthBarEased)
            {
                continue;
            }
            segmentEndHealth = this->bossHealth[j];
            if (interpBossHealthBarEased < segmentEndHealth)
            {
                segmentEndHealth = interpBossHealthBarEased;
            }
            healthBarRect.left = interpBossHealthEased * 320.0f + 64.0f;
            healthBarRect.top = 19.0f;
            healthBarRect.right = segmentEndHealth * 320.0f + 64.0f;
            healthBarRect.bottom = 23.0f;
            color1 = interpBossHealthBarAlpha << 24 | (this->bossColor[j] & 0xffffff);
            color2 = interpBossHealthBarAlpha << 24 | ((i32)this->bossColor[j] >> 2 & 0x3f3f3fU);
            ScreenEffect::DrawColoredQuad(&healthBarRect, color1, color1, color2, color2);
        }
        g_AnmManager->DrawInterpNoRotation(
            &this->impl->vms0[11]); // what is responsible for drawing name
        healthBarRect.left = 33.0f;
        healthBarRect.top = 19.0f;
        healthBarRect.right = healthBarRect.left + 3.0f;
        healthBarRect.bottom = healthBarRect.top + 4.0f;

        // secondsRemaining used as bossLifeMarkers here. its reused for its
        // actual name later on
        secondsRemaining = this->bossLifeMarkers;
        markerGap = (this->bossLifeMarkers <= 5) + 1;
        for (j = 0; j < secondsRemaining; j++)
        {
            healthBarRect.left = (f32)j * 26.0f / (f32)secondsRemaining + 35.0f;
            healthBarRect.right =
                (f32)(j + 1) * 26.0f / (f32)secondsRemaining + 35.0f - (f32)markerGap;
            color1 = interpBossHealthBarAlpha << 24 | (0xffffff - j * 255 / 9);
            color2 = interpBossHealthBarAlpha << 24 | 0x202020;
            ScreenEffect::DrawColoredQuad(&healthBarRect, color1, color1, color2, color2);
        }
        timerPos = ZunVec3(384.0f, 16.0f, 0.0f);
        if (this->spellcardSecondsRemaining >= 20)
        {
            timeColor = g_SpellcardTimeColors[0];
        }
        else if (this->spellcardSecondsRemaining >= 10)
        {
            timeColor = g_SpellcardTimeColors[1];
        }
        else if (this->spellcardSecondsRemaining >= 5)
        {
            timeColor = g_SpellcardTimeColors[2];
        }
        else
        {
            timeColor = g_SpellcardTimeColors[3];
        }
        g_AsciiManager.SetColor(interpBossHealthBarAlpha << 24 | timeColor);
        secondsRemaining =
            this->spellcardSecondsRemaining > 99 ? 99 : this->spellcardSecondsRemaining;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (ShouldPlaySpellcardCountdownWarning(
                secondsRemaining, this->lastSpellcardSecondsRemaining))
#else
        if (secondsRemaining < 10 &&
            this->lastSpellcardSecondsRemaining != this->spellcardSecondsRemaining)
#endif
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_LOW_SPELL_TIME_WARNING, 0);
        }
        AsciiManager::AddFormatText(&g_AsciiManager, &timerPos, "%.2d", secondsRemaining);
        g_AsciiManager.color = 0xffffffff;
        this->lastSpellcardSecondsRemaining = this->spellcardSecondsRemaining;
    }
}

ZunResult Gui::AddedCallback(Gui *arg)
{
    return arg->ActualAddedCallback();
}

ZunResult Gui::DeletedCallback(Gui *arg)
{
    g_AnmManager->ReleaseAnm(ANM_FILE_STAGE_TEXT);
    g_AnmManager->ReleaseAnm(ANM_FILE_FACE_STAGE);
    g_AnmManager->ReleaseAnm(ANM_FILE_FACE_STAGE_1);
    g_AnmManager->ReleaseAnm(ANM_FILE_FACE_STAGE_2);
    g_AnmManager->ReleaseAnm(ANM_FILE_FACE_STAGE_3);
    arg->FreeMsgFile();
    if (IsInitialStageLoad())
    {
        g_AnmManager->ReleaseAnm(ANM_FILE_FRONT_0);
        g_AnmManager->ReleaseAnm(ANM_FILE_LOADING);
        g_AnmManager->ReleaseAnm(ANM_FILE_FACE_0);
        g_AnmManager->ReleaseAnm(ANM_FILE_FACE_1);
        g_AnmManager->ReleaseAnm(ANM_FILE_FACE_2);
        g_AnmManager->ReleaseAnm(ANM_FILE_FRONT_1);
        delete arg->impl;
        arg->impl = NULL;
    }
    return ZUN_SUCCESS;
}

ZunResult Gui::RegisterChain()
{
    Gui *mgr = &g_Gui;

    if (IsInitialStageLoad())
    {
        memset(mgr, 0, sizeof(Gui));
        mgr->impl = new GuiImpl;
    }

    g_GuiCalcChain.callback = (ChainCallback)OnUpdate;
    g_GuiCalcChain.addedCallback = NULL;
    g_GuiCalcChain.deletedCallback = NULL;
    g_GuiCalcChain.addedCallback = (ChainLifecycleCallback)AddedCallback;
    g_GuiCalcChain.deletedCallback = (ChainLifecycleCallback)DeletedCallback;
    g_GuiCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_GuiCalcChain, 13))
    {
        return ZUN_ERROR;
    }

    g_GuiDrawChain.callback = (ChainCallback)OnDraw;
    g_GuiDrawChain.addedCallback = NULL;
    g_GuiDrawChain.deletedCallback = NULL;
    g_GuiDrawChain.arg = mgr;
    g_Chain.AddToDrawChain(&g_GuiDrawChain, 12);
    return ZUN_SUCCESS;
}

GuiImpl::GuiImpl()
{
}

GuiMsgVm::GuiMsgVm()
{
}

void Gui::CutChain()
{
    g_Chain.Cut(&g_GuiCalcChain);
    g_Chain.Cut(&g_GuiDrawChain);
}
