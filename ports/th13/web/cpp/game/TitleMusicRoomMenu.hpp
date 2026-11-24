#pragma once

#include "HudDrawData.hpp"
#include "TitleMainMenu.hpp"
#include "TitleState.hpp"

#include <cstdio>
#include <functional>
#include <string>

namespace th13 {

inline constexpr i32 kTitleScreenMusicRoom = 13;

struct TitleMusicRoomPlatform {
    virtual ~TitleMusicRoomPlatform() = default;
    virtual i32 track_count() const { return 0; }
    virtual std::string track_title(i32 track) {
        (void)track;
        return {};
    }
    virtual std::string track_comment(i32 track, i32 line) {
        (void)track;
        (void)line;
        return {};
    }
    virtual bool track_heard(i32 track) const {
        (void)track;
        return false;
    }
    virtual void mark_heard(i32 track) { (void)track; }
    virtual bool play_track(i32 track) {
        (void)track;
        return false;
    }
    virtual bool play_locked_response() { return false; }
    virtual bool resume_title_music() { return false; }
    virtual std::string locked_title(i32 track) const {
        char prefix[16];
        std::snprintf(prefix, sizeof prefix, "No.%2d ", int(track) + 1);
        std::string text = prefix;
        for (i32 i = 0; i < 11; ++i)
            text += "\x81\x48";
        return text;
    }
    virtual std::string locked_comment(i32 line) const {
        static const char* const kLines[8] = {
            "\x81\x40",
            "\x81\x40\x81\x40\x81\x96\x81\x96\x91\x49\x91\xf0\x82\xb5\x82\xbd\x8b\xc8"
            "\x82\xcd\x82\xdc\x82\xbe\x83\x51\x81\x5b\x83\x80\x92\x86\x82\xc5\x8d\xc4"
            "\x90\xb6\x82\xb3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb9\x82\xf1\x81\x96"
            "\x81\x96",
            "\x20",
            "\x81\x40\x81\x40\x81\x40\x81\x40\x8b\xc8\x82\xcc\x83\x52\x83\x81\x83\x93"
            "\x83\x67\x82\xaa\x83\x6c\x83\x5e\x83\x6f\x83\x8c\x82\xc9\x82\xc8\x82\xe9"
            "\x8b\xb0\x82\xea\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb7\x81\x42",
            "\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40"
            "\x82\xbb\x82\xea\x82\xc5\x82\xe0\x8d\xc4\x90\xb6\x82\xb5\x82\xdc\x82\xb7"
            "\x82\xa9\x81\x48",
            "\x81\x40",
            "\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xa2\x95\xfb"
            "\x82\xcd\x81\x41\x82\xe0\x82\xa4\x88\xea\x93\x78\x8c\x88\x92\xe8\x83\x7b"
            "\x83\x5e\x83\x93\x82\xf0\x89\x9f\x82\xb5\x82\xc4\x82\xad\x82\xbe\x82\xb3"
            "\x82\xa2\x81\x42",
            "\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xad\x82\xc8"
            "\x82\xa2\x95\xfb\x82\xcd\x81\x41\x83\x4a\x81\x5b\x83\x5c\x83\x8b\x82\xf0"
            "\x88\xda\x93\xae\x82\xb5\x82\xc4\x82\xad\x82\xbe\x82\xb3\x82\xa2\x81\x42",
        };
        return line >= 0 && line < 8 ? std::string(kLines[line]) : std::string{};
    }
};

class TitleMusicRoomMenu {
    TitleState& state;
    TitleMusicRoomPlatform& host;
    i32 row_ = 0;
    i32 comment_track_ = 0;
    i32 reveal_lines_ = 0;
    bool locked_armed_ = false;
    i32 frames_ = 0;
    bool fail(const std::string& reason);
    bool sound(i32 id);
    bool leave();

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleMusicRoomMenu(TitleState& state_, TitleMusicRoomPlatform& host_) : state(state_), host(host_) {}
    bool update(u32 pressed, u32 repeated);
    bool draw_texts(HudDrawServices& draw);
    i32 row() const { return row_; }
    i32 comment_track() const { return comment_track_; }
    i32 revealed_lines() const { return reveal_lines_; }
    bool locked_armed() const { return locked_armed_; }
};

}
