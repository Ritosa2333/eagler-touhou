#pragma once
#include "dialogue_core.hpp"
#include "stage_sprites.hpp"
namespace soew {
// Original MIKO_K.MPN portraits and shared dialogue-box sprites. Text is a host
// callback (CP932->MS Gothic), not a claim of PC-98 ROM glyph equivalence.
struct DialogueRenderer {
    std::vector<std::array<std::uint8_t,256>> tiles;
private:
    static auto &cache(){static asset_cache_detail::Cache<DialogueRenderer,1> value;return value;}
public:
    static asset_cache_detail::Stats cache_stats(){return cache().stats();}
    static DialogueRenderer load(const std::filesystem::path &root){
        const std::vector<asset_cache_detail::Input> inputs={{root/"MIKO_K.MPN",StageScene::read(root/"MIKO_K.MPN")}};
        return *cache().get(inputs,[&]{const auto &b=inputs[0].bytes;
        if(b.size()<54||std::string_view(reinterpret_cast<const char*>(b.data()),4)!="MPTN")throw std::runtime_error("Invalid dialogue MPN");
        const unsigned count=std::to_integer<unsigned>(b[4])+1;
        if(b.size()!=54+count*128||count<185)throw std::runtime_error("Invalid dialogue portrait tile count");
        DialogueRenderer out;out.tiles.resize(count);
        for(unsigned t=0;t<count;++t)for(unsigned y=0;y<16;++y)for(unsigned x=0;x<16;++x){unsigned c=0;for(unsigned p=0;p<4;++p)if(std::to_integer<unsigned>(b[54+t*128+p*32+y*2+x/8])&(128>>(x&7)))c|=1<<p;out.tiles[t][y*16+x]=std::uint8_t(c);}
        return out;});
    }
    void render(std::span<std::uint32_t> pixels,const StageBackground &bg,const StageSprites &bank,const StageDialogue &d)const{
        if(!d.active()||!d.show_portrait())return;
        for(unsigned i=0;i<12;++i){const unsigned top=i<3?34+i:i==11?38:37;const int x=d.box_left+int(i*32);bank.put(pixels,bg,top,x,320);bank.put(pixels,bg,top+8,x,352);}
        if(d.phase!=StageDialogue::Phase::Text&&d.phase!=StageDialogue::Phase::KeyDelay&&d.phase!=StageDialogue::Phase::IntroAppearance)return;
        // FACE_COL_0 is a fill, not an MPN tile index. During Sigma's
        // appearance the preceding intro box still has its blank portrait.
        const unsigned face=d.stage_id==5&&d.phase==StageDialogue::Phase::IntroAppearance?StageDialogue::face_col_0:d.face();
        for(unsigned row=0;row<3;++row)for(unsigned col=0;col<3;++col)for(unsigned y=0;y<16;++y)for(unsigned x=0;x<16;++x){const auto c=bg.palette[face==StageDialogue::face_col_0?0:tiles.at(face+row*16+col)[y*16+x]];pixels[(328+row*16+y)*640+40+col*16+x]=(unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];}
    }
};
}
