#include "StageResource.hpp"
namespace th09 {
namespace {float real(const u8* p){u32 n=read32(p);float f;std::memcpy(&f,&n,4);return f;}Vec3 vector(const u8* p){return {real(p),real(p+4),real(p+8)};}}
bool StageResource::load(const u8* bytes,u32 size){
    objects.clear();instances.clear();instructions.clear();animation_count=0;
    if(!bytes||size<0x490)return false;const u32 count=read16(bytes),declared=read16(bytes+2),instance_offset=read32(bytes+4),script_offset=read32(bytes+8);
    if(count>4096||0x490+u64(count)*4>size||instance_offset>=size||script_offset>=size)return false;
    for(u32 n=0;n<count;++n){u32 off=read32(bytes+0x490+n*4);if(off>size||size-off<28)return false;const u8* p=bytes+off;
        StageObject o;o.id=i16(read16(p));o.layer=p[2];o.flags=1;o.position=vector(p+4);o.size=vector(p+16);off+=28;bool ended=false;
        while(off<=size&&size-off>=4){p=bytes+off;const i16 type=i16(read16(p));if(type<0){ended=true;break;}const u16 length=read16(p+2);
            if(length<8||length>size-off||type>1||(type==0&&length<28)||(type==1&&length<36))return false;
            StagePrimitive prim;prim.type=type;prim.script=i16(read16(p+4));prim.animation=animation_count++;prim.position=vector(p+8);
            if(type==0)prim.size={real(p+20),real(p+24)};else{prim.end=vector(p+20);prim.width=real(p+32);}o.primitives.push_back(prim);off+=length;
        }if(!ended)return false;objects.push_back(std::move(o));
    }if(animation_count!=declared)return false;
    bool ended=false;for(u32 off=instance_offset;off<=size&&size-off>=16;off+=16){const i16 id=i16(read16(bytes+off));if(id<0){ended=true;break;}if(u32(id)>=objects.size())return false;instances.push_back({id,read16(bytes+off+2),vector(bytes+off+4)});}if(!ended)return false;
    ended=false;for(u32 off=script_offset;off<=size&&size-off>=20;off+=20){const u8* p=bytes+off;StageInstruction i{signed_bits(read32(p)),i16(read16(p+4)),read16(p+6),{read32(p+8),read32(p+12),read32(p+16)}};instructions.push_back(i);if(i.time==-1){ended=true;break;}}
    return ended;
}
}
