#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <dwmapi.h>
#include <commdlg.h>
#include <bcrypt.h>
#include "app_icon_resource.h"
#include "th03/assets/decode.hpp"
#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include "menu.hpp"
#include "japanese.hpp"
#include "opening.hpp"
#include "frame_pacing.hpp"
#include "host_perf.hpp"
#include "hiscore.hpp"
#include "score_animation.hpp"
#include "audio_preview.hpp"
#include "stage_scene.hpp"
#include "encounter_scene.hpp"
#include "title_demo.hpp"
#include "ending_surface.hpp"
#include "native_score_store.hpp"
#include "replay_session.hpp"
#include "replay_menu.hpp"
#include "replay_slots.hpp"
#include "encounter_info.hpp"
#include "shot_input.hpp"
#include "settings.hpp"
#include "practice_menu.hpp"
#include "practice_panel.hpp"
#include <tuple>
#include "practice_session.hpp"
#include "practice_targets.hpp"
#include "practice_music.hpp"
#include <random>
#include <future>
#include "hud_renderer.hpp"
#include "window_presenter.hpp"
#include <iomanip>
#include <sstream>
#include <set>

namespace fs = std::filesystem;
std::uint64_t host_us(){
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
std::uint64_t host_ms(){return host_us()/1000;}
struct Viewport { int x, y, w, h; };
Viewport fit(int w, int h, bool crt) {
    if(w <= 0 || h <= 0) return {0, 0, 0, 0};
    const int aw = crt ? 4 : 8, ah = crt ? 3 : 5;
    int rw = w, rh = static_cast<int>(static_cast<long long>(w) * ah / aw);
    if(rh > h) { rh = h; rw = static_cast<int>(static_cast<long long>(h) * aw / ah); }
    return {(w - rw) / 2, (h - rh) / 2, rw, rh};
}
std::vector<std::byte> read(const fs::path &path,std::size_t max_bytes=4*1024*1024) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if(!f) throw std::runtime_error("Missing asset: " + path.filename().string());
    const auto size = f.tellg();
    if(size < 0 || static_cast<std::uint64_t>(size)>max_bytes) throw std::runtime_error("Invalid asset size");
    std::vector<std::byte> bytes(static_cast<std::size_t>(size));
    f.seekg(0);
    if(!f.read(reinterpret_cast<char *>(bytes.data()), size)) throw std::runtime_error("Asset read failed");
    return bytes;
}
void verify_chinese_manifest(const fs::path &root,std::string_view payload){
    const auto bytes=read(root/"locale"/"manifest.json",256*1024);
    soew::SettingsStore::Reader json{{reinterpret_cast<const char*>(bytes.data()),bytes.size()}};
    const auto object=[&](auto&& field){
        std::set<std::string> keys;
        json.take('{');
        if(json.accept('}'))return;
        do{
            const auto key=json.str();
            if(!keys.insert(key).second)throw std::runtime_error("Duplicate locale manifest key");
            json.take(':');field(key);
        }while(json.accept(','));
        json.take('}');
    };
    std::string expected_hash;
    bool version_ok=false,pack_found=false;
    object([&](const std::string &key){
        if(key=="version"){
            auto v=json.value();version_ok=v.integer&&v.text=="1";
        }else if(key=="assets")object([&](const std::string &asset){
            if(asset!="chinese-v1.txt"){json.value();return;}
            bool format=false,size=false,records=false;
            object([&](const std::string &name){
                const auto value=json.value();
                if(name=="format")format=value.string&&value.text=="SOEW-CHINESE-V1";
                else if(name=="size")size=value.integer&&value.text==std::to_string(payload.size());
                else if(name=="display_records")records=value.integer&&value.text=="1754";
                else if(name=="sha256"&&value.string)expected_hash=value.text;
                else throw std::runtime_error("Unexpected Chinese manifest metadata");
            });
            pack_found=format&&size&&records;
        });else json.value();
    });
    json.ws();
    if(!version_ok||!pack_found||json.i!=json.s.size()||expected_hash.size()!=64)
        throw std::runtime_error("Invalid Chinese manifest entry");
    struct HashProvider {
        BCRYPT_ALG_HANDLE value=nullptr;
        ~HashProvider(){if(value)BCryptCloseAlgorithmProvider(value,0);}
    } provider;
    std::array<unsigned char,32> digest{};
    if(BCryptOpenAlgorithmProvider(&provider.value,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0||
       BCryptHash(provider.value,nullptr,0,reinterpret_cast<PUCHAR>(const_cast<char*>(payload.data())),
                  static_cast<ULONG>(payload.size()),digest.data(),static_cast<ULONG>(digest.size()))<0)
        throw std::runtime_error("Chinese pack SHA-256 calculation failed");
    constexpr char hex[]="0123456789abcdef";
    for(std::size_t i=0;i<digest.size();++i){
        if(expected_hash[2*i]!=hex[digest[i]>>4]||expected_hash[2*i+1]!=hex[digest[i]&15])
            throw std::runtime_error("Chinese display pack SHA-256 mismatch");
    }
}
struct Frame { int w, h; std::vector<std::uint32_t> pixels; };
Frame load(const fs::path &root, const char *name, bool external, const th03::assets::Palette16Rgb4 *override_palette=nullptr) {
    auto image = th03::assets::decode_pi(read(root / name));
    if(!image) throw std::runtime_error("PI decode failed: " + std::string(name));
    auto palette = image->palette;
    if(external) {
        const auto decoded = th03::assets::decode_rgb48(read(root / "OP.RGB"));
        if(!decoded) throw std::runtime_error("OP.RGB decode failed");
        palette = *decoded;
    }
    if(override_palette)palette=*override_palette;
    if(!palette) throw std::runtime_error("PI requires palette");
    Frame result{image->image.width, image->image.height, {}};
    result.pixels.reserve(image->image.indices.size());
    for(const auto index : image->image.indices) {
        const auto c = palette->colors.at(index);
        result.pixels.push_back((std::uint32_t(c.r) * 17 << 16) | (std::uint32_t(c.g) * 17 << 8) | (std::uint32_t(c.b) * 17));
    }
    if(name==std::string_view("EYE.PI")){
        if(result.w!=soew::StageIntro::width||result.h!=soew::StageIntro::height)throw std::runtime_error("Unexpected stage intro dimensions");
    }else if(!((result.w==640&&(result.h==400||result.h==171))||(result.w==192&&result.h==144))) throw std::runtime_error("Unexpected scene/portrait dimensions");
    return result;
}
void dump(const Frame &frame, const fs::path &path) {
    if(fs::exists(path)) throw std::runtime_error("Refusing to overwrite dump");
    BITMAPFILEHEADER file{};
    file.bfType = 0x4d42; file.bfOffBits = sizeof(file) + sizeof(BITMAPINFOHEADER);
    file.bfSize = file.bfOffBits + static_cast<DWORD>(frame.pixels.size() * 4);
    BITMAPINFOHEADER info{};
    info.biSize = sizeof(info); info.biWidth = frame.w; info.biHeight = -frame.h;
    info.biPlanes = 1; info.biBitCount = 32; info.biCompression = BI_RGB;
    std::ofstream stream(path, std::ios::binary);
    stream.write(reinterpret_cast<const char *>(&file), sizeof(file));
    stream.write(reinterpret_cast<const char *>(&info), sizeof(info));
    stream.write(reinterpret_cast<const char *>(frame.pixels.data()), static_cast<std::streamsize>(frame.pixels.size() * 4));
    if(!stream) throw std::runtime_error("Dump write failed");
}
void jp_text(Frame &frame,std::wstring_view text,int x,int y,std::uint32_t color) {
    // Fixed 8/16px cells, monochrome GDI glyphs. NOT a PC98 ROM font.
    const auto dc=CreateCompatibleDC(nullptr);
    if(!dc)throw std::runtime_error("Japanese text DC failed");
    BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth=frame.w;info.bmiHeader.biHeight=-frame.h;
    info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
    void *bits=nullptr;auto bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&bits,nullptr,0);
    auto font=CreateFontW(-16,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,FIXED_PITCH,L"MS Gothic");
    if(!bitmap || !font){if(bitmap)DeleteObject(bitmap);if(font)DeleteObject(font);DeleteDC(dc);throw std::runtime_error("Japanese text allocation failed");}
    const auto old_bitmap=SelectObject(dc,bitmap),old_font=SelectObject(dc,font);
    std::copy(frame.pixels.begin(),frame.pixels.end(),static_cast<std::uint32_t*>(bits));
    SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB((color>>16)&255,(color>>8)&255,color&255));
    for(wchar_t c:text){const int width=jp::cell_width(std::wstring_view(&c,1));RECT clip{x,y,x+width,y+16};ExtTextOutW(dc,x,y,ETO_CLIPPED,&clip,&c,1,nullptr);x+=width;}
    GdiFlush();std::copy_n(static_cast<std::uint32_t*>(bits),frame.pixels.size(),frame.pixels.begin());
    SelectObject(dc,old_font);SelectObject(dc,old_bitmap);DeleteObject(font);DeleteObject(bitmap);DeleteDC(dc);
}
bool chinese_display(soew::Language lang){
    return lang==soew::Language::ChineseSimplified||lang==soew::Language::ChineseTraditional;
}
std::size_t display_scalar_units(std::wstring_view text){
    return text.size()>=2&&text[0]>=0xd800&&text[0]<=0xdbff&&text[1]>=0xdc00&&text[1]<=0xdfff?2:1;
}
int display_scalar_width(std::wstring_view scalar){return jp::cell_width(scalar.substr(0,1));}
std::wstring_view fit_display_text(std::wstring_view text,int max_width,soew::Language lang){
    std::size_t used=0;
    while(used<text.size()){
        const auto scalar=text.substr(used,chinese_display(lang)?display_scalar_units(text.substr(used)):1);
        max_width-=display_scalar_width(scalar);
        if(max_width<0)break;
        used+=scalar.size();
    }
    return text.substr(0,used);
}
int display_text_width(std::wstring_view text,soew::Language lang){
    if(!chinese_display(lang))return jp::cell_width(text);
    int width=0;
    while(!text.empty()){
        width+=display_scalar_width(text);
        text.remove_prefix(display_scalar_units(text));
    }
    return width;
}
// Probe actual installed faces, then cache each glyph as a 16-pixel cell mask.
class ChineseFont {
    struct Face {HFONT font;std::vector<unsigned char> cmap;};
    struct Glyph {std::array<std::uint16_t,16> rows{};bool present=false;};
    HDC dc_=nullptr;
    HBITMAP bitmap_=nullptr;
    HFONT ascii_=nullptr;
    HGDIOBJ old_bitmap_=nullptr,old_font_=nullptr;
    std::uint32_t *pixels_=nullptr;
    std::vector<Face> faces_;
    std::map<unsigned,Glyph> glyphs_;
    static unsigned u16(const std::vector<unsigned char> &a,std::size_t i){return unsigned(a[i])<<8|a[i+1];}
    static unsigned u32(const std::vector<unsigned char> &a,std::size_t i){return u16(a,i)<<16|u16(a,i+2);}
    static WORD supplementary_index(const std::vector<unsigned char> &a,unsigned cp){
        if(a.size()<4)return 0;
        for(unsigned i=0,n=std::min<std::size_t>(u16(a,2),(a.size()-4)/8);i<n;++i){
            const auto offset=u32(a,8+i*8);
            if(offset>a.size()-4||a.size()-offset<16||u16(a,offset)!=12)continue;
            const auto groups=u32(a,offset+12);
            if(groups>(a.size()-offset-16)/12)continue;
            for(unsigned lo=0,hi=groups;lo<hi;){
                const unsigned mid=lo+(hi-lo)/2;
                const auto pos=offset+16+12*mid;
                const auto first=u32(a,pos),last=u32(a,pos+4);
                if(cp<first)hi=mid;
                else if(cp>last)lo=mid+1;
                else {const auto index=u32(a,pos+8)+cp-first;return index>0&&index<0xffff?static_cast<WORD>(index):0;}
            }
        }
        return 0;
    }
    void add_face(const wchar_t *name){
        const auto face=CreateFontW(-16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
            OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,name);
        if(!face)return;
        const auto prior=SelectObject(dc_,face);
        wchar_t actual[LF_FACESIZE]{};
        std::vector<unsigned char> cmap;
        if(GetTextFaceW(dc_,LF_FACESIZE,actual)&&_wcsicmp(actual,name)==0){
            constexpr DWORD cmap_tag=0x70616d63;
            const auto size=GetFontData(dc_,cmap_tag,0,nullptr,0);
            if(size!=GDI_ERROR&&size<=4*1024*1024){
                cmap.resize(size);
                if(GetFontData(dc_,cmap_tag,0,cmap.data(),size)!=size)cmap.clear();
            }
            faces_.push_back({face,std::move(cmap)});
            SelectObject(dc_,prior);
        }else{SelectObject(dc_,prior);DeleteObject(face);}
    }
    const Glyph& get(std::wstring_view scalar){
        const unsigned cp=scalar.size()==1?unsigned(scalar[0]):0x10000+((unsigned(scalar[0])-0xd800)<<10)+(unsigned(scalar[1])-0xdc00);
        const auto [it,fresh]=glyphs_.try_emplace(cp);
        auto &glyph=it->second;
        if(fresh&&dc_&&bitmap_){
            const int width=display_scalar_width(scalar);
            const bool ascii=cp>=0x20&&cp<=0x7e;
            const auto rasterize=[&](HFONT font,const std::vector<unsigned char> *cmap){
                SelectObject(dc_,font);
                WORD index=0;
                if(scalar.size()==1){
                    if(GetGlyphIndicesW(dc_,scalar.data(),1,&index,GGI_MARK_NONEXISTING_GLYPHS)==GDI_ERROR)return false;
                }else if(cmap)index=supplementary_index(*cmap,cp);
                if(!index||index==0xffff)return false;
                const auto glyph_index=static_cast<wchar_t>(index);
                TEXTMETRICW metric{};if(!GetTextMetricsW(dc_,&metric))return false;
                int x;
                if(ascii){
                    ABC ink{};
                    if(!GetCharABCWidthsI(dc_,0,1,&index,&ink)||ink.abcB>unsigned(width))return false;
                    // Center the ink, not the advance, so side bearings cannot clip an 8-pixel cell.
                    x=(width-int(ink.abcB))/2-ink.abcA;
                }else{
                    SIZE size{};if(!GetTextExtentPointI(dc_,&index,1,&size))return false;
                    x=(width-int(size.cx))/2;
                }
                const int y=(16-int(metric.tmHeight))/2;
                RECT clip{0,0,width,16};
                GdiFlush();std::fill_n(pixels_,256,0u);
                if(!ExtTextOutW(dc_,x,y,ETO_GLYPH_INDEX|ETO_CLIPPED,&clip,&glyph_index,1,nullptr))return false;
                GdiFlush();
                for(int row=0;row<16;++row)for(int col=0;col<width;++col)
                    if(pixels_[row*16+col]&0xffffffu)glyph.rows[row]|=std::uint16_t(1u<<col);
                return std::any_of(glyph.rows.begin(),glyph.rows.end(),[](auto row){return row!=0;})||cp==0x20||cp==0xa0||cp==0x3000;
            };
            if(ascii&&ascii_)glyph.present=rasterize(ascii_,nullptr);
            if(!glyph.present)for(const auto &face:faces_){
                glyph.present=rasterize(face.font,&face.cmap);
                if(glyph.present)break;
            }
            if(!glyph.present){ // A visible replacement rather than a silent blank.
                for(int row=2;row<14;++row)glyph.rows[row]=std::uint16_t(row==2||row==13?((1u<<(width-2))-1)*2:2|(1u<<(width-2)));
            }
        }
        return glyph;
    }
public:
    explicit ChineseFont(bool traditional){
        dc_=CreateCompatibleDC(nullptr);
        if(!dc_)return;
        BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth=16;info.bmiHeader.biHeight=-16;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
        void *bits=nullptr;bitmap_=CreateDIBSection(dc_,&info,DIB_RGB_COLORS,&bits,nullptr,0);
        if(!bitmap_)return;
        pixels_=static_cast<std::uint32_t*>(bits);old_bitmap_=SelectObject(dc_,bitmap_);old_font_=GetCurrentObject(dc_,OBJ_FONT);
        SetBkMode(dc_,TRANSPARENT);SetTextColor(dc_,RGB(255,255,255));
        // Use a narrow Latin face for the 8px half-width cells, independent of the CJK locale.
        ascii_=CreateFontW(-14,7,0,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,
            OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,FIXED_PITCH|FF_MODERN,L"Courier New");
        if(traditional){for(const auto face:{L"Microsoft JhengHei",L"MingLiU",L"PMingLiU",L"MingLiU-ExtB"})add_face(face);}
        else for(const auto face:{L"Microsoft YaHei",L"SimSun",L"SimHei",L"DengXian"})add_face(face);
        for(const auto face:{L"SimSun-ExtB",L"SimSun-ExtG",L"MingLiU-ExtB",L"Microsoft YaHei",L"Microsoft JhengHei",L"MS Gothic",L"Segoe UI Symbol"})add_face(face);
    }
    ~ChineseFont(){
        if(old_font_)SelectObject(dc_,old_font_);
        if(ascii_)DeleteObject(ascii_);
        for(const auto &face:faces_)DeleteObject(face.font);
        if(old_bitmap_)SelectObject(dc_,old_bitmap_);
        if(bitmap_)DeleteObject(bitmap_);
        if(dc_)DeleteDC(dc_);
    }
    ChineseFont(const ChineseFont&)=delete;
    ChineseFont& operator=(const ChineseFont&)=delete;
    bool supports(std::wstring_view scalar){return get(scalar).present;}
    void paint(Frame &frame,std::wstring_view scalar,int x,int y,std::uint32_t color){
        const auto &glyph=get(scalar);
        const int width=display_scalar_width(scalar);
        for(int row=std::max(0,-y);row<16&&y+row<frame.h;++row)
            for(int col=std::max(0,-x);col<width&&x+col<frame.w;++col)
                if(glyph.rows[row]&(1u<<col))frame.pixels[(y+row)*frame.w+x+col]=color;
    }
};
ChineseFont& chinese_font(soew::Language lang){
    if(lang==soew::Language::ChineseTraditional){static ChineseFont hant(true);return hant;}
    static ChineseFont hans(false);return hans;
}
void display_text(Frame &frame,std::wstring_view text,int x,int y,std::uint32_t color,soew::Language lang,int max_width=640){
    if(!chinese_display(lang)){jp_text(frame,text,x,y,color);return;}
    auto &font=chinese_font(lang);
    text=fit_display_text(text,std::min(max_width,frame.w-x),lang);
    while(!text.empty()){
        const auto scalar=text.substr(0,display_scalar_units(text));
        font.paint(frame,scalar,x,y,color);
        x+=display_scalar_width(scalar);
        text.remove_prefix(scalar.size());
    }
}
void draw_practice_assist(Frame &frame,const soew::PracticeAssist &assist,soew::Language lang){
    using A=soew::PracticeAssist;
    constexpr std::array<std::wstring_view,A::rows> keys={L"F1",L"F2",L"F3",L"F4",L"F5",L"F6",L"F7",L"U",L"Tab"};
    constexpr int height=A::rows*A::pitch;
    static std::optional<std::tuple<soew::Language,std::uint8_t,bool>> last;
    static Frame text{A::width,height,std::vector<std::uint32_t>(A::width*height)};
    const auto state=std::tuple{lang,assist.tools,assist.info};
    if(last!=state){
        std::fill(text.pixels.begin(),text.pixels.end(),0u);
        for(int i=0;i<A::rows;++i){
            const bool on=assist.active_row(i);
            std::wstring label=on?L"[":L" ";label+=keys[i];label+=L": ";label+=soew::practice_tool_labels[i](lang);if(on)label+=L"]";
            display_text(text,label,5,i*A::pitch+1,on?0x66ff88:0xffffff,lang,A::width-10);
        }
        last=state;
    }
    for(int row=0;row<height;++row)for(int col=0;col<A::width;++col){
        auto &dest=frame.pixels[(A::top+row)*frame.w+A::left+col];
        const auto glyph=text.pixels[row*A::width+col];
        dest=glyph?glyph:((dest&0xfefefeu)>>1);
    }
}
// Development diagnostics occupy only the original HUD's blank rows 7..14.
void draw_debug_info(Frame &frame,const soew::EncounterScene &s,const std::optional<soew::PracticeSession> &session,
                     bool show_details,soew::Language preferred_language){
    const auto encounter=soew::encounter_info(s);
    // Keep the existing replay scene language for Japanese/English; Chinese is display-only.
    const auto language=soew::gameplay_display_language(preferred_language,s.language);
    const auto tr=[&](const wchar_t *ja,const wchar_t *en,const wchar_t *hans,const wchar_t *hant){
        return soew::localized(language,ja,en,hans,hant);
    };
    const auto info_label=[&](std::wstring_view key){return soew::encounter_info_text(key,language);};
    if(!show_details&&!encounter.active)return;
    constexpr int side=128,bar_width=120,line_pitch=10;
    const auto dc=CreateCompatibleDC(nullptr);if(!dc)throw std::runtime_error("Info DC failed");
    BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=side;info.bmiHeader.biHeight=-side;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
    void *bits=nullptr;auto bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&bits,nullptr,0);
    const auto charset=language==soew::Language::ChineseSimplified?GB2312_CHARSET:
        language==soew::Language::ChineseTraditional?CHINESEBIG5_CHARSET:SHIFTJIS_CHARSET;
    auto font=CreateFontW(-10,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,charset,
        OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,FIXED_PITCH,tr(L"MS Gothic",L"MS Gothic",L"SimSun",L"PMingLiU"));
    if(!bitmap||!font){if(bitmap)DeleteObject(bitmap);if(font)DeleteObject(font);DeleteDC(dc);throw std::runtime_error("Info bitmap failed");}
    auto oldbmp=SelectObject(dc,bitmap);auto oldfont=SelectObject(dc,font);auto *pixels=static_cast<std::uint32_t*>(bits);
    std::fill_n(pixels,side*side,0u);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(180,220,255));
    const auto text=[&](std::wstring_view value,int left,int top,int width){
        RECT r{left,top,left+width,std::min(top+line_pitch,side)};
        DrawTextW(dc,value.data(),int(value.size()),&r,DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
    };
    int y=0;
    const auto line=[&](std::wstring_view value){text(value,1,y,126);y+=line_pitch;};
    const auto ratio=[](unsigned current,unsigned max){return std::to_wstring(current)+L"/"+std::to_wstring(max);};
    struct Bar {int left,top,width,height;unsigned current,max;bool segmented=false;};
    std::array<Bar,5> bars{};
    unsigned bar_count=0;
    if(show_details){
        line(tr(L"ランク ",L"RANK ",L"动态等级 ",L"動態等級 ")+std::to_wstring(s.life.playperf));
        line(tr(L"パワー ",L"PWR ",L"火力 ",L"火力 ")+std::to_wstring(s.pickups.power)+L"  Lv "+std::to_wstring(s.pickups.level()));
        line(tr(L"フレーム ",L"FRAME ",L"帧数 ",L"幀數 ")+std::to_wstring(s.ticks));
        if(session)line(tr(L"シード ",L"SEED ",L"种子 ",L"種子 ")+std::to_wstring(session->seed()));
    }
    if(encounter.active){
        std::wstring title(info_label(encounter.label));
        if(encounter.phases>1)title+=L" P"+ratio(encounter.phase+1,encounter.phases);
        line(title);
        if(encounter.has_health){
            line(L"HP "+ratio(encounter.health,encounter.max_health));
            if(encounter.phase_cut_count){
                int left=0;
                for(unsigned i=0;i<=encounter.phase_cut_count;++i){
                    const int right=i==encounter.phase_cut_count?bar_width:int(bar_width*encounter.phase_cuts[i]/encounter.total_max_health);
                    text(L"P"+std::to_wstring(encounter.phases-i),1+(left+right-12)/2,y,14);
                    left=right;
                }
                y+=line_pitch;
            }
            bars[bar_count++]={0,y,bar_width,6,encounter.total_health,encounter.total_max_health,true};y+=8;
        }
        const bool show_protection=show_details&&encounter.protection.kind!=soew::EncounterProtectionKind::None;
        if(encounter.dying)line(tr(L"退場中",L"RETIRING",L"退场中",L"退場中"));
        else{
            const std::wstring_view status=encounter.status;
            if(!status.empty()&&(!show_protection||status==L"OPENING"||status==L"TRANSITION"))line(info_label(status));
            if(show_protection){
                SetTextColor(dc,RGB(255,224,151));
                for(const auto &value:soew::encounter_protection_lines(encounter.protection,language))
                    if(!value.empty())line(value);
                SetTextColor(dc,RGB(180,220,255));
            }
        }
        if(encounter.pending)line(tr(L"撃破判定待ち",L"DAMAGE GATE WAIT",L"等待击破判定",L"等待擊破判定"));
        if(encounter.has_timeout){
            const auto unit=soew::encounter_timer_unit(encounter.timeout_unit,language);
            line(tr(L"残り ",L"TIME ",L"剩余 ",L"剩餘 ")+std::to_wstring(encounter.timeout)+L" "+std::wstring(unit)+(encounter.timeout_pending?L" *":L""));
            if(encounter.timeout_pending)line(tr(L"時間切れ判定待ち",L"TIME GATE WAIT",L"等待超时判定",L"等待超時判定"));
        }else if(encounter.has_health&&!encounter.dying)line(tr(L"残り --",L"TIME --",L"剩余 --",L"剩餘 --"));
        // Shields and paired actors have their own meters, not segments of the boss bar.
        for(unsigned i=0;i<encounter.target_count;++i){
            const auto &target=encounter.targets[i];
            const int left=int(i%2)*63,top=y+int(i/2)*(line_pitch+4);
            text(std::wstring(info_label(target.label))+L" "+std::to_wstring(target.health),left+1,top,61);
            bars[bar_count++]={left,top+line_pitch,57,3,target.health,target.max_health};
        }
    }else if(session)line(tr(L"開始段階 ",L"START PHASE ",L"起始阶段 ",L"起始階段 ")+std::to_wstring(session->recipe.boss_phase));
    GdiFlush();
    for(unsigned i=0;i<bar_count;++i){
        const auto &bar=bars[i];
        const auto scale=[&](unsigned health){return int(bar.width*std::min(health,bar.max)/bar.max);};
        const int fill=bar.current?std::max(1,scale(bar.current)):0;
        for(int x=0;x<bar.width;++x){
            std::uint32_t color=x<fill?0x0000d8ffu:0x00303030u;
            if(bar.segmented)for(unsigned cut=0;cut<encounter.phase_cut_count;++cut)
                if(x==scale(encounter.phase_cuts[cut]))color=x<fill?0x00ffffffu:0x00707070u;
            for(int row=bar.top;row<bar.top+bar.height&&row<side;++row)pixels[row*side+bar.left+x]=color;
        }
    }
    for(int row=0;row<side;++row)std::copy_n(pixels+row*side,side,frame.pixels.begin()+(112+row)*640+448);
    SelectObject(dc,oldfont);SelectObject(dc,oldbmp);DeleteObject(font);DeleteObject(bitmap);DeleteDC(dc);
}
void rounded_box(Frame &frame,int left,int top,int right,int bottom,std::uint32_t color) {
    constexpr int radius=8;
    for(int y=top;y<=bottom;++y)for(int x=left;x<=right;++x){
        int cx=std::clamp(x,left+radius,right-radius),cy=std::clamp(y,top+radius,bottom-radius);
        if(x>=0&&x<640&&y>=0&&y<400&&(x-cx)*(x-cx)+(y-cy)*(y-cy)<=radius*radius)frame.pixels[y*640+x]=color;
    }
}
void shot_box(Frame &frame,int left,int top,int right,int bottom,std::uint32_t color){
    rounded_box(frame,left+8,top+8,right+8,bottom+8,color);
    rounded_box(frame,left,top,right,bottom,0);
}
void compose_shots(Frame &frame,const std::array<Frame,3> &portraits,const Menu &menu,std::uint32_t selected_color,std::uint32_t unselected_color) {
    const auto lang=menu.language;
    const bool en=lang==soew::Language::English;
    const auto put=[&](std::wstring_view line,int x,int y,std::uint32_t color,int width=640){display_text(frame,line,x,y,color,lang,width);};
    const auto& descriptions=en?jp::descriptions_en:jp::descriptions;
    const auto& extra_note=en?jp::extra_note_en:jp::extra_note;
    for(int i=0;i<3;++i) {
        const int x=(i==0?16:i==1?224:432)+(menu.shot==i?0:8);
        const int y=(i==1?224:128)+(menu.shot==i?0:8);
        const auto &pic=portraits[i];
        for(int row=0;row<pic.h;++row) for(int col=0;col<pic.w;++col) {
            // RMW checkerboard blackening approximates darken_pic_at's 0xAAAA/5555.
            frame.pixels[(y+row)*640+x+col]=(menu.shot!=i && ((row+col)%2==0))?0:pic.pixels[row*pic.w+col];
        }
    }
    // Source color indices 7 and12; actual RGB values are passed from TSELECT.
    for(int i=0;i<3;++i) {
        const int x=i==0?16:i==1?224:432,y=i==1?136:296;
        const auto color=menu.shot==i?selected_color:unselected_color;
        shot_box(frame,x,y,x+192,y+64,color);
        for(int row=0;row<3;++row)put(chinese_display(lang)?std::wstring_view(soew::text::shot_description[i][row](lang)):descriptions[i][row],x+8,y+8+row*16,color,176);
    }
    shot_box(frame,128,16,512,48,selected_color);
    put(chinese_display(lang)?std::wstring_view(soew::localized(lang,L"",L"",L"请选择灵梦的战斗方式",L"請選擇靈夢的戰鬥方式")):en?jp::choose_en:jp::choose,144,24,selected_color,368);
    if(menu.extra_mode){
        shot_box(frame,16,48,624,96,selected_color);
        if(chinese_display(lang)){
            put(soew::localized(lang,L"",L"",L"额外关卡的难度与初始资源固定。",L"額外關卡的難度與初始資源固定。"),32,56,0xffffff,592);
            put(soew::localized(lang,L"",L"",L"额外难度，三条命，一次灵击。",L"額外難度，三條命，一次靈擊。"),32,72,0xffffff,592);
        }else{put(extra_note[0],32,56,0xffffff);put(extra_note[1],32,72,0xffffff);}
    }
    const auto &clears=menu.extra_mode?menu.cleared_extra:menu.cleared_main;
    for(unsigned i=0;i<3;++i)if(clears[i])put(chinese_display(lang)?std::wstring_view(soew::localized(lang,L"",L"",L"☆☆ 已通关 ☆☆",L"☆☆ 已通關 ☆☆")):jp::cleared,i==0?16:i==1?224:432,112,0xffffff);
    if(menu.shot_confirmed)put(soew::localized(lang,L"ショットを選択しました",L"Shot selected",L"已选定射击类型",L"已選定射擊類型"),224,384,0xffffff);
}
void compose_options(Frame &frame,const th03::assets::BfntSheet &font,const Menu &menu,std::string_view warning) {
    namespace t=soew::text;
    const auto language=menu.language;
    constexpr std::uint32_t white=0xffffff,yellow=0xffff00;
    const auto fallback=[&](std::wstring_view text,int x,int y,std::uint32_t color){jp_text(frame,text,x,y,color);};
    const bool chinese=chinese_display(language);
    const auto width=[&](std::wstring_view text){return chinese?display_text_width(text,language):menu_text_width(text);};
    const auto draw=[&](std::wstring_view text,int x,int y,std::uint32_t color){
        if(chinese){display_text(frame,text,x+4,y+4,0,language);display_text(frame,text,x,y,color,language);}
        else{draw_menu_text(frame.pixels,font,text,x+4,y+4,0,fallback);draw_menu_text(frame.pixels,font,text,x,y,color,fallback);}
    };
    const auto hint=[&](std::wstring_view text,int y,std::uint32_t color){
        const int x=320-display_text_width(text,language)/2;
        display_text(frame,text,x+2,y+2,0,language);
        display_text(frame,text,x,y,color,language);
    };
    const std::wstring_view title=t::options_title(language);
    draw(title,320-width(title)/2,180,yellow);
    for(unsigned row=0;row<Menu::option_rows.size();++row)for(unsigned column=0;column<Menu::option_rows[row].size();++column){
        const auto item=Menu::option_rows[row][column];
        if(item==Menu::Option::Empty)continue;
        const auto color=menu.option()==item?white:yellow;
        const int x=24+int(column)*312,y=204+int(row)*22;
        const auto label=t::option_labels[static_cast<unsigned>(item)](language);
        if(!Menu::option_count(item,menu.chinese_available)){draw(label,x+(280-width(label))/2,y,color);continue;}
        const auto value=t::option_value(language,item,menu.option_value(item));
        draw(label,x,y,color);
        draw(value,x+280-width(value),y,color);
    }
    hint(t::options_controls(language),360,yellow);
    if(!warning.empty())hint(soew::settings_warning_text(language,warning),380,white);
    else if(menu.option()==Menu::Option::Music)hint(t::options_music_hint(language),380,yellow);
    else if(menu.option()==Menu::Option::Effects)hint(t::options_effects_hint(language),380,yellow);
    else if(menu.option()==Menu::Option::Language&&language!=menu.language_preference)hint(t::options_language_unavailable(language),380,yellow);
}
void compose_frontend(Frame &frame,const th03::assets::BfntSheet &font,const Menu &menu) {
    draw_menu(frame.pixels,font,menu);
}
Frame score_frame(const soew::Scores &scores,int rank,const th03::assets::BfntSheet &font,const std::array<soew::LogoCel,2> &logo,const std::array<std::uint32_t,16> &palette,std::uint8_t transparent,unsigned animation_frame,soew::Language lang) {
    const auto tr=[&](const wchar_t* ja,const wchar_t* en,const wchar_t* hans,const wchar_t* hant){return soew::localized(lang,ja,en,hans,hant);};
    Frame frame{640,400,std::vector<std::uint32_t>(640*400,0)};
    soew::score_logo_background(frame.pixels,logo,palette,transparent,animation_frame);
    if(chinese_display(lang)){
        display_text(frame,tr(L"",L"",L"最高分",L"最高分"),176,32,0x00ff00,lang);
        display_text(frame,rank==4?soew::text::extra(lang):soew::text::difficulty.at(rank)(lang),320,32,0x00ff00,lang);
    }else{
        draw_label(frame.pixels,font,"HISCORE",176,32,0x00ff00);
        draw_label(frame.pixels,font,rank==4?" EXTRA ":rank_names.at(rank),320,32,0x00ff00);
    }
    display_text(frame,tr(L"順位",L"Rank",L"名次",L"名次"),64,64,0x00ff00,lang);
    display_text(frame,tr(L"名前",L"Name",L"名字",L"名字"),104,64,0x00ff00,lang);
    display_text(frame,tr(L"得点",L"Score",L"得分",L"得分"),208,64,0x00ff00,lang);
    display_text(frame,tr(L"面",L"Stage",L"关卡",L"關卡"),352,64,0x00ff00,lang);
    display_text(frame,tr(L"型",L"Shot",L"机体",L"機體"),400,64,0x00ff00,lang);
    display_text(frame,tr(L"日付",L"Date",L"日期",L"日期"),448,64,0x00ff00,lang);
    for(int i=0;i<10;++i){
        const auto &e=scores.at(rank).entries[i];const int y=112+i*16;
        draw_label(frame.pixels,font,std::to_string(i+1),64,y,0xffffff);
        for(int j=0;j<6;++j)draw_glyph(frame.pixels,font,e.name[j],96+j*16,y,0xffffff);
        std::ostringstream number;number<<std::setw(8)<<std::setfill('0')<<e.score;
        draw_label(frame.pixels,font,number.str(),208,y,0xffffff);
        draw_label(frame.pixels,font,e.stage==127?"ALL":std::to_string(e.stage),352,y,0xffffff);
        draw_label(frame.pixels,font,std::string(1,char('A'+e.shot)),400,y,0xffffff);
        std::wostringstream date;date<<std::setw(4)<<std::setfill(L'0')<<e.year<<L'/'<<std::setw(2)<<unsigned(e.month)<<L'/'<<std::setw(2)<<unsigned(e.day);
        jp_text(frame,date.str(),448,y,0xffffff);
    }
    display_text(frame,tr(L"読み取り専用・キーを押すと戻ります",L"Read only. Press a key to return.",L"只读。按任意键返回。",L"唯讀。按任意鍵返回。"),160,320,0xffffff,lang);
    return frame;
}
template<std::size_t N>
void score_gaiji_line(Frame &frame,const th03::assets::BfntSheet &font,const std::array<std::uint8_t,N> &text,int x,int y,std::uint32_t color){
    for(const auto glyph:text){draw_glyph(frame.pixels,font,glyph,x,y,color);x+=16;}
}
void compose_native_board(Frame &frame,const th03::assets::BfntSheet &font,
                          const std::vector<soew::NativeScoreEntry> &stored,unsigned rank,soew::Language lang,
                          bool title_board,bool before_continue,const soew::NativeScoreEntry *pending,const soew::ScoreSignature &signature){
    namespace g=soew::score_gaiji;
    constexpr std::uint32_t white=0xffffff,green=0x00ff00;
    const auto tr=[&](const wchar_t* ja,const wchar_t* en,const wchar_t* hans,const wchar_t* hant){return soew::localized(lang,ja,en,hans,hant);};
    // MAIN.EXE uses TRAM cells for registration; OP.EXE moves the same columns
    // right for the title board and includes the selected difficulty.
    const int name_x=title_board?96:80,point_x=title_board?208:208,stage_x=title_board?352:352;
    const int first_y=title_board?112:96;
    if(chinese_display(lang)){
        display_text(frame,L"最高分",title_board?176:160,32,green,lang);
        display_text(frame,L"名字",name_x+16,64,green,lang);
        display_text(frame,L"得分",point_x+8,64,green,lang);
        display_text(frame,tr(L"",L"",L"关卡",L"關卡"),stage_x-24,64,green,lang);
    }else{
        score_gaiji_line(frame,font,g::title,title_board?176:160,32,green);
        score_gaiji_line(frame,font,g::name,name_x+16,64,green);
        score_gaiji_line(frame,font,g::point,point_x+8,64,green);
        score_gaiji_line(frame,font,g::stage,stage_x-24,64,green);
    }
    display_text(frame,rank==4?soew::text::extra(lang):soew::text::difficulty.at(rank)(lang),title_board?320:424,32,green,lang);
    if(title_board)display_text(frame,tr(L"ローカル記録",L"LOCAL RECORDS",L"本地记录",L"本機紀錄"),464,64,green,lang);
    const auto rows=soew::NativeScoreStore::display_rows(stored,pending);
    for(unsigned i=0;i<rows.size();++i){
        const auto &row=rows[i];const int y=first_y+int(i)*16;
        const auto color=row.signing?green:white;
        const auto draw=[&](unsigned g,int x){draw_glyph(frame.pixels,font,g,x,y,color);};
        const int single=title_board?72:48,ten=title_board?64:40;
        if(i==9){draw(g::zero+1,ten);draw(g::zero,ten+16);}else draw(g::zero+i+1,single);
        for(unsigned c=0;c<6;++c){
            const int x=name_x+int(c)*16;
            if(row.signing&&signature.position==c){
                for(int line=0;line<16;++line)std::fill_n(frame.pixels.begin()+(y+line)*640+x,16,green);
                draw_glyph(frame.pixels,font,signature.cells[c],x,y,0);
            }else if(row.signing)draw(signature.cells[c],x);
            else if(!row.entry)draw(g::bullet,x);
            else if(c<row.entry->name.size()){
                const auto ch=static_cast<unsigned char>(row.entry->name[c]);
                if(row.entry->gaiji_name)draw(ch,x);
                else if((ch>='A'&&ch<='Z')||(ch>='0'&&ch<='9')||ch==' ')draw(glyph_code(char(ch)),x);
                else jp_text(frame,std::wstring(1,wchar_t(ch)),x+4,y,color);
            }
        }
        const auto points=std::to_string(row.points%100000000u);
        if(row.points)draw_label(frame.pixels,font,points,point_x+(8-int(points.size()))*16,y,color);
        if(row.stage)draw(row.stage==soew::NativeScoreEntry::all_stages?g::all:g::zero+row.stage,stage_x);
        else jp_text(frame,L"-",stage_x+4,y,color);
        if(title_board)draw(row.entry?unsigned(glyph_code(char('A'+row.entry->shot_type))):unsigned(glyph_code('B')),408);
    }
    if(pending){
        for(unsigned r=0;r<g::alphabet.size();++r)for(unsigned c=0;c<g::alphabet[r].size();++c){
            const int x=name_x+int(c)*16,y=288+int(r)*16;
            if(r==signature.row&&c==signature.column){
                for(int line=0;line<16;++line)std::fill_n(frame.pixels.begin()+(y+line)*640+x,16,green);
                draw_glyph(frame.pixels,font,g::alphabet[r][c],x,y,0);
            }else draw_glyph(frame.pixels,font,g::alphabet[r][c],x,y,white);
        }
        display_text(frame,tr(L"方向：移動　Z：選択　X：削除　Esc：登録しない",L"Arrows: move   Z/Enter: select   X: erase   Esc: skip",L"方向键：移动  Z / Enter：选择  X：删除  Esc：跳过",L"方向鍵：移動  Z / Enter：選擇  X：刪除  Esc：略過"),48,352,white,lang);
    }else if(title_board)display_text(frame,tr(L"左右：難易度　その他のキー：戻る",L"Left/Right: difficulty   Other key: return",L"左右：切换难度  其他键：返回",L"左右：切換難度  其他鍵：返回"),72,336,white,lang);
    else display_text(frame,before_continue?tr(L"Z / Enter：コンティニューへ",L"Z / Enter: Continue prompt",L"Z / Enter：前往续关选项",L"Z / Enter：前往接關選項"):tr(L"Z / Enter / Esc：次へ",L"Z / Enter / Esc: next",L"Z / Enter / Esc：下一步",L"Z / Enter / Esc：下一步"),56,328,white,lang);
}
void compose_game_over(Frame &frame,const th03::assets::BfntSheet &font,unsigned n,soew::Language lang){
    namespace g=soew::score_gaiji;
    if(n>=g::fade_frames)soew::EncounterScene::apply_mima_tone(frame.pixels,50);
    if(n<2*g::fade_frames){
        const bool entering=n>=g::fade_frames;
        const auto half=int((n%g::fade_frames)/2);
        for(int col=0;col<24;++col){
            const auto cel=std::clamp(half-col/2,0,8);
            for(int y=16;y<384;y+=16)draw_glyph(frame.pixels,font,(entering?0xf1:0xcf)+cel,32+col*16,y,0);
        }
        return;
    }
    const auto pos=g::game_over_positions(n-2*g::fade_frames);
    const auto put=[&](int x,int y){
        if(chinese_display(lang)){
            const auto title=soew::text::game_over(lang);
            display_text(frame,title,x*8+(int(g::game_over.size())*16-display_text_width(title,lang))/2,y*16,0xffffff,lang);
        }else score_gaiji_line(frame,font,g::game_over,x*8,y*16,0xffffff);
    };
    put(18,pos.top);put(18,pos.bottom);put(pos.left,12);put(pos.right,12);
}
std::wstring cp932(std::string_view text){
    const int size=MultiByteToWideChar(932,MB_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),nullptr,0);
    if(!size)throw std::runtime_error("Invalid CP932 music comment");
    std::wstring result(size,L' ');MultiByteToWideChar(932,MB_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),result.data(),size);return result;
}
template<typename Bonus,std::size_t N>
void compose_clear_bonus(Frame &frame,const th03::assets::BfntSheet &font,const Bonus &b,
                         const std::array<std::wstring_view,N> &labels,int total_digits,soew::Language lang){
    namespace o=soew::stage_overlay;
    constexpr std::uint32_t white=0xffffff;
    const auto text=[&](std::wstring_view value,int col,int row){display_text(frame,value,col*o::cell_w,row*o::cell_h,white,lang);};
    const auto number=[&](std::uint32_t value,int col,int row,int digits){
        // overlay_uint_put prints the rightmost digits, skipping leading zeroes.
        do {
            --digits;
            draw_glyph(frame.pixels,font,o::digit_zero_gaiji+value%10,(col+digits*2)*o::cell_w,row*o::cell_h,white);
            value/=10;
        }while(value&&digits);
    };
    if(soew::is_chinese(lang))text(soew::localized(lang,L"",L"",L"通关奖励",L"通關獎勵"),o::title_column,o::title_row);
    else for(unsigned i=0;i<o::bonus_gaiji.size();++i)
        draw_glyph(frame.pixels,font,o::bonus_gaiji[i],(o::title_column+int(i)*2)*o::cell_w,o::title_row*o::cell_h,white);
    for(unsigned i=0;i<N;++i){
        const int row=o::first_row+int(i)*2;
        text(labels[i],o::label_column,row);
        number(b.rows[i],o::value_column,row,o::value_digits);
    }
    text(soew::localized(lang,L"点",L"Point",L"点",L"點"),o::point_column-(lang==soew::Language::English?1:0),o::formula_row);
    number(b.point_items,o::point_value_column,o::formula_row,o::point_digits);
    text(L"×",o::times_column,o::formula_row);
    number(b.subtotal*10,o::sum_column,o::formula_row,o::value_digits);
    text(L"＝",o::equals_column,o::formula_row);
    number(b.internal_bonus*10,o::result_column(total_digits),o::formula_row,total_digits);
}
void compose_curtain(Frame &frame,const th03::assets::BfntSheet &font,bool entering,unsigned frame_number){
    const unsigned half=frame_number/2;
    for(unsigned col=0;col<24;++col){
        const unsigned cel=std::clamp<int>(int(half)-int(col/2),0,8);
        for(unsigned y=16;y<384;y+=16)draw_glyph(frame.pixels,font,entering?0xf1+cel:0xcf+cel,32+int(col*16),int(y),0);
    }
}
void compose_stage_subtitle(Frame &frame,soew::Language lang,std::size_t stage,bool original_layout){
    namespace o=soew::stage_overlay;
    constexpr int y=o::subtitle_row*o::cell_h;
    constexpr std::uint32_t white=0xffffff;
    std::wstring text(o::subtitle(lang,stage));
    const auto layout=o::layout_subtitle(text,stage,original_layout,[lang](std::wstring_view value){return display_text_width(value,lang);});
    // Reserve the original two text cells without asking the host font to draw their wave.
    text[layout.wave_at]=L'\u3000';
    display_text(frame,text,layout.text_x,y,white,lang);
    o::paint_subtitle_wave(frame.pixels,frame.w,layout.wave_x,y,white);
}
void compose_transition(Frame &frame,const soew::EncounterScene &s,const th03::assets::BfntSheet &font,
                        soew::Language viewer=soew::Language::Japanese){
    namespace o=soew::stage_overlay;
    const auto lang=soew::gameplay_display_language(viewer,s.language);
    const bool en=lang==soew::Language::English,zh=soew::is_chinese(lang),hans=lang==soew::Language::ChineseSimplified;
    using P=soew::StageTransition::Phase;const auto phase=s.transition.phase;
    const bool regular_bonus=phase==P::BonusAck||phase==P::ExtraAck;
    const bool extra_bonus=s.extra_settlement.showing_bonus();
    if(regular_bonus||extra_bonus||phase==P::Leave){
        // bonus_title_put tones the graphics palette, not the white text RAM.
        soew::EncounterScene::apply_mima_tone(frame.pixels,62);
        constexpr std::array<std::wstring_view,6> regular_en={L"Difficulty",L"Stage",L"Bombs",L"Misses",L"Start bombs",L"Start lives"};
        constexpr std::array<std::wstring_view,4> extra_en={L"Clear",L"Misses",L"Bombs",L"Clear time"};
        if(regular_bonus)compose_clear_bonus(frame,font,s.transition.bonus,zh?(hans?o::regular_labels_hans:o::regular_labels_hant):en?regular_en:o::regular_labels,6,lang);
        else if(extra_bonus)compose_clear_bonus(frame,font,s.extra_settlement.bonus,zh?(hans?o::extra_labels_hans:o::extra_labels_hant):en?extra_en:o::extra_labels,7,lang);
    }
    if(phase==P::Leave||phase==P::Entry)compose_curtain(frame,font,phase==P::Entry,s.transition.frame);
    if(s.dialogue.phase==soew::StageDialogue::Phase::IntroAppearance){
        const auto amount=soew::StageDialogue::appearance_white_mix(unsigned(s.stage),s.dialogue.current,s.appearance_frame,s.reduced_effects);
        if(amount)for(int y=16;y<384;++y)for(int x=32;x<416;++x){
            auto &v=frame.pixels[y*640+x];std::uint32_t c=0;
            for(unsigned shift:{0u,8u,16u})c|=((((v>>shift)&255)*(255-amount)+255*amount)/255)<<shift;
            v=c;
        }
    }
    if(phase==P::Battle&&!s.dialogue.active()&&!s.life.game_over&&!s.preboss_started&&!s.transition.boss_music_frame&&s.transition.opening_caption&&s.ticks<o::opening_frames){
        constexpr int title_y=o::stage_row*o::cell_h;
        if(zh){
            const auto title=soew::stage_label(lang,s.stage);
            display_text(frame,title,o::playfield_center*o::cell_w-display_text_width(title,lang)/2,title_y,0xffff00,lang);
        }else if(s.stage==5){
            for(unsigned i=0;i<o::extra_gaiji.size();++i)
                draw_glyph(frame.pixels,font,o::extra_gaiji[i],(16+int(i)*2)*o::cell_w,title_y,0xffff00);
        }else{
            constexpr int stage_x=(o::playfield_center-6)*o::cell_w;
            for(unsigned i=0;i<o::stage_gaiji.size();++i)
                draw_glyph(frame.pixels,font,o::stage_gaiji[i],stage_x+int(i)*16,title_y,0xffff00);
            draw_glyph(frame.pixels,font,o::digit_zero_gaiji+s.stage+1,stage_x+int(o::stage_gaiji.size())*16,title_y,0xffff00);
        }
        compose_stage_subtitle(frame,lang,s.stage,!en&&!zh);
    }
    if(phase==P::Battle&&!s.dialogue.active()&&!s.life.game_over&&!s.paused){
        const auto start=s.transition.boss_music_frame;
        if((start||(!s.preboss_started&&s.transition.opening_caption))&&o::show_bgm(s.ticks-start.value_or(0))){
            constexpr int bgm_y=o::bgm_row*o::cell_h;
            draw_glyph(frame.pixels,font,o::note_gaiji,o::bgm_note_column*o::cell_w,bgm_y,0xffff00);
            constexpr std::array<int,12> titles={1,6,2,7,3,8,4,9,5,10,13,14};
            const auto i=s.stage*2+int(start.has_value());
            display_text(frame,(en||zh)?soew::music_title(titles[i],lang):o::bgm_titles[i],o::bgm_title_column*o::cell_w,bgm_y,0xffffff,lang,(o::playfield_right-o::bgm_title_column)*o::cell_w);
        }
    }
    if(phase==P::LoadWait||phase==P::Ready){for(int y=16;y<384;++y)std::fill(frame.pixels.begin()+y*640+32,frame.pixels.begin()+y*640+416,0);}
}
void compose_dialogue_text(Frame &frame,const soew::EncounterScene &scene,
                           soew::Language viewer=soew::Language::Japanese,const soew::ChineseDisplayPack *pack=nullptr){
    if(scene.stage==4&&scene.mima_delay)return; // The cursor has advanced, but the next box is not visible yet.
    const auto &d=scene.dialogue;if(d.phase!=soew::StageDialogue::Phase::Text&&d.phase!=soew::StageDialogue::Phase::KeyDelay)return;
    for(unsigned line=0;line<2;++line){
        const unsigned n=d.reveal.bytes(line);
        if(pack&&soew::is_chinese(viewer)){
            const auto record=d.current*2+line;
            const auto label=pack->line(viewer,soew::locale_text_assets.at(d.stage_id),record);
            const auto expanded=soew::expand_continues(label,d.continues_used);
            const auto visible=soew::chinese_text_prefix(expanded,int(n)*8);
            if(!visible.empty())display_text(frame,soew::wide_utf8(visible),112,336+int(line*16),0xffffff,viewer,288);
            continue;
        }
        const auto &raw=d.boxes.at(d.current).lines[line].text;
        // A two-byte source reveal can split a CP932 character in translated
        // or mixed-width records. Keep the byte cursor, defer that glyph only.
        unsigned end=0;while(end<n){unsigned c=std::to_integer<unsigned>(raw[end]);unsigned width=(c>=0x81&&c<=0x9f)||(c>=0xe0&&c<=0xfc)?2:1;if(end+width>n)break;end+=width;}
        if(end)jp_text(frame,cp932(std::string_view(reinterpret_cast<const char*>(raw.data()),end)),112,336+int(line*16),0xffffff);
    }
}
using MusicRoomText = std::array<std::array<std::wstring,20>,15>;
Frame music_frame(const Frame &background,const Menu &menu,const MusicRoomText &comments,const std::array<std::uint32_t,16> &palette,const soew::ChineseDisplayPack *pack,bool audio_active=false,bool midi_mode=false){
    const auto lang=menu.language;
    const auto tr=[&](const wchar_t* ja,const wchar_t* en,const wchar_t* hans,const wchar_t* hant){return soew::localized(lang,ja,en,hans,hant);};
    Frame frame=background;
    const auto put=[&](std::wstring_view text,int x,int y,std::uint32_t color,int width=640){display_text(frame,text,x,y,color,lang,width);};
    for(int i=0;i<15;++i){
        const auto label=L"NO."+std::to_wstring(i+1)+std::wstring(i<9?L"    ":L"   ")+std::wstring(soew::music_title(i,lang));
        put(label,16,96+i*16,palette[menu.music_selected==i?15:3],288);
    }
    put(tr(L"　　　　戻る",L"         Quit",L"　　　　返回",L"　　　　返回"),16,352,palette[menu.music_selected==16?15:3]);
    const auto track=menu.music_comment;
    if(pack&&soew::is_chinese(lang)){
        const auto line=[&](std::size_t row){return soew::wide_utf8(pack->line(lang,"MUSIC.TXT",std::size_t(track)*20+row));};
        put(line(0),160,64,palette[15],320);
        for(int row=1;row<20;++row)put(line(row),304,80+(row-1)*16,palette[13],320);
    }else{
        if(lang==soew::Language::Japanese)jp_text(frame,comments[track][0],160,64,palette[15]);
        else put(L"No. "+std::to_wstring(track+1)+L"   "+std::wstring(soew::music_title(track,lang)),160,64,palette[15]);
        for(int row=1;row<20;++row){
            const auto& line=comments[track][row];
            const auto en_label=line.find(L"＜ＦＭ　Ｖｅｒｓｉｏｎ＞")!=std::wstring::npos?L"<FM Version>":line.find(L"＜ＭＩＤＩ　Ｖｅｒｓｉｏｎ＞")!=std::wstring::npos?L"<MIDI Version>":nullptr;
            jp_text(frame,lang==soew::Language::English&&en_label?std::wstring_view(en_label):std::wstring_view(line),304,80+(row-1)*16,palette[13]);
        }
    }
    put(midi_mode?tr(L"MIDI音源・F5でFM切替",L"MIDI audio | F5: change to FM",L"MIDI 音源 | F5：切换到 FM",L"MIDI 音源 | F5：切換到 FM"):tr(L"FM音源・F5でMIDI切替",L"FM audio | F5: change to MIDI",L"FM 音源 | F5：切换到 MIDI",L"FM 音源 | F5：切換到 MIDI"),16,384,0xffffff,288);
    put(audio_active?tr(L"F4で選択曲試聴・同じ曲で停止",L"F4: play track / stop current track",L"F4：试听所选曲目／再次按下停止",L"F4：試聽所選曲目／再次按下停止"):tr(L"F4で選択曲試聴・初期は無音",L"F4: play selected track",L"F4：试听所选曲目",L"F4：試聽所選曲目"),304,384,0xffffff,320);
    return frame;
}
unsigned test_chinese_ascii(){
    using soew::Language;
    constexpr auto hant=Language::ChineseTraditional,hans=Language::ChineseSimplified;
    constexpr std::uint32_t ink=0xffffff;
    unsigned checks=0;
    const auto check=[&](bool ok){++checks;if(!ok)throw std::runtime_error("Chinese ASCII rendering test failed");};
    const auto render=[&](std::wstring_view text,Language lang,int width=640){
        Frame sample{640,16,std::vector<std::uint32_t>(640*16,0)};
        display_text(sample,text,0,0,ink,lang,width);
        return sample.pixels;
    };
    for(wchar_t c=L' ';c<=L'~';++c){
        const std::wstring_view text(&c,1);
        check(chinese_font(hant).supports(text)&&chinese_font(hans).supports(text));
        const auto glyph=render(text,hant);
        check(glyph==render(text,hans));
        check((std::find(glyph.begin(),glyph.end(),ink)!=glyph.end())==(c!=L' '));
    }
    for(const std::wstring_view label:{L"FM",L"MIDI",L"NO.10",L"Magic"}){
        const int width=int(label.size())*8;
        check(display_text_width(label,hant)==width);
        check(render(label,hant,width)==render(label,hant));
        check(render(label,hant,width-1)==render(label.substr(0,label.size()-1),hant));
    }
    check(render(L"NO.10",hant)!=render(L"NC.10",hant));
    check(render(L"NO.10",hant)!=render(L"N0.10",hant));
    constexpr std::wstring_view mixed=L"NO.10 ＜戀色 Magic＞";
    check(display_text_width(mixed,hant)==160);
    check(fit_display_text(mixed,143,hant)==L"NO.10 ＜戀色 Magi");
    return checks;
}
#include "practice_retry_host.hpp"
struct TitleDemoHost {
    using Run=soew::TitleDemoRun<soew::EncounterScene>;
    struct Pcm { AudioPreview::Prepared stage,title; };
    soew::TitleDemoController controller{true};
    PresentationClock clock;
    std::bitset<256> consumed_keys;
    bool reconcile_keys=false,menu_loop=false,stage_audio_published=false;
    int stage_track=-1;
    unsigned score_frame=0;
    unsigned scroll_phase=soew::scroll_subpixels;
    soew::MotionFrame motion_before;
    std::shared_ptr<std::atomic_bool> cancel;
    std::optional<std::future<Run>> stage_load;
    std::optional<std::future<Pcm>> pcm_load;
    std::optional<Run> ready_scene;
    AudioPreview::Prepared title_pcm;
};
struct App {
    RetryHostProbe retry_probe;
    std::array<Frame, 3> frames;
    std::optional<soew::ChineseDisplayPack> chinese_text;
    std::array<Frame, 3> portraits;
    std::array<Frame, 3> opening_strips;
    Opening opening;
    PresentationClock clock;
    bool focused=false,minimized=false,resizing=false;
    bool timed_test=false,score_timed_test=false;
    ULONGLONG test_started=0;
    unsigned timer_callbacks=0;
    unsigned pause_frame=0,test_phase=0;
    std::uint32_t selected_color=0xffffff,unselected_color=0x777777;
    th03::assets::BfntSheet font;
    Menu menu;
    fs::path assets_root;
    std::optional<Frame> stage_intro_art;
    soew::StageIntro stage_intro;
    std::optional<soew::EncounterScene> stage_scene;
    std::optional<soew::EndingPlayer> ending_player;
    std::optional<soew::EndingSurface> ending_surface;
    bool stage4_trial=false,stage5_trial=false,stage5_mainline=false,extra_trial=false,extra_candidate=false;
    unsigned extra_shot=1;
    std::optional<soew::NativeScoreStore> native_scores;
    enum class ScoreReview { None, GameOver, BeforeContinue, Terminal };
    ScoreReview score_review=ScoreReview::None;
    std::optional<soew::NativeScoreEntry> pending_score;
    soew::ScoreSignature score_signature;
    unsigned game_over_frame=0,review_rank=0;
    bool score_review_armed=false,score_release=false,death_presented=false;
    soew::ShotInput z_input;
    soew::PracticeMenu practice_menu;
    soew::PracticePanel practice_panel;
    soew::PracticeAssist practice_assist;
    bool run_assisted=false;
    std::optional<soew::PracticeStore> practice_store;
    std::optional<soew::PracticeBuilder> practice_builder;
    std::optional<std::future<soew::PracticeBuilder>> practice_load;
    bool practice_retry=false,practice_retry_release=false,last_paint_practice=false;
    bool practice_audio_started=false;
    int practice_audio_track=-1;
    bool practice_load_cancelled=false;
    std::shared_ptr<std::atomic_bool> practice_audio_cancel;
    unsigned practice_bgm_measure=0;
    std::optional<soew::PracticeSession> practice_session;
    bool practice_visible=false,practice_result=false,practice_escape=false;
    int practice_choice=0,pause_choice=0;
    fs::path replay_dir;
    std::optional<soew::NativeReplayRecorder> recorder;
    RecordedHostTiming recording_timing;
    std::unique_ptr<soew::HostPerf> host_perf;
    HostTimingSample pending_replay_timing{};
    std::optional<soew::ReplaySlowdownStats> replay_details_timing;
    std::optional<soew::NativeReplay> replay_playback;
    std::size_t replay_cursor=0;
    struct ReplaySeek {
        int target;
        std::size_t boundary,cursor=0;
        ULONGLONG started;
        soew::NativeReplay run;
        std::unique_ptr<soew::EncounterScene> scene;
        soew::ReplaySession session;
    };
    std::optional<ReplaySeek> replay_seek;
    std::optional<Menu> replay_menu_restore;
    std::optional<soew::ReplaySession> replay_session;
    soew::ReplayMenu replay_menu;
    soew::ReplayMenuCatalog replay_catalog;
    std::array<soew::ReplaySlotEntry,100> replay_slots;
    std::vector<soew::ReplaySlotEntry> replay_legacy;
    std::optional<soew::ReplaySlotEntry> replay_confirm;
    std::optional<soew::NativeReplay> replay_pending;
    bool replay_recording=true,replay_return_browser=false;
    soew::StaffMusicClock staff_music;
    bool staff_song_started=false;
    ULONGLONG staff_music_poll=0;
    std::uint64_t staff_song_generation=0;
    ULONGLONG practice_started=0;
    std::optional<soew::SettingsStore> settings;
    bool settings_enabled=false;
    bool stage_entry_release=false,run_pause_menu=false;
    int run_entry_stage=0;
    bool run_entry_stage4_trial=false;
    std::string settings_warning;
    fs::path evidence_dir;
    PresentationClock stage_clock; // host test cadence, NOT verified original scheduler
    unsigned scroll_phase=soew::scroll_subpixels;
    soew::MotionFrame motion_before;
    unsigned motion_stride=1;
    bool display_frames_active=false;
    bool paced_paint_due=false;
    soew::LockedGameClock locked_clock;
    HostTimingSample locked_timing{};
    soew::FrameRateMeter frame_rate;
    AudioPreview audio;
    std::array<std::vector<std::byte>,15> songs,midi_songs;
    std::vector<std::byte> soundfont;
    bool midi_mode=false;
    std::vector<std::byte> effect_bank;
    int loaded_track=0;
    bool title_audio_started=false;
    unsigned title_effect_phase=0;
    bool fm_enabled=false;
    int playing_track=-1;
    bool gameplay_audio_enabled=false;
    int gameplay_audio_attempted=-1;
    unsigned gameplay_audio_starts=0;
    // Workers borrow read-only song/font data above. Futures must join before those arrays are destroyed.
    std::optional<std::future<AudioPreview::Prepared>> practice_audio_load,gameplay_audio_load;
    std::optional<TitleDemoHost> title_demo;
    std::shared_ptr<std::atomic_bool> gameplay_audio_cancel;
    unsigned gameplay_audio_generation=0,gameplay_audio_load_generation=0;
    int gameplay_audio_load_track=-1;
    bool audio_test_silent=false;
    soew::LocalizedText audio_status{L"",L""};
    Frame music_background;
    std::array<MusicRoomText,2> music_comments;
    bool legacy_content=false;
    const MusicRoomText& current_music_comments() const { return music_comments[menu.language==soew::Language::English]; }
    std::array<std::uint32_t,16> music_palette{};
    std::optional<soew::Scores> scores;
    bool scores_visible=false,native_board_visible=false;
    int score_rank=1;
    std::array<soew::LogoCel,2> score_logo;
    std::array<std::uint32_t,16> score_palette{};
    std::uint8_t score_transparent=0;
    soew::ScoreAnimation score_animation;
    PresentationClock score_clock;
    std::string score_error;
    Frame composed;
    soew::WindowPresenter presenter;
    bool menu_visible = true;
    std::wstring notice;
    const wchar_t* tr(const wchar_t* ja,const wchar_t* en,const wchar_t* hans=nullptr,const wchar_t* hant=nullptr) const { return soew::localized(menu.language,ja,en,hans,hant); }
    int scene = 0;
    bool crt = false,window_fullscreen=false;
    WINDOWPLACEMENT windowed_placement{};
    LONG_PTR windowed_style=0;
    soew::DialogueHold dialogue_hold;
    soew::MimaDialogueTap mima_dialogue_tap;
    std::optional<std::uint32_t> last_practice_seed;
    bool smoke = false;
    bool async_practice_test=false,async_exit_test=false;
    int async_practice_step=0;
    unsigned retry_prior_starts=0;
    std::uint64_t retry_start_hash=0;
    std::uint32_t retry_prior_seed=0;
    ULONGLONG async_practice_started=0,async_practice_phase_started=0;
    unsigned paints = 0, sizes = 0, keys = 0, checks = 0;
    bool failed = false;
    void check(bool ok, int line=__builtin_LINE()) { ++checks; failed |= !ok; if(!ok)std::cerr<<"Window check "<<checks<<" failed at line "<<line<<'\n'; }
};
using HostPerf=soew::HostPerf;
HostPerf::Phase host_perf_phase(const App &app,bool enabled){
    using P=HostPerf::Phase;
    if(!enabled)return P::Paused;
    if(app.practice_builder||app.practice_load||app.practice_audio_load||app.replay_seek)return P::Loading;
    if(app.ending_player)return P::Ending;
    if(app.opening.active){
        const auto f=app.opening.frame;
        return f<Opening::door_end?P::OpeningDoor:f<Opening::hold_end?P::OpeningHold:f<Opening::flash_end?P::OpeningFlash:P::OpeningFade;
    }
    if(!app.stage_scene)return app.menu_visible?P::Title:P::Other;
    const auto &s=*app.stage_scene;
    if(s.paused||app.replay_menu.active()||app.practice_result)return P::Paused;
    if(app.stage_intro.active())return P::Loading;
    if(s.stage==3){
        if(s.marisa_enabled)return P::Stage4Marisa;
        if(s.dialogue.active())return P::Stage4Dialogue;
        if(s.runtime.scroll.done)return P::Stage4MapEnd;
        if(s.midboss4.entries==2)return s.midboss4.active?P::Stage4SecondMidboss:P::Stage4FormationTail;
    }
    return s.dialogue.active()?P::Dialogue:P::Gameplay;
}
void host_perf_resources(App &app){
    if(!app.host_perf||!app.host_perf->resource_due())return;
    HostPerf::Buffers b;
    const auto size=[](const auto &v){return HostPerf::BufferSize{v.size(),v.capacity()};};
    if(app.recorder)b.replay_events=size(app.recorder->replay().events);
    if(app.stage_scene){
        const auto &s=*app.stage_scene;
        b.hit_sparks=size(s.shots.hit_sparks);b.hit_score_adds=size(s.shots.hit_score_lowword_adds);
        b.bomb_sounds=size(s.bomb.sounds);b.pickup_sounds=size(s.pickups.sounds);b.enemy_kills=size(s.kills_recorded);
    }
    app.host_perf->resources(b);
}
Frame opening_frame(const App &app) {
    const HostPerf::Span span(app.host_perf.get(),HostPerf::Metric::OpeningFrame);
    Frame output=app.frames[1];const auto f=app.opening.frame;
    if(f<Opening::door_end){const int width=app.opening.exposed_columns();for(int y=0;y<400;++y)for(int x=0;x<640;++x)if(x<320-width||x>=320+width)output.pixels[y*640+x]=0;}
    if(f>=Opening::hold_end){
        const auto strip=std::min(2u,(f-Opening::hold_end)/5);
        std::copy(app.opening_strips[strip].pixels.begin(),app.opening_strips[strip].pixels.end(),output.pixels.begin());
    }
    if(f>=Opening::flash_end){
        Frame title=app.frames[0];compose_frontend(title,app.font,app.menu);
        const unsigned alpha=app.opening.title_alpha();
        for(std::size_t i=0;i<output.pixels.size();++i){std::uint32_t color=0;for(unsigned shift:{0u,8u,16u}){const auto a=(output.pixels[i]>>shift)&255,b=(title.pixels[i]>>shift)&255;color|=((a*(255-alpha)+b*alpha)/255)<<shift;}output.pixels[i]=color;}
    }
    return output;
}
void save_settings(App &app){if(!app.settings_enabled||!app.settings||app.replay_playback)return;if(!app.settings->save(soew::Settings::from(app.menu))){app.settings_warning=app.settings->warning;std::cerr<<"Settings not saved: "<<app.settings_warning<<'\n';}else app.settings_warning=app.settings->warning;}
void audio_error(App &app){std::cerr<<"Audio preview error: "<<app.audio.error()<<'\n';app.audio_status={L"音声の出力を停止しました",L"Audio error; output stopped"};app.audio.stop();app.fm_enabled=false;}
void cancel_gameplay_audio(App &app){
    ++app.gameplay_audio_generation;
    if(app.gameplay_audio_cancel)app.gameplay_audio_cancel->store(true,std::memory_order_relaxed);
}
void drain_gameplay_audio(App &app){
    if(app.gameplay_audio_load&&app.gameplay_audio_load->wait_for(std::chrono::seconds(0))==std::future_status::ready){
        try{app.gameplay_audio_load->get();}catch(const std::exception&){}
        app.gameplay_audio_load.reset();app.gameplay_audio_cancel.reset();
    }
}
void start_preview(App &app,int track,bool effect_only=false,unsigned start_measure=0){
    cancel_gameplay_audio(app);if(!app.smoke&&app.stage_scene)app.gameplay_audio_attempted=-1;
    app.playing_track=-1;if(track>=0&&track<15)app.loaded_track=track;
    if(track<0||track>=static_cast<int>(app.songs.size())){app.audio.stop();app.audio_status={L"試聴を停止しました",L"Preview stopped"};return;}
    const auto music=app.menu.music;
    app.midi_mode=music==2;
    const HostPerf::Span span(app.host_perf.get(),HostPerf::Metric::AudioPublish);
    try{const auto &song=app.midi_mode?app.midi_songs.at(track):app.songs.at(track);
        if(song.empty()||(app.midi_mode&&app.soundfont.empty())){app.audio_status={L"音源が見つからないため試聴できません",L"Preview unavailable: audio source missing"};app.audio.stop();return;}
        if(!app.audio.start(song,app.audio_test_silent,WAVE_MAPPER,app.midi_mode?std::span<const std::byte>(app.soundfont):std::span<const std::byte>{},app.effect_bank,app.songs.at(track),music==0||effect_only,start_measure)){audio_error(app);return;}
        app.playing_track=music==0||effect_only?-1:track;
        app.fm_enabled=music!=0&&!effect_only;
        const auto file=std::string(soew::music_files.at(track))+(app.midi_mode?"MD":"");
        app.audio_status=music==0?soew::LocalizedText{L"音楽・効果音なし",L"Music and sound effects off"}:soew::LocalizedText{L"",L""};
        std::cout<<"preview_track="<<track<<" file="<<file<<" song_fnv="<<app.audio.song_hash()<<" prefill_pcm_fnv="<<app.audio.pcm_hash()<<'\n';
    }catch(const std::exception&){app.audio.stop();app.fm_enabled=false;app.audio_status={L"試聴できませんでした",L"Could not play preview"};}
}
int scene_music_track(const soew::EncounterScene &s){
    return s.sigma_music?14:s.mima_music?10:s.stones_enabled?8:s.stage==1&&(s.meira_appearance||s.meira_enabled)?7:soew::gameplay_music_track(s.stage,s.preboss_started,s.rika_enabled,s.marisa_music);
}
using DemoPhase=soew::TitleDemoPhase;
using DemoOutcome=soew::TitleDemoOutcome;

