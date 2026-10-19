#include "ChapterCheckpoint.hpp"
#include <algorithm>
namespace th15 {
namespace {
template<class T>void put(u8* p,u32 at,const T& v){std::memcpy(p+at,&v,sizeof v);}
template<class T>void get(const u8* p,u32 at,T& v){std::memcpy(&v,p+at,sizeof v);}
template<class S,class P,class V,class Copy>void fields(S& s,P& p,V& v,Copy copy){
 copy(0,s.stage);copy(4,s.starting_stage);copy(8,s.chapter);copy(0xc,s.stage_frame);copy(0x10,s.run_clock);copy(0x14,s.character);copy(0x18,s.subcharacter);copy(0x20,s.difficulty);copy(0x24,s.continues);copy(0x34,s.spell_id);copy(0x48,s.initial_point_value);copy(0x5c,s.checkpoint_power);copy(0x90,s.alternating_pieces);copy(0x1a4,s.stage_deaths);copy(0x1c8,s.chapter_deaths);
 copy(0x3c,p.deaths);copy(0x50,p.power);copy(0x58,p.power_step);copy(0x60,p.extra_lives);copy(0x64,p.life_pieces);copy(0x6c,p.bombs);copy(0x70,p.bomb_pieces);
 copy(0x1c,v.score);copy(0x2c,v.graze_total);copy(0x30,v.graze_chapter);copy(0x40,v.point_items);copy(0x44,v.point_value);copy(0x4c,v.max_point_value);copy(0x54,v.max_power);copy(0x68,v.life_piece_tier);copy(0x74,v.chapter_value);copy(0x7c,v.chapter_count);copy(0x80,v.collection_timer);copy(0x84,v.collection_position);
}
}
bool ChapterCheckpoint::write_file(std::vector<u8>& out){
 error.clear();out.clear();if(!available){error="No chapter progress to serialize";return false;}if(saved_music.size()>=128){error="Checkpoint music name exceeds original field";return false;}out.assign(file_metadata.begin(),file_metadata.end());fields(saved_progress,saved_player,saved_score,[&](u32 at,const auto& value){put(out.data(),at,value);});put(out.data(),0x28,saved_rank);put(out.data(),0x94,saved_total);put(out.data(),0x98,saved_defeated);std::memset(out.data()+0x9c,0,128);std::memcpy(out.data()+0x9c,saved_music.data(),saved_music.size());return true;
}
bool ChapterCheckpoint::read_file(const u8* data,u32 size,u32& consumed){
 error.clear();consumed=0;if(!data||size<0x1cc){error="Truncated chapter progress record";return false;}auto s=saved_progress;auto p=saved_player;auto v=saved_score;fields(s,p,v,[&](u32 at,auto& value){get(data,at,value);});if(s.stage<1||s.stage>7||s.character<0||s.character>3||s.difficulty<0||s.difficulty>4||s.chapter<0){error="Invalid checkpoint chapter selection";return false;}const auto* name=data+0x9c;const auto* end=std::find(name,name+128,u8(0));if(end==name+128){error="Unterminated checkpoint music name";return false;}
 i32 rank,total,defeated;get(data,0x28,rank);get(data,0x94,total);get(data,0x98,defeated);v.difficulty=s.difficulty;std::string wave(reinterpret_cast<const char*>(name),end-name);saved_progress=s;saved_player=p;saved_score=v;saved_rank=rank;saved_total=total;saved_defeated=defeated;saved_music=std::move(wave);std::memcpy(file_metadata.data(),data,0x1cc);available=true;consumed=0x1cc;return true;
}
}
