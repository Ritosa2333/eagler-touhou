#include "RecordStore.hpp"
#include <algorithm>
namespace th15 {
namespace {
template<class T>T read(const u8* p,u32 at){T v;std::memcpy(&v,p+at,sizeof v);return v;}
template<class T>void write(u8* p,u32 at,T v){std::memcpy(p+at,&v,sizeof v);}
}
bool RecordStore::fail(const char* message){if(error.empty())error=message;return false;}
bool RecordStore::character_valid(i32 character)const noexcept{return character>=0&&character<5;}
bool RecordStore::stage_valid(i32 character,i32 stage,i32 difficulty)const noexcept{return character_valid(character)&&stage>=0&&stage<8&&difficulty>=0&&difficulty<5;}
void RecordStore::count(i32& value,i32 limit)noexcept{if(value<limit)value=wrapping_add(value,1);}
void RecordStore::decode(){
 for(u32 character=0;character<5;character++){const auto* bytes=file.characters[character].data();auto& result=characters[character];
  for(u32 mode=0;mode<2;mode++){auto& output=result.modes[mode];const u32 base=mode*0x5188;
   for(u32 rank=0;rank<6;rank++)for(u32 n=0;n<10;n++){auto& row=output.scores[rank][n];const u32 at=base+0x10+rank*320+n*32;row.score=read<i32>(bytes,at);row.stage=bytes[at+4];row.continues=i8(bytes[at+5]);std::memcpy(row.name.data(),bytes+at+6,9);row.reserved=bytes[at+15];for(u32 k=0;k<4;k++)row.details[k]=read<i32>(bytes,at+16+k*4);}
   for(u32 n=0;n<119;n++){auto& row=output.spells[n];const u32 at=base+0x8d0+n*0x9c;std::memcpy(row.name.data(),bytes+at,128);for(u32 k=0;k<2;k++){row.captures[k]=read<i32>(bytes,at+0x80+k*4);row.attempts[k]=read<i32>(bytes,at+0x88+k*4);}row.identifier=read<i32>(bytes,at+0x90);row.difficulty=read<i32>(bytes,at+0x94);row.score=read<i32>(bytes,at+0x98);}
   output.play_centiseconds=read<u64>(bytes,base+0x5158);output.plays=read<i32>(bytes,base+0x5154);for(u32 rank=0;rank<5;rank++){output.clears[rank]=read<i32>(bytes,base+0x5160+rank*4);output.without_continue[rank]=read<i32>(bytes,base+0x517c+rank*4);}
  }
  for(u32 rank=0;rank<5;rank++)for(u32 stage=0;stage<8;stage++){auto& row=result.practice[rank][stage];const u32 at=0xa318+(rank*8+stage)*8;row.score=read<i32>(bytes,at);row.cleared=bytes[at+4];row.visited=bytes[at+5];}
 }
 std::memcpy(name.data(),file.settings.data()+12,9);std::copy_n(file.settings.data()+0x16,8,endings.begin());ending_seen=file.settings[0x1e];std::copy(file.settings.begin()+0x26,file.settings.begin()+0x3a,music.begin());total_play_centiseconds=read<u64>(file.settings.data(),0x48);
}
void RecordStore::encode(){
 for(u32 character=0;character<5;character++){auto* bytes=file.characters[character].data();const auto& input=characters[character];
  for(u32 mode=0;mode<2;mode++){const auto& bank=input.modes[mode];const u32 base=mode*0x5188;
   for(u32 rank=0;rank<6;rank++)for(u32 n=0;n<10;n++){const auto& row=bank.scores[rank][n];const u32 at=base+0x10+rank*320+n*32;write(bytes,at,row.score);bytes[at+4]=row.stage;bytes[at+5]=u8(row.continues);std::memcpy(bytes+at+6,row.name.data(),9);bytes[at+15]=row.reserved;for(u32 k=0;k<4;k++)write(bytes,at+16+k*4,row.details[k]);}
   for(u32 n=0;n<119;n++){const auto& row=bank.spells[n];const u32 at=base+0x8d0+n*0x9c;std::memcpy(bytes+at,row.name.data(),128);for(u32 k=0;k<2;k++){write(bytes,at+0x80+k*4,row.captures[k]);write(bytes,at+0x88+k*4,row.attempts[k]);}write(bytes,at+0x90,row.identifier);write(bytes,at+0x94,row.difficulty);write(bytes,at+0x98,row.score);}
   write(bytes,base+0x5158,bank.play_centiseconds);write(bytes,base+0x5154,bank.plays);for(u32 rank=0;rank<5;rank++){write(bytes,base+0x5160+rank*4,bank.clears[rank]);write(bytes,base+0x517c+rank*4,bank.without_continue[rank]);}
  }
  for(u32 rank=0;rank<5;rank++)for(u32 stage=0;stage<8;stage++){const auto& row=input.practice[rank][stage];const u32 at=0xa318+(rank*8+stage)*8;write(bytes,at,row.score);bytes[at+4]=row.cleared;bytes[at+5]=row.visited;}
 }
 std::memcpy(file.settings.data()+12,name.data(),9);std::copy(endings.begin(),endings.end(),file.settings.begin()+0x16);file.settings[0x1e]=ending_seen;std::copy(music.begin(),music.end(),file.settings.begin()+0x26);write(file.settings.data(),0x48,total_play_centiseconds);
}
void RecordStore::reset(Rng& random){error.clear();file.reset(random);decode();}
bool RecordStore::open(const u8* bytes,u32 size){encode();if(!file.open(bytes,size))return fail(file.error.c_str());decode();error.clear();return true;}
bool RecordStore::save(std::vector<u8>& output){if(!error.empty())return false;encode();return file.save(output)||fail(file.error.c_str());}
bool RecordStore::import_blocks(const u8* blocks,const u8* settings){if(!blocks||!settings)return fail("Missing score blocks");std::memcpy(file.characters.data(),blocks,ScoreFile::character_size*5);std::memcpy(file.settings.data(),settings,ScoreFile::settings_size);decode();return true;}
SessionHighScore RecordStore::high_score(const SessionRecordQuery& query){
 if(!character_valid(query.character)||query.difficulty<0||query.difficulty>=5){fail("Invalid high score selection");return {};}
 if(query.scope==RecordScope::Spell){if(query.spell<0||query.spell>=119){fail("Invalid spell record");return {};}return {mode(query.character,query.legacy).spells[query.spell].score,0};}
 if(query.scope==RecordScope::Stage){if(!stage_valid(query.character,query.stage,query.difficulty)){fail("Invalid stage record");return {};}return {characters[query.character].practice[query.difficulty][query.stage].score,0};}
 const auto& row=mode(query.character,query.legacy).scores[query.difficulty][0];return {row.score,row.continues};
}
bool RecordStore::mark_stage(i32 character,i32 stage,i32 difficulty){if(!stage_valid(character,stage,difficulty))return fail("Invalid stage visit");characters[character].practice[difficulty][stage].visited=1;return true;}
bool RecordStore::count_play(i32 character,bool legacy){if(!character_valid(character))return fail("Invalid play record");count(mode(character,legacy).plays,9999999);return true;}
bool RecordStore::stage_clear(i32 character,i32 stage,i32 difficulty){if(!stage_valid(character,stage,difficulty))return fail("Invalid stage clear");characters[character].practice[difficulty][stage].cleared=1;return true;}
bool RecordStore::finished_run(i32 character,bool legacy,i32 difficulty,bool without_continue){if(!character_valid(character)||difficulty<0||difficulty>=5)return fail("Invalid run completion");auto& bank=mode(character,legacy);count(bank.clears[difficulty],99999);if(without_continue)count(bank.without_continue[difficulty],99999);return true;}
bool RecordStore::spell_score(i32 character,bool legacy,i32 spell,i32 score){if(!character_valid(character)||spell<0||spell>=119)return fail("Invalid spell score");auto& row=mode(character,legacy).spells[spell];if(score>row.score)row.score=score;return true;}
bool RecordStore::remove_checkpoint(i32 character,i32 difficulty){return checkpoint_remove?checkpoint_remove(character,difficulty):fail("Checkpoint storage unavailable");}
bool RecordStore::spell_begin(i32 character,bool legacy,i32 spell,bool practice,const std::string& label){
 if(!character_valid(character)||character==4||spell<0||spell>=119||label.size()>=64)return fail("Invalid spell history");auto& personal=mode(character,legacy).spells[spell];auto& all=characters[4].modes[0].spells[spell];
 std::copy(label.begin(),label.end(),personal.name.begin());personal.name[label.size()]=0;std::copy(label.begin(),label.end(),all.name.begin());all.name[label.size()]=0;
 count(personal.attempts[practice?1:0],99999);count(all.attempts[practice?1:0],99999);return true;
}
bool RecordStore::spell_capture(i32 character,bool legacy,i32 spell,bool practice){if(!character_valid(character)||character==4||spell<0||spell>=119)return fail("Invalid spell capture history");count(mode(character,legacy).spells[spell].captures[practice?1:0],99999);count(characters[4].modes[0].spells[spell].captures[practice?1:0],99999);return true;}
bool RecordStore::unlock_music(i32 track){if(track<0||track>=20)return fail("Invalid music record");music[track]=1;return true;}
bool RecordStore::begin_ending(i32 index,i32 difficulty,u32& first_seen_flags){if(index<0||index>=8)return fail("Invalid ending record");if(!endings[u32(index)])first_seen_flags|=1;if(!ending_seen)first_seen_flags|=2;endings[u32(index)]|=1;if(difficulty>=1&&difficulty<=3)endings[u32(index)]|=u8(1u<<difficulty);ending_seen=1;return true;}
bool RecordStore::unlock_all(){
 if(!error.empty())return false;encode();std::fill_n(file.settings.data()+0x16,16,u8(0x11));
 for(u32 variant=0;variant<2;variant++)for(u32 spell=0;spell<119;spell++){auto* bytes=file.characters[4].data();const u32 at=variant*0x5188+0x958+spell*0x9c;const i32 value=read<i32>(bytes,at);if(value<99999)write(bytes,at,wrapping_add(value,1));}
 for(u32 character=0;character<4;character++){auto* bytes=file.characters[character].data();for(u32 variant=0;variant<2;variant++){const u32 at=variant*0x5188+0x517c;write(bytes,at,wrapping_add(read<i32>(bytes,at),1));}
  // The original also marks the unused sixth practice row and the next
  // row's stage-zero byte. Preserve those file bytes rather than truncating.
  for(u32 rank=0;rank<6;rank++)for(u32 stage=1;stage<=8;stage++)bytes[0xa318+(rank*8+stage)*8+5]=1;
 }
 decode();return true;
}
bool RecordStore::add_play_time(i32 character,bool legacy,u64 duration){if(!character_valid(character))return fail("Invalid play-time character");total_play_centiseconds+=duration;mode(character,legacy).play_centiseconds+=duration;return true;}
// The original uses unsigned comparison and inserts equal scores ahead of the
// existing row. The whole displaced row, including its spare byte, moves down.
i32 RecordStore::score_rank(i32 character,bool legacy,i32 difficulty,i32 value)const noexcept{
 if(!character_valid(character)||difficulty<0||difficulty>=6)return -1;const auto& rows=characters[character].modes[legacy?1:0].scores[difficulty];i32 slot=0;while(slot<10&&u32(rows[slot].score)>u32(value))slot++;return slot==10?-1:slot;
}
i32 RecordStore::insert_score(i32 character,bool legacy,i32 difficulty,const RunScoreSubmission& value){
 if(!character_valid(character)||difficulty<0||difficulty>=6){fail("Invalid score registration");return -1;}
 auto& rows=mode(character,legacy).scores[difficulty];const i32 slot=score_rank(character,legacy,difficulty,value.score);if(slot<0)return -1;
 for(i32 n=9;n>slot;n--)rows[n]=rows[n-1];auto& row=rows[slot];row.score=value.score;row.stage=u8(value.stage);row.continues=i8(u8(value.continues));row.name.fill(' ');row.name[8]=0;
 row.details={value.timestamp[0],value.timestamp[1],signed_bits(float_to_bits(value.slowdown)),value.deaths};return slot;
}
bool RecordStore::practice_score(i32 character,i32 difficulty,i32 stage,i32 value){if(!stage_valid(character,stage,difficulty))return fail("Invalid practice score registration");auto& row=characters[character].practice[difficulty][stage];if(u32(row.score)<u32(value))row.score=value;return true;}
bool RecordStore::score_name(i32 character,bool legacy,i32 difficulty,i32 slot,const std::array<char,9>& value){if(!character_valid(character)||difficulty<0||difficulty>=6||slot<0||slot>=10||value[8])return fail("Invalid registered score name");auto& row=mode(character,legacy).scores[difficulty][slot];for(u32 n=0;n<9;n++){row.name[n]=name[n]=value[n];if(!value[n])break;}return true;}
u32 RecordStore::music_mask()const noexcept{u32 result=0;for(u32 n=0;n<20;n++)if(music[n])result|=1u<<n;return result;}
TitleRecords RecordStore::title_records()const noexcept{TitleRecords result;for(u32 character=0;character<4;character++){for(u32 mode=0;mode<2;mode++)result.clears[character][mode]=characters[character].modes[mode].without_continue;for(u32 difficulty=0;difficulty<5;difficulty++)for(u32 stage=0;stage<6;stage++)result.practice_stages[character][difficulty][stage]=characters[character].practice[difficulty][stage+1].visited!=0;}return result;}
}
