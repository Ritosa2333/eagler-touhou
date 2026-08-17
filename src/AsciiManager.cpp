#include "AsciiManager.hpp"
#include "StageMenu.hpp"

#include "AnmManager.hpp"
#include "PracticeRuntime.hpp"
#include "ChainPriorities.hpp"
#include "Controller.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"
#include "Localization.hpp"
#include "Supervisor.hpp"
#include "utils.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
#include <unordered_set>
#endif
#include <vector>
#include <SDL3/SDL.h>

AsciiManager g_AsciiManager;
static ChainElem g_AsciiManagerCalcChain;
static ChainElem g_AsciiManagerOnDrawMenusChain;
static ChainElem g_AsciiManagerOnDrawPopupsChain;

namespace
{
bool g_LastScoreLengthTen = false;

bool IsSingleByteSpriteTranslation(const char *text)
{
    if (text == nullptr)
        return false;
    for (const unsigned char *cursor = reinterpret_cast<const unsigned char *>(text); *cursor; ++cursor)
        if (*cursor >= 0x80)
            return false;
    return true;
}

template <typename T>
bool AppendPrintfPiece(std::string &output, const std::string &specifier, T value)
{
    const int length = std::snprintf(nullptr, 0, specifier.c_str(), value);
    if (length < 0 || length > 4096)
        return false;
    std::vector<char> buffer(static_cast<std::size_t>(length) + 1);
    if (std::snprintf(buffer.data(), buffer.size(), specifier.c_str(), value) != length)
        return false;
    output.append(buffer.data(), static_cast<std::size_t>(length));
    return true;
}

bool FormatLegacyAscii(std::string &output, const char *format, va_list args, bool translateStrings)
{
    if (format == nullptr)
        return false;
    output.clear();
    for (std::size_t index = 0; format[index] != '\0'; ++index)
    {
        if (format[index] != '%')
        {
            output.push_back(format[index]);
            continue;
        }
        const std::size_t start = index++;
        if (format[index] == '\0')
            return false;
        if (format[index] == '%')
        {
            output.push_back('%');
            continue;
        }
        while (std::strchr("-+ #0'", format[index]) != nullptr)
            ++index;
        if (format[index] == '*')
            return false;
        while (format[index] >= '0' && format[index] <= '9')
            ++index;
        if (format[index] == '$')
            return false;
        if (format[index] == '.')
        {
            ++index;
            if (format[index] == '*')
                return false;
            while (format[index] >= '0' && format[index] <= '9')
                ++index;
        }
        if (format[index] == '\0' || std::strchr("hljztLI", format[index]) != nullptr)
            return false;

        const char conversion = format[index];
        const std::string specifier(format + start, index - start + 1);
        if (conversion == 'd' || conversion == 'i' || conversion == 'c')
        {
            if (!AppendPrintfPiece(output, specifier, va_arg(args, int)))
                return false;
        }
        else if (std::strchr("uoxX", conversion) != nullptr)
        {
            if (!AppendPrintfPiece(output, specifier, va_arg(args, unsigned int)))
                return false;
        }
        else if (std::strchr("fFeEgGaA", conversion) != nullptr)
        {
            if (!AppendPrintfPiece(output, specifier, va_arg(args, double)))
                return false;
        }
        else if (conversion == 's')
        {
            const char *value = va_arg(args, const char *);
            if (value == nullptr)
                value = "(null)";
            const char *translated = translateStrings ? Localization::AsciiString(value) : value;
            if (translated != value && !IsSingleByteSpriteTranslation(translated))
                translated = value;
            if (!AppendPrintfPiece(output, specifier, translated))
                return false;
        }
        else if (conversion == 'p')
        {
            if (!AppendPrintfPiece(output, specifier, va_arg(args, void *)))
                return false;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool FormatLocalizedAsciiText(char *output, std::size_t outputSize, const char *format, va_list args,
                              bool &known, Localization::AsciiEntryView &entry)
{
    const bool active = Localization::Active();
    known = active && Localization::LookupAscii(format, entry);
    const char *selectedFormat = known && entry.hasTranslation &&
                                         IsSingleByteSpriteTranslation(entry.text)
                                     ? entry.text
                                     : format;
    std::string formatted;
    va_list localizedArgs;
    va_copy(localizedArgs, args);
    const bool ok = FormatLegacyAscii(formatted, selectedFormat, localizedArgs, active);
    va_end(localizedArgs);
    if (ok && formatted.size() < outputSize)
    {
        std::memcpy(output, formatted.c_str(), formatted.size() + 1);
        return true;
    }
    va_list fallbackArgs;
    va_copy(fallbackArgs, args);
    const int written = std::vsnprintf(output, outputSize, format, fallbackArgs);
    va_end(fallbackArgs);
    return written >= 0 && static_cast<std::size_t>(written) < outputSize;
}

float AsciiCharWidth(const AsciiManager *manager)
{
    return 14.0f * manager->scale.x;
}

float AlignCenter(const AsciiManager *manager, float center, const char *text)
{
    return center - std::strlen(text) * AsciiCharWidth(manager) * 0.5f;
}

float AlignRight(const AsciiManager *manager, float right, const char *text)
{
    return right - std::strlen(text) * AsciiCharWidth(manager);
}

void AddStringWithScaleX(AsciiManager *manager, const ZunVec3 &position, const char *text, float scaleX)
{
    const float originalScaleX = manager->scale.x;
    manager->scale.x = scaleX;
    manager->AddString(&position, text);
    manager->scale.x = originalScaleX;
}

void AddTenDigitScoreAndAdvance(AsciiManager *manager, ZunVec3 &position, bool leadingZeroes, int score)
{
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), leadingZeroes ? "%.10d" : "%10d", score);
    const float originalScaleX = manager->scale.x;
    manager->scale.x = 0.9f;
    manager->AddString(&position, buffer);
    position.x += 10.0f * AsciiCharWidth(manager);
    manager->scale.x = originalScaleX;
}

const char *SingleByteIdTranslation(const char *id, const char *fallback)
{
    const char *translated = Localization::AsciiStringById(id, fallback);
    return translated != fallback && !IsSingleByteSpriteTranslation(translated) ? fallback : translated;
}

#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
void DebugLogLocalizedAsciiHit(const AsciiManager *manager, int firstString, const ZunVec3 &sourcePos,
                               const Localization::AsciiEntryView &entry)
{
    static std::unordered_set<std::string> seen;
    if (entry.id == nullptr || !seen.emplace(entry.id).second)
        return;
    SDL_Log("th06 thcrap ASCII display: id=%s source=(%.3f,%.3f) strings=%d",
            entry.id, sourcePos.x, sourcePos.y, manager->numStrings - firstString);
    for (int index = firstString; index < manager->numStrings; ++index)
    {
        const AsciiManagerString &string = manager->strings[index];
        SDL_Log("th06 thcrap ASCII display part: id=%s part=%d text=%s pos=(%.3f,%.3f) scale=(%.3f,%.3f)",
                entry.id, index - firstString, string.text, string.position.x, string.position.y,
                string.scale.x, string.scale.y);
    }
}
#endif
} // namespace

AsciiManager::AsciiManager()
{
}

StageMenu::StageMenu()
{
}

ChainCallbackResult AsciiManager::OnUpdate(AsciiManager *mgr)
{
    // Text is produced by the calc chain and must remain available for every
    // presentation frame until the next 60 Hz simulation tick.
    mgr->numStrings = 0;
    mgr->vm0.UpdatePrev();
    mgr->vm1.UpdatePrev();
    for (AnmVm &vm : mgr->gameMenu.menuSprites) vm.UpdatePrev();
    mgr->gameMenu.menuBackground.UpdatePrev();
    for (AnmVm &vm : mgr->retryMenu.menuSprites) vm.UpdatePrev();
    mgr->retryMenu.menuBackground.UpdatePrev();
    if (!g_GameManager.isInGameMenu && !g_GameManager.isInRetryMenu)
    {
        AsciiManagerPopup *curPopup = &mgr->popups[0];
        i32 i = 0;
        for (; i < ARRAY_SIZE_SIGNED(mgr->popups); i++, curPopup++)
        {
            if (!curPopup->inUse)
            {
                continue;
            }

            curPopup->position.y -= 0.5f * g_Supervisor.effectiveFramerateMultiplier;
            curPopup->timer.Tick();
            if (curPopup->timer > 60)
            {
                curPopup->inUse = false;
            }
        }
    }
    else if (g_GameManager.isInGameMenu)
    {
        if (!PracticeRuntime::UpdatePauseMenu())
            mgr->gameMenu.OnUpdateGameMenu();
    }
    if (g_GameManager.isInRetryMenu)
    {
        mgr->retryMenu.OnUpdateRetryMenu();
    }

    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult AsciiManager::OnDrawMenus(AsciiManager *mgr)
{
    if (PracticeRuntime::Active() && PracticeRuntime::GetConfig().mode != 0 &&
        g_GameManager.isInGameMenu && !g_GameManager.isInReplay)
    {
        PracticeRuntime::DrawPauseMenuPanel();
        mgr->DrawStrings();
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    mgr->DrawStrings();
    mgr->gameMenu.OnDrawGameMenu();
    mgr->retryMenu.OnDrawRetryMenu();
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult AsciiManager::OnDrawPopups(AsciiManager *mgr)
{
    if (g_Supervisor.hasD3dHardwareVertexProcessing)
    {
        mgr->DrawPopupsWithHwVertexProcessing();
    }
    else
    {
        mgr->DrawPopupsWithoutHwVertexProcessing();
    }
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult AsciiManager::RegisterChain()
{
    AsciiManager *mgr = &g_AsciiManager;

    g_AsciiManagerCalcChain.callback = (ChainCallback)AsciiManager::OnUpdate;
    g_AsciiManagerCalcChain.addedCallback = NULL;
    g_AsciiManagerCalcChain.deletedCallback = NULL;
    g_AsciiManagerCalcChain.addedCallback = (ChainAddedCallback)AsciiManager::AddedCallback;
    g_AsciiManagerCalcChain.deletedCallback = (ChainDeletedCallback)AsciiManager::DeletedCallback;
    g_AsciiManagerCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_AsciiManagerCalcChain, TH_CHAIN_PRIO_CALC_ASCIIMANAGER) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    g_AsciiManagerOnDrawMenusChain.callback = (ChainCallback)OnDrawMenus;
    g_AsciiManagerOnDrawMenusChain.addedCallback = NULL;
    g_AsciiManagerOnDrawMenusChain.deletedCallback = NULL;
    g_AsciiManagerOnDrawMenusChain.arg = mgr;
    g_Chain.AddToDrawChain(&g_AsciiManagerOnDrawMenusChain, TH_CHAIN_PRIO_DRAW_ASCIIMANAGER_MENUS);

    g_AsciiManagerOnDrawPopupsChain.callback = (ChainCallback)OnDrawPopups;
    g_AsciiManagerOnDrawPopupsChain.addedCallback = NULL;
    g_AsciiManagerOnDrawPopupsChain.deletedCallback = NULL;
    g_AsciiManagerOnDrawPopupsChain.arg = mgr;
    g_Chain.AddToDrawChain(&g_AsciiManagerOnDrawPopupsChain, TH_CHAIN_PRIO_DRAW_ASCIIMANAGER_POPUPS);

    return ZUN_SUCCESS;
}

ZunResult AsciiManager::AddedCallback(AsciiManager *s)
{
    int x, y, z;

    if (g_AnmManager->LoadAnm(ANM_FILE_ASCII, "data/ascii.anm", ANM_OFFSET_ASCII) != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: failed to load data/ascii.anm");
        return ZUN_ERROR;
    }
    if (g_AnmManager->LoadAnm(ANM_FILE_ASCIIS, "data/asciis.anm", ANM_OFFSET_ASCIIS) != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: failed to load data/asciis.anm");
        return ZUN_ERROR;
    }
    if (g_AnmManager->LoadAnm(ANM_FILE_CAPTURE, "data/capture.anm", ANM_OFFSET_CAPTURE) != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: failed to load data/capture.anm");
        return ZUN_ERROR;
    }
    s->InitializeVms();
    return ZUN_SUCCESS;
}

void AsciiManager::InitializeVms()
{
    memset(this, 0, sizeof(AsciiManager));

    this->color = 0xffffffff;
    this->scale.x = 1.0;
    this->scale.y = 1.0;

    this->vm1.flags.anchor = AnmVmAnchor_TopLeft;
    AnmVm *vm1 = &this->vm1;
    AnmManager *mgr1 = g_AnmManager;
    vm1->Initialize();
    mgr1->SetActiveSprite(vm1, 0);

    AnmManager *mgr0 = g_AnmManager;
    this->vm0.Initialize();
    mgr0->SetActiveSprite(&this->vm0, 0x20);

    this->vm1.pos.z = 0.1;
    this->isSelected = 0;
}

ZunResult AsciiManager::DeletedCallback(AsciiManager *s)
{
    g_AnmManager->ReleaseAnm(ANM_FILE_ASCII);
    g_AnmManager->ReleaseAnm(ANM_FILE_ASCIIS);
    g_AnmManager->ReleaseAnm(ANM_FILE_CAPTURE);
    return ZUN_SUCCESS;
}

void AsciiManager::CutChain()
{
    g_Chain.Cut(&g_AsciiManagerCalcChain);
    g_Chain.Cut(&g_AsciiManagerOnDrawMenusChain);
    // What about g_AsciiManagerOnDrawPopupsChain? It looks like zun forgot
    // to free it!
}

void AsciiManager::AddString(const ZunVec3 *position, const char *text)
{
    if (g_SuppressAnmAdvance)
    {
        return;
    }
    if (this->numStrings >= 0x100)
    {
        return;
    }

    AsciiManagerString *curString = &this->strings[this->numStrings];
    this->numStrings += 1;
    std::snprintf(curString->text, sizeof(curString->text), "%s", text);
    curString->position = *position;
    curString->color = this->color;
    curString->scale.x = this->scale.x;
    curString->scale.y = this->scale.y;
    curString->isGui = this->isGui;
    if (g_Supervisor.cfg.IsSoftwareTexturing())
    {
        curString->isSelected = this->isSelected;
    }
    else
    {
        curString->isSelected = 0;
    }
}

void AsciiManager::AddFormatText(const ZunVec3 *position, const char *fmt, ...)
{
    char tmpBuffer[512];
    std::va_list args;
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
    const int debugFirstString = this->numStrings;
#endif

    va_start(args, fmt);
    Localization::AsciiEntryView entry{};
    bool known = false;
    const bool formatted = FormatLocalizedAsciiText(tmpBuffer, sizeof(tmpBuffer), fmt, args, known, entry);
    if (!formatted)
    {
        va_end(args);
        return;
    }

    ZunVec3 localPos = *position;
    static constexpr const char prefix[] = "th06_ascii_";
    const char *id = known && std::strncmp(entry.id, prefix, sizeof(prefix) - 1) == 0
                         ? entry.id + sizeof(prefix) - 1
                         : nullptr;

    if (id != nullptr && std::strcmp(id, "score_format") == 0)
    {
        const int score = va_arg(args, int);
        const bool isTenDigit = score >= 1000000000;
        if (isTenDigit)
        {
            g_LastScoreLengthTen = true;
            AddTenDigitScoreAndAdvance(this, localPos, false, score);
        }
        else if (g_LastScoreLengthTen)
        {
            g_LastScoreLengthTen = false;
            AddTenDigitScoreAndAdvance(this, localPos, true, score);
        }
        else
        {
            char scoreBuffer[32];
            std::snprintf(scoreBuffer, sizeof(scoreBuffer), fmt, score);
            this->AddString(&localPos, scoreBuffer);
        }
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
        DebugLogLocalizedAsciiHit(this, debugFirstString, *position, entry);
#endif
        va_end(args);
        return;
    }

    static constexpr const char resultPrefix[] = "result_score_format";
    if (id != nullptr && std::strncmp(id, resultPrefix, sizeof(resultPrefix) - 1) == 0)
    {
        const char *name = va_arg(args, const char *);
        const int score = va_arg(args, int);
        this->AddString(&localPos, name != nullptr ? name : "(null)");
        localPos.x += 9.0f * AsciiCharWidth(this);
        AddTenDigitScoreAndAdvance(this, localPos, false, score);
        const char *suffix = id + sizeof(resultPrefix) - 1;
        if (std::strcmp(suffix, "_clear") == 0)
        {
            this->AddString(&localPos, SingleByteIdTranslation("th06_ascii_result_clear", "(C)"));
        }
        else
        {
            int stage = 1;
            if (std::strcmp(suffix, "_1") != 0)
                stage = va_arg(args, int);
            char stageSuffix[4] = {'(', static_cast<char>('0' + stage % 10), ')', '\0'};
            this->AddString(&localPos, stageSuffix);
        }
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
        DebugLogLocalizedAsciiHit(this, debugFirstString, *position, entry);
#endif
        va_end(args);
        return;
    }

    static constexpr const char rankPrefix[] = "result_rank_";
    if (id != nullptr && std::strncmp(id, rankPrefix, sizeof(rankPrefix) - 1) == 0)
    {
        const char *rank = id + sizeof(rankPrefix) - 1;
        const char *fallback = fmt;
        while (*fallback == ' ')
            ++fallback;
        std::string regularId = std::string(prefix) + rank;
        const char *text = SingleByteIdTranslation(regularId.c_str(), fallback);
        localPos.x = AlignRight(this, 398.0f, text);
        this->AddString(&localPos, text);
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
        DebugLogLocalizedAsciiHit(this, debugFirstString, *position, entry);
#endif
        va_end(args);
        return;
    }

    if (id != nullptr)
    {
        if (std::strncmp(id, "centered", 8) == 0)
        {
            localPos.x = AlignCenter(this, 224.0f, tmpBuffer);
        }
        else if (std::strcmp(id, "fullpower") == 0)
        {
            const float center = localPos.x + std::strlen("Full Power Mode!!") *
                                                   AsciiCharWidth(this) * 0.5f;
            localPos.x = AlignCenter(this, center, tmpBuffer);
        }
        else if (std::strcmp(id, "bonus_format") == 0)
        {
            const float sourceWidth = std::strlen("BONUS 12345678") * AsciiCharWidth(this);
            const float sourceHalf = sourceWidth * 0.5f;
            const float shift = 224.0f - sourceHalf - 104.0f;
            const float center = localPos.x + sourceHalf + shift;
            localPos.x = AlignCenter(this, center, tmpBuffer);
        }
    }
    this->AddString(&localPos, tmpBuffer);
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
    if (known)
        DebugLogLocalizedAsciiHit(this, debugFirstString, *position, entry);
#endif
    va_end(args);
}

#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
bool AsciiManager::DebugLocalizedFormatSelfTest()
{
    if (!Localization::Active())
        return false;
    AsciiManager manager;
    manager.numStrings = 0;
    manager.scale = {1.0f, 1.0f};
    manager.color = COLOR_WHITE;
    manager.isGui = 0;
    manager.isSelected = false;
    g_LastScoreLengthTen = false;

    ZunVec3 pos{};
    pos.x = 100.0f;
    pos.y = 40.0f;

    manager.AddFormatText(&pos, "%.9d", 1000000000);
    if (manager.numStrings != 1 || std::strcmp(manager.strings[0].text, "1000000000") != 0 ||
        manager.strings[0].scale.x != 0.9f)
        return false;
    manager.AddFormatText(&pos, "%.9d", 123);
    if (manager.numStrings != 2 || std::strcmp(manager.strings[1].text, "0000000123") != 0 ||
        manager.strings[1].scale.x != 0.9f)
        return false;
    manager.AddFormatText(&pos, "%.9d", 123);
    if (manager.numStrings != 3 || std::strcmp(manager.strings[2].text, "000000123") != 0 ||
        manager.strings[2].scale.x != 1.0f)
        return false;

    const int resultStart = manager.numStrings;
    manager.AddFormatText(&pos, "%8s %9d(%d)", "ReimuA", 123, 4);
    if (manager.numStrings != resultStart + 3 ||
        std::strcmp(manager.strings[resultStart].text, "ReimuA") != 0 ||
        std::strcmp(manager.strings[resultStart + 1].text, "       123") != 0 ||
        std::strcmp(manager.strings[resultStart + 2].text, "(4)") != 0 ||
        manager.strings[resultStart].position.x != 100.0f ||
        manager.strings[resultStart + 1].position.x != 226.0f ||
        manager.strings[resultStart + 1].scale.x != 0.9f ||
        manager.strings[resultStart + 2].position.x != 352.0f)
        return false;

    const int clearStart = manager.numStrings;
    manager.AddFormatText(&pos, "%8s %9d(C)", "ReimuA", 456);
    if (manager.numStrings != clearStart + 3 ||
        std::strcmp(manager.strings[clearStart + 2].text, "(C)") != 0)
        return false;

    const int rankStart = manager.numStrings;
    manager.AddFormatText(&pos, "     Easy");
    if (manager.numStrings != rankStart + 1 ||
        manager.strings[rankStart].position.x !=
            398.0f - std::strlen(manager.strings[rankStart].text) * AsciiCharWidth(&manager))
        return false;

    const int centeredStart = manager.numStrings;
    manager.AddFormatText(&pos, "STAGE %d", 2);
    if (manager.numStrings != centeredStart + 1 ||
        manager.strings[centeredStart].position.x !=
            224.0f - std::strlen(manager.strings[centeredStart].text) * AsciiCharWidth(&manager) * 0.5f)
        return false;

    const int fullPowerStart = manager.numStrings;
    manager.AddFormatText(&pos, "Full Power Mode!!");
    const float expectedFullPowerX = 100.0f + std::strlen("Full Power Mode!!") * 7.0f -
                                     std::strlen(manager.strings[fullPowerStart].text) * 7.0f;
    if (manager.numStrings != fullPowerStart + 1 ||
        manager.strings[fullPowerStart].position.x != expectedFullPowerX)
        return false;

    const int bonusStart = manager.numStrings;
    manager.AddFormatText(&pos, "BONUS %8d", 123);
    const float bonusSourceWidth = std::strlen("BONUS 12345678") * 14.0f;
    const float bonusCenter = 100.0f + bonusSourceWidth * 0.5f +
                              (224.0f - bonusSourceWidth * 0.5f - 104.0f);
    if (manager.numStrings != bonusStart + 1 || std::strstr(manager.strings[bonusStart].text, "123") == nullptr ||
        manager.strings[bonusStart].position.x !=
            bonusCenter - std::strlen(manager.strings[bonusStart].text) * AsciiCharWidth(&manager) * 0.5f)
        return false;

    const int practiceStart = manager.numStrings;
    manager.AddFormatText(&pos, "STAGE %d  %.9d", 2, 123);
    if (manager.numStrings != practiceStart + 1 ||
        std::strstr(manager.strings[practiceStart].text, "123") == nullptr)
        return false;

    const int replayStart = manager.numStrings;
    manager.AddFormatText(&pos, "%s %9d", "Stage1", 123);
    const char *translatedStage = Localization::AsciiString("Stage1");
    if (translatedStage != nullptr && !IsSingleByteSpriteTranslation(translatedStage))
        translatedStage = "Stage1";
    if (manager.numStrings != replayStart + 1 ||
        std::strstr(manager.strings[replayStart].text, translatedStage != nullptr ? translatedStage : "Stage1") == nullptr ||
        std::strstr(manager.strings[replayStart].text, "123") == nullptr)
        return false;

    const int unknownStart = manager.numStrings;
    manager.AddFormatText(&pos, "Unknown %d", 42);
    if (manager.numStrings != unknownStart + 1 ||
        std::strcmp(manager.strings[unknownStart].text, "Unknown 42") != 0 ||
        manager.strings[unknownStart].position.x != pos.x)
        return false;
    return true;
}
#endif

void AsciiManager::DrawStrings(void)
{
    i32 padding_1;
    i32 padding_2;
    i32 padding_3;
    i32 i;
    bool guiString;
    f32 charWidth;
    AsciiManagerString *string;
    u8 *text;

    guiString = true;
    string = this->strings;
    this->vm0.flags.isVisible = 1;
    this->vm0.flags.anchor = AnmVmAnchor_TopLeft;
    for (i = 0; i < this->numStrings; i++, string++)
    {
        this->vm0.pos = string->position;
        text = (u8 *)string->text;
        this->vm0.scaleX = string->scale.x;
        this->vm0.scaleY = string->scale.y;
        charWidth = 14 * string->scale.x;
        if (guiString != string->isGui)
        {
            guiString = string->isGui;
            if (guiString)
            {
                g_Supervisor.viewport.x = g_GameManager.arcadeRegionTopLeftPos.x;
                g_Supervisor.viewport.y = g_GameManager.arcadeRegionTopLeftPos.y;
                g_Supervisor.viewport.width = g_GameManager.arcadeRegionSize.x;
                g_Supervisor.viewport.height = g_GameManager.arcadeRegionSize.y;
            }
            else
            {
                g_Supervisor.viewport.x = 0;
                g_Supervisor.viewport.y = 0;
                g_Supervisor.viewport.width = GAME_WINDOW_WIDTH;
                g_Supervisor.viewport.height = GAME_WINDOW_HEIGHT;
            }

            g_AnmManager->SetProjectionMode(PROJECTION_MODE_PERSPECTIVE);
            g_Supervisor.viewport.Set();
        }
        while (*text != '\0')
        {
            if (*text == '\n')
            {
                this->vm0.pos.y = 16 * string->scale.y + this->vm0.pos.y;
                this->vm0.pos.x = string->position.x;
            }
            else if (*text == ' ')
            {
                this->vm0.pos.x += charWidth;
            }
            else
            {
                if (!string->isSelected)
                {
                    this->vm0.sprite = &g_AnmManager->sprites[*text - 0x15];
                    this->vm0.color = string->color;
                }
                else
                {
                    this->vm0.sprite = &g_AnmManager->sprites[*text + 0x61];
                    this->vm0.color = 0xFFFFFFFF;
                }
                g_AnmManager->DrawNoRotation(&this->vm0);
                this->vm0.pos.x += charWidth;
            }
            text++;
        }
    }
}

void AsciiManager::CreatePopup1(const ZunVec3 *position, i32 value, ZunColor color)
{
    AsciiManagerPopup *popup;
    i32 characterCount;

    if (this->nextPopupIndex1 >= (ARRAY_SIZE_SIGNED(this->popups) - 3))
    {
        this->nextPopupIndex1 = 0;
    }

    popup = &this->popups[this->nextPopupIndex1];
    popup->inUse = 1;
    characterCount = 0;

    if (value >= 0)
    {
        while (value)
        {
            popup->digits[characterCount++] = (char)(value % 10);

            value /= 10;
        }
    }
    else
    {
        popup->digits[characterCount++] = '\n';
    }

    if (characterCount == 0)
    {
        popup->digits[characterCount++] = '\0';
    }

    popup->characterCount = characterCount;
    popup->color = color;
    popup->timer.InitializeForPopup();
    popup->position = *position;

    this->nextPopupIndex1++;
}

void AsciiManager::CreatePopup2(const ZunVec3 *position, i32 value, ZunColor color)
{
    AsciiManagerPopup *popup;
    i32 characterCount;

    if (this->nextPopupIndex2 >= 3)
    {
        this->nextPopupIndex2 = 0;
    }

    popup = &this->popups[0x200 + this->nextPopupIndex2];
    popup->inUse = 1;
    characterCount = 0;

    if (value >= 0)
    {
        while (value)
        {
            popup->digits[characterCount++] = (char)(value % 10);

            value /= 10;
        }
    }
    else
    {
        popup->digits[characterCount++] = '\n';
    }

    if (characterCount == 0)
    {
        popup->digits[characterCount++] = '\0';
    }

    popup->characterCount = characterCount;
    popup->color = color;
    popup->timer.InitializeForPopup();
    popup->position = *position;

    this->nextPopupIndex2++;
}

enum UpdateGameMenuState
{
    GAME_MENU_PAUSE_OPENING,
    GAME_MENU_PAUSE_CURSOR_UNPAUSE,
    GAME_MENU_PAUSE_CURSOR_QUIT,
    GAME_MENU_PAUSE_SELECTED_UNPAUSE,
    GAME_MENU_QUIT_CURSOR_YES,
    GAME_MENU_QUIT_CURSOR_NO,
    GAME_MENU_QUIT_SELECTED_YES,
};

#define GAME_MENU_SPRITE_TITLE_PAUSE 0
#define GAME_MENU_SPRITE_CURSOR_UNPAUSE 1
#define GAME_MENU_SPRITE_CURSOR_QUIT 2
#define GAME_MENU_SPRITE_TITLE_QUIT 3
#define GAME_MENU_SPRITE_CURSOR_YES 4
#define GAME_MENU_SPRITE_CURSOR_NO 5

#define GAME_MENU_SPRITES_START_PAUSE GAME_MENU_SPRITE_TITLE_PAUSE
#define GAME_MENU_SPRITES_COUNT_PAUSE 3
#define GAME_MENU_SPRITES_END_PAUSE (GAME_MENU_SPRITES_START_PAUSE + GAME_MENU_SPRITES_COUNT_PAUSE)
#define GAME_MENU_SPRITES_START_QUIT GAME_MENU_SPRITE_TITLE_QUIT
#define GAME_MENU_SPRITES_COUNT_QUIT 3
#define GAME_MENU_SPRITES_END_QUIT (GAME_MENU_SPRITES_START_QUIT + GAME_MENU_SPRITES_COUNT_QUIT)

i32 StageMenu::OnUpdateGameMenu()
{
    i32 vmIdx;

    if (WAS_PRESSED(TH_BUTTON_MENU))
    {
        this->curState = GAME_MENU_PAUSE_SELECTED_UNPAUSE;
        for (vmIdx = 0; vmIdx < ARRAY_SIZE_SIGNED(this->menuSprites); vmIdx++)
        {
            if (this->menuSprites[vmIdx].flags.isVisible)
            {
                this->menuSprites[vmIdx].pendingInterrupt = 2;
            }
        }
        this->numFrames = 0;
        this->menuBackground.pendingInterrupt = 1;
    }
    if (WAS_PRESSED(TH_BUTTON_Q))
    {
        this->curState = GAME_MENU_QUIT_SELECTED_YES;
        for (vmIdx = 0; vmIdx < ARRAY_SIZE_SIGNED(this->menuSprites); vmIdx++)
        {
            if (this->menuSprites[vmIdx].flags.isVisible)
            {
                this->menuSprites[vmIdx].pendingInterrupt = 2;
            }
        }
        this->numFrames = 0;
    }
    switch (this->curState)
    {
    case GAME_MENU_PAUSE_OPENING:
        for (vmIdx = 0; vmIdx < ARRAY_SIZE_SIGNED(this->menuSprites); vmIdx++)
        {
            g_AnmManager->SetAndExecuteScriptIdx(&this->menuSprites[vmIdx], vmIdx + 2);
        }
        for (vmIdx = GAME_MENU_SPRITES_START_PAUSE; vmIdx < GAME_MENU_SPRITES_END_PAUSE; vmIdx++)
        {
            this->menuSprites[vmIdx].pendingInterrupt = 1;
        }
        this->curState++;
        this->numFrames = 0;
        if (g_Supervisor.lockableBackbuffer)
        {
            g_AnmManager->RequestScreenshot();
            g_AnmManager->SetAndExecuteScriptIdx(&this->menuBackground, ANM_SCRIPT_CAPTURE_PAUSE_BG);
            this->menuBackground.pos.x = GAME_REGION_LEFT;
            this->menuBackground.pos.y = GAME_REGION_TOP;
            this->menuBackground.pos.z = 0.0f;
        }
    case GAME_MENU_PAUSE_CURSOR_UNPAUSE:
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_UNPAUSE].color = COLOR_LIGHT_RED;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_QUIT].color = COLOR_SET_ALPHA(COLOR_GREY, 0x80);
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_UNPAUSE].scaleY = 1.7f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_UNPAUSE].scaleX = 1.7f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_QUIT].scaleY = 1.5f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_QUIT].scaleX = 1.5f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_UNPAUSE].posOffset = ZunVec3(-4.0f, -4.0f, 0.0f);
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_QUIT].posOffset = ZunVec3(0.0f, 0.0f, 0.0f);
        if (4 <= this->numFrames)
        {
            if (WAS_PRESSED(TH_BUTTON_UP) || WAS_PRESSED(TH_BUTTON_DOWN))
            {
                this->curState = GAME_MENU_PAUSE_CURSOR_QUIT;
            }
            if (WAS_PRESSED(TH_BUTTON_SHOOT))
            {
                for (vmIdx = GAME_MENU_SPRITES_START_PAUSE; vmIdx < GAME_MENU_SPRITES_END_PAUSE; vmIdx++)
                {
                    this->menuSprites[vmIdx].pendingInterrupt = 2;
                }
                this->curState = GAME_MENU_PAUSE_SELECTED_UNPAUSE;
                this->numFrames = 0;
                this->menuBackground.pendingInterrupt = 1;
            }
        }
        break;
    case GAME_MENU_PAUSE_CURSOR_QUIT:
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_UNPAUSE].color = COLOR_SET_ALPHA(COLOR_GREY, 0x80);
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_QUIT].color = COLOR_LIGHT_RED;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_UNPAUSE].scaleY = 1.5f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_UNPAUSE].scaleX = 1.5f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_QUIT].scaleY = 1.7f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_QUIT].scaleX = 1.7f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_UNPAUSE].posOffset = ZunVec3(0.0f, 0.0f, 0.0f);
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_QUIT].posOffset = ZunVec3(-4.0f, -4.0f, 0.0f);
        if (4 <= this->numFrames)
        {
            if (WAS_PRESSED(TH_BUTTON_UP) || WAS_PRESSED(TH_BUTTON_DOWN))
            {
                this->curState = GAME_MENU_PAUSE_CURSOR_UNPAUSE;
            }
            if (WAS_PRESSED(TH_BUTTON_SHOOT))
            {
                for (vmIdx = GAME_MENU_SPRITES_START_PAUSE; vmIdx < GAME_MENU_SPRITES_END_PAUSE; vmIdx++)
                {
                    this->menuSprites[vmIdx].pendingInterrupt = 2;
                }
                for (vmIdx = GAME_MENU_SPRITES_START_QUIT; vmIdx < GAME_MENU_SPRITES_END_QUIT; vmIdx++)
                {
                    this->menuSprites[vmIdx].pendingInterrupt = 1;
                }
                this->curState = GAME_MENU_QUIT_CURSOR_NO;
                this->numFrames = 0;
            }
        }
        break;
    case GAME_MENU_PAUSE_SELECTED_UNPAUSE:
        /* Close menu, wait 20 frames for the animation? */
        if (20 <= this->numFrames)
        {
            this->curState = GAME_MENU_PAUSE_OPENING;
            PracticeRuntime::FilterUnpauseInput();
            g_GameManager.isInGameMenu = 0;
            for (vmIdx = 0; vmIdx < ARRAY_SIZE_SIGNED(this->menuSprites); vmIdx++)
            {
                this->menuSprites[vmIdx].SetInvisible();
            }
        }
        break;
    case GAME_MENU_QUIT_CURSOR_YES:
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_YES].color = COLOR_LIGHT_RED;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_NO].color = COLOR_SET_ALPHA(COLOR_GREY, 0x80);
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_YES].scaleY = 1.7f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_YES].scaleX = 1.7f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_NO].scaleY = 1.5f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_NO].scaleX = 1.5f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_YES].posOffset = ZunVec3(-4.0f, -4.0f, 0.0f);
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_NO].posOffset = ZunVec3(0.0f, 0.0f, 0.0f);
        if (4 <= this->numFrames)
        {
            if (WAS_PRESSED(TH_BUTTON_UP) || WAS_PRESSED(TH_BUTTON_DOWN))
            {
                this->curState = GAME_MENU_QUIT_CURSOR_NO;
            }
            if (WAS_PRESSED(TH_BUTTON_SHOOT))
            {
                for (vmIdx = GAME_MENU_SPRITES_START_QUIT; vmIdx < GAME_MENU_SPRITES_END_QUIT; vmIdx++)
                {
                    this->menuSprites[vmIdx].pendingInterrupt = 2;
                }
                this->curState = GAME_MENU_QUIT_SELECTED_YES;
                this->numFrames = 0;
            }
        }
        break;
    case GAME_MENU_QUIT_CURSOR_NO:
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_YES].color = COLOR_SET_ALPHA(COLOR_GREY, 0x80);
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_NO].color = COLOR_LIGHT_RED;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_YES].scaleY = 1.5f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_YES].scaleX = 1.5f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_NO].scaleY = 1.7f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_NO].scaleX = 1.7f;
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_YES].posOffset = ZunVec3(0.0f, 0.0f, 0.0f);
        this->menuSprites[GAME_MENU_SPRITE_CURSOR_NO].posOffset = ZunVec3(-4.0f, -4.0f, 0.0f);
        if (GAME_MENU_SPRITE_CURSOR_YES <= this->numFrames)
        {
            if (WAS_PRESSED(TH_BUTTON_UP) || WAS_PRESSED(TH_BUTTON_DOWN))
            {
                this->curState = GAME_MENU_QUIT_CURSOR_YES;
            }
            if (WAS_PRESSED(TH_BUTTON_SHOOT))
            {
                for (vmIdx = GAME_MENU_SPRITES_START_PAUSE; vmIdx < GAME_MENU_SPRITES_END_PAUSE; vmIdx++)
                {
                    this->menuSprites[vmIdx].pendingInterrupt = 1;
                }
                for (vmIdx = GAME_MENU_SPRITES_START_QUIT; vmIdx < GAME_MENU_SPRITES_END_QUIT; vmIdx++)
                {
                    this->menuSprites[vmIdx].pendingInterrupt = 2;
                }
                this->curState = GAME_MENU_PAUSE_CURSOR_QUIT;
                this->numFrames = 0;
            }
        }
        break;
    case GAME_MENU_QUIT_SELECTED_YES:
        if (20 <= this->numFrames)
        {
            this->curState = GAME_MENU_PAUSE_OPENING;
            g_GameManager.isInGameMenu = 0;
            g_Supervisor.curState = SUPERVISOR_STATE_MAINMENU;
            for (vmIdx = 0; vmIdx < ARRAY_SIZE_SIGNED(this->menuSprites); vmIdx++)
            {
                this->menuSprites[vmIdx].SetInvisible();
            }
        }
    }
    for (vmIdx = 0; vmIdx < ARRAY_SIZE_SIGNED(this->menuSprites); vmIdx++)
    {
        g_AnmManager->ExecuteScript(&this->menuSprites[vmIdx]);
    }
    if (g_Supervisor.lockableBackbuffer)
    {
        g_AnmManager->ExecuteScript(&this->menuBackground);
    }
    this->numFrames++;
    return 0;
}

