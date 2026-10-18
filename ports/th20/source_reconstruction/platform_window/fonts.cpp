#include "fonts.hpp"
#include "data_constants.hpp"
#include <cwchar>
#ifdef TH_SDL3
#include "platform/Fonts.hpp"
#endif

namespace th20::source::platform_window {
std::uint8_t font_available[3]{};
#ifndef TH_SDL3
HFONT fonts[22]{};
std::uint8_t* current_font_probe=nullptr;
int CALLBACK mark_font_available(const LOGFONTW*,const TEXTMETRICW*,DWORD,LPARAM) {
    *current_font_probe=1;return 1;
}
namespace {
HFONT create_font(int height,int weight,const wchar_t* face) {
    return CreateFontW(height,0,0,0,weight,0,0,0,SHIFTJIS_CHARSET,0,0,2,0x11,face);
}
}
void initialize_fonts() {
    HDC screen=GetDC(nullptr);LOGFONTW description{};
    current_font_probe=&font_available[0];wcscpy_s(description.lfFaceName,data::font_probe_gothic);
    EnumFontFamiliesExW(screen,&description,mark_font_available,0,0);
    current_font_probe=&font_available[2];wcscpy_s(description.lfFaceName,data::font_probe_mincho);
    EnumFontFamiliesExW(screen,&description,mark_font_available,0,0);
    const wchar_t* gothic;
    constexpr int modern_heights[12]={24,30,36,42,48,54,60,66,72,90,96,48};
    constexpr int legacy_heights[10]={24,28,32,36,40,44,48,60,64,32};
    if(font_available[1]) {
        gothic=L"游ゴシック";
        for(int index=0;index<12;++index) fonts[index]=create_font(modern_heights[index],index==11?600:400,gothic);
    } else if(font_available[0]) {
        gothic=L"メイリオ";
        for(int index=2;index<12;++index) fonts[index]=create_font(modern_heights[index],index==11?600:400,gothic);
    } else {
        gothic=L"ＭＳ ゴシック";
        for(int index=2;index<12;++index) fonts[index]=create_font(legacy_heights[index-2],index==11?600:400,gothic);
    }
    const wchar_t* mincho=font_available[2]?L"Yu Mincho":L"ＭＳ 明朝";
    ReleaseDC(nullptr,screen);
    constexpr int mincho_heights[5]={32,40,48,60,64};
    for(int index=0;index<5;++index) fonts[index+13]=create_font(mincho_heights[index],700,mincho);
    fonts[20]=create_font(15,700,mincho);fonts[21]=create_font(15,700,mincho);
    fonts[18]=create_font(15,400,gothic);fonts[19]=create_font(15,400,gothic);
    // Original does not write slot 12, or slots 0/1 in fallback branches.
}
#else
FontSlot font_slots[22]{};
void initialize_fonts() {
    web::fonts::initialize();
    // These flags are probes for Meiryo and Yu Mincho, respectively, not
    // generic glyph availability. The shipped shared face is MS Gothic.
    // Claiming Meiryo selected larger text cells and a six-pixel source crop,
    // which clipped dialogue and shifted text in its speech bubble.
    font_available[0]=0;
    font_available[2]=0;
    constexpr int legacy_heights[10]={24,28,32,36,40,44,48,60,64,32};
    for(int index=2;index<12;++index) font_slots[index]={legacy_heights[index-2],index==11?600:400,web::fonts::gothic};
    constexpr int mincho_heights[5]={32,40,48,60,64};
    for(int index=0;index<5;++index) font_slots[index+13]={mincho_heights[index],700,web::fonts::mincho};
    font_slots[20]={15,700,web::fonts::mincho};font_slots[21]={15,700,web::fonts::mincho};
    font_slots[18]={15,400,web::fonts::gothic};font_slots[19]={15,400,web::fonts::gothic};
    // Original does not write slot 12, or slots 0/1 in fallback branches.
}
#endif
}
namespace th20::source::program_entry::unrecovered {
void fn_00416d20() {platform_window::initialize_fonts();}
}