bool title_demo_active(const App &app){return app.title_demo&&app.title_demo->controller.phase()!=DemoPhase::Idle;}
bool live_assist_controls(const App &app){
    return app.stage_scene&&!app.replay_playback&&!title_demo_active(app)&&!app.stage_scene->paused&&
        !app.stage_scene->life.game_over&&!app.stage_scene->continue_prompt()&&!app.stage_scene->extra_settlement.terminal()&&
        !app.stage_scene->extra_settlement.showing_bonus()&&!app.stage_scene->stage5_result&&
        !app.stage_intro.active()&&!app.ending_player&&app.score_review==App::ScoreReview::None&&!app.score_release&&
        !app.run_pause_menu&&!app.practice_result&&!app.practice_escape&&!app.practice_visible&&
        !app.practice_builder&&!app.practice_load&&!app.practice_audio_load&&!app.replay_menu.active();
}
void apply_live_assists(App &app){
    app.stage_scene->practice_assist.tools=app.practice_assist.tools;
    if(app.practice_assist.tools){
        // Replay events carry inputs, not live simulation modifiers.
        app.run_assisted=true;
        app.recorder.reset();
    }
}
bool title_demo_foreground(HWND hwnd,const App &app){
    return app.focused&&GetForegroundWindow()==hwnd&&!app.minimized&&!app.resizing&&IsWindowEnabled(hwnd)&&!app.title_demo->menu_loop;
}
bool title_demo_idle_screen(const App &app){
    return app.menu_visible&&!app.opening.active&&!app.menu.options&&!app.menu.music_room&&!app.menu.shot_screen&&
        !app.stage_scene&&!app.ending_player&&!app.scores_visible&&app.score_review==App::ScoreReview::None&&!app.score_release&&
        !app.practice_visible&&!app.practice_builder&&!app.practice_load&&!app.practice_audio_load&&
        !app.replay_menu.active()&&!app.replay_seek&&!app.gameplay_audio_load;
}
TitleDemoHost::Run no_title_demo_scene(const soew::TitleDemoSpec &){throw std::logic_error("Title demo loaded out of order");}
void verify_title_demo_recording(const soew::TitleDemoSpec &spec,std::span<const std::byte> bytes){
    constexpr std::array<std::string_view,3> hashes={
        "420233da2d030226471d27b1411a791713fb5313bb28a8f67c14b3285dc3d58f",
        "21eb02164011ad513c53c849ef9e247c95c8acc0fc3f2a87da4f4d7f3c5f7dda",
        "d11be53ea58633b3e3221d8a002bfd5ce30c1c89a052366a840e08d4d933a272"};
    const auto digest=soew::replay_slots_detail::replay_digest(
        {reinterpret_cast<const std::uint8_t*>(bytes.data()),bytes.size()});
    if(!digest||!spec.number||spec.number>hashes.size())throw std::runtime_error("Could not identify title demo recording");
    constexpr std::string_view hex="0123456789abcdef";
    const auto expected=hashes[spec.number-1];
    for(std::size_t i=0;i<digest->size();++i)
        if(expected[2*i]!=hex[(*digest)[i]>>4]||expected[2*i+1]!=hex[(*digest)[i]&15])
            throw std::runtime_error("Unverified title demo recording");
}
void title_demo_activity(App &app){
    if(app.title_demo&&app.title_demo->controller.phase()==DemoPhase::Idle)
        app.title_demo->controller.update(false,false,no_title_demo_scene);
}
void cancel_title_demo_work(TitleDemoHost &demo){
    if(demo.cancel)demo.cancel->store(true,std::memory_order_relaxed);
    // These futures must not be destroyed while running: async destruction joins.
    demo.ready_scene.reset();
}
void publish_title_demo_audio(App &app,AudioPreview::Prepared pcm,int track){
    const HostPerf::Span span(app.host_perf.get(),HostPerf::Metric::AudioPublish);
    if(!app.audio.publish(std::move(pcm),WAVE_MAPPER,false,app.audio.active()))throw std::runtime_error(app.audio.error());
    app.loaded_track=app.playing_track=track;
    app.midi_mode=app.menu.music==2;app.fm_enabled=true;app.audio_status={L"",L""};
}
void restore_title_demo_audio(App &app){
    auto &demo=*app.title_demo;
    if(demo.stage_audio_published){
        demo.stage_audio_published=false;
        try{publish_title_demo_audio(app,std::move(demo.title_pcm),0);}
        catch(const std::exception &e){std::cerr<<"Title demo title audio restore: "<<e.what()<<'\n';start_preview(app,0);}
    }else if(app.menu.music!=0&&(app.playing_track!=0||!app.audio.active())){
        // The music room may have left a different song playing before idle.
        start_preview(app,0);
    }
    demo.title_pcm={};
}
void release_title_demo_menu(App &app,bool focused,std::uint64_t now){
    auto &demo=*app.title_demo;
    if(demo.controller.phase()!=DemoPhase::Rearm||!focused||demo.consumed_keys.any())return;
    demo.controller.update(true,false,no_title_demo_scene);
    demo.clock.reset(now);
    app.menu.update(0);
}
bool title_demo_plain_key(WPARAM key){
    if(key>=256)return false;
    switch(key){
    case VK_SHIFT:case VK_LSHIFT:case VK_RSHIFT:
    case VK_CONTROL:case VK_LCONTROL:case VK_RCONTROL:
    case VK_MENU:case VK_LMENU:case VK_RMENU:
    case VK_LWIN:case VK_RWIN:case VK_APPS:case VK_F10:case VK_SNAPSHOT:return false;
    default:return !(GetKeyState(VK_MENU)&0x8000)&&!(GetKeyState(VK_LWIN)&0x8000)&&!(GetKeyState(VK_RWIN)&0x8000)&&
        !(key==VK_ESCAPE&&(GetKeyState(VK_CONTROL)&0x8000));
    }
}
bool title_demo_keydown(HWND hwnd,App &app,WPARAM key,bool repeat){
    if(!app.title_demo||!title_demo_plain_key(key)||!title_demo_foreground(hwnd,app))return false;
    auto &demo=*app.title_demo;
    const auto phase=demo.controller.phase();
    if(phase==DemoPhase::Idle){title_demo_activity(app);return false;}
    demo.consumed_keys.set(key);
    if(repeat||phase==DemoPhase::Rearm)return true;
    // Handle key-down in the message itself; a worker/timer must never get the
    // last playback step before an already-dispatched cancel gets observed.
    if(phase==DemoPhase::Score&&!demo.score_frame)demo.controller.update(true,false,no_title_demo_scene);
    demo.controller.update(true,true,no_title_demo_scene);
    if(demo.controller.phase()==DemoPhase::Rearm){
        cancel_title_demo_work(demo);restore_title_demo_audio(app);
        demo.clock.reset(host_ms());app.menu.allowed=false;
        InvalidateRect(hwnd,nullptr,FALSE);
    }
    return true;
}
void begin_title_demo_pcm(App &app){
    auto &demo=*app.title_demo;
    const bool midi=app.menu.music==2,silent=app.audio_test_silent;
    const int track=demo.stage_track;
    const std::span<const std::byte> stage=midi?app.midi_songs.at(track):app.songs.at(track),
        title=midi?app.midi_songs[0]:app.songs[0],font=midi?std::span<const std::byte>(app.soundfont):std::span<const std::byte>{},
        effects=app.effect_bank,stage_carrier=app.songs.at(track),title_carrier=app.songs[0];
    const auto cancel=demo.cancel;
    demo.pcm_load.emplace(std::async(std::launch::async,[=]{
        TitleDemoHost::Pcm pcm;
        if(midi&&font.empty())throw std::runtime_error("Title demo MIDI SoundFont unavailable");
        pcm.stage=AudioPreview::prepare(stage,silent,font,effects,stage_carrier,false,0,cancel.get());
        if(pcm.stage)pcm.title=AudioPreview::prepare(title,silent,font,effects,title_carrier,false,0,cancel.get());
        return pcm;
    }));
}

