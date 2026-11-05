#pragma once
#include "AnmRenderer.hpp"
#include <string>
#include <vector>
namespace th11 {
struct AsciiStyle {
    u32 color=0xffffffff;
    Vec2 scale{1,1};
    i32 font=0;
    bool shadow=false,playfield=false;
    i32 pass=0;
};
struct AsciiRequest {std::string text;Vec3 position;AsciiStyle style;};
// Original ASCII supervisor 4014e0 / 401670, using the shipped sprite atlas.
class AsciiText {
public:
    explicit AsciiText(AnmResource& resource):resource(resource){requests.reserve(320);}
    i32 spacing=9;
    std::vector<AsciiRequest> requests;
    void clear(){requests.clear();}
    bool add(const char*,Vec3,const AsciiStyle&);
    bool draw(AnmRenderer&,i32 pass,SceneCamera& full,SceneCamera& play);
private:
    AnmResource& resource;
};
}
