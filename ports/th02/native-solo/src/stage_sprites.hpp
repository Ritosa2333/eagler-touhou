#pragma once
#include "render_motion.hpp"
#include "stage_scene.hpp"
#include <optional>
namespace soew {
// master.lib BFNT_ENTRY_PAT appends to super_patnum. Header START/END supply
// count only, NOT global pattern IDs. Initial MIKO10+MIKO32 38+MIKO16 80=128.
struct StageSprites {
    struct Sprite {unsigned width=0,height=0;std::uint8_t transparent=0;std::vector<std::uint8_t> pixels;};
    std::array<std::optional<Sprite>,512> sprites;
    unsigned next=0;
    void append(const std::filesystem::path &path){append_bytes(path,StageScene::read(path));}
    void append_bytes(const std::filesystem::path &path,std::vector<std::byte> bytes){
        // Original MIKO16 member has ONE trailing zero beyond declared glyphs.
        // Keep original bytes read-only, trim only this verified decoder view.
        if(path.filename()=="MIKO16.BFT"&&bytes.size()==10321&&bytes.back()==std::byte{0})bytes.pop_back();
        auto sheet=th03::assets::decode_bfnt(bytes,th03::assets::BfntConsumerPolicy::superimpose);
        if(!sheet)throw std::runtime_error("Stage BFNT decode failed: "+path.filename().string()+": "+sheet.error().message);
        for(const auto &glyph:sheet->glyphs){
            if(next>=sprites.size())throw std::runtime_error("Stage sprite bank overflow");
            const auto *bitmap=std::get_if<th03::assets::PlanarBitmap4>(&glyph.pixels);if(!bitmap)throw std::runtime_error("Nonplanar stage sprite");
            Sprite image;image.width=sheet->header.pixel_width;image.height=sheet->header.pixel_height;image.transparent=sheet->transparent_color;image.pixels.resize(image.width*image.height);
            for(unsigned y=0;y<image.height;++y)for(unsigned x=0;x<image.width;++x){unsigned color=0;for(unsigned p=0;p<4;++p)if(bitmap->color_planes[p].at(y*(image.width/8)+x/8)&(128>>(x%8)))color|=1<<p;image.pixels[y*image.width+x]=static_cast<std::uint8_t>(color);}
            sprites[next++]=std::move(image);
        }
    }
private:
    static auto &cache(){static asset_cache_detail::Cache<StageSprites,6> value;return value;}
public:
    static asset_cache_detail::Stats cache_stats(){return cache().stats();}
    static StageSprites load(const std::filesystem::path &root,int stage){
        if(stage<0||stage>5)throw std::runtime_error("Invalid sprite stage");
        const auto prefix="STAGE"+std::to_string(stage);std::vector<asset_cache_detail::Input> inputs;
        for(const auto &name:{std::string("MIKO.BFT"),std::string("MIKO32.BFT"),std::string("MIKO16.BFT"),prefix+".BFT",prefix+".BMT",prefix+".BBT"})inputs.push_back({root/name,StageScene::read(root/name)});
        return *cache().get(inputs,[&]{StageSprites bank;
            for(unsigned i=0;i<inputs.size();++i){if(i==3&&bank.next!=128)throw std::runtime_error("Original shared sprite bank mismatch");bank.append_bytes(inputs[i].path,inputs[i].bytes);}return bank;});
    }
    // Fourth-stage preboss swaps only the stage-local patterns. Return a
    // separate bank so a failed load cannot mutate the live or cached bank.
    enum class Stage4Art { MimaIntro, Marisa };
    StageSprites stage4_art(const std::filesystem::path &root,Stage4Art part)const{
        StageSprites out;
        for(unsigned i=0;i<128;++i)out.sprites[i]=sprites[i];
        out.next=128;
        const auto append_checked=[&](const char *file,unsigned count,unsigned w,unsigned h){
            const unsigned first=out.next;
            out.append(root/file);
            if(out.next!=first+count)throw std::runtime_error("Unexpected Stage 4 sprite count");
            for(unsigned i=first;i<out.next;++i){
                const auto &s=out.sprites[i];
                if(!s||s->width!=w||s->height!=h)throw std::runtime_error("Unexpected Stage 4 sprite size");
            }
        };
        if(part==Stage4Art::MimaIntro)append_checked("MIMA.BFT",64,32,32);
        else{
            append_checked("STAGE3_B.BFT",10,48,96);
            append_checked("STAGE3_B.BTT",4,32,32);
        }
        return out;
    }
    enum class Stage5Art { Intro, Winged, Final };
    StageSprites stage5_art(const std::filesystem::path &root,Stage5Art part)const{
        StageSprites out;
        for(unsigned i=0;i<128;++i)out.sprites[i]=sprites[i];
        out.next=128;
        const auto append_checked=[&](const char *file,unsigned count,unsigned w,unsigned h){
            const unsigned first=out.next;
            out.append(root/file);
            if(out.next!=first+count)throw std::runtime_error("Unexpected Stage 5 sprite count");
            for(unsigned i=first;i<out.next;++i){
                const auto &s=out.sprites[i];
                if(!s||s->width!=w||s->height!=h)throw std::runtime_error("Unexpected Stage 5 sprite size");
            }
        };
        if(part==Stage5Art::Intro)append_checked("MIMA.BFT",64,32,32);
        else if(part==Stage5Art::Winged){append_checked("MIMA1.BFT",9,48,128);append_checked("STAGE3_B.BTT",4,32,32);}
        else append_checked("MIMA2.BFT",3,48,128);
        return out;
    }
    StageSprites extra_art(const std::filesystem::path &root)const{
        // sigma_init (th02_main.asm:15902–15905) replaces the stage-local bank.
        StageSprites out;
        for(unsigned i=0;i<128;++i)out.sprites[i]=sprites[i];
        out.next=128;
        out.append(root/"STAGE5B1.BFT");
        if(out.next!=136)throw std::runtime_error("Unexpected Sigma first sprite count");
        out.append(root/"STAGE5B2.BFT");
        if(out.next!=142)throw std::runtime_error("Unexpected Sigma second sprite count");
        for(unsigned i=128;i<out.next;++i){
            const auto &s=out.sprites[i];
            if(!s||s->width!=64||s->height!=96)throw std::runtime_error("Unexpected Sigma sprite size");
        }
        return out;
    }
    enum class Paint { Artwork, AllPlanesHit, Color4Hit };
    static unsigned paint_index(Paint paint,unsigned index){
        // Both GRCG hit modes use the cel's put mask, never its color planes.
        return paint==Paint::Color4Hit?4u:paint==Paint::AllPlanesHit?15u:index;
    }
    void put(std::span<std::uint32_t> target,const StageBackground &bg,unsigned id,int left,int top,Paint paint=Paint::Artwork)const{
        put(target,bg,id,RenderPoint::pixels(left,top),paint);
    }
    void put(std::span<std::uint32_t> target,const StageBackground &bg,unsigned id,RenderPoint point,
             Paint paint=Paint::Artwork,unsigned scale=1,bool zero_clear=false,SpriteClip clip={})const{
        if(id>=sprites.size()||!sprites[id])throw std::runtime_error("Unregistered original sprite pattern "+std::to_string(id));
        const auto &image=*sprites[id];
        const auto transparent=zero_clear?0:image.transparent;
        const auto texel=[&](int x,int y){
            const auto c=image.pixels[unsigned(y)*image.width+unsigned(x)];
            if(c==transparent)return render_clear;
            const auto &rgb=bg.palette[paint_index(paint,c)];
            return (unsigned(rgb[0])<<16)|(unsigned(rgb[1])<<8)|rgb[2];
        };
        if(scale==1)blit_motion_sprite(target,point,image.width,image.height,texel,clip);
        else blit_motion_sprite(target,point,image.width*scale,image.height*scale,[&](int x,int y){return texel(x/int(scale),y/int(scale));},clip);
    }
};
}
