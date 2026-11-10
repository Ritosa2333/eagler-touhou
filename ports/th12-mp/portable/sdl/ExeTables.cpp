#include "ExeTables.hpp"
#include "BinaryReader.hpp"
#include <algorithm>
#include <cstring>

namespace th12host {
namespace {
u32 word(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
u16 half(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
// The two TH12 1.00b identities upstream accepts (scripts/retail-versions.json).
constexpr const char* known[][2]{
    {"d8d644d2e64957a3031b1a1399d0502e1ddaa5252d2c4e492770ad6717827628","Japanese retail 1.00b"},
    {"99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417","1.00b behaviour-reference executable"}};

struct Image {
    const std::vector<u8>& b;u32 base=0;
    struct Section {u32 va,vsize,raw,rsize;};std::vector<Section> sections;
    bool parse(){
        if(b.size()<0x100||b[0]!='M'||b[1]!='Z')return false;const u32 pe=word(&b[0x3c]);
        if(pe+0xf8>b.size()||std::memcmp(&b[pe],"PE\0\0",4))return false;
        const u32 count=half(&b[pe+6]),optional=half(&b[pe+20]);base=word(&b[pe+24+28]);
        const u32 table=pe+24+optional;if(table+count*40>b.size())return false;
        for(u32 i=0;i<count;++i){const u8* s=&b[table+i*40];sections.push_back({word(s+12),word(s+8),word(s+20),word(s+16)});}
        return true;
    }
    // A pointer to `length` file bytes at virtual address `va`, or null.
    const u8* at(u32 va,u32 length)const{
        if(va<base)return nullptr;const u32 rva=va-base;
        for(const auto& s:sections)if(rva>=s.va&&rva-s.va+length<=std::max(s.vsize,s.rsize)){
            const u32 offset=s.raw+(rva-s.va);if(rva-s.va+length>s.rsize||offset+length>b.size())return nullptr;return &b[offset];}
        return nullptr;
    }
};
}

bool ExeTables::load(const std::vector<u8>& exe){
    sha256=sha256_hex(exe.data(),exe.size());version.clear();
    for(const auto& k:known)if(sha256==k[0])version=k[1];
    Image image{exe};if(!image.parse()){error="th12.exe is not a Windows executable";return false;}
    const auto bad=[&](const char* what){error=std::string("th12.exe: ")+what+" table not found (needs the 1.00b executable; SHA-256 "+sha256+")";return false;};
    // Bullet visual types: 31 rows of 0xd0 bytes at 0x4af280.
    const u8* rows=image.at(0x4af280,31*0xd0);if(!rows)return bad("bullet");
    for(u32 i=0;i<31;++i){const u8* r=rows+i*0xd0;auto& t=bullets[i];t.script=i32(word(r));
        for(u32 j=0;j<16;++j)t.colors[j]={i32(word(r+4+j*12)),i32(word(r+8+j*12)),i32(word(r+12+j*12))};
        std::memcpy(&t.size,r+0xc4,4);t.flags=word(r+0xc8);
        if(t.script<0||t.script>4096)return bad("bullet");}
    // Sounds: 60 records of 20 bytes at 0x4ae5a0; file names through the
    // pointer table at 0x4aea50.
    for(u32 i=0;i<60;++i){const u8* r=image.at(0x4ae5a0+i*20,20);if(!r)return bad("sound");
        const u32 name_index=word(r+4);const u8* pointer=image.at(0x4aea50+name_index*4,4);if(!pointer||name_index>256)return bad("sound");
        const u8* name=image.at(word(pointer),1);if(!name)return bad("sound");
        std::string file;for(u32 n=0;n<200&&image.at(word(pointer)+n,1)&&name[n];++n)file+=char(name[n]);
        if(file.size()<8||file.compare(0,3,"se_")||file.compare(file.size()-4,4,".wav"))return bad("sound");
        auto& s=sounds[i];s.id=i32(word(r));s.file=file;std::memcpy(&s.volume,r+8,2);s.loop=word(r+12)!=0;}
    std::sort(sounds.begin(),sounds.end(),[](const SoundRecord& a,const SoundRecord& b){return a.id<b.id;});
    for(u32 i=1;i<60;++i)if(sounds[i].id==sounds[i-1].id)return bad("sound");
    // Background near/far planes (prepare-presentation.py).
    const u8* planes=image.at(0x4a3e14,8);if(!planes)return bad("camera");
    std::memcpy(&near_plane,planes,4);std::memcpy(&far_plane,planes+4,4);
    if(!(near_plane>0&&far_plane>near_plane&&far_plane<100000))return bad("camera");
    // Music Room warning text: eight string pointers (prepare-menu-assets.py music_warning).
    music_warning.clear();
    if(const u8* table=image.at(0x4b33c0,32))for(u32 i=0;i<8;++i){
        std::string line;const u32 va=word(table+i*4);for(u32 n=0;n<256;++n){const u8* c=image.at(va+n,1);if(!c||!*c)break;line+=char(*c);}
        music_warning.push_back(line);}
    error.clear();return true;
}

bool ExeTables::load_prepared(const std::vector<u8>& bytes){
    try{
        BinaryReader r(bytes);r.magic("TH12META",8);if(r.word()!=1)throw std::runtime_error("Unsupported platform tables");
        near_plane=r.real();far_plane=r.real();if(!(near_plane>0&&far_plane>near_plane))throw std::runtime_error("Invalid camera planes");
        if(r.word()!=bullets.size())throw std::runtime_error("Invalid bullet table size");
        for(auto& type:bullets){type.script=r.integer();for(auto& color:type.colors){color.sprite=r.integer();color.spawn=r.integer();color.cancel=r.integer();}type.size=r.real();type.flags=r.word();}
        if(r.word()!=sounds.size())throw std::runtime_error("Invalid sound table size");
        for(auto& sound:sounds){sound.id=r.integer();sound.volume=i16(r.integer());sound.loop=r.word()!=0;sound.file=r.text();if(sound.id<0||sound.id>=64||sound.file.find('/')!=std::string::npos)throw std::runtime_error("Invalid sound record");}
        const auto count=r.word();if(count>64)throw std::runtime_error("Invalid music warning");music_warning.clear();for(u32 i=0;i<count;++i)music_warning.push_back(r.text());
        if(!r.ended())throw std::runtime_error("Trailing platform table bytes");warning_utf8=true;version="prepared TH12 1.00b";return true;
    }catch(const std::exception& e){error=e.what();return false;}
}

// FIPS 180-4 SHA-256, used once at start-up to name the executable version.
std::string sha256_hex(const u8* data,size_t size){
    static constexpr u32 k[64]{0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    u32 h[8]{0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    const auto rotr=[](u32 x,u32 n){return (x>>n)|(x<<(32-n));};
    const auto block=[&](const u8* p){
        u32 w[64];for(u32 i=0;i<16;++i)w[i]=u32(p[i*4])<<24|u32(p[i*4+1])<<16|u32(p[i*4+2])<<8|p[i*4+3];
        for(u32 i=16;i<64;++i){const u32 s0=rotr(w[i-15],7)^rotr(w[i-15],18)^(w[i-15]>>3),s1=rotr(w[i-2],17)^rotr(w[i-2],19)^(w[i-2]>>10);w[i]=w[i-16]+s0+w[i-7]+s1;}
        u32 a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
        for(u32 i=0;i<64;++i){const u32 t1=hh+(rotr(e,6)^rotr(e,11)^rotr(e,25))+((e&f)^(~e&g))+k[i]+w[i],t2=(rotr(a,2)^rotr(a,13)^rotr(a,22))+((a&b)^(a&c)^(b&c));
            hh=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;}
        h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=hh;
    };
    size_t i=0;for(;i+64<=size;i+=64)block(data+i);
    u8 tail[128]{};const size_t rest=size-i;std::memcpy(tail,data+i,rest);tail[rest]=0x80;
    const size_t length=rest+9<=64?64:128;const u64 bits=u64(size)*8;for(int n=0;n<8;++n)tail[length-1-n]=u8(bits>>(8*n));
    block(tail);if(length==128)block(tail+64);
    static constexpr char digits[]="0123456789abcdef";std::string out;
    for(u32 v:h)for(int n=7;n>=0;--n)out+=digits[(v>>(n*4))&15];return out;
}
}