void StageMenu::OnDrawGameMenu()
{
    i32 vmIdx;

    if (g_GameManager.isInGameMenu)
    {
        g_Supervisor.viewport.x = g_GameManager.arcadeRegionTopLeftPos.x;
        g_Supervisor.viewport.y = g_GameManager.arcadeRegionTopLeftPos.y;
        g_Supervisor.viewport.width = g_GameManager.arcadeRegionSize.x;
        g_Supervisor.viewport.height = g_GameManager.arcadeRegionSize.y;
        g_AnmManager->SetProjectionMode(PROJECTION_MODE_PERSPECTIVE);
        g_Supervisor.viewport.Set();
        if (g_Supervisor.lockableBackbuffer && this->curState != GAME_MENU_PAUSE_OPENING)
        {
            AnmVm menuBackground = this->menuBackground;
            menuBackground.flags.zWriteDisable = 1;
            g_AnmManager->DrawNoRotation(&menuBackground);
        }
        for (vmIdx = 0; vmIdx < ARRAY_SIZE_SIGNED(this->menuSprites); vmIdx++)
        {
            if (this->menuSprites[vmIdx].flags.isVisible)
            {
                g_AnmManager->DrawNoRotation(&this->menuSprites[vmIdx]);
            }
        }
    }
    g_AnmManager->FlushVertexBuffer();
    return;
}

