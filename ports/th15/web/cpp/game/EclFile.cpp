#include "EclFile.hpp"
#include <cmath>
namespace th15 {
namespace {
template<class T>void put(u8* p,u32 at,const T& v){std::memcpy(p+at,&v,sizeof v);}
template<class T>void get(const u8* p,u32 at,T& v){std::memcpy(&v,p+at,sizeof v);}
u32 word(const u8* p,u32 at){u32 v;get(p,at,v);return v;}
const EclInstruction* locate(EclProgram& p,i32 routine,i32 offset){if(routine<0||u32(routine)>=p.definitions.size()||offset<0)return nullptr;const auto& d=p.definitions[routine];const auto& s=p.subroutine(routine);if(s.size<32||u32(offset)>s.size-32||offset%4)return nullptr;const auto* code=d.file->bytes.data()+s.offset+16;u32 cursor=0;while(cursor<u32(offset)){const auto* i=reinterpret_cast<const EclInstruction*>(code+cursor);if(i->length<16||i->length%4||i->length>s.size-16-cursor)return nullptr;cursor+=i->length;}return cursor==u32(offset)?reinterpret_cast<const EclInstruction*>(code+offset):nullptr;}
bool address(const EclContext& c,const EclInstruction* target,i32& routine,i32& offset){if(!target){routine=offset=0;return true;}if(!c.program)return false;const auto ptr=reinterpret_cast<std::uintptr_t>(target);for(u32 i=0;i<c.program->definitions.size();i++){const auto& d=c.program->definitions[i];const auto& sub=c.program->subroutine(i);const auto base=reinterpret_cast<std::uintptr_t>(d.file->bytes.data()+sub.offset+16);if(sub.size>=32&&ptr>=base&&ptr-base<=sub.size-32){routine=i;offset=ptr-base;return locate(*c.program,routine,offset)==target;}}return false;}
}
bool EclFile::write(const EclContext& c,Block& out){
 error.clear();out.fill(0);auto* p=out.data();put(p,0,c.time);put(p,4,c.routine);put(p,8,c.instruction_offset);put(p,12,c.stack);put(p,0x100c,c.stack_pointer);put(p,0x1010,c.local_base);put(p,0x1014,c.thread_id);put(p,0x101c,c.thread_state);put(p,0x1020,c.difficulty);put(p,0x1024,c.interpolators);put(p,0x11e4,c.thread_flags);
 for(u32 i=0;i<8;i++){i32 routine,offset;if(!address(c,c.interpolation_targets[i],routine,offset)){error="ECL checkpoint interpolation destination is unavailable";return false;}put(p,0x11a4+i*8,routine);put(p,0x11a8+i*8,offset);}return true;
}
bool EclFile::read(const u8* data,u32 size,EclContext& out,EclProgram& program){
 error.clear();if(!data||size<0x11e8){error="Truncated ECL checkpoint context";return false;}const i32 routine=signed_bits(word(data,4)),offset=signed_bits(word(data,8));EclContext c;c.program=&program;
 if(routine>=0){if(u32(routine)>=program.definitions.size()||!c.bind(program,routine)){error="ECL checkpoint routine is unavailable";return false;}if(offset>=0){c.instruction=locate(program,routine,offset);if(!c.instruction){error="ECL checkpoint cursor is not an instruction boundary";return false;}}else c.instruction=nullptr;}else{c.routine=routine;c.instruction=nullptr;}
 c.instruction_offset=offset;get(data,0,c.time);get(data,12,c.stack);get(data,0x100c,c.stack_pointer);get(data,0x1010,c.local_base);get(data,0x1014,c.thread_id);get(data,0x101c,c.thread_state);get(data,0x1020,c.difficulty);get(data,0x1024,c.interpolators);get(data,0x11e4,c.thread_flags);
 if(!std::isfinite(c.time)||c.stack_pointer<0||c.stack_pointer>4096||c.stack_pointer%4||c.local_base<0||c.local_base>4096||c.local_base%4){error="Invalid ECL checkpoint context stack/time";return false;}
 for(u32 i=0;i<8;i++){const i32 target_routine=signed_bits(word(data,0x11a4+i*8)),target_offset=signed_bits(word(data,0x11a8+i*8));c.interpolation_targets[i]=locate(program,target_routine,target_offset);if(c.interpolators[i].duration&&!c.interpolation_targets[i]){error="ECL checkpoint interpolation instruction is unavailable";return false;}}
 out=std::move(c);return true;
}
bool EclFile::write_threads(const EclThreadsSnapshot& threads,std::vector<u8>& out,u32 owner_offset,u32 child_offset){
 error.clear();if(owner_offset>out.size()||out.size()-owner_offset<0x1210||child_offset>out.size()||threads.children.size()>65536){error="ECL checkpoint owner/child region is unavailable";return false;}Block main;if(!write(threads.main,main))return false;std::memcpy(out.data()+owner_offset+16,main.data(),main.size());put(out.data()+owner_offset,0x1200,threads.children.empty()?u32(0):u32(1));
 // Each child owns an 0x11e8 context followed by its 16-byte list node.
 for(u32 i=0;i<threads.children.size();i++){Block child;if(!write(threads.children[i],child))return false;const u32 at=child_offset+i*0x11f8;if(at>out.size()){error="ECL checkpoint child region has a gap";return false;}out.resize(at+0x11f8,0);std::memcpy(out.data()+at,child.data(),child.size());put(out.data()+at,0x11ec,i+1<threads.children.size()?u32(1):u32(0));}return true;
}
bool EclFile::read_threads(const u8* data,u32 size,EclThreadsSnapshot& out,EclProgram& p,u32 child_offset,u32& consumed){
 error.clear();consumed=0;if(!data||size<0x1210||child_offset>size){error="Truncated ECL checkpoint owner";return false;}EclThreadsSnapshot next;if(!read(data+16,size-16,next.main,p))return false;u32 at=child_offset;if(word(data,0x1200)){bool more=true;while(more){if(next.children.size()>=65536||size-at<0x11f8){error="Truncated ECL checkpoint child list";return false;}more=word(data+at,0x11ec)!=0;next.children.emplace_back();if(!read(data+at,0x11e8,next.children.back(),p))return false;at+=0x11f8;}}out=std::move(next);consumed=at-child_offset;return true;
}
}
