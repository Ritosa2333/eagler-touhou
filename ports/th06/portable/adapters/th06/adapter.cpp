#include "thprac/portable/session.hpp"
#include "thprac/portable/ecl_interpreter.hpp"
#include "thprac/portable/th06/adapter.hpp"
#include "thprac/portable/th06/patches.hpp"

#include <cstdlib>
#include <string>
#include <vector>
#include <cstring>
#include "PurpleRandom.hpp"
#include "Rng.hpp"
#include "BulletManager.hpp"

#ifdef TH_DEV_TOOLS
#include <SDL3/SDL_log.h>
#endif

#include "RuntimeExtension.hpp"
#include "AnmManager.hpp"
#include "timeline_warps.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace thprac::portable::th06::adapter {
namespace {
Session g_Session;
PatchContext g_Context;
using std::pair;

#include "th06_ecl_helpers.inc"
#include "th06_full_patch.inc"
#include "th06_legacy_patch.inc"

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
        if (!session || session.game !== 'th06') return 0;
        return stringToNewUTF8(JSON.stringify(session));
    }));
    if (json == nullptr) {
        g_Session = {};
        g_Session.game = Game::Th06;
        g_Context = {};
        return false;
    }
    const std::string value(json);
    std::free(json);
    return DeserializeReplaySession(value, Game::Th06, g_Session);
#else
    return g_Session.active;
#endif
}

bool EclLoaded(const char* path, std::uint8_t* data, std::size_t size)
{
    if (!RefreshFromHost() || g_Session.mode != 1 || g_Session.section == 0) {
        // THSectionPatch is called only from th06_patch_main's mode==1 body.
        // A hidden section value in Original mode is therefore inert.  Clear
        // every section-derived portable side effect at the same boundary so
        // a prior advanced section cannot leak into this run.
        g_Context = {};
        return true;
    }
    g_Context = {};
    const std::string expected = "ecldata" + std::to_string(g_Session.stage + 1) + ".ecl";
    if (path == nullptr || !EndsWith(path, expected.c_str()))
        return false;
    static constexpr std::uint32_t crc[] = {0xce4995b1u,0x65d97a32u,0x25803cc6u,0x49e683e9u,0xcfc2dad9u,0xdde6c391u,0xb892a4d6u};
    if (data == nullptr || g_Session.stage < 0 || g_Session.stage >= 7 || Crc32(data, size) != crc[g_Session.stage])
        return false;
    std::vector<std::uint8_t> candidate(data,data+size);
    PatchContext candidateContext;
    candidateContext.seed=g_Rng.seed;
    ECLHelper ecl(candidate.data(),candidate.size());
    if(g_Session.purpleSections) ApplySection(ecl, candidateContext, g_Session, g_Session.section);
    else ApplyLegacySection(ecl,candidateContext,g_Session,g_Session.section);
    const bool valid = ecl.Valid();
#ifdef TH_DEV_TOOLS
    SDL_Log("TH06 thprac ECL patch: path=%s stage=%d section=%d valid=%d warpCount=%d warpTime=%d",
            path, g_Session.stage, g_Session.section, valid ? 1 : 0,
            g_Context.warpCount, g_Context.warpTime);
#endif
    if (!valid) return false;
    std::memcpy(data,candidate.data(),size);
    g_Context=candidateContext;
    return true;
}

void GameStarted(void*)
{
    if (!g_Session.active || g_Session.mode != 1 || g_Session.section == 0)
        return;
    if (g_Context.bulletInsertIndex >= 0)
        g_BulletManager.nextBulletIndex=g_Context.bulletInsertIndex % 640;
    if (g_Context.warpTime > 0) {
        RuntimeExtension::SetTimelineTime(g_Context.warpCount, g_Context.warpTime);
    } else for (const auto& warp : generated::th06TimelineWarps) {
        if (warp.section == g_Session.section) {
            RuntimeExtension::SetTimelineTime(warp.count,
                g_Session.dialogue && warp.dialogueTime ? warp.dialogueTime : warp.time);
            break;
        }
    }
    // ECLNameFix() from upstream: late-stage direct warps skip the dialogue
    // that normally loads these boss-name/effect sprites.
    if (g_Context.nameFix && g_AnmManager) {
        if (g_Session.stage == 5) {
            g_AnmManager->LoadAnm(ANM_FILE_EFFECTS, "data/eff06.anm", ANM_OFFSET_EFFECTS);
        } else if (g_Session.stage == 6) {
            g_AnmManager->LoadAnm(ANM_FILE_EFFECTS, "data/eff07.anm", ANM_OFFSET_EFFECTS);
            g_AnmManager->LoadAnm(ANM_FILE_FACE_STAGE_A, "data/face12c.anm", ANM_OFFSET_FACE_STAGE_A);
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
} // namespace thprac::portable::th06::adapter

extern "C" bool ThpracPortableTh06SetSessionJson(const char* json)
{
    using namespace thprac::portable;
    using namespace thprac::portable::th06::adapter;
    if (json == nullptr) {
        g_Session = {};
        g_Session.game = Game::Th06;
        g_Context = {};
        return false;
    }
    return DeserializeReplaySession(json, Game::Th06, g_Session);
}
