// Dialogue, spell-card and menu text drawn into small RGBA canvases, the job
// upstream's renderer.mjs gave an OffscreenCanvas 2D context (32px MS Gothic /
// MS Mincho with a black outline). FreeType renders the glyphs; the player's
// msgothic.ttc / msmincho.ttc are used when present, otherwise the Switch's
// built-in Japanese font.
#pragma once
#include "Platform.hpp"
#include <memory>
#include <string>

namespace th12host {
std::u32string decode_cp932(const std::string& bytes);
struct Canvas {u32 width=0,height=0;std::vector<u8> rgba;void reset(u32 w,u32 h){width=w;height=h;rgba.assign(size_t(w)*h*4,0);}};
class TextRaster {
public:
    TextRaster();~TextRaster();
    bool initialize();
    std::string font_names()const;
    // Canvas fillText with textBaseline=top, textAlign=left|right|center.
    // pixel_height is the CSS font size; weight >= 600 is synthesised bold.
    void fill(Canvas& canvas,const std::u32string& text,float x,float y,bool serif,u32 weight,float pixel_height,u32 rgb,i32 align=0);
    float measure(const std::u32string& text,bool serif,u32 weight,float pixel_height);
private:
    struct Impl;std::unique_ptr<Impl> impl;
};
}
