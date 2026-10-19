#include "LanguagePatches.hpp"
#include <emscripten.h>
#include <zlib.h>
#include <cstdio>
#include <map>
namespace th075::sdl {
namespace {
bool read_patch(const std::string& path,std::vector<u8>& bytes){
 auto* f=std::fopen(path.c_str(),"rb");if(!f)return false;
 std::fseek(f,0,SEEK_END);const auto size=std::ftell(f);std::rewind(f);
 if(size<20||size>134217728){std::fclose(f);return false;}
 bytes.resize(std::size_t(size));const bool ok=std::fread(bytes.data(),1,bytes.size(),f)==bytes.size();std::fclose(f);return ok;
}
void append(std::vector<u8>& out,u32 value){for(u8 i=0;i<4;i++)out.push_back(u8(value>>(8*i)));}
void crypt(std::vector<u8>& text,bool music){u8 key=music?0x5c:99,step=music?0x5a:98;for(auto& b:text){b^=key;key=u8(key+step);step=u8(step+(music?0x3d:66));}}
bool images(const std::vector<u8>& source,ByteReader& patch,u32 count,std::vector<u8>& out){
 ByteReader base(source.data(),source.size());const auto palettes=base.byte();if(palettes>127||!base.take(palettes*512))return false;
 const auto header=base.offset;struct Image{u32 width,height;std::size_t first,last;};std::vector<Image> records;
 while(base.offset<source.size()){
  const auto first=base.offset;const auto width=base.dword(),height=base.dword(),stride=base.dword();const auto depth=base.byte();const auto packed=base.dword();
  if(!base.valid||!width||!height||width>8192||height>8192||stride<width||stride>8192||(depth!=8&&depth!=16&&depth!=24&&depth!=32))return false;
  const auto bytes=packed?std::uint64_t(packed):std::uint64_t(stride)*height*(depth>=24?4:depth/8);if(bytes>134217728||!base.take(std::size_t(bytes)))return false;
  records.push_back({width,height,first,base.offset});
 }
 std::map<u32,std::vector<u8>> replacements;
 for(u32 i=0;i<count;i++){
  const auto index=patch.dword(),width=patch.dword(),height=patch.dword(),packed=patch.dword(),size=patch.dword();
  if(!patch.valid||index>=records.size()||replacements.count(index)||width!=records[index].width||height!=records[index].height||size>134217728||(packed?packed!=size||size%8:std::uint64_t(width)*height*4!=size))return false;
  const auto* pixels=patch.take(size);if(!pixels)return false;
  if(packed){ByteReader runs(pixels,size);std::uint64_t total=0;while(runs.offset<size){const auto count=runs.dword();runs.dword();if(!count)return false;total+=count;}if(total!=std::uint64_t(width)*height)return false;}
  auto& record=replacements[index];append(record,width);append(record,height);append(record,width);record.push_back(32);append(record,packed);record.insert(record.end(),pixels,pixels+size);
 }
 if(patch.offset!=patch.size)return false;out.assign(source.begin(),source.begin()+header);
 for(u32 i=0;i<records.size();i++){const auto found=replacements.find(i);if(found!=replacements.end())out.insert(out.end(),found->second.begin(),found->second.end());else out.insert(out.end(),source.begin()+records[i].first,source.begin()+records[i].last);}
 return true;
}
bool text(const std::vector<u8>& source,ByteReader& patch,u32 count,bool music,std::vector<u8>& out){
 std::map<u32,std::vector<u8>> replacements;
 for(u32 i=0;i<count;i++){const auto index=patch.dword(),size=patch.dword();if(!patch.valid||size>1048576||replacements.count(index))return false;const auto* data=patch.take(size);if(!data)return false;replacements[index]={data,data+size};}
 if(patch.offset!=patch.size)return false;
 auto plain=source;crypt(plain,music);out.clear();out.reserve(plain.size());u32 quote=0,used=0;bool comment=false,lineStart=true;
 for(std::size_t at=0;at<plain.size();){
  const auto ch=plain[at];if(ch=='\n'){comment=false;lineStart=true;}else if(lineStart&&ch!=' '&&ch!='\t'&&ch!='\r'){comment=ch=='#';lineStart=false;}
  if(ch!='"'||comment){out.push_back(ch);at++;continue;}
  const auto start=++at;while(at<plain.size()&&plain[at]!='"'&&plain[at]!='\n'&&plain[at]!='\r')at++;if(at>=plain.size()||plain[at]!='"')return false;
  out.push_back('"');const auto found=replacements.find(quote++);
  if(found!=replacements.end()){out.insert(out.end(),found->second.begin(),found->second.end());used++;}else out.insert(out.end(),plain.begin()+start,plain.begin()+at);
  out.push_back('"');at++;
 }
 if(used!=count)return false;crypt(out,music);return true;
}
}
bool localized_option_description(u8 index,std::string& text){
 const auto size=EM_ASM_INT({
  if(!Module.th075Language)return 0;
  if(!Module.th075OptionDescriptions){
   let rows;try{rows=JSON.parse(FS.readFile('/thcrap/th075/localization/options.json',{encoding:'utf8'})).optionDescriptions;}catch{return 0;}
   if(!Array.isArray(rows)||rows.length!==7||rows.some(row=>!Array.isArray(row)||!row.length||row.length>65536||row.some(n=>!Number.isInteger(n)||n<0||n>255)))return 0;
   Module.th075OptionDescriptions=rows.map(row=>new Uint8Array(row));
  }
  const value=Module.th075OptionDescriptions[$0];return value?value.length:0;
 },index);
 if(size<=0)return false;text.resize(std::size_t(size));
 EM_ASM({HEAPU8.set(Module.th075OptionDescriptions[$0],$1);},index,text.data());return true;
}
bool apply_language_patch(const std::string& path,std::vector<u8>& bytes,std::string& error){
 if(!EM_ASM_INT({return !!Module.th075Language;}))return true;
 for(const bool picture:{true,false}){
  const std::string file="/thcrap/th075/"+std::string(picture?"images/":"text/")+path+".bin";
  // Missing localized resources deliberately keep the Japanese resource.
  if(!EM_ASM_INT({return FS.analyzePath(UTF8ToString($0)).exists;},file.c_str()))continue;
  std::vector<u8> patch,output;if(!read_patch(file,patch)){error="Invalid TH075 language replacement";return false;}
  ByteReader in(patch.data(),patch.size());const auto* magic=in.take(4);const auto version=in.dword(),crc=in.dword(),size=in.dword(),count=in.dword();
  const bool valid=magic&&!std::memcmp(magic,picture?"E75I":"E75T",4)&&version==(picture?2u:1u)&&size==bytes.size()&&crc==u32(crc32(0,bytes.data(),uInt(bytes.size())))&&count<=65536;
  if(!valid||!(picture?images(bytes,in,count,output):text(bytes,in,count,path=="musicroom.dat",output))){error="TH075 language replacement does not match original resource: "+path;return false;}
  bytes=std::move(output);
 }
 return true;
}
}
