#pragma once
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <cstdio>
#include <cstring>
#include <string>

namespace Netplay {
// Title shells transport this report through the existing authenticated
// runtime-info channel. This object owns report revisions, never game state.
class SpectatorStageView {
public:
    using StageLabel = const char* (*)(std::uint32_t);
    static std::string Run(const SpectatorStagePlayback& playback) {
        if (playback.History().Segments().empty()) return {};
        const auto& cp = playback.History().Segments().front().checkpoint;
        char text[40];
        std::snprintf(text, sizeof(text), "s%08x%08x_%08x", unsigned(cp.sessionId >> 32),
            unsigned(cp.sessionId & 0xffffffffu), unsigned(cp.gameplayAbi));
        return text;
    }
    const char* State(const SpectatorStagePlayback& playback, StageLabel label = nullptr) {
        if (!Usable(playback)) return nullptr;
        const auto run = Run(playback);
        if (run != run_) {run_ = run; revision_ = 0;}
        if (revision_ == 0xfffffffeu) return nullptr;
        const auto& history = playback.History();
        const auto& segments = history.Segments();
        report_ = "{\"version\":1,\"run\":\"" + run_ + "\",\"revision\":";
        Number(++revision_);
        Field("min", segments.front().checkpoint.frame);
        Field("position", playback.Position()); Field("received", history.Received());
        Field("edge", playback.Edge()); Field("target", playback.Target());
        Flag("paused", playback.Paused()); Flag("seeking", playback.Seeking());
        Flag("following", playback.Following()); Flag("ended", history.Ended());
        report_ += ",\"nodes\":[";
        std::size_t count = segments.size();
        for (const auto& segment : segments) count += segment.nodes.size();
        // Retained input remains seekable even when an unusually long run has
        // more bookmarks than the UI's bounded report permits.
        std::size_t skip = count > 512 ? count - 512 : 0;
        bool comma = false;
        for (const auto& segment : segments) {
            Node(segment.checkpoint, true, skip, comma, label);
            for (const auto& node : segment.nodes) Node(node, false, skip, comma, label);
        }
        report_ += "]}"; return report_.c_str();
    }
    bool Control(SpectatorStagePlayback& playback, const char* run, const char* action,
                 std::uint32_t frame = 0) const {
        if (!Usable(playback) || !run || !action || Run(playback) != run) return false;
        using Action = SpectatorStagePlayback::Action;
        if (!std::strcmp(action, "pause")) return playback.Control(Action::Pause);
        if (!std::strcmp(action, "play")) return playback.Control(Action::Play);
        if (!std::strcmp(action, "live")) return playback.Control(Action::Live);
        if (std::strcmp(action, "seek") || frame > playback.Edge()) return false;
        return playback.Control(Action::Seek, frame);
    }
    void Clear() {run_.clear(); report_.clear(); revision_ = 0;}
private:
    static bool Usable(const SpectatorStagePlayback& playback) {
        const auto& segments = playback.History().Segments();
        return playback.Ready() && playback.Initialized() && !playback.Failed() && !segments.empty() &&
            playback.Position() >= segments.front().checkpoint.frame;
    }
    void Number(std::uint32_t value) {report_ += std::to_string(value);}
    void Field(const char* name, std::uint32_t value) {report_ += ",\""; report_ += name; report_ += "\":"; Number(value);}
    void Flag(const char* name, bool value) {report_ += ",\""; report_ += name; report_ += value ? "\":true" : "\":false";}
    void Node(const SpectatorStageEvent& event, bool checkpoint, std::size_t& skip,
              bool& comma, StageLabel label) {
        if (skip) {--skip; return;}
        if (comma) report_ += ',';
        comma = true; report_ += "{\"id\":"; Number(event.serial);
        report_ += checkpoint || event.node == SpectatorNodeKind::Stage ? ",\"kind\":\"stage\"" : ",\"kind\":\"boss\"";
        Field("frame", event.frame); Field("stage", event.stage);
        Field("bossSlots", checkpoint ? 0 : event.bossSlots);
        if (label) {
            const auto* text = label(event.stage);
            if (text && *text && std::strlen(text) <= 48) {
                bool valid = true;
                for (const auto* p = text; *p; ++p) if (static_cast<unsigned char>(*p) < 32 || *p == 127) valid = false;
                if (valid) {
                    report_ += ",\"label\":\"";
                    for (const auto* p = text; *p; ++p) {if (*p == '"' || *p == '\\') report_ += '\\'; report_ += *p;}
                    report_ += '"';
                }
            }
        }
        report_ += '}';
    }
    std::string run_, report_;
    std::uint32_t revision_ = 0;
};
}
