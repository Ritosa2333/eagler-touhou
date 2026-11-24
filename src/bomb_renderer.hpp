#pragma once
#include "bomb_core.hpp"
#include "stage_sprites.hpp"
#include "th03/assets/decode.hpp"
namespace soew {
// Original packed mask/overlay data and PI art. Portable clipped composition;
// retained PC-98 VRAM/text overlay and palette timing are not emulated here.
struct BombRenderer {
    std::vector<std::uint8_t> portrait;
    std::array<std::array<std::uint32_t,24>,15> masks{};
    std::array<std::array<std::uint64_t,23>,18> overlays{};
    unsigned image_w=0,image_h=0;
private:
    static auto &cache(){static asset_cache_detail::Cache<BombRenderer,3> value;return value;}
public:
    static asset_cache_detail::Stats cache_stats(){return cache().stats();}
    static BombRenderer load(const std::filesystem::path &root,unsigned shot){
        if(shot>2)throw std::runtime_error("Invalid bomb resource type");
        const auto pi_path=root/("BOMB"+std::to_string(shot+1)+".PI");
        std::vector<asset_cache_detail::Input> inputs={{root/"BOMBS.BFT",StageScene::read(root/"BOMBS.BFT")},{pi_path,StageScene::read(pi_path)}};
        if(shot==1)inputs.push_back({root/"BOMB1.BFT",StageScene::read(root/"BOMB1.BFT")});
        return *cache().get(inputs,[&]{BombRenderer out;const auto &data=inputs[0].bytes;
        if(data.size()!=1112)throw std::runtime_error("Unexpected BOMBS.BFT size");
        for(unsigned f=0;f<15;++f)for(unsigned y=0;y<24;++y)for(unsigned x=0;x<24;++x)
            if(std::to_integer<unsigned>(data.at(32+f*72+y*3+x/8))&(128>>(x&7)))out.masks[f][y]|=1u<<x;
        if(shot==1){const auto &bytes=inputs[2].bytes;if(bytes.size()!=5264)throw std::runtime_error("Unexpected BOMB1.BFT size");
            // sub_EA6B: 23 rows,12 bytes/row, two nibbles per byte.
            // Each nonzero nibble marks two8px text cells (16px total).
            for(unsigned f=0;f<18;++f)for(unsigned y=0;y<23;++y)for(unsigned x=0;x<24;++x){const unsigned value=std::to_integer<unsigned>(bytes.at(80+f*288+y*12+x/2));if(value&(x&1?15:240))out.overlays[f][y]|=std::uint64_t(3)<<(x*2);}}
        const auto pi=th03::assets::decode_pi(inputs[1].bytes);
        if(!pi)throw std::runtime_error("Bomb PI decode failed");
        out.image_w=pi->image.width;out.image_h=pi->image.height;out.portrait=pi->image.indices;return out;});
    }
    void render(std::span<std::uint32_t> pixels,const StageBackground &bg,const BombCore &b,const MotionSample &motion={})const{
        if(!b.active||b.visual==BombCore::None||b.visual==BombCore::Recovery)return;
        auto color=[&](unsigned i){const auto &c=bg.palette.at(i);return (unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];};
        auto put=[&](int x,int y,unsigned col){if(x>=32&&x<416&&y>=16&&y<384)pixels[y*640+x]=color(col);};
        auto fill=[&](unsigned col){for(int y=16;y<384;++y)for(int x=32;x<416;++x)put(x,y,col);};
        constexpr std::array<std::array<unsigned,5>,8> dots={{{0,0,0,16,60},{0,0,16,16,126},{0,16,16,56,255},{16,56,124,254,255},{0,16,16,56,255},{0,0,16,16,255},{0,0,0,16,126},{0,0,0,0,60}}};
        const auto particle_at=[&](RenderPoint p,unsigned cel,unsigned col){
            const auto rgb=color(col);blit_motion_sprite(pixels,p,8,8,[&](int x,int y){return dots[y].at(cel)&(128>>x)?rgb:render_clear;});
        };
        const auto particle=[&](int x,int y,unsigned cel,unsigned col){particle_at(RenderPoint::pixels(x,y),cel,col);};
        const auto *prev=motion.previous;
        const bool continuing=prev&&prev->bomb_visual==b.visual&&prev->bomb_shot==b.shot_type;
        const auto circle=[&](int radius,unsigned start,unsigned col,int old_radius,unsigned old_start){
            const auto position=[](int r,unsigned a){return RenderPoint::pixels(220+BombCore::sar(r*enemy_sin(a+64),8),196+BombCore::sar(r*enemy_sin(a),8));};
            for(unsigned i=0;i<64;++i){
                const auto p=position(radius,(start+i*4)&255);
                particle_at(continuing?motion_lerp(position(old_radius,(old_start+i*4)&255),p,motion.phase):p,4,col);
            }
        };
        if(b.visual==BombCore::Circle){
            const int prior=continuing?prev->bomb_frame:b.frame;
            circle(256-b.frame*8,unsigned(b.frame),15,256-prior*8,unsigned(prior));return;
        }
        const int f=b.frame;
        if(b.visual==BombCore::Mask){const unsigned cel=unsigned(f-1)/2;
            // The original frame31 reads beyond its15-frame allocation. Do not
            // fabricate a sixteenth frame or reproduce unsafe heap contents.
            if(cel<masks.size())for(unsigned y=0;y<24;++y)for(unsigned x=0;x<24;++x)if(masks[cel][y]&(1u<<x))for(unsigned py=0;py<16;++py)for(unsigned px=0;px<16;++px)put(32+int(x*16+px),16+int(y*16+py),b.shot_type==0?3:b.shot_type==1?15:2);
            return;
        }
        bool image=false;int image_x=0,image_y=0;
        if(b.shot_type==0&&f>=32&&f<136){fill(f>64?11:0);image=f>64&&f!=70;image_x=112;image_y=90;}
        if(b.shot_type==1&&f>=32&&f<164){fill(0);if(f>=133){image=true;image_x=32;image_y=84;}}
        if(b.shot_type==2&&f>=32&&f<112){fill(f>=100&&!(f&1)?15:2);if(f>=100&&(f&1)){image=true;image_x=32;image_y=16;}}
        if(image)for(unsigned y=0;y<image_h;++y)for(unsigned x=0;x<image_w;++x)put(image_x+int(x),image_y+int(y),portrait.at(y*image_w+x));
        if(b.shot_type==1&&f>32&&f<132&&b.overlay_frame){const auto &rows=overlays.at(b.overlay_frame-1);for(unsigned y=0;y<23;++y)for(unsigned x=0;x<48;++x)if(rows[y]&(std::uint64_t(1)<<x))for(unsigned py=0;py<16;++py)for(unsigned px=0;px<8;++px)put(32+int(x*8+px),16+int(y*16+py),2);}
        if(b.shot_type==2&&f>32&&f<100)for(unsigned col=0;col<48;++col){
            const int bottom=b.columns[col],left=32+int(col*8);
            const int edge=continuing?motion_lerp(prev->bomb_columns[col]*256,bottom*256,motion.phase):bottom*256;
            const int whole=RenderPoint::floor(edge),frac=edge-whole*256;
            for(int y=16;y<whole&&y<384;++y)for(int x=0;x<8;++x)put(left+x,y,1);
            const auto rgb=color(1);
            blit_motion_sprite(pixels,{left*256,edge},8,4,[&](int x,int y){return dots[y+4][4]&(128>>x)?rgb:render_clear;});
            if(frac)blit_motion_sprite(pixels,{left*256,(whole-1)*256+frac},8,1,[&](int x,int){return dots[4][4]&(128>>x)?render_clear:rgb;});
        }
        if(b.shot_type==1&&f>=133&&f<164){
            const unsigned ring=unsigned((f-133)%8+1);
            const unsigned prior=continuing&&prev->bomb_frame>=133?unsigned((prev->bomb_frame-133)%8+1):ring;
            const unsigned from=prior<=ring?prior:ring;
            circle(int(ring*32),ring,14,int(from*32),from);
        }
        for(const auto &p:b.particles)if(p.draw_state)particle((p.x/8)*8,p.y,p.draw_state-1,b.shot_type==0?15:b.shot_type==1?4:14);
    }
};
}
