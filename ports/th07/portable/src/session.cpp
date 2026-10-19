#include "thprac/portable/session.hpp"

#include <algorithm>
#include <cstdlib>
#include <sstream>

namespace thprac::portable {
namespace {
template <typename T> T Clamp(T value, T low, T high)
{
    return std::max(low, std::min(value, high));
}

bool FindValue(const std::string& json, const char* key, std::size_t& position)
{
    const std::string needle = std::string("\"") + key + "\"";
    position = json.find(needle);
    if (position == std::string::npos)
        return false;
    position = json.find(':', position + needle.size());
    if (position == std::string::npos)
        return false;
    position = json.find_first_not_of(" \t\r\n", position + 1);
    return position != std::string::npos;
}

std::int64_t Number(const std::string& json, const char* key, std::int64_t fallback)
{
    std::size_t position;
    if (!FindValue(json, key, position))
        return fallback;
    char* end = nullptr;
    const auto value = std::strtoll(json.c_str() + position, &end, 10);
    return end == json.c_str() + position ? fallback : value;
}

bool Boolean(const std::string& json, const char* key, bool fallback)
{
    std::size_t position;
    if (!FindValue(json, key, position))
        return fallback;
    if (json.compare(position, 4, "true") == 0)
        return true;
    if (json.compare(position, 5, "false") == 0)
        return false;
    return fallback;
}
} // namespace

void Normalize(Session& s)
{
    s.mode = Clamp(s.mode, 0, 1);
    s.stage = Clamp(s.stage, 0, s.game == Game::Th06 ? 6 : 7);
    s.warp = Clamp(s.warp, 0, s.game == Game::Th06 ? 9 : 8);
    s.section = Clamp(s.section, 0, 19999);
    s.phase = Clamp(s.phase, 0, 64);
    s.frame = std::max(0, s.frame);
    s.score = Clamp<std::int64_t>(s.score, 0, 9999999990LL);
    s.life = Clamp(s.life, 0, 8);
    s.bomb = Clamp(s.bomb, 0, 8);
    s.power = Clamp(s.power, 0, 128);
    s.graze = Clamp(s.graze, 0, 99999);
    s.point = Clamp(s.point, 0, 9999);
    s.pointTotal = Clamp(s.pointTotal, 0, 9999);
    s.pointStage = Clamp(s.pointStage, 0, 9999);
    s.cherry = Clamp(s.cherry, 0, 9999990);
    s.cherryMax = Clamp(s.cherryMax, 0, 9999990);
    s.cherryPlus = Clamp(s.cherryPlus, 0, 50000);
    s.spellBonus = Clamp(s.spellBonus, 0, 30);
    // TH06 Reset() and TH07 memset Reset() both leave rank at zero, and TH07
    // also permits 33..99 while rankLock is enabled.  UI bounds are not the
    // replay/session wire-format bounds.
    s.rank = Clamp(s.rank, 0, 99);
    s.fakeType = Clamp(s.fakeType, 0, 4);
    s.difficulty = Clamp(s.difficulty, 0, s.game == Game::Th06 ? 4 : 5);
    s.shotType = Clamp(s.shotType, 0, s.game == Game::Th06 ? 3 : 5);
}

std::string SerializeReplaySession(const Session& input)
{
    Session s = input;
    Normalize(s);
    std::ostringstream out;
    out << "{\"schema\":\"thprac/portable-replay/1\",\"game\":\""
        << (s.game == Game::Th06 ? "th06" : "th07")
        << "\",\"source\":\"advanced-practice\",\"params\":{"
        << "\"mode\":" << s.mode << ",\"stage\":" << s.stage
        << ",\"warp\":" << s.warp << ",\"section\":" << s.section
        << ",\"phase\":" << s.phase << ",\"frame\":" << s.frame
        << ",\"dlg\":" << (s.dialogue ? "true" : "false")
        << ",\"score\":" << s.score << ",\"life\":" << s.life
        << ",\"bomb\":" << s.bomb << ",\"power\":" << s.power
        << ",\"graze\":" << s.graze << ",\"point\":" << s.point
        << ",\"point_total\":" << s.pointTotal << ",\"point_stage\":" << s.pointStage
        << ",\"cherry\":" << s.cherry << ",\"cherryMax\":" << s.cherryMax
        << ",\"cherryPlus\":" << s.cherryPlus << ",\"spellBonus\":" << s.spellBonus
        << ",\"rank\":" << s.rank << ",\"rankLock\":" << (s.rankLock ? "true" : "false")
        << ",\"fakeType\":" << s.fakeType << ",\"difficulty\":" << s.difficulty
        << ",\"shotType\":" << s.shotType << "}}\n";
    return out.str();
}

bool DeserializeReplaySession(const std::string& json, Game expectedGame, Session& output)
{
    const bool portableSchema = json.find("\"schema\":\"thprac/portable-replay/1\"") != std::string::npos ||
        json.find("\"schema\":\"eagler-touhou/thprac-replay/1\"") != std::string::npos ||
        json.find("\"schema\":\"eagler-touhou/thprac-session/1\"") != std::string::npos;
    // Upstream thprac v2.3.0.3 ReplaySaveParam stores THPracParam::GetJson()
    // directly in the replay trailer. That payload is flat and identifies
    // itself by version+game instead of one of the portable schema wrappers.
    const bool upstreamSchema = json.find("\"version\":") != std::string::npos;
    const char* game = expectedGame == Game::Th06 ? "th06" : "th07";
    if ((!portableSchema && !upstreamSchema) ||
        json.find(std::string("\"game\":\"") + game + "\"") == std::string::npos)
        return false;
    Session s;
    s.game = expectedGame;
    s.active = true;
    s.mode = static_cast<std::int32_t>(Number(json, "mode", 0));
    s.stage = static_cast<std::int32_t>(Number(json, "stage", 0));
    s.warp = static_cast<std::int32_t>(Number(json, "warp", 0));
    s.section = static_cast<std::int32_t>(Number(json, "section", 0));
    s.phase = static_cast<std::int32_t>(Number(json, "phase", 0));
    s.frame = static_cast<std::int32_t>(Number(json, "frame", 0));
    s.dialogue = Boolean(json, "dlg", false);
    s.score = Number(json, "score", 0);
    // A standalone portable Session decode starts from zero. This matches
    // TH07 ReadJson() directly. TH06 THGuiRep has an additional source-level
    // quirk: its ReadJson() does not Reset mRepParam, so candidate fields can
    // inherit across omitted keys; that stateful candidate merge is owned by
    // TH06 PracticeRuntime before the fully resolved live Session reaches this
    // adapter, and must not be reimplemented here as a second owner.
    s.life = static_cast<std::int32_t>(Number(json, "life", 0));
    s.bomb = static_cast<std::int32_t>(Number(json, "bomb", 0));
    s.power = static_cast<std::int32_t>(Number(json, "power", 0));
    s.graze = static_cast<std::int32_t>(Number(json, "graze", 0));
    s.point = static_cast<std::int32_t>(Number(json, "point", 0));
    s.pointTotal = static_cast<std::int32_t>(Number(json, "point_total", 0));
    s.pointStage = static_cast<std::int32_t>(Number(json, "point_stage", 0));
    s.cherry = static_cast<std::int32_t>(Number(json, "cherry", 0));
    s.cherryMax = static_cast<std::int32_t>(Number(json, "cherryMax", 0));
    s.cherryPlus = static_cast<std::int32_t>(Number(json, "cherryPlus", 0));
    s.spellBonus = static_cast<std::int32_t>(Number(json, "spellBonus", 0));
    s.rank = static_cast<std::int32_t>(Number(json, "rank", 0));
    s.rankLock = Boolean(json, "rankLock", false);
    s.fakeType = static_cast<std::int32_t>(Number(json, "fakeType", 0));
    s.difficulty = static_cast<std::int32_t>(Number(json, "difficulty", 0));
    s.shotType = static_cast<std::int32_t>(Number(json, "shotType", 0));
    Normalize(s);
    output = s;
    return output.active;
}

} // namespace thprac::portable
