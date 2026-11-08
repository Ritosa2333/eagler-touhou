// TH13 ANM 归档（version==8，无全局魔数的 entry 链）。
// 布局依据 docs/plans/anm-pipeline.md §二（raw/front.anm 逐字节复核）：
//   entry 头 0x28 + sprite 偏移表 + {id,offset} 脚本表 + name1 + THTX；
//   sprite 记录 20 字节 {u32 id; f32 x,y,w,h}；指令头 8 字节
//   {i16 opcode; u16 size; i16 time; u16 mask}，opcode==-1 为终止符。
// entry 内 id 仅是元数据：装载器把 sprite/script 按表序追加到归档全局表
// （0x46e0b0/0x46e230），ins_300 用的编号即全局序号——解析顺序不可换。
#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th13 {
struct AnmInstruction {
    i16 opcode; u16 length; i16 time; u16 references;
    template<class T> T argument(u32 index) const noexcept {
        T value; std::memcpy(&value,reinterpret_cast<const u8*>(this)+8+index*4,sizeof value); return value;
    }
};
static_assert(sizeof(AnmInstruction)==8);
struct AnmSprite {
    u32 source_id=0,texture=0;
    float x=0,y=0,width=0,height=0;
    float u0=0,v0=0,u1=0,v1=0;
};
struct AnmTexture {
    enum class Kind {Embedded,External,Blank,RenderTarget};
    std::string name;
    Kind kind=Kind::Embedded;
    u16 width=0,height=0,format=0,pixel_width=0,pixel_height=0,pixel_format=0;
    i16 source_x=0,source_y=0;
    u32 priority=0;
    bool low_resolution_scale=false;
    std::vector<u8> pixels;
    bool rgba(std::vector<u8>& output) const;
};
struct AnmScript {
    i32 source_id=0;
    u32 file_offset=0;
    std::vector<u8> bytes;
    // 00461a60 / 0046f990 index the owning file, not other ANM files
    // packed into the same web bank. Every entry in one file shares these.
    u32 sprite_base=0,script_base=0;
};
class AnmResource {
public:
    std::vector<AnmTexture> textures;
    std::vector<AnmSprite> sprites;
    std::vector<AnmScript> scripts;
    bool open(const u8* bytes,u32 size);
    // Pack another file while retaining its local sprite/child-script domain.
    // ECL file+script pairs are translated by StagePresentation; instructions
    // inside each ANM continue to use that file's original local indices.
    bool append(const u8* bytes,u32 size);
};
}
