#pragma once
#include <array>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>
namespace soew {
// ReC98 th02/formats/mpn.hpp/map.hpp and th02_main.asm:25610..25667.
// Only background prefix is decoded; DT1 enemy/script payload remains untouched.
struct StageBackground {
    std::array<std::array<std::uint8_t,3>,16> palette{};
    std::vector<std::array<std::uint8_t,256>> tiles;
    std::array<std::uint8_t,16*8*24> sections{};
    std::vector<std::uint8_t> map;
    std::size_t script_offset=0;
    // th02_main.asm:21494–21545 selects only the intensity plane and
    // ORs the tile image into the terrain. Keep this in palette-index space.
    static constexpr unsigned stage4_overlay_index(unsigned x,unsigned y){return (y/16)*16+x/16;}
    std::uint8_t stage4_overlay_color(std::uint8_t terrain,unsigned x,unsigned y)const{
        const auto &image=tiles.at(stage4_overlay_index(x,y));
        return std::uint8_t(terrain|(image[(y%16)*16+x%16]&8u));
    }
    static StageBackground decode(std::span<const std::byte> mpn,std::span<const std::byte> section_file,std::span<const std::byte> dt1){
        const auto byte=[](auto b){return std::to_integer<std::uint8_t>(b);};
        const auto u16=[&](auto b,std::size_t i){return unsigned(byte(b[i]))|(unsigned(byte(b[i+1]))<<8);};
        StageBackground out;
        if(mpn.size()<54||std::string_view(reinterpret_cast<const char*>(mpn.data()),4)!="MPTN")throw std::runtime_error("Invalid MPN header");
        const auto count=unsigned(byte(mpn[4]))+1;
        if(mpn.size()!=54+count*128)throw std::runtime_error("Invalid MPN size");
        for(unsigned i=0;i<16;++i)for(unsigned c=0;c<3;++c)out.palette[i][c]=byte(mpn[6+i*3+c]);
        out.tiles.resize(count);
        for(unsigned tile=0;tile<count;++tile)for(unsigned y=0;y<16;++y)for(unsigned x=0;x<16;++x){
            unsigned color=0;for(unsigned plane=0;plane<4;++plane)if(byte(mpn[54+tile*128+plane*32+y*2+x/8])&(0x80>>(x%8)))color|=1<<plane;
            out.tiles[tile][y*16+x]=static_cast<std::uint8_t>(color);
        }
        if(section_file.size()!=10+out.sections.size()||std::string_view(reinterpret_cast<const char*>(section_file.data()),10)!="MIKO_MAP  ")throw std::runtime_error("Invalid MAP sections");
        for(unsigned i=0;i<out.sections.size();++i){out.sections[i]=byte(section_file[10+i]);if(out.sections[i]>=count)throw std::runtime_error("MAP tile outside MPN");}
        if(dt1.size()<6||u16(dt1,2)!=0||u16(dt1,0)!=dt1.size()-4)throw std::runtime_error("Invalid DT1 payload header");
        const auto length=u16(dt1,4);if(length<3||length>320||6+length+2>dt1.size())throw std::runtime_error("Invalid DT1 map length");
        for(unsigned i=0;i<length;++i){auto section=byte(dt1[6+i]);if(section>=16)throw std::runtime_error("Invalid DT1 map section");out.map.push_back(section);}
        out.script_offset=6+length;return out;
    }
};

// Extra road VRAM terrain: tile.cpp:181–260,272–295,452–481. Tile IDs
// are 16x16 ring cells; the two pages are pixel planes because a bulk put
// can start on an odd 8px boundary and scroll reloads only one line at a time.
struct StageTileRing {
    static constexpr int left=32,right=416,width=right-left,height=400,columns=24,rows=25;
    struct Row {
        std::array<std::uint8_t,columns> cells{};
        int map_row=-1;
        unsigned generation=0;
    };
    std::array<Row,rows> ring{};
    std::array<std::vector<std::uint8_t>,2> pages;
    std::array<std::uint8_t,columns> new_row_cache{};
    unsigned front_page=1,reloads=0,writes=0;
    int display_line=0;

