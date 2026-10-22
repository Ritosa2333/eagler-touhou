#pragma once

namespace eagler::audio {

// A read-only replay reconstructs the music timeline as well as the world.
// Its historical SFX can stay suppressed while native load/play/stop/fade
// commands establish the track at the selected playback position.
class ReplayMusicScope {
public:
    ReplayMusicScope() { ++depth_; }
    ~ReplayMusicScope() { --depth_; }
    ReplayMusicScope(const ReplayMusicScope&) = delete;
    ReplayMusicScope& operator=(const ReplayMusicScope&) = delete;
    static bool Active() { return depth_ != 0; }

private:
    inline static unsigned depth_ = 0;
};

} // namespace eagler::audio