enum RetryGameMenuState
{
    RETRY_MENU_OPENING,
    RETRY_MENU_CURSOR_YES,
    RETRY_MENU_CURSOR_NO,
    RETRY_MENU_SELECTED_YES,
    RETRY_MENU_SELECTED_NO,
};

#define RETRY_MENU_SPRITE_TITLE 0
#define RETRY_MENU_SPRITE_RETRIES_LABEL 1
#define RETRY_MENU_SPRITE_YES 2
#define RETRY_MENU_SPRITE_NO 3
#define RETRY_MENU_SPRITE_RETRIES_NUMBER 4

#define RETRY_MENU_SPRITES_START RETRY_MENU_SPRITE_TITLE
#define RETRY_MENU_SPRITES_COUNT 4
#define RETRY_MENU_SPRITES_END (RETRY_MENU_SPRITES_START + RETRY_MENU_SPRITES_COUNT)

i32 StageMenu::OnUpdateRetryMenu()
{
    i32 idx;

    if (g_GameManager.isInPracticeMode)
    {
        g_GameManager.isInRetryMenu = 0;
        g_GameManager.guiScore = g_GameManager.score;
        g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
        return 1;
    }
    if (g_GameManager.isInReplay)
    {
        g_GameManager.isInRetryMenu = 0;
        g_Supervisor.curState = SUPERVISOR_STATE_MAINMENU_REPLAY;
        g_GameManager.guiScore = g_GameManager.score;
        return 1;
    }
    if (g_GameManager.numRetries >= 3 || g_GameManager.difficulty >= EXTRA)
    {
        g_GameManager.isInRetryMenu = 0;
        g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
        g_GameManager.guiScore = g_GameManager.score;
        return 1;
    }
    switch (this->curState)
    {
    case RETRY_MENU_OPENING:
        if (this->numFrames == 0)
        {
            for (idx = RETRY_MENU_SPRITES_START; idx < RETRY_MENU_SPRITES_END; idx++)
            {
                if (idx < 2)
                {
                    g_AnmManager->SetAndExecuteScriptIdx(&this->menuSprites[idx], idx + 8);
                }
                else
                {
                    g_AnmManager->SetAndExecuteScriptIdx(&this->menuSprites[idx], idx + 4);
                }
                this->menuSprites[idx].pendingInterrupt = 1;
            }
            if (g_Supervisor.lockableBackbuffer)
            {
                g_AnmManager->RequestScreenshot();
                g_AnmManager->SetAndExecuteScriptIdx(&this->menuBackground, ANM_SCRIPT_CAPTURE_PAUSE_BG);
                this->menuBackground.pos.x = GAME_REGION_LEFT;
                this->menuBackground.pos.y = GAME_REGION_TOP;
                this->menuBackground.pos.z = 0.0f;
            }
        }
        if (this->numFrames > 8)
            break;
        this->curState += RETRY_MENU_CURSOR_NO;
        this->numFrames = 0;
    case RETRY_MENU_CURSOR_YES:
        this->menuSprites[RETRY_MENU_SPRITE_YES].color = COLOR_LIGHT_RED;
        this->menuSprites[RETRY_MENU_SPRITE_NO].color = COLOR_SET_ALPHA(COLOR_GREY, 0x80);
        this->menuSprites[RETRY_MENU_SPRITE_YES].scaleY = 1.7f;
        this->menuSprites[RETRY_MENU_SPRITE_YES].scaleX = 1.7f;
        this->menuSprites[RETRY_MENU_SPRITE_NO].scaleY = 1.5f;
        this->menuSprites[RETRY_MENU_SPRITE_NO].scaleX = 1.5f;
        this->menuSprites[RETRY_MENU_SPRITE_YES].posOffset = ZunVec3(-4.0f, -4.0f, 0.0f);
        this->menuSprites[RETRY_MENU_SPRITE_NO].posOffset = ZunVec3(0.0f, 0.0f, 0.0f);
        if (4 <= this->numFrames)
        {
            if (WAS_PRESSED(TH_BUTTON_UP) || WAS_PRESSED(TH_BUTTON_DOWN))
            {
                this->curState = RETRY_MENU_CURSOR_NO;
            }
            if (WAS_PRESSED(TH_BUTTON_SHOOT))
            {
                for (idx = RETRY_MENU_SPRITES_START; idx < RETRY_MENU_SPRITES_END; idx++)
                {
                    this->menuSprites[idx].pendingInterrupt = 2;
                }
                this->curState = RETRY_MENU_SELECTED_YES;
                this->menuBackground.pendingInterrupt = 1;
                this->numFrames = 0;
            }
        }
        break;
    case RETRY_MENU_CURSOR_NO:
        this->menuSprites[RETRY_MENU_SPRITE_NO].color = COLOR_LIGHT_RED;
        this->menuSprites[RETRY_MENU_SPRITE_YES].color = COLOR_SET_ALPHA(COLOR_GREY, 0x80);
        this->menuSprites[RETRY_MENU_SPRITE_YES].scaleY = 1.5f;
        this->menuSprites[RETRY_MENU_SPRITE_YES].scaleX = 1.5f;
        this->menuSprites[RETRY_MENU_SPRITE_NO].scaleY = 1.7f;
        this->menuSprites[RETRY_MENU_SPRITE_NO].scaleX = 1.7f;
        this->menuSprites[RETRY_MENU_SPRITE_NO].posOffset = ZunVec3(-4.0f, -4.0f, 0.0f);
        this->menuSprites[RETRY_MENU_SPRITE_YES].posOffset = ZunVec3(0.0f, 0.0f, 0.0f);
        if (this->numFrames >= 30)
        {
            if (WAS_PRESSED(TH_BUTTON_UP) || WAS_PRESSED(TH_BUTTON_DOWN))
            {
                this->curState = RETRY_MENU_CURSOR_YES;
            }
            if (WAS_PRESSED(TH_BUTTON_SHOOT))
            {
                for (idx = RETRY_MENU_SPRITES_START; idx < RETRY_MENU_SPRITES_END; idx++)
                {
                    this->menuSprites[idx].pendingInterrupt = 2;
                }
                this->curState = RETRY_MENU_SELECTED_NO;
                this->numFrames = 0;
            }
        }
        break;
    case RETRY_MENU_SELECTED_NO:
        if (this->numFrames >= 20)
        {
            this->curState = 0;
            this->numFrames = 0;
            g_GameManager.isInRetryMenu = 0;
            g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
            for (idx = RETRY_MENU_SPRITES_START; idx < RETRY_MENU_SPRITES_END; idx++)
            {
                this->menuSprites[idx].SetInvisible();
            }
            g_GameManager.guiScore = g_GameManager.score;
            return 0;
        }
        break;
    case RETRY_MENU_SELECTED_YES:
        if (this->numFrames >= 30)
        {
            this->curState = 0;
            this->numFrames = 0;
            g_GameManager.isInRetryMenu = 0;
            for (idx = RETRY_MENU_SPRITES_START; idx < RETRY_MENU_SPRITES_END; idx++)
            {
                this->menuSprites[idx].SetInvisible();
            }
            g_GameManager.numRetries++;
            g_GameManager.guiScore = g_GameManager.numRetries;
            g_GameManager.nextScoreIncrement = 0;
            g_GameManager.score = g_GameManager.guiScore;
            g_GameManager.livesRemaining = g_Supervisor.defaultConfig.lifeCount;
            g_GameManager.bombsRemaining = g_Supervisor.defaultConfig.bombCount;
            g_GameManager.grazeInStage = 0;
            g_GameManager.currentPower = 0;
            g_GameManager.pointItemsCollectedInStage = 0;
            g_GameManager.extraLives = 0;
            g_Gui.flags.flag0 = 2;
            g_Gui.flags.flag1 = 2;
            g_Gui.flags.flag3 = 2;
            g_Gui.flags.flag4 = 2;
            g_Gui.flags.flag2 = 2;
            return 0;
        }
        break;
    }
    for (idx = RETRY_MENU_SPRITES_START; idx < RETRY_MENU_SPRITES_END; idx++)
    {
        g_AnmManager->ExecuteScript(&this->menuSprites[idx]);
    }
    if (g_Supervisor.lockableBackbuffer)
    {
        g_AnmManager->ExecuteScript(&this->menuBackground);
    }
    this->numFrames++;
    return 0;
}

