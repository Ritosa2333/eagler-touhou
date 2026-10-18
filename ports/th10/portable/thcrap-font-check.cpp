// Test-only font-host fixture: production exports and game content stay untouched.
#include "../th10_web/cpp/sdl/FontHost.cpp"
#include <cassert>
#include <iostream>
namespace th10::Localization {
bool Active(){return true;}
const char* FontFile(){return "test.ttf";}
}
extern "C" int test_font_layout(){
    BitmapDescription description{};
    const int width=1024,height=65;const unsigned short bits=16;
    std::memcpy(reinterpret_cast<char*>(&description)+4,&width,4);
    std::memcpy(reinterpret_cast<char*>(&description)+8,&height,4);
    std::memcpy(reinterpret_cast<char*>(&description)+14,&bits,2);
    u8* pixels=nullptr;const auto bitmap=fonts_bitmap(&description,&pixels),context=fonts_context();
    fonts_select(context,bitmap);const auto font=fonts_font(32,"",128);
    fonts_select(context,font);fonts_color(context,0xabcdef);
    auto& b=get(bitmap);auto clear=[&]{std::fill(b.pixels.begin(),b.pixels.end(),0);};
    auto draw=[&](const std::string& s,int x=0){fonts_text(context,x,0,s.c_str(),u32(s.size()));};
    auto measure=[&](const std::string& s){int w=0,h=0;assert(TTF_GetStringSize(get(font).face,s.c_str(),s.size(),&w,&h));return w;};
    const std::string speaker="Reimu ",reference="Marisa ",body="I can't let things go on like this.";
    draw("<ts$"+speaker+"$"+reference+">The Hakurei Shrine.");
    const int stop=std::max(measure(speaker),measure(reference));
    assert(layout_tabs.size()==1&&layout_tabs[0]==stop);
    clear();draw("<r$"+speaker+">"+body);const auto actual=b.pixels;
    clear();draw(speaker,stop-measure(speaker));draw(body,stop);
    assert(b.pixels==actual); // Both text runs use real TTF metrics, not a whole-line right offset.
    clear();draw("<l$> The continuation.");const auto continued=b.pixels;
    clear();draw(" The continuation.",stop);assert(b.pixels==continued);
    // Chinese branch; the runner installs the Chinese pack's actual subset font.
    layout_tabs.clear();draw("<ts$灵梦  >　　早晨的博丽神社。");
    const int chinese_stop=measure("灵梦  ");assert(layout_tabs[0]==chinese_stop);
    clear();draw("<r$魔理沙  >「那，这个鸟屋型的救世主，代表什么意思？」");const auto chinese=b.pixels;
    clear();draw("魔理沙  ",chinese_stop-measure("魔理沙  "));draw("「那，这个鸟屋型的救世主，代表什么意思？」",chinese_stop);
    assert(b.pixels==chinese);
    fonts_delete_object(font);fonts_delete_context(context);fonts_delete_object(bitmap);sdl_fonts_shutdown();
    assert(layout_tabs.empty());assert(failures==0);
    std::cout<<"TH10 real-font raster: speaker/body and continuation pixels match separately positioned reference runs PASS\n";
    return 0;
}
