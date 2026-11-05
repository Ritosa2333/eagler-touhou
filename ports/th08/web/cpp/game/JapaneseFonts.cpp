#include "JapaneseFonts.hpp"
#ifdef TH_ENABLE_THCRAP
#include "Localization.hpp"
#endif
#include <algorithm>
namespace th08 {
#ifdef TH_NATIVE_PLATFORM
bool sdl_glyph(i32 height,i32 weight,u16 code,JapaneseFont::Glyph&,std::vector<u8>&);
#endif
namespace {
u16 short_word(const u8* p){return u16(p[0])|(u16(p[1])<<8);}
u32 word(const u8* p){return u32(short_word(p))|(u32(short_word(p+2))<<16);}
}
bool utf8_valid(const u8* text,u32 size) noexcept {
    if(!text)return size==0;
    const u8* end=text+size;
    while(text<end){
        const u8 first=*text;u32 length;
        if(first<0x80)length=1;
        else if(first>=0xc2&&first<=0xdf)length=2;
        else if(first>=0xe0&&first<=0xef)length=3;
        else if(first>=0xf0&&first<=0xf4)length=4;
        else return false;
        if(u32(end-text)<length)return false;
        for(u32 i=1;i<length;i++)if((text[i]&0xc0)!=0x80)return false;
        if((length==3&&first==0xe0&&text[1]<0xa0)||(length==3&&first==0xed&&text[1]>=0xa0)||
           (length==4&&first==0xf0&&text[1]<0x90)||(length==4&&first==0xf4&&text[1]>=0x90))return false;
        text+=length;
    }
    return true;
}
u16 utf8_next(const u8*& cursor,const u8* end) noexcept {
    if(cursor>=end)return 0;
    const u8 first=*cursor++;
    if(first<0x80)return first;
    const u32 length=(first&0xe0)==0xc0?2:(first&0xf0)==0xe0?3:(first&0xf8)==0xf0?4:1;
    if(length==1)return 0x30fb;
    u32 code=first&(length==2?0x1f:length==3?0x0f:0x07);
    for(u32 i=1;i<length;++i){
        if(cursor>=end||(*cursor&0xc0)!=0x80)return 0x30fb;
        code=(code<<6)|(*cursor++&0x3f);
    }
    // The glyph pipeline is BMP-addressed; a non-BMP code point renders as
    // the same middle-dot fallback the CP932 path uses for unmapped bytes.
    return code>0xffff?0x30fb:u16(code);
}
u32 utf8_display_columns(const u8* text,u32 size) noexcept {
    if(!text)return 0;
    u32 columns=0;const u8* cursor=text;const u8* end=text+size;
    while(cursor<end){const u16 code=utf8_next(cursor,end);if(!code)break;columns+=code<0x80?1:2;}
    return columns;
}
bool Cp932::load(const u8* data,u32 size){if(!data||size!=131072)return false;mapping.resize(65536);for(u32 i=0;i<65536;++i)mapping[i]=short_word(data+i*2);return true;}
u16 Cp932::next(const u8*& cursor,const u8* end) const noexcept {
    if(cursor>=end)return 0;
    u32 n=*cursor++;
    if((n>=0x81&&n<=0x9f)||(n>=0xe0&&n<=0xfc)){
        if(cursor>=end||!*cursor)return 0x30fb;
        n=(n<<8)|*cursor++;
    }
    return loaded()?mapping[n]:n<=0x80?u16(n):0x30fb;
}
bool JapaneseFont::load(const u8* data,u32 size){
    if(!data||size<32||std::memcmp(data,"T8GF",4)||word(data+4)!=1||word(data+24)!=size)return false;
    const u32 count=word(data+16),start=word(data+20);
    if(count>65536||start!=32+count*20||start>size)return false;
    std::vector<Glyph> parsed;parsed.reserve(count);
    for(u32 i=0;i<count;++i){const auto* p=data+32+i*20;Glyph g{word(p),word(p+4),i16(short_word(p+8)),i16(short_word(p+10)),i16(short_word(p+12)),short_word(p+14),short_word(p+16)};
        if(g.code>65535||(i&&g.code<=parsed.back().code)||g.offset<start||g.offset>size||u64(g.width)*g.height>size-g.offset)return false;
        for(u32 j=0;j<u32(g.width)*g.height;++j)if(data[g.offset+j]>15)return false;
        parsed.push_back(g);
    }
    height=i32(word(data+8));weight=i32(word(data+12));bytes.assign(data,data+size);glyphs=std::move(parsed);return true;
}
const JapaneseFont::Glyph* JapaneseFont::find(u16 code) const noexcept {
#ifdef TH_NATIVE_PLATFORM
    if(native){auto found=rasters.find(code);if(found!=rasters.end())return &found->second.glyph;
        Raster raster;if(!sdl_glyph(height,weight,code,raster.glyph,raster.pixels))return nullptr;
        return &rasters.emplace(code,std::move(raster)).first->second.glyph;}
#endif
    const auto at=std::lower_bound(glyphs.begin(),glyphs.end(),code,[](const Glyph& g,u16 c){return g.code<c;});
    return at!=glyphs.end()&&at->code==code?&*at:nullptr;
}
const u8* JapaneseFont::coverage(const Glyph& g) const noexcept {
#ifdef TH_NATIVE_PLATFORM
    if(native)return rasters.find(u16(g.code))->second.pixels.data();
#endif
    return bytes.data()+g.offset;
}
bool JapaneseFonts::load_blend(const u8* data,u32 size){if(!data||size!=131072)return false;for(u32 i=0;i<size;++i)if(data[i]>31)return false;blend.assign(data,data+size);return true;}
bool JapaneseFonts::load_font(const u8* data,u32 size){
    auto font=std::make_unique<JapaneseFont>();if(!font->load(data,size))return false;
    for(auto& f:fonts)if(f->height==font->height&&f->weight==font->weight){f=std::move(font);return true;}
    fonts.push_back(std::move(font));return true;
}
const JapaneseFont* JapaneseFonts::find(i32 height,i32 weight) const noexcept {
    for(const auto& f:fonts)if(f->height==height&&f->weight==weight)return f.get();
#ifdef TH_NATIVE_PLATFORM
    if(height>0&&height<=64){auto f=std::make_unique<JapaneseFont>();f->height=height;f->weight=weight;f->native=true;auto* result=f.get();fonts.push_back(std::move(f));return result;}
#endif
    return nullptr;
}
bool JapaneseFonts::draw(const JapaneseFont& font,PixelSurface& image,i32 x,i32 y,u32 color,const u8* text,u32 size) const {
    if(!text||!encoding.loaded()||blend.size()!=131072||image.format!=25||!TextureResample::valid(image,{0,0,i32(image.width),i32(image.height)}))return false;
    const u32 red=color&255,green=(color>>8)&255,blue=(color>>16)&255;
    const auto* cursor=text;const auto* end=text+size;
#ifdef TH_ENABLE_THCRAP
    // Prepared translations are UTF-8; original assets stay CP932. Detect the
    // encoding per string at this rasterization boundary. A disabled or
    // absent pack keeps the exact CP932 byte path.
    const bool utf8=Localization::Active()&&utf8_valid(text,size);
#else
    const bool utf8=false;
#endif
    while(cursor<end){const u16 code=utf8?utf8_next(cursor,end):encoding.next(cursor,end);
        auto* glyph=font.find(code);if(!glyph)glyph=font.find(0x30fb);if(!glyph)return false;
        const auto* coverage=font.coverage(*glyph);
        for(u32 j=0;j<glyph->height;++j){const i64 row=i64(y)+glyph->top+j;if(row<0||row>=image.height)continue;
            for(u32 i=0;i<glyph->width;++i){const i64 column=i64(x)+glyph->left+i;const u32 index=coverage[j*glyph->width+i];if(!index||column<0||column>=image.width)continue;
                auto* p=image.pixels+row*image.pitch+column*2;const u32 before=short_word(p),base=index*8192;
                const u16 value=(blend[base+((before>>10)&31)*256+red]<<10)|(blend[base+((before>>5)&31)*256+green]<<5)|blend[base+(before&31)*256+blue];
                p[0]=u8(value);p[1]=u8(value>>8);
            }
        }
        x=wrapping_add(x,glyph->advance);
    }
    return true;
}
}