void StageMenu::OnDrawRetryMenu()
{
    int idx;

    if (g_GameManager.isInRetryMenu)
    {
        g_Supervisor.viewport.x = g_GameManager.arcadeRegionTopLeftPos.x;
        g_Supervisor.viewport.y = g_GameManager.arcadeRegionTopLeftPos.y;
        g_Supervisor.viewport.width = g_GameManager.arcadeRegionSize.x;
        g_Supervisor.viewport.height = g_GameManager.arcadeRegionSize.y;
        g_AnmManager->SetProjectionMode(PROJECTION_MODE_PERSPECTIVE);
        g_Supervisor.viewport.Set();
        //        g_Supervisor.d3dDevice->SetViewport(&g_Supervisor.viewport);
        if (g_Supervisor.lockableBackbuffer && (this->curState != RETRY_MENU_OPENING || this->numFrames > 2))
        {
            g_AnmManager->DrawNoRotation(&this->menuBackground);
        }
        if (this->curState == RETRY_MENU_CURSOR_YES || this->curState == RETRY_MENU_CURSOR_NO)
        {
            this->menuSprites[RETRY_MENU_SPRITE_RETRIES_NUMBER] = this->menuSprites[RETRY_MENU_SPRITE_RETRIES_LABEL];
            this->menuSprites[RETRY_MENU_SPRITE_RETRIES_NUMBER].pos.x +=
                8.0f * this->menuSprites[RETRY_MENU_SPRITE_RETRIES_NUMBER].scaleX;
            this->menuSprites[RETRY_MENU_SPRITE_RETRIES_NUMBER].sprite =
                &g_AnmManager->sprites[30 - g_GameManager.numRetries];
            g_AnmManager->DrawNoRotation(&this->menuSprites[RETRY_MENU_SPRITE_RETRIES_NUMBER]);
        }
        for (idx = RETRY_MENU_SPRITES_START; idx < RETRY_MENU_SPRITES_END; idx++)
        {
            if (this->menuSprites[idx].flags.isVisible)
            {
                g_AnmManager->DrawNoRotation(&this->menuSprites[idx]);
            }
        }
    }
    g_AnmManager->FlushVertexBuffer();
    return;
}

