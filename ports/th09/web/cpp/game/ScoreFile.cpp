#include "ScoreFile.hpp"
#include "Binary.hpp"
#include "ResourceCrypt.hpp"
namespace th09 {
namespace {u8 rotate(u8 value){return u8((value<<3)|(value>>5));}}
bool ScoreFile::valid() const {
    if(bytes.size()<24||read16(bytes.data()+4)!=4||read32(bytes.data()+8)!=24||read32(bytes.data()+12)!=bytes.size())return false;
    if(read32(bytes.data()+16)!=bytes.size()-24)return false;
    bool found=false;u8 version=0;
    for(u32 at=24;at<bytes.size();){
        if(bytes.size()-at<12)return false;
        const auto* chapter=bytes.data()+at;const u32 size=read16(chapter+4);
        if(size<12||size>bytes.size()-at)return false;
        if(read32(chapter)==fourcc('T','H','9','K')){found=true;version=chapter[8];}
        at+=size;
    }
    return found&&version==1;
}
bool ScoreFile::assign(const u8* data,u32 size){bytes.assign(data,data+size);if(valid())return true;bytes.clear();return false;}
bool ScoreFile::decode(const u8* data,u32 size){
    bytes.clear();if(size<24||size>4*1024*1024)return false;
    std::vector<u8> plain(size);resource_crypt(data,plain.data(),size,{0x3a,0xcd,0x100,0xc00},false);
    u8 key=0;u16 sum=0;
    for(u32 i=2;i<size;++i){key=rotate(u8(key+plain[i-1]));plain[i]^=key;if(i>=4)sum=u16(sum+plain[i]);}
    const u32 payload=read32(plain.data()+16),packed=read32(plain.data()+20);
    if(read16(plain.data()+2)!=sum||read16(plain.data()+4)!=4||read32(plain.data()+8)!=24||payload>0xa0000||read32(plain.data()+12)!=payload+24||packed>size-24)return false;
    bytes.resize(24+payload);std::memcpy(bytes.data(),plain.data(),24);u32 written=0;
    if(!codec.decode(plain.data()+24,packed,bytes.data()+24,payload,written)||written!=payload||!valid()){bytes.clear();return false;}
    return true;
}
std::vector<u8> ScoreFile::encode(Rng& rng){
    if(!valid())return {};
    auto packed=codec.encode(bytes.data()+24,bytes.size()-24);
    std::vector<u8> plain(24+packed.size());std::memcpy(plain.data(),bytes.data(),24);std::memcpy(plain.data()+24,packed.data(),packed.size());
    write32(plain.data()+20,packed.size());write16(plain.data()+2,0);
    plain[1]=u8(rng.below(256));plain[6]=u8(rng.below(256));
    u16 sum=0;for(u32 i=4;i<plain.size();++i)sum=u16(sum+plain[i]);write16(plain.data()+2,sum);
    u8 key=plain[1];for(u32 i=2;i<plain.size();++i){const u8 value=plain[i];key=rotate(key);plain[i]^=key;key=u8(key+value);}
    std::vector<u8> output(plain.size());resource_crypt(plain.data(),output.data(),output.size(),{0x3a,0xcd,0x100,0xc00},true);return output;
}
}
