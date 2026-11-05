#include "ScoreStore.hpp"
#include "PlayTime.hpp"
#include <algorithm>
namespace th08 {
namespace {
template<class T>void append(std::vector<u8>& bytes,const T& value){const auto* data=reinterpret_cast<const u8*>(&value);bytes.insert(bytes.end(),data,data+sizeof(value));}
void chapter(ScoreChapter& h,u32 magic,u16 length,u8 version){h.magic=magic;h.length=h.copy_length=length;h.version=version;}
}
void ScoreStore::reset(){
    header={};file_header={};last_name_saved=false;
    session.last_name={};chapter(session.last_name.header,fourcc('L','S','N','M'),sizeof(LastName),1);std::memcpy(session.last_name.name,"        ",9);
    for(i32 difficulty=0;difficulty<5;difficulty++)for(i32 character=0;character<12;character++){
        auto& list=scores[difficulty][character];list.clear();
        for(i32 rank=0;rank<10;rank++){
            HighScore entry;std::memset(&entry,0,sizeof(entry));chapter(entry.header,fourcc('D','M','Y','S'),sizeof(entry),4);entry.score=100000-10000*rank;entry.difficulty=difficulty;entry.padding[0]=1;
            std::memcpy(entry.name,"--------",9);std::memcpy(entry.date,"--/--",6);list.push_back(entry);
        }
    }
}
i32 ScoreStore::link(const HighScore& score,i32 difficulty,i32 character){
    if(u32(difficulty)>=5||u32(character)>=12)return -1;
    auto& list=scores[difficulty][character];const auto position=std::find_if(list.begin(),list.end(),[&](const HighScore& current){return current.score<=score.score;});const i32 rank=position-list.begin();list.insert(position,score);return rank;
}
bool ScoreStore::load(const u8* bytes,u32 size,bool load_game_records){
    reset();ScoreFile file;const bool valid=file.decode(bytes,size);
    if(valid)std::memcpy(&header,file.decoded().data(),sizeof(header));
    for(i32 difficulty=0;difficulty<5;difficulty++)for(i32 character=0;character<12;character++){
        const auto stored=file.high_scores(character,difficulty);
        // GetHighScore traverses the file in order and LinkScore inserts equal
        // scores before existing entries. Preserve that order with owned values.
        for(auto entry=stored.rbegin();entry!=stored.rend();++entry)link(*entry,difficulty,character);
    }
    last_name_saved=file.copy_chapter(fourcc('L','S','N','M'),1,&session.last_name,sizeof(LastName),true);
    if(load_game_records){file.spells(session.records);file.clears(session.clears);file.practice(session.practices);}
    return valid;
}
std::vector<u8> ScoreStore::save(u32 now,ScoreSaveIdentity identity){
    std::vector<u8> bytes;bytes.reserve(350000);append(bytes,header);
    chapter(file_header,fourcc('T','H','8','K'),sizeof(file_header),1);append(bytes,file_header);
    for(i32 difficulty=0;difficulty<5;difficulty++)for(i32 character=0;character<12;character++){
        auto& list=scores[difficulty][character];for(u32 i=0;i<std::min<std::size_t>(10,list.size());i++){
            auto& entry=list[i];if(entry.header.magic!=fourcc('H','S','C','R'))continue;
            entry.character=character;entry.difficulty=difficulty;chapter(entry.header,entry.header.magic,sizeof(entry),4);entry.header.unknown=0;append(bytes,entry);
        }
    }
    for(auto& entry:session.clears){chapter(entry.header,fourcc('C','L','R','D'),sizeof(entry),4);append(bytes,entry);}
    for(u32 i=0;i<spell_count;i++){auto& entry=session.records[i];if(entry.header.magic!=fourcc('C','A','T','K'))continue;entry.number=i;chapter(entry.header,entry.header.magic,sizeof(entry),3);append(bytes,entry);}
    for(const auto& entry:session.practices)if(entry.used)append(bytes,entry);
    append(bytes,session.last_name);chapter(session.last_words.header,fourcc('F','L','S','P'),sizeof(LastWords),1);append(bytes,session.last_words);
    // UpdatePlayTime discards a backwards clock interval; UpdateGameTime uses
    // a different rollover rule, so choose the origin before sharing its carries.
    update_time(now);append(bytes,session.statistics);
    VersionRecord version;chapter(version.header,fourcc('V','R','S','M'),sizeof(version),1);std::memcpy(version.version,"0100d",6);version.exe_size=identity.size;version.exe_checksum=identity.checksum;append(bytes,version);
    ScoreHeader output=header;output.version=1;output.header_size=sizeof(output);output.total_size=bytes.size();output.payload_size=bytes.size()-sizeof(output);std::memcpy(bytes.data(),&output,sizeof(output));
    ScoreFile file;if(!file.assign_decoded(bytes.data(),bytes.size()))return {};return file.encode(session.random);
}
void ScoreStore::update_time(u32 now){if(now<previous_clock)previous_clock=now;accumulate_play_time(session.statistics.total_time,previous_clock,now);}
}
