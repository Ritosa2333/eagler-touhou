#pragma once
#include "TitleMain.hpp"
namespace th10 {
struct TitleAudioSettings {std::int8_t music,effects;u8 mode;};
struct TitleOptionsEnvironment : TitleMainEnvironment {
    TitleAudioSettings* settings;
    i32 *music_volume,*effects_volume,*effects_gain;
    virtual void apply_music_volume()=0;
    virtual void bind_digit(AnmVm& vm,i32 sprite)=0;
};
struct TitleOptions {
    TitleMenu& title;
    TitleOptionsEnvironment& environment;
    void apply_volume();
    i32 update();
private:
    void display_mode();
    void bind(i32 script,i32 sprite);
    void visible(i32 script,bool value);
    void leave(i32 sound);
};
}