// No normal-game stage, input sampler, recorder or score-review state is used here.
bool advance_title_demo(HWND hwnd,App &app,std::uint64_t now){
    if(!app.title_demo)return false;
    auto &demo=*app.title_demo;
    const bool focused=title_demo_foreground(hwnd,app);
    auto phase=demo.controller.phase();
    auto ready=[](const auto &future){return future&&future->wait_for(std::chrono::seconds(0))==std::future_status::ready;};
    if(phase!=DemoPhase::Load){
        auto drain=[&](auto &future){if(ready(future)){try{future->get();}catch(const std::exception &){}future.reset();}};
        drain(demo.stage_load);drain(demo.pcm_load);
        if(!demo.stage_load&&!demo.pcm_load)demo.cancel.reset();
    }
    if(focused&&demo.reconcile_keys){
        for(unsigned key=0;key<demo.consumed_keys.size();++key)
            if(demo.consumed_keys.test(key)&&!(GetAsyncKeyState(int(key))&0x8000))demo.consumed_keys.reset(key);
        demo.reconcile_keys=demo.consumed_keys.any();
    }
    if(phase==DemoPhase::Rearm){
        release_title_demo_menu(app,focused,now);
        return true;
    }
    if(phase==DemoPhase::Idle){
        if(!focused||!title_demo_idle_screen(app)||demo.stage_load||demo.pcm_load){
            title_demo_activity(app);demo.clock.reset(now);return false;
        }
        BYTE keys[256]{};
        const bool idle=GetKeyboardState(keys)&&std::none_of(std::begin(keys)+VK_BACK,std::end(keys),[](BYTE state){return (state&0x80)!=0;});
        if(!idle){title_demo_activity(app);demo.clock.reset(now);return false;}
        for(unsigned steps=demo.clock.advance(now,true);steps&&demo.controller.phase()==DemoPhase::Idle;--steps)
            demo.controller.update(true,false,no_title_demo_scene);
        phase=demo.controller.phase();
        if(phase==DemoPhase::Idle)return false;
        app.menu.allowed=false;
    }
    if(!focused){demo.clock.reset(now);return true;}
    if(phase==DemoPhase::Load){
        auto load=[&](const soew::TitleDemoSpec &){return std::move(*demo.ready_scene);};
        try{
            if(!demo.stage_load&&!demo.pcm_load&&!demo.ready_scene){
                const auto root=app.assets_root;
                const auto spec=*demo.controller.current();
                const auto language=soew::simulation_language(app.menu.language);
                demo.cancel=std::make_shared<std::atomic_bool>(false);
                const auto cancel=demo.cancel;
                demo.stage_load.emplace(std::async(std::launch::async,[root,spec,language,cancel]{
                    if(cancel->load(std::memory_order_relaxed))return TitleDemoHost::Run{};
                    return soew::load_title_demo(root,spec,language,verify_title_demo_recording);
                }));
            }
            if(ready(demo.stage_load)){
                auto future=std::move(*demo.stage_load);demo.stage_load.reset();
                demo.ready_scene.emplace(future.get());
                if(!demo.ready_scene->scene)throw std::runtime_error("Title demo scene unavailable");
                demo.stage_track=scene_music_track(*demo.ready_scene->scene);
                if(app.menu.music!=0)begin_title_demo_pcm(app);
            }
            if(!demo.ready_scene||(demo.pcm_load&&!ready(demo.pcm_load)))return true;
            if(demo.pcm_load){
                auto future=std::move(*demo.pcm_load);demo.pcm_load.reset();auto pcm=future.get();
                if(!pcm.stage)throw std::runtime_error(pcm.stage.error());
                if(!pcm.title)throw std::runtime_error(pcm.title.error());
                demo.title_pcm=std::move(pcm.title);demo.stage_audio_published=true;
                publish_title_demo_audio(app,std::move(pcm.stage),demo.stage_track);
                auto &scene=*demo.ready_scene->scene;
                scene.sound.enabled=!app.effect_bank.empty();
                scene.sound_output=[&app](unsigned id){if(app.audio.active()&&!app.audio.effect(id))audio_error(app);};
            }
            demo.controller.update(true,false,load);
            demo.ready_scene.reset();demo.clock.reset(now);demo.motion_before.stage=-1;demo.scroll_phase=soew::scroll_subpixels;
        }catch(const std::exception &e){
            demo.controller.update(true,false,[&](const soew::TitleDemoSpec &)->TitleDemoHost::Run{throw std::runtime_error(e.what());});
        }
    }else{
        const auto *scene=demo.controller.scene();
        const auto pace=scene?scene->presentation_slowdown():1;
        const auto steps=app.display_frames_active?app.locked_clock.advance(pace):demo.clock.advance(now,true,pace);
        if(app.display_frames_active)demo.scroll_phase=soew::display_subframe(demo.scroll_phase,app.locked_clock.subframe(pace,soew::scroll_subpixels),steps);
        const HostPerf::Span span(steps&&scene?app.host_perf.get():nullptr,HostPerf::Metric::Simulation);
        for(unsigned i=0;i<steps&&demo.controller.phase()==phase;++i){
            if(scene&&app.display_frames_active&&app.menu.smooth_background)scene->capture_motion(demo.motion_before);
            demo.controller.update(true,false,no_title_demo_scene);
            if(phase==DemoPhase::Score)++demo.score_frame;
        }
        if(steps||app.display_frames_active)InvalidateRect(hwnd,nullptr,FALSE);
    }
    if(demo.controller.phase()!=phase){
        if(demo.controller.phase()==DemoPhase::Score){
            demo.score_frame=0;restore_title_demo_audio(app);demo.clock.reset(now);
        }else if(demo.controller.phase()==DemoPhase::Rearm){
            if(demo.controller.outcome()==DemoOutcome::Failed)std::cerr<<"Title demo disabled: "<<demo.controller.error()<<'\n';
            cancel_title_demo_work(demo);restore_title_demo_audio(app);
            release_title_demo_menu(app,focused,now);
        }
        InvalidateRect(hwnd,nullptr,FALSE);
    }
    return true;
}
void sync_gameplay_bgm(App &app,bool foreground){
    if(!app.stage_scene)return;
    const auto &s=*app.stage_scene;
    if(s.mima_music_stopped){
        if(app.gameplay_audio_attempted!=-2){
            cancel_gameplay_audio(app);
            if(!app.audio.stop())audio_error(app);
            app.playing_track=-1;app.gameplay_audio_attempted=-2;
        }
        return;
    }
    const int wanted=scene_music_track(s);
    if(wanted>=6&&wanted!=13&&wanted!=s.stage+1&&!app.stage_scene->transition.boss_music_frame)
        app.stage_scene->transition.boss_music_frame=s.ticks;
    const bool pause=app.stage_intro.active()||soew::gameplay_music_paused(foreground,s.paused,s.life.game_over,s.stage2_preview_boundary||s.stage3_preview_boundary||(s.rika_enabled&&s.rika.done&&!s.transitions_enabled));
    if(app.smoke){
        if(app.gameplay_audio_attempted!=wanted){app.gameplay_audio_attempted=wanted;start_preview(app,wanted,false,app.practice_bgm_measure);app.practice_bgm_measure=0;if(app.audio.active())++app.gameplay_audio_starts;}
    }else{
        if(app.gameplay_audio_load&&app.gameplay_audio_load_track!=wanted&&app.gameplay_audio_load_generation==app.gameplay_audio_generation){cancel_gameplay_audio(app);app.gameplay_audio_attempted=-1;}
        if(app.gameplay_audio_load&&app.gameplay_audio_load->wait_for(std::chrono::seconds(0))==std::future_status::ready){
            try{
                auto prepared=app.gameplay_audio_load->get();
                if(app.gameplay_audio_load_generation==app.gameplay_audio_generation&&app.gameplay_audio_load_track==wanted){
                    if(!prepared){app.audio_status={L"ステージ曲を準備できませんでした",L"Could not prepare stage music"};}
                    else{
                        const HostPerf::Span span(app.host_perf.get(),HostPerf::Metric::AudioPublish);
                        if(!app.audio.publish(std::move(prepared),WAVE_MAPPER,pause))audio_error(app);
                        else{app.loaded_track=wanted;app.playing_track=app.menu.music==0?-1:wanted;app.midi_mode=app.menu.music==2;app.fm_enabled=app.menu.music!=0;++app.gameplay_audio_starts;app.audio_status={L"",L""};}
                    }
                }
            }catch(const std::exception&){app.audio_status={L"ステージ曲を準備できませんでした",L"Could not prepare stage music"};}
            app.gameplay_audio_load.reset();app.gameplay_audio_cancel.reset();
        }
        if(app.gameplay_audio_attempted!=wanted&&!app.gameplay_audio_load&&!app.practice_audio_load){
            app.gameplay_audio_attempted=wanted;
            const bool midi=app.menu.music==2,off=app.menu.music==0,silent=app.audio_test_silent;
            const auto *song=midi?&app.midi_songs.at(wanted):&app.songs.at(wanted);const auto *font=midi?&app.soundfont:nullptr;const auto *effects=&app.effect_bank,*carrier=&app.songs.at(wanted);const auto measure=app.practice_bgm_measure;app.practice_bgm_measure=0;
            app.gameplay_audio_cancel=std::make_shared<std::atomic_bool>(false);const auto cancel=app.gameplay_audio_cancel;
            app.gameplay_audio_load_track=wanted;app.gameplay_audio_load_generation=app.gameplay_audio_generation;
            try{app.gameplay_audio_load.emplace(std::async(std::launch::async,[song,font,effects,carrier,off,silent,measure,cancel](){const auto song_copy=*song,font_copy=font?*font:std::vector<std::byte>{},effects_copy=*effects,carrier_copy=*carrier;return AudioPreview::prepare(song_copy,silent,font_copy,effects_copy,carrier_copy,off,measure,cancel.get());}));}
            catch(const std::exception&){app.audio_status={L"ステージ曲を準備できませんでした",L"Could not prepare stage music"};app.gameplay_audio_cancel.reset();}
        }
    }
    if(!app.audio.pause(pause))audio_error(app);
}
void stop_gameplay_bgm(App &app){
    cancel_gameplay_audio(app);
    if(!app.audio.stop())audio_error(app);
    app.playing_track=-1;app.gameplay_audio_attempted=-1;app.audio_status={L"",L""};
}
soew::ShotInputMode shot_mode(const App &app){
    const bool dialogue=app.stage_scene&&app.stage_scene->dialogue.active();
    return soew::shot_input_mode(app.menu.autofire,app.menu.fast_autofire,dialogue);
}
const soew::StageDialogue *live_mima_tap_dialogue(const App &app){
    if(!app.stage_scene||app.replay_playback)return nullptr;
    const auto &scene=*app.stage_scene;
    if(!scene.mima_enabled||scene.mima.event!=soew::MimaCore::Event::Form1Defeated||
        !scene.dialogue.mima_pre_final_dialogue())return nullptr;
    return &scene.dialogue;
}
void arm_stage_intro(App &app,bool authentic_entry){
    // Synthetic smoke calls step the scene directly, not via the host timer.
    app.stage_intro.arm(host_ms(),authentic_entry&&!app.smoke&&app.menu.stage_intro&&app.stage_intro_art.has_value());
}
void launch_stage(App &app,int id,bool stage4_trial=false){
    app.death_presented=false;app.score_review=App::ScoreReview::None;app.pending_score.reset();
    if(!app.stage_scene)app.practice_assist.info=app.menu.show_more_info;
    app.run_assisted=false;
    app.mima_dialogue_tap.reset((GetAsyncKeyState('Z')&0x8000)!=0);
    app.dialogue_hold.clear();
    cancel_gameplay_audio(app);
    if(!app.audio.stop())throw std::runtime_error("Audio stop before stage failed");
    app.z_input.clear();
    auto rules=app.menu.rules();
    if(id==5&&app.extra_trial)rules={4,5,2,1,int(app.extra_shot)};
    if(app.replay_playback){const auto &r=app.replay_playback->config;rules={r.rank,id,r.lives,r.bombs,r.shot};}
    const bool extra=id==5&&(app.extra_trial||app.extra_candidate||(app.replay_playback&&app.replay_playback->config.extra));
    if(id==5&&!extra&&!app.smoke&&app.gameplay_audio_enabled)throw std::runtime_error("Extra is not enabled in this build mode");
    if(extra&&!app.extra_trial&&!app.replay_playback&&(!app.menu.extra_mode||!app.menu.unlocked))throw std::runtime_error("Extra is locked");
    const soew::NativeReplayConfig config=app.replay_playback?app.replay_playback->config:soew::NativeReplayConfig{1234,id,rules.shot,rules.rank,rules.lives,rules.bombs,app.menu.music,app.menu.reduced,app.menu.autofire,stage4_trial,id==4&&app.stage5_trial,app.stage5_mainline,extra};
    app.stage_scene=soew::load_replay_scene(app.assets_root,config,soew::simulation_language(app.menu.language));
    arm_stage_intro(app,(id==0||id==5)&&!stage4_trial&&!app.stage5_trial&&!app.extra_trial);
    app.replay_session.emplace(config,app.replay_playback&&!app.replay_playback->summary);
    app.stage_scene->sound.enabled=app.menu.music!=0&&!app.effect_bank.empty();
    app.stage_scene->sound_output=[&app](unsigned id){if(app.menu.music!=0&&!app.effect_bank.empty()&&app.audio.active()&&!app.audio.effect(id))audio_error(app);};
    app.run_entry_stage=id;app.run_entry_stage4_trial=stage4_trial;app.run_pause_menu=false;
    app.recorder.reset();app.recording_timing={};
    if(!app.replay_playback&&app.replay_recording&&(app.settings_enabled||app.smoke)&&(id<5||extra))app.recorder.emplace(config);
    app.stage_entry_release=true;app.stage_clock.reset(host_ms());app.scroll_phase=soew::scroll_subpixels;app.motion_before.stage=-1;app.gameplay_audio_attempted=-1;
    if(app.gameplay_audio_enabled)sync_gameplay_bgm(app,app.smoke||(app.focused&&!app.minimized&&!app.resizing));
    else app.audio_status={L"診断ステージ：音声なし",L"Diagnostic stage: audio disabled"};
    if(app.gameplay_audio_enabled&&app.menu.music!=0&&app.audio.active())app.audio.effect(13);
    app.notice=app.tr(L"Esc：一時停止　会話中はZ長押しで早送り",L"Esc: Pause Menu | Hold Z during dialogue to fast-forward",L"Esc：暂停菜单 | 对话时按住 Z 快进",L"Esc：暫停選單 | 對話時按住 Z 快轉");
}
void return_to_title(App &app){
    app.stage_intro.cancel();
    app.mima_dialogue_tap.reset((GetAsyncKeyState('Z')&0x8000)!=0);
    stop_gameplay_bgm(app);app.stage_scene.reset();app.ending_player.reset();app.ending_surface.reset();
    app.pending_score.reset();app.score_review=App::ScoreReview::None;app.score_release=false;app.run_pause_menu=false;
    app.recorder.reset();app.replay_playback.reset();app.replay_cursor=0;app.replay_session.reset();app.replay_seek.reset();
    app.practice_session.reset();app.practice_result=app.practice_escape=false;app.practice_bgm_measure=0;
    app.replay_pending.reset();app.replay_confirm.reset();app.replay_menu={};
    if(app.replay_menu_restore){app.menu=*app.replay_menu_restore;app.replay_menu_restore.reset();}
    app.z_input.clear();app.menu.shot_confirmed=false;app.menu.shot_screen=false;app.menu.extra_mode=false;app.menu.allowed=false;app.menu_visible=true;
    start_preview(app,0);
}
void publish_replay(App &app,soew::NativeReplay replay,std::unique_ptr<soew::EncounterScene> scene,
                    soew::ReplaySession session,std::size_t cursor,bool from_start=true){
    stop_gameplay_bgm(app);
    app.replay_menu_restore=app.menu;app.replay_cursor=cursor;app.replay_playback=std::move(replay);
    const auto &run=*app.replay_playback;
    app.menu.music=run.config.music;app.menu.autofire=run.config.autofire;app.menu.reduced=run.config.reduced;
    app.stage_scene.emplace(std::move(*scene));app.replay_session.emplace(std::move(session));
    app.run_entry_stage=app.stage_scene->stage;app.run_entry_stage4_trial=run.config.stage4_trial;app.run_pause_menu=false;
    app.recorder.reset();app.practice_session.reset();app.practice_visible=app.practice_result=app.practice_escape=false;
    app.z_input.clear();app.dialogue_hold.clear();app.mima_dialogue_tap.reset((GetAsyncKeyState('Z')&0x8000)!=0);
    app.stage_entry_release=true;app.stage_clock.reset(host_ms());app.scroll_phase=soew::scroll_subpixels;app.motion_before.stage=-1;
    app.stage_scene->sound.enabled=app.menu.music!=0&&(run.practice||!app.effect_bank.empty());
    app.stage_scene->sound_output=[&app](unsigned id){if(app.menu.music!=0&&!app.effect_bank.empty()&&app.audio.active()&&!app.audio.effect(id))audio_error(app);};
    app.practice_bgm_measure=run.practice?soew::practice_music_measure(run.practice->spec):0;
    arm_stage_intro(app,from_start&&cursor==0&&(run.practice?run.practice->spec.section==soew::PracticeSection::StageStart:
        ((run.config.stage==0||run.config.stage==5)&&!run.config.stage4_trial&&!run.config.stage5_trial)));
    if(app.gameplay_audio_enabled) sync_gameplay_bgm(app,app.smoke||(app.focused&&!app.minimized&&!app.resizing));
    app.notice=run.practice?app.tr(L"練習リプレイ再生中　Esc：停止",L"Practice replay | Esc: Stop",L"正在回放练习录像 | Esc：停止",L"正在重播練習錄影 | Esc：停止"):
        app.tr(L"リプレイ再生中　Esc：停止",L"Replay playback | Esc: Stop",L"正在回放录像 | Esc：停止",L"正在重播錄影 | Esc：停止");
}
void launch_replay(App &app,const fs::path &file,int target=-1){
    auto run=soew::load_native_replay(file);
    const auto boundary=target<0?0:soew::story_replay_stage_start(run,target);
    soew::ReplaySession session(run);
    auto scene=run.practice?soew::load_practice_replay_scene(app.assets_root,run):
        std::make_unique<soew::EncounterScene>(soew::load_replay_scene(app.assets_root,run.config,soew::simulation_language(app.menu.language)));
    if(target<0)publish_replay(app,std::move(run),std::move(scene),std::move(session),0);
    else {
        // Run the same sound state machine, without an output callback.
        scene->sound.enabled=run.config.music!=0&&!app.effect_bank.empty();
        app.replay_seek.emplace(App::ReplaySeek{target,boundary,0,host_ms(),std::move(run),std::move(scene),std::move(session)});
    }
}
void advance_replay_seek(App &app){
    auto &seek=*app.replay_seek;
    const auto started=std::chrono::steady_clock::now();
    const auto fail=[&](std::string_view error){app.replay_seek.reset();app.replay_menu.complete_play(false,error);};
    if(host_ms()-seek.started>120000){fail("SEEK BUDGET");return;}
    for(unsigned i=0;i<256;++i){
        const auto step=seek.session.step_playback(*seek.scene,seek.run,seek.cursor,seek.target);
        if(step.status!=soew::ReplayStepStatus::Running||seek.cursor>seek.boundary){
            std::cerr<<"Replay seek failed: "<<step.error<<'\n';fail("SEEK MISMATCH");return;
        }
        if(seek.scene->stage==seek.target&&seek.cursor==seek.boundary&&step.stage_advanced){
            publish_replay(app,std::move(seek.run),std::move(seek.scene),std::move(seek.session),seek.cursor,false);
            app.replay_seek.reset();app.replay_return_browser=true;app.replay_menu.complete_play(true);return;
        }
        if(std::chrono::steady_clock::now()-started>=std::chrono::milliseconds(6))break;
    }
}
void refresh_replay_catalog(App &app){
    soew::ReplaySlotStore store(app.replay_dir);
    app.replay_catalog={};
    for(std::size_t i=0;i<app.replay_slots.size();++i){
        auto &entry=app.replay_slots[i];entry=store.inspect(i,false);
        using State=soew::ReplaySlotState;
        app.replay_catalog.slots[i]=entry.status==soew::ReplaySlotStatus::Empty?State::Empty:
            entry.status==soew::ReplaySlotStatus::Ready?(entry.can_replace()?State::Ready:State::ReadyProtected):
            (entry.can_replace()?State::Unavailable:State::UnavailableProtected);
    }
    app.replay_legacy=store.legacy_entries();app.replay_catalog.legacy_count=app.replay_legacy.size();
}
void open_replay_browser(App &app){
    try{refresh_replay_catalog(app);app.replay_menu.begin_browse();app.menu.selected=7;}
    catch(const std::exception &e){const std::string message=e.what();std::cerr<<"Cannot open Replay: "<<message<<'\n';app.notice=app.tr(L"リプレイを開けませんでした",L"Cannot open replay",L"无法打开录像",L"無法開啟錄影");}
}
void exit_replay_playback(App &app,std::wstring message){
    const bool browse=app.replay_return_browser;app.replay_return_browser=false;
    return_to_title(app);if(browse)open_replay_browser(app);app.notice=std::move(message);
}
void finalize_replay(App &app){
    if(app.recorder&&app.replay_session&&app.replay_session->summary()&&app.recorder->can_save()&&
       !app.recorder->replay().summary&&!app.recorder->finalize(*app.replay_session->summary()))app.recorder.reset();
}
bool begin_replay_save(App &app,soew::ReplaySaveOrigin origin){
    if(app.run_assisted||!app.recorder||!app.recorder->can_save())return false;
    try{
        auto recorder=*app.recorder;
        if(!recorder.replay().summary){
            if(!app.stage_scene)return false;
            if(app.practice_session){
                if(!recorder.finalize(soew::practice_replay_summary(*app.stage_scene,*app.practice_session)))
                    throw std::runtime_error("Incomplete Practice replay");
            }else{
                if(!app.replay_session)return false;
                auto session=*app.replay_session;
                if(!recorder.finalize(session.stop(*app.stage_scene)))throw std::runtime_error("Incomplete replay");
            }
        }
        refresh_replay_catalog(app);app.replay_confirm.reset();app.replay_pending=recorder.replay();
        app.pending_replay_timing=app.recording_timing.total;
        app.replay_menu.begin_save(origin);return true;
    }catch(const std::exception &e){const std::string message=e.what();std::cerr<<"Replay not saved: "<<message<<'\n';app.notice=app.tr(L"リプレイを保存できませんでした",L"Could not save replay",L"无法保存录像",L"無法儲存錄影");return false;}
}
void handle_replay_menu(App &app,unsigned input){
    using Action=soew::ReplayMenuAction;using Screen=soew::ReplayMenuScreen;
    const auto prior=app.replay_menu.screen();const auto origin=app.replay_menu.origin();
    const auto action=app.replay_menu.update(input,app.replay_catalog);
    try{
        soew::ReplaySlotStore store(app.replay_dir);
        if(prior==Screen::Browse&&app.replay_menu.screen()==Screen::Details){
            const auto selected=app.replay_menu.selected_entry();
            const auto &entry=selected.legacy?app.replay_legacy.at(selected.index):app.replay_slots.at(selected.index);
            app.replay_menu.set_playback_stages(entry.listing?entry.listing->seek_stages:0);
            app.replay_details_timing=store.slowdown_stats(entry);
        }
        if(app.replay_menu.screen()==Screen::Overwrite&&prior!=Screen::Overwrite){
            const auto slot=app.replay_menu.selected_entry().index;
            app.replay_confirm=store.inspect(slot);
            if(!app.replay_confirm->can_replace())app.replay_confirm.reset();
        }
        if(action.action==Action::ToggleRecording){
            app.replay_recording=!app.replay_recording;
            if(app.settings_enabled&&app.settings){
                auto path=app.settings->path.parent_path()/"replay-options.txt";
                std::ofstream out(path,std::ios::binary|std::ios::trunc);out<<(app.replay_recording?"on\n":"off\n");out.close();
                if(!out)app.notice=app.tr(L"記録の設定を保存できませんでした",L"Recording setting could not be saved",L"无法保存录像设置",L"無法儲存錄影設定");
            }
        }
        else if(action.action==Action::Play){
            const auto &entry=action.entry.legacy?app.replay_legacy.at(action.entry.index):app.replay_slots.at(action.entry.index);
            if(entry.status!=soew::ReplaySlotStatus::Ready)throw std::runtime_error("Replay is unavailable");
            launch_replay(app,entry.path,action.stage);
            if(!app.replay_seek){app.replay_return_browser=true;app.replay_menu.complete_play(true);}
        }else if(action.action==Action::CancelPlay){
            app.replay_seek.reset();app.notice=app.tr(L"面指定の再生を取り消しました",L"Replay stage start cancelled",L"已取消从指定关卡回放",L"已取消從指定關卡重播");
        }else if(action.action==Action::Write){
            if(!app.replay_pending||(action.replace&&!app.replay_confirm))throw std::runtime_error("Replay slot changed; choose again");
            app.replay_pending->name=app.replay_menu.name();
            const auto timing=app.pending_replay_timing;
            const auto result=store.save(action.entry.index,*app.replay_pending,action.replace?&*app.replay_confirm:nullptr,
                soew::ReplaySlowdownStats{timing.active_ms,timing.lost_ms});
            if(result!=soew::ReplaySlotSaveResult::Saved){refresh_replay_catalog(app);throw std::runtime_error("Replay slot changed; choose again");}
            const auto file=store.path(action.entry.index).filename().wstring();
            app.replay_menu.complete_write(true);return_to_title(app);app.notice=app.tr(L"リプレイを保存しました：",L"Replay saved: ",L"录像已保存：",L"錄影已儲存：")+file;
        }else if(action.action==Action::Back||action.action==Action::Discard){
            app.replay_pending.reset();app.replay_confirm.reset();app.menu.allowed=false;
            if(app.practice_session)app.practice_menu.allowed=false;
            else if(prior!=Screen::Browse&&origin!=soew::ReplaySaveOrigin::Pause)return_to_title(app);
        }
    }catch(const std::exception &e){
        std::cerr<<"Replay action failed: "<<e.what()<<'\n';
        app.notice=app.tr(L"リプレイの操作に失敗しました",L"Replay operation failed",L"录像操作失败",L"錄影操作失敗");
        if(action.action==Action::Play){if(!app.replay_menu.active())open_replay_browser(app);else app.replay_menu.complete_play(false);}
        if(action.action==Action::Write)app.replay_menu.complete_write(false);
    }
}
Frame practice_frame(const App &app){
    auto frame=app.frames[2];
    auto native=app.menu;
    native.begin_shot_select(app.practice_panel.shot);
    compose_shots(frame,app.portraits,native,app.selected_color,app.unselected_color);
    const auto lang=app.menu.language;
    using T=soew::PracticeText;
    const bool loading=!app.practice_load_cancelled&&(app.practice_load||app.practice_audio_load||app.practice_builder);
    struct TextRun {std::wstring value;RECT bounds;std::uint32_t color;T style;bool right;};
    std::vector<TextRun> runs;
    soew::draw_practice_panel(frame.pixels,app.practice_panel,app.practice_menu,lang,loading,
        app.selected_color,app.unselected_color,
        [&](int l,int t,int r,int b,std::uint32_t color){shot_box(frame,l,t,r,b,color);},
        [&](std::wstring_view label,int x,int y,int right,int height,std::uint32_t color,T style,bool align_right){
            if(style==T::Stage){
                for(const auto c:label){draw_glyph(frame.pixels,app.font,glyph_code(static_cast<char>(c)),x,y,color);x+=16;}
            }else runs.push_back({std::wstring(label),{x,y,right,y+height},color,style,align_right});
        });
    if(runs.empty())return frame;
    struct Surface {
        HDC dc=CreateCompatibleDC(nullptr);
        HBITMAP bitmap=nullptr;
        HGDIOBJ saved_bitmap=nullptr,saved_font=nullptr;
        std::array<HFONT,2> fonts{};
        ~Surface(){
            if(saved_font)SelectObject(dc,saved_font);
            if(saved_bitmap)SelectObject(dc,saved_bitmap);
            for(auto font:fonts)if(font)DeleteObject(font);
            if(bitmap)DeleteObject(bitmap);
            if(dc)DeleteDC(dc);
        }
    } canvas;
    BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=frame.w;
    info.bmiHeader.biHeight=-frame.h;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
    void *bits=nullptr;canvas.bitmap=CreateDIBSection(canvas.dc,&info,DIB_RGB_COLORS,&bits,nullptr,0);
    if(!canvas.dc||!canvas.bitmap)throw std::runtime_error("Practice panel surface allocation failed");
    const auto family=soew::localized(lang,L"MS Gothic",L"MS Gothic",L"SimSun",L"PMingLiU");
    const auto charset=lang==soew::Language::ChineseSimplified?GB2312_CHARSET:lang==soew::Language::ChineseTraditional?CHINESEBIG5_CHARSET:SHIFTJIS_CHARSET;
    for(unsigned i=0;i<canvas.fonts.size();++i){
        constexpr std::array<int,2> sizes={16,14};
        canvas.fonts[i]=CreateFontW(-sizes[i],0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,charset,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,family);
        if(!canvas.fonts[i])throw std::runtime_error("Practice panel font allocation failed");
    }
    canvas.saved_bitmap=SelectObject(canvas.dc,canvas.bitmap);
    canvas.saved_font=SelectObject(canvas.dc,canvas.fonts[0]);
    SetBkMode(canvas.dc,TRANSPARENT);
    auto *surface=static_cast<std::uint32_t*>(bits);
    std::copy(frame.pixels.begin(),frame.pixels.end(),surface);
    for(auto &run:runs){
        SelectObject(canvas.dc,canvas.fonts[unsigned(run.style)]);
        SetTextColor(canvas.dc,RGB((run.color>>16)&255,(run.color>>8)&255,run.color&255));
        DrawTextW(canvas.dc,run.value.data(),int(run.value.size()),&run.bounds,
            DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_END_ELLIPSIS|(run.right?DT_RIGHT:DT_LEFT));
    }
    GdiFlush();std::copy_n(surface,frame.pixels.size(),frame.pixels.begin());
    return frame;
}
void compose_replay_menu(App &app){
    using Screen=soew::ReplayMenuScreen;
    auto &f=app.composed;
    f=Frame{640,400,std::vector<std::uint32_t>(640*400,0x111426)};
    const auto lang=app.menu.language;
    const auto draw=[&](std::wstring_view t,int x,int y,std::uint32_t color=0xffffff,int max_width=640){display_text(f,t,x,y,color,lang,max_width);};
    const auto wide=[](std::string_view s){return std::wstring(s.begin(),s.end());};
    const auto tr=[&](const wchar_t* ja,const wchar_t* en,const wchar_t* hans,const wchar_t* hant){return soew::localized(lang,ja,en,hans,hant);};
    namespace words=soew::text;
    const auto status=[&](soew::NativeReplayOutcome o){return (o==soew::NativeReplayOutcome::Clear?words::clear:o==soew::NativeReplayOutcome::GameOver?words::game_over:words::menu_return)(lang);};
    const auto stage=[&](int s){
        if(!soew::is_chinese(lang)||s==5)return soew::stage_label(lang,s);
        return L"第 "+std::to_wstring(s+1)+tr(L"",L"",L" 关",L" 關");
    };
    const auto screen=app.replay_menu.screen();
    const auto selected=app.replay_menu.selected_entry();
    const auto title=(screen==Screen::Browse||screen==Screen::Details||screen==Screen::Starting?words::replay_browser:words::save_replay)(lang);
    draw(title,320-display_text_width(title,lang)/2,24,0xffff00);
    if(screen==Screen::Name){
        draw(words::name(lang)+wide(app.replay_menu.name_editor().text()),64,88);
        const auto &editor=app.replay_menu.name_editor();
        for(std::size_t i=0;i<editor.columns*editor.rows;++i){
            if(!editor.selectable(i))continue;
            const auto label=i==editor.end_key?tr(L"完",L"END",L"完",L"完"):i==editor.back_key?tr(L"消",L"DEL",L"删",L"刪"):i==editor.advance_key?L">":i==editor.space_key?tr(L"空",L"SP",L"空",L"空"):std::wstring(1,wchar_t(editor.character(i)));
            draw(label,52+int(i%editor.columns)*33-display_text_width(label,lang)/2,152+int(i/editor.columns)*30,i==editor.selected_key()?0xffff00:0xff88ff);
        }
        draw(tr(L"方向キー：選択　Z / Enter：決定　X / Esc：戻る",L"Arrows: select   Z/Enter: confirm   X/Esc: back",L"方向键：选择  Z / Enter：确认  X / Esc：返回",L"方向鍵：選擇  Z / Enter：確認  X / Esc：返回"),64,334);
    }else if(screen==Screen::SaveChoice||screen==Screen::Overwrite){
        draw(screen==Screen::Overwrite?tr(L"上書きしますか？",L"Overwrite Replay?",L"覆盖录像？",L"覆寫錄影？"):tr(L"このリプレイを保存しますか？",L"Save this replay?",L"保存这段录像？",L"儲存這段錄影？"),72,100);
        draw(screen==Screen::Overwrite?words::yes(lang):words::save_replay(lang),112,152,app.replay_menu.choice_yes()?0xffff00:0xffffff);
        draw(screen==Screen::Overwrite?words::no(lang):tr(L"保存しない",L"Discard Replay",L"不保存",L"不儲存"),112,180,!app.replay_menu.choice_yes()?0xffff00:0xffffff);
    }else if(screen==Screen::Starting&&app.replay_seek){
        const auto &seek=*app.replay_seek;
        draw(tr(L"再生準備中：",L"Preparing replay: ",L"正在准备回放：",L"正在準備重播：")+stage(seek.target),64,116,0xffff00);
        draw(std::to_wstring(seek.cursor*100/seek.boundary)+L"%",64,148);
        draw(tr(L"X / Esc：取り消す",L"X/Esc: cancel",L"X / Esc：取消",L"X / Esc：取消"),64,330);
    }else if(screen==Screen::Details){
        const auto &entry=selected.legacy?app.replay_legacy.at(selected.index):app.replay_slots.at(selected.index);
        if(entry.listing){const auto &l=*entry.listing;
            draw(words::name(lang)+(l.name.empty()?entry.path.stem().wstring():wide(l.name)),64,68);
            auto start=stage(l.config.stage);
            if(l.practice){
                soew::PracticeMenu setup;setup.spec=l.practice->spec;setup.capabilities=soew::practice_capabilities();
                using Row=soew::PracticeMenu::Row;
                const auto section=setup.spec.section;
                const auto row=section==soew::PracticeSection::Chapter?Row::Chapter:section==soew::PracticeSection::Boss?Row::Phase:Row::Section;
                start=std::wstring(tr(L"練習 / ",L"Practice / ",L"练习 / ",L"練習 / "))+start+L" / "+soew::practice_value(setup,row,lang);
                draw(tr(L"シード：",L"Seed: ",L"种子：",L"種子：")+std::to_wstring(l.practice->spec.seed),64,172);
            }
            draw(tr(L"開始地点：",L"Start Point: ",L"起点：",L"起點：")+start+tr(L"　タイプ ",L"   Shot ",L"  机体 ",L"  機體 ")+std::wstring(1,wchar_t(L'A'+l.config.shot)),64,92);
            if(l.played_at){SYSTEMTIME date{};ULARGE_INTEGER raw{};raw.QuadPart=(l.played_at+11644473600ull)*10000000ull;FILETIME ft{raw.LowPart,raw.HighPart};
                if(FileTimeToSystemTime(&ft,&date))draw(tr(L"日付 (UTC)：",L"Date (UTC): ",L"日期 (UTC)：",L"日期 (UTC)：")+std::to_wstring(date.wYear)+L"-"+std::to_wstring(date.wMonth)+L"-"+std::to_wstring(date.wDay),64,116);}
            if(l.summary){const auto &sum=*l.summary;
                draw(std::wstring(status(sum.outcome))+tr(L"　最終得点：",L"   Final Score: ",L"  最终得分：",L"  最終得分：")+std::to_wstring(std::uint64_t(sum.final_score)*10),64,140);
                if(!l.practice){
                    draw(tr(L"ステージ別得点",L"Stage Splits",L"各关得分",L"各關得分"),64,172,0xffff00);
                    for(std::size_t i=0;i<sum.stage_scores.size();++i)if(sum.stage_scores[i])draw(stage(int(i))+L"   "+std::to_wstring(std::uint64_t(*sum.stage_scores[i])*10),64,194+int(i)*19);
                }
            }else draw(tr(L"旧形式 v1：結果とステージ別得点は未検証",L"Legacy v1: no verified result or splits",L"旧格式 v1：结果与各关得分未经验证",L"舊格式 v1：結果與各關得分未經驗證"),64,140);
            auto timing_value=std::wstring(tr(L"未記録",L"Not recorded",L"未记录",L"未記錄"));
            if(const auto &t=app.replay_details_timing;t&&t->active_ms){
                const auto tenths=static_cast<unsigned>(1000.0L*t->lost_ms/t->active_ms+0.5L);
                timing_value=t->lost_ms&&!tenths?L"<0.1%":std::to_wstring(tenths/10)+L"."+std::to_wstring(tenths%10)+L"%";
            }
            draw(tr(L"処理落ち：",L"Slowdown: ",L"处理落：",L"處理落：")+timing_value,332,194,0xffff00);
            draw(tr(L"記録時の処理落ち（演出を除く）",L"Recording-time simulation loss",L"录制时丢失的模拟时间（不含演出）",L"錄影時丟失的模擬時間（不含演出）"),332,215);
        }
        if(app.replay_menu.playback_stages()){
            const auto target=app.replay_menu.playback_stage();
            draw(target<0?std::wstring(tr(L"最初から再生",L"Play from the beginning",L"从头回放",L"從頭重播")):
                tr(L"この面から再生：",L"Play from this stage: ",L"从本关回放：",L"從本關重播：")+stage(target),64,312,0xffff00);
            draw(tr(L"方向キー：開始面　Z / Enter：再生　X / Esc：戻る",L"Arrows: start point   Z/Enter: play   X/Esc: back",L"方向键：起点  Z / Enter：回放  X / Esc：返回",L"方向鍵：起點  Z / Enter：重播  X / Esc：返回"),64,346);
        }else draw(tr(L"Z / Enter：再生　X / Esc：戻る",L"Z/Enter: play    X/Esc: back",L"Z / Enter：回放  X / Esc：返回",L"Z / Enter：重播  X / Esc：返回"),64,330);
    }else{
        const auto page=app.replay_menu.page();const bool legacy=page>=soew::replay_menu_slot_pages;
        draw(tr(L"ページ ",L"Page ",L"页码 ",L"頁碼 ")+std::to_wstring(page+1)+L" / "+std::to_wstring(app.replay_menu.page_count(app.replay_catalog)),40,57,0xffff00);
        draw(tr(L"番号",L"Slot",L"栏位",L"欄位"),40,83);draw(tr(L"名前",L"Name",L"名字",L"名字"),104,83);draw(tr(L"結果",L"Result",L"结果",L"結果"),330,83);
        for(std::size_t row=0;row<10;++row){
            const auto i=(page-(legacy?soew::replay_menu_slot_pages:0))*10+row;
            if(legacy&&i>=app.replay_legacy.size())break;
            const auto &e=legacy?app.replay_legacy[i]:app.replay_slots[i];
            const auto color=row==app.replay_menu.row()?0xffff00u:0xffffffu;
            const int y=107+int(row)*20;
            draw(legacy?L"V1":(i<10?L"0":L"")+std::to_wstring(i),40,y,color);
            auto name=e.listing?(e.listing->name.empty()?e.path.stem().wstring():wide(e.listing->name)):
                std::wstring(e.status==soew::ReplaySlotStatus::Empty?tr(L"空き",L"None",L"空",L"空"):tr(L"無効",L"Invalid",L"无效",L"無效"));
            if(!chinese_display(lang))while(jp::cell_width(name)>216)name.pop_back();
            draw(name,104,y,color,216);
            if(e.listing){
                const auto &l=*e.listing;
                const auto result=l.summary?status(l.summary->outcome):tr(L"旧形式・読取専用",L"Legacy / read only",L"旧格式 / 只读",L"舊格式 / 唯讀");
                draw(l.practice?std::wstring(tr(L"練習 / ",L"Practice / ",L"练习 / ",L"練習 / "))+result:std::wstring(result),330,y,color);
            }
        }
        if(screen==Screen::Browse)draw(app.replay_recording?tr(L"R：記録する",L"R: Recording ON",L"R：录像已开启",L"R：錄影已開啟"):tr(L"R：記録しない",L"R: Recording OFF",L"R：录像已关闭",L"R：錄影已關閉"),40,322);
        draw(tr(L"左右：ページ切替　Z / Enter：選択　X / Esc：戻る",L"Left/Right: page   Z/Enter: select   X/Esc: back",L"左右：翻页  Z / Enter：选择  X / Esc：返回",L"左右：翻頁  Z / Enter：選擇  X / Esc：返回"),40,349);
    }
    if(!app.replay_menu.error().empty())draw(soew::replay_menu_error(app.replay_menu.error(),lang),40,377,0xff8888);
}
bool score_keys_released(){
    for(int key:{VK_UP,VK_DOWN,VK_LEFT,VK_RIGHT,VK_NUMPAD8,VK_NUMPAD2,VK_NUMPAD4,VK_NUMPAD6,
                 int('Z'),int('X'),VK_RETURN,VK_ESCAPE,VK_SPACE,VK_BACK})
        if(GetAsyncKeyState(key)&0x8000)return false;
    return true;
}
void open_score_review(App &app,App::ScoreReview review,unsigned rank){
    app.score_review=review;app.review_rank=rank;app.score_review_armed=false;
    app.score_clock.reset(host_ms());app.stage_clock.reset(host_ms());
    app.dialogue_hold.clear();app.mima_dialogue_tap.clear();app.z_input.clear();
}
void open_terminal_score_review(App &app,soew::NativeScoreEntry entry){
    open_score_review(app,App::ScoreReview::Terminal,entry.difficulty);
    app.pending_score.reset();app.score_signature={};
    if(!app.run_assisted&&app.native_scores&&app.native_scores->qualifies(entry))app.pending_score=std::move(entry);
}
void test_live_assist_host(){
    auto host=std::make_unique<App>();
    auto &app=*host;
    const auto check=[](bool pass){if(!pass)throw std::runtime_error("Live assist host test failed");};
    check(!live_assist_controls(app));
    app.stage_scene.emplace();
    check(live_assist_controls(app));
    app.stage_scene->paused=true;
    check(!live_assist_controls(app));
    app.stage_scene->paused=false;
    app.replay_playback.emplace();
    check(!live_assist_controls(app)&&app.stage_scene->practice_assist.tools==0);
    app.replay_playback.reset();
    app.recorder.emplace();
    check(app.recorder->tick(0,false));
    app.practice_assist.open=true;
    app.practice_assist.toggle_row(soew::PracticeAssist::tool_rows);
    apply_live_assists(app);
    check(!app.run_assisted&&app.recorder->can_save());
    app.native_scores.emplace(fs::current_path());
    const soew::NativeScoreEntry score{"",1,0,0,100000,false,1};
    open_terminal_score_review(app,score);
    check(app.pending_score.has_value()&&!live_assist_controls(app));
    app.score_review=App::ScoreReview::None;
    app.practice_assist.toggle(soew::PracticeTool::Invincible);
    apply_live_assists(app);
    check(app.run_assisted&&!app.recorder&&app.stage_scene->practice_assist.enabled(soew::PracticeTool::Invincible));
    app.stage_scene.emplace(); // The next stage has a new scene.
    apply_live_assists(app);
    check(app.stage_scene->practice_assist.enabled(soew::PracticeTool::Invincible));
    app.practice_assist.toggle(soew::PracticeTool::Invincible);
    apply_live_assists(app);
    check(app.run_assisted&&app.stage_scene->practice_assist.tools==0);
    open_terminal_score_review(app,score);
    check(!app.pending_score&&app.score_review==App::ScoreReview::Terminal);
}
void finish_score_flow(App &app,soew::ReplaySaveOrigin origin){
    app.score_review=App::ScoreReview::None;app.pending_score.reset();
    if(!begin_replay_save(app,origin))return_to_title(app);
}
void handle_live_death_result(App &app){
    const auto &s=*app.stage_scene;
    if(s.continues_used){finish_score_flow(app,soew::ReplaySaveOrigin::GameOver);return;}
    open_terminal_score_review(app,{"",unsigned(s.rank),unsigned(s.player.shot),0,s.score_core.score,false,unsigned(s.stage+1)});
}
void release_score_controls(App &app){
    app.score_review=App::ScoreReview::None;app.score_release=true;
    app.stage_clock.reset(host_ms());app.z_input.clear();
}
bool intercept_live_game_over(App &app,soew::ReplayStepStatus status){
    if(app.replay_playback||app.practice_session)return false;
    const auto &s=*app.stage_scene;
    if(!s.life.game_over&&!s.extra_settlement.terminal()){app.death_presented=false;return false;}
    if(s.stage5_result||s.extra_settlement.phase==soew::ExtraSettlement::Phase::Clear)return false;
    if(!app.death_presented){
        app.death_presented=true;app.game_over_frame=0;
        open_score_review(app,App::ScoreReview::GameOver,unsigned(s.rank));return true;
    }
    if(status==soew::ReplayStepStatus::Finished){handle_live_death_result(app);return true;}
    return false;
}
void acknowledge_game_over(App &app){
    const auto &s=*app.stage_scene;
    if(app.replay_session&&app.replay_session->summary())handle_live_death_result(app);
    else if(s.rank<4&&s.continues_used==0)
        open_score_review(app,App::ScoreReview::BeforeContinue,unsigned(s.rank));
    else release_score_controls(app);
}
void refresh_extra_unlock(App &app){
    auto status=app.scores?soew::clear_status(*app.scores):soew::ClearStatus{};
    if(app.extra_candidate&&app.native_scores){
        std::array<bool,3> main{},extra{};
        for(unsigned i=0;i<3;++i){main[i]=app.native_scores->has_good_clear(i);extra[i]=app.native_scores->has_extra_clear(i);}
        status=soew::merge_clear_status(status,main,extra);
    }
    app.menu.unlocked=status.unlocked;app.menu.cleared_main=status.main;app.menu.cleared_extra=status.extra;
}
void start_stage5_ending(App &app){
    const auto &s=*app.stage_scene;
    auto run=soew::EndingRun{};
    run.continues_used=s.continues_used;run.shot_type=unsigned(s.player.shot);run.rank=unsigned(s.rank);
    run.score=soew::ending_resident_score(s.score_core.score,s.continues_used);
    run.start_lives=int(s.run_start_lives);run.start_bombs=int(s.run_start_bombs);
    run.skill=soew::ending_calculate_skill(run.rank,run.continues_used,std::uint8_t(s.life.total_misses),s.bomb.total_used,s.items.item_skill);
    const bool extra=s.stage==5;
    const bool cleared=extra&&s.extra_settlement.phase==soew::ExtraSettlement::Phase::Clear;
    const bool good=!extra&&s.stage5_result&&s.stage5_result->good_ending;
    const soew::NativeScoreEntry entry{"",run.rank,run.shot_type,run.continues_used,s.score_core.score,good,
        extra&&!cleared?soew::NativeScoreEntry::extra_stage:soew::NativeScoreEntry::all_stages};
    if(app.native_scores&&!app.replay_playback&&!app.run_assisted){
        const auto status=cleared?app.native_scores->record_extra_clear(entry.shot_type):
            good?app.native_scores->record_good_clear(entry.shot_type):soew::NativeScoreStore::Status::Ok;
        if(status!=soew::NativeScoreStore::Status::Ok)app.settings_warning="Could not save the native clear record";
        if(app.extra_candidate)refresh_extra_unlock(app);
    }
    run.extra_unlocked=extra?cleared&&(app.menu.unlocked||app.extra_trial):app.menu.unlocked;
    soew::EndingSurfaceHooks hooks;
    const auto language=app.menu.language;
    hooks.read_asset=[&app,language](std::string_view name){
        return read(name.ends_with(".TXT") ? soew::locale_asset(app.assets_root,language,name) : app.assets_root/fs::path(name));
    };
    if(soew::is_chinese(language)&&app.chinese_text)
        hooks.chinese_line=[&app,language](std::string_view asset,std::size_t record){return app.chinese_text->line(language,asset,record);};
    hooks.draw_text=[language](soew::EndingSurface &out,soew::EndingSurfaceText encoding,std::string_view text,int x,int y,std::uint8_t ink){
        std::wstring wide;
        if(encoding==soew::EndingSurfaceText::Cp932)wide=cp932(text);
        else wide=soew::wide_utf8(text);
        Frame mask{640,400,std::vector<std::uint32_t>(640*400)};
        display_text(mask,wide,x,y,0xffffff,language);
        const int width=soew::is_chinese(language)?display_text_width(wide,language):int(wide.size())*16;
        for(int py=std::max(0,y);py<std::min(400,y+16);++py)
            for(int px=std::max(0,x);px<std::min(640,x+width);++px)
                if(mask.pixels[py*640+px])out.put_pixel(px,py,ink);
    };
    hooks.play_song=[&app](std::string_view name){
        for(unsigned i=0;i<soew::music_files.size();++i)if(soew::music_files[i]==name){
            start_preview(app,int(i));
            if(name=="ENDING.M"){
                app.staff_song_started=true;
                app.staff_song_generation=app.audio.playback_generation();
                app.staff_music_poll=0;
                app.staff_music={};
            }
            return;
        }
        throw std::runtime_error("Unknown ending song");
    };
    hooks.register_score=[&app,entry](const soew::EndingVerdict &){
        if(!app.replay_playback&&!app.practice_session&&!entry.continues_used)open_terminal_score_review(app,entry);
    };
    app.staff_song_started=false;
    app.staff_music_poll=0;
    app.staff_music={};
    auto script=soew::make_ending_script(run);
    stop_gameplay_bgm(app);app.ending_surface.emplace(std::move(hooks),language);app.ending_player.emplace(std::move(script));app.stage_scene.reset();app.z_input.clear();
    app.notice=extra?(cleared?app.tr(L"エキストラクリア",L"Extra clear",L"额外关卡已通关",L"額外關卡已通關"):app.tr(L"エキストラ：ゲームオーバー",L"Extra game over",L"额外关卡：游戏结束",L"額外關卡：遊戲結束")):app.tr(L"５面のエンディング",L"Stage 5 ending",L"第 5 关结局",L"第 5 關結局");
}
void apply_next_stage(App &app){
    if(!app.stage_scene||app.stage_scene->transition.phase!=soew::StageTransition::Phase::Ready||!app.stage_scene->transition_error.empty())return;
    try{
        auto next=app.stage_scene->next_stage();
        if(next.stage==4&&(app.replay_playback?app.replay_playback->config.stage5_mainline:app.stage5_mainline))next.enable_stage5_preview();
        if(app.recorder)app.recorder->next_stage();
        cancel_gameplay_audio(app);app.stage_scene=std::move(next);app.stage_entry_release=true;app.z_input.clear();app.stage_clock.reset(host_ms());app.gameplay_audio_attempted=-1;sync_gameplay_bgm(app,app.smoke||(app.focused&&!app.minimized&&!app.resizing));app.notice=app.tr(L"ステージ開始　Esc：メニュー　P：一時停止",L"Stage entry | Esc menu | P pause",L"关卡开始 | Esc：菜单 | P：暂停",L"關卡開始 | Esc：選單 | P：暫停");
    }
    catch(const std::exception &e){app.stage_scene->transition_error=e.what();std::cerr<<"Stage transition failed: "<<e.what()<<'\n';app.notice=app.tr(L"ステージを切り替えられませんでした。直前の画面を保持しています",L"Stage transition failed; previous scene retained",L"无法切换关卡，已保留原场景",L"無法切換關卡，已保留原場景");}
}
bool practice_keys_released(){for(int key:{VK_UP,VK_DOWN,VK_LEFT,VK_RIGHT,VK_NUMPAD8,VK_NUMPAD2,VK_NUMPAD4,VK_NUMPAD6,VK_NUMPAD1,VK_NUMPAD3,VK_NUMPAD7,VK_NUMPAD9,int('Z'),int('X'),VK_SPACE,VK_RETURN,VK_ESCAPE,VK_PRIOR,VK_NEXT,int('R'),int('Q')})if(GetAsyncKeyState(key)&0x8000)return false;return true;}
void reset_practice_to_menu(App &app,const std::string &warning){app.practice_retry=app.practice_retry_release=false;stop_gameplay_bgm(app);app.stage_scene.reset();app.recorder.reset();app.practice_session.reset();app.practice_builder.reset();app.practice_result=app.practice_escape=false;app.practice_visible=true;app.practice_choice=0;app.practice_panel.open(app.practice_menu,soew::PracticePanel::Page::Setup);app.practice_menu.allowed=false;app.practice_menu.warning=warning;app.z_input.clear();app.stage_clock.reset(host_ms());app.menu.shot_screen=false;app.menu_visible=true;app.notice=soew::practice_warning(warning,app.menu.language);}
void save_practice(App &app){if(app.practice_store&&!app.practice_store->save(app.practice_menu.spec)){app.practice_menu.warning=app.practice_store->warning;app.notice=app.tr(L"練習設定を保存できませんでした",L"Practice settings could not be saved",L"无法保存练习设置",L"無法儲存練習設定");}}
void begin_practice(App &app,const soew::PracticeSpec &spec){
    app.practice_started=host_ms();
    if(app.practice_load||app.practice_builder||app.practice_audio_load){app.practice_menu.warning="Previous request is still cancelling";return;}
    const bool retry=app.stage_scene&&app.practice_session;
    const auto root=app.assets_root;const auto recipe=spec;const bool reduced=app.menu.reduced;const auto language=soew::simulation_language(app.menu.language);app.practice_load_cancelled=false;
    std::uint32_t seed=0;
    try{seed=soew::choose_practice_seed(recipe,app.last_practice_seed,[]{return std::random_device{}();});}
    catch(const std::exception &e){app.practice_menu.warning=e.what();app.notice=soew::practice_warning(e.what(),app.menu.language);return;}
    app.dialogue_hold.clear();app.mima_dialogue_tap.reset((GetAsyncKeyState('Z')&0x8000)!=0);
    try{
        if(app.smoke)app.practice_builder.emplace(root,recipe,soew::practice_checkpoint(recipe),seed,reduced,language);
        else app.practice_load.emplace(std::async(std::launch::async,[root,recipe,seed,reduced,language](){return soew::PracticeBuilder(root,recipe,soew::practice_checkpoint(recipe),seed,reduced,language);}));
    }catch(const std::exception&e){app.practice_menu.warning=e.what();app.notice=soew::practice_warning(e.what(),app.menu.language);return;}
    // The request now owns its seed and worker. Only accepted attempts replace the recording.
    app.recorder.reset();app.recording_timing={};cancel_gameplay_audio(app);app.gameplay_audio_attempted=-1;
    app.practice_audio_started=false;app.practice_bgm_measure=0;app.practice_retry=retry;
    if(!retry){app.practice_visible=true;app.practice_result=app.practice_escape=false;app.practice_choice=0;app.practice_assist.info=app.menu.show_more_info;}
    app.z_input.clear();
    if(app.stage_scene)app.stage_scene->paused=true;
    if(app.audio.active())app.audio.pause(true);
    if(!retry)save_practice(app);
    app.notice.clear();
}
void advance_practice(App &app){
    if(app.practice_audio_load&&app.practice_load_cancelled){if(app.practice_audio_load->wait_for(std::chrono::seconds(0))!=std::future_status::ready)return;try{app.practice_audio_load->get();}catch(const std::exception&){}app.practice_audio_load.reset();app.practice_audio_started=false;app.practice_load_cancelled=false;return;}
    if(app.practice_load){if(app.practice_load->wait_for(std::chrono::seconds(0))!=std::future_status::ready)return;
        try{auto ready=app.practice_load->get();app.practice_load.reset();if(app.practice_load_cancelled){app.practice_load_cancelled=false;return;}app.practice_builder.emplace(std::move(ready));}
        catch(const std::exception&e){app.practice_load.reset();if(app.practice_load_cancelled){app.practice_load_cancelled=false;return;}reset_practice_to_menu(app,e.what());return;}}
    if(!app.practice_builder)return;
    const auto status=app.practice_builder->advance(256);
    if(status==soew::PracticeBuilder::Status::Preparing)return;
    if(status!=soew::PracticeBuilder::Status::Ready){const auto error=app.practice_builder->error();reset_practice_to_menu(app,error);return;}
    std::optional<AudioPreview::Prepared> prepared_audio;
    if(!app.smoke&&app.gameplay_audio_enabled){
        drain_gameplay_audio(app);if(app.gameplay_audio_load)return;
        if(!app.practice_audio_started){
            const auto &r=app.practice_builder->recipe();const auto *s=app.practice_builder->scene();
            const int track=scene_music_track(*s);
            if(app.retry_probe.waiting)app.retry_probe.audio=RetryHostProbe::Clock::now();
            app.practice_audio_track=track;const std::span<const std::byte> song=app.menu.music==2?app.midi_songs.at(track):app.songs.at(track),font=app.menu.music==2?std::span<const std::byte>(app.soundfont):std::span<const std::byte>{},effects=app.effect_bank,carrier=app.songs.at(track);const bool off=app.menu.music==0,silent=app.audio_test_silent;const auto measure=soew::practice_music_measure(r);
            app.practice_audio_cancel=std::make_shared<std::atomic_bool>(false);const auto cancel=app.practice_audio_cancel;
            const bool allow_checkpoint=!app.retry_probe.force_exact;
            try{app.practice_audio_load.emplace(std::async(std::launch::async,[song,font,effects,carrier,off,silent,measure,cancel,allow_checkpoint](){return AudioPreview::prepare(song,silent,font,effects,carrier,off,measure,cancel.get(),allow_checkpoint);}));app.practice_audio_started=true;}catch(const std::exception&e){reset_practice_to_menu(app,e.what());}return;
        }
        if(!app.practice_audio_load)return;
        if(app.practice_audio_load->wait_for(std::chrono::seconds(0))!=std::future_status::ready)return;
        if(app.practice_retry&&!app.focused&&app.retry_probe.mode<0)return; // Remain on the paused frame until focus returns.
        try{prepared_audio.emplace(app.practice_audio_load->get());}catch(const std::exception&e){app.practice_audio_load.reset();app.practice_audio_started=false;reset_practice_to_menu(app,e.what());return;}app.practice_audio_load.reset();
        if(app.practice_load_cancelled){app.practice_builder.reset();app.practice_audio_started=false;app.practice_load_cancelled=false;return;}
        if(!*prepared_audio){const auto error=prepared_audio->error();reset_practice_to_menu(app,error);return;}
    }
    if(app.retry_probe.waiting)app.retry_probe.device=RetryHostProbe::Clock::now();
    std::cout<<"practice_prepare_ms="<<host_ms()-app.practice_started<<" updates="<<app.practice_builder->prepared_ticks()<<'\n';
    const auto recipe=app.practice_builder->recipe();const auto seed=app.practice_builder->seed();auto next=app.practice_builder->release();
    // Practice audio always publishes to the same fixed WinMM format. Reuse
    // the title device on entry too; publish still replaces every PCM buffer.
    const bool replace_output=prepared_audio&&app.audio.active();
    if(!replace_output)stop_gameplay_bgm(app);
    app.practice_retry_release=app.practice_retry&&!app.smoke;app.practice_retry=false;app.last_paint_practice=false;
    app.stage_scene.emplace(std::move(*next));app.practice_session.emplace(recipe,soew::practice_checkpoint(recipe),seed);app.practice_builder.reset();app.practice_result=app.practice_escape=false;app.practice_visible=false;
    app.run_assisted=false;
    app.stage_scene->sound.enabled=app.menu.music!=0;
    app.stage_scene->sound_output=[&app](unsigned id){if(app.gameplay_audio_enabled&&app.menu.music!=0&&!app.effect_bank.empty()&&app.audio.active()&&!app.audio.effect(id))audio_error(app);};
    app.stage_entry_release=true;app.z_input.clear();app.stage_clock.reset(host_ms());app.scroll_phase=soew::scroll_subpixels;app.motion_before.stage=-1;app.gameplay_audio_attempted=-1;
    arm_stage_intro(app,recipe.section==soew::PracticeSection::StageStart);
    app.practice_bgm_measure=soew::practice_music_measure(recipe);
    const bool practice_window_enabled=(app.smoke||app.retry_probe.mode>=0||app.focused)&&!app.minimized&&!app.resizing;
    if(prepared_audio){const bool paused=app.stage_intro.active()||!practice_window_enabled;if(!app.audio.publish(std::move(*prepared_audio),WAVE_MAPPER,paused,replace_output)){const auto error=app.audio.error();reset_practice_to_menu(app,error);return;}app.gameplay_audio_attempted=app.practice_audio_track;app.loaded_track=app.practice_audio_track;app.playing_track=app.menu.music==0?-1:app.practice_audio_track;app.midi_mode=app.menu.music==2;app.fm_enabled=app.menu.music!=0;++app.gameplay_audio_starts;app.practice_bgm_measure=0;app.practice_audio_started=false;}
    if(app.replay_recording&&(app.settings_enabled||app.smoke||(app.async_practice_test&&app.retry_probe.mode<0))){
        auto recorded=recipe;recorded.seed=seed;
        const auto config=soew::practice_replay_config(recorded,app.menu.music,app.menu.reduced,app.menu.autofire);
        app.recorder.emplace(config,soew::NativeReplayPractice{recorded,app.stage_scene->language,soew::practice_replay_start_digest(*app.stage_scene),soew::practice_start_rule(recorded)});
    }
    app.last_practice_seed=seed;
    if(app.retry_probe.waiting){
        app.retry_probe.ready=RetryHostProbe::Clock::now();
        app.retry_probe.intro_expected=app.stage_intro.active();
        app.retry_probe.seen=true;
    }
    app.dialogue_hold.clear();
    sync_gameplay_bgm(app,practice_window_enabled);
    app.notice=app.tr(L"練習 | シード ",L"Practice | Seed ",L"练习 | 种子 ",L"練習 | 種子 ")+std::to_wstring(seed)+
        (recipe.seed_mode==soew::PracticeSeedMode::Random?app.tr(L" | R：再抽選",L" | R: reroll",L" | R：重抽种子",L" | R：重抽種子"):
            app.tr(L" | R：同じシードで再挑戦",L" | R: retry same seed",L" | R：用相同种子重试",L" | R：用相同種子重試"))+
        app.tr(L" | Esc：メニュー",L" | Esc: menu",L" | Esc：菜单",L" | Esc：選單");
}
void leave_practice(App &app,bool title){app.practice_retry_release=app.last_paint_practice=false;stop_gameplay_bgm(app);app.stage_scene.reset();app.recorder.reset();app.practice_session.reset();app.practice_builder.reset();app.practice_result=app.practice_escape=false;app.practice_visible=!title;if(!title)app.practice_panel.open(app.practice_menu,soew::PracticePanel::Page::Setup);app.practice_menu.allowed=false;app.menu.selected=6;app.menu.allowed=false;app.menu.shot_screen=false;app.z_input.clear();app.menu_visible=true;start_preview(app,0);}
bool set_fullscreen(HWND hwnd,App &app,bool enabled){
    if(enabled==app.window_fullscreen)return true;
    const auto changed=[](HWND window,LONG_PTR style){SetLastError(0);return SetWindowLongPtrW(window,GWL_STYLE,style)!=0||GetLastError()==0;};
    constexpr UINT refresh=SWP_FRAMECHANGED|SWP_NOOWNERZORDER|SWP_NOACTIVATE;
    if(enabled){
        WINDOWPLACEMENT placement{};placement.length=sizeof(placement);
        MONITORINFO monitor{};monitor.cbSize=sizeof(monitor);
        if(!GetWindowPlacement(hwnd,&placement)||!GetMonitorInfoW(MonitorFromWindow(hwnd,MONITOR_DEFAULTTONEAREST),&monitor))return false;
        const auto style=GetWindowLongPtrW(hwnd,GWL_STYLE);
        if(!changed(hwnd,(style&~WS_OVERLAPPEDWINDOW)|WS_POPUP))return false;
        const auto &r=monitor.rcMonitor;
        if(!SetWindowPos(hwnd,HWND_TOP,r.left,r.top,r.right-r.left,r.bottom-r.top,refresh)){
            changed(hwnd,style);SetWindowPlacement(hwnd,&placement);
            SetWindowPos(hwnd,nullptr,0,0,0,0,refresh|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);return false;
        }
        app.windowed_style=style;app.windowed_placement=placement;
    }else{
        const auto fullscreen_style=GetWindowLongPtrW(hwnd,GWL_STYLE);
        if(!changed(hwnd,(app.windowed_style&~WS_VISIBLE)|(fullscreen_style&WS_VISIBLE)))return false;
        if(!SetWindowPlacement(hwnd,&app.windowed_placement)){
            changed(hwnd,fullscreen_style);SetWindowPos(hwnd,nullptr,0,0,0,0,refresh|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);return false;
        }
        SetWindowPos(hwnd,nullptr,0,0,0,0,refresh|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER);
    }
    app.window_fullscreen=enabled;app.dialogue_hold.clear();app.stage_clock.reset(host_ms());
    return true;
}
void caption(HWND hwnd, const App &app) {
    const auto lang=app.menu.language;
    const wchar_t* mode=app.extra_trial?app.tr(L"エキストラ試遊版（保存なし）",L"Extra trial (no save)",L"额外关卡试玩（不保存）",L"額外關卡試玩（不儲存）"):
        app.stage5_trial?app.tr(L"５面試遊版",L"Stage 5 trial",L"第 5 关试玩",L"第 5 關試玩"):app.stage4_trial?app.tr(L"４面互換試遊版",L"Stage 4 compatibility trial",L"第 4 关兼容性试玩",L"第 4 關相容性試玩"):L"";
    auto title=std::wstring(app.tr(L"東方封魔録",L"Story of Eastern Wonderland",L"东方封魔录",L"東方封魔錄"));
    if(*mode)title+=L" | "+std::wstring(mode);
    if(!app.notice.empty())title+=L" | "+app.notice;
    if(const auto status=app.audio_status(lang);*status)title+=L" | "+std::wstring(status);
    if(!app.settings_warning.empty())title+=L" | "+soew::settings_warning_text(lang,app.settings_warning);
    title+=app.tr(L" | F2：画面比率　F8：リプレイ",L" | F2: aspect   F8: Open Replay",L" | F2：画面比例  F8：打开录像",L" | F2：畫面比例  F8：開啟錄影");
    if(app.legacy_content)title+=L" | "+std::wstring(soew::text::legacy_content(lang));
    SetWindowTextW(hwnd,title.c_str());
}
LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w, LPARAM l) {
    auto *app = reinterpret_cast<App *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if(message == WM_NCCREATE) {
        app = static_cast<App *>(reinterpret_cast<CREATESTRUCTW *>(l)->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    }
    if(!app) return DefWindowProcW(hwnd, message, w, l);
    switch(message) {
    case WM_SYSKEYDOWN:
        title_demo_activity(*app);
        break; // Preserve the system menu and OS shortcuts.
    case WM_SYSKEYUP: case WM_KEYUP:
        if(app->title_demo&&w<256&&app->title_demo->consumed_keys.test(w)){
            app->title_demo->consumed_keys.reset(w);
            release_title_demo_menu(*app,title_demo_foreground(hwnd,*app),host_ms());
            if(message==WM_KEYUP)return 0;
        }
        if(message==WM_SYSKEYUP||(title_demo_active(*app)&&!title_demo_plain_key(w)))break;
        if(title_demo_active(*app))return 0;
        if(app->stage_scene&&app->stage_intro.active())return 0;
        if(w=='Z'){app->dialogue_hold.clear();app->mima_dialogue_tap.event(live_mima_tap_dialogue(*app),false);}
        if(app->replay_menu.active()&&(app->smoke||practice_keys_released()))app->replay_menu.update(0,app->replay_catalog);
        if(app->stage_scene&&w=='Z')app->z_input.event(false,shot_mode(*app));
        // Synthetic test messages cannot change GetAsyncKeyState.
        if(app->smoke) {app->menu.update(0);app->practice_menu.update(0);return 0;}
        // Rearm only after all relevant physical keys have been released.
        if(!(GetAsyncKeyState(VK_LEFT)&0x8000) && !(GetAsyncKeyState(VK_RIGHT)&0x8000) && !(GetAsyncKeyState(VK_NUMPAD4)&0x8000) && !(GetAsyncKeyState(VK_NUMPAD6)&0x8000) && !(GetAsyncKeyState(VK_NUMPAD8)&0x8000) && !(GetAsyncKeyState(VK_NUMPAD2)&0x8000) && !(GetAsyncKeyState(VK_UP)&0x8000) && !(GetAsyncKeyState(VK_DOWN)&0x8000) && !(GetAsyncKeyState('Z')&0x8000) && !(GetAsyncKeyState(VK_SPACE)&0x8000) && !(GetAsyncKeyState('X')&0x8000) && !(GetAsyncKeyState(VK_RETURN)&0x8000) && !(GetAsyncKeyState(VK_ESCAPE)&0x8000) && (!app->menu.options||!(GetAsyncKeyState(VK_TAB)&0x8000))) app->menu.update(0);
        if(practice_keys_released())app->practice_menu.update(0);
        return 0;
    case WM_KILLFOCUS:
        app->recording_timing.boundary();
        app->staff_music_poll=0;
        app->mima_dialogue_tap.reset((GetAsyncKeyState('Z')&0x8000)!=0);
        app->dialogue_hold.clear();
        if(!title_demo_active(*app))save_settings(*app);
        if(app->title_demo){
            title_demo_activity(*app);
            app->title_demo->clock.reset(host_ms());
            app->title_demo->reconcile_keys=true;
        }
        app->z_input.clear();
        if(!app->audio.pause(true))audio_error(*app);
        app->focused=false;app->clock.reset(host_ms());app->score_clock.reset(host_ms());app->menu.allowed=false;app->practice_menu.allowed=false;return 0;
    case WM_SETFOCUS:
        app->mima_dialogue_tap.reset((GetAsyncKeyState('Z')&0x8000)!=0);
        if(app->title_demo)app->title_demo->clock.reset(host_ms());
        app->focused=true;app->clock.reset(host_ms());app->score_clock.reset(host_ms());app->menu.allowed=false;app->practice_menu.allowed=false;app->z_input.clear();return 0;
    case WM_ENTERSIZEMOVE:
        app->recording_timing.boundary();
        if(app->title_demo){title_demo_activity(*app);app->title_demo->clock.reset(host_ms());}
        app->mima_dialogue_tap.reset((GetAsyncKeyState('Z')&0x8000)!=0);
        app->dialogue_hold.clear();app->resizing=true;app->clock.reset(host_ms());app->score_clock.reset(host_ms());return 0;
    case WM_EXITSIZEMOVE:
        if(app->title_demo)app->title_demo->clock.reset(host_ms());
        app->resizing=false;app->clock.reset(host_ms());app->score_clock.reset(host_ms());return 0;
    case WM_CHAR: case WM_DEADCHAR:
        if(title_demo_active(*app)||app->ending_player||app->score_review!=App::ScoreReview::None)return 0;
        break;
    case WM_KEYDOWN: {
        ++app->keys;
        const bool repeat=(l & (1LL<<30))!=0;
        if(title_demo_keydown(hwnd,*app,w,repeat))return 0;
        if(title_demo_active(*app))break;
        if(app->stage_scene&&app->stage_intro.active())return 0;
        if(app->score_review!=App::ScoreReview::None){
            using Review=App::ScoreReview;using Key=soew::ScoreSignature::Key;
            if((app->focused||app->smoke)&&app->score_review_armed){
                const bool accept=w=='Z'||w==VK_RETURN, cancel=w==VK_ESCAPE;
                if(app->score_review==Review::GameOver){
                    if(!repeat&&(accept||cancel||w=='X'||w==VK_SPACE))acknowledge_game_over(*app);
                }else if(app->score_review==Review::BeforeContinue){
                    if(!repeat&&(accept||cancel||w=='X'||w==VK_SPACE))release_score_controls(*app);
                }else{
                    bool done=!repeat&&(cancel||(!app->pending_score&&(accept||w=='X'||w==VK_SPACE)));
                    if(app->pending_score&&!cancel){
                        const auto key=(w==VK_UP||w==VK_NUMPAD8)?std::optional(Key::Up):(w==VK_DOWN||w==VK_NUMPAD2)?std::optional(Key::Down):
                            (w==VK_LEFT||w==VK_NUMPAD4)?std::optional(Key::Left):(w==VK_RIGHT||w==VK_NUMPAD6)?std::optional(Key::Right):
                            !repeat&&accept?std::optional(Key::Select):!repeat&&w=='X'?std::optional(Key::Bomb):
                            !repeat&&w==VK_BACK?std::optional(Key::Backspace):!repeat&&w==VK_SPACE?std::optional(Key::Space):std::nullopt;
                        if(key&&app->score_signature.update(*key)){
                            auto &entry=*app->pending_score;entry.name=app->score_signature.name();entry.gaiji_name=true;
                            const auto status=app->native_scores->submit(entry);
                            if(status!=soew::NativeScoreStore::Status::Ok&&status!=soew::NativeScoreStore::Status::NotRanked)
                                app->settings_warning="Could not save the native score; existing records retained";
                            done=true;
                        }
                    }
                    if(done)finish_score_flow(*app,app->ending_player?soew::ReplaySaveOrigin::Clear:soew::ReplaySaveOrigin::GameOver);
                }
            }
            caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;
        }
        if(app->score_release)return 0;
        if(!repeat&&(w==VK_ESCAPE||w=='P'))app->mima_dialogue_tap.clear();
        if(w=='Z'){
            const bool live=app->focused&&!app->minimized&&!app->resizing&&!app->stage_entry_release&&
                app->stage_scene&&!app->stage_scene->paused;
            const auto *dialogue=live?live_mima_tap_dialogue(*app):nullptr;
            app->mima_dialogue_tap.event(dialogue,true,repeat);
        }
        if(app->replay_menu.active()){
            using M=soew::ReplayMenu;
            if(!repeat){
                const unsigned input=w==VK_UP?M::up:w==VK_DOWN?M::down:w==VK_LEFT?M::left:w==VK_RIGHT?M::right:
                    (w==VK_RETURN||w=='Z')?M::confirm:(w==VK_ESCAPE||w=='X')?M::cancel:w=='R'?M::toggle_recording:w==VK_BACK?M::erase:0;
                if(input)handle_replay_menu(*app,input);
            }
            caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;
        }
        if(app->practice_audio_load&&!app->practice_load_cancelled){if(w==VK_ESCAPE&&!repeat){app->practice_load_cancelled=true;if(app->practice_audio_cancel)app->practice_audio_cancel->store(true,std::memory_order_relaxed);reset_practice_to_menu(*app,"Practice entry cancelled");}return 0;}
        if(app->practice_load&&!app->practice_load_cancelled){if(w==VK_ESCAPE&&!repeat){app->practice_load_cancelled=true;reset_practice_to_menu(*app,"Practice entry cancelled");}return 0;}
        if(app->practice_builder){if(w==VK_ESCAPE&&!repeat){app->practice_builder->cancel();reset_practice_to_menu(*app,"Practice preparation cancelled");}InvalidateRect(hwnd,nullptr,FALSE);return 0;}
        if(app->practice_visible){
            if(repeat)return 0;
            auto &m=app->practice_menu;
            auto &panel=app->practice_panel;
            if(!m.number_editing()&&(w==VK_PRIOR||w==VK_NEXT)&&panel.focused(soew::PracticePanel::Field::Power)&&m.allowed){m.allowed=false;const int delta=w==VK_PRIOR?1:-1;const auto level=soew::PickupCore::levels.at(unsigned(m.spec.power)>>2);int power=m.spec.power;while(power+delta>=1&&power+delta<=80){power+=delta;if(soew::PickupCore::levels.at(unsigned(power)>>2)!=level)break;}m.spec.power=power;save_practice(*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
            if(m.number_editing()){
                if((GetKeyState(VK_CONTROL)&0x8000)&&w=='A')m.number_select_all();
                else if((GetKeyState(VK_CONTROL)&0x8000)&&w=='V'){bool valid=false;if(OpenClipboard(hwnd)){const auto data=GetClipboardData(CF_UNICODETEXT);if(data){const auto count=GlobalSize(data)/sizeof(wchar_t);const auto text=static_cast<const wchar_t*>(GlobalLock(data));if(text){for(std::size_t i=0;i<count&&i<=256;++i){if(text[i]==0){valid=m.number_paste(std::wstring_view(text,i));break;}}GlobalUnlock(data);}}CloseClipboard();}if(!valid)m.warning="Invalid pasted number";else m.warning.clear();}
                else if(!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_SHIFT)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)&&w>='0'&&w<='9')m.number_digit(unsigned(w-'0'));else if(!(GetKeyState(VK_CONTROL)&0x8000)&&!(GetKeyState(VK_MENU)&0x8000)&&w>=VK_NUMPAD0&&w<=VK_NUMPAD9)m.number_digit(unsigned(w-VK_NUMPAD0));
                else if(w==VK_OEM_MINUS||w==VK_SUBTRACT)m.number_minus();else if(w==VK_BACK)m.number_backspace();else if(w==VK_DELETE)m.number_delete();else if(w==VK_LEFT)m.number_editor.move_left();else if(w==VK_RIGHT)m.number_editor.move_right();else if(w==VK_HOME)m.number_editor.home();else if(w==VK_END)m.number_editor.end();else if(w==VK_RETURN){if(m.number_enter())save_practice(*app);}else if(w==VK_ESCAPE)m.number_cancel();
            }
            else{
                using M=soew::PracticeMenu;
                const auto before=m.spec;
                unsigned in=0;
                if(w==VK_UP||w==VK_NUMPAD8)in=M::up;else if(w==VK_DOWN||w==VK_NUMPAD2)in=M::down;
                else if(w==VK_LEFT||w==VK_NUMPAD4)in=M::left;else if(w==VK_RIGHT||w==VK_NUMPAD6)in=M::right;
                else if(w==VK_RETURN||w=='Z')in=M::confirm;else if(w==VK_ESCAPE||w=='X')in=M::cancel;
                const auto action=in?panel.update(m,in):M::Action::None;
                if(action==soew::PracticeMenu::Action::Start)begin_practice(*app,m.spec);
                else if(action==soew::PracticeMenu::Action::Back){app->practice_visible=false;app->menu.begin_practice_shot_select(panel.shot);}
                else if(action==soew::PracticeMenu::Action::NewSeed){m.spec.seed=std::random_device{}();save_practice(*app);}
                else if(m.spec!=before)save_practice(*app);
            }caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;
        }
        if(live_assist_controls(*app)){
            auto &assist=app->practice_assist;
            const int row=w>=VK_F1&&w<=VK_F7?int(w-VK_F1):w=='U'?7:w==VK_TAB?8:-1;
            if(w==VK_BACK||row>=0){
                if(!repeat){if(w==VK_BACK)assist.open=!assist.open;else assist.toggle_row(row);InvalidateRect(hwnd,nullptr,FALSE);}
                return 0;
            }
        }
        if(app->stage_scene&&app->practice_session){
            if(!repeat&&w=='R'){begin_practice(*app,app->practice_session->recipe);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
            if(!repeat&&w=='Q'&&(app->practice_escape||app->practice_result)){
                leave_practice(*app,false);caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;
            }
            if(app->practice_escape){
                if(!repeat&&app->practice_menu.allowed){
                    if(w==VK_UP||w==VK_NUMPAD8)app->pause_choice=Menu::ring(app->pause_choice,-1,4);
                    else if(w==VK_DOWN||w==VK_NUMPAD2)app->pause_choice=Menu::ring(app->pause_choice,1,4);
                    const int action=(w==VK_ESCAPE||w=='P'||w=='X')?0:(w==VK_RETURN||w=='Z')?app->pause_choice:-1;
                    if(action==0){app->practice_escape=false;app->stage_scene->paused=false;app->stage_entry_release=true;app->stage_clock.reset(host_ms());app->z_input.clear();}
                    if(action==1)begin_practice(*app,app->practice_session->recipe);
                    if(action==2)begin_replay_save(*app,soew::ReplaySaveOrigin::Pause);
                    if(action==3)leave_practice(*app,false);
                }
                caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;
            }
            if(app->practice_result){
                if(!repeat&&app->practice_menu.allowed){app->practice_menu.allowed=false;
                    if(w==VK_UP)app->practice_choice=Menu::ring(app->practice_choice,-1,4);else if(w==VK_DOWN)app->practice_choice=Menu::ring(app->practice_choice,1,4);
                    else if(w==VK_RETURN||w=='Z'){
                        if(app->practice_choice==0)begin_practice(*app,app->practice_session->recipe);
                        else if(app->practice_choice==1)begin_replay_save(*app,soew::ReplaySaveOrigin::PracticeResult);
                        else leave_practice(*app,app->practice_choice==3);
                    }
                }InvalidateRect(hwnd,nullptr,FALSE);return 0;
            }
            if(w==VK_ESCAPE&&!repeat){app->practice_escape=true;app->pause_choice=0;app->practice_choice=0;app->practice_menu.allowed=false;app->stage_scene->paused=true;app->z_input.clear();app->audio.pause(true);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
        }
        if(app->ending_player){
            if(app->replay_playback&&!repeat&&(w==VK_ESCAPE||w=='Q')){exit_replay_playback(*app,app->tr(L"リプレイを停止しました",L"Replay stopped",L"已停止录像回放",L"已停止錄影重播"));caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
            return 0;
        }
        if(app->stage_scene){
            if(app->run_pause_menu){
                int action=-1;
                if(!repeat){
                    if(w==VK_UP||w==VK_NUMPAD8)app->pause_choice=Menu::ring(app->pause_choice,-1,4);
                    else if(w==VK_DOWN||w==VK_NUMPAD2)app->pause_choice=Menu::ring(app->pause_choice,1,4);
                    else if(w==VK_ESCAPE||w=='P'||w=='X')action=0;
                    else if(w=='R')action=1;
                    else if(w=='Q')action=3;
                    else if(w==VK_RETURN||w=='Z')action=app->pause_choice;
                }
                if(action==0){
                    app->run_pause_menu=false;app->stage_scene->paused=false;
                    app->stage_entry_release=true;app->z_input.clear();app->stage_clock.reset(host_ms());
                }else if(action==1&&!app->replay_playback){
                    try{launch_stage(*app,app->run_entry_stage,app->run_entry_stage4_trial);}
                    catch(const std::exception &error){return_to_title(*app);std::cerr<<"Restart failed: "<<error.what()<<'\n';app->notice=app->tr(L"やり直せませんでした",L"Could not restart",L"无法重新开始",L"無法重新開始");}
                }else if(action==2&&app->recorder&&app->recorder->can_save()){
                    begin_replay_save(*app,soew::ReplaySaveOrigin::Pause);
                }else if(action==3){
                    if(app->replay_playback)exit_replay_playback(*app,app->tr(L"リプレイを停止しました",L"Replay stopped",L"已停止录像回放",L"已停止錄影重播"));
                    else {return_to_title(*app);app->notice=app->tr(L"保存せずに終了しました",L"Run ended without saving",L"已退出，未保存录像",L"已離開，未儲存錄影");}
                }
                app->recording_timing.boundary();
                if(app->stage_scene&&app->gameplay_audio_enabled)sync_gameplay_bgm(*app,app->smoke||(app->focused&&GetForegroundWindow()==hwnd&&!app->minimized&&!app->resizing));
            }else if(!repeat&&w==VK_ESCAPE&&(app->stage_scene->continue_prompt()||(!app->replay_playback&&app->stage_scene->life.game_over&&!app->stage_scene->paused))){
                if(!app->replay_playback&&!app->practice_session&&app->stage_scene->continue_choice==soew::EncounterScene::ContinueChoice::Choose){
                    const auto step=app->replay_session->cancel_continue(*app->stage_scene);
                    if(app->recorder&&!app->recorder->record(step.event))app->recorder.reset();
                    finalize_replay(*app);
                    if(step.status==soew::ReplayStepStatus::Finished)handle_live_death_result(*app);
                }
            }else if(!repeat&&w==VK_ESCAPE){
                app->run_pause_menu=true;app->pause_choice=0;app->stage_scene->paused=true;app->z_input.clear();app->stage_clock.reset(host_ms());
                if(app->audio.active()&&!app->audio.pause(true))audio_error(*app);
            }else if(!repeat&&w=='P'){
                app->stage_scene->paused=!app->stage_scene->paused;app->z_input.clear();app->stage_clock.reset(host_ms());
                if(!app->stage_scene->paused)app->stage_entry_release=true;
                if(app->gameplay_audio_enabled)sync_gameplay_bgm(*app,app->smoke||(app->focused&&GetForegroundWindow()==hwnd&&!app->minimized&&!app->resizing));
            }else if(w=='Z'&&!repeat&&app->focused&&!app->stage_scene->paused&&!app->stage_entry_release)app->z_input.event(true,shot_mode(*app));
            caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;
        }
        if(w==VK_F8&&!repeat&&!app->opening.active&&!app->scores_visible&&!app->menu.options&&!app->menu.music_room&&!app->menu.shot_screen){
            wchar_t path[32768]{};
            const auto folder=app->replay_dir.wstring();
            OPENFILENAMEW picker{};picker.lStructSize=sizeof(picker);picker.hwndOwner=hwnd;
            picker.lpstrFilter=app->tr(L"東方封魔録リプレイ (*.rep)\0*.rep\0\0",L"SoEW Replay (*.rep)\0*.rep\0\0",L"东方封魔录录像 (*.rep)\0*.rep\0\0",L"東方封魔錄錄影 (*.rep)\0*.rep\0\0");
            picker.lpstrTitle=app->tr(L"リプレイを開く",L"Open Replay",L"打开录像",L"開啟錄影");picker.lpstrFile=path;picker.nMaxFile=32768;
            picker.lpstrInitialDir=folder.c_str();picker.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR;
            if(GetOpenFileNameW(&picker)){
                try{launch_replay(*app,path);}
                catch(const std::exception &error){std::cerr<<"Cannot play replay: "<<error.what()<<'\n';app->notice=app->tr(L"リプレイを再生できませんでした",L"Cannot play replay",L"无法回放录像",L"無法重播錄影");}
                caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);
            }
            return 0;
        }
        if(w==VK_F5&&!repeat&&!app->opening.active&&!app->menu.options){app->menu.music=app->menu.music==2?1:2;save_settings(*app);start_preview(*app,app->loaded_track);caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
        if(w==VK_F4&&!repeat&&!app->opening.active){
            if(app->menu.music_room){
                const int track=soew::preview_f4_target(app->menu.music_selected,app->playing_track,app->audio.active());
                if(track<0){if(!app->audio.stop())audio_error(*app);app->fm_enabled=false;app->playing_track=-1;app->audio_status={L"試聴を停止しました",L"Preview stopped"};}
                else{app->fm_enabled=true;app->menu.music_comment=track;start_preview(*app,track);}
            }else if(app->audio.active()){if(!app->audio.stop())audio_error(*app);app->fm_enabled=false;app->playing_track=-1;app->audio_status={L"試聴を停止しました",L"Preview stopped"};}
            else{app->fm_enabled=true;app->audio_status={L"音楽室で曲を選ぶと試聴できます",L"Select a track in the music room to preview"};}
            caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);return 0;
        }
        if(w==VK_F3 && !repeat){if(!app->audio.stop())audio_error(*app);else app->audio_status={L"",L""};app->scores_visible=false;app->menu.music_room=false;app->menu.options=false;app->menu.shot_screen=false;app->menu.extra_mode=false;app->menu.shot_confirmed=false;app->menu.selected=0;app->menu.allowed=false;app->menu_visible=true;app->opening.restart();app->title_audio_started=false;app->title_effect_phase=0;app->clock.reset(host_ms());app->score_clock.reset(host_ms());InvalidateRect(hwnd,nullptr,FALSE);return 0;}
        if(app->opening.active){if(w==VK_RETURN||w=='Z'||w==VK_SPACE||w==VK_ESCAPE){app->opening.skip();if(!app->title_audio_started){app->title_audio_started=true;start_preview(*app,0);}app->menu.allowed=false;InvalidateRect(hwnd,nullptr,FALSE);}return 0;}
        if(app->scores_visible){
            if(app->native_board_visible&&!repeat&&(w==VK_LEFT||w==VK_RIGHT)){
                app->score_rank=Menu::ring(app->score_rank,w==VK_LEFT?-1:1,5);
                app->score_animation.reset();InvalidateRect(hwnd,nullptr,FALSE);return 0;
            }
            if(app->menu.allowed&&!repeat){app->scores_visible=false;app->menu.selected=2;app->menu.allowed=false;app->notice.clear();InvalidateRect(hwnd,nullptr,FALSE);caption(hwnd,*app);}return 0;
        }
        if(w == VK_F1 && !repeat) {app->menu_visible=!app->menu_visible;if(!app->menu_visible){if(!app->audio.stop())audio_error(*app);app->menu.music_room=false;app->audio_status={L"",L""};}}
        else if(w == VK_F2 && !repeat) app->crt = !app->crt;
        else if(!app->menu_visible && w >= '1' && w <= '3') app->scene=static_cast<int>(w-'1');
        else if(app->menu_visible) {
            unsigned input=0;
            if(w==VK_UP || w==VK_NUMPAD8) input=Menu::up;
            if(w==VK_DOWN || w==VK_NUMPAD2) input=Menu::down;
            if(w=='Z' || w==VK_RETURN || w==VK_SPACE) input=Menu::confirm;
            if(w==VK_ESCAPE||(w=='X'&&(app->menu.options||app->menu.practice_shot_select()))) input=Menu::cancel;
            if(w==VK_LEFT || w==VK_NUMPAD4) input=Menu::left;
            if(w==VK_RIGHT || w==VK_NUMPAD6) input=Menu::right;
            if(w==VK_TAB&&app->menu.options) input=Menu::tab;
            if(input) {
                const bool was_shots=app->menu.shot_screen;
                const bool was_music=app->menu.music_room,was_options=app->menu.options,was_allowed=app->menu.allowed;
                const auto previous_settings=soew::Settings::from(app->menu);
                const bool reset_options=was_options&&was_allowed&&(input&Menu::confirm)&&app->menu.option()==Menu::Option::Reset;
                const int action=app->menu.update(input);
                bool fullscreen_failed=false;
                if(was_options&&app->menu.fullscreen!=previous_settings.fullscreen){
                    const auto desired=app->menu.fullscreen;
                    app->menu.fullscreen=previous_settings.fullscreen;
                    if(set_fullscreen(hwnd,*app,desired))app->menu.fullscreen=desired;
                    else fullscreen_failed=true;
                }
                if(was_options&&soew::Settings::from(app->menu)!=previous_settings)save_settings(*app);
                if(fullscreen_failed)app->settings_warning="Could not switch fullscreen mode";
                if(was_shots&&was_allowed&&(input&Menu::confirm)&&app->menu.shot_confirmed){
                    if(app->practice_panel.open_from_shot(app->practice_menu,app->menu))app->practice_visible=true;
                    else try{launch_stage(*app,app->menu.rules().stage);}catch(const std::exception &error){app->stage_scene.reset();app->menu.shot_confirmed=false;std::cerr<<"Stage load failed: "<<error.what()<<'\n';app->notice=app->tr(L"ステージを読み込めませんでした",L"Could not load stage",L"无法载入关卡",L"無法載入關卡");}
                }
                if(was_music&&app->menu.music_room&&was_allowed&&(input&Menu::confirm))start_preview(*app,app->menu.music_selected);
                if(was_options&&was_allowed&&(previous_settings.music!=app->menu.music||reset_options))start_preview(*app,app->loaded_track);
                if(action==Menu::practice_entry)app->menu.begin_practice_shot_select(app->practice_menu.spec.shot);
                if(action==7){open_replay_browser(*app);}
                if(action==5) { DestroyWindow(hwnd); return 0; }
                if(action==2){
                    app->scores_visible=true;app->native_board_visible=true;app->score_rank=app->menu.rank;
                    app->score_animation.reset();app->score_clock.reset(host_ms());
                    app->notice=app->tr(L"移植版のローカルハイスコア",L"Local high scores",L"本地最高分",L"本機最高分");
                }
                else if(action>=0&&action!=Menu::practice_entry) app->notice=app->tr(L"この操作にはまだ対応していません",L"Selected action is not implemented yet",L"此操作暂不支持",L"此操作尚未支援");
                else if(!app->stage_scene)app->notice=app->menu.music_room?app->tr(L"音楽室：決定で再生。戻っても再生は続きます",L"Music room: confirm plays selected track; exit keeps playing"):
                    app->menu.practice_shot_select()?soew::text::confirm_back(app->menu.language):
                    app->menu.shot_screen?app->tr(L"左右：ショット選択　Z / Enter：開始　Esc：戻る",L"Left/Right: choose shot   Z/Enter: start   Esc: back",L"左右：选择机体  Z / Enter：开始  Esc：返回",L"左右：選擇機體  Z / Enter：開始  Esc：返回"):L"";
            }
        } else if(w==VK_ESCAPE) {DestroyWindow(hwnd);return 0;}
        caption(hwnd, *app); InvalidateRect(hwnd, nullptr, FALSE); return 0;
    }
    case WM_POWERBROADCAST:
        if(w==PBT_APMSUSPEND||w==PBT_APMRESUMEAUTOMATIC||w==PBT_APMRESUMESUSPEND)app->recording_timing.boundary();
        break;
    case WM_SIZE: ++app->sizes;if(app->minimized!=(w==SIZE_MINIMIZED)){
        app->recording_timing.boundary();
        if(app->title_demo){title_demo_activity(*app);app->title_demo->clock.reset(host_ms());}
    }app->minimized=(w==SIZE_MINIMIZED);if(app->minimized)app->mima_dialogue_tap.reset((GetAsyncKeyState('Z')&0x8000)!=0);app->clock.reset(host_ms());app->score_clock.reset(host_ms());InvalidateRect(hwnd, nullptr, FALSE);return 0;
    case WM_ACTIVATE:
        if(app->title_demo){title_demo_activity(*app);app->title_demo->clock.reset(host_ms());app->title_demo->reconcile_keys=true;}
        break;
    case WM_ENTERMENULOOP: case WM_EXITMENULOOP:
        if(app->title_demo){
            title_demo_activity(*app);app->title_demo->clock.reset(host_ms());
            app->title_demo->menu_loop=message==WM_ENTERMENULOOP;app->title_demo->reconcile_keys=true;
        }
        break;
    case WM_LBUTTONDOWN:
        if(app->practice_assist.open&&live_assist_controls(*app)){
            RECT client{};GetClientRect(hwnd,&client);
            const auto view=fit(client.right,client.bottom,app->crt);
            const int x=static_cast<short>(LOWORD(l))-view.x,y=static_cast<short>(HIWORD(l))-view.y;
            if(x>=0&&y>=0&&x<view.w&&y<view.h){
                const int row=soew::PracticeAssist::row_at(x*640/view.w,y*400/view.h);
                if(row>=0){app->practice_assist.toggle_row(row);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
            }
        }
        [[fallthrough]];
    case WM_RBUTTONDOWN: case WM_MBUTTONDOWN: case WM_XBUTTONDOWN: case WM_MOUSEWHEEL:
        title_demo_activity(*app);
        break;
    case WM_DPICHANGED: {
        if(app->window_fullscreen)return 0;
        const auto *r = reinterpret_cast<const RECT *>(l);
        SetWindowPos(hwnd, nullptr, r->left, r->top, r->right-r->left, r->bottom-r->top, SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps{}; const auto dc = BeginPaint(hwnd, &ps);
        if(!dc){app->failed=true;std::cerr<<"BeginPaint failed\n";PostMessageW(hwnd,WM_CLOSE,0,0);return 0;}
        try {
        RECT client{}; GetClientRect(hwnd, &client);
        if(app->display_frames_active){
            if(!app->paced_paint_due){
                app->presenter.repaint(dc,client.right,client.bottom);
                EndPaint(hwnd,&ps);return 0;
            }
            app->paced_paint_due=false;
        }
        const auto lang=app->menu.language;
        const auto tr=[&](const wchar_t* ja,const wchar_t* en,const wchar_t* hans,const wchar_t* hant){return soew::localized(lang,ja,en,hans,hant);};
        const auto draw=[&](std::wstring_view text,int x,int y,std::uint32_t color,int max_width=640){display_text(app->composed,text,x,y,color,lang,max_width);};
        namespace words=soew::text;
        const auto gameplay_frame=[&]{app->composed.w=640;app->composed.h=400;app->composed.pixels.resize(640*400);};
        {
        const HostPerf::Span composition(app->host_perf.get(),HostPerf::Metric::Composition);
        // Keep the last committed practice image (including its pause/results overlay) across resize or expose.
        if(!app->practice_retry||!app->last_paint_practice){
        // Do not clear the visible DC: composition can take several milliseconds.
        if(const auto *demo=app->title_demo?app->title_demo->controller.scene():nullptr){
            gameplay_frame();
            demo->render(app->composed.pixels,true,app->menu.smooth_background,app->title_demo->scroll_phase,&app->title_demo->motion_before);
            soew::mask_gameplay_tram(app->composed.pixels);
            soew::HudState hud;
            hud.score=demo->score_core.score;hud.high_score=hud.score;hud.lives=demo->life.lives;hud.bombs=demo->life.bombs;
            hud.power=int(demo->pickups.power);hud.difficulty=demo->rank;
            if(app->scores){const auto stored=app->scores->at(demo->rank).entries[0].score;hud.high_score=std::max(hud.high_score,stored/10);hud.high_continues=stored%10;}
            soew::draw_original_hud(app->composed.pixels,app->font,hud);
        }else if(app->title_demo&&app->title_demo->controller.phase()==DemoPhase::Score){
            constexpr unsigned rank=1; // The stock demos use Normal, regardless of menu selection.
            if(app->scores)app->composed=score_frame(*app->scores,rank,app->font,app->score_logo,app->score_palette,app->score_transparent,app->title_demo->score_frame,lang);
            else{
                app->composed=Frame{640,400,std::vector<std::uint32_t>(640*400)};
                static const std::vector<soew::NativeScoreEntry> empty;
                const auto &entries=app->native_scores?app->native_scores->entries(rank):empty;
                compose_native_board(app->composed,app->font,entries,rank,lang,true,false,nullptr,{});
                for(int y=336;y<352;++y)std::fill_n(app->composed.pixels.begin()+y*640,640,0u);
                draw(tr(L"読み取り専用・キーを押すと戻ります",L"Read only. Press a key to return.",L"只读。按任意键返回。",L"唯讀。按任意鍵返回。"),160,336,0xffffff);
            }
        }else{
        if(!app->stage_scene){
        app->composed=app->frames[app->menu_visible ? (app->menu.shot_screen?2:0) : app->scene];
        if(app->menu_visible) {
            if(app->menu.shot_screen)compose_shots(app->composed,app->portraits,app->menu,app->selected_color,app->unselected_color);
            else if(app->menu.options)compose_options(app->composed,app->font,app->menu,app->settings_warning);
            else compose_frontend(app->composed,app->font,app->menu);
        }
        if(app->menu.music_room)app->composed=music_frame(app->music_background,app->menu,app->current_music_comments(),app->music_palette,app->chinese_text?&*app->chinese_text:nullptr,app->audio.active(),app->midi_mode);
        if(app->scores_visible){
            if(app->native_board_visible){
                app->composed=Frame{640,400,std::vector<std::uint32_t>(640*400)};
                static const std::vector<soew::NativeScoreEntry> empty;
                const auto &entries=app->native_scores?app->native_scores->entries(unsigned(app->score_rank)):empty;
                compose_native_board(app->composed,app->font,entries,unsigned(app->score_rank),lang,true,false,nullptr,app->score_signature);
            }else if(app->scores){
                app->composed=score_frame(*app->scores,app->extra_candidate?app->score_rank:app->menu.rank,app->font,app->score_logo,app->score_palette,app->score_transparent,app->score_animation.frame,lang);
                if(app->native_scores)draw(app->extra_candidate?tr(L"左右：難易度切替　Tab：移植版記録",L"Left/Right: rank   Tab: native scores",L"左右：切换难度  Tab：移植版记录",L"左右：切換難度  Tab：移植版紀錄"):tr(L"Tab：移植版記録",L"Tab: native scores",L"Tab：移植版记录",L"Tab：移植版紀錄"),80,352,0xffff00);
            }
        }
        if(app->opening.active)app->composed=opening_frame(*app);
        }else{
            gameplay_frame();
            using Phase=soew::StageTransition::Phase;
            const auto phase=app->stage_scene->transition.phase;
            const bool show_stage_intro=app->menu.stage_intro&&app->stage_intro_art&&
                (app->stage_intro.showing_eye()||phase==Phase::LoadWait||phase==Phase::Ready);
            if(show_stage_intro)soew::StageIntro::paint(app->composed.pixels,app->stage_intro_art->pixels);
            else{
                app->stage_scene->render(app->composed.pixels,!app->practice_result,app->menu.smooth_background,app->scroll_phase,
                    &app->motion_before,app->motion_stride);
                if(!app->stage_intro.revealing()&&app->menu.hitboxes!=soew::HitboxMode::Off&&!app->practice_result&&!app->replay_menu.active())app->stage_scene->render_hitboxes(app->composed.pixels);
                if(app->stage_intro.revealing())compose_curtain(app->composed,app->font,true,app->stage_intro.reveal_frame());
                else{
                    compose_dialogue_text(app->composed,*app->stage_scene,lang,app->chinese_text?&*app->chinese_text:nullptr);
                    compose_transition(app->composed,*app->stage_scene,app->font,lang);
                }
                soew::mask_gameplay_tram(app->composed.pixels);
            }
            soew::HudState hud;hud.score=app->stage_scene->score_core.score;hud.high_score=hud.score;hud.lives=app->stage_scene->life.lives;hud.bombs=app->stage_scene->life.bombs;hud.power=int(app->stage_scene->pickups.power);hud.difficulty=app->stage_scene->rank;
            if(app->scores){const auto stored=app->scores->at(std::min(4,app->stage_scene->rank)).entries[0].score;hud.high_score=std::max(hud.high_score,stored/10);hud.high_continues=stored%10;}
            soew::draw_original_hud(app->composed.pixels,app->font,hud);
            const bool show_details=app->replay_playback&&!app->practice_session?app->menu.show_more_info:app->practice_assist.info;
            if(!app->stage_intro.active()&&!show_stage_intro&&(show_details||app->practice_session))draw_debug_info(app->composed,*app->stage_scene,app->practice_session,show_details,app->menu.language);
            if(app->run_pause_menu||app->practice_escape){
                const auto center=[&](std::wstring_view text,int y,std::uint32_t color){draw(text,224-display_text_width(text,lang)/2,y,color);};
                center(words::pause(lang),188,0xffffff);
                const auto stage=std::size_t(app->stage_scene->stage);
                compose_stage_subtitle(app->composed,lang,stage,false);
                draw(soew::music_title(scene_music_track(*app->stage_scene),lang),224,368,0xffffff);
                draw_glyph(app->composed.pixels,app->font,soew::stage_overlay::note_gaiji,208,368,0xffff00);
                const std::array options={words::resume(lang),words::restart(lang),tr(L"リプレイを保存して終了",L"Save Replay and Exit",L"保存录像并退出",L"儲存錄影並離開"),tr(L"保存せず終了",L"Exit Without Saving",L"不保存并退出",L"不儲存並離開")};
                for(int row=0;row<4;++row){
                    const auto text=std::wstring_view(options[row]);const int y=224+row*16;
                    const bool disabled=(row==2&&(!app->recorder||!app->recorder->can_save()))||(row==1&&app->replay_playback);
                    center(text,y,disabled?0x777777:app->pause_choice==row?0xffffff:0xff55ff);
                    if(app->pause_choice==row){const int width=display_text_width(text,lang);std::fill_n(app->composed.pixels.begin()+(y+13)*640+224-width/2,width,disabled?0x777777:0xffffff);}
                }
            }
            if(app->score_review==App::ScoreReview::GameOver)compose_game_over(app->composed,app->font,app->game_over_frame,lang);
            else if(app->stage_scene->continue_prompt()){
                namespace g=soew::score_gaiji;
                const auto &s=*app->stage_scene;
                soew::EncounterScene::apply_mima_tone(app->composed.pixels,50);
                if(chinese_display(lang)){
                    const auto center=[&](std::wstring_view text,int row,std::uint32_t color){draw(text,216-display_text_width(text,lang)/2,row*16,color);};
                    center(tr(L"",L"",L"是否续关？",L"是否接關？"),12,0xffffff);
                    center(words::yes(lang),14,s.continue_yes?0xffff00:0xffffff);
                    center(words::no(lang),15,s.continue_yes?0xffffff:0xffff00);
                    center(tr(L"",L"",L"剩余续关次数：",L"剩餘接關次數：")+std::to_wstring(3-s.continues_used),20,0x00ff00);
                }else{
                    score_gaiji_line(app->composed,app->font,g::continue_prompt,18*8,12*16,0xffffff);
                    score_gaiji_line(app->composed,app->font,g::yes,24*8,14*16,s.continue_yes?0xffff00:0xffffff);
                    score_gaiji_line(app->composed,app->font,g::no,25*8,15*16,s.continue_yes?0xffffff:0xffff00);
                    score_gaiji_line(app->composed,app->font,g::credit,18*8,20*16,0x00ff00);
                    draw_glyph(app->composed.pixels,app->font,g::zero+3-s.continues_used,32*8,20*16,0x00ff00);
                }
            }else if(app->stage_scene->life.game_over&&app->score_review==App::ScoreReview::None){
                compose_game_over(app->composed,app->font,soew::score_gaiji::animation_frames,lang);
            }
            const auto &scene=*app->stage_scene;
            if(scene.stage==3&&scene.runtime.blocked&&scene.runtime.dispatch.incomplete_boundary){
                draw(tr(L"４面の編隊データはここまでです",L"Stage 4 formation data ends here",L"第 4 关的编队数据到此为止",L"第 4 關的編隊資料到此為止"),48,176,0xffff00);
                draw(tr(L"Esc：メニューへ戻る",L"Esc: return to menu",L"Esc：返回菜单",L"Esc：返回選單"),48,200,0xffffff);
            }else if(scene.runtime.blocked||scene.weapon_blocked||scene.mima.blocked_reason()||scene.sigma.blocked){
                draw(tr(L"未対応の場面です　Esc：戻る",L"Unsupported encounter - Esc",L"此场景暂不支持  Esc：返回",L"此場景尚未支援  Esc：返回"),48,184,0xffff00);
            }else if(scene.stage5_entry_only&&scene.transition.phase==soew::StageTransition::Phase::Battle){
                draw(tr(L"５面：入口",L"Stage 5 Entry",L"第 5 关：入口",L"第 5 關：入口"),112,176,0xffff00);
                draw(tr(L"Esc：メニューへ戻る",L"Esc: return to menu",L"Esc：返回菜单",L"Esc：返回選單"),112,200,0xffffff);
            }
        }
        if(app->ending_surface){
            app->composed=Frame{640,400,std::vector<std::uint32_t>(640*400)};
            app->ending_surface->render(app->composed.pixels);
            if(app->ending_player->staff_playing()){
                const std::wstring_view hint=words::staff_fast_forward(lang);
                const int x=624-display_text_width(hint,lang);
                for(int y=12;y<32;++y)std::fill_n(app->composed.pixels.begin()+y*640+x-4,632-x,0u);
                draw(hint,x,14,0xffffff);
            }
        }
        if(app->score_review==App::ScoreReview::BeforeContinue||app->score_review==App::ScoreReview::Terminal){
            static const std::vector<soew::NativeScoreEntry> empty;
            const auto &entries=app->native_scores?app->native_scores->entries(app->review_rank):empty;
            if(!app->stage_scene)app->composed=Frame{640,400,std::vector<std::uint32_t>(640*400)};
            else soew::EncounterScene::apply_mima_tone(app->composed.pixels,50);
            compose_native_board(app->composed,app->font,entries,app->review_rank,lang,false,
                app->score_review==App::ScoreReview::BeforeContinue,app->pending_score?&*app->pending_score:nullptr,app->score_signature);
        }
        if(app->practice_visible)app->composed=practice_frame(*app);
        if(app->practice_session&&app->stage_scene&&!app->practice_visible&&app->practice_result){
            for(int y=112;y<302;++y)std::fill_n(app->composed.pixels.begin()+y*640+48,352,0x111111);
            using PracticeResult=soew::PracticeSession::Result;
            const auto result=app->practice_session->simulation_error()?tr(L"練習中断：エラー",L"Practice Stopped: Error",L"练习中断：出错",L"練習中斷：發生錯誤"):
                app->practice_session->result()==PracticeResult::Success?tr(L"練習クリア",L"Practice Clear",L"练习通关",L"練習通關"):tr(L"練習ゲームオーバー",L"Practice Game Over",L"练习失败",L"練習失敗");
            draw(result,80,128,0xffffff,304);
            const bool random=app->practice_session->recipe.seed_mode==soew::PracticeSeedMode::Random;
            const wchar_t *items[]={random?tr(L"R　新しいシードで再挑戦",L"R  Retry with new seed",L"R  用新种子重试",L"R  用新種子重試"):tr(L"R　同じシードで再挑戦",L"R  Retry same seed",L"R  用相同种子重试",L"R  用相同種子重試"),words::save_replay(lang),tr(L"Q　練習メニュー",L"Q  Practice Menu",L"Q  练习菜单",L"Q  練習選單"),tr(L"タイトルへ戻る",L"Return to Title",L"返回标题画面",L"返回標題畫面")};
            for(int i=0;i<4;++i){
                const bool disabled=i==1&&(!app->recorder||!app->recorder->can_save());
                draw(items[i],80,164+i*32,disabled?0x777777:app->practice_choice==i?0xffff00:0xaaaaaa,304);
            }
        }
        if(app->practice_assist.open&&live_assist_controls(*app))
            draw_practice_assist(app->composed,app->practice_assist,lang);
        if(app->replay_menu.active())compose_replay_menu(*app);
        if(app->legacy_content&&app->menu_visible&&!app->opening.active&&!app->menu.options&&!app->menu.shot_screen&&!app->menu.music_room&&!app->scores_visible&&!app->stage_scene&&!app->ending_surface&&!app->practice_visible&&!app->replay_menu.active()){
            const auto warning=words::legacy_content(lang);
            draw(warning,320-display_text_width(warning,lang)/2,196,0xffb030);
        }
        }
        }
        }
        const auto &frame=app->composed;
        const auto view=fit(client.right,client.bottom,app->crt);
        const auto prepared=[&]{
            const HostPerf::Span span(app->host_perf.get(),HostPerf::Metric::PresentPrepare);
            return app->presenter.prepare(dc,client.right,client.bottom,frame.pixels,640,400,view.x,view.y,view.w,view.h);
        }();
        if(prepared){
            const auto *demo=app->title_demo?app->title_demo->controller.scene():nullptr;
            const bool active_play=app->stage_scene&&!app->stage_scene->paused&&!app->stage_intro.active()&&
                !app->ending_player&&!app->replay_menu.active()&&!app->replay_seek&&
                !app->practice_retry&&app->score_review==App::ScoreReview::None;
            const bool show_fps=app->focused&&!app->minimized&&!app->resizing&&
                (active_play||(demo&&!demo->paused))&&view.w>16&&view.h>16;
            if(show_fps){
                const auto fps=app->frame_rate.tenths(host_us());
                app->presenter.corner_label(fps?L"FPS "+std::to_wstring(*fps/10)+L"."+std::to_wstring(*fps%10):L"FPS --",
                    {view.x,view.y,view.x+view.w-8,view.y+view.h-8});
            }else app->frame_rate.reset();
            {const HostPerf::Span span(app->host_perf.get(),HostPerf::Metric::PresentCommit);app->presenter.commit(dc);}
            if(show_fps)app->frame_rate.submitted(host_us());
            if(app->host_perf)app->host_perf->submitted();
            app->last_paint_practice=app->practice_session&&!app->practice_visible;app->retry_probe.presented(*app);
        }
        ++app->paints;
        }catch(const std::exception &error){
            // Never unwind through USER32, and always balance Begin/EndPaint.
            app->failed=true;std::cerr<<"Presentation failed: "<<error.what()<<'\n';
            EndPaint(hwnd,&ps);PostMessageW(hwnd,WM_CLOSE,0,0);return 0;
        }
        EndPaint(hwnd, &ps); return 0;
    }
    case WM_TIMER:
        if(w==3&&app->async_practice_test){
            const auto now=host_ms();
            auto fail=[&](const char *reason){std::cerr<<"async_practice_failure="<<reason<<" step="<<app->async_practice_step<<" elapsed_ms="<<now-app->async_practice_started<<'\n';app->failed=true;DestroyWindow(hwnd);};
            if(now-app->async_practice_started>120000){fail("timeout");return 0;}
            if(app->retry_probe.mode>=0){app->retry_probe.step(hwnd,*app);return 0;}
            auto start=[&](int music){app->menu.music=music;app->practice_visible=true;app->practice_panel.open(app->practice_menu,soew::PracticePanel::Page::Setup);app->practice_menu.allowed=true;app->async_practice_phase_started=now;SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);};
            auto log=[&](const char *label){std::cout<<"async_practice="<<label<<" elapsed_ms="<<now-app->async_practice_phase_started<<" total_ms="<<now-app->async_practice_started<<'\n';};
            switch(app->async_practice_step){
            case 0:
                app->menu.reduced=true;
                app->practice_menu.spec=soew::PracticeSpec{};app->practice_menu.spec.stage=1;app->practice_menu.spec.section=soew::PracticeSection::Chapter;app->practice_menu.spec.chapter_id=50;
                if(!soew::validate_practice(app->practice_menu.spec,app->practice_menu.capabilities).empty()){fail("invalid chapter 50");break;}
                start(1);if(!app->practice_load){fail("scene future not started");break;}SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);if(!app->practice_load_cancelled){fail("scene cancellation not recorded");break;}log("scene_cancel_requested");app->async_practice_step=1;break;
            case 1:
                if(app->practice_load||app->practice_audio_load)break;
                if(app->stage_scene||app->practice_session||app->practice_builder||app->practice_load_cancelled){fail("cancelled scene published");break;}
                log("scene_cancel_drained");start(1);if(!app->practice_load){fail("restart scene future missing");break;}app->async_practice_step=2;break;
            case 2:
                if(app->practice_audio_load&&app->async_exit_test){std::cout<<"async_exit=destroy_during_audio\n";DestroyWindow(hwnd);break;}
                if(app->practice_audio_load){SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);if(!app->practice_load_cancelled){fail("audio cancellation not recorded");break;}log("audio_cancel_requested");app->async_practice_step=3;}
                else if(app->stage_scene||(!app->practice_load&&!app->practice_builder)){fail("audio future not reached");}
                break;
            case 3:
                if(app->practice_load||app->practice_audio_load)break;
                if(app->stage_scene||app->practice_session||app->practice_builder||app->practice_load_cancelled){fail("cancelled audio published");break;}
                log("audio_cancel_drained");start(1);if(!app->practice_load){fail("FM scene future missing");break;}app->async_practice_step=4;break;
            case 4: case 5:
                if(app->stage_scene&&app->practice_session){
                    const int music=app->async_practice_step==4?1:2;
                    const auto measure=soew::practice_music_measure(app->practice_session->recipe);
                    if(app->practice_session->recipe.stage!=1||app->practice_session->recipe.chapter_id!=50||!app->stage_scene->reduced_effects||!app->audio.active()||app->midi_mode!=(music==2)||app->audio.requested_measure()!=measure||app->audio.positioned_measure()!=measure){fail("published scene or audio mismatch");break;}
                    log(music==1?"fm_measure_90":"midi_measure_90");
                    if(music==1){leave_practice(*app,false);start(2);if(!app->practice_load){fail("MIDI scene future missing");break;}app->async_practice_step=5;}
                    else {app->stage_scene->meira_appearance=false;app->stage_scene->meira_enabled=false;app->stage_scene->preboss_started=false;app->stage_scene->rika_enabled=false;app->gameplay_audio_attempted=-1;sync_gameplay_bgm(*app,true);if(!app->gameplay_audio_load){fail("gameplay audio future missing");break;}app->async_practice_step=6;}
                }else if(!app->practice_load&&!app->practice_builder&&!app->practice_audio_load){fail("practice preparation failed");}break;
            case 6:
                sync_gameplay_bgm(*app,false);
                if(app->gameplay_audio_load)break;
                if(app->playing_track!=soew::gameplay_music_track(1,false,false)||!app->audio.paused()){fail("gameplay audio publication mismatch");break;}
                app->stage_scene->meira_appearance=true;sync_gameplay_bgm(*app,false);if(!app->gameplay_audio_load){fail("boss audio future missing");break;}app->async_practice_step=7;break;
            case 7:
                sync_gameplay_bgm(*app,false);if(app->gameplay_audio_load)break;
                if(app->playing_track!=7||!app->audio.paused()){fail("boss audio publication mismatch");break;}
                log("gameplay_stage_to_boss");leave_practice(*app,false);app->practice_menu.spec.chapter_id=0;app->practice_menu.spec.seed_mode=soew::PracticeSeedMode::Fixed;start(0);app->async_practice_step=8;break;
            case 8: case 11:{
                if(!app->practice_session)break;
                const bool random=app->async_practice_step==11;
                if(!app->recorder){fail("recording not created");break;}
                app->retry_start_hash=app->recorder->replay().practice->start_hash;app->retry_prior_seed=app->practice_session->seed();
                app->retry_prior_starts=app->gameplay_audio_starts;
                SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
                const auto pause_pixels=app->composed.pixels;const auto ticks=app->practice_session->samples();
                SendMessageW(hwnd,WM_KEYDOWN,'R',0);SendMessageW(hwnd,WM_KEYDOWN,'R',1LL<<30);SendMessageW(hwnd,WM_KEYDOWN,'R',0);
                InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
                if(!app->practice_retry||app->practice_visible||!app->practice_load||app->recorder||app->composed.pixels!=pause_pixels||app->practice_session->samples()!=ticks){fail("retry did not hold previous frame and recording boundary");break;}
                SendMessageW(hwnd,WM_KILLFOCUS,0,0);app->async_practice_step=random?12:9;break;
            }
            case 9: case 12:
                // Hold publication until the asynchronous result is ready while unfocused.
                if(!app->practice_audio_load||app->practice_audio_load->wait_for(std::chrono::seconds(0))!=std::future_status::ready)break;
                advance_practice(*app);if(!app->practice_retry||app->gameplay_audio_starts!=app->retry_prior_starts){fail("published while unfocused");break;}
                SendMessageW(hwnd,WM_SETFOCUS,0,0);app->async_practice_step++;break;
            case 10: case 13:{
                if(app->practice_retry)break;
                const bool random=app->async_practice_step==13;
                if(!app->recorder||!app->practice_session||!app->recorder->replay().events.empty()||app->practice_session->samples()||app->gameplay_audio_starts!=app->retry_prior_starts+1||app->practice_escape||app->practice_visible){fail("retry published with samples or wrong count");break;}
                if(random?(((app->practice_session->seed()^app->retry_prior_seed)&0xffffffu)==0):(app->recorder->replay().practice->start_hash!=app->retry_start_hash||app->practice_session->seed()!=app->retry_prior_seed)){fail("retry seed or starting digest mismatch");break;}
                if(!random){leave_practice(*app,false);app->practice_menu.spec.seed_mode=soew::PracticeSeedMode::Random;start(0);app->async_practice_step=11;}
                else{SendMessageW(hwnd,WM_KEYDOWN,'R',0);SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->async_practice_step=14;}
                break;
            }
            case 14:
                if(app->practice_load||app->practice_audio_load)break;
                if(!app->practice_visible||app->practice_retry||app->recorder||app->stage_scene||app->practice_session){fail("cancelled retry published");break;}
                log("retry_pause_pixels_focus_fixed_random_cancel");DestroyWindow(hwnd);break;
            }
            return 0;
        }
        if(w==2){
            const auto now=host_ms();
            ++app->timer_callbacks;
            if(!app->stage_scene||!app->gameplay_audio_enabled||app->practice_builder||app->practice_load||app->practice_audio_load)drain_gameplay_audio(*app);
            if(app->focused&&GetForegroundWindow()==hwnd&&practice_keys_released())app->practice_menu.update(0);
            const bool enabled=(app->timed_test || app->retry_probe.mode>=0 || app->score_timed_test || (app->focused&&GetForegroundWindow()==hwnd))&&!app->minimized&&!app->resizing; // timed test controls activity; normal runs require real foreground
            if(app->host_perf){app->host_perf->timer(host_perf_phase(*app,enabled));host_perf_resources(*app);}
            const bool replay_menu_active=app->replay_menu.active();
            const auto previous_bgm=app->gameplay_audio_attempted;
            if(!replay_menu_active&&app->stage_scene&&app->gameplay_audio_enabled&&!app->practice_builder&&!app->practice_load&&!app->practice_audio_load){sync_gameplay_bgm(*app,enabled);if(previous_bgm!=app->gameplay_audio_attempted)caption(hwnd,*app);}
            if(app->audio.active()){
                const HostPerf::Span span(app->host_perf.get(),HostPerf::Metric::AudioPump);
                if(app->host_perf)app->host_perf->audio_queue(app->audio.queued());
                if((!app->stage_scene&&!app->audio.pause(!enabled||app->practice_builder.has_value()||app->practice_load.has_value()||app->practice_audio_load.has_value()))||!app->audio.pump()){audio_error(*app);caption(hwnd,*app);InvalidateRect(hwnd,nullptr,FALSE);}
            }
            app->retry_probe.audio_pumped(*app);
            if(advance_title_demo(hwnd,*app,now))return 0;
            if(replay_menu_active){
                app->staff_music_poll=0;
                app->stage_clock.advance(now,false);
                if(enabled&&practice_keys_released()&&!(GetAsyncKeyState(VK_BACK)&0x8000))app->replay_menu.update(0,app->replay_catalog);
                if(app->replay_seek){
                    advance_replay_seek(*app);InvalidateRect(hwnd,nullptr,FALSE);
                    if(!app->replay_seek)caption(hwnd,*app);
                }
                return 0;
            }
            if(app->practice_builder||app->practice_load||app->practice_audio_load){advance_practice(*app);if(app->async_practice_test)PostMessageW(hwnd,WM_TIMER,3,0);if(!app->practice_retry){InvalidateRect(hwnd,nullptr,FALSE);caption(hwnd,*app);}return 0;}
            if(app->score_review!=App::ScoreReview::None){
                app->stage_clock.advance(now,false);
                if(app->score_review==App::ScoreReview::GameOver){
                    namespace g=soew::score_gaiji;
                    const auto steps=app->score_clock.advance(now,enabled);
                    const bool extra_core_wait=app->stage_scene->rank==4&&!app->replay_session->summary();
                    const auto end=g::animation_frames-(extra_core_wait?g::hold_frames:0u);
                    app->game_over_frame=std::min(end,app->game_over_frame+steps);
                    // Extra already has a 30-tick hold and a replay-visible acknowledgement gate.
                    // Let those original ticks do their job after the host-only convergence.
                    if(extra_core_wait&&app->game_over_frame>=end)release_score_controls(*app);
                    if(steps)InvalidateRect(hwnd,nullptr,FALSE);
                }
                if(enabled&&score_keys_released()&&(app->score_review!=App::ScoreReview::GameOver||app->game_over_frame>=soew::score_gaiji::animation_frames))
                    app->score_review_armed=true;
                return 0;
            }
            if(app->score_release){
                app->stage_clock.advance(now,false);
                if(enabled&&score_keys_released())app->score_release=false;
                return 0;
            }
            const auto held=[app](int key){return app->retry_probe.mode<0&&(GetAsyncKeyState(key)&0x8000)!=0;};
            const bool z_held=held('Z');
            const bool fast_replay=app->replay_playback&&z_held;
            if(app->stage_scene){
                if(app->stage_intro.active()){
                    const auto prior_intro_frame=app->stage_intro.elapsed;
                    app->stage_intro.advance(now,enabled&&!app->stage_scene->paused,soew::replay_view_speed(fast_replay));
                    if(app->stage_intro.elapsed!=prior_intro_frame)InvalidateRect(hwnd,nullptr,FALSE);
                    if(!app->stage_intro.active()){
                        app->retry_probe.intro_finished();
                        app->stage_clock.reset(now);app->stage_entry_release=true;
                        app->z_input.clear();app->dialogue_hold.clear();
                        app->mima_dialogue_tap.reset((GetAsyncKeyState('Z')&0x8000)!=0);
                        InvalidateRect(hwnd,nullptr,FALSE);
                    }
                    return 0; // Pure presentation: no Tick, input sample or replay event.
                }
                if(app->practice_retry_release){
                    // Diagnostic taps use synchronous down/up messages, not the
                    // desktop's physical key state. Keep the real input gate intact.
                    if(!enabled||(app->retry_probe.mode<0&&!practice_keys_released())){
                        app->stage_clock.reset(now);app->z_input.clear();app->dialogue_hold.clear();return 0;
                    }
                    app->practice_retry_release=false;
                }
                const auto pace=app->stage_scene->presentation_slowdown();
                const bool timing_active=enabled&&app->recorder&&!app->replay_playback&&!app->stage_scene->paused&&
                    !app->stage_entry_release&&!app->ending_player&&!app->stage_scene->life.game_over&&
                    !app->stage_scene->transition.active()&&!app->stage_scene->dialogue.active()&&
                    !app->stage_scene->continue_prompt()&&soew::replay_tick_context(*app->stage_scene)==soew::NativeReplayContext::Gameplay;
                HostTimingSample timing;
                const bool stage_active=enabled&&!app->stage_scene->paused;
                unsigned base_steps=0;
                if(app->display_frames_active&&stage_active){
                    base_steps=app->locked_clock.advance(pace);timing=app->locked_timing;
                }else base_steps=app->stage_clock.advance(now,stage_active,pace,(timing_active||app->host_perf)?&timing:nullptr);
                if(app->display_frames_active&&stage_active)
                    app->scroll_phase=soew::display_subframe(app->scroll_phase,app->locked_clock.subframe(pace,soew::scroll_subpixels),base_steps);
                app->recording_timing.observe(timing_active,timing);
                if(app->host_perf)app->host_perf->schedule(pace,stage_active,base_steps,timing.lost_ms,timing.active_ms);
                // Replay never reads from the live stage-entry release gate.
                const auto stride=soew::replay_view_speed(fast_replay);
                const auto steps=base_steps*stride;
                soew::Keys pressed;
                bool dialog_group=false;
                if(!app->replay_playback){
                    pressed.z=false;pressed.space=held(VK_SPACE);pressed.x=held('X');pressed.enter=held(VK_RETURN);pressed.q=held('Q');
                    // PC98 key_delay polls BIOS groups5/3/0, not arrows/space.
                    for(int key:{int('Z'),int('X'),int('C'),int('V'),int('B'),int('N'),int('M'),VK_OEM_6,int('O'),int('P'),VK_OEM_3,VK_OEM_4,VK_RETURN,int('A'),int('S'),int('D'),VK_ESCAPE,int('1'),int('2'),int('3'),int('4'),int('5'),int('6'),int('7')})dialog_group|=held(key);
                    pressed.up=held(VK_UP);pressed.down=held(VK_DOWN);pressed.left=held(VK_LEFT);pressed.right=held(VK_RIGHT);
                    pressed.focus=held(VK_SHIFT);
                    pressed.n8=held(VK_NUMPAD8);pressed.n2=held(VK_NUMPAD2);pressed.n4=held(VK_NUMPAD4);pressed.n6=held(VK_NUMPAD6);pressed.n7=held(VK_NUMPAD7);pressed.n9=held(VK_NUMPAD9);pressed.n1=held(VK_NUMPAD1);pressed.n3=held(VK_NUMPAD3);
                    if(app->stage_entry_release){if(!z_held&&!held(VK_SPACE)&&!held('X')&&!held(VK_RETURN)&&!held('R')&&!held(VK_ESCAPE))app->stage_entry_release=false;else{pressed.z=pressed.space=pressed.x=pressed.enter=false;app->z_input.clear();}}
                    if(!z_held&&app->z_input.blocked)app->z_input.event(false,shot_mode(*app));
                    if(!enabled||app->stage_scene->paused){app->z_input.clear();app->dialogue_hold.clear();app->mima_dialogue_tap.reset(z_held);}
                }
                unsigned iterations=0;
                {
                const HostPerf::Span span(steps?app->host_perf.get():nullptr,HostPerf::Metric::Simulation);
                for(unsigned i=0;i<steps&&app->stage_scene;++i){
                    ++iterations;
                    if(app->display_frames_active&&app->menu.smooth_background&&i%stride==0){
                        app->stage_scene->capture_motion(app->motion_before);
                        app->motion_stride=stride;
                    }
                    app->stage_scene->set_hitbox_mode(app->menu.hitboxes);
                    soew::ReplayStep step;
                    if(app->replay_playback){
                        step=app->replay_session->step_playback(*app->stage_scene,*app->replay_playback,app->replay_cursor);
                    }else{
                        pressed.z=!app->stage_entry_release&&app->z_input.tick(shot_mode(*app));
                        auto input=soew::sense(pressed);
                        bool group=dialog_group&&!app->stage_entry_release;
                        bool fast=false;
                        if(const auto *dialogue=live_mima_tap_dialogue(*app)){
                            app->dialogue_hold.clear();
                            const auto tap=app->mima_dialogue_tap.poll(*dialogue);
                            input=tap.accelerate?soew::Shot:0;group=tap.group;
                        }else{
                            app->mima_dialogue_tap.reset(z_held);
                            fast=app->dialogue_hold.poll(now,!app->stage_entry_release&&z_held&&app->stage_scene->dialogue.active());
                        }
                        apply_live_assists(*app);
                        if(app->practice_session){
                            const auto event=soew::practice_replay_tick(*app->stage_scene,input,group,fast);
                            const auto samples=app->practice_session->samples();
                            app->practice_session->tick(*app->stage_scene,event.input,event.dialogue_group,event.dialogue_fast_forward);
                            if(app->recorder&&app->practice_session->samples()!=samples&&!app->recorder->record(event))app->recorder.reset();
                            if(app->practice_session->result()!=soew::PracticeSession::Result::Running){
                                if(app->practice_session->simulation_error()){
                                    app->recorder.reset();
                                    std::cerr<<"Practice simulation stopped: "<<app->practice_session->error()<<'\n';
                                    app->notice=app->tr(L"シミュレーションのエラーで練習を停止しました",L"Practice stopped: simulation error",L"模拟出错，练习已停止",L"模擬發生錯誤，練習已停止");
                                }
                                if(app->stage_scene->boss_terminal_frame()){InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);}
                                app->practice_result=true;app->practice_menu.allowed=false;app->practice_choice=0;app->stage_scene->paused=true;app->audio.pause(true);break;
                            }
                        }else{
                            step=app->replay_session->tick(*app->stage_scene,soew::normalize_replay_tick(input,group,fast));
                            if(app->recorder&&step.consumed_tick&&(!app->recorder->record(step.event)||(step.stage_advanced&&!app->recorder->next_stage())))app->recorder.reset();
                            finalize_replay(*app);
                        }
                    }
                    if(step.status==soew::ReplayStepStatus::Failed){
                        app->stage_scene->invalidate_hitboxes();
                        std::cerr<<"Run step failed: "<<step.error<<'\n';
                        if(app->replay_playback)exit_replay_playback(*app,app->tr(L"リプレイの内容が一致しませんでした",L"Replay mismatch",L"录像内容不一致",L"錄影內容不一致"));
                        else {app->recorder.reset();app->notice=app->tr(L"プレイを停止しました",L"Run stopped",L"游戏已停止",L"遊戲已停止");app->stage_scene->paused=true;}
                        caption(hwnd,*app);break;
                    }
                    if(step.stage_advanced){app->stage_entry_release=true;app->z_input.clear();app->dialogue_hold.clear();app->mima_dialogue_tap.reset(held('Z'));}
                    if(intercept_live_game_over(*app,step.status)){caption(hwnd,*app);break;}
                    if(step.status==soew::ReplayStepStatus::Finished){
                        const bool ending=(!app->replay_playback||!app->replay_playback->practice)&&
                            (app->stage_scene->stage5_result||app->stage_scene->extra_settlement.terminal());
                        if(ending){
                            try{start_stage5_ending(*app);}catch(const std::exception &e){return_to_title(*app);std::cerr<<"Ending setup: "<<e.what()<<'\n';}
                        }else if(app->replay_playback){exit_replay_playback(*app,app->tr(L"リプレイが終了しました",L"Replay finished",L"录像回放结束",L"錄影重播結束"));}
                        else if(!begin_replay_save(*app,soew::ReplaySaveOrigin::GameOver))return_to_title(*app);
                        else app->stage_scene->paused=true;
                        caption(hwnd,*app);break;
                    }
                    if(app->gameplay_audio_enabled){const auto prior=app->gameplay_audio_attempted;sync_gameplay_bgm(*app,enabled);if(prior!=app->gameplay_audio_attempted)caption(hwnd,*app);}
                    // Present the last Marisa frame before a catch-up tick can replace
                    // it with dialogue. Time pacing never adds simulation samples.
                    if(app->stage_scene->boss_terminal_frame()){
                        InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
                        app->stage_clock.defer_batch(now);break;
                    }
                    if((app->stage_scene->marisa_enabled||app->stage_scene->stones_enabled)&&app->stage_scene->presentation_slowdown()!=pace){
                        app->stage_clock.defer_batch(now);break;
                    }
                }
                }
                if(app->host_perf)app->host_perf->simulation_steps(iterations);
                if(steps||app->display_frames_active){
                    InvalidateRect(hwnd,nullptr,FALSE);
                    // The benchmark posts synthetic timer messages. Commit its
                    // first playable frame now instead of letting queued test
                    // messages starve the lower-priority WM_PAINT.
                    if(app->retry_probe.waiting&&app->retry_probe.seen&&app->retry_probe.first_paint==RetryHostProbe::Clock::time_point{})
                        UpdateWindow(hwnd);
                }
            }
            if(app->ending_player){
                const auto steps=app->stage_clock.advance(now,enabled);
                const bool live_key=!app->replay_playback&&!practice_keys_released();
                const bool fast_live_staff=enabled&&app->focused&&GetForegroundWindow()==hwnd&&z_held;
                try{
                    if(app->replay_menu.active()){app->staff_music_poll=0;return 0;}
                    std::optional<unsigned> music_measure;
                    if(app->staff_song_started&&app->ending_player->staff_playing()){
                        const auto previous=app->staff_music_poll;
                        app->staff_music_poll=enabled?now:0;
                        const auto elapsed=enabled&&previous?static_cast<unsigned>(std::min<ULONGLONG>(now-previous,1000)):0;
                        const bool active=!app->audio_test_silent&&app->menu.music!=0&&app->playing_track==12&&app->audio.active()&&
                            app->audio.playback_generation()==app->staff_song_generation;
                        music_measure=app->staff_music.update(active,app->audio.completed_frames(),app->audio.completed_measure(),elapsed);
                    }
                    const auto render_ending=[&](const auto &step){app->ending_surface->apply(step,app->ending_player->script().verdict);};
                    for(unsigned i=0;i<steps&&!app->ending_player->finished();++i){
                        const bool at_verdict=app->replay_playback?
                            app->ending_player->advance_replay_frame(fast_replay,music_measure,render_ending):
                            app->ending_player->advance_display_frame({live_key,live_key,music_measure},fast_live_staff,render_ending);
                        if(at_verdict){app->stage_clock.defer_batch(now);break;}
                    }
                    if(app->ending_player->finished()&&app->score_review==App::ScoreReview::None){
                        if(app->replay_playback)exit_replay_playback(*app,app->tr(L"リプレイが終了しました",L"Replay finished",L"录像回放结束",L"錄影重播結束"));
                        else finish_score_flow(*app,soew::ReplaySaveOrigin::Clear);
                        caption(hwnd,*app);
                    }
                }catch(const std::exception &e){
                    return_to_title(*app);std::cerr<<"Ending playback: "<<e.what()<<'\n';
                    app->notice=app->tr(L"エンディングの再生を停止しました",L"Ending playback stopped",L"已停止播放结局",L"已停止播放結局");caption(hwnd,*app);
                }
                if(steps)InvalidateRect(hwnd,nullptr,FALSE);
                return 0;
            }
            const auto score_steps=app->score_clock.advance(now,enabled&&app->scores_visible);
            for(unsigned i=0;i<score_steps;++i)if(app->score_animation.tick()){app->scores_visible=false;app->menu.selected=2;app->menu.allowed=false;app->notice.clear();caption(hwnd,*app);break;}
            if(score_steps)InvalidateRect(hwnd,nullptr,FALSE);
            const auto steps=app->clock.advance(now,enabled&&app->opening.active);
            for(unsigned i=0;i<steps;++i){
                app->opening.tick();
                if(app->title_effect_phase==0&&app->opening.frame>=Opening::hold_end){
                    app->title_effect_phase=1;
                    // OFF submits neither the opening effect nor music. Avoid opening
                    // a silent output device during the visible opening animation.
                    if(app->menu.music!=0){start_preview(*app,0,true);app->audio.effect(6);}
                }
                if(!app->title_audio_started&&app->opening.frame>=Opening::flash_end){
                    app->title_audio_started=true;
                    if(app->menu.music!=0)start_preview(*app,0);
                }
            }
            if(steps)InvalidateRect(hwnd,nullptr,FALSE);
            // Permit normal first input after focus return only if no control is held.
            if(enabled&&!app->menu.allowed){
                bool held=false;for(int key:{VK_UP,VK_DOWN,VK_LEFT,VK_RIGHT,VK_NUMPAD8,VK_NUMPAD2,VK_NUMPAD4,VK_NUMPAD6,int('Z'),VK_SPACE,VK_RETURN,VK_ESCAPE})held|=(GetAsyncKeyState(key)&0x8000)!=0;
                if(app->menu.options)held=held||(GetAsyncKeyState(VK_TAB)&0x8000)||(GetAsyncKeyState('X')&0x8000);
                if(!held)app->menu.update(0);
            }
            if(app->score_timed_test){
                if(!app->scores_visible){app->check(app->score_animation.frame==2001);app->check(app->menu.selected==2);std::cout<<"score_auto_return_ms="<<now-app->test_started<<'\n';DestroyWindow(hwnd);}
                else if(now-app->test_started>5000){app->check(false);DestroyWindow(hwnd);}
            }
            if(app->timed_test){
                const auto elapsed=now-app->test_started;
                if(app->test_phase==0&&elapsed>=300){app->pause_frame=app->opening.frame;SendMessageW(hwnd,WM_ENTERSIZEMOVE,0,0);app->test_phase=1;}
                if(app->test_phase==1&&elapsed>=600){app->check(app->opening.frame==app->pause_frame);SendMessageW(hwnd,WM_EXITSIZEMOVE,0,0);app->test_phase=2;}
                if(!app->opening.active){app->check(app->opening.frame==Opening::end);app->check(app->timer_callbacks>10);std::cout<<"timed_opening_ms="<<now-app->test_started<<" callbacks="<<app->timer_callbacks<<'\n';DestroyWindow(hwnd);}
                else if(now-app->test_started>10000){app->check(false);std::cout<<"timed_test_timeout foreground="<<enabled<<'\n';DestroyWindow(hwnd);}
            }
            return 0;
        }
        if(app->smoke) {
            KillTimer(hwnd, 1);
            for(unsigned f:{0u,20u,49u,67u,68u,73u,78u,85u,100u,121u}) {
                app->opening.active=true;app->opening.frame=f;
                const auto frame=opening_frame(*app);app->check(frame.pixels.size()==640*400);
                InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
            }
            SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);app->check(!app->opening.active);
            SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);
            app->menu.rank=3;app->menu.music=2;app->menu.lives=4;app->menu.bombs=1;app->menu.reduced=true;
            SendMessageW(hwnd,WM_KEYDOWN,VK_F3,0);
            app->check(app->opening.active&&app->opening.frame==0&&app->menu.rank==3&&app->menu.music==2&&app->menu.lives==4&&app->menu.bombs==1&&app->menu.reduced);
            SendMessageW(hwnd,WM_KILLFOCUS,0,0);app->check(!app->focused);
            SendMessageW(hwnd,WM_SETFOCUS,0,0);app->check(app->focused&&!app->menu.allowed);
            SendMessageW(hwnd,WM_ENTERSIZEMOVE,0,0);app->check(app->resizing);
            SendMessageW(hwnd,WM_EXITSIZEMOVE,0,0);app->check(!app->resizing);
            app->menu=Menu{};app->keys=0;
            app->opening.skip();
            app->menu_visible=false;
            for(const auto size : std::array<std::array<int, 2>, 5>{{{800, 600}, {1200, 500}, {400, 800}, {640, 400}, {1000, 700}}}) {
                SetWindowPos(hwnd, HWND_TOPMOST, 20, 20, size[0], size[1], SWP_SHOWWINDOW);
                RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
                for(const auto key : std::array<WPARAM, 5>{'1', '2', '3', VK_F2, VK_F2}) {
                    SendMessageW(hwnd, WM_KEYDOWN, key, 0);
                    InvalidateRect(hwnd, nullptr, FALSE); UpdateWindow(hwnd);
                    RECT r{}; GetClientRect(hwnd, &r); auto v = fit(r.right, r.bottom, app->crt);
                    app->check(v.w > 0 && v.h > 0 && v.x >= 0 && v.y >= 0 && v.x + v.w <= r.right && v.y + v.h <= r.bottom);
                    GdiFlush(); DwmFlush();
                    const auto dc = GetDC(hwnd);
                    const auto &f = app->frames[app->scene];
                    const auto sampled = GetPixel(dc, v.x + v.w / 2, v.y + v.h / 2);
                    ReleaseDC(hwnd, dc);
                    // GDI stretch rounding can select either adjacent source pixel.
                    bool matches = false;
                    for(int sy = f.h / 2 - 1; sy <= f.h / 2 + 1; ++sy) for(int sx = f.w / 2 - 1; sx <= f.w / 2 + 1; ++sx) {
                        const auto pixel = f.pixels[std::size_t(sy) * f.w + sx];
                        matches |= sampled == RGB((pixel >> 16) & 255, (pixel >> 8) & 255, pixel & 255);
                    }
                    if(!matches) std::cout << "pixel_mismatch scene=" << app->scene << " actual=" << sampled << '\n';
                    app->check(matches);
                }
            }
            app->check(app->scene == 2 && !app->crt && app->keys == 25 && app->paints >= 25 && app->sizes >= 5);
            app->menu_visible=true; app->menu.update(0);
            SendMessageW(hwnd,WM_KEYDOWN,VK_DOWN,0); app->check(app->menu.selected==6);
            SendMessageW(hwnd,WM_KEYDOWN,VK_DOWN,1LL<<30); app->check(app->menu.selected==6);
            SendMessageW(hwnd,WM_KEYUP,VK_DOWN,0);
            SendMessageW(hwnd,WM_KEYDOWN,VK_UP,0); app->check(app->menu.selected==0);
            SendMessageW(hwnd,WM_KEYUP,VK_UP,0);
            SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0); app->check(app->menu.shot_screen && app->menu.shot==1);
            SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);
            SendMessageW(hwnd,WM_KEYDOWN,VK_LEFT,0);app->check(app->menu.shot==0);
            SendMessageW(hwnd,WM_KEYUP,VK_LEFT,0);
            SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);app->check(app->menu.shot_confirmed&&app->stage_scene.has_value());
            if(app->stage_scene){const auto old_x=app->stage_scene->player.x;app->stage_scene->tick(soew::Right);app->check(app->stage_scene->player.x>old_x);}
            UpdateWindow(hwnd);
            SendMessageW(hwnd,WM_KEYDOWN,'P',0);app->check(app->stage_scene&&app->stage_scene->paused);
            SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);
            SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->check(app->run_pause_menu&&app->stage_scene&&app->stage_scene->paused);
            SendMessageW(hwnd,WM_KEYUP,VK_ESCAPE,0);SendMessageW(hwnd,WM_KEYDOWN,'Q',0);app->check(!app->stage_scene&&!app->menu.shot_screen&&!app->run_pause_menu);
            SendMessageW(hwnd,WM_KEYUP,VK_ESCAPE,0);
            SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);
            app->menu.selected=3;app->menu.update(0);
            const auto option_key=[&](WPARAM key){SendMessageW(hwnd,WM_KEYDOWN,key,0);SendMessageW(hwnd,WM_KEYUP,key,0);};
            using Option=Menu::Option;
            option_key(VK_RETURN);app->check(app->menu.options&&app->menu.option()==Option::Rank);
            const int initial_rank=app->menu.rank;
            option_key('Z');app->check(app->menu.rank==initial_rank);
            option_key(VK_RIGHT);app->check(app->menu.option()==Option::Rank&&app->menu.rank==Menu::ring(initial_rank,1,4));
            option_key(VK_TAB);option_key(VK_DOWN);option_key(VK_DOWN);
            app->check(app->menu.option()==Option::Fullscreen);
            RECT windowed{},full{},restored{};GetWindowRect(hwnd,&windowed);
            option_key(VK_RIGHT);GetWindowRect(hwnd,&full);
            MONITORINFO monitor{};monitor.cbSize=sizeof(monitor);
            app->check(GetMonitorInfoW(MonitorFromWindow(hwnd,MONITOR_DEFAULTTONEAREST),&monitor)&&EqualRect(&full,&monitor.rcMonitor)&&app->window_fullscreen&&app->menu.fullscreen);
            option_key(VK_RIGHT);GetWindowRect(hwnd,&restored);
            app->check(!app->window_fullscreen&&!app->menu.fullscreen&&EqualRect(&windowed,&restored));
            option_key(VK_DOWN);app->check(app->menu.option()==Option::Hitboxes);
            using Hitboxes=soew::HitboxMode;
            const auto initial_hitboxes=app->menu.hitboxes;
            option_key(VK_RIGHT);option_key(VK_RIGHT);
            app->check(app->menu.hitboxes==static_cast<Hitboxes>(Menu::ring(static_cast<int>(initial_hitboxes),2,3)));
            option_key(VK_LEFT);
            app->check(app->menu.hitboxes==static_cast<Hitboxes>(Menu::ring(static_cast<int>(initial_hitboxes),1,3)));
            option_key(VK_LEFT);app->check(app->menu.hitboxes==initial_hitboxes);
            option_key(VK_DOWN);app->check(app->menu.option()==Option::Language);
            const auto initial_language=app->menu.language;
            InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
            auto initial_options=app->frames[0];compose_options(initial_options,app->font,app->menu,app->settings_warning);
            option_key(VK_RIGHT);
            InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
            auto translated_options=app->frames[0];compose_options(translated_options,app->font,app->menu,app->settings_warning);
            app->check(app->menu.language!=initial_language&&translated_options.pixels!=initial_options.pixels);
            if(!app->evidence_dir.empty()){
                const bool jp=initial_language==soew::Language::Japanese;
                dump(initial_options,app->evidence_dir/(jp?"options-ja.bmp":"options-en.bmp"));
                dump(translated_options,app->evidence_dir/(jp?"options-en.bmp":"options-ja.bmp"));
            }
            option_key(VK_LEFT);
            app->check(app->menu.language==initial_language);
            option_key(VK_TAB);option_key(VK_DOWN);app->check(app->menu.option()==Option::StageIntro&&app->menu.stage_intro);
            option_key(VK_LEFT);app->check(!app->menu.stage_intro);
            option_key(VK_RIGHT);app->check(app->menu.stage_intro);
            option_key(VK_DOWN);option_key(VK_TAB);app->check(app->menu.option()==Option::Back);
            option_key(VK_RETURN);app->check(!app->menu.options&&app->menu.selected==3);
            app->menu.selected=2;app->menu.update(0);
            SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);app->check(app->scores_visible&&app->scores.has_value());
            UpdateWindow(hwnd);
            SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,1LL<<30);app->check(app->scores_visible);
            SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->check(!app->scores_visible&&app->menu.selected==2);
            // In-memory synthetic clear state only; never touch loaded score bytes.
            const Menu saved_menu=app->menu;
            app->menu.unlocked=true;app->menu.cleared_main={true,false,true};app->menu.cleared_extra={false,true,false};app->menu.selected=1;app->menu.update(0);
            SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);app->check(app->menu.shot_screen&&app->menu.extra_mode);
            UpdateWindow(hwnd);
            app->check(app->menu.rules().rank==4&&app->menu.rules().bombs==1&&app->menu.rules().lives==2);
            SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->check(!app->menu.extra_mode&&app->menu.selected==1);
            app->audio_test_silent=true;
            app->menu=saved_menu;app->menu.selected=4;app->menu.update(0);
            SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);app->check(app->menu.music_room);
            // Returning to title now intentionally restarts OP; begin the F4
            // activation test with no preview already playing under its cursor.
            app->audio.stop();
            SendMessageW(hwnd,WM_KEYDOWN,VK_F4,0);app->check(app->fm_enabled&&app->audio.active());
            UpdateWindow(hwnd);
            SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);SendMessageW(hwnd,WM_KEYDOWN,VK_DOWN,0);app->check(app->menu.music_selected==1&&app->menu.music_comment==0);
            SendMessageW(hwnd,WM_KEYUP,VK_DOWN,0);SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);app->check(app->menu.music_comment==1);
            const auto title_song_hash=[&](){std::uint32_t h=2166136261u;for(auto byte:app->songs[0]){h^=std::to_integer<std::uint8_t>(byte);h*=16777619u;}return h;}();
            std::set<std::uint32_t> routed_pcm_hashes;
            for(int track=0;track<15;++track){
                // Deliberately leave stale comment=0 to reproduce the reported path.
                app->menu.music_comment=0;app->menu.music_selected=track;
                if(app->audio.active()&&app->playing_track==track){app->audio.stop();}
                SendMessageW(hwnd,WM_KEYDOWN,VK_F4,0);
                std::uint32_t expected=2166136261u;for(auto byte:app->songs[track]){expected^=std::to_integer<std::uint8_t>(byte);expected*=16777619u;}
                app->check(app->audio.active()&&app->playing_track==track&&app->menu.music_comment==track);
                app->check(app->audio.song_hash()==expected);
                if(track>0)app->check(app->audio.song_hash()!=title_song_hash);
                const auto pcm=app->audio.pcm_hash();app->check(pcm!=2166136261u);routed_pcm_hashes.insert(pcm);
                SendMessageW(hwnd,WM_KEYDOWN,VK_F4,1LL<<30);app->check(app->audio.active()&&app->playing_track==track);
                SendMessageW(hwnd,WM_KEYDOWN,VK_F4,0);app->check(!app->audio.active()&&!app->fm_enabled);
                SendMessageW(hwnd,WM_KEYDOWN,VK_F4,0);app->check(app->audio.active()&&app->audio.pcm_hash()==pcm);
                // Next iteration switches while active, without confirm.
            }
            app->check(routed_pcm_hashes.size()==15);
            app->check(app->audio.effect(2));
            // Native Music Room confirm restarts even the same track; cursor
            // movement and exiting retain the existing song/stream.
            app->menu.music_selected=3;app->menu.update(0);SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);app->check(app->playing_track==3&&app->menu.music_comment==3);const auto room_hash=app->audio.song_hash();
            app->menu.update(0);SendMessageW(hwnd,WM_KEYDOWN,VK_DOWN,0);app->check(app->audio.song_hash()==room_hash&&app->menu.music_comment==3);
            app->menu.music_selected=3;app->menu.update(0);SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);app->check(app->playing_track==3&&app->audio.submitted()==3840);
            app->menu.update(0);SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->check(!app->menu.music_room&&app->playing_track==3&&app->audio.active());
            app->menu.selected=3;app->menu.update(0);option_key(VK_RETURN);option_key(VK_DOWN);
            app->check(app->menu.option()==Option::Music);
            option_key(VK_RIGHT);app->check(app->menu.music==2&&app->midi_mode&&app->playing_track==3);
            option_key(VK_RIGHT);app->check(app->menu.music==0&&app->playing_track==-1&&app->audio.active());
            option_key(VK_RIGHT);app->check(app->menu.music==1&&!app->midi_mode&&app->playing_track==3);
            option_key(VK_ESCAPE);app->check(!app->menu.options&&app->playing_track==3);
            app->menu.selected=4;app->menu.update(0);SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);
            for(unsigned id=1;id<=18;++id)app->check(app->audio.effect(id));
            app->check(app->audio.effect_calls()>=18);
            app->menu.music_selected=16;SendMessageW(hwnd,WM_KEYDOWN,VK_F4,0);app->check(!app->audio.active());
            SendMessageW(hwnd,WM_KEYDOWN,VK_F5,0);app->check(app->midi_mode&&app->audio.active());
            std::set<std::uint32_t> midi_hashes;
            for(int track=0;track<15;++track){
                app->menu.music_selected=track;app->menu.music_comment=0;
                SendMessageW(hwnd,WM_KEYDOWN,VK_F4,0);
                app->check(app->audio.active()&&app->playing_track==track&&app->midi_mode);
                std::uint32_t hash=2166136261u;for(auto byte:app->midi_songs[track]){hash^=std::to_integer<std::uint8_t>(byte);hash*=16777619u;}
                app->check(app->audio.song_hash()==hash);midi_hashes.insert(app->audio.pcm_hash());
            }
            app->check(midi_hashes.size()==15);app->check(app->audio.effect(9));
            SendMessageW(hwnd,WM_KEYDOWN,VK_F5,0);app->check(!app->midi_mode&&app->audio.active()&&app->playing_track==14);
            SendMessageW(hwnd,WM_KEYDOWN,VK_F5,0);app->check(app->midi_mode&&app->audio.active()&&app->playing_track==14);
            UpdateWindow(hwnd);
            SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->check(!app->menu.music_room&&app->menu.selected==4);app->check(app->audio.active());
            UpdateWindow(hwnd);
            // Physical message edges are retained independently of async polling.
            app->menu.autofire=false;launch_stage(*app,0);app->focused=true;
            app->check(app->stage_entry_release&&!app->z_input.tick(false));
            app->stage_entry_release=false; // Simulate the release poll before sending new physical edges.
            app->z_input.event(false,false);
            SendMessageW(hwnd,WM_KEYDOWN,'Z',0);SendMessageW(hwnd,WM_KEYUP,'Z',0);app->check(app->z_input.tick(false));app->check(!app->z_input.tick(false));
            app->menu.autofire=true;app->z_input.event(true,true);app->check(app->z_input.tick(true));SendMessageW(hwnd,WM_KEYUP,'Z',0);app->check(!app->z_input.tick(true)&&app->z_input.edges.empty());
            app->z_input.event(true,true);SendMessageW(hwnd,WM_KILLFOCUS,0,0);app->check(!app->z_input.tick(true));app->menu.autofire=false;app->stage_scene.reset();
            for(int stage=0;stage<6;++stage){
                try{launch_stage(*app,stage);app->check(app->stage_scene.has_value());app->check(app->stage_scene->script.complete()==(stage!=3));UpdateWindow(hwnd);for(unsigned i=0;i<10;++i)app->stage_scene->tick(soew::Up);app->check(app->stage_scene->ticks==10);app->stage_scene.reset();}catch(const std::exception &error){std::cerr<<"scene regression "<<stage<<": "<<error.what()<<'\n';app->check(false);}
            }
            // Repeated unchanged stage paints must preserve full pixels and
            // simulation state. Repainting must not advance the logic clock.
            const auto smoke_repaint=[&]{
                const auto before=app->paints;
                InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
                // An occluded smoke-test window may not get an update region.
                // Exercise the same compositor even in that case.
                if(app->paints==before)SendMessageW(hwnd,WM_PAINT,0,0);
                return app->paints>before;
            };
            launch_stage(*app,0);std::vector<std::uint32_t> expected_stage;
            for(unsigned i=0;i<80;++i){const bool painted=smoke_repaint();if(i==0)expected_stage=app->composed.pixels;app->check(painted&&app->composed.pixels==expected_stage);app->check(app->stage_scene->ticks==0);}
            for(unsigned i=0;i<40;++i)app->stage_scene->tick(soew::Shot);
            app->check(app->stage_scene->shots.fired==7);app->check(app->stage_scene->ticks==40);
            app->menu.hitboxes=soew::HitboxMode::All;app->stage_scene->set_hitbox_mode(app->menu.hitboxes);
            app->stage_scene->tick(0);app->check(smoke_repaint());
            const auto overlay_pixels=app->composed.pixels;const auto overlay_tick=app->stage_scene->ticks;
            app->check(smoke_repaint()&&app->composed.pixels==overlay_pixels&&app->stage_scene->ticks==overlay_tick);
            app->menu.hitboxes=soew::HitboxMode::Off;app->stage_scene->set_hitbox_mode(app->menu.hitboxes);
            app->check(smoke_repaint()&&app->composed.pixels!=overlay_pixels);
            // Keep subsequent historical checks on their original frame schedule.
            launch_stage(*app,0);for(unsigned i=0;i<40;++i)app->stage_scene->tick(soew::Shot);
            InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
            for(unsigned shot_type=0;shot_type<3;++shot_type){app->stage_scene->player.shot=int(shot_type);for(unsigned level=0;level<10;++level){constexpr unsigned power[]={1,4,8,16,24,32,40,52,64,80};app->stage_scene->pickups.power=power[level];app->stage_scene->tick(soew::Shot);InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);app->check(!app->stage_scene->weapon_blocked);}}
            app->stage_scene->pickups.power=1;app->stage_scene->life.register_hit();for(unsigned i=0;i<68;++i)app->stage_scene->tick(0);
            app->stage_scene->enable_bombing();app->stage_scene->life.bombs=1;app->stage_scene->bomb.stage_used=0;
            app->stage_scene->tick(soew::Bomb);app->check(app->stage_scene->bomb.active&&app->stage_scene->life.bombs==0&&app->stage_scene->life.bomb_invincible);app->stage_scene->tick(soew::Bomb);app->check(app->stage_scene->bomb.stage_used==1);
            for(unsigned i=0;i<210&&app->stage_scene->bomb.active;++i){app->stage_scene->tick(0);InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);}
            app->check(!app->stage_scene->bomb.active&&!app->stage_scene->life.bomb_invincible&&app->stage_scene->life.invincibility>0);
            app->check(app->stage_scene->life.stage_misses==1);app->check(!app->stage_scene->life.miss_active);app->check(unsigned(app->stage_scene->life.lives)+1==unsigned(app->menu.lives));
            app->stage_scene->life.lives=0;app->stage_scene->life.invincibility=0;app->stage_scene->life.register_hit();for(unsigned i=0;i<24;++i)app->stage_scene->tick(0);app->check(app->stage_scene->life.game_over);const auto terminal_tick=app->stage_scene->ticks;app->stage_scene->tick(soew::Right);app->check(app->stage_scene->ticks==terminal_tick);
            InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
            // Exercise normal map end and all16 dialogue boxes through actual
            // window paints, without changing the live game entry or resources.
            const auto bgm_before=app->gameplay_audio_starts;
            launch_stage(*app,0);app->check(app->audio.active()&&app->playing_track==1&&app->gameplay_audio_starts==bgm_before+1);
            const auto song_begin=app->audio.song_hash();for(unsigned i=0;i<8;++i)sync_gameplay_bgm(*app,true);app->check(app->gameplay_audio_starts==bgm_before+1);
            SendMessageW(hwnd,WM_KEYDOWN,'P',0);app->check(app->audio.paused());const auto submitted=app->audio.submitted();app->check(app->audio.pump()&&app->audio.submitted()==submitted);
            SendMessageW(hwnd,WM_KEYDOWN,'P',0);app->check(!app->audio.paused());sync_gameplay_bgm(*app,false);app->check(app->audio.paused());sync_gameplay_bgm(*app,true);app->check(!app->audio.paused());
            for(unsigned i=0;i<10000&&!app->stage_scene->dialogue.active();++i){app->stage_scene->life.invincibility=200;app->stage_scene->tick(0);}
            app->check(app->stage_scene->dialogue.active()&&!app->stage_scene->rika_enabled);
            sync_gameplay_bgm(*app,true);app->check(app->audio.active()&&app->playing_track==6&&app->audio.song_hash()!=song_begin&&app->gameplay_audio_starts==bgm_before+2);
            for(unsigned box=0;box<16;++box){while(app->stage_scene->dialogue.phase==soew::StageDialogue::Phase::SlideIn||app->stage_scene->dialogue.phase==soew::StageDialogue::Phase::Text)app->stage_scene->tick(soew::Shot,true);InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);app->check(app->stage_scene->dialogue.current==box);for(unsigned i=0;i<4;++i)app->stage_scene->tick(0,false);for(unsigned i=0;i<2;++i)app->stage_scene->tick(soew::OK,true);}
            for(unsigned i=0;i<24;++i)app->stage_scene->tick(0,false);
            app->check(app->stage_scene->normal_rika_handoff&&app->stage_scene->rika_stage_frame==0);
            sync_gameplay_bgm(*app,true);app->check(app->playing_track==6&&app->gameplay_audio_starts==bgm_before+2);
            for(unsigned i=0;i<100;++i){app->stage_scene->life.invincibility=200;app->stage_scene->tick(0);}
            InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);app->check(app->stage_scene->rika_stage_frame==100);
            app->stage_scene->rika.done=true;app->stage_scene->life.invincibility=0;app->stage_scene->tick(0);app->check(app->stage_scene->dialogue.postboss);
            for(unsigned box=16;box<24;++box){while(app->stage_scene->dialogue.phase==soew::StageDialogue::Phase::SlideIn||app->stage_scene->dialogue.phase==soew::StageDialogue::Phase::Text)app->stage_scene->tick(soew::Shot,true);InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);app->check(app->stage_scene->dialogue.current==box);for(unsigned i=0;i<4;++i)app->stage_scene->tick(0,false);for(unsigned i=0;i<2;++i)app->stage_scene->tick(soew::OK,true);}
            for(unsigned ack=0;ack<2;++ack){InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);if(ack==0&&!app->evidence_dir.empty()){Frame proof{640,400,std::vector<std::uint32_t>(640*400)};app->stage_scene->render(proof.pixels);compose_transition(proof,*app->stage_scene,app->font);dump(proof,app->evidence_dir/"stage1-bonus-proof.bmp");}for(unsigned i=0;i<4;++i)app->stage_scene->tick(0,false);for(unsigned i=0;i<2;++i)app->stage_scene->tick(soew::OK,true);}
            for(unsigned i=0;i<142;++i){app->stage_scene->tick(0);if(i%2==0){InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);}}
            app->check(app->stage_scene->transition.phase==soew::StageTransition::Phase::Ready);apply_next_stage(*app);app->check(app->stage_scene->stage==1&&app->playing_track==2);
            for(unsigned i=0;i<42;++i){app->stage_scene->tick(0);InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);}app->stage_scene->tick(0);app->check(app->stage_scene->ticks==1);InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);if(!app->evidence_dir.empty()){Frame proof{640,400,std::vector<std::uint32_t>(640*400)};app->stage_scene->render(proof.pixels);compose_transition(proof,*app->stage_scene,app->font);dump(proof,app->evidence_dir/"stage2-entry-proof.bmp");}
            SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->check(app->run_pause_menu&&app->stage_scene->paused&&app->audio.paused());
            SendMessageW(hwnd,WM_KEYDOWN,'R',1LL<<30);app->check(app->stage_scene->stage==1);
            SendMessageW(hwnd,WM_KEYDOWN,'R',0);app->check(app->stage_scene->stage==0&&app->stage_scene->ticks==0&&!app->run_pause_menu&&app->stage_entry_release);
            SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);SendMessageW(hwnd,WM_KEYDOWN,'Q',0);
            app->check(!app->stage_scene&&!app->run_pause_menu&&app->playing_track==0);
            // Exercise Meira dialogue/BGM through the same host routing.
            launch_stage(*app,1);app->stage_scene->runtime.scroll.done=true;app->stage_scene->scroll.done=true;app->stage_scene->tick(0);
            for(unsigned box=0;box<24;++box){while(app->stage_scene->dialogue.phase==soew::StageDialogue::Phase::SlideIn||app->stage_scene->dialogue.phase==soew::StageDialogue::Phase::Text||app->stage_scene->dialogue.phase==soew::StageDialogue::Phase::IntroAppearance){app->stage_scene->tick(soew::Shot,true);sync_gameplay_bgm(*app,true);}InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);app->check(app->stage_scene->dialogue.current==box);for(unsigned i=0;i<4;++i)app->stage_scene->tick(0,false);for(unsigned i=0;i<2;++i)app->stage_scene->tick(soew::OK,true);}
            for(unsigned i=0;i<24;++i)app->stage_scene->tick(0);
            sync_gameplay_bgm(*app,true);app->check(app->stage_scene->meira_enabled&&app->playing_track==7);
            for(unsigned i=0;i<500;++i){app->stage_scene->life.invincibility=200;app->stage_scene->tick(0);if(i%16==0){InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);}}
            if(!app->evidence_dir.empty()){Frame proof{640,400,std::vector<std::uint32_t>(640*400)};app->stage_scene->render(proof.pixels);dump(proof,app->evidence_dir/"meira-battle.bmp");}
            SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);SendMessageW(hwnd,WM_KEYDOWN,'Q',0);
            // Also bind both FM songs through the actual stage launch path.
            app->menu.music=1;app->midi_mode=false;launch_stage(*app,0);app->check(app->audio.active()&&app->playing_track==1&&!app->midi_mode);const auto fm_stage_hash=app->audio.song_hash();
            app->stage_scene->preboss_started=true;sync_gameplay_bgm(*app,true);app->check(app->audio.active()&&app->playing_track==6&&app->audio.song_hash()!=fm_stage_hash);
            app->stage_scene->life.game_over=true;sync_gameplay_bgm(*app,true);app->check(app->audio.paused());
            app->stage_scene->life.game_over=false;sync_gameplay_bgm(*app,true);app->check(!app->audio.paused());
            app->stage_scene->rika_enabled=true;app->stage_scene->rika.done=true;sync_gameplay_bgm(*app,true);app->check(!app->audio.paused());
            SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);SendMessageW(hwnd,WM_KEYDOWN,'Q',0);app->check(!app->stage_scene&&app->playing_track==0);
            // The standalone compatibility launch is opt-in and isolated from normal launch.
            app->menu.music=1;launch_stage(*app,3,true);
            app->check(app->stage_scene->script.stage4_compatibility&&!app->stage_scene->script.complete()&&app->stage_scene->transitions_enabled);
            for(unsigned i=0;i<10000&&!app->stage_scene->dialogue.active()&&!app->stage_scene->runtime.blocked;++i){
                app->stage_scene->life.invincibility=200;app->stage_scene->tick(0);
            }
            app->check(app->stage_scene->runtime.scroll.done&&app->stage_scene->dialogue.active()&&!app->stage_scene->runtime.blocked);
            InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
            SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);SendMessageW(hwnd,WM_KEYDOWN,'Q',0);
            app->check(!app->stage_scene&&app->loaded_track==0);
            // Silent device smoke: verify both Stage 4 tracks through each host audio mode.
            // The scene test independently verifies the line at which marisa_music is set.
            for(int mode:{1,2,0}){
                app->menu.music=mode;launch_stage(*app,3);
                app->check(app->audio.active()&&app->loaded_track==4&&app->playing_track==(mode==0?-1:4)&&app->midi_mode==(mode==2));
                const auto stage_hash=app->audio.song_hash();
                app->stage_scene->marisa_music=true;sync_gameplay_bgm(*app,true);
                app->check(app->audio.active()&&app->loaded_track==9&&app->playing_track==(mode==0?-1:9)&&app->midi_mode==(mode==2));
                if(mode)app->check(app->audio.song_hash()!=stage_hash);
                SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);SendMessageW(hwnd,WM_KEYDOWN,'Q',0);
                app->check(!app->stage_scene&&app->loaded_track==0&&app->playing_track==(mode==0?-1:0));
            }
            // Host rendering/audio check, independent of the input-driven Stage 5 scene test.
            for(unsigned used:{0u,1u}){
                app->menu.music=used?2:1;
                if(!used){
                    // Exercise the same Stage 4 settlement handoff as the mainline candidate.
                    launch_stage(*app,3,true);
                    auto &fourth=*app->stage_scene;
                    for(unsigned i=0;i<45000&&fourth.transition.phase!=soew::StageTransition::Phase::Ready;++i){
                        fourth.life.invincibility=200;
                        const auto phase=fourth.dialogue.phase;
                        const bool key=fourth.dialogue.active()?(phase==soew::StageDialogue::Phase::Text||
                            (phase==soew::StageDialogue::Phase::KeyDelay&&fourth.dialogue.key_delay.waiting_press)):
                            fourth.transition.gate.waiting_press;
                        fourth.tick(fourth.dialogue.active()&&key?soew::Shot:0,key);
                        if(fourth.runtime.blocked||fourth.weapon_blocked)break;
                    }
                    if(fourth.transition.phase!=soew::StageTransition::Phase::Ready||!fourth.marisa.cleared)
                        throw std::runtime_error("Stage 4 smoke did not reach settlement");
                    app->stage5_mainline=true;apply_next_stage(*app);app->stage5_mainline=false;
                }else{
                    app->stage5_trial=true;launch_stage(*app,4);app->stage5_trial=false;
                }
                if(!app->stage_scene||app->stage_scene->stage!=4||!app->stage_scene->stage5_preview)
                    throw std::runtime_error("Stage 5 smoke handoff failed");
                app->check(app->playing_track==5);
                auto &route=*app->stage_scene;
                if(used){
                    // Exercise the normal Continue gate with an isolated Game Over fixture.
                    route.life.game_over=true;route.tick(0);route.tick(0);route.tick(soew::OK,true);
                    app->check(route.continues_used==1&&!route.life.game_over);
                }
                bool saw_mima_track=false,saw_intro_mask=false,saw_winged_mask=false;
                for(unsigned i=0;i<130000&&!route.stage5_result;++i){
                    route.life.invincibility=200;
                    const auto phase=route.dialogue.phase;
                    const bool key=phase==soew::StageDialogue::Phase::Text||
                        (phase==soew::StageDialogue::Phase::KeyDelay&&route.dialogue.key_delay.waiting_press);
                    const int dx=route.mima.left+56-route.player.x;
                    const std::uint16_t move=dx>4?soew::Right:dx<-4?soew::Left:0;
                    const auto input=route.dialogue.active()?std::uint16_t(key?soew::Shot:0):
                        std::uint16_t(route.mima_enabled?(soew::Shot|move|(route.player.y>256?soew::Up:0)):0);
                    route.tick(input,key);
                    if(route.dialogue.phase==soew::StageDialogue::Phase::IntroAppearance){
                        const auto flash=route.dialogue.current==soew::StageDialogue::stage5_intro_end&&route.appearance_frame==90;
                        const auto winged=route.dialogue.current==soew::StageDialogue::stage5_winged&&route.appearance_frame==18;
                        if((flash&&!saw_intro_mask)||(winged&&!saw_winged_mask)){
                            app->check(smoke_repaint());
                            const auto &pixels=app->composed.pixels;
                            const auto black=[](std::uint32_t p){return !(p&0x00ffffffu);};
                            const auto at=[&](int x,int y){return pixels[y*640+x];};
                            app->check(black(at(31,200))&&black(at(416,200))&&black(at(447,200))&&black(at(576,200))&&black(at(639,200)));
                            app->check(black(at(100,15))&&black(at(100,384)));
                            unsigned visible_pixels=0;
                            for(int y=16;y<320;++y)for(int x=32;x<416;++x)visible_pixels+=!black(at(x,y));
                            app->check(visible_pixels>1000); // Reduced may leave the far corner black.
                            saw_intro_mask|=flash;saw_winged_mask|=winged;
                        }
                    }
                    if(route.runtime.blocked||route.weapon_blocked||route.mima.blocked_reason())break;
                    if(route.mima_music&&!saw_mima_track){sync_gameplay_bgm(*app,true);saw_mima_track=app->playing_track==10;}
                    if(i%512==0){InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);}
                }
                app->check(route.runtime.scroll.done&&saw_mima_track&&route.stage5_result.has_value());
                app->check(saw_intro_mask&&saw_winged_mask);
                if(!route.stage5_result)throw std::runtime_error("Stage 5 smoke did not reach ending");
                start_stage5_ending(*app);
                app->check(!app->stage_scene&&app->ending_player->script().branch==(used?soew::EndingBranch::Bad:soew::EndingBranch::Good));
                bool saw_story=false,saw_credits=false,saw_staff_hint=false,staff_boundary=false;
                for(unsigned i=0;i<30000&&!app->ending_player->finished();++i){
                    const bool gate=(i%16)>=8;
                    const bool boundary=app->ending_player->advance_display_frame({true,gate,std::nullopt},true,
                        [&](const auto &step){app->ending_surface->apply(step,app->ending_player->script().verdict);});
                    if(app->playing_track==12){
                        if((app->ending_player->staff_playing()&&!saw_staff_hint)||boundary)
                            app->check(app->midi_mode==(used!=0)&&app->audio.active()&&app->audio.requested_measure()==0&&app->audio.positioned_ticks()==0);
                        if(app->ending_player->staff_playing()&&!saw_staff_hint){
                            std::vector<std::uint32_t> pixels(640*400);
                            app->ending_surface->render(pixels);
                            app->check(smoke_repaint()&&app->composed.pixels!=pixels);
                            saw_staff_hint=true;
                        }
                        if(boundary){
                            std::vector<std::uint32_t> pixels(640*400);
                            app->ending_surface->render(pixels);
                            app->check(smoke_repaint()&&app->composed.pixels==pixels&&!staff_boundary);
                            staff_boundary=true;
                        }
                    }
                    saw_story|=app->playing_track==11;saw_credits|=app->playing_track==12;
                    if(i%512==0){InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);}
                }
                app->check(app->ending_player->finished()&&saw_story&&saw_credits&&saw_staff_hint&&staff_boundary);
                return_to_title(*app);app->check(!app->ending_player&&!app->ending_surface&&app->playing_track==0);
            }
            // A silent Story replay accelerates the ending without consuming replay ticks.
            {
                app->menu.music=0;app->stage5_trial=true;launch_stage(*app,4);app->stage5_trial=false;
                app->replay_playback.emplace();app->replay_cursor=123;
                start_stage5_ending(*app);
                bool staff_boundary=false;
                for(unsigned i=0;i<30000&&!app->ending_player->finished();++i){
                    staff_boundary|=app->ending_player->advance_replay_frame(true,std::nullopt,
                        [&](const auto &step){app->ending_surface->apply(step,app->ending_player->script().verdict);});
                }
                app->check(app->ending_player->finished()&&staff_boundary&&app->loaded_track==12&&app->playing_track==-1&&
                    !app->midi_mode&&app->audio.requested_measure()==0&&app->audio.positioned_ticks()==0&&
                    app->replay_cursor==123&&app->score_review==App::ScoreReview::None&&!app->pending_score);
                return_to_title(*app);
            }
            // The real title gate remains closed until all three legacy/native flags are present.
            {
                const auto saved_scores=app->scores;
                const auto saved_menu=app->menu;
                app->extra_candidate=true;app->scores=soew::Scores{};refresh_extra_unlock(*app);
                app->menu.selected=1;app->menu.update(0);SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);
                app->check(!app->stage_scene&&!app->menu.shot_screen);
                app->scores->at(0).cleared=318;app->scores->at(1).cleared=118;app->scores->at(2).cleared=218;
                refresh_extra_unlock(*app);
                app->menu.selected=1;app->menu.update(0);SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);
                app->check(app->menu.shot_screen&&app->menu.extra_mode);
                app->menu.update(0);SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);
                app->check(app->stage_scene&&app->stage_scene->stage==5&&app->stage_scene->extra_road_enabled);
                return_to_title(*app);app->extra_candidate=false;app->menu=saved_menu;app->scores=saved_scores;
            }
            for(int mode:{1,2,0,-1}){
                const bool win=mode>=0;
                app->menu.music=win?mode:2;app->extra_trial=true;app->extra_shot=1;launch_stage(*app,5);
                auto &route=*app->stage_scene;
                app->check(route.rank==4&&route.life.lives==2&&route.life.bombs==1&&app->loaded_track==13&&app->playing_track==(mode==0?-1:13));
                bool cue=false;
                if(!win){route.life.lives=0;route.life.register_hit();}
                for(unsigned i=0;i<60000&&!route.extra_settlement.terminal();++i){
                    if(win)route.life.invincibility=200;
                    const auto phase=route.dialogue.phase;
                    const bool key=route.extra_settlement.showing_bonus()||!win?route.extra_settlement.gate.waiting_press:
                        phase==soew::StageDialogue::Phase::Text||(phase==soew::StageDialogue::Phase::KeyDelay&&route.dialogue.key_delay.waiting_press);
                    std::uint16_t input=route.dialogue.active()&&key?soew::Shot:0;
                    if(win&&route.sigma_enabled&&!route.dialogue.active()&&!route.extra_settlement.showing_bonus()){
                        const int dx=route.sigma.left[route.runtime.page^1]+44-route.player.x;
                        input=soew::Shot|(dx>4?soew::Right:dx<-4?soew::Left:0)|(route.player.y>230?soew::Up:0);
                    }
                    route.tick(input,key);
                    if(route.runtime.blocked||route.weapon_blocked||route.sigma.blocked)break;
                    if(route.sigma_music&&!cue){sync_gameplay_bgm(*app,true);cue=app->loaded_track==14&&app->playing_track==(mode==0?-1:14);}
                    if(i%512==0){InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);}
                }
                if(!route.extra_settlement.terminal())throw std::runtime_error("EX smoke did not reach its result");
                app->check(route.extra_settlement.phase==(win?soew::ExtraSettlement::Phase::Clear:soew::ExtraSettlement::Phase::Lost));
                app->check(!route.continue_prompt()&&!route.continues_used&&(!win||(cue&&route.extra_midboss.finished&&route.sigma.done)));
                start_stage5_ending(*app);
                bool extra_card=false;
                for(unsigned i=0;i<3000&&!app->ending_player->finished();++i){
                    app->ending_player->tick({true,i%16>=8,std::nullopt},[&](const auto &step){
                        extra_card|=step.event.asset=="EXTRA.PI";
                        app->ending_surface->apply(step,app->ending_player->script().verdict);
                    });
                    if(i%32==0){InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);}
                }
                app->check(app->ending_player->finished()&&extra_card==win&&app->score_review==App::ScoreReview::Terminal&&app->review_rank==4&&!app->pending_score);
                app->check(!app->native_scores&&!app->settings_enabled&&!app->practice_store);
                InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
                app->score_review_armed=true;SendMessageW(hwnd,WM_KEYDOWN,win?VK_RETURN:VK_ESCAPE,0);
                app->check(!app->ending_player&&!app->pending_score&&app->score_review==App::ScoreReview::None&&app->loaded_track==0&&app->playing_track==(mode==0?-1:0));
                app->extra_trial=false;
            }
            app->menu.music=1;
            app->menu.shot_screen=false;app->menu.music_room=true;app->menu.options=false;
            for(unsigned i=0;i<40;++i){app->menu.update(0);SendMessageW(hwnd,WM_KEYDOWN,i%2?VK_UP:VK_DOWN,0);UpdateWindow(hwnd);app->check(app->composed.pixels.size()==640*400);SendMessageW(hwnd,WM_KEYUP,i%2?VK_UP:VK_DOWN,0);}
            app->menu.music_room=false;app->menu.options=false;app->menu.selected=Menu::practice_entry;app->menu.allowed=true;
            SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);app->check(app->menu.practice_shot_select()&&!app->practice_visible);
            SendMessageW(hwnd,WM_KEYUP,VK_RETURN,0);SendMessageW(hwnd,WM_KEYDOWN,VK_RETURN,0);
            app->check(app->practice_visible&&!app->menu.shot_screen&&!app->stage_scene);
            InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
            if(!app->evidence_dir.empty())dump(practice_frame(*app),app->evidence_dir/"practice-menu.bmp");
            app->practice_menu.spec=soew::PracticeSpec{};app->practice_menu.spec.dynamic_rank=-6;begin_practice(*app,app->practice_menu.spec);for(unsigned step=0;step<250&&app->practice_builder;++step)advance_practice(*app);app->check(app->stage_scene&&app->practice_session&&app->stage_scene->meira_enabled&&app->playing_track==7);InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);
            app->check(app->stage_scene->life.playperf==app->practice_session->recipe.dynamic_rank&&app->stage_scene->bullet_core.playperf==app->practice_session->recipe.dynamic_rank);
            app->check(app->stage_scene->ticks==0);
            const auto dump_practice_info=[&](bool details,const char* filename){
                if(app->evidence_dir.empty())return;
                Frame proof{640,400,std::vector<std::uint32_t>(640*400)};
                app->stage_scene->render(proof.pixels);
                draw_debug_info(proof,*app->stage_scene,app->practice_session,details,app->menu.language);
                dump(proof,app->evidence_dir/filename);
            };
            app->menu.show_more_info=true;InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);app->check(app->composed.w==640);dump_practice_info(true,"hud-more-info.bmp");
            app->menu.show_more_info=false;InvalidateRect(hwnd,nullptr,FALSE);UpdateWindow(hwnd);app->check(app->composed.w==640);dump_practice_info(false,"hud-practice-boss-only.bmp");
            const auto practice_tick=app->stage_scene->ticks;SendMessageW(hwnd,WM_KEYDOWN,'R',0);app->check(app->practice_builder.has_value());for(unsigned step=0;step<250&&app->practice_builder;++step)advance_practice(*app);app->check(app->stage_scene->ticks==practice_tick);
            SendMessageW(hwnd,WM_KEYUP,'R',0);SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->check(app->practice_escape&&app->stage_scene->paused);SendMessageW(hwnd,WM_KEYUP,VK_ESCAPE,0);SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->check(!app->practice_escape&&!app->stage_scene->paused);
            app->stage_scene->meira.done=true;app->practice_session->tick(*app->stage_scene,0);app->check(app->practice_session->result()==soew::PracticeSession::Result::Success&&!app->stage_scene->dialogue.postboss);leave_practice(*app,false);app->check(app->practice_visible&&!app->stage_scene);
            auto random_recipe=app->practice_menu.spec;random_recipe.seed_mode=soew::PracticeSeedMode::Random;
            begin_practice(*app,random_recipe);for(unsigned step=0;step<250&&app->practice_builder;++step)advance_practice(*app);
            if(!app->practice_session)throw std::runtime_error("Random Practice failed to launch");
            const auto random_seed=app->practice_session->seed();
            SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);SendMessageW(hwnd,WM_KEYUP,VK_ESCAPE,0);
            SendMessageW(hwnd,WM_KEYDOWN,'R',0);SendMessageW(hwnd,WM_KEYUP,'R',0);
            for(unsigned step=0;step<250&&app->practice_builder;++step)advance_practice(*app);
            app->check(app->practice_session&&app->practice_session->recipe==random_recipe&&((app->practice_session->seed()^random_seed)&0x00ffffffu)!=0);
            leave_practice(*app,false);
            begin_practice(*app,app->practice_menu.spec);SendMessageW(hwnd,WM_KEYDOWN,VK_ESCAPE,0);app->check(!app->practice_builder&&!app->stage_scene&&!app->practice_session&&app->practice_visible);
            app->practice_menu.allowed=false;SendMessageW(hwnd,WM_KEYDOWN,'A',0);app->check(!app->practice_menu.allowed);
            app->practice_menu.allowed=true;SendMessageW(hwnd,WM_KILLFOCUS,0,0);app->check(!app->practice_menu.allowed);SendMessageW(hwnd,WM_SETFOCUS,0,0);app->check(!app->practice_menu.allowed);
            auto bad=app->practice_menu.spec;bad.stage=8;begin_practice(*app,bad);advance_practice(*app);app->check(app->practice_visible&&!app->stage_scene&&!app->practice_session&&!app->practice_menu.warning.empty());app->practice_visible=false;
            if(!app->evidence_dir.empty()){
                const auto replay_dir=app->evidence_dir/"rep";
                app->replay_dir=replay_dir;launch_stage(*app,0);
                for(unsigned i=0;i<36;++i){const std::uint16_t input=i<18?soew::Left:soew::Right;
                    const auto step=app->replay_session->tick(*app->stage_scene,soew::normalize_replay_tick(input));
                    if(!step.consumed_tick||!app->recorder->record(step.event))throw std::runtime_error("Replay fixture input was not recorded");
                }
                const auto expected=soew::replay_state_digest(*app->stage_scene);
                const auto tap=[&](WPARAM key){SendMessageW(hwnd,WM_KEYDOWN,key,0);SendMessageW(hwnd,WM_KEYUP,key,0);};
                tap(VK_ESCAPE);tap(VK_DOWN);tap(VK_DOWN);
                app->check(app->run_pause_menu&&app->pause_choice==2);
                tap(VK_RETURN);
                if(app->replay_menu.screen()!=soew::ReplayMenuScreen::Name)throw std::runtime_error("Replay name screen did not open");
                compose_replay_menu(*app);dump(app->composed,app->evidence_dir/"replay-name.bmp");
                tap(VK_LEFT);tap(VK_UP);tap(VK_RETURN); // END on the character grid
                if(app->replay_menu.screen()!=soew::ReplayMenuScreen::SaveSlots)throw std::runtime_error("Replay slot screen did not open");
                tap(VK_RETURN);app->check(!app->stage_scene&&!app->recorder);
                const auto file=soew::ReplaySlotStore(replay_dir).path(0);
                if(!fs::is_regular_file(file))throw std::runtime_error("Replay was not saved");
                const auto title_song=app->audio.song_hash(),title_pcm=app->audio.pcm_hash();
                const auto title_samples=app->audio.submitted();
                const auto check_replay_audio=[&]{
                    const auto before=app->audio.submitted();
                    const auto begin=host_ms();
                    app->timed_test=true; // Exercise the real timer without depending on window activation.
                    while(app->audio.active()&&app->audio.submitted()==before&&host_ms()-begin<1000){
                        if(WaitForSingleObject(app->audio.event(),20)==WAIT_FAILED)break;
                        SendMessageW(hwnd,WM_TIMER,2,0);
                    }
                    app->timed_test=false;
                    app->check(app->audio.active()&&!app->audio.paused()&&app->playing_track==0&&
                        app->audio.song_hash()==title_song&&app->audio.submitted()>before);
                };
                open_replay_browser(*app);app->replay_menu.update(0,app->replay_catalog);
                app->check(app->audio.submitted()==title_samples&&app->audio.pcm_hash()==title_pcm);
                check_replay_audio();
                compose_replay_menu(*app);dump(app->composed,app->evidence_dir/"replay-browser.bmp");
                const auto browse_samples=app->audio.submitted();
                tap(VK_RETURN);app->check(app->replay_menu.screen()==soew::ReplayMenuScreen::Details);
                app->check(app->audio.submitted()==browse_samples);
                check_replay_audio();
                compose_replay_menu(*app);dump(app->composed,app->evidence_dir/"replay-details.bmp");
                tap(VK_RETURN);app->check(app->stage_scene&&app->replay_playback&&!app->recorder);
                if(!app->stage_scene||!app->replay_playback)throw std::runtime_error("Replay did not start");
                soew::ReplayStep step;
                for(unsigned i=0;i<40&&step.status==soew::ReplayStepStatus::Running;++i)
                    step=app->replay_session->step_playback(*app->stage_scene,*app->replay_playback,app->replay_cursor);
                app->check(step.status==soew::ReplayStepStatus::Finished&&app->stage_scene->ticks==36&&soew::replay_state_digest(*app->stage_scene)==expected);
                exit_replay_playback(*app,app->tr(L"リプレイが終了しました",L"Replay finished"));app->check(app->replay_menu.active());
                check_replay_audio();
                const auto return_samples=app->audio.submitted();
                app->replay_menu.update(0,app->replay_catalog);
                tap(VK_ESCAPE);app->check(!app->replay_menu.active()&&app->audio.submitted()==return_samples);
            }
            DestroyWindow(hwnd);
        }
        return 0;
    case WM_DESTROY: cancel_gameplay_audio(*app);if(app->practice_audio_cancel)app->practice_audio_cancel->store(true,std::memory_order_relaxed);
        if(app->title_demo)cancel_title_demo_work(*app->title_demo);
        if(!title_demo_active(*app))save_settings(*app);
        if(!app->audio.stop())std::cerr<<"Audio cleanup: "<<app->audio.error()<<'\n';PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd, message, w, l);
}
bool locked_display_active(HWND hwnd,const App &app){
    if(app.smoke||app.timed_test||app.score_timed_test||app.async_practice_test||app.retry_probe.mode>=0||
       !app.focused||app.minimized||app.resizing||GetForegroundWindow()!=hwnd)return false;
    if(app.stage_scene)return !app.stage_intro.active()&&!app.ending_player&&!app.replay_menu.active()&&
        !app.practice_retry&&!app.score_release&&app.score_review==App::ScoreReview::None&&app.stage_scene->motion_active();
    if(app.title_demo&&title_demo_foreground(hwnd,app)){
        if(const auto *scene=app.title_demo->controller.scene())return scene->motion_active();
    }
    return false;
}
void run_window_loop(HWND hwnd,App &app){
    struct DisplayTimer {
        HANDLE handle=nullptr;
        ~DisplayTimer(){if(handle)CloseHandle(handle);}
        void wait(std::uint64_t delay_us){
            if(!handle){
                handle=CreateWaitableTimerExW(nullptr,nullptr,CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,TIMER_ALL_ACCESS);
                if(!handle)handle=CreateWaitableTimerW(nullptr,FALSE,nullptr);
            }
            LARGE_INTEGER due{};due.QuadPart=-static_cast<LONGLONG>(delay_us*10);
            if(!handle||!SetWaitableTimer(handle,&due,0,nullptr,nullptr,FALSE)||
               MsgWaitForMultipleObjectsEx(1,&handle,INFINITE,QS_ALLINPUT,MWMO_INPUTAVAILABLE)==WAIT_FAILED)
                throw std::runtime_error("Display frame timer failed");
        }
    } timer;
    soew::DisplayCadence cadence;
    std::uint64_t last_frame_us=0;
    MSG msg{};
    for(;;){
        const bool precise=locked_display_active(hwnd,app);
        if(precise!=app.display_frames_active){
            app.display_frames_active=precise;
            app.frame_rate.reset();
            if(precise){
                KillTimer(hwnd,2);
                cadence.reset(host_us());last_frame_us=0;app.locked_clock={};
            }
            else{
                if(PeekMessageW(&msg,nullptr,WM_QUIT,WM_QUIT,PM_REMOVE))break;
                const auto now=host_ms();
                app.stage_clock.reset(now);
                if(app.title_demo)app.title_demo->clock.reset(now);
                if(!SetTimer(hwnd,2,10,nullptr))throw std::runtime_error("Opening timer failed");
            }
        }
        if(precise){
            const auto now=host_us();
            // Service a due frame before ordinary messages so a busy queue
            // cannot hold up simulation/presentation until it becomes empty.
            if(cadence.consume(now)){
                const auto elapsed_ms=last_frame_us?now/1000-last_frame_us/1000:0;
                const auto lost_ms=cadence.late_us>=cadence.period_numerator()/cadence.rate/2?cadence.late_us/1000:0;
                app.locked_timing={elapsed_ms,std::min(elapsed_ms,lost_ms)};
                last_frame_us=now;
                app.paced_paint_due=true;
                InvalidateRect(hwnd,nullptr,FALSE);
                SendMessageW(hwnd,WM_TIMER,2,0);
                if(app.paced_paint_due)UpdateWindow(hwnd);
                app.paced_paint_due=false;
                // Even when rendering runs over budget, let input/close through.
                if(!PeekMessageW(&msg,nullptr,0,0,PM_REMOVE))continue;
            }else if(!PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){
                const auto before_wait=host_us();
                if(before_wait<cadence.deadline())timer.wait(cadence.deadline()-before_wait);
                continue;
            }
            if(msg.hwnd==hwnd&&msg.message==WM_TIMER&&msg.wParam==2)continue;
        }else{
            const auto status=GetMessageW(&msg,nullptr,0,0);
            if(status<0)throw std::runtime_error("Windows message loop failed");
            if(!status)break;
        }
        if(msg.message==WM_QUIT)break;
        TranslateMessage(&msg);DispatchMessageW(&msg);
    }
}
int wmain(int argc, wchar_t **argv) {
    try {
        wchar_t executable[32768];
        const auto len = GetModuleFileNameW(nullptr, executable, 32768);
        if(len == 0 || len >= 32768) throw std::runtime_error("Executable path unavailable");
        fs::path assets = fs::path(executable).parent_path() / "content";
        fs::path evidence_dir,host_perf_log;
        const fs::path install_dir=fs::path(executable).parent_path();
        fs::path settings_dir=install_dir/"user", replay_file;
        const bool portable=fs::is_regular_file(install_dir/"portable.ini");
        if(portable)settings_dir=install_dir/"save"; // Keep earlier user/ files untouched.
        bool settings_probe=false,explicit_settings_dir=false,stage4_trial=false,stage5_trial=false,stage5_mainline=false,extra_trial=false,extra_candidate=false;
        int extra_shot=-1;
        bool stage_dump_fire=false,stage_dump_bomb=false,stage_dump_protected=false,rika_probe=false,preboss_dump=false,assist_dump=false;
        unsigned rika_probe_damage=0;
        unsigned stage_dump_ticks=0;
        int stage_dump_id=-1;
        unsigned score_frame_number=0;
        std::optional<soew::Language> language_override;
        using PracticePage=soew::PracticePanel::Page;
        std::optional<PracticePage> practice_dump;
        bool practice_shots_dump=false;
        RetryHostProbe retry_probe;
        auto &retry_mode=retry_probe.mode,&retry_chapter=retry_probe.chapter,&retry_stage=retry_probe.stage,&retry_samples=retry_probe.sample_count;
        using RetrySection=soew::PracticeSection;
        bool require_retry_checkpoint=false,force_exact_retry=false;
        std::optional<bool> title_demo_override;
        fs::path dump_path; bool async_practice_test=false,async_exit_test=false; bool dump_only = false, smoke = false, test = false, options_dump=false, shots_dump=false,skip_opening=false,timed_test=false,scores_dump=false,score_timed_test=false,music_dump=false;
        for(int i = 1; i < argc; ++i) {
            const std::wstring arg = argv[i];
            if(arg == L"--assets" && i + 1 < argc) assets = argv[++i];
            else if(arg == L"--replay" && i + 1 < argc) replay_file = argv[++i];
            else if(arg == L"--language" && i + 1 < argc){
                const std::wstring_view value=argv[++i];
                if(value==L"ja")language_override=soew::Language::Japanese;
                else if(value==L"en")language_override=soew::Language::English;
                else if(value==L"zh-Hans")language_override=soew::Language::ChineseSimplified;
                else if(value==L"zh-Hant")language_override=soew::Language::ChineseTraditional;
                else throw std::runtime_error("Expected --language ja, en, zh-Hans or zh-Hant");
            }
            else if(arg == L"--smoke-evidence" && i + 1 < argc) {evidence_dir=argv[++i];}
            else if(arg == L"--host-perf" && i + 1 < argc){
                host_perf_log=argv[++i];
                if(!host_perf_log.is_absolute()||host_perf_log.extension()!=L".txt"||
                   !fs::is_directory(host_perf_log.parent_path())||fs::exists(fs::symlink_status(host_perf_log)))
                    throw std::runtime_error("--host-perf needs an unused absolute .txt path in an existing directory");
            }
            else if(arg == L"--settings-probe")settings_probe=true;
            else if(arg == L"--settings-dir" && i + 1 < argc) {explicit_settings_dir=true;settings_dir=argv[++i];if(settings_dir.empty())throw std::runtime_error("Empty settings directory");}
            else if(arg == L"--dump" && i + 1 < argc) { dump_path = argv[++i]; dump_only = true; }
            else if(arg == L"--practice-menu" && i + 1 < argc){
                const std::wstring_view page=argv[++i];
                if(page==L"shots")practice_shots_dump=true;
                else if(page==L"stages")practice_dump=PracticePage::Stages;
                else if(page==L"setup")practice_dump=PracticePage::Setup;
                else throw std::runtime_error("Expected --practice-menu shots, stages or setup");
            }
            else if(arg == L"--stage-fire")stage_dump_fire=true;
            else if(arg == L"--stage-bomb")stage_dump_bomb=true;
            else if(arg == L"--stage-protected")stage_dump_protected=true;
            else if(arg == L"--rika-probe")rika_probe=true;
            else if(arg == L"--preboss-dump")preboss_dump=true;
            else if(arg == L"--practice-assist-dump")assist_dump=true;
            else if(arg == L"--rika-damage"&&i+1<argc){rika_probe_damage=std::stoul(argv[++i]);if(rika_probe_damage>2260)throw std::runtime_error("Invalid diagnostic Rika damage");}
            else if(arg == L"--stage-ticks"&&i+1<argc){stage_dump_ticks=std::stoul(argv[++i]);if(stage_dump_ticks>100000)throw std::runtime_error("Scene tick limit exceeded");}
            else if(arg == L"--stage-dump"&&i+1<argc){stage_dump_id=std::stoi(argv[++i]);if(stage_dump_id<0||stage_dump_id>5)throw std::runtime_error("Invalid scene index");}
            else if(arg == L"--score-timed-test"){score_timed_test=true;scores_dump=true;score_frame_number=1990;}
            else if(arg == L"--timed-test") timed_test=true;
            else if(arg == L"--skip-opening") skip_opening=true;
            else if(arg == L"--title-demo") title_demo_override=true;
            else if(arg == L"--no-title-demo") title_demo_override=false;
            else if(arg == L"--shots") shots_dump=true;
            else if(arg == L"--score-frame" && i+1<argc){score_frame_number=std::stoul(argv[++i]);if(score_frame_number>2001)throw std::runtime_error("Score frame out of range");}
            else if(arg == L"--music-room") music_dump=true;
            else if(arg == L"--scores") scores_dump=true;
            else if(arg == L"--options") options_dump=true;
            else if(arg == L"--retry-benchmark"&&i+2<argc){async_practice_test=true;retry_mode=std::stoi(argv[++i]);retry_chapter=std::stoi(argv[++i]);if(retry_mode<0||retry_mode>2||retry_chapter<0)throw std::runtime_error("Expected music 0..2 and a nonnegative chapter ID");}
            else if(arg == L"--retry-stage"&&i+1<argc){const auto stage=std::stoi(argv[++i]);if(stage<1||stage>6)throw std::runtime_error("Expected retry stage 1..6 (6 is Extra)");retry_stage=stage-1;}
            else if(arg == L"--retry-section"&&i+1<argc){
                const std::wstring_view section=argv[++i];
                if(section==L"start")retry_probe.section=RetrySection::StageStart;
                else if(section==L"chapter")retry_probe.section=RetrySection::Chapter;
                else if(section==L"midboss")retry_probe.section=RetrySection::Midboss;
                else if(section==L"midboss2")retry_probe.section=RetrySection::Midboss2;
                else if(section==L"boss")retry_probe.section=RetrySection::Boss;
                else throw std::runtime_error("Unknown retry section");
            }
            else if(arg == L"--retry-phase"&&i+1<argc)retry_probe.phase=std::stoi(argv[++i]);
            else if(arg == L"--retry-samples"&&i+1<argc){retry_samples=std::stoi(argv[++i]);if(retry_samples<1||retry_samples>20)throw std::runtime_error("Expected retry sample count 1..20");}
            else if(arg == L"--require-retry-checkpoint") require_retry_checkpoint=true;
            else if(arg == L"--force-exact-retry") force_exact_retry=true;
            else if(arg == L"--retry-intro-off") retry_probe.disable_intro=true;
            else if(arg == L"--async-practice-test") async_practice_test=true;
            else if(arg == L"--async-exit-test") {async_practice_test=true;async_exit_test=true;}
            else if(arg == L"--stage4-trial") stage4_trial=true;
            else if(arg == L"--stage5-trial") stage5_trial=true;
            else if(arg == L"--stage5-mainline") stage5_mainline=true;
            else if(arg == L"--extra-trial") extra_trial=true;
            else if(arg == L"--extra-candidate") extra_candidate=true;
            else if(arg == L"--extra-shot"&&i+1<argc){
                const std::wstring shot=argv[++i];
                if(shot.size()!=1||shot[0]<L'A'||shot[0]>L'C')throw std::runtime_error("Extra shot must be A, B or C");
                extra_shot=shot[0]-L'A';
            }
            else if(arg == L"--smoke-test") smoke = true;
            else if(arg == L"--self-test") test = true;
            else throw std::runtime_error("Unknown/incomplete argument");
        }
        if(retry_mode>=0){
            const auto error=soew::validate_practice(retry_probe.recipe(),soew::practice_capabilities());
            if(!error.empty())throw std::runtime_error("Invalid retry benchmark recipe: "+error);
        }
        const bool pinned_retry_target=retry_mode>=0&&soew::practice_music_measure(retry_probe.recipe())!=0;
        if(require_retry_checkpoint&&(retry_mode<1||!pinned_retry_target||force_exact_retry))
            throw std::runtime_error("--require-retry-checkpoint requires a positioned FM/MIDI benchmark without --force-exact-retry");
        if(force_exact_retry&&retry_mode<0)
            throw std::runtime_error("--force-exact-retry requires --retry-benchmark");
        if(retry_probe.disable_intro&&(retry_mode<0||retry_probe.section!=RetrySection::StageStart))
            throw std::runtime_error("--retry-intro-off requires a stage-start retry benchmark");
        if((practice_dump||practice_shots_dump)&&!dump_only)throw std::runtime_error("--practice-menu requires --dump");
        if(assist_dump&&(!dump_only||stage_dump_id<0))throw std::runtime_error("--practice-assist-dump requires --stage-dump and --dump");
        const bool diagnostic=smoke||test||dump_only||timed_test||score_timed_test||async_practice_test||settings_probe||stage_dump_id>=0||rika_probe||preboss_dump||options_dump||shots_dump||scores_dump||music_dump;
        if(!replay_file.empty()&&(diagnostic||stage4_trial||stage5_trial||stage5_mainline||extra_trial||extra_candidate))throw std::runtime_error("Replay is a standalone launch mode");
        const bool interactive_title=!diagnostic&&replay_file.empty()&&!stage4_trial&&!stage5_trial&&!extra_trial;
        if(title_demo_override.value_or(false)&&!interactive_title)
            throw std::runtime_error("--title-demo requires an interactive title launch");
        if(portable&&!diagnostic&&replay_file.empty()&&!stage4_trial&&!stage5_trial&&!stage5_mainline&&!extra_trial&&!extra_candidate)extra_candidate=true;
        const bool title_demo=interactive_title&&title_demo_override.value_or((portable||extra_candidate)&&
            std::all_of(soew::title_demos.begin(),soew::title_demos.end(),[&](const auto &spec){
                std::error_code ignored;
                return fs::is_regular_file(assets/spec.filename,ignored);
            }));
        if(int(stage4_trial)+int(stage5_trial)+int(stage5_mainline)+int(extra_trial)+int(extra_candidate)>1)throw std::runtime_error("Choose only one trial or mainline mode");
        if((stage4_trial||stage5_trial||stage5_mainline||extra_trial||extra_candidate)&&(smoke||test||dump_only||timed_test||score_timed_test||async_practice_test||settings_probe||stage_dump_id>=0||rika_probe||preboss_dump||options_dump||shots_dump||scores_dump||music_dump))
            throw std::runtime_error("Stage trial and mainline modes are standalone launches");
        if((stage5_mainline||extra_candidate)&&!explicit_settings_dir&&!portable)throw std::runtime_error("Mainline candidate requires an explicit user directory");
        if(extra_shot!=-1&&!extra_trial)throw std::runtime_error("--extra-shot requires --extra-trial");
        if(settings_probe){if(!explicit_settings_dir)throw std::runtime_error("Settings probe requires explicit isolated directory");soew::SettingsStore probe(settings_dir/"soew-settings.json");const auto x=probe.load();std::cout<<"settings_probe rank="<<x.rank<<" music="<<x.music<<" lives="<<x.lives<<" bombs="<<x.bombs<<" reduced="<<x.reduced<<" autofire="<<x.autofire<<" writable="<<probe.writable<<'\n';return probe.warning.empty()?0:1;}
        if(test) {
            unsigned checks = 0;
            for(int w : {0, 1, 3, 320, 640, 800, 1024, 1920, 3840}) for(int h : {0, 1, 7, 200, 400, 600, 1080, 2160}) for(bool crt : {false, true}) {
                auto v = fit(w, h, crt); ++checks;
                if(v.x < 0 || v.y < 0 || v.w < 0 || v.h < 0 || v.x + v.w > w || v.y + v.h > h) throw std::runtime_error("Viewport test failed");
            }
            auto v = fit(800, 600, false); if(v.w != 800 || v.h != 500 || v.y != 50) throw std::runtime_error("8:5 test failed");
            v = fit(800, 600, true); if(v.w != 800 || v.h != 600) throw std::runtime_error("4:3 test failed");
            unsigned jp_checks=0;
            for(const auto& descriptions:{jp::descriptions,jp::descriptions_en}) for(const auto& desc:descriptions)for(auto line:desc){++jp_checks;if(jp::cell_width(line)>176)throw std::runtime_error("Description exceeds source box");}
            for(const auto header:{jp::choose,jp::choose_en}){++jp_checks;if(jp::cell_width(header)>368)throw std::runtime_error("Header exceeds source box");}
            for(const auto& notes:{jp::extra_note,jp::extra_note_en})for(auto line:notes){++jp_checks;if(jp::cell_width(line)>592)throw std::runtime_error("Extra note exceeds box");}
            ++jp_checks;if(jp::cell_width(jp::cleared)>208)throw std::runtime_error("Clear marker exceeds source column spacing");
            Frame sample{640,400,std::vector<std::uint32_t>(640*400,0)};
            jp_text(sample,L"演出",10,10,0xffffff);++jp_checks;
            if(std::count_if(sample.pixels.begin(),sample.pixels.end(),[](auto p){return p!=0;})<10)throw std::runtime_error("Japanese glyph rendering failed");
            test_live_assist_host();
            std::cout << "viewport_checks=" << checks + 2 << " menu_checks=" << test_menu() << " japanese_checks=" << jp_checks << " chinese_ascii_checks=" << test_chinese_ascii() << " opening_checks=" << test_opening() << " failures=0\n"; return 0;
        }
        if(async_practice_test&&(smoke||test||dump_only||timed_test||score_timed_test))throw std::runtime_error("Async practice test cannot be combined with other test modes");
        App app{};app.assets_root=assets;app.evidence_dir=evidence_dir;app.async_practice_test=async_practice_test;app.async_exit_test=async_exit_test;
        if(title_demo)app.title_demo.emplace();
        if(!host_perf_log.empty())app.host_perf=std::make_unique<HostPerf>();
        if(!evidence_dir.empty()&&(!smoke||!fs::is_directory(evidence_dir)))throw std::runtime_error("Smoke evidence requires existing isolated directory");
        app.retry_probe=retry_probe;
        app.retry_probe.require_checkpoint=require_retry_checkpoint;app.retry_probe.force_exact=force_exact_retry;
        app.practice_menu.capabilities=soew::practice_capabilities();
        app.replay_dir=explicit_settings_dir?settings_dir.parent_path()/"rep":install_dir/"rep";
        app.settings_enabled=replay_file.empty()&&!(extra_trial||stage4_trial||stage5_trial||async_practice_test||smoke||test||dump_only||timed_test||score_timed_test||stage_dump_id>=0||rika_probe||preboss_dump);
        soew::validate_locale_assets(assets);
        try { app.chinese_text=soew::ChineseDisplayPack::load(assets,[&](std::string_view text){verify_chinese_manifest(assets,text);}); }
        catch(const std::exception& error){std::cerr<<"Chinese display pack unavailable: "<<error.what()<<'\n';}
        app.menu.set_chinese_available(app.chinese_text.has_value());
        if(app.settings_enabled){
            app.settings.emplace(settings_dir/"soew-settings.json");
            app.settings->load().apply(app.menu);
            std::ifstream replay_preferences(settings_dir/"replay-options.txt",std::ios::binary);
            char option[5]{};replay_preferences.read(option,sizeof(option));
            if(replay_preferences.gcount()==4&&std::string_view(option,4)=="off\n")app.replay_recording=false;
            app.practice_store.emplace(settings_dir.parent_path());app.practice_store->path=settings_dir/"soew-practice.json";app.practice_menu.spec=app.practice_store->load(app.menu.bombs);
            app.settings_warning=app.settings->warning;
            if(!app.settings_warning.empty())std::cerr<<"Settings warning: "<<app.settings_warning<<'\n';
        }
        if(!replay_file.empty()){
            soew::SettingsStore preferences(settings_dir/"soew-settings.json");
            const auto settings=preferences.load();
            app.menu.set_language_preference(settings.language);
            app.menu.stage_intro=settings.stage_intro;
            app.menu.smooth_background=settings.smooth_background;
        }
        if(language_override){
            if(app.settings_enabled)throw std::runtime_error("--language requires a test, dump, or read-only replay mode");
            if(soew::is_chinese(*language_override)&&!app.chinese_text)throw std::runtime_error("Requested Chinese display pack is unavailable");
            app.menu.set_language_preference(*language_override);
        }
        app.legacy_content=!soew::has_locale_assets(assets);
        if(app.legacy_content)std::cerr<<"Locale overlay missing; using legacy content files\n";
        const auto title_root=soew::locale_asset(assets,app.menu.language,"OP2.PI").parent_path();
        app.frames={load(title_root, "OP2.PI", true), load(assets, "OP.PI", false), load(assets, "TSELECT.PI", false)};
        const auto select=th03::assets::decode_pi(read(assets / "TSELECT.PI"));
        if(!select || !select->palette)throw std::runtime_error("Selection palette unavailable");
        const auto rgb=[](const th03::assets::Rgb4 &c){return (std::uint32_t(c.r)*17<<16)|(std::uint32_t(c.g)*17<<8)|(std::uint32_t(c.b)*17);};
        app.selected_color=rgb(select->palette->colors[12]);
        app.unselected_color=rgb(select->palette->colors[7]);
        app.portraits={load(assets,"TS1.PI",false,&*select->palette),load(assets,"TS2.PI",false,&*select->palette),load(assets,"TS3.PI",false,&*select->palette)};
        try{app.stage_intro_art=load(assets,"EYE.PI",false);}
        catch(const std::exception &error){std::cerr<<"Stage intro unavailable: "<<error.what()<<'\n';}
        const auto font_bytes=read(assets / "MIKOFT.BFT");
        // The stock Japanese sheet is 8224 bytes; the supported English
        // variant appends 0x42. Pass only the BFNT sheet to the strict decoder.
        if(font_bytes.size()!=8224 && (font_bytes.size()!=8225 || font_bytes.back()!=std::byte{0x42}))
            throw std::runtime_error("Unexpected TH02 font size or trailer");
        auto font=th03::assets::decode_bfnt(std::span<const std::byte>(font_bytes).first(8224),th03::assets::BfntConsumerPolicy::gaiji);
        if(!font) throw std::runtime_error("MIKOFT.BFT decode failed: "+font.error().message);
        if(font->header.pixel_width!=16 || font->header.pixel_height!=16 || font->glyphs.size()!=256) throw std::runtime_error("Unexpected gaiji sheet");
        app.font=std::move(*font);
        app.menu.options=options_dump;
        app.menu.shot_screen=shots_dump;
        std::cout << "gaiji_glyphs=" << app.font.glyphs.size() << '\n';
        try{app.scores=soew::decode_scores(read(assets/"HUUHI.DAT"));std::cout<<"score_records=50 checksums=valid\n";}catch(const std::exception &e){app.score_error=e.what();std::cout<<"score_unavailable="<<app.score_error<<'\n';}
        if(app.scores){const auto status=soew::clear_status(*app.scores);app.menu.unlocked=status.unlocked;app.menu.cleared_main=status.main;app.menu.cleared_extra=status.extra;std::cout<<"extra_unlocked="<<status.unlocked<<" main_clears="<<status.main[0]<<status.main[1]<<status.main[2]<<" extra_clears="<<status.extra[0]<<status.extra[1]<<status.extra[2]<<'\n';}
        if(practice_shots_dump){
            auto frame=app.frames[2];
            app.menu.begin_practice_shot_select(app.practice_menu.spec.shot);
            compose_shots(frame,app.portraits,app.menu,app.selected_color,app.unselected_color);
            dump(frame,dump_path);
            return 0;
        }
        if(practice_dump){
            app.practice_panel.open(app.practice_menu,*practice_dump);
            dump(practice_frame(app),dump_path);
            return 0;
        }
        auto score_palette=th03::assets::decode_rgb48(read(assets/"OP_H.RGB"));
        if(!score_palette)throw std::runtime_error("Score palette invalid");
        for(unsigned i=0;i<16;++i)app.score_palette[i]=rgb(score_palette->colors[i]);
        const auto logo=th03::assets::decode_bfnt(read(assets/"OP_H.BFT"),th03::assets::BfntConsumerPolicy::superimpose);
        if(!logo||logo->header.pixel_width!=64||logo->header.pixel_height!=32||logo->glyphs.size()!=2)throw std::runtime_error("Score logo BFNT invalid");
        app.score_transparent=logo->transparent_color;
        for(unsigned cel=0;cel<2;++cel){
            const auto *bitmap=std::get_if<th03::assets::PlanarBitmap4>(&logo->glyphs[cel].pixels);
            if(!bitmap)throw std::runtime_error("Score logo planar layout invalid");
            for(unsigned pixel=0;pixel<64*32;++pixel){std::uint8_t color=0;for(unsigned plane=0;plane<4;++plane)if(bitmap->color_planes[plane].at(pixel/8)&(0x80>>(pixel%8)))color|=1<<plane;app.score_logo[cel].indices[pixel]=color;}
        }
        std::cout<<"score_logo=2x64x32 transparent="<<unsigned(app.score_transparent)<<'\n';
        app.music_background=load(assets,"OP3.PI",false);
        const auto music_pi=th03::assets::decode_pi(read(assets/"OP3.PI"));
        if(!music_pi||!music_pi->palette)throw std::runtime_error("Music room palette missing");
        for(unsigned i=0;i<16;++i)app.music_palette[i]=rgb(music_pi->palette->colors[i]);
        for(const auto lang:{soew::Language::Japanese,soew::Language::English}){
            const auto comments=soew::decode_music_comments(read(soew::locale_asset(assets,lang,"MUSIC.TXT")));
            auto& localized_comments=app.music_comments[lang==soew::Language::English];
            for(unsigned track=0;track<15;++track)for(unsigned line=0;line<20;++line){
                auto value=cp932(comments[track][line]);
                if(jp::cell_width(value)>320)throw std::runtime_error("Music comment exceeds source width");
                localized_comments[track][line]=std::move(value);
            }
        }
        if(fs::exists(assets/"GeneralUser-GS-SoftSynth-1.44.sf2"))app.soundfont=read(assets/"GeneralUser-GS-SoftSynth-1.44.sf2",40*1024*1024);
        for(unsigned i=0;i<15;++i){const auto path=assets/fs::path(std::string(soew::music_files[i])+"MD");if(fs::exists(path))app.midi_songs[i]=read(path);}
        if(fs::exists(assets/"HUUMA.EFC"))app.effect_bank=read(assets/"HUUMA.EFC");
        unsigned songs_loaded=0;
        for(unsigned i=0;i<15;++i){const auto path=assets/fs::path(soew::music_files[i]);if(fs::exists(path)){app.songs[i]=read(path);++songs_loaded;}}
        std::cout<<"music_comments=15x20 audio=optional_FM_off songs="<<songs_loaded<<'\n';
        app.menu.music_room=music_dump;
        if(dump_only&&music_dump){dump(music_frame(app.music_background,app.menu,app.current_music_comments(),app.music_palette,app.chinese_text?&*app.chinese_text:nullptr),dump_path);return 0;}
        app.scores_visible=scores_dump;
        app.score_animation.frame=score_frame_number;
        if(scores_dump&&!app.scores)throw std::runtime_error(app.score_error);
        if(dump_only&&scores_dump){dump(score_frame(*app.scores,app.menu.rank,app.font,app.score_logo,app.score_palette,app.score_transparent,app.score_animation.frame,app.menu.language),dump_path);return 0;}
        if(dump_only&&stage_dump_id>=0){
            launch_stage(app,stage_dump_id);
            if(rika_probe){app.stage_scene->bind_rika_battle({0,0,0,0,0,0});app.stage_scene->rika.damage_taken=std::uint16_t(rika_probe_damage);}
            for(unsigned i=0;i<stage_dump_ticks;++i){if(stage_dump_protected)app.stage_scene->life.invincibility=200;app.stage_scene->tick((stage_dump_fire?soew::Shot:0)|(stage_dump_bomb&&i==0?soew::Bomb:0));}
            if(preboss_dump){if(!app.stage_scene->dialogue.active())throw std::runtime_error("Map has not reached preboss dialogue");while(app.stage_scene->dialogue.phase==soew::StageDialogue::Phase::SlideIn||app.stage_scene->dialogue.phase==soew::StageDialogue::Phase::Text)app.stage_scene->tick(soew::Shot,true);}
            Frame frame{640,400,std::vector<std::uint32_t>(640*400)};app.stage_scene->render(frame.pixels);
            compose_dialogue_text(frame,*app.stage_scene,app.menu.language,app.chinese_text?&*app.chinese_text:nullptr);
            if(assist_dump){app.practice_assist.toggle(soew::PracticeTool::Invincible);app.practice_assist.toggle(soew::PracticeTool::AutoBomb);draw_practice_assist(frame,app.practice_assist,app.menu.language);}
            dump(frame,dump_path);return 0;
        }
        if(rika_probe)throw std::runtime_error("Rika probe currently requires --stage-dump and --dump; no normal game dialogue bypass");
        if(dump_only) {
            auto frame=app.frames[shots_dump?2:0];
            if(shots_dump)compose_shots(frame,app.portraits,app.menu,app.selected_color,app.unselected_color);
            else if(options_dump)compose_options(frame,app.font,app.menu,app.settings_warning);
            else compose_frontend(frame,app.font,app.menu);
            dump(frame,dump_path);std::cout<<"Menu dumped: 640x400 over OP2/OP.RGB\n";return 0;
        }
        const auto opening_pi=th03::assets::decode_pi(read(assets/"OP.PI"));
        if(!opening_pi||!opening_pi->palette)throw std::runtime_error("Opening palette unavailable");
        app.opening_strips={load(assets,"OPA.PI",false,&*opening_pi->palette),load(assets,"OPB.PI",false,&*opening_pi->palette),load(assets,"OPC.PI",false,&*opening_pi->palette)};
        if(!replay_file.empty()||extra_trial||stage4_trial||stage5_trial||async_practice_test||smoke||skip_opening||options_dump||shots_dump||scores_dump||music_dump)app.opening.skip();
        app.stage4_trial=stage4_trial;app.stage5_trial=stage5_trial;app.stage5_mainline=stage5_mainline||extra_candidate;
        app.extra_trial=extra_trial;app.extra_candidate=extra_candidate;
        if(extra_shot>=0)app.extra_shot=unsigned(extra_shot);
        if(app.stage5_mainline){
            fs::create_directories(settings_dir);
            app.native_scores.emplace(settings_dir);
            const auto status=app.native_scores->load();
            if(status!=soew::NativeScoreStore::Status::Ok&&status!=soew::NativeScoreStore::Status::Missing)
                app.settings_warning="Native scores could not be loaded; existing records retained";
        }
        refresh_extra_unlock(app);
        app.smoke = smoke;app.audio_test_silent=smoke||async_practice_test;app.gameplay_audio_enabled=true;
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.lpfnWndProc=window_proc;
        wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"SoEWNativePrototype";wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
        const auto resource=MAKEINTRESOURCEW(IDI_SOEW_APP_ICON);
        wc.hIcon=static_cast<HICON>(LoadImageW(wc.hInstance,resource,IMAGE_ICON,GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON),LR_SHARED));
        wc.hIconSm=static_cast<HICON>(LoadImageW(wc.hInstance,resource,IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),LR_SHARED));
        if(!wc.hIcon||!wc.hIconSm)throw std::runtime_error("Application icon resource unavailable");
        if(!RegisterClassExW(&wc)) throw std::runtime_error("Window class registration failed");
        const auto hwnd = CreateWindowW(wc.lpszClassName, app.tr(L"東方封魔録",L"Story of Eastern Wonderland"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700, nullptr, nullptr, wc.hInstance, &app);
        if(!hwnd) throw std::runtime_error("Window creation failed");
        if(app.menu.fullscreen&&!set_fullscreen(hwnd,app,true)){
            app.menu.fullscreen=false;app.settings_warning="Could not start in fullscreen mode";
        }
        caption(hwnd, app); ShowWindow(hwnd, app.window_fullscreen?SW_SHOW:SW_SHOWNORMAL); UpdateWindow(hwnd);
        if(smoke && !SetTimer(hwnd, 1, 200, nullptr)) throw std::runtime_error("Test timer failed");
        app.score_timed_test=score_timed_test;
        app.timed_test=timed_test;app.test_started=host_ms();app.clock.reset(app.test_started);
        if(timed_test){SetWindowPos(hwnd,HWND_TOPMOST,20,20,1000,700,SWP_SHOWWINDOW);SetForegroundWindow(hwnd);SetFocus(hwnd);}
        if(!replay_file.empty()){
            launch_replay(app,replay_file);
            caption(hwnd,app);InvalidateRect(hwnd,nullptr,FALSE);
        }else if(extra_trial){
            launch_stage(app,5);
            app.notice=app.tr(L"エキストラ試遊　方向キー：移動　Z：ショット　X：ボム　P：一時停止　Esc：メニュー",L"Extra trial | Arrows: move | Z: shot | X: Bomb | P: pause | Esc: menu");
            caption(hwnd,app);InvalidateRect(hwnd,nullptr,FALSE);
        }else if(stage5_trial){
            launch_stage(app,4);
            app.notice=app.tr(L"５面試遊　方向キー：移動　Z：ショット　X：ボム　P：一時停止　Esc：メニュー",L"Stage 5 trial | Arrows: move | Z: shot | X: Bomb | P: pause | Esc: menu");
            caption(hwnd,app);InvalidateRect(hwnd,nullptr,FALSE);
        }else if(stage4_trial){
            launch_stage(app,3,true);
            app.notice=app.tr(L"４面互換試遊　Z：ショット　X：ボム　P：一時停止　Esc：メニュー",L"Stage 4 compatibility trial | Z: shot | X: Bomb | P: pause | Esc: menu");
            caption(hwnd,app);InvalidateRect(hwnd,nullptr,FALSE);
        }else if(!app.opening.active&&!app.title_audio_started){app.title_audio_started=true;start_preview(app,0);}
        if(!smoke && !SetTimer(hwnd,2,10,nullptr))throw std::runtime_error("Opening timer failed");
        if(async_practice_test){app.async_practice_started=host_ms();if(!SetTimer(hwnd,3,10,nullptr))throw std::runtime_error("Async test timer failed");}
        run_window_loop(hwnd,app);
        if(app.host_perf){
            std::ofstream log(host_perf_log,std::ios::out|std::ios::noreplace);
            if(!log)throw std::runtime_error("Could not create host performance report");
            app.host_perf->report(log);log.close();
            if(!log)throw std::runtime_error("Could not write host performance report");
        }
        std::cout << "window_checks=" << app.checks << " paints=" << app.paints << " size_events=" << app.sizes << " key_events=" << app.keys << " failures=" << (app.failed ? 1 : 0) << '\n';
        return app.failed ? 1 : 0;
    } catch(const std::exception &e) { std::cerr << "ERROR: " << e.what() << '\n'; return 1; }
}
