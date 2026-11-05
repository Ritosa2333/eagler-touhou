// Source adapter for thprac_purple TH15 (MIT); see generated include/license.
#include "PracticeConfig.hpp"
#include "PracticeSections.hpp"
#include "PracticeSiteChecks.hpp"
#include "PracticeAbTest.hpp"
#include "EclResource.hpp"
#include <utility>
#include <algorithm>
namespace th15 {
namespace {
using std::pair;
u32 crc32(const u8* data,u32 size){u32 crc=~0u;for(u32 i=0;i<size;++i){crc^=data[i];for(int bit=0;bit<8;++bit)crc=(crc>>1)^(0xedb88320u&u32(-i32(crc&1)));}return ~crc;}
struct JumpSite {u32 file,start,target;};
class ECLHelper {
 std::vector<std::vector<u8>>& files;
 u32 file=0;size_t position=0;
public:
 bool valid=true;
 std::vector<PracticeWrite> writes;
 std::vector<JumpSite> jumps;
 explicit ECLHelper(std::vector<std::vector<u8>>& buffers):files(buffers){SetFile(0);}
 void SetFile(u32 index){file=index;position=0;if(index>=files.size())valid=false;}
 void SetPos(size_t offset){position=offset;}
 void Jump(u32 start,u32 target){jumps.push_back({file,start,target});}
 template<class T>ECLHelper& operator<<(T value){
  static_assert(sizeof(T)==1||sizeof(T)==2||sizeof(T)==4,"upstream word width");
  if(file>=files.size()||position>files[file].size()||sizeof(T)>files[file].size()-position){valid=false;return *this;}
  writes.push_back({file,u32(position),sizeof(T)});std::memcpy(files[file].data()+position,&value,sizeof(T));position+=sizeof(T);return *this;
 }
 template<class K,class T>ECLHelper& operator<<(pair<K,T> value){SetPos(size_t(value.first));return *this<<value.second;}
};
class Patcher {
 const PracticeConfig& thPracParam;
 PracticePatchEffects& effects;
 void ECLSetChapter(size_t chapter){effects.chapter_set=i32(chapter);}
 void ECLSkipChapter(size_t count){effects.chapter_disable=i32(count);}
 void ECLStarsBGMSync(){effects.stars_bgm_sync=true;}
#include "PracticePatches.inc"
public:
 ECLHelper ecl;
 Patcher(PracticeBuffers& buffers,const PracticeConfig& config):thPracParam(config),effects(buffers.effects),ecl(buffers.ecl){}
 bool apply(){if(thPracParam.section>=10000)THStageWarp(ecl,(thPracParam.section-10000)/100,thPracParam.section%100);else THPatch(ecl,thPracParam.section);return ecl.valid;}
};
}
bool patch_practice_buffers(PracticeBuffers& output,const PracticeConfig& config,std::string& error,std::vector<PracticeWrite>* writes){
 error.clear();if(writes)writes->clear();if(!config.valid()){error="Invalid purple TH15 practice parameters";return false;}
 if(config.mode!=1||!config.section)return true;
#ifndef TH15_PRACTICE_SITE_GENERATION
 bool covered=false;
 for(const auto& site:practice_site_checks){
  if(site.stage!=config.stage)continue;covered=true;
  if(site.file<0||size_t(site.file)>=output.ecl.size()){error="TH15 practice include ordinal mismatch";return false;}
  const auto& bytes=output.ecl[site.file];
  if(site.offset>bytes.size()||site.length>bytes.size()-site.offset||crc32(bytes.data()+site.offset,site.length)!=site.crc){error="TH15 practice instruction-site mismatch: file "+std::to_string(site.file)+", offset "+std::to_string(site.offset);return false;}
 }
 if(!covered){error="TH15 practice source-site inventory unavailable";return false;}
#endif
 std::vector<EclResource> verified(output.ecl.size());
 std::vector<std::vector<bool>> instruction_byte(output.ecl.size()),boundary(output.ecl.size());
 for(size_t i=0;i<output.ecl.size();++i){
  const auto& before=output.ecl[i];
  if(!verified[i].open(before.data(),u32(before.size()))){error="Invalid original ECL for TH15 practice";return false;}
  instruction_byte[i].resize(before.size());boundary[i].resize(before.size());
  for(const auto& sub:verified[i].subroutines)for(u32 p=sub.offset+16;p<sub.offset+sub.size;){
   boundary[i][p]=true;u16 length;std::memcpy(&length,before.data()+p+6,2);
   std::fill(instruction_byte[i].begin()+p,instruction_byte[i].begin()+p+length,true);p+=length;
  }
 }
 PracticeBuffers transaction=output;transaction.effects={};Patcher patcher(transaction,config);
 if(!patcher.apply()){error="TH15 purple patch exceeds owned ECL buffers";return false;}
 // Native purple intentionally extends Boss's entry into the following
 // Boss1 header for Junko/S9/S10. AB Test replaces a contiguous routine
 // region with its own complete ECLH payload. These are source-owned,
 // section-specific ranges, not permission to rewrite arbitrary headers.
 const auto source_replacement=[&](size_t file,size_t offset){
  if(config.stage==6&&file==3&&(config.section==TH15_ST7_END_S4||config.section==TH15_ST7_END_S8||config.section==TH15_ST7_END_S9||config.section==TH15_ST7_END_S10))
   return offset>=0x71c&&offset<0x72c;
  if(config.stage==2&&file==2&&config.section==TH15_ST8_AB_TEST)
   return offset>=0x694&&offset<0x694+sizeof(th15_abtest_ecl_file);
  return false;
 };
 for(size_t i=0;i<verified.size();++i)for(size_t p=0;p<output.ecl[i].size();++p)
  if(output.ecl[i][p]!=transaction.ecl[i][p]&&!instruction_byte[i][p]&&!source_replacement(i,p)){error="TH15 purple patch changed an ECL directory/header: file "+std::to_string(i)+", offset "+std::to_string(p);return false;}
 // Purple st4 boss7 jumps to its *new* instruction at 0x42b4. Unlike
 // an arbitrary offset, that target is a fully authored instruction whose
 // complete bytes were written by this transaction inside a source routine.
 std::vector<std::vector<bool>> authored(output.ecl.size());
 for(size_t i=0;i<authored.size();++i)authored[i].resize(output.ecl[i].size());
 for(const auto& write:patcher.ecl.writes)std::fill(authored[write.file].begin()+write.offset,authored[write.file].begin()+write.offset+write.length,true);
 const auto target_valid=[&](u32 file,u32 offset){
  if(file>=boundary.size()||offset>=boundary[file].size())return false;
  if(boundary[file][offset])return true;
  const auto& bytes=transaction.ecl[file];if(bytes.size()-offset<16)return false;
  u16 length;std::memcpy(&length,bytes.data()+offset+6,2);
  if(length<16||length%4||length>bytes.size()-offset)return false;
  for(u32 p=offset;p<offset+length;++p)if(!authored[file][p]||!instruction_byte[file][p])return false;
  return true;
 };
 for(const auto& jump:patcher.ecl.jumps)
  if(!target_valid(jump.file,jump.start)||!target_valid(jump.file,jump.target)){error="TH15 purple jump misses a source/authored instruction boundary: file "+std::to_string(jump.file)+", start "+std::to_string(jump.start)+", target "+std::to_string(jump.target);return false;}
 if(writes)*writes=std::move(patcher.ecl.writes);output=std::move(transaction);return true;
}
}
