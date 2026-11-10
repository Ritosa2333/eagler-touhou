#pragma once
#include "AudioDevice.hpp"
#include "FontDevice.hpp"
#include "../game/ScreenViews.hpp"
#include "../game/TitleTrophies.hpp"
#include <deque>

namespace th17::sdl {
// Presentation owns these ANMs so their sparkle RNG and handles never enter
// the run, and the notice can finish during a scene change.
class TrophyNotice {
    static constexpr i32 bank=0,interval=240;
    Rng visual;
    AnmEnvironment environment;
    AnmManager animations{visual,environment};
    std::deque<i32> pending;
    i32 age=interval;
    u32 root=0;
    bool start(const TrophyEntry&);
    GraphicsDevice& graphics;
    FontDevice& fonts;
    AudioDevice& audio;
    const TrophyCatalog& catalog;
    AssetSource& assets;
public:
    std::string error;
    TrophyNotice(GraphicsDevice& g,FontDevice& f,AudioDevice& a,const TrophyCatalog& c,AssetSource& source):graphics(g),fonts(f),audio(a),catalog(c),assets(source){}
    ~TrophyNotice();
    void enqueue(i32 id){if(catalog.entry(id))pending.push_back(id);}
    bool draw(AnmRenderer&,ScreenViews&,const AnmEnvironment&);
};
}
