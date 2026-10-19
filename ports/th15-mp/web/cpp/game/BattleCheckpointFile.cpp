#include "BattleCheckpoint.hpp"
#include <algorithm>
namespace th15 {
namespace {
constexpr u32 bank_magic=0x314d4e41;
u32 read_word(const u8* p){u32 n;std::memcpy(&n,p,4);return n;}
void append_word(std::vector<u8>& out,u32 n){const u32 at=out.size();out.resize(at+4);std::memcpy(out.data()+at,&n,4);}
}
CheckpointHeader BattleCheckpoint::file_header(i64 timestamp,u32 flags)const noexcept{
 const auto& s=chapter.state();std::array<i32,10> retries{};for(u32 i=0;i<9;i++)retries[i]=s.stage_deaths[i];retries[9]=s.chapter_deaths;return CheckpointHeader::create(timestamp,s.character,s.difficulty,s.stage,s.chapter,retries,flags);
}
bool BattleCheckpoint::write_file(std::vector<u8>& out,i64 stamp,u32 flags){
 out.clear();CheckpointFile file;return prepare_file(file,stamp,flags)&&check(file.encode(out),file.error);
}
bool BattleCheckpoint::prepare_file(CheckpointFile& file,i64 stamp,u32 flags){
 error.clear();file.header=file_header(stamp,flags);auto& parts=file.sections;
 if(!check(chapter.write_file(parts[0]),chapter.error)||!check(player.write_file(parts[1],animations),player.error)||!check(background.write_file(parts[2]),background.error)||!check(enemies.write_file(parts[3]),enemies.error)||!check(bullets.write_file(parts[4]),bullets.error)||!check(items.write_file(parts[5],animations),items.error)||!check(effects.write_file(parts[6],animations),effects.error))return false;
 popups.write_checkpoint_file(parts[7]);if(!bomb||!check(bomb->write_file(parts[8]),bomb->error))return false;if(!animations.resource_names.empty()){
  std::vector<std::pair<i32,std::string>> names(animations.resource_names.begin(),animations.resource_names.end());std::sort(names.begin(),names.end());const u32 at=parts[8].size();append_word(parts[8],names.size());for(const auto& name:names){append_word(parts[8],u32(name.first));append_word(parts[8],name.second.size()+1);parts[8].insert(parts[8].end(),name.second.begin(),name.second.end());parts[8].push_back(0);}append_word(parts[8],u32(parts[8].size()-at));append_word(parts[8],bank_magic);
 }
 return true;
}
bool BattleCheckpoint::read_file(const u8* data,u32 size,u32 flags){
 error.clear();CheckpointFile file;if(!check(file.open(data,size),file.error))return false;if(!file.header.compatible(progress.character,progress.difficulty,flags)||file.header.stage()!=progress.stage){error="Pointdevice checkpoint differs from active run selection";return false;}
 // Every module resolves resources through this scene's typed owners. File
 // process addresses are never dereferenced or used as callbacks.
 std::unordered_map<i32,i32> mapping;u32 bomb_size=file.sections[8].size();const auto& bomb_part=file.sections[8];
 if(bomb_size>=8&&read_word(bomb_part.data()+bomb_size-4)==bank_magic){const u32 length=read_word(bomb_part.data()+bomb_size-8);if(length<4||length>bomb_size-8||bomb_size-8-length<0x54){error="Invalid checkpoint resource table length";return false;}const u32 start=bomb_size-8-length,end=bomb_size-8,count=read_word(bomb_part.data()+start);u32 at=start+4;if(count>4096){error="Checkpoint resource table exceeds limit";return false;}for(u32 i=0;i<count;i++){if(end-at<8){error="Truncated checkpoint resource table";return false;}const i32 id=signed_bits(read_word(bomb_part.data()+at));const u32 bytes=read_word(bomb_part.data()+at+4);at+=8;if(id<0||!bytes||bytes>1025||bytes>end-at||bomb_part[at+bytes-1]||mapping.count(id)){error="Invalid checkpoint resource name";return false;}std::string name(reinterpret_cast<const char*>(bomb_part.data()+at),bytes-1);if(name.find(char(0))!=std::string::npos){error="Invalid checkpoint resource name terminator";return false;}i32 current=-1;for(const auto& candidate:animations.resource_names)if(candidate.second==name){current=candidate.first;break;}mapping.emplace(id,current);at+=bytes;}if(at!=end){error="Unexpected bytes in checkpoint resource table";return false;}bomb_size=start;}
 struct BankGuard{AnmManager& manager;const std::unordered_map<i32,i32>* previous;~BankGuard(){manager.checkpoint_banks=previous;}} banks{animations,animations.checkpoint_banks};if(!mapping.empty())animations.checkpoint_banks=&mapping;
 struct Guard{AnmCheckpoint& pool;AnmCheckpoint::Mark mark;bool committed=false;~Guard(){if(!committed)pool.rollback(mark);}} guard{animation_pool,animation_pool.mark()};u32 used=0;
 auto complete=[&](bool ok,u32 index,const std::string& failure)->bool{if(!check(ok,failure))return false;if(used!=(index==8?bomb_size:file.sections[index].size())){error="Unexpected bytes after checkpoint module "+std::to_string(index);return false;}return true;};auto& p=file.sections;
 if(!complete(chapter.read_file(p[0].data(),p[0].size(),used),0,chapter.error))return false;
 const auto& saved=chapter.state();if(saved.stage!=file.header.stage()||saved.character!=file.header.character()||saved.difficulty!=file.header.difficulty()||saved.chapter!=file.header.chapter()){error="Checkpoint header differs from chapter payload";return false;}
 if(!complete(player.read_file(p[1].data(),p[1].size(),used,animations),1,player.error)||!complete(background.read_file(p[2].data(),p[2].size(),used),2,background.error)||!complete(enemies.read_file(p[3].data(),p[3].size(),used),3,enemies.error)||!complete(bullets.read_file(p[4].data(),p[4].size(),used),4,bullets.error)||!complete(items.read_file(p[5].data(),p[5].size(),used,animations),5,items.error)||!complete(effects.read_file(p[6].data(),p[6].size(),used,animations),6,effects.error)||!check(popups.read_checkpoint_file(p[7].data(),p[7].size()),popups.error)||!bomb||!complete(bomb->read_file(p[8].data(),bomb_size,used),8,bomb->error))return false;
 guard.committed=true;return true;
}
}
