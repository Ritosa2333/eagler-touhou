#include "PracticeConfig.hpp"
#include "PracticeSections.hpp"
#include "PracticeAbTest.hpp"
#include "EclResource.hpp"
#include <algorithm>
namespace th15 {
bool patch_practice_program(EclProgram& program,const PracticeConfig& config,PracticePatchEffects& effects,std::string& error){
 PracticeBuffers next;
 for(const auto& file:program.files)next.ecl.push_back(file->bytes);
 std::vector<PracticeWrite> writes;
 if(!patch_practice_buffers(next,config,error,&writes))return false;
 if(config.mode!=1||!config.section)return true;
 std::vector<std::vector<EclSubroutine>> layout;
 for(const auto& file:program.files)layout.push_back(file->subroutines);
 // Purple's replacement is longer than Boss's original tail. Keep its
 // original lookup ordinal/name, extend only as far as the authored entry,
 // and fail closed if a caller attempts to enter the destroyed Boss1.
 if(config.stage==6&&(config.section==TH15_ST7_END_S4||config.section==TH15_ST7_END_S8||config.section==TH15_ST7_END_S9||config.section==TH15_ST7_END_S10)){
  auto& subs=layout[3];EclSubroutine* boss=nullptr;EclSubroutine* retired=nullptr;
  for(auto& sub:subs){if(sub.name=="Boss")boss=&sub;if(sub.name=="Boss1")retired=&sub;}
  if(!boss||!retired||boss->offset!=0x3e8||boss->offset+boss->size!=0x71c||retired->offset!=0x71c){error="Purple Extra entry routine layout mismatch";return false;}
  u32 end=0x6d0;
  for(const auto& write:writes)if(write.file==3&&write.offset>=0x6d0&&write.offset<0x800)end=std::max(end,write.offset+write.length);
  for(u32 p=0x6d0;p<end;){u16 length;std::memcpy(&length,next.ecl[3].data()+p+6,2);if(length<16||length%4||length>end-p){error="Purple Extra entry instruction extent mismatch";return false;}p+=length;}
  boss->size=end-boss->offset;retired->size=16;
 }
 if(config.section==TH15_ST8_AB_TEST){
  auto& subs=layout[2];EclSubroutine* replacement=nullptr;
  const u32 begin=0x694,end=begin+sizeof(th15_abtest_ecl_file);
  if(std::memcmp(next.ecl[2].data()+begin,"ECLH",4)){error="Purple AB Test routine header mismatch";return false;}
  for(auto& sub:subs){if(sub.offset==begin)replacement=&sub;else if(sub.offset>begin&&sub.offset<end)sub.size=16;}
  if(!replacement){error="Purple AB Test routine ordinal unavailable";return false;}
  for(u32 p=begin+16;p<end;){u16 length;std::memcpy(&length,next.ecl[2].data()+p+6,2);if(length<16||length%4||length>end-p){error="Purple AB Test instruction extent mismatch";return false;}p+=length;}
  replacement->size=end-begin;
 }
 // No binding/pointers exist at this boundary; unique_ptr addresses and
 // definition ordinals remain stable. Publication follows all validation.
 for(size_t i=0;i<program.files.size();++i){program.files[i]->bytes=std::move(next.ecl[i]);program.files[i]->subroutines=std::move(layout[i]);}
 effects=next.effects;return true;
}
}
