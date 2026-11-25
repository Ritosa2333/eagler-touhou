#pragma once
#include "bomb_core.hpp"
#include "stage_sprites.hpp"
#include "th03/assets/decode.hpp"
namespace soew {
// Original BFT masks, PI portraits and TRAM overlay, composed in hardware layer order.
struct BombRenderer {
    std::vector<std::uint8_t> portrait;
    std::array<std::array<std::uint32_t,24>,16> masks{};
    std::array<std::array<std::uint64_t,23>,18> overlays{};
    unsigned image_w=0,image_h=0;
private:
    static auto &cache(){static asset_cache_detail::Cache<BombRenderer,3> value;return value;}
public:
    static constexpr std::uint32_t reverse_red=0xff0000;
    static constexpr unsigned dac_component(unsigned ch,unsigned tone){
        const unsigned out=tone>100?15-(15-ch)*(200-tone)/100:ch*tone/100;
        // The PC-98 reference capture repeats the analog DAC bits into its
        // six-bit VGA channel, then expands that channel to eight bits.
        const unsigned dac6=(out<<2)|(out>>2);
        return (dac6<<2)|(dac6>>4);
    }
    static constexpr std::uint32_t toned(std::uint32_t rgb,unsigned tone){
        return (dac_component((rgb>>20)&15,tone)<<16)|(dac_component((rgb>>12)&15,tone)<<8)|dac_component((rgb>>4)&15,tone);
    }
    static StageBackground::Palette palette(StageBackground::Palette colors,const BombCore &b){
        if(!b.active||b.shot_type!=0)return colors;
        if(b.visual==BombCore::Mask)colors[3]={64,16,64};
        if(b.visual==BombCore::Body&&b.frame>=32&&b.frame<136){
            if(b.frame<64){
                const auto purple=std::uint8_t(256-4*b.frame);
                colors[0]={purple,0,purple};
            }else{
                const auto grey=std::uint8_t(b.frame>=86&&b.frame%3==0?160:255);
                colors[0]={grey,grey,grey};
            }
        }
        return colors;
    }
    static asset_cache_detail::Stats cache_stats(){return cache().stats();}
    static BombRenderer load(const std::filesystem::path &root,unsigned shot){
        if(shot>2)throw std::runtime_error("Invalid bomb resource type");
        const auto pi_path=root/("BOMB"+std::to_string(shot+1)+".PI");
        std::vector<asset_cache_detail::Input> inputs={{root/"BOMBS.BFT",StageScene::read(root/"BOMBS.BFT")},{pi_path,StageScene::read(pi_path)}};
        if(shot==1)inputs.push_back({root/"BOMB1.BFT",StageScene::read(root/"BOMB1.BFT")});
        return *cache().get(inputs,[&]{BombRenderer out;const auto &data=inputs[0].bytes;
        if(data.size()!=1112)throw std::runtime_error("Unexpected BOMBS.BFT size");
        // Frame 31 reads 72 bytes past the 15 on-disk cels. Use the bytes
        // observed at that read in Japanese MAIN.EXE (same for A/B/C).
        constexpr std::array<unsigned,72> frame31={
              0, 0, 0,   0, 0, 0,   0, 0, 1,   0,105,115,
              0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0,
              0, 0, 0,   0, 0, 0,   0, 0, 0,  65,65,65,
             65,65,65,  34,34,34,  34,34,34,  20,20,20,
             20, 8, 8,   8, 8, 0,   0, 0, 0,   0, 0, 0,
              0, 0, 0,   0, 0, 0,   0, 0, 0,   0, 0, 0};
        for(unsigned f=0;f<out.masks.size();++f)for(unsigned y=0;y<24;++y)for(unsigned x=0;x<24;++x){
            const unsigned at=y*3+x/8;
            const auto bits=f==15?frame31[at]:std::to_integer<unsigned>(data[32+f*72+at]);
            if(bits&(128>>(x&7)))out.masks[f][y]|=1u<<x;
        }
        if(shot==1){const auto &bytes=inputs[2].bytes;if(bytes.size()!=5264)throw std::runtime_error("Unexpected BOMB1.BFT size");
            // sub_EA6B: 23 rows,12 bytes/row, two nibbles per byte.
            // Each nonzero nibble marks two8px text cells (16px total).
            for(unsigned f=0;f<18;++f)for(unsigned y=0;y<23;++y)for(unsigned x=0;x<24;++x){const unsigned value=std::to_integer<unsigned>(bytes.at(80+f*288+y*12+x/2));if(value&(x&1?15:240))out.overlays[f][y]|=std::uint64_t(3)<<(x*2);}}
        const auto pi=th03::assets::decode_pi(inputs[1].bytes);
        if(!pi)throw std::runtime_error("Bomb PI decode failed");
        out.image_w=pi->image.width;out.image_h=pi->image.height;out.portrait=pi->image.indices;return out;});
    }
    void render(std::span<std::uint32_t> pixels,const StageBackground::Palette &colors,const BombCore &b,bool reduced)const{
        if(!b.active||b.visual==BombCore::None||b.visual==BombCore::Recovery)return;
        const int f=b.frame;
        const auto color=[&](unsigned i){
            const auto &c=colors.at(i);return (unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];
        };
        const auto put=[&](int x,int y,std::uint32_t rgb){if(x>=32&&x<416&&y>=16&&y<384)pixels[y*640+x]=rgb;};
        const auto fill=[&](std::uint32_t rgb){for(int y=16;y<384;++y)std::fill_n(pixels.begin()+y*640+32,384,rgb);};
        constexpr std::array<std::array<unsigned,5>,8> dots={{{0,0,0,16,60},{0,0,16,16,126},{0,16,16,56,255},{16,56,124,254,255},{0,16,16,56,255},{0,0,16,16,255},{0,0,0,16,126},{0,0,0,0,60}}};
        const auto particle=[&](int x,int y,unsigned cel,unsigned col){
            const auto rgb=color(col);
            blit_motion_sprite(pixels,RenderPoint::pixels(x,y),8,8,[&](int dx,int dy){return dots[dy][cel]&(128>>dx)?rgb:render_clear;});
        };
        const auto circle=[&](int radius,unsigned start,unsigned col){
            for(unsigned i=0;i<64;++i){
                const auto d=enemy_vector(start+i*4,radius);
                particle(220+d[0],196+d[1],4,col);
            }
        };
        if(b.visual==BombCore::Circle){
            circle(256-f*8,unsigned(f),15);return;
        }
        if(b.visual==BombCore::Mask){const unsigned cel=unsigned(f-1)/2;
            const auto rgb=color(b.shot_type==0?3:b.shot_type==1?15:2);
            for(unsigned y=0;y<23;++y)for(unsigned x=0;x<24;++x)if(masks[cel][y]&(1u<<x))
                for(unsigned py=0;py<16;++py)std::fill_n(pixels.begin()+(16+y*16+py)*640+32+x*16,16,rgb);
            return;
        }
        const auto image=[&](int left,int top){
            for(unsigned y=0;y<image_h;++y)for(unsigned x=0;x<image_w;++x)
                put(left+int(x),top+int(y),color(portrait[y*image_w+x]));
        };
        // th02_main.asm loc_E6CC, loc_EB32, loc_E915: three separate graphics
        // programs. Their white fills and skipped PI puts are intentional.
        if(b.shot_type==0&&f<136){
            fill(color(f>64?11:0));
            if(f>64&&f!=70&&(!reduced||(f&1)))image(112,90);
        }else if(b.shot_type==1&&f<164){
            fill(color(15));
            if(f>=133&&(!reduced||(f&1)))image(32,84);
        }else if(b.shot_type==2&&f<112){
            if(f<100)fill(color(2));
            else if(f&1)image(32,16);
            else fill(color(15));
        }
        for(const auto &p:b.particles)if(p.draw_state)particle((p.x/8)*8,p.y,p.draw_state-1,b.shot_type==0?15:b.shot_type==1?4:14);
        if(b.shot_type==2&&f>32&&f<100)for(unsigned col=0;col<48;++col){
            const int bottom=b.columns[col],left=32+int(col*8);
            const auto rgb=color(1);
            for(int y=16;y<bottom&&y<384;++y)std::fill_n(pixels.begin()+y*640+left,8,rgb);
            // bomb_smear_put_8 decrements its half-height counter as soon
            // as it loads the first tip row, so only three rows get drawn.
            blit_motion_sprite(pixels,RenderPoint::pixels(left,bottom),8,3,[&](int x,int y){return dots[y+4][4]&(128>>x)?rgb:render_clear;});
        }
        if(b.shot_type==1&&f>=133&&f<164){
            const unsigned ring=unsigned((f-133)%8+1);
            circle(int(ring*32),ring,14);
        }
    }
    void render_tone(std::span<std::uint32_t> pixels,const BombCore &b)const{
        if(!b.active)return;
        // master.lib palette_show: reduce to four bits *before* integer
        // multiplication, complementing at tones 101–200.
        const unsigned t=std::clamp(b.palette_tone,0,200);
        std::array<unsigned,16> dac{};
        for(unsigned ch=0;ch<dac.size();++ch)dac[ch]=dac_component(ch,t);
        for(int y=16;y<384;++y)for(auto &pixel:pixels.subspan(y*640+32,384))
            pixel=(dac[(pixel>>20)&15]<<16)|(dac[(pixel>>12)&15]<<8)|dac[(pixel>>4)&15];
    }
    void render_overlay(std::span<std::uint32_t> pixels,const BombCore &b)const{
        if(!b.active||b.shot_type!=1||b.visual!=BombCore::Body||!b.overlay_frame||b.frame>=132)return;
        const auto &rows=overlays.at(b.overlay_frame-1);
        for(unsigned y=0;y<rows.size();++y)for(unsigned x=0;x<48;++x)
            if(rows[y]&(std::uint64_t(1)<<x))for(unsigned py=0;py<16;++py)
                std::fill_n(pixels.begin()+(16+y*16+py)*640+32+x*8,8,reverse_red);
    }
};
}
