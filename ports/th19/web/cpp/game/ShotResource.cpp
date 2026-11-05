#include "ShotResource.hpp"
namespace th19 {
bool ShotFile::load(const u8* data,u32 size) {
    if(size<4)return false;
    version=u16(data[0])|(u16(data[1])<<8);
    header=u16(data[2])|(u16(data[3])<<8);
    bytes.assign(data,data+size);
    return true;
}
}
