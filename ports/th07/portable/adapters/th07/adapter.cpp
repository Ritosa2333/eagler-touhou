// Source-derived adapter; see portable/IMPORT.json for provenance.
#include "thprac/portable/session.hpp"
#include "thprac/portable/ecl_interpreter.hpp"
#include "thprac/portable/th07/adapter.hpp"
#include "thprac/portable/th07/patches.hpp"

#include <cstdlib>
#include <string>
#include <vector>
#include <cstring>

#include "RuntimeExtension.hpp"
#include "BulletManager.hpp"
#include "Gui.hpp"
#include "Stage.hpp"
#include "timeline_warps.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace thprac::portable::th07::adapter {
namespace {
Session g_Session;
PatchContext g_Context;
using std::pair;

void ECLSetHealth(ECLHelper& ecl, int offset, std::int32_t eclTime, std::int32_t health)
{
    ecl.SetPos(offset); ecl << eclTime << 0x0010006e << 0x0000ff00 << health;
    ecl << 0x100000 << 0x000c0000 << 0x0000ff00;
}
void ECLSetTime(ECLHelper& ecl, int offset, std::int32_t eclTime, std::int32_t time)
{
    ecl.SetPos(offset); ecl << eclTime << 0x00100072 << 0x0000ff00 << time;
    ecl << 0x100000 << 0x000c0000 << 0x0000ff00;
}
void ECLCallSub(ECLHelper& ecl, int offset, std::int32_t eclTime, std::int32_t subId)
{
    ecl.SetPos(offset); ecl << eclTime << 0x00100029 << 0x0000ff00 << subId;
    ecl << 0x100000 << 0x000c0000 << 0x0000ff00;
}
void ECLForceSpell(ECLHelper& ecl, int offset, std::int32_t eclTime, std::int32_t p110,
                   std::int32_t p112, std::int32_t p113, std::int32_t p114, std::int32_t p115)
{
    ecl.SetPos(offset);
    ecl << eclTime << 0x00100070 << 0x0000ff00 << p112;
    for (int shift : {24,16,8,0}) ecl << eclTime << 0x00100071 << (0x100 << ((24-shift)/8)) << ((p113 >> shift) & 0xff);
    ecl << eclTime << 0x00100072 << 0x0000ff00 << p114;
    for (int shift : {24,16,8,0}) ecl << eclTime << 0x00100073 << (0x100 << ((24-shift)/8)) << ((p115 >> shift) & 0xff);
    ecl << eclTime << 0x0010006e << 0x0000ff00 << p110;
}

#include "th07_full_patch.inc"

bool EndsWith(const std::string& value, const char* suffix)
{
    const std::string ending(suffix);
    return value.size() >= ending.size() &&
        value.compare(value.size() - ending.size(), ending.size(), ending) == 0;
}

bool RefreshFromHost()
{
#ifdef __EMSCRIPTEN__
    char* json = reinterpret_cast<char*>(EM_ASM_PTR({
        const session = Module.eaglerOptions?.thpracSession;
        if (!session || session.game !== 'th07') return 0;
        return stringToNewUTF8(JSON.stringify(session));
    }));
    if (json == nullptr) {
        g_Session = {};
        g_Session.game = Game::Th07;
        g_Context = {};
        return false;
    }
    const std::string value(json);
    std::free(json);
    return DeserializeReplaySession(value, Game::Th07, g_Session);
#else
    return g_Session.active;
#endif
}

bool EclLoaded(const char* path, std::uint8_t* data, std::size_t size)
{
    // Upstream th07_patch_main calls THSectionPatch() only when
    // thPracParam.mode == 1. State(3) still stores CalcSection() when the
    // visible Mode is Original, so section!=0 alone is not ownership of an
    // ECL patch. Keep that hidden widget state inert and clear any previous
    // section-derived context at the same boundary.
    if (!RefreshFromHost() || g_Session.mode != 1 || g_Session.section == 0) {
        // Upstream THReset() owns thFakeShot and every section-derived patch
        // value. A whole-stage/new session must never inherit the previous
        // section's PatchContext.
        g_Context = {};
        return true;
    }
    // A new section application owns a fresh context even if validation below
    // fails; stale state is never a valid fallback.
    g_Context = {};
    const std::string expected = "ecldata" + std::to_string(g_Session.stage + 1) + ".ecl";
    if (path == nullptr || !EndsWith(path, expected.c_str()))
        return false;
    static constexpr std::uint32_t crc[] = {0xb0aec128u,0xb5b3ccc6u,0x034533b8u,0x794f283eu,0x49e331a9u,0x92758a31u,0x22ebe51du,0xbfa7c466u};
    if (data == nullptr || g_Session.stage < 0 || g_Session.stage >= 8 || Crc32(data, size) != crc[g_Session.stage])
        return false;
    if (data == nullptr) return false;
    std::vector<std::uint8_t> candidate(data, data + size);
    PatchContext candidateContext;
    ECLHelper ecl(candidate.data(), candidate.size());
    if (g_Session.section >= 10000 && g_Session.section < 20000) {
        // Upstream THStageWarp is mostly a runtime timeline jump, but three
        // chapter starts also mutate the loaded ECL.  Those writes must not
        // disappear when chapter practice is represented as a timeline warp.
        const int stage = (g_Session.section - 10000) / 100;
        const int portion = (g_Session.section - 10000) % 100;
        if (stage == 3 && portion == 3) {
            ecl << pair{0xbb3c, static_cast<std::int16_t>(1559)}
                << pair{0xbb5c, static_cast<std::int16_t>(1560)};
        } else if (stage == 7 && portion == 6) {
            ecl << pair{0xee3e, static_cast<std::int16_t>(0x68)};
        } else if (stage == 8 && portion == 6) {
            ecl << pair{0x1265a, static_cast<std::int16_t>(0x68)};
        }
    } else {
        ApplySection(ecl, candidateContext, g_Session, g_Session.section);
    }
    if (!ecl.Valid()) return false;
    std::memcpy(data, candidate.data(), size);
    g_Context = candidateContext;
    return true;
}

void GameStarted(void*)
{
    if (!g_Session.active || g_Session.mode != 1 || g_Session.section == 0)
        return;
    if (g_Context.warpTime > 0) {
        RuntimeExtension::SetTimelineTime(g_Context.warpCount, g_Context.warpTime);
    } else for (const auto& warp : generated::th07TimelineWarps) {
        if (warp.section == g_Session.section) {
            RuntimeExtension::SetTimelineTime(warp.count,
                g_Session.dialogue && warp.dialogueTime ? warp.dialogueTime : warp.time);
            break;
        }
    }
    // Exact portable equivalent of upstream ECLNameFix(). Direct boss warps
    // bypass MsgRead(), where the original game normally copies these sprites
    // and configures Extra/Phantasm spell backgrounds and star drops.
    if (g_Context.nameFix) {
        switch (g_Session.stage) {
        case 0: Gui::CopyEnemyNameTexture(1551); break;
        case 3: Gui::CopyEnemyNameTexture(1557); break;
        case 5:
            Gui::CopyEnemyNameTexture(1561);
            g_Stage.spellcardVmsIdx = 2;
            g_BulletManager.itemType = ITEM_STAR;
            break;
        case 6:
            Gui::CopyEnemyNameTexture(1563);
            g_Stage.spellcardVmsIdx = 1;
            g_Stage.numSpellcardVms = 2;
            g_BulletManager.itemType = ITEM_STAR;
            break;
        case 7:
            Gui::CopyEnemyNameTexture(1565);
            g_Stage.spellcardVmsIdx = 2;
            g_BulletManager.itemType = ITEM_STAR;
            break;
        }
    }
}

struct Installer {
    Installer()
    {
        RuntimeExtension::SetEclLoadedHook(EclLoaded);
        RuntimeExtension::SetGameStartedHook(GameStarted);
    }
} g_Installer;
} // namespace

} // namespace thprac::portable::th07::adapter

extern "C" bool ThpracPortableTh07SetSessionJson(const char* json)
{
    using namespace thprac::portable;
    using namespace thprac::portable::th07::adapter;
    // Session replacement is also a patch-context ownership boundary. This is
    // the portable equivalent of THReset() setting thFakeShot back to -1.
    g_Context = {};
    if (json == nullptr) {
        g_Session = {};
        g_Session.game = Game::Th07;
        return false;
    }
    return DeserializeReplaySession(json, Game::Th07, g_Session);
}

extern "C" std::int32_t ThpracPortableTh07FakeShot()
{
    using namespace thprac::portable::th07::adapter;
    return g_Context.fakeShot;
}
