#include "ResultScreen.hpp"
#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "BulletManager.hpp"
#include "Chain.hpp"
#include "ChainPriorities.hpp"
#include "Controller.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include "Localization.hpp"
#include "Player.hpp"
#include "PracticeRuntime.hpp"
#include "ReplayExtension.hpp"
#include "ReplayManager.hpp"
#include "Rng.hpp"
#include "SoundPlayer.hpp"
#include "Stage.hpp"
#include "Touch.hpp"
#include "ZunMath.hpp"
#include "i18n.hpp"
#include "utils.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif
// #include <direct.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef TH_DEV_TOOLS
#include <SDL3/SDL_log.h>
#endif
#include <ctime>

static const f32 g_DifficultyWeightsList[5] = {-30.0f, -10.0f, 20.0f, 30.0f, 30.0f};

static constexpr u32 g_DefaultMagic = MakeMagic('S', 'Y', 'M', 'D');

#ifdef __EMSCRIPTEN__
static bool g_NetplayEndingCycleResultAudit = false;
#endif

// EoSD assumes every character in this array is a single byte, which is a safe assumption in SJIS, but not
//   in UTF-8, so we have to encode '･' with an escape sequence
static const char *const g_AlphabetList =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ.,:;\xA5@abcdefghijklmnopqrstuvwxyz+-/*=%0123456789(){}[]<>#!?'\"$      --";

static const char *const g_CharacterList[6] = {TH_HAKUREI_REIMU_SPIRIT,  TH_HAKUREI_REIMU_DREAM,
                                               TH_KIRISAME_MARISA_DEVIL, TH_KIRISAME_MARISA_LOVE,
                                               TH_SATSUKI_RIN_FLOWER,    TH_SATSUKI_RIN_WIND};

static const f32 g_SpellcardsWeightsList[5] = {1.0f, 1.5f, 1.5f, 2.0f, 2.5f};

static const char *const g_RightAlignedDifficultyList[5] = {"     Easy", "   Normal", "     Hard", "  Lunatic",
                                                            "    Extra"};

static const char *const g_ShortCharacterList2[4] = {"ReimuA ", "ReimuB ", "MarisaA", "MarisaB"};

static const char *LocalizedStatsCharacterName(i32 index)
{
    static const char *const ids[4] = {
        "th06 Stats ReimuA",
        "th06 Stats ReimuB",
        "th06 Stats MarisaA",
        "th06 Stats MarisaB",
    };
    if (index >= 0 && index < ARRAY_SIZE_SIGNED(ids))
        return Localization::StringById(ids[index], g_CharacterList[index]);
    return g_CharacterList[index];
}

static void DrawResultShotTypeText(AnmVm *vm, const char *text)
{
    const bool localized = Localization::Active();
#ifdef TH_DEV_TOOLS
    static bool loggedLocalized = false;
    static bool loggedOriginal = false;
    bool &logged = localized ? loggedLocalized : loggedOriginal;
    if (!logged)
    {
        SDL_Log("TH06 thcrap result shot-type layout: localization=%d mode=%s text=%s",
                localized ? 1 : 0, localized ? "left" : "center", text != nullptr ? text : "(null)");
        logged = true;
    }
#endif
    if (localized)
        g_AnmManager->DrawVmTextFmt(vm, COLOR_RGB(COLOR_WHITE), COLOR_RGB(COLOR_BLACK), "%s", text);
    else
        g_AnmManager->DrawStringFormat2(vm, COLOR_RGB(COLOR_WHITE), COLOR_RGB(COLOR_BLACK), text);
}

namespace
{
bool ShouldSkipPersistentResultWrite()
{
    // Playback is read-only. A completed Replay may enter the ResultScreen
    // teardown path, but it must never update score.dat/PSCR or create a
    // Replay-of-a-Replay.
    if (g_GameManager.isInReplay)
        return true;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Multiplayer has a deliberately different lives/power/score economy.
    // Keep Replay saving available, but never merge a room result into the
    // ordinary single-player score.dat progression/high-score tables.
    return MultiplayerGameplay::IsMultiplayer();
#else
    return false;
#endif
}
} // namespace

static ResultScreenState ResolveFromGameResultState(bool directReplaySave, bool isInPracticeMode,
                                                    bool thpracActive, bool isInReplay)
{
    (void)isInPracticeMode;
    (void)thpracActive;
    (void)isInReplay;
    // Replay playback is diverted back to the Replay menu by the StageMenu
    // boundary before this patched ResultScreen path is entered. Keep this
    // helper faithful to thprac's ResultScreen patches instead of adding a
    // second Replay owner here.
    if (directReplaySave)
        return RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION;
    // th06_preplay_1 permanently changes the vanilla Practice branch's
    // immediate state from EXIT (0x11) to WRITING_HIGHSCORE_NAME (0x09).
    // The non-Practice branch already used 0x09, so after the patch every
    // natural Practice enters replay-save result flow through the same normal
    // result-state chain instead of taking the vanilla Practice exit.
    // normal from-game ResultScreen starts there. This is not mode-gated.
    return RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME;
}

#ifdef TH_DEV_TOOLS
static bool g_DebugStatsAuditRequested = false;
static bool g_DebugShotTypeAuditRequested = false;
static bool g_DebugSpellAuditRequested = false;
static ResultScreen *g_DebugStatsAuditResult = nullptr;
static int g_DebugSpellAuditId = -1;
#endif

#define DEFAULT_HIGH_SCORE_NAME "Nanashi "

static bool ValidateScoreRecords(const ScoreRaw *scoreRaw, u32 availableSize, bool requireHeaderRecord)
{
    if (scoreRaw == NULL || availableSize < sizeof(ScoreRaw) || scoreRaw->dataOffset < sizeof(ScoreRaw) ||
        scoreRaw->dataOffset > scoreRaw->fileLen || scoreRaw->fileLen > availableSize)
    {
        return false;
    }

    u32 remaining = scoreRaw->fileLen - scoreRaw->dataOffset;
    const Th6k *record = scoreRaw->ShiftBytes(scoreRaw->dataOffset);
    bool foundHeader = false;
    while (remaining != 0)
    {
        if (remaining < sizeof(Th6k) || record->th6kLen < sizeof(Th6k) || record->th6kLen > remaining)
        {
            return false;
        }
        foundHeader |= record->magic == TH6K_MAGIC;
        remaining -= record->th6kLen;
        record = record->ShiftBytes(record->th6kLen);
    }
    return !requireHeaderRecord || foundHeader;
}

#ifdef TH_DEV_TOOLS
ZunResult ResultScreen::DebugRegisterStatsAudit()
{
    g_GameManager.difficulty = NORMAL;
    g_GameManager.score = 123456789;
    g_GameManager.guiScore = 123456789;
    g_GameManager.counat = 19800;
    g_GameManager.numRetries = 2;
    g_GameManager.deaths = 1;
    g_GameManager.pointItemsCollected = 321;
    g_GameManager.grazeInTotal = 456;
    g_GameManager.isGameCompleted = 1;
    g_GameManager.isInPracticeMode = 0;
    g_GameManager.isInReplay = 0;
    g_DebugStatsAuditRequested = true;
    const ZunResult result = ResultScreen::RegisterChain(0);
    if (result != ZUN_SUCCESS)
    {
        g_DebugStatsAuditRequested = false;
        g_DebugStatsAuditResult = nullptr;
    }
    return result;
}

ZunResult ResultScreen::DebugRegisterSpellAudit()
{
    g_DebugSpellAuditRequested = true;
    g_DebugSpellAuditId = -1;
    const ZunResult result = ResultScreen::RegisterChain(0);
    if (result != ZUN_SUCCESS)
    {
        g_DebugSpellAuditRequested = false;
        g_DebugStatsAuditResult = nullptr;
    }
    return result;
}

ZunResult ResultScreen::DebugRegisterShotTypeAudit()
{
    g_DebugShotTypeAuditRequested = true;
    const ZunResult result = ResultScreen::RegisterChain(0);
    if (result != ZUN_SUCCESS)
    {
        g_DebugShotTypeAuditRequested = false;
        g_DebugStatsAuditResult = nullptr;
    }
    return result;
}

void ResultScreen::DebugCloseStatsAudit()
{
    if (g_DebugStatsAuditResult != nullptr && g_DebugStatsAuditResult->calcChain != nullptr)
        g_Chain.Cut(g_DebugStatsAuditResult->calcChain);
}

#endif

bool ResultScreen::DebugThpracResultRoutingSelfTest()
{
    return ResolveFromGameResultState(true, true, true, false) == RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION &&
           ResolveFromGameResultState(true, true, true, true) == RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION &&
           ResolveFromGameResultState(false, true, true, false) == RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME &&
           ResolveFromGameResultState(false, true, false, false) == RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME &&
           ResolveFromGameResultState(false, true, true, true) == RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME &&
           ResolveFromGameResultState(false, false, false, true) == RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME &&
           ResolveFromGameResultState(false, false, false, false) == RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME;
}

ScoreDat *ResultScreen::OpenScore(const char *path)
{
    u8 *bytes;
    i32 bytesShifted;
    i32 fileLen;
    Th6k *decryptedFilePointer;
    i32 remainingData;
    i32 scoreListNodeSize;
    u16 checksum;
    u8 xorValue;
    i32 scoreDatSize;
    ScoreRaw *scoreRaw;
    ScoreDat *scoreDat;

    scoreDat = (ScoreDat *)calloc(1, sizeof(ScoreDat));
    if (scoreDat == NULL)
    {
        return NULL;
    }
    scoreRaw = (ScoreRaw *)FileSystem::OpenPath(path, true);
    if (scoreRaw == NULL)
    {
    FAILED_TO_READ:
        scoreDatSize = sizeof(ScoreRaw);
        scoreRaw = (ScoreRaw *)std::calloc(1, scoreDatSize);
        if (scoreRaw == NULL)
        {
            free(scoreDat);
            return NULL;
        }
        scoreRaw->dataOffset = sizeof(ScoreRaw);
        scoreRaw->fileLen = sizeof(ScoreRaw);
    }
    else
    {
        if (g_LastFileSize < sizeof(ScoreRaw))
        {
            free(scoreRaw);
            goto FAILED_TO_READ;
        }

        remainingData = g_LastFileSize - 2;
        checksum = 0;
        xorValue = 0;
        bytesShifted = 0;
        bytes = &scoreRaw->xorseed[1];

        while (0 < remainingData)
        {

            xorValue += bytes[0];
            // Invert top 3 bits and bottom 5 bits
            xorValue = (xorValue & 0xe0) >> 5 | (xorValue & 0x1f) << 3;
            // xor one byte later with the resulting inverted bits
            bytes[1] ^= xorValue;
            if (bytesShifted >= 2)
            {
                checksum += bytes[1];
            }
            bytes++;
            remainingData--;
            bytesShifted++;
        }
        if (scoreRaw->csum != checksum)
        {
            free(scoreRaw);
            goto FAILED_TO_READ;
        }
        if (!ValidateScoreRecords(scoreRaw, g_LastFileSize, true))
        {
            free(scoreRaw);
            goto FAILED_TO_READ;
        }
    }

    scoreDat->rawScoreFile = scoreRaw;

    scoreListNodeSize = sizeof(ScoreListNode);
    scoreDat->scores = (ScoreListNode *)std::malloc(scoreListNodeSize);
    if (scoreDat->scores == NULL)
    {
        free(scoreRaw);
        free(scoreDat);
        return NULL;
    }
    scoreDat->scores->next = NULL;
    scoreDat->scores->data = NULL;
    scoreDat->scores->prev = NULL;
    return scoreDat;
}

u32 ResultScreen::GetHighScore(ScoreDat *scoreDat, ScoreListNode *node, u32 character, u32 difficulty)
{
    u32 score;
    u32 dataScore;
    i32 remainingSize;
    Hscr *highScore;
    ScoreRaw *scoreHeader;

    if (scoreDat == NULL || scoreDat->rawScoreFile == NULL || scoreDat->scores == NULL)
    {
        return 1000000;
    }
    scoreHeader = scoreDat->rawScoreFile;

    if (node == NULL)
    {
        ResultScreen::FreeAllScores(scoreDat->scores);
        scoreDat->scores->next = NULL;
        scoreDat->scores->data = NULL;
        scoreDat->scores->prev = NULL;
    }

    remainingSize = scoreHeader->fileLen;
    highScore = (Hscr *)scoreHeader->ShiftBytes(scoreHeader->dataOffset);
    remainingSize -= scoreHeader->dataOffset;

    while (remainingSize >= (i32)sizeof(Th6k))
    {
        if (highScore->base.th6kLen < sizeof(Th6k) || highScore->base.th6kLen > (u32)remainingSize)
        {
            break;
        }
        if (highScore->base.th6kLen >= sizeof(Hscr) && highScore->base.magic == HSCR_MAGIC &&
            highScore->base.version == TH6K_VERSION &&
            highScore->character == character && highScore->difficulty == difficulty)
        {
            if (node != NULL)
            {
                ResultScreen::LinkScore(node, highScore);
            }
            else
            {
                ResultScreen::LinkScore(scoreDat->scores, highScore);
            }
        }

        remainingSize -= highScore->base.th6kLen;
        highScore = (Hscr *)((u8 *)highScore + highScore->base.th6kLen);
    }
    if (scoreDat->scores->next != NULL)
    {
        if (scoreDat->scores->next->data->score > 1000000)
        {
            dataScore = scoreDat->scores->next->data->score;
        }
        else
        {
            dataScore = 1000000;
        }
        score = dataScore;
    }
    else
    {
        score = 1000000;
    }
    return score;
}

i32 ResultScreen::LinkScore(ScoreListNode *prevNode, Hscr *newScore)
{
    i32 scoresAmount;
    ScoreListNode *nextNode;
    i32 scoreNodeSize;

    scoresAmount = 0;
    while (prevNode->next != NULL)
    {
        if (prevNode->next->data != NULL && prevNode->next->data->score <= newScore->score)
        {
            break;
        }
        prevNode = prevNode->next;
        scoresAmount++;
    }
    nextNode = prevNode->next;
    scoreNodeSize = sizeof(ScoreListNode);

    prevNode->next = (ScoreListNode *)std::malloc(scoreNodeSize);
    if (prevNode->next == NULL)
    {
        return scoresAmount;
    }
    prevNode->next->prev = prevNode;
    prevNode = prevNode->next;
    prevNode->data = newScore;
    prevNode->next = nextNode;
    return scoresAmount;
}

void ResultScreen::FreeAllScores(ScoreListNode *scores)
{
    ScoreListNode *next;
    scores = scores->next;
    while (scores != NULL)
    {
        next = scores->next;
        free(scores);
        scores = next;
    }
}

ZunResult ResultScreen::ParseCatk(ScoreDat *scoreDat, Catk *outCatk)
{

    i32 cursor;
    Catk *parsedCatk;
    const ScoreRaw *header;
    if (scoreDat == NULL || scoreDat->rawScoreFile == NULL || outCatk == NULL)
    {
        return ZUN_ERROR;
    }
    header = scoreDat->rawScoreFile;

    parsedCatk = (Catk *)header->ShiftBytes(header->dataOffset);
    cursor = header->fileLen - header->dataOffset;
    while (cursor >= (i32)sizeof(Th6k))
    {
        if (parsedCatk->base.th6kLen < sizeof(Th6k) || parsedCatk->base.th6kLen > (u32)cursor)
            break;
        if (parsedCatk->base.th6kLen >= sizeof(Catk) && parsedCatk->base.magic == CATK_MAGIC &&
            parsedCatk->base.version == TH6K_VERSION)
        {
            if (parsedCatk->idx >= CATK_NUM_CAPTURES)
                break;

            outCatk[parsedCatk->idx] = *parsedCatk;
        }
        cursor -= parsedCatk->base.th6kLen;
        parsedCatk = (Catk *)parsedCatk->base.ShiftBytes(parsedCatk->base.th6kLen);
    }
    return ZUN_SUCCESS;
}

ZunResult ResultScreen::ParseClrd(ScoreDat *scoreDat, Clrd *outClrd)
{
    i32 cursor;
    Clrd *parsedClrd;
    const ScoreRaw *header;
    i32 characterShotType;
    i32 difficulty;
    if (scoreDat == NULL || scoreDat->rawScoreFile == NULL || outClrd == NULL)
    {
        return ZUN_ERROR;
    }
    header = scoreDat->rawScoreFile;

    for (characterShotType = 0; characterShotType < CLRD_NUM_CHARACTERS; characterShotType++)
    {
        memset(&outClrd[characterShotType], 0, sizeof(Clrd));

        outClrd[characterShotType].base.magic = CLRD_MAGIC;
        outClrd[characterShotType].base.unkLen = sizeof(Clrd);
        outClrd[characterShotType].base.th6kLen = sizeof(Clrd);
        outClrd[characterShotType].base.version = TH6K_VERSION;
        outClrd[characterShotType].characterShotType = characterShotType;

        for (difficulty = 0; difficulty < ARRAY_SIZE_SIGNED(outClrd[0].difficultyClearedWithoutRetries); difficulty++)
        {
            outClrd[characterShotType].difficultyClearedWithRetries[difficulty] = 1;
            outClrd[characterShotType].difficultyClearedWithoutRetries[difficulty] = 1;
        }
    }

    parsedClrd = (Clrd *)header->ShiftBytes(header->dataOffset);
    cursor = header->fileLen - header->dataOffset;
    while (cursor >= (i32)sizeof(Th6k))
    {
        if (parsedClrd->base.th6kLen < sizeof(Th6k) || parsedClrd->base.th6kLen > (u32)cursor)
            break;
        if (parsedClrd->base.th6kLen >= sizeof(Clrd) && parsedClrd->base.magic == CLRD_MAGIC &&
            parsedClrd->base.version == TH6K_VERSION)
        {
            if (parsedClrd->characterShotType >= CLRD_NUM_CHARACTERS)
                break;

            outClrd[parsedClrd->characterShotType] = *parsedClrd;
        }
        cursor -= parsedClrd->base.th6kLen;
        parsedClrd = (Clrd *)(((u8 *)&parsedClrd->base) + parsedClrd->base.th6kLen);
    }
    return ZUN_SUCCESS;
}

ZunResult ResultScreen::ParsePscr(ScoreDat *scoreDat, Pscr *outClrd)
{
    i32 cursor;
    Pscr *parsedPscr;
    const ScoreRaw *header;
    i32 stage;
    i32 character;
    i32 difficulty;
    Pscr *pscr;

    if (scoreDat == NULL || scoreDat->rawScoreFile == NULL || outClrd == NULL)
    {
        return ZUN_ERROR;
    }
    header = scoreDat->rawScoreFile;

    for (pscr = outClrd, character = 0; character < PSCR_NUM_CHARS_SHOTTYPES; character++)
    {
        for (stage = 0; stage < PSCR_NUM_STAGES; stage++)
        {
            for (difficulty = 0; difficulty < PSCR_NUM_DIFFICULTIES; difficulty++, pscr++)
            {

                std::memset(pscr, 0, sizeof(Pscr));

                pscr->base.magic = PSCR_MAGIC;
                pscr->base.unkLen = sizeof(Pscr);
                pscr->base.th6kLen = sizeof(Pscr);
                pscr->base.version = 16;
                pscr->character = character;
                pscr->difficulty = difficulty;
                pscr->stage = stage;
            }
        }
    }

    parsedPscr = (Pscr *)header->ShiftBytes(header->dataOffset);
    cursor = header->fileLen - header->dataOffset;

    while (cursor >= (i32)sizeof(Th6k))
    {
        if (parsedPscr->base.th6kLen < sizeof(Th6k) || parsedPscr->base.th6kLen > (u32)cursor)
            break;
        if (parsedPscr->base.th6kLen >= sizeof(Pscr) && parsedPscr->base.magic == PSCR_MAGIC &&
            parsedPscr->base.version == TH6K_VERSION)
        {
            pscr = parsedPscr;
            if (pscr->character >= PSCR_NUM_CHARS_SHOTTYPES || pscr->difficulty >= PSCR_NUM_DIFFICULTIES + 1 ||
                pscr->stage >= PSCR_NUM_STAGES + 1)
                break;

            outClrd[pscr->character * 6 * 4 + pscr->stage * 4 + pscr->difficulty] = *pscr;
        }
        cursor -= parsedPscr->base.th6kLen;
        parsedPscr = (Pscr *)((u8 *)parsedPscr + parsedPscr->base.th6kLen);
    }
    return ZUN_SUCCESS;
}

void ResultScreen::ReleaseScoreDat(ScoreDat *scoreDat)
{
    if (scoreDat == NULL)
        return;
    ScoreListNode *scores;
    if (scoreDat->scores != NULL)
        ResultScreen::FreeAllScores(scoreDat->scores);
    scores = scoreDat->scores;
    free(scores);
    free(scoreDat->rawScoreFile);
    free(scoreDat);
}

void ResultScreen::WriteScore(ResultScreen *resultScreen)
{
    if (ShouldSkipPersistentResultWrite())
        return;

    u8 *fileBuffer;
    u8 originalByte;
    i32 fileBufferSize;
    ScoreRaw *scoreRaw;
    i32 characterSlot;
    u8 xorValue;
    i32 remainingSize;
    i32 shotType;
    i32 stage;
    const Pscr *pscr;
    Catk *catk;
    Clrd *clrd;
    i32 character;
    const ScoreListNode *currentCharacter;
    i32 sizeOfFile;
    u8 *bytes;
    i32 difficulty;

    sizeOfFile = 0;

    fileBufferSize = SCORE_DAT_FILE_BUFFER_SIZE;
    fileBuffer = (u8 *)malloc(fileBufferSize);
    if (fileBuffer == NULL || resultScreen == NULL || resultScreen->scoreDat == NULL ||
        resultScreen->scoreDat->rawScoreFile == NULL)
    {
        free(fileBuffer);
        return;
    }

    std::memcpy(fileBuffer + sizeOfFile, resultScreen->scoreDat->rawScoreFile, sizeof(ScoreRaw));

    sizeOfFile += sizeof(ScoreRaw);
    resultScreen->fileHeader.magic = TH6K_MAGIC;
    resultScreen->fileHeader.unkLen = sizeof(Th6k);
    resultScreen->fileHeader.th6kLen = sizeof(Th6k);
    resultScreen->fileHeader.version = TH6K_VERSION;

    std::memcpy(fileBuffer + sizeOfFile, &resultScreen->fileHeader, sizeof(Th6k));
    sizeOfFile += sizeof(Th6k);

    for (difficulty = 0; difficulty < HSCR_NUM_DIFFICULTIES; difficulty++)
    {

        for (character = 0; character < HSCR_NUM_CHARS_SHOTTYPES; character++)
        {
            currentCharacter = resultScreen->scores[difficulty][character].next;
            characterSlot = 0;
            for (;;)
            {
                if (currentCharacter != NULL)
                {

                    if (currentCharacter->data->base.magic == HSCR_MAGIC)
                    {
                        currentCharacter->data->character = character;
                        currentCharacter->data->difficulty = difficulty;
                        currentCharacter->data->base.unkLen = sizeof(Hscr);
                        currentCharacter->data->base.th6kLen = sizeof(Hscr);
                        currentCharacter->data->base.version = TH6K_VERSION;
                        currentCharacter->data->base.unk_9 = 0;
                        std::memcpy(fileBuffer + sizeOfFile, currentCharacter->data, sizeof(Hscr));
                        sizeOfFile += sizeof(Hscr);
                    }
                    currentCharacter = currentCharacter->next;
                    characterSlot++;

                    if (characterSlot >= HSCR_NUM_SCORES_SLOTS)
                    {
                        break;
                    }
                    else
                    {
                        continue;
                    }
                };
                break;
            };
        }
    };

    clrd = g_GameManager.clrd;
    for (difficulty = 0; difficulty < CLRD_NUM_CHARACTERS; difficulty++, clrd++)
    {
        clrd->base.magic = CLRD_MAGIC;
        clrd->base.unkLen = sizeof(Clrd);
        clrd->base.th6kLen = sizeof(Clrd);
        clrd->base.version = TH6K_VERSION;
        std::memcpy(fileBuffer + sizeOfFile, clrd, sizeof(Clrd));

        sizeOfFile += sizeof(Clrd);
    }
    catk = &g_GameManager.catk[0];
    for (difficulty = 0; difficulty < CATK_NUM_CAPTURES; difficulty++, catk++)
    {
        if (catk->base.magic == CATK_MAGIC)
        {
            catk->idx = difficulty;
            catk->base.unkLen = sizeof(Catk);
            catk->base.th6kLen = sizeof(Catk);
            catk->base.version = TH6K_VERSION;
            std::memcpy(fileBuffer + sizeOfFile, catk, sizeof(Catk));
            sizeOfFile += sizeof(Catk);
        }
    }
    pscr = &g_GameManager.pscr[0][0][0];
    for (difficulty = 0; difficulty < PSCR_NUM_DIFFICULTIES; difficulty++)
    {
        for (stage = 0; stage < PSCR_NUM_STAGES; stage++)
        {
            for (shotType = 0; shotType < PSCR_NUM_CHARS_SHOTTYPES; shotType++, pscr++)
            {
                if (pscr->score != 0)
                {
                    std::memcpy(fileBuffer + sizeOfFile, pscr, sizeof(Pscr));
                    sizeOfFile += sizeof(Pscr);
                }
            }
        }
    }
    scoreRaw = (ScoreRaw *)fileBuffer;
    scoreRaw->dataOffset = sizeof(Pscr);
    scoreRaw->fileLen = sizeOfFile;
    scoreRaw->csum = 0;

    scoreRaw->xorseed[1] = g_Rng.GetRandomU16InRange(0x100);
    scoreRaw->unk[0] = g_Rng.GetRandomU16InRange(0x100);
    scoreRaw->unk_8 = 0x10;

    for (remainingSize = 4; remainingSize < sizeOfFile; remainingSize++)
    {
        scoreRaw->csum += fileBuffer[remainingSize];
    }
    xorValue = 0;
    originalByte = 0;

    bytes = (u8 *)scoreRaw->ShiftOneByte();
    remainingSize = sizeOfFile;

    remainingSize -= 2;
    xorValue = bytes[0];

    while (remainingSize > 0)
    {
        originalByte = bytes[1];
        xorValue = (xorValue & 0xe0) >> 5 | (xorValue & 0x1f) << 3;
        bytes[1] ^= xorValue;
        xorValue += originalByte;
        bytes++;
        remainingSize--;
    }
    FileSystem::WriteDataToFile("score.dat", fileBuffer, sizeOfFile);
    std::free(fileBuffer);
}

i32 ResultScreen::LinkScoreEx(Hscr *out, i32 difficulty, i32 character)
{
    return ResultScreen::LinkScore(&this->scores[difficulty][character], out);
}

void ResultScreen::FreeScore(i32 difficulty, i32 character)
{
    ResultScreen::FreeAllScores(&this->scores[difficulty][character]);
}

i32 ResultScreen::HandleResultKeyboard()
{
    i32 idx;
    AnmVm *sprite;
    i32 replayNameIdx;
    i32 replayNameIdx2;

    if (this->frameTimer == 0)
    {
        this->charUsed = g_GameManager.character;
        this->diffSelected = g_GameManager.difficulty;

        sprite = &this->unk_40[0];
        for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
        {
            sprite->pendingInterrupt = this->diffSelected + 3;
        }

        DrawResultShotTypeText(this->unk_28a0, LocalizedStatsCharacterName(this->charUsed * 2));
        if (g_GameManager.shotType != SHOT_TYPE_A)
        {
            this->unk_28a0[0].color = COLOR_TRANSPARENT_WHITE;
        }

        DrawResultShotTypeText(&this->unk_28a0[1], LocalizedStatsCharacterName(this->charUsed * 2));
        if (g_GameManager.shotType != SHOT_TYPE_B)
        {
            this->unk_28a0[1].color = COLOR_TRANSPARENT_WHITE;
        }

        this->hscr.character = this->charUsed * 2 + g_GameManager.shotType;
        this->hscr.difficulty = this->diffSelected;
        this->hscr.score = g_GameManager.score;
        this->hscr.base.version = 16;
        this->hscr.base.magic = *(i32 *)"HSCR";

        if (g_GameManager.isGameCompleted == 0)
        {
            this->hscr.stage = g_GameManager.currentStage;
        }
        else
        {
            this->hscr.stage = 99;
        }

        this->hscr.base.unk_9 = 1;
        std::strcpy(this->hscr.name, "        ");

        if (this->LinkScoreEx(&this->hscr, this->diffSelected, this->charUsed * 2 + g_GameManager.shotType) >= 10)
            goto RETURN_TO_STATS_SCREEN_WITHOUT_SOUND;

        this->cursor = 0;
        std::strcpy(this->replayName, "");
    }
    if (this->frameTimer < 30)
    {
        return 0;
    }
    if (WAS_PRESSED_PERIODIC(TH_BUTTON_UP))
    {
        for (;;)
        {
            this->selectedCharacter -= RESULT_KEYBOARD_COLUMNS;

            if (this->selectedCharacter < 0)
            {
                this->selectedCharacter += RESULT_KEYBOARD_CHARACTERS;
            }

            if (g_AlphabetList[this->selectedCharacter] == ' ')
            {
                continue;
            }
            break;
        };
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
    }
    if (WAS_PRESSED_PERIODIC(TH_BUTTON_DOWN))
    {
        for (;;)
        {
            this->selectedCharacter += RESULT_KEYBOARD_COLUMNS;

            if (this->selectedCharacter >= RESULT_KEYBOARD_CHARACTERS)
            {
                this->selectedCharacter -= RESULT_KEYBOARD_CHARACTERS;
            }

            if (g_AlphabetList[this->selectedCharacter] == ' ')
            {
                continue;
            }
            break;
        };
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
    }
    if (WAS_PRESSED_PERIODIC(TH_BUTTON_LEFT))
    {
        for (;;)
        {
            this->selectedCharacter--;
            if (this->selectedCharacter % RESULT_KEYBOARD_COLUMNS == RESULT_KEYBOARD_COLUMNS - 1)
            {
                this->selectedCharacter += RESULT_KEYBOARD_COLUMNS;
            }

            if (this->selectedCharacter < 0)
            {
                this->selectedCharacter = RESULT_KEYBOARD_COLUMNS - 1;
            }

            if (g_AlphabetList[this->selectedCharacter] == ' ')
            {
                continue;
            }
            break;
        };
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
    }
    if (WAS_PRESSED_PERIODIC(TH_BUTTON_RIGHT))
    {
        for (;;)
        {
            this->selectedCharacter++;

            if (this->selectedCharacter % RESULT_KEYBOARD_COLUMNS == 0)
            {
                this->selectedCharacter -= RESULT_KEYBOARD_COLUMNS;
            }

            if (g_AlphabetList[this->selectedCharacter] == ' ')
            {
                continue;
            }
            break;
        };
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
    }
    if (WAS_PRESSED_PERIODIC(TH_BUTTON_SELECTMENU))
    {
        replayNameIdx = this->cursor >= 8 ? 7 : this->cursor;

        if (this->selectedCharacter < RESULT_KEYBOARD_SPACE)
        {
            this->hscr.name[replayNameIdx] = g_AlphabetList[this->selectedCharacter];
        }
        else if (this->selectedCharacter == RESULT_KEYBOARD_SPACE)
        {
            this->hscr.name[replayNameIdx] = ' ';
        }
        else
        {
            goto RETURN_TO_STATS_SCREEN;
        }

        if (this->cursor < 8)
        {
            this->cursor++;
            if (this->cursor == 8)
            {
                this->selectedCharacter = RESULT_KEYBOARD_END;
            }
        }
        g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
    }

    if (WAS_PRESSED_PERIODIC(TH_BUTTON_RETURNMENU))
    {
        replayNameIdx2 = this->cursor >= 8 ? 7 : this->cursor;

        if (this->cursor > 0)
        {
            this->cursor--;
            this->hscr.name[replayNameIdx2] = ' ';
        }
        g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
    }
    if (WAS_PRESSED(TH_BUTTON_MENU))
    {
    RETURN_TO_STATS_SCREEN:
        g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);

    RETURN_TO_STATS_SCREEN_WITHOUT_SOUND:

        this->resultScreenState = RESULT_SCREEN_STATE_STATS_SCREEN;
        this->frameTimer = 0;

        sprite = &this->unk_40[0];
        for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
        {
            sprite->pendingInterrupt = 2;
        }
        std::strcpy(this->replayName, this->hscr.name);
    }
    return 0;
}

i32 ResultScreen::HandleReplaySaveKeyboard()
{
    AnmVm *sprite;
    i32 replayNameCharacter2;
    char replayPath[64];
    i32 replayNameCharacter;
    char replayToReadPath[64];
    ReplayHeader *replayLoaded;
    i32 idx;
    i32 saveInterrupt;
    std::time_t time;
    const std::tm *tm;

    time = std::time(NULL);
    tm = std::localtime(&time);

    switch (this->resultScreenState)
    {
    case RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION:
        if (this->frameTimer == 60)
        {
            if (g_GameManager.numRetries != 0)
            {
                saveInterrupt = 0xc;
            }
            else
            {
                if (g_Supervisor.framerateMultiplier < 0.99f)
                {
                    saveInterrupt = 0xd;
                }
                else
                {
                    saveInterrupt = 9;
                }
            }
            sprite = &this->unk_40[1];
            for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
            {
                sprite->pendingInterrupt = saveInterrupt;
            }
            if (saveInterrupt != 9)
            {
                this->resultScreenState = RESULT_SCREEN_STATE_CANT_SAVE_REPLAY;
            }
            this->cursor = 0;
        }
        sprite = &this->unk_40[16];
        if (this->cursor == 0)
        {
            sprite[0].color = COLOR_COMBINE_ALPHA(COLOR_PASTEL_RED, sprite[0].color);
            sprite[1].color = COLOR_COMBINE_ALPHA(COLOR_ASHEN_GREY, sprite[1].color);
        }
        else
        {
            sprite[0].color = COLOR_COMBINE_ALPHA(COLOR_ASHEN_GREY, sprite[0].color);
            sprite[1].color = COLOR_COMBINE_ALPHA(COLOR_PASTEL_RED, sprite[1].color);
        }
        if (this->frameTimer < 80)
        {
            return 0;
        }
        ResultScreen::MoveCursorHorizontally(this, 2);
        if (WAS_PRESSED(TH_BUTTON_RETURNMENU) || WAS_PRESSED(TH_BUTTON_MENU))
        {
            goto EXIT_WITH_SOUND;
        }
        if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {

            if (this->cursor == 0)
            {
            GO_TO_CHOOSE_REPLAY_FILE:

                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
                this->resultScreenState = RESULT_SCREEN_STATE_CHOOSING_REPLAY_FILE;

                sprite = &this->unk_40[0];
                for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
                {
                    sprite->pendingInterrupt = 0xa;
                }

                this->frameTimer = 0;
                goto CHOOSE_REPLAY_FILE;
            }

        EXIT_WITH_SOUND:

            this->frameTimer = 0;
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
            this->resultScreenState = RESULT_SCREEN_STATE_EXITING;
            sprite = &this->unk_40[0];
            for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
            {
                sprite->pendingInterrupt = 2;
            }
        }
        break;
    case RESULT_SCREEN_STATE_CANT_SAVE_REPLAY:

        if (this->frameTimer < 20)
        {
            return 0;
        }

        if (WAS_PRESSED(TH_BUTTON_SELECTMENU) || WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {

            this->frameTimer = 0;
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
            this->resultScreenState = RESULT_SCREEN_STATE_EXITING;
            sprite = &this->unk_40[0];
            for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
            {
                sprite->pendingInterrupt = 2;
            }
        }
        break;

    case RESULT_SCREEN_STATE_CHOOSING_REPLAY_FILE:

    CHOOSE_REPLAY_FILE:

        if (this->frameTimer == 0)
        {
            FileSystem::CreateDir("./replay");

            for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->replays); idx++)
            {
                std::sprintf(replayToReadPath, "./replay/th6_%.2d.rpy", idx + 1);
                replayLoaded = (ReplayHeader *)FileSystem::OpenPath(replayToReadPath, 1);
                if (replayLoaded == NULL)
                {
                    std::sprintf(replayToReadPath, "./replay/th6_%.2d.rpyx", idx + 1);
                    replayLoaded = (ReplayHeader *)FileSystem::OpenPath(replayToReadPath, 1);
                    if (replayLoaded == NULL)
                    {
                        continue;
                    }
                }

                if (ReplayExtension::MatchesPath(replayToReadPath, reinterpret_cast<const u8 *>(replayLoaded), g_LastFileSize) &&
                    ReplayManager::ValidateReplayData(replayLoaded, g_LastFileSize) == ZUN_SUCCESS)
                {
                    this->replays[idx] = *replayLoaded;
                }
                std::free(replayLoaded);
            }
        }

        if (this->frameTimer < 20)
        {
            return 0;
        }

        MoveCursor(this, 15);
        this->replayNumber = this->cursor;
        if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
            this->replayNumber = this->cursor;
            this->frameTimer = 0;
            sprintf(this->defaultReplay.date, "%02i/%02i/%02i", tm->tm_mon, tm->tm_mday, tm->tm_year % 100);
            (this->defaultReplay).score = g_GameManager.score;
            if (*(i32 *)&this->replays[this->cursor].magic != *(i32 *)&"T6RP" ||
                this->replays[this->cursor].version != GAME_VERSION)
            {
                sprite = &this->unk_40[0];
                for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
                {
                    sprite->pendingInterrupt = 0xf;
                }
                sprite = &this->unk_40[this->replayNumber + 0x16];
                sprite->pendingInterrupt = 0xe;
                this->resultScreenState = RESULT_SCREEN_STATE_WRITING_REPLAY_NAME;
            }
            else
            {
                sprite = &this->unk_40[0];
                for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
                {
                    sprite->pendingInterrupt = 0xb;
                }
                sprite = &this->unk_40[this->replayNumber + 0x16];
                sprite->pendingInterrupt = 0xe;
                this->resultScreenState = RESULT_SCREEN_STATE_OVERWRITE_REPLAY_FILE;
            }
            this->cursor = 0;
            this->selectedCharacter = 0;
        }
        if (WAS_PRESSED(10))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
            this->resultScreenState = RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION;
            sprite = &this->unk_40[0];
            for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
            {
                sprite->pendingInterrupt = 2;
            }
            this->frameTimer = 0;
        }
        break;
    case RESULT_SCREEN_STATE_WRITING_REPLAY_NAME:
        if (this->frameTimer < 30)
        {
            return 0;
        }
        if (WAS_PRESSED_PERIODIC(TH_BUTTON_UP))
        {
            for (;;)
            {
                this->selectedCharacter -= RESULT_KEYBOARD_COLUMNS;

                if (this->selectedCharacter < 0)
                {
                    this->selectedCharacter += RESULT_KEYBOARD_CHARACTERS;
                }

                if (g_AlphabetList[this->selectedCharacter] == ' ')
                {
                    continue;
                }
                break;
            };
            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        }
        if (WAS_PRESSED_PERIODIC(TH_BUTTON_DOWN))
        {
            for (;;)
            {
                this->selectedCharacter += RESULT_KEYBOARD_COLUMNS;

                if (this->selectedCharacter >= RESULT_KEYBOARD_CHARACTERS)
                {
                    this->selectedCharacter -= RESULT_KEYBOARD_CHARACTERS;
                }

                if (g_AlphabetList[this->selectedCharacter] == ' ')
                {
                    continue;
                }
                break;
            };
            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        }
        if (WAS_PRESSED_PERIODIC(TH_BUTTON_LEFT))
        {
            for (;;)
            {
                this->selectedCharacter--;
                if (this->selectedCharacter % RESULT_KEYBOARD_COLUMNS == RESULT_KEYBOARD_COLUMNS - 1)
                {
                    this->selectedCharacter += RESULT_KEYBOARD_COLUMNS;
                }

                if (this->selectedCharacter < 0)
                {
                    this->selectedCharacter = RESULT_KEYBOARD_COLUMNS - 1;
                }

                if (g_AlphabetList[this->selectedCharacter] == ' ')
                {
                    continue;
                }
                break;
            };
            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        }
        if (WAS_PRESSED_PERIODIC(TH_BUTTON_RIGHT))
        {
            for (;;)
            {
                this->selectedCharacter++;
                if (this->selectedCharacter % RESULT_KEYBOARD_COLUMNS == 0)
                {
                    this->selectedCharacter -= RESULT_KEYBOARD_COLUMNS;
                }

                if (g_AlphabetList[this->selectedCharacter] == ' ')
                {
                    continue;
                }
                break;
            };
            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        }
        if (WAS_PRESSED_PERIODIC(TH_BUTTON_SELECTMENU))
        {

            replayNameCharacter = this->cursor >= 8 ? 7 : this->cursor;

            if (this->selectedCharacter < RESULT_KEYBOARD_SPACE)
            {
                this->replayName[replayNameCharacter] = g_AlphabetList[this->selectedCharacter];
            }
            else if (this->selectedCharacter == RESULT_KEYBOARD_SPACE)
            {
                this->replayName[replayNameCharacter] = ' ';
            }
            else
            {
                std::sprintf(replayPath, "./replay/th6_%.2d.rpy", this->replayNumber + 1);
                ReplayManager::SaveReplay(replayPath, this->replayName);
                this->frameTimer = 0;
                this->resultScreenState = RESULT_SCREEN_STATE_EXITING;
                sprite = &this->unk_40[0];
                for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
                {
                    sprite->pendingInterrupt = 2;
                }
            }
            if (this->cursor < 8)
            {
                this->cursor++;
                if (this->cursor == 8)
                {
                    this->selectedCharacter = RESULT_KEYBOARD_END;
                }
            }
            g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
        }

        if (WAS_PRESSED_PERIODIC(TH_BUTTON_RETURNMENU))
        {
            replayNameCharacter2 = this->cursor >= 8 ? 7 : this->cursor;

            if (this->cursor > 0)
            {
                this->cursor--;
                this->replayName[replayNameCharacter2] = ' ';
            }
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
        }
        if (WAS_PRESSED(TH_BUTTON_MENU))
        {
            goto GO_TO_CHOOSE_REPLAY_FILE;
        }
        break;

    case RESULT_SCREEN_STATE_OVERWRITE_REPLAY_FILE:
        sprite = &this->unk_40[16];
        if (this->cursor == 0)
        {
            sprite[0].color = COLOR_COMBINE_ALPHA(COLOR_PASTEL_RED, sprite[0].color);
            sprite[1].color = COLOR_COMBINE_ALPHA(COLOR_ASHEN_GREY, sprite[1].color);
        }
        else
        {
            sprite[0].color = COLOR_COMBINE_ALPHA(COLOR_ASHEN_GREY, sprite[0].color);
            sprite[1].color = COLOR_COMBINE_ALPHA(COLOR_PASTEL_RED, sprite[1].color);
        }

        if (this->frameTimer < 20)
        {
            return 0;
        }
        MoveCursorHorizontally(this, 2);

        if (WAS_PRESSED(TH_BUTTON_RETURNMENU) || WAS_PRESSED(TH_BUTTON_MENU))
        {
            goto GO_TO_CHOOSE_REPLAY_FILE;
        }

        if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {

            this->frameTimer = 0;
            if (this->cursor == 0)
            {
                sprite = &this->unk_40[0];
                for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->unk_40); idx++, sprite++)
                {
                    sprite->pendingInterrupt = 15;
                }
                sprite = &this->unk_40[this->replayNumber + 22];
                sprite->pendingInterrupt = 14;
                this->resultScreenState = RESULT_SCREEN_STATE_WRITING_REPLAY_NAME;
                break;
            }
            goto GO_TO_CHOOSE_REPLAY_FILE;
        }
    }
    return 0;
}

void ResultScreen::MoveCursor(ResultScreen *resultScreen, i32 length)
{
    if (WAS_PRESSED_PERIODIC(TH_BUTTON_UP))
    {
        resultScreen->cursor--;
        if (resultScreen->cursor < 0)
        {
            resultScreen->cursor += length;
        }
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
    }
    if (WAS_PRESSED_PERIODIC(TH_BUTTON_DOWN))
    {
        resultScreen->cursor++;
        if (resultScreen->cursor >= length)
        {
            resultScreen->cursor -= length;
        }
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
    }
}

bool ResultScreen::MoveCursorHorizontally(ResultScreen *resultScreen, i32 length)
{
    if (WAS_PRESSED_PERIODIC(TH_BUTTON_LEFT))
    {
        resultScreen->cursor--;
        if (resultScreen->cursor < 0)
        {
            resultScreen->cursor += length;
        }
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        return true;
    }
    else if (WAS_PRESSED_PERIODIC(TH_BUTTON_RIGHT))
    {
        resultScreen->cursor++;
        if (resultScreen->cursor >= length)
        {
            resultScreen->cursor -= length;
        }
        g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        return true;
    }
    else
    {
        return false;
    }
}

ZunResult ResultScreen::CheckConfirmButton()
{
    AnmVm *viewport;

    switch (this->resultScreenState)
    {
    case RESULT_SCREEN_STATE_STATS_SCREEN:
        if (this->frameTimer <= 30)
        {
            viewport = &this->unk_40[37];
            viewport->pendingInterrupt = 16;
        }
        if (this->frameTimer >= 90 && WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            viewport = &this->unk_40[37];
            viewport->pendingInterrupt = 2;
            this->frameTimer = 0;
            this->resultScreenState = RESULT_SCREEN_STATE_STATS_TO_SAVE_TRANSITION;
        }
        break;

    case RESULT_SCREEN_STATE_STATS_TO_SAVE_TRANSITION:
        if (this->frameTimer >= 30)
        {
            this->frameTimer = 59;
            this->resultScreenState = RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION;
        }
        break;
    }
    return ZUN_SUCCESS;
}

u32 ResultScreen::DrawFinalStats() const
{
    f32 completion;
    f32 unknownFloat;
    ZunVec3 strPos;
    const AnmVm *viewport;
    i32 color;
    f32 slowdownRate;

    switch (this->resultScreenState)
    {
    case RESULT_SCREEN_STATE_STATS_SCREEN:
    case RESULT_SCREEN_STATE_STATS_TO_SAVE_TRANSITION:

        viewport = &this->unk_40[37];
        color = viewport->color;
        g_AsciiManager.color = color;
        unknownFloat = 0.0;

        completion = g_GameManager.difficulty < 4 ? g_GameManager.counat / 89500.0f : g_GameManager.counat / 39600.0f;
        strPos = viewport->pos;
        strPos.x += 224.0f;
        strPos.y += 32.0f;
        g_AsciiManager.AddFormatText(&strPos, "%9d", g_GameManager.score);

        if (g_GameManager.guiScore < 2000000)
        {
            unknownFloat -= 20.0f;
        }
        else if (g_GameManager.guiScore < 200000000)
        {
            unknownFloat += (g_GameManager.guiScore - 2000000) / 198000000.0f * 60.0f - 20.0f;
        }
        else
        {
            unknownFloat += 40.0f;
        }

        strPos.y += 22.0f;
        // base_tsa/th06.v1.02h.js::result_rank_format rewrites this exact
        // AddString call into the variadic ASCII printer so ascii_vpatchf_th06
        // can replace the right-aligned duplicate with the regular rank text.
        g_AsciiManager.AddFormatText(&strPos, g_RightAlignedDifficultyList[g_GameManager.difficulty]);

        unknownFloat += g_DifficultyWeightsList[g_GameManager.difficulty];
        strPos.y += 22.0f;
        if (g_GameManager.difficulty == EASY || !g_GameManager.isGameCompleted)
        {
            g_AsciiManager.AddFormatText(&strPos, "    %3.2f%%", completion * 100.0f);
            unknownFloat += completion * 70.0f;
        }
        else
        {
            g_AsciiManager.AddFormatText(&strPos, "      100%%");
            unknownFloat += 70.0f;
        }
        strPos.y += 22.0f;
        g_AsciiManager.AddFormatText(&strPos, "%9d", g_GameManager.numRetries);

        unknownFloat -= g_GameManager.numRetries * 10.0f;
        strPos.y += 22.0f;

        g_AsciiManager.AddFormatText(&strPos, "%9d", g_GameManager.deaths);

        unknownFloat -= g_GameManager.deaths * 5.0f - 10.0f;

        strPos.y += 22.0f;

        g_AsciiManager.AddFormatText(&strPos, "%9d", g_GameManager.bombsUsed);

        unknownFloat -= g_GameManager.bombsUsed * 2.0f - 10.0f;
        strPos.y += 22.0f;

        g_AsciiManager.AddFormatText(&strPos, "%9d", g_GameManager.spellcardsCaptured);

        unknownFloat += g_GameManager.spellcardsCaptured * g_SpellcardsWeightsList[g_GameManager.difficulty];

        slowdownRate = (g_Supervisor.unk1b4 / g_Supervisor.unk1b8 - 0.5f) * 2;

        if (slowdownRate < 0.0f)
        {
            slowdownRate = 0.0f;
        }
        else if (slowdownRate >= 1.0f)
        {
            slowdownRate = 1.0f;
        }

        slowdownRate = Touch::UsedCheatMovementThisRun() ? 100.0f : (1 - slowdownRate) * 100.0f;

        strPos.y += 22.0f;
        g_AsciiManager.AddFormatText(&strPos, "    %3.2f%%", slowdownRate);

        if (slowdownRate < 50.0f)
        {
            unknownFloat -= 70.0f * slowdownRate / 100.0f;
        }
        else
        {
            unknownFloat = -999.0f;
        }
        // Useless calculations, maybe in earlier versions it showed the point items and graze, but it was later
        // removed? unknowFloat is also unused, maybe it was some kind of grading system
        if (g_GameManager.pointItemsCollected < 800)
        {
            unknownFloat += 0.01f * g_GameManager.pointItemsCollected;
        }
        else
        {
            unknownFloat += 8.0f;
        }

        if (g_GameManager.grazeInTotal < 5000)
        {
            unknownFloat += 0.0025f * g_GameManager.grazeInTotal;
        }
        else
        {
            unknownFloat += 12.5f;
        }

        g_AsciiManager.color = COLOR_WHITE;
    }
    return 0;
}

ZunResult ResultScreen::RegisterChain(i32 unk)
{

    i32 unused[16];
    ResultScreen *resultScreen;
    resultScreen = new ResultScreen();

    utils::DebugPrint(TH_DBG_RESULTSCREEN_COUNAT, g_GameManager.counat);

    resultScreen->calcChain = g_Chain.CreateElem((ChainCallback)ResultScreen::OnUpdate);
    resultScreen->calcChain->addedCallback = (ChainAddedCallback)ResultScreen::AddedCallback;
    resultScreen->calcChain->deletedCallback = (ChainDeletedCallback)ResultScreen::DeletedCallback;
    resultScreen->calcChain->arg = resultScreen;

    if (unk != 0)
    {
        const bool directReplaySave = PracticeRuntime::ConsumeResultReplaySaveRequest();
        resultScreen->resultScreenState = ResolveFromGameResultState(
            directReplaySave, g_GameManager.isInPracticeMode != 0,
            PracticeRuntime::Active(), g_GameManager.isInReplay != 0);

#if defined(__EMSCRIPTEN__) && defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
        if (MultiplayerGameplay::IsMultiplayer())
        {
            g_NetplayEndingCycleResultAudit = EM_ASM_INT({
                return Module.eaglerOptions?.netplayEndingCycle ? 1 : 0;
            }) != 0;
            EM_ASM({
                globalThis.__eaglerNetplayResultEntered = true;
                globalThis.__eaglerNetplayResultCompleted = false;
                globalThis.__eaglerNetplayResultState = $0;
            }, static_cast<int>(resultScreen->resultScreenState));
        }
#endif

        if (directReplaySave)
        {
            // Upstream th06_result_screen_create is enabled only by the
            // advanced-practice Pause->Exit path and disables itself after one
            // invocation.  This must still run before the vanilla Practice
            // flag check because Extra clears that flag.
            std::memset(resultScreen->replayName, ' ', sizeof(resultScreen->replayName));
#ifdef TH_DEV_TOOLS
            SDL_Log("TH06 thprac result: save replay question");
#endif
        }
        else if (resultScreen->resultScreenState == RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME &&
                 g_GameManager.isInPracticeMode)
        {
            // thprac v2.3.0.3's th06_preplay_1 is a permanent one-byte patch
            // at the vanilla Practice branch of ResultScreen::RegisterChain:
            // it changes the initial state from EXIT (0x11) to
            // WRITING_HIGHSCORE_NAME (0x09).  That normal result flow then
            // reaches Stats -> SAVE_REPLAY_QUESTION, allowing a naturally
            // finished/missed Practice run to be saved as a replay.  This is
            // separate from the one-shot Pause->Exit hook above, which jumps
            // directly to SAVE_REPLAY_QUESTION (and is also needed for Extra,
            // where thprac deliberately clears isInPracticeMode).
#ifdef TH_DEV_TOOLS
            SDL_Log("TH06 thprac result: natural Practice enters replay-save result flow");
#endif
        }
    }

#ifdef TH_DEV_TOOLS
    if (g_DebugStatsAuditRequested)
    {
        resultScreen->resultScreenState = RESULT_SCREEN_STATE_STATS_SCREEN;
        resultScreen->frameTimer = 0;
        g_DebugStatsAuditResult = resultScreen;
        g_DebugStatsAuditRequested = false;
        SDL_Log("TH06 thcrap result stats audit: registered read-only stats chain");
    }
    else if (g_DebugShotTypeAuditRequested)
    {
        resultScreen->resultScreenState = RESULT_SCREEN_STATE_BEST_SCORES_EASY;
        resultScreen->diffSelected = EASY;
        resultScreen->cursor = 1;
        resultScreen->charUsed = 0;
        resultScreen->frameTimer = 20;
        g_DebugStatsAuditResult = resultScreen;
        g_DebugShotTypeAuditRequested = false;
        SDL_Log("TH06 thcrap result shot-type audit: registered read-only best-scores chain");
    }
    else if (g_DebugSpellAuditRequested)
    {
        resultScreen->resultScreenState = RESULT_SCREEN_STATE_SPELLCARDS;
        resultScreen->lastResultScreenState = RESULT_SCREEN_STATE_SPELLCARDS;
        resultScreen->previousCursor = 0;
        resultScreen->cursor = 0;
        resultScreen->lastSpellcardSelected = -1;
        resultScreen->frameTimer = 0;
        g_DebugStatsAuditResult = resultScreen;
        g_DebugSpellAuditRequested = false;
        SDL_Log("TH06 thcrap result spell audit: registered read-only spell-card chain");
    }
#endif

    if (g_Chain.AddToCalcChain(resultScreen->calcChain, TH_CHAIN_PRIO_CALC_RESULTSCREEN))
    {
        return ZUN_ERROR;
    }

    resultScreen->drawChain = g_Chain.CreateElem((ChainCallback)ResultScreen::OnDraw);
    resultScreen->drawChain->arg = resultScreen;
    g_Chain.AddToDrawChain(resultScreen->drawChain, TH_CHAIN_PRIO_DRAW_RESULTSCREEN);

    return ZUN_SUCCESS;
}

ResultScreen::ResultScreen()
{
    i32 unused[12];
    std::memset(this, 0, sizeof(ResultScreen));
    this->cursor = 1;
}

ChainCallbackResult ResultScreen::OnUpdate(ResultScreen *resultScreen)
{
    i32 difficulty;
    i32 characterShotType;
#ifdef __EMSCRIPTEN__
    if (g_NetplayEndingCycleResultAudit)
    {
        EM_ASM({ globalThis.__eaglerNetplayResultState = $0; },
               static_cast<int>(resultScreen->resultScreenState));
    }
#endif
    AnmVm *vm;
    i32 i;
    for (AnmVm &vm : resultScreen->unk_40)
    {
        vm.UpdatePrev();
    }
    for (AnmVm &vm : resultScreen->unk_28a0)
    {
        vm.UpdatePrev();
    }
    resultScreen->unk_39a0.UpdatePrev();
    switch (resultScreen->resultScreenState)
    {

    case RESULT_SCREEN_STATE_EXIT:
        g_Supervisor.curState = SUPERVISOR_STATE_MAINMENU;
        return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;

    case RESULT_SCREEN_STATE_INIT:

        if (resultScreen->frameTimer == 0)
        {

            vm = &resultScreen->unk_40[0];
            for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, vm++)
            {
                vm->pendingInterrupt = 1;
                vm->flags.colorOp = 1;
                if (((g_Supervisor.cfg.opts >> GCOS_USE_D3D_HW_TEXTURE_BLENDING) & 1) == 0)
                {
                    vm->color &= COLOR_BLACK;
                }
                else
                {
                    vm->color &= COLOR_WHITE;
                }
            }

            vm = &resultScreen->unk_40[1];
            for (i = 0; i <= 6; i++, vm++)
            {
                if (i == resultScreen->cursor)
                {
                    if (((g_Supervisor.cfg.opts >> GCOS_USE_D3D_HW_TEXTURE_BLENDING) & 1) == 0)
                    {
                        vm->color = COLOR_DARK_GREY;
                    }
                    else
                    {
                        vm->color = COLOR_WHITE;
                    }

                    vm->posOffset = ZunVec3(-4.0f, -4.0f, 0.0f);
                }
                else
                {
                    if (((g_Supervisor.cfg.opts >> GCOS_USE_D3D_HW_TEXTURE_BLENDING) & 1) == 0)
                    {
                        vm->color = COLOR_SET_ALPHA(COLOR_BLACK, 176);
                    }
                    else
                    {
                        vm->color = COLOR_SET_ALPHA(COLOR_WHITE, 176);
                    }
                    vm->posOffset = ZunVec3(0.0f, 0.0f, 0.0f);
                }
            }
        }

        if (resultScreen->frameTimer < 20)
        {
            break;
        }

        resultScreen->resultScreenState++;
        resultScreen->frameTimer = 0;

    case RESULT_SCREEN_STATE_CHOOSING_DIFFICULTY:

        ResultScreen::MoveCursor(resultScreen, 7);

        vm = &resultScreen->unk_40[1];
        for (i = 0; i <= 6; i++, vm++)
        {
            if (i == resultScreen->cursor)
            {
                if (((g_Supervisor.cfg.opts >> GCOS_USE_D3D_HW_TEXTURE_BLENDING) & 1) == 0)
                {
                    vm->color = COLOR_DARK_GREY;
                }
                else
                {
                    vm->color = COLOR_WHITE;
                }
                vm->posOffset = ZunVec3(-4.0f, -4.0f, 0.0f);
            }
            else
            {
                if (((g_Supervisor.cfg.opts >> GCOS_USE_D3D_HW_TEXTURE_BLENDING) & 1) == 0)
                {
                    vm->color = COLOR_SET_ALPHA(COLOR_BLACK, 176);
                }
                else
                {
                    vm->color = COLOR_SET_ALPHA(COLOR_WHITE, 176);
                }
                vm->posOffset = ZunVec3(0.0f, 0.0f, 0.0f);
            }
        }

        if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            vm = &resultScreen->unk_40[0];
            switch (resultScreen->cursor)
            {
            case RESULT_SCREEN_CURSOR_EASY:
            case RESULT_SCREEN_CURSOR_NORMAL:
            case RESULT_SCREEN_CURSOR_HARD:
            case RESULT_SCREEN_CURSOR_LUNATIC:
            case RESULT_SCREEN_CURSOR_EXTRA:
                for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, vm++)
                {
                    vm->pendingInterrupt = resultScreen->cursor + 3;
                }
                resultScreen->diffSelected = resultScreen->cursor;

                resultScreen->resultScreenState = resultScreen->cursor + RESULT_SCREEN_STATE_BEST_SCORES_EASY;
                resultScreen->lastResultScreenState = resultScreen->resultScreenState;
                resultScreen->frameTimer = 0;
                resultScreen->cursor = resultScreen->lastBestScoresCursor;
                resultScreen->charUsed = -1;
                resultScreen->lastSpellcardSelected = -1;
                break;

            case RESULT_SCREEN_CURSOR_SPELLCARDS:
                for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, vm++)
                {
                    vm->pendingInterrupt = resultScreen->cursor + 3;
                }
                resultScreen->diffSelected = resultScreen->cursor;
                resultScreen->resultScreenState = RESULT_SCREEN_STATE_SPELLCARDS;
                resultScreen->lastResultScreenState = resultScreen->resultScreenState;
                resultScreen->frameTimer = 0;
                resultScreen->charUsed = -1;
                resultScreen->cursor = resultScreen->previousCursor;
                resultScreen->lastSpellcardSelected = -1;
                break;

            case RESULT_SCREEN_CURSOR_EXIT:
                for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, vm++)
                {
                    vm->pendingInterrupt = 2;
                }
                resultScreen->resultScreenState = RESULT_SCREEN_STATE_EXITING;
                g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
            }
        }
        if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {
            resultScreen->cursor = RESULT_SCREEN_CURSOR_EXIT;
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
        }
        break;

    case RESULT_SCREEN_STATE_EXITING:

        if (resultScreen->frameTimer < 60)
        {
            break;
        }
        else
        {
            g_Supervisor.curState = SUPERVISOR_STATE_MAINMENU;
            return CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB;
        }

    case RESULT_SCREEN_STATE_BEST_SCORES_EXTRA:

        if (IS_PRESSED(TH_BUTTON_FOCUS) || IS_PRESSED(TH_BUTTON_SKIP))
        {

            if (resultScreen->cheatCodeStep < 5)
            {
                if (WAS_PRESSED(TH_BUTTON_HOME))
                {
                    resultScreen->cheatCodeStep++;
                }
                else if (WAS_PRESSED(TH_BUTTON_WRONG_CHEATCODE))
                {
                    resultScreen->cheatCodeStep = 0;
                }
            }
            else if (resultScreen->cheatCodeStep < 7)
            {
                if (WAS_PRESSED(TH_BUTTON_Q))
                {

                    resultScreen->cheatCodeStep++;
                }
                else if (WAS_PRESSED(TH_BUTTON_WRONG_CHEATCODE))
                {
                    resultScreen->cheatCodeStep = 0;
                }
            }
            else if (resultScreen->cheatCodeStep < 10)
            {
                if (WAS_PRESSED(TH_BUTTON_S))
                {
                    resultScreen->cheatCodeStep++;
                }
                else if (WAS_PRESSED(TH_BUTTON_WRONG_CHEATCODE))
                {
                    resultScreen->cheatCodeStep = 0;
                }
            }
            else
            {
                for (characterShotType = 0; characterShotType < HSCR_NUM_CHARS_SHOTTYPES; characterShotType++)
                {
                    for (difficulty = 0; difficulty < HSCR_NUM_DIFFICULTIES; difficulty++)
                    {
                        g_GameManager.clrd[characterShotType].difficultyClearedWithRetries[difficulty] = 99;
                        g_GameManager.clrd[characterShotType].difficultyClearedWithoutRetries[difficulty] = 99;
                    }
                }
                resultScreen->cheatCodeStep = 0;
                g_SoundPlayer.PlaySoundByIdx(SOUND_1UP);
            }
        }
        else
        {
            resultScreen->cheatCodeStep = 0;
        }
    case RESULT_SCREEN_STATE_BEST_SCORES_EASY:
    case RESULT_SCREEN_STATE_BEST_SCORES_NORMAL:
    case RESULT_SCREEN_STATE_BEST_SCORES_HARD:
    case RESULT_SCREEN_STATE_BEST_SCORES_LUNATIC:

        if (resultScreen->charUsed != resultScreen->cursor && resultScreen->frameTimer == 20)
        {
            resultScreen->charUsed = resultScreen->cursor;
            DrawResultShotTypeText(&resultScreen->unk_28a0[0],
                                   LocalizedStatsCharacterName(resultScreen->charUsed * 2));
            DrawResultShotTypeText(&resultScreen->unk_28a0[1],
                                   LocalizedStatsCharacterName(resultScreen->charUsed * 2 + 1));
        }
        if (resultScreen->frameTimer < 30)
        {
            break;
        }
        if (ResultScreen::MoveCursorHorizontally(resultScreen, 2))
        {
            resultScreen->frameTimer = 0;
            vm = &resultScreen->unk_40[0];
            for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, vm++)
            {
                vm->pendingInterrupt = resultScreen->diffSelected + 3;
            }
        }
        if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
            resultScreen->resultScreenState = RESULT_SCREEN_STATE_INIT;
            resultScreen->frameTimer = 1;
            vm = &resultScreen->unk_40[0];
            for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, vm++)
            {
                vm->pendingInterrupt = 1;
            }
            resultScreen->lastBestScoresCursor = resultScreen->cursor;
            resultScreen->cursor = resultScreen->diffSelected;
        }

        break;

    case RESULT_SCREEN_STATE_SPELLCARDS:

        if (resultScreen->lastSpellcardSelected != resultScreen->cursor && resultScreen->frameTimer == 20)
        {

            resultScreen->lastSpellcardSelected = resultScreen->cursor;
            for (i = resultScreen->lastSpellcardSelected * 10; i < resultScreen->lastSpellcardSelected * 10 + 10; i++)
            {
                if (i >= ARRAY_SIZE_SIGNED(g_GameManager.catk))
                {
                    break;
                }
                if (g_GameManager.catk[i].numAttempts == 0)
                {
                    g_AnmManager->DrawVmTextFmt(&resultScreen->unk_28a0[i % 10], COLOR_RGB(COLOR_WHITE),
                                                COLOR_RGB(COLOR_BLACK), TH_UNKNOWN_SPELLCARD);
                }
                else
                {
                    const char *spellName = Localization::SpellName(static_cast<u32>(i), g_GameManager.catk[i].name);
#ifdef TH_DEV_TOOLS
                    static bool loggedLocalizedSpellDisplay = false;
                    static bool loggedOriginalSpellDisplay = false;
                    bool &loggedSpellDisplay = spellName != g_GameManager.catk[i].name
                                                   ? loggedLocalizedSpellDisplay
                                                   : loggedOriginalSpellDisplay;
                    if (!loggedSpellDisplay)
                    {
                        SDL_Log("TH06 result spell display: localization=%d id=%d original=%s displayed=%s",
                                Localization::Active() ? 1 : 0, i, g_GameManager.catk[i].name, spellName);
                        loggedSpellDisplay = true;
                    }
#endif
                    if (Localization::Active())
                        g_AnmManager->DrawVmTextFmt(&resultScreen->unk_28a0[i % 10], COLOR_RGB(COLOR_WHITE),
                                                    COLOR_RGB(COLOR_BLACK), "%s", spellName);
                    else
                        g_AnmManager->DrawVmTextFmt(&resultScreen->unk_28a0[i % 10], COLOR_RGB(COLOR_WHITE),
                                                    COLOR_RGB(COLOR_BLACK), spellName);
                }
            }
        }
        if (resultScreen->frameTimer < 30)
        {
            break;
        }
        if (ResultScreen::MoveCursorHorizontally(resultScreen, 7))
        {
            resultScreen->frameTimer = 0;
            vm = &resultScreen->unk_40[0];
            for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, vm++)
            {
                vm->pendingInterrupt = resultScreen->diffSelected + 3;
            }
        }
        if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
            resultScreen->resultScreenState = RESULT_SCREEN_STATE_INIT;
            resultScreen->frameTimer = 1;
            vm = &resultScreen->unk_40[0];
            for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, vm++)
            {
                vm->pendingInterrupt = 1;
            }
            resultScreen->previousCursor = resultScreen->cursor;
            resultScreen->cursor = resultScreen->diffSelected;
        }
        break;

    case RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME:
        resultScreen->HandleResultKeyboard();
        break;

    case RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION:
    case RESULT_SCREEN_STATE_CANT_SAVE_REPLAY:
    case RESULT_SCREEN_STATE_CHOOSING_REPLAY_FILE:
    case RESULT_SCREEN_STATE_WRITING_REPLAY_NAME:
    case RESULT_SCREEN_STATE_OVERWRITE_REPLAY_FILE:
        resultScreen->HandleReplaySaveKeyboard();
        break;

    case RESULT_SCREEN_STATE_STATS_SCREEN:
    case RESULT_SCREEN_STATE_STATS_TO_SAVE_TRANSITION:
        resultScreen->CheckConfirmButton();
        break;
    };

    vm = &resultScreen->unk_40[0];
    for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, vm++)
    {
        g_AnmManager->ExecuteScript(vm);
    }
    resultScreen->frameTimer++;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult ResultScreen::OnDraw(ResultScreen *resultScreen)
{
    AnmVm *sprite;
    char keyboardCharacter[2];
    ZunVec2 charPos;

    i32 spellcardIdx;
    ZunVec3 spritePos;
    const ScoreListNode *ShootScoreListNodeB;
    i32 column;
    i32 row;
    const ScoreListNode *ShootScoreListNodeA;

    char name[9];

    ZunVec3 strPos;

    sprite = &resultScreen->unk_40[0];
    g_Supervisor.viewport.x = 0;
    g_Supervisor.viewport.y = 0;
    g_Supervisor.viewport.width = GAME_WINDOW_WIDTH;
    g_Supervisor.viewport.height = GAME_WINDOW_HEIGHT;

    //    g_Supervisor.d3dDevice->SetViewport(&g_Supervisor.viewport);
    g_AnmManager->SetProjectionMode(PROJECTION_MODE_PERSPECTIVE);
    g_Supervisor.viewport.Set();
    g_AnmManager->CopySurfaceToBackBuffer(0, 0, 0, 0, 0);

    for (row = 0; row < ARRAY_SIZE_SIGNED(resultScreen->unk_40); row++, sprite++)
    {
        spritePos = sprite->pos;
        sprite->pos = sprite->prevPos.Lerp(sprite->pos, g_RenderAlpha) + sprite->posOffset;
        g_AnmManager->DrawNoRotation(sprite);
        sprite->pos = spritePos;
    }
    sprite = &resultScreen->unk_40[14];
    if (sprite->pos.x < 640.0f)
    {
        if (resultScreen->lastResultScreenState != 8)
        {
            // These text VMs are positioned relative to the moving result
            // panel. Their positions are derived render state, so interpolating
            // their independently stored prevPos makes a newly visible label
            // fly in from the VM's old/default origin. Interpolate the owning
            // panel once and derive every child from that authoritative result.
            spritePos = sprite->prevPos.Lerp(sprite->pos, g_RenderAlpha) + sprite->posOffset;
            AnmVm characterNameA = resultScreen->unk_28a0[0];
            characterNameA.pos = spritePos;
            g_AnmManager->DrawNoRotation(&characterNameA);

            spritePos.x += 320.0f;

            AnmVm characterNameB = resultScreen->unk_28a0[1];
            characterNameB.pos = spritePos;
            g_AnmManager->DrawNoRotation(&characterNameB);

            spritePos.x -= 320.0f;
            spritePos.y += 36.0f;

            ShootScoreListNodeA = resultScreen->scores[resultScreen->diffSelected][resultScreen->charUsed * 2].next;
            ShootScoreListNodeB = resultScreen->scores[resultScreen->diffSelected][resultScreen->charUsed * 2 + 1].next;
            for (row = 0; row < 10; row++)
            {
                if (resultScreen->resultScreenState == RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME)
                {
                    if (g_GameManager.shotType == SHOT_TYPE_A)
                    {
                        if (ShootScoreListNodeA->data->base.unk_9 != 0)
                        {
                            g_AsciiManager.color = 0xfff0f0ff;

                            std::strcpy(name, "        ");
                            name[8] = 0;

                            name[resultScreen->cursor >= 8 ? 7 : resultScreen->cursor] = '_';
                            g_AsciiManager.AddFormatText(&spritePos, "   %8s", &name);
                        }
                        else
                        {
                            g_AsciiManager.color = 0x80ffffc0;
                        }
                    }
                    else
                    {
                        g_AsciiManager.color = 0x80ffc0c0;
                    }
                }
                else
                {
                    g_AsciiManager.color = 0xffffc0c0;
                }
                g_AsciiManager.AddFormatText(&spritePos, "%2d", row + 1);

                spritePos.x += 36.0f;
                if (ShootScoreListNodeA->data->stage <= 6)
                {
                    g_AsciiManager.AddFormatText(&spritePos, "%8s %9d(%d)", ShootScoreListNodeA->data->name,
                                                 ShootScoreListNodeA->data->score, ShootScoreListNodeA->data->stage);
                }
                else if (ShootScoreListNodeA->data->stage == 7)
                {
                    g_AsciiManager.AddFormatText(&spritePos, "%8s %9d(1)", ShootScoreListNodeA->data->name,
                                                 ShootScoreListNodeA->data->score);
                }
                else
                {
                    g_AsciiManager.AddFormatText(&spritePos, "%8s %9d(C)", ShootScoreListNodeA->data->name,
                                                 ShootScoreListNodeA->data->score);
                }
                spritePos.x += 300.0f;
                if (resultScreen->resultScreenState == RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME)
                {
                    if (g_GameManager.shotType == SHOT_TYPE_B)
                    {
                        if (ShootScoreListNodeB->data->base.unk_9 != 0)
                        {
                            g_AsciiManager.color = 0xfffff0f0;

                            std::strcpy(name, "        ");
                            name[8] = 0;

                            name[resultScreen->cursor >= 8 ? 7 : resultScreen->cursor] = '_';
                            g_AsciiManager.AddFormatText(&spritePos, "%8s", &name);
                        }
                        else
                        {
                            g_AsciiManager.color = 0xc0c0c0ff;
                        }
                    }
                    else
                    {
                        g_AsciiManager.color = 0x80c0c0ff;
                    }
                }
                else
                {
                    g_AsciiManager.color = 0xffc0c0ff;
                }
                if (ShootScoreListNodeB->data->stage <= 6)
                {
                    g_AsciiManager.AddFormatText(&spritePos, "%8s %9d(%d)", ShootScoreListNodeB->data->name,
                                                 ShootScoreListNodeB->data->score, ShootScoreListNodeB->data->stage);
                }
                else if (ShootScoreListNodeB->data->stage == 7)
                {
                    g_AsciiManager.AddFormatText(&spritePos, "%8s %9d(1)", ShootScoreListNodeB->data->name,
                                                 ShootScoreListNodeB->data->score);
                }
                else
                {
                    g_AsciiManager.AddFormatText(&spritePos, "%8s %9d(C)", ShootScoreListNodeB->data->name,
                                                 ShootScoreListNodeB->data->score);
                }

                spritePos.x -= 336.0f;
                spritePos.y += 18.0f;
                ShootScoreListNodeA = ShootScoreListNodeA->next;
                ShootScoreListNodeB = ShootScoreListNodeB->next;
            }
        }
        else
        {
            spritePos = sprite->prevPos.Lerp(sprite->pos, g_RenderAlpha) + sprite->posOffset;
            spritePos.y += 16.0f;

            for (row = 0; row < 10; row++)
            {
                spellcardIdx = resultScreen->lastSpellcardSelected * 10 + row;
                if (spellcardIdx >= ARRAY_SIZE_SIGNED(g_GameManager.catk))
                {
                    break;
                }

                if (g_GameManager.catk[spellcardIdx].numAttempts == 0)
                {
                    g_AsciiManager.color = 0x80c0c0ff;
                }
                else if (g_GameManager.catk[spellcardIdx].numSuccess == 0)
                {
                    g_AsciiManager.color = 0xffc0a0a0;
                }
                else
                {
                    g_AsciiManager.color = 0xfff0f0ff - row * 0x80800;
                }
                g_AsciiManager.AddFormatText(&spritePos, "No.%.2d", spellcardIdx + 1);

                AnmVm spellcardName = resultScreen->unk_28a0[row];
                spellcardName.pos = spritePos;
                spellcardName.pos.x += 96.0f;
                g_AnmManager->DrawNoRotation(&spellcardName);

                if (Localization::Active())
                {
                    // base_tsa/th06 result_spell_cap_pos_1 saves the original
                    // X, changes the capture counter offset from 368 to 472,
                    // and result_spell_cap_pos_2 restores that exact saved X.
                    const f32 originalX = spritePos.x;
                    spritePos.x += 472.0f;
#ifdef TH_DEV_TOOLS
                    static bool loggedLocalizedCapturePos = false;
                    if (!loggedLocalizedCapturePos)
                    {
                        SDL_Log("TH06 thcrap result spell capture position: localization=1 base=%.3f capture=%.3f",
                                static_cast<double>(originalX), static_cast<double>(spritePos.x));
                        loggedLocalizedCapturePos = true;
                    }
#endif
                    g_AsciiManager.AddFormatText(&spritePos, "%3d/%3d", g_GameManager.catk[spellcardIdx].numSuccess,
                                                 g_GameManager.catk[spellcardIdx].numAttempts);
                    spritePos.x = originalX;
                }
                else
                {
                    const f32 originalX = spritePos.x;
                    spritePos.x += 368.0f;
#ifdef TH_DEV_TOOLS
                    static bool loggedOriginalCapturePos = false;
                    if (!loggedOriginalCapturePos)
                    {
                        SDL_Log("TH06 thcrap result spell capture position: localization=0 base=%.3f capture=%.3f",
                                static_cast<double>(originalX), static_cast<double>(spritePos.x));
                        loggedOriginalCapturePos = true;
                    }
#endif
                    g_AsciiManager.AddFormatText(&spritePos, "%3d/%3d", g_GameManager.catk[spellcardIdx].numSuccess,
                                                 g_GameManager.catk[spellcardIdx].numAttempts);
                    spritePos.x -= 368.0f;
                }
                spritePos.y += 30.0f;
            }
        }
    }
    if (resultScreen->resultScreenState == RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME ||
        resultScreen->resultScreenState == RESULT_SCREEN_STATE_WRITING_REPLAY_NAME)
    {
        spritePos = ZunVec3(160.0f, 356.0f, 0.0f);

        for (row = 0; row < RESULT_KEYBOARD_ROWS; row++)
        {
            for (column = 0; column < RESULT_KEYBOARD_COLUMNS; column++)
            {
                charPos.y = 0.0f;
                charPos.x = 0.0f;
                if (resultScreen->selectedCharacter == row * RESULT_KEYBOARD_COLUMNS + column)
                {
                    g_AsciiManager.color = COLOR_KEYBOARD_KEY_HIGHLIGHT;
                    if (resultScreen->frameTimer % 64 < 32)
                    {
                        charPos.y = 1.2f + 0.8f * (resultScreen->frameTimer % 0x20) / 32.0f;
                    }
                    else
                    {
                        charPos.y = 2.0f - 0.8f * (resultScreen->frameTimer % 0x20) / 32.0f;
                    }
                    g_AsciiManager.scale.x = charPos.y;
                    g_AsciiManager.scale.y = charPos.y;
                    charPos.y = -(charPos.y - 1.0f) * 8.0f;
                    charPos.x = charPos.y;
                }
                else
                {
                    g_AsciiManager.color = COLOR_KEYBOARD_KEY_NORMAL;
                    g_AsciiManager.scale.x = 1.0f;
                    g_AsciiManager.scale.y = 1.0f;
                }
                strPos = spritePos;
                strPos.x += charPos.y;
                strPos.y += charPos.x;
                keyboardCharacter[0] = g_AlphabetList[row * RESULT_KEYBOARD_COLUMNS + column];
                keyboardCharacter[1] = '\0';

                if (row == 5)
                {
                    if (column == 14)
                    {
                        keyboardCharacter[0] = 0x80; // SP
                    }
                    else if (column == 15)
                    {
                        keyboardCharacter[0] = 0x81; // END
                    }
                }

                g_AsciiManager.AddString(&strPos, keyboardCharacter);

                spritePos.x += 20.0f;
            }
            spritePos.x -= column * 20;
            spritePos.y += 18.0f;
        }
    }
    g_AsciiManager.scale.x = 1.0;
    g_AsciiManager.scale.y = 1.0;
    if ((resultScreen->resultScreenState >= RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION) &&
        (resultScreen->resultScreenState <= RESULT_SCREEN_STATE_OVERWRITE_REPLAY_FILE))
    {
        sprite = &resultScreen->unk_40[15];
        for (row = 0; row < 6; row++, sprite++)
        {
            g_AnmManager->DrawInterpNoRotation(sprite);
        }
        sprite = &resultScreen->unk_40[21];
        spritePos = sprite->pos;
        sprite++;
        g_AsciiManager.AddFormatText(&spritePos, "No.   Name     Date     Player Score");
        for (row = 0; row < ARRAY_SIZE_SIGNED(resultScreen->replays); row++)
        {
            spritePos = sprite->pos;
            sprite++;
            if (row == resultScreen->replayNumber)
            {
                g_AsciiManager.color = COLOR_LIGHT_RED;
            }
            else
            {
                g_AsciiManager.color = COLOR_GREY;
            }
            if (resultScreen->resultScreenState == RESULT_SCREEN_STATE_WRITING_REPLAY_NAME)
            {
                g_AsciiManager.AddFormatText(&spritePos, "No.%.2d %8s %8s %7s %9d", row + 1, &resultScreen->replayName,
                                             resultScreen->defaultReplay.date,
                                             g_ShortCharacterList2[g_GameManager.CharacterShotType()],
                                             resultScreen->defaultReplay.score);
                g_AsciiManager.color = 0xfff0f0ff;

                std::strcpy(name, "        ");

                name[8] = 0;

                name[resultScreen->cursor >= 8 ? 7 : resultScreen->cursor] = '_';
                g_AsciiManager.AddFormatText(&spritePos, "      %8s", &name);
            }
            else if (*(i32 *)&resultScreen->replays[row].magic != *(i32 *)"T6RP" ||
                     resultScreen->replays[row].version != GAME_VERSION)
            {
                g_AsciiManager.AddFormatText(&spritePos, "No.%.2d -------- --/--/-- -------         0", row + 1);
            }
            else
            {
                g_AsciiManager.AddFormatText(&spritePos, "No.%.2d %8s %8s %7s %9d", row + 1,
                                             resultScreen->replays[row].name, resultScreen->replays[row].date,
                                             g_ShortCharacterList2[resultScreen->replays[row].shottypeChara],
                                             resultScreen->replays[row].score);
            }
        }
    }
    g_AsciiManager.color = COLOR_WHITE;
    resultScreen->DrawFinalStats();

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult ResultScreen::AddedCallback(ResultScreen *resultScreen)
{

    i32 slot;
    i32 characterShot;
    AnmVm *sprite;
    i32 i;

    if (resultScreen->resultScreenState != RESULT_SCREEN_STATE_EXIT)
    {

        if (g_AnmManager->LoadSurface(0, "data/result/result.jpg") != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }

        if (g_AnmManager->LoadAnm(ANM_FILE_RESULT00, "data/result00.anm", ANM_OFFSET_RESULT00) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }

        if (g_AnmManager->LoadAnm(ANM_FILE_RESULT01, "data/result01.anm", ANM_OFFSET_RESULT01) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }

        if (g_AnmManager->LoadAnm(ANM_FILE_RESULT02, "data/result02.anm", ANM_OFFSET_RESULT02) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }

        if (g_AnmManager->LoadAnm(ANM_FILE_RESULT03, "data/result03.anm", ANM_OFFSET_RESULT03) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }

        sprite = &resultScreen->unk_40[0];
        for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_40); i++, sprite++)
        {

            sprite->pos = ZunVec3(0.0f, 0.0f, 0.0f);
            sprite->posOffset = ZunVec3(0.0f, 0.0f, 0.0f);

            // Execute all the scripts from the start of result00 to the end of result02
            g_AnmManager->SetAndExecuteScriptIdx(sprite, ANM_SCRIPT_RESULT00_START + i);
        }

        sprite = &resultScreen->unk_28a0[0];
        for (i = 0; i < ARRAY_SIZE_SIGNED(resultScreen->unk_28a0); i++, sprite++)
        {
            g_AnmManager->InitializeAndSetSprite(sprite, ANM_SCRIPT_TEXT_RESULTSCREEN_CHARACTER_NAME + i);

            sprite->pos = ZunVec3(0.0f, 0.0f, 0.0f);

            sprite->flags.anchor = AnmVmAnchor_TopLeft;

            sprite->fontWidth = 15;
            sprite->fontHeight = 15;
        }
    }

    for (i = 0; i < HSCR_NUM_DIFFICULTIES; i++)
    {
        for (characterShot = 0; characterShot < HSCR_NUM_CHARS_SHOTTYPES; characterShot++)
        {
            for (slot = 0; slot < HSCR_NUM_SCORES_SLOTS; slot++)
            {
                resultScreen->defaultScore[i][characterShot][slot].score = 1000000 - slot * 100000;
                resultScreen->defaultScore[i][characterShot][slot].base.magic = g_DefaultMagic;
                resultScreen->defaultScore[i][characterShot][slot].difficulty = i;
                resultScreen->defaultScore[i][characterShot][slot].base.version = TH6K_VERSION;
                resultScreen->defaultScore[i][characterShot][slot].base.unkLen = sizeof(Hscr);
                resultScreen->defaultScore[i][characterShot][slot].base.th6kLen = sizeof(Hscr);
                resultScreen->defaultScore[i][characterShot][slot].stage = 1;
                resultScreen->defaultScore[i][characterShot][slot].base.unk_9 = 0;

                resultScreen->LinkScoreEx(resultScreen->defaultScore[i][characterShot] + slot, i, characterShot);

                std::strcpy(resultScreen->defaultScore[i][characterShot][slot].name, DEFAULT_HIGH_SCORE_NAME);
            }
        }
    }

    resultScreen->lastBestScoresCursor = 0;
    resultScreen->scoreDat = ResultScreen::OpenScore("score.dat");

    for (i = 0; i < HSCR_NUM_DIFFICULTIES; i++)
    {
        for (characterShot = 0; characterShot < HSCR_NUM_CHARS_SHOTTYPES; characterShot++)
        {
            ResultScreen::GetHighScore(resultScreen->scoreDat, &resultScreen->scores[i][characterShot], characterShot,
                                       i);
        }
    }

    if (resultScreen->resultScreenState != RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME &&
        resultScreen->resultScreenState != RESULT_SCREEN_STATE_EXIT)
    {
        ParseCatk(resultScreen->scoreDat, g_GameManager.catk);
        ParseClrd(resultScreen->scoreDat, g_GameManager.clrd);
        ParsePscr(resultScreen->scoreDat, (Pscr *)g_GameManager.pscr);
    }
#ifdef TH_DEV_TOOLS
    if (resultScreen == g_DebugStatsAuditResult &&
        resultScreen->resultScreenState == RESULT_SCREEN_STATE_SPELLCARDS)
    {
        static const char auditFallback[] = "TH06_RESULT_SPELL_AUDIT";
        for (int spellId = 0; spellId < CATK_NUM_CAPTURES; ++spellId)
        {
            const char *localized = Localization::SpellName(static_cast<u32>(spellId), auditFallback);
            if (localized != auditFallback)
            {
                Catk &catk = g_GameManager.catk[spellId];
                std::memset(&catk, 0, sizeof(catk));
                catk.idx = static_cast<u16>(spellId);
                catk.numAttempts = 1;
                catk.numSuccess = 1;
                std::strncpy(catk.name, auditFallback, sizeof(catk.name) - 1);
                g_DebugSpellAuditId = spellId;
                resultScreen->cursor = spellId / 10;
                resultScreen->lastSpellcardSelected = -1;
                SDL_Log("TH06 thcrap result spell audit: fixture id=%d localized=%s", spellId, localized);
                break;
            }
        }
        if (g_DebugSpellAuditId < 0)
        {
            Catk &catk = g_GameManager.catk[0];
            std::memset(&catk, 0, sizeof(catk));
            catk.idx = 0;
            catk.numAttempts = 1;
            catk.numSuccess = 1;
            std::strncpy(catk.name, auditFallback, sizeof(catk.name) - 1);
            g_DebugSpellAuditId = 0;
            resultScreen->cursor = 0;
            resultScreen->lastSpellcardSelected = -1;
            SDL_Log("TH06 result spell audit: no translated record; using fallback fixture id=0 localization=%d",
                    Localization::Active() ? 1 : 0);
        }
        // AddedCallback initializes every result ANM VM after RegisterChain's
        // state selection, which clears any earlier pendingInterrupt. Apply
        // the exact real Spell Cards transition interrupt here, after that
        // initialization and score parsing have finished.
        for (AnmVm &vm : resultScreen->unk_40)
            vm.pendingInterrupt = RESULT_SCREEN_CURSOR_SPELLCARDS + 3;
    }
#endif

    if (!ShouldSkipPersistentResultWrite() &&
        resultScreen->resultScreenState == RESULT_SCREEN_STATE_EXIT &&
        g_GameManager.pscr[g_GameManager.CharacterShotType()][g_GameManager.currentStage - 1][g_GameManager.difficulty]
                .score < g_GameManager.score)
    {
        g_GameManager.pscr[g_GameManager.CharacterShotType()][g_GameManager.currentStage - 1][g_GameManager.difficulty]
            .score = g_GameManager.score;
    }

    resultScreen->unk_39a0.activeSpriteIndex = -1;

    return ZUN_SUCCESS;
}

ZunResult ResultScreen::DeletedCallback(ResultScreen *resultScreen)
{
    i32 character;
    i32 difficulty;
#ifdef TH_DEV_TOOLS
    const bool readOnlyStatsAudit = resultScreen == g_DebugStatsAuditResult;
#else
    const bool readOnlyStatsAudit = false;
#endif

    if (resultScreen->scoreDat != NULL)
    {
        if (!readOnlyStatsAudit)
            ResultScreen::WriteScore(resultScreen);
        ResultScreen::ReleaseScoreDat(resultScreen->scoreDat);
    }

    resultScreen->scoreDat = NULL;
    for (difficulty = 0; difficulty < HSCR_NUM_DIFFICULTIES; difficulty++)
    {
        for (character = 0; character < HSCR_NUM_CHARS_SHOTTYPES; character++)
        {
            resultScreen->FreeScore(difficulty, character);
        }
    }
    g_AnmManager->ReleaseAnm(ANM_FILE_RESULT00);
    g_AnmManager->ReleaseAnm(ANM_FILE_RESULT01);
    g_AnmManager->ReleaseAnm(ANM_FILE_RESULT02);
    g_AnmManager->ReleaseAnm(ANM_FILE_RESULT03);
    g_AnmManager->ReleaseSurface(0);

    g_Chain.Cut(resultScreen->drawChain);

    resultScreen->drawChain = NULL;

#ifdef __EMSCRIPTEN__
    if (g_NetplayEndingCycleResultAudit)
    {
        EM_ASM({
            globalThis.__eaglerNetplayResultCompleted = true;
            globalThis.__eaglerNetplayResultState = -1;
        });
        g_NetplayEndingCycleResultAudit = false;
    }
#endif

#ifdef TH_DEV_TOOLS
    if (readOnlyStatsAudit)
    {
        g_DebugStatsAuditResult = nullptr;
        SDL_Log("TH06 thcrap result stats audit: closed without WriteScore");
    }
#endif

    delete resultScreen;
    resultScreen = NULL;

    return ZUN_SUCCESS;
}