void AsciiManager::DrawPopupsWithHwVertexProcessing()
{
    const u8 *currentDigit;
    const AsciiManagerPopup *currentPopup;
    i32 i;
    i32 j;

    currentPopup = this->popups;
    g_Supervisor.viewport.x = g_GameManager.arcadeRegionTopLeftPos.x;
    g_Supervisor.viewport.y = g_GameManager.arcadeRegionTopLeftPos.y;
    g_Supervisor.viewport.width = g_GameManager.arcadeRegionSize.x;
    g_Supervisor.viewport.height = g_GameManager.arcadeRegionSize.y;
    g_AnmManager->SetProjectionMode(PROJECTION_MODE_PERSPECTIVE);
    g_Supervisor.viewport.Set();

    for (i = 0; i < ARRAY_SIZE_SIGNED(this->popups); i++, currentPopup++)
    {
        if (currentPopup->inUse == 0)
        {
            continue;
        }

        this->vm1.pos.x = currentPopup->position.x - (currentPopup->characterCount * 4);
        this->vm1.pos.y = currentPopup->position.y;
        this->vm1.color = currentPopup->color;

        currentDigit = (u8 *)currentPopup->digits + currentPopup->characterCount - 1;
        for (j = currentPopup->characterCount; 0 < j; j--)
        {
            this->vm1.sprite = g_AnmManager->sprites + *currentDigit;
            if (*currentDigit >= '\n')
            {
                this->vm1.matrix.m[0][0] = 0.1875f;
                this->vm1.matrix.m[1][1] = 0.03125f;
                g_AnmManager->Draw2(&this->vm1);
                this->vm1.matrix.m[0][0] = 0.03125f;
                this->vm1.matrix.m[1][1] = 0.03125f;
            }
            else
            {
                g_AnmManager->Draw2(&this->vm1);
            }

            this->vm1.pos.x += 8.0f;
            currentDigit--;
        }
    }

    return;
}

