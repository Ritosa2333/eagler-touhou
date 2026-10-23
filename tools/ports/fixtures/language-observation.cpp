#define TH_ENABLE_THCRAP 1
#include "JapaneseFonts.cpp"
#include "Localization.cpp"
#include "RuntimeOverride.cpp"
#include <eagler/diagnostics/LanguageTrace.hpp>
#include <cassert>
#include <cstring>
#include <string>

namespace th08 {
bool TextureResample::valid(const PixelSurface& image,const TextureRect& rect) noexcept {
    return image.format==25&&image.pixels&&image.pitch>=image.width*2&&rect.left>=0&&rect.top>=0&&
        rect.right>rect.left&&rect.bottom>rect.top&&u32(rect.right)<=image.width&&u32(rect.bottom)<=image.height;
}
}

static void put16(std::vector<th08::u8>& bytes,std::size_t at,th08::u16 value){bytes[at]=th08::u8(value);bytes[at+1]=th08::u8(value>>8);}
static void put32(std::vector<th08::u8>& bytes,std::size_t at,th08::u32 value){put16(bytes,at,th08::u16(value));put16(bytes,at+2,th08::u16(value>>16));}

int main(){
    using namespace th08;
    using namespace eagler::diagnostics;
    auto& trace=language_trace();trace.enable(true);trace.set_scene(1);

    const char* spell=Localization::SpellName(7,"fallback");assert(std::strcmp(spell,"fallback")==0);
    assert(std::strcmp(Localization::SpellName(7,"fallback"),"fallback")==0);
    assert(std::strcmp(Localization::StringById("scene.title","source"),"source")==0);

    std::vector<u8> cp932(131072,0);
    for(u32 code=0;code<65536;++code)put16(cp932,code*2,u16(code));
    JapaneseFonts fonts;assert(fonts.encoding.load(cp932.data(),u32(cp932.size())));
    std::vector<u8> blend(131072,0);assert(fonts.load_blend(blend.data(),u32(blend.size())));

    std::vector<u8> glyph_file(74,0);std::memcpy(glyph_file.data(),"T8GF",4);
    put32(glyph_file,4,1);put32(glyph_file,8,16);put32(glyph_file,12,400);put32(glyph_file,16,2);
    put32(glyph_file,20,72);put32(glyph_file,24,u32(glyph_file.size()));
    put32(glyph_file,32,65);put32(glyph_file,36,72);put16(glyph_file,40,1);put16(glyph_file,46,1);put16(glyph_file,48,1);
    put32(glyph_file,52,0x30fb);put32(glyph_file,56,73);put16(glyph_file,60,1);put16(glyph_file,66,1);put16(glyph_file,68,1);
    glyph_file[72]=15;glyph_file[73]=15;
    assert(fonts.load_font(glyph_file.data(),u32(glyph_file.size())));
    const auto* font=fonts.find(16,400);assert(font!=nullptr);
    u8 pixels[4]{};PixelSurface image{25,2,1,4,pixels};const u8 text[]={'A','B'};

    trace.set_phase(LanguageTrace::Phase::Preparation);
    assert(fonts.draw(*font,image,0,0,0xffffffff,text,sizeof(text)));
    for(u32 i=0;i<5000;++i)trace.glyph(16,400,u16(i),i32(i),LanguageTrace::GlyphSource::Bitmap,
        true,false,false,false,false,true);
    trace.set_phase(LanguageTrace::Phase::Runtime);trace.set_scene(2);
    assert(fonts.draw(*font,image,0,0,0xffffffff,text,sizeof(text)));
    trace.present(1,2);trace.present(2,2);

    const auto size=trace.json_size();assert(size>1);
    std::vector<char> short_json(size-1,char(0x5a));
    assert(trace.write_json(short_json.data(),short_json.size())==0);
    for(const char byte:short_json)assert(byte==char(0x5a));
    std::vector<char> json(size,0x7f);assert(trace.write_json(nullptr,size)==0);
    assert(trace.write_json(json.data(),size)==size-1&&json[size-1]=='\0');
    const std::string document(json.data());
    assert(document.find("\"schema\":\"eagler-touhou/language-trace/1\"")!=std::string::npos);
    assert(document.find("\"operation\":\"spell-name\"")!=std::string::npos);
    assert(document.find("\"preparationGlyphs\":{\"observations\":5002,\"groupCapacity\":64,\"droppedObservations\":0,\"overflow\":false,\"groups\":[")!=std::string::npos);
    assert(document.find("\"kind\":\"glyph\",\"phase\":\"prepare\"")==std::string::npos);
    assert(document.find("\"droppedEvents\":0")!=std::string::npos);
    assert(document.find("\"textHex\":\"66616c6c6261636b\"")!=std::string::npos);
    assert(document.find("\"valueHex\":\"66616c6c6261636b\"")!=std::string::npos);
    assert(document.find("\"kind\":\"raster\",\"phase\":\"prepare\"")!=std::string::npos);
    assert(document.find("\"textHex\":\"4142\"")!=std::string::npos);
    assert(document.find("\"source\":\"bitmap\"")!=std::string::npos);
    assert(document.find("\"d\":65")!=std::string::npos);
    assert(document.find("\"d\":12539")!=std::string::npos);
    assert(document.find("\"flags\":33")!=std::string::npos);
    assert(document.find("\"flags\":48")!=std::string::npos);
    assert(document.find("\"kind\":\"present\"")!=std::string::npos);
    assert(document.find("\"a\":1,\"b\":2")!=std::string::npos);
    assert(document.find("\"count\":2")!=std::string::npos);

    trace.enable(true);
    for(u32 i=0;i<LanguageTrace::preparation_glyph_capacity+1;++i)
        trace.glyph(i,400,u16(i),i32(i),LanguageTrace::GlyphSource::Bitmap,false,false,false,false,false,false);
    trace.set_phase(LanguageTrace::Phase::Runtime);
    trace.glyph(16,400,65,65,LanguageTrace::GlyphSource::Bitmap,true,false,false,false,false,true);
    const auto preparation_overflow_size=trace.json_size();std::vector<char> preparation_overflow_json(preparation_overflow_size);
    assert(trace.write_json(preparation_overflow_json.data(),preparation_overflow_json.size())==preparation_overflow_size-1);
    const std::string preparation_overflow_document(preparation_overflow_json.data());
    assert(preparation_overflow_document.find("\"preparationGlyphs\":{\"observations\":65,\"groupCapacity\":64,\"droppedObservations\":1,\"overflow\":true")!=std::string::npos);
    assert(preparation_overflow_document.find("\"droppedEvents\":0")!=std::string::npos);
    assert(preparation_overflow_document.find("\"kind\":\"glyph\",\"phase\":\"runtime\"")!=std::string::npos);

    trace.enable(true);const u8 overlong[129]{};trace.raster(overlong,sizeof(overlong),LanguageTrace::Encoding::Cp932,16,400);
    trace.raster(overlong,sizeof(overlong),LanguageTrace::Encoding::Cp932,16,400);
    for(u32 i=0;i<LanguageTrace::event_capacity-1;++i)
        trace.lookup(LanguageTrace::LookupOperation::SpellName,"key","value","fallback",true,true,i);
    const auto overflow_size=trace.json_size();std::vector<char> overflow_json(overflow_size);
    assert(trace.write_json(overflow_json.data(),overflow_json.size())==overflow_size-1);
    const std::string overflow_document(overflow_json.data());
    assert(overflow_document.find("\"overflow\":true")!=std::string::npos);
    assert(overflow_document.find("\"stringTruncated\":true")!=std::string::npos);
    assert(overflow_document.find("\"textTruncated\":true")!=std::string::npos);
    assert(overflow_document.find("\"droppedEvents\":1")!=std::string::npos);
}