    static int roll_y(int y){return (y%height+height)%height;}
    static std::uint8_t map_tile(const StageBackground &bg,unsigned row,unsigned col){
        return bg.sections.at((bg.map.at(row/8)*8+row%8)*columns+col);
    }
    void copy_line(const StageBackground &bg,unsigned page,int x,int y,unsigned image,unsigned local_y){
        const auto &tile=bg.tiles.at(image);
        for(int i=0;i<16&&x+i<right;++i)pages[page][y*width+x-left+i]=tile[local_y*16+i];
    }
    void restore_cell(const StageBackground &bg,unsigned page,unsigned row,unsigned col){
        const auto image=ring.at(row).cells.at(col);
        for(unsigned dy=0;dy<16;++dy)copy_line(bg,page,left+int(col)*16,int(row*16+dy),image,dy);
    }
    void reset(const StageBackground &bg){
        *this=StageTileRing{};
        for(auto &page:pages)page.assign(width*height,0);
        for(int row=0;row<24;++row){
            auto &r=ring[row];r.map_row=23-row;
            for(unsigned col=0;col<columns;++col){
                r.cells[col]=map_tile(bg,unsigned(r.map_row),col);
                restore_cell(bg,1,unsigned(row),col);
                restore_cell(bg,0,unsigned(row),col);
            }
        }
        // VRAM row 24 is initially covered; scroll installs it before it enters the playfield.
    }
    const char *write_error(const StageBackground &bg,int x,unsigned vram_y,unsigned image)const{
        if(image>=bg.tiles.size())return "tile image outside MPN";
        if(x<left||x>=right||(x&7)||vram_y>=height)return "tile write outside 8px-aligned playfield VRAM";
        return nullptr;
    }
    void set_and_put_both(const StageBackground &bg,int x,unsigned vram_y,unsigned image){
        if(const auto *error=write_error(bg,x,vram_y,image))throw std::runtime_error(error);
        const auto row=vram_y/16;
        ring[row].cells[(x-left)/16]=static_cast<std::uint8_t>(image);
        for(unsigned page:{front_page,front_page^1u})
            for(unsigned dy=0;dy<16;++dy)copy_line(bg,page,x,int(row*16+dy),image,dy);
        ++writes;
    }
    void scroll_both(const StageBackground &bg,int line,unsigned tile_line,unsigned top_row,bool reload){
        if(reload){
            auto &row=ring.at(unsigned(line/16));
            row.map_row=int(top_row);row.generation=++reloads;
            for(unsigned col=0;col<columns;++col)row.cells[col]=new_row_cache[col]=map_tile(bg,top_row,col);
        }
        // Original source is the separate [tiles_for_new_row] cache, not the
        // mutable tile_ring row: an AF write can change a cell but not this strip.
        for(unsigned page:{front_page,front_page^1u})
            for(unsigned col=0;col<columns;++col)
                copy_line(bg,page,left+int(col)*16,line,new_row_cache[col],tile_line);
        display_line=line;
    }
    void flip(unsigned back_page){front_page=back_page;}
    std::uint8_t pixel(unsigned page,int x,int screen_y)const{return pages.at(page).at(roll_y(screen_y+display_line)*width+x-left);}
    void render(std::span<std::uint32_t> pixels,const StageBackground &bg,int terrain_bottom=384)const{
        if(pixels.size()!=640*400)throw std::runtime_error("Scene buffer must be640x400");
        std::fill(pixels.begin(),pixels.end(),0u);
        const auto &page=pages[front_page];
        for(int y=16;y<terrain_bottom;++y){
            const auto *src=page.data()+roll_y(y+display_line)*width;
            for(int x=0;x<width;++x){const auto &c=bg.palette[src[x]];pixels[y*640+left+x]=(std::uint32_t(c[0])<<16)|(std::uint32_t(c[1])<<8)|c[2];}
        }
    }
};
}