void AsciiManager::DrawPopupsWithoutHwVertexProcessing()
{
    const u8 *currentDigit;
    const AsciiManagerPopup *currentPopup;
    i32 i;
    i32 j;

    currentPopup = this->popups;
    g_Supervisor.viewport.x = g_GameManager.arcadeRegionTopLeftPos.x;
    g_Supervisor.viewport.y = g_GameManager.arcadeRegionTopLeftPos.y;
    g_Supervisor.viewport.width = g_GameManager.arcadeRegionSize.x;
    g_Supervisor.viewport.height = g_GameManager.arcadeRegionSize.y;
    g_AnmManager->SetProjectionMode(PROJECTION_MODE_PERSPECTIVE);
    g_Supervisor.viewport.Set();

    for (i = 0; i < ARRAY_SIZE_SIGNED(this->popups); i++, currentPopup++)
    {
        if (currentPopup->inUse == 0)
        {
            continue;
        }

        this->vm1.pos.x = currentPopup->position.x - (currentPopup->characterCount * 4);
        this->vm1.pos.y = currentPopup->position.y;
        this->vm1.color = currentPopup->color;

        currentDigit = (u8 *)currentPopup->digits + currentPopup->characterCount - 1;
        for (j = currentPopup->characterCount; 0 < j; j--)
        {
            this->vm1.sprite = g_AnmManager->sprites + *currentDigit;
            if (*currentDigit >= '\n')
            {
                this->vm1.matrix.m[0][0] = 0.1875f;
                this->vm1.matrix.m[1][1] = 0.03125f;
                g_AnmManager->DrawNoRotation(&this->vm1);
                this->vm1.matrix.m[0][0] = 0.03125f;
                this->vm1.matrix.m[1][1] = 0.03125f;
            }
            else
            {
                g_AnmManager->Draw2(&this->vm1);
            }

            this->vm1.pos.x += 8.0f;
            currentDigit--;
        }
    }

    return;
}
