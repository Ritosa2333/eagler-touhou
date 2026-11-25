#pragma once
#include "render_motion.hpp"
#include "stage_background.hpp"
#include "stage_script.hpp"
#include "player_motion.hpp"
#include "stage_progress.hpp"
#include "th03/assets/decode.hpp"
#include <filesystem>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
namespace soew {
inline constexpr unsigned scroll_subpixels=motion_subpixels;
// Interpolate towards the current scroll step without reading future tiles.
inline unsigned background_scroll_lag(const StageScroll &scroll,unsigned subframe){
    if(scroll.done||!scroll.display_pixels)return 0;
    const unsigned age=(unsigned(scroll.cycle)+scroll.interval-1)%scroll.interval;
    return scroll.speed*((scroll.interval-age)*scroll_subpixels-subframe)/scroll.interval;
}
inline int scroll_sample_rows(unsigned lag){return int((lag+scroll_subpixels-1)/scroll_subpixels);}
inline void filter_stage_background(std::span<std::uint32_t> pixels,unsigned lag,int terrain_bottom){
    const int whole=int(lag/scroll_subpixels);
    const unsigned phase=(lag%scroll_subpixels+2)/4;
    // Combine a fixed [1 2 1]/4 vertical filter with fractional scroll.
    // Without the prefilter, one-pixel stripes disappear at half a pixel
    // and return at integer positions, modulating contrast as the map moves.
    const unsigned wa=64-phase,wb=128-phase,wc=64+phase,wd=phase;
    constexpr unsigned rb=0x00ff00ff,green=0x0000ff00;
    const auto raw=[&](int y){return pixels.data()+std::min(y,terrain_bottom-1)*640+32;};
    std::array<std::uint32_t,384> previous;
    if(!whole)std::copy_n(raw(15),384,previous.begin());
    for(int y=16;y<384;++y){
        auto *out=raw(y);
        const auto *a=whole?raw(y+whole-1):previous.data(),*b=raw(y+whole),*c=raw(y+whole+1),*d=raw(y+whole+2);
        for(int x=0;x<384;++x){
            const auto ca=a[x],cb=b[x],cc=c[x],cd=d[x];
            if(!whole)previous[x]=cb;
            out[x]=((((ca&rb)*wa+(cb&rb)*wb+(cc&rb)*wc+(cd&rb)*wd)>>8)&rb)|
                   ((((ca&green)*wa+(cb&green)*wb+(cc&green)*wc+(cd&green)*wd)>>8)&green);
        }
    }
    std::fill_n(raw(15),384,0u);
    for(int y=384;y<terrain_bottom;++y)std::fill_n(raw(y),384,0u);
}
namespace asset_cache_detail {
struct Stats {std::uint64_t hits=0,decodes=0;std::size_t entries=0;};
struct Input {std::filesystem::path path;std::vector<std::byte> bytes;};
template<class T,std::size_t Capacity> class Cache {
    struct Entry {std::vector<std::uint64_t> hashes;std::shared_ptr<const T> value;};
    std::mutex mutex_;std::map<std::string,Entry> entries_;Stats stats_;
public:
    template<class Decode> std::shared_ptr<const T> get(const std::vector<Input>&inputs,Decode decode){
        std::string key;std::vector<std::uint64_t> hashes;
        for(const auto &input:inputs){const auto path=std::filesystem::canonical(input.path).generic_string();key+=std::to_string(path.size())+":"+path;
            std::uint64_t hash=14695981039346656037ull;for(auto byte:input.bytes){hash^=std::to_integer<unsigned char>(byte);hash*=1099511628211ull;}hashes.push_back(hash);hashes.push_back(input.bytes.size());}
        std::lock_guard lock(mutex_);const auto found=entries_.find(key);
        if(found!=entries_.end()&&found->second.hashes==hashes){++stats_.hits;return found->second.value;}
        // Decode under the per-payload lock so concurrent cold loads do not duplicate work.
        auto value=std::make_shared<const T>(decode());
        if(found==entries_.end()&&entries_.size()>=Capacity)entries_.erase(entries_.begin());
        entries_.insert_or_assign(std::move(key),Entry{std::move(hashes),value});++stats_.decodes;return value;
    }
    Stats stats(){std::lock_guard lock(mutex_);auto out=stats_;out.entries=entries_.size();return out;}
};
}

// P1 development scene: real terrain and original player art, NO combat yet.
class StageScene {
    struct Terrain {StageBackground background;StageScript script;};
    static auto &terrain_cache(){static asset_cache_detail::Cache<Terrain,6> cache;return cache;}
    static auto &art_cache(){static asset_cache_detail::Cache<th03::assets::BfntSheet,1> cache;return cache;}
public:
    static asset_cache_detail::Stats cache_stats(){return terrain_cache().stats();}
    static asset_cache_detail::Stats player_art_cache_stats(){return art_cache().stats();}
    StageBackground background;
    StageScript script;
    th03::assets::BfntSheet player_art;
    PlayerMotion player;
    StageScroll scroll;
    FormationDispatch encounter_observer;
    std::uint64_t observed_requests=0; // No enemy pool/AI exists in this scene.
    int stage=0,rank=1,lives=2,bombs=3;
    bool paused=false;
    std::uint64_t ticks=0;
    static std::vector<std::byte> read(const std::filesystem::path&p){std::ifstream f(p,std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Missing stage resource: "+p.filename().string());auto n=f.tellg();if(n<1||n>1000000)throw std::runtime_error("Invalid stage size");std::vector<std::byte>b(static_cast<std::size_t>(n));f.seekg(0);if(!f.read(reinterpret_cast<char*>(b.data()),n))throw std::runtime_error("Stage read failed");return b;}
    static StageScene load(const std::filesystem::path &assets,int id,int shot,int selected_rank,int spare,int stock){
        if(id<0||id>5||shot<0||shot>2||selected_rank<0||selected_rank>4)throw std::runtime_error("Invalid stage launch rules");
        StageScene scene;scene.stage=id;scene.rank=selected_rank;scene.lives=spare;scene.bombs=stock;scene.player=PlayerMotion(shot);scene.scroll=StageScroll(id);
        const auto name="STAGE"+std::to_string(id);
        const std::vector<asset_cache_detail::Input> terrain_inputs={{assets/(name+".MPN"),read(assets/(name+".MPN"))},{assets/(name+".MAP"),read(assets/(name+".MAP"))},{assets/(name+".DT1"),read(assets/(name+".DT1"))}};
        const auto terrain=terrain_cache().get(terrain_inputs,[&]{
            // Structural audit mode keeps incomplete timelines explicitly marked.
            return Terrain{StageBackground::decode(terrain_inputs[0].bytes,terrain_inputs[1].bytes,terrain_inputs[2].bytes),StageScript::decode(terrain_inputs[2].bytes,true)};
        });
        scene.background=terrain->background;scene.script=terrain->script;
        const std::vector<asset_cache_detail::Input> art_inputs={{assets/"MIKO.BFT",read(assets/"MIKO.BFT")}};
        const auto art=art_cache().get(art_inputs,[&]{
            auto decoded=th03::assets::decode_bfnt(art_inputs[0].bytes,th03::assets::BfntConsumerPolicy::superimpose);
            if(!decoded)throw std::runtime_error("Player BFT decoding failed");
            if(decoded->header.pixel_width!=32||decoded->header.pixel_height!=48||decoded->glyphs.size()<4)throw std::runtime_error("Unexpected original player art");
            return std::move(*decoded);
        });
        scene.player_art=*art;
        return scene;
    }
    void tick(std::uint16_t input){
        if(paused)return;
        if(scroll.take_progress_event())encounter_observer.dispatch(script,scroll.progress,false,[&](const auto &){++observed_requests;return false;});
        player.tick(input);scroll.finish_tick(static_cast<unsigned>(background.map.size()));++ticks;
    }
    void render_terrain(std::span<std::uint32_t> pixels,const StageBackground::Palette &colors,
                        bool stage4_overlay,int terrain_bottom,int terrain_top)const{
        if(pixels.size()!=640*400)throw std::runtime_error("Scene buffer must be640x400");
        std::fill(pixels.begin(),pixels.end(),0u);
        // Initial map row0 is bottom/top368, row22 top16 (tile.cpp:285..294).
        // Source map continues upward while screen terrain moves down. Last
        // undisplayable tile step is excluded by scroll.display_pixels.
        for(int y=terrain_top;y<terrain_bottom;++y){
            const unsigned row=(scroll.display_pixels+383-unsigned(y))/16;
            const auto tile_start=(background.map.at(row/8)*8+row%8)*24;
            const auto tile_y=unsigned((y-int(scroll.display_pixels%16)+16)%16);
            const bool overlay_row=stage4_overlay&&y>=152&&y<248;
            for(unsigned col=0;col<24;++col){
                const auto &tile=background.tiles.at(background.sections.at(tile_start+col));
                for(unsigned dx=0;dx<16;++dx){
                    const unsigned x=32+col*16+dx;
                    auto index=tile[tile_y*16+dx];
                    if(overlay_row&&x>=176&&x<272)
                        index=background.stage4_overlay_color(index,x-176,unsigned(y-152));
                    const auto &c=colors[index];
                    pixels[y*640+x]=(std::uint32_t(c[0])<<16)|(std::uint32_t(c[1])<<8)|c[2];
                }
            }
        }
    }
    void render(std::span<std::uint32_t> pixels,bool draw_player=true,bool stage4_overlay=false,int terrain_bottom=384,int terrain_top=16)const{
        render_terrain(pixels,background.palette,stage4_overlay,terrain_bottom,terrain_top);
        if(!draw_player)return;
        const unsigned glyph_id=player.pose==PlayerMotion::LeanLeft?2:player.pose==PlayerMotion::LeanRight?3:0;
        const auto &glyph=player_art.glyphs.at(glyph_id);const auto *bitmap=std::get_if<th03::assets::PlanarBitmap4>(&glyph.pixels);if(!bitmap)throw std::runtime_error("Player glyph is not planar");
        for(int y=0;y<48;++y)for(int x=0;x<32;++x){unsigned index=0;for(unsigned p=0;p<4;++p)if(bitmap->color_planes[p].at(y*4+x/8)&(128>>(x%8)))index|=1<<p;if(index==player_art.transparent_color)continue;const int px=player.x+x,py=player.y+y;if(px<32||px>=416||py<16||py>=384)continue;const auto c=background.palette[index];pixels[py*640+px]=(std::uint32_t(c[0])<<16)|(std::uint32_t(c[1])<<8)|c[2];}
    }
};
}
