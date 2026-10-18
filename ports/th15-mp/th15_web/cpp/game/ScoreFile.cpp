#include "ScoreFile.hpp"
#include "Lzss.hpp"
#include "ResourceCrypt.hpp"
#include <algorithm>
namespace th15 {
namespace {
template<class T>T read(const u8* p,u32 at){T v;std::memcpy(&v,p+at,sizeof v);return v;}
template<class T>void write(u8* p,u32 at,T v){std::memcpy(p+at,&v,sizeof v);}
constexpr u8 spell_ranks[]={2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4};
}
u32 ScoreFile::checksum(const u8* p,u32 size)noexcept{u32 result=0;for(u32 n=8;n<size;n++)result+=p[n];return result;}
bool ScoreFile::fail(const char* reason){if(error.empty())error=reason;return false;}
void ScoreFile::reset(Rng& random){
 error.clear();header.fill(0);write(header.data(),0,u32(0x31354854));write(header.data(),8,u16(3));write(header.data(),12,u32(0x100));
 settings.fill(0);write(settings.data(),0,u32(0x15453));write(settings.data(),8,u32(settings_size));std::memset(settings.data()+12,' ',8);
 for(u32 n=0;n<494;n++)write(settings.data(),0x50+n*2,random.next16());
 for(auto& block:characters){block.fill(0);write(block.data(),0,u32(0x15243));write(block.data(),8,u32(character_size));
  for(u32 mode=0;mode<2;mode++){
   const u32 base=mode*0x5188;
   for(u32 difficulty=0;difficulty<6;difficulty++)for(u32 slot=0;slot<10;slot++){
    const u32 at=base+0x10+difficulty*320+slot*32;write(block.data(),at,u32(100000-slot*10000));block[at+4]=1;std::memset(block.data()+at+6,'-',8);
   }
   for(u32 spell=0;spell<119;spell++){const u32 at=base+0x960+spell*0x9c;write(block.data(),at,i32(spell));write(block.data(),at+4,i32(i8(spell_ranks[spell])));}
  }
 }
}
bool ScoreFile::open(const u8* bytes,u32 size){
 error.clear();if(!bytes||size<header.size()||read<u32>(bytes,0)!=0x31354854||read<u16>(bytes,8)!=3)return fail("Invalid score file header");
 const u32 packed=read<u32>(bytes,16),unpacked=read<u32>(bytes,20);if(packed>size-header.size()||unpacked>2*1024*1024)return fail("Score file size outside supported range");
 std::vector<u8> decoded(unpacked),compressed(packed);if(packed&&!resource_crypt(bytes+header.size(),compressed.data(),packed,{0xac,0x35,0x10,packed},false))return fail("Score file decryption failed");
 Lzss codec;u32 written=0;if(!codec.decode(compressed.data(),packed,decoded.data(),unpacked,written)||written!=unpacked)return fail("Invalid compressed score file");
 auto next_characters=characters;auto next_settings=settings;
 for(u32 cursor=0;cursor<unpacked;){
  if(unpacked-cursor<12)return fail("Truncated score block");const auto* p=decoded.data()+cursor;const u16 kind=read<u16>(p,0);if(kind!=0x5243&&kind!=0x5453)break;
  const u32 length=read<u32>(p,8);if(length<12||length>unpacked-cursor)return fail("Invalid score block length");
  if(read<u16>(p,2)==1){
   if(kind==0x5243&&length==character_size&&checksum(p,character_size)==read<u32>(p,4)){
    const u32 character=read<u32>(p,12);if(character>=characters.size())return fail("Score character outside original range");std::copy(p,p+character_size,next_characters[character].begin());
   }else if(kind==0x5453&&length==settings_size&&checksum(p,settings_size)==read<u32>(p,4))std::copy(p,p+settings_size,next_settings.begin());
  }
  cursor+=length;
 }
 std::copy(bytes,bytes+header.size(),header.begin());characters=std::move(next_characters);settings=std::move(next_settings);return true;
}
bool ScoreFile::save(std::vector<u8>& output){
 if(!error.empty())return false;std::vector<u8> decoded;decoded.reserve(character_size*5+settings_size);
 for(u32 n=0;n<characters.size();n++){auto& block=characters[n];if(read<u16>(block.data(),0)!=0x5243)continue;write(block.data(),12,n);write(block.data(),4,checksum(block.data(),character_size));decoded.insert(decoded.end(),block.begin(),block.end());}
 write(settings.data(),4,checksum(settings.data(),settings_size));decoded.insert(decoded.end(),settings.begin(),settings.end());
 Lzss codec;const auto packed=codec.encode(decoded.data(),decoded.size());if(packed.empty())return fail("Score file compression failed");std::vector<u8> encrypted(packed.size());
 if(!resource_crypt(packed.data(),encrypted.data(),packed.size(),{0xac,0x35,0x10,u32(packed.size())},true))return fail("Score file encryption failed");
 write(header.data(),4,u32(header.size()+packed.size()));write(header.data(),16,u32(packed.size()));write(header.data(),20,u32(decoded.size()));output.assign(header.begin(),header.end());output.insert(output.end(),encrypted.begin(),encrypted.end());return true;
}
}
