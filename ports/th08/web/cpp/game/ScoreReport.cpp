#include "ScoreReport.hpp"
#include "ScoreReportText.hpp"
#include "ResultText.hpp"
#include <cstdio>
#include <algorithm>
namespace th08 {
namespace {
template<class... Args>void add(std::string& out,const char* format,Args... args){const i32 size=std::snprintf(nullptr,0,format,args...);if(size<=0)return;const auto offset=out.size();out.resize(offset+size+1);std::snprintf(out.data()+offset,size+1,format,args...);out.resize(offset+size);}
std::string padded_name(const char* name){std::string value(name);if(value.size()<48)value.resize(48,' ');return value;}
}
std::string score_report(GameplaySession& session,ScoreStore& scores,const char* timestamp,u32 now){
    namespace t=score_report_text;std::string out;
    out+=t::header;out+=t::description;out+=t::read_only;out+=t::permission;add(out,t::timestamp,timestamp);out+=t::blank2;add(out,t::version,t::version_number);out+=t::blank;
    for(i32 difficulty=0;difficulty<5;difficulty++){
        bool heading=false;for(i32 character=0;character<12;character++){
            const auto& entries=scores.list(difficulty,character);std::string body;
            for(u32 rank=0;rank<std::min<std::size_t>(10,entries.size());rank++){
                const auto& h=entries[rank];if(h.header.magic!=fourcc('H','S','C','R'))continue;
                add(body,t::rank,rank+1);add(body,t::score,h.score,i32(h.retries));add(body,t::name,h.name);
                if(h.stage==99)body+=t::all_clear;else add(body,t::stage,t::stages[h.stage<10?h.stage:0]);
                add(body,t::date,h.date);add(body,t::lag,double(h.lag));add(body,t::duration,h.play_frames/3600,h.play_frames/60%60);add(body,t::lives,i32(h.config.lives)+1);
                add(body,t::points,h.points);add(body,t::time_orbs,h.time_orbs);add(body,t::deaths,h.deaths);add(body,t::bombs,h.bombs);add(body,t::last_spells,h.last_spells);add(body,t::pauses,h.pauses);add(body,t::retries,i32(h.retries));
                add(body,t::humanity,(Extended::from_int(h.humanity)/number(100)).to_double());body+=t::spells;
                for(i32 spell=0;spell<222;spell++)if(h.spell_counters[spell]){const auto& record=session.records[spell];const auto name=padded_name(record.header.magic==fourcc('C','A','T','K')?record.name:result_text::unknown_spell);add(body,t::spell,spell+1,name.c_str(),record.game.captures[12],record.game.attempts[12]);}
            }
            if(body.empty())continue;if(!heading){out+=t::section;add(out,t::difficulty,t::difficulties[difficulty]);heading=true;}
            out+=t::separator;add(out,t::character,result_text::characters[character]);add(out,t::body,body.c_str());
        }
    }
    out+=t::section;out+=t::spell_list;
    for(i32 index=0;index<222;index++){const auto& r=session.records[index];if(r.header.magic!=fourcc('C','A','T','K')||(!r.game.attempts[12]&&!r.practice.attempts[12]))continue;const auto name=padded_name(r.name);add(out,t::spell_history,index+1,name.c_str(),r.game.captures[12],r.game.attempts[12],r.practice.captures[12],r.practice.attempts[12],t::difficulties[r.difficulty<5?r.difficulty:0]);}
    scores.update_time(now);out+=t::section;const auto& play=session.statistics;
    add(out,t::total_time,play.total_time[0],play.total_time[1],play.total_time[2]);add(out,t::game_time,play.game_time[0],play.game_time[1],play.game_time[2]);out+=t::play_heading;
    for(i32 character=0;character<13;character++){u32 values[6];for(i32 d=0;d<6;d++){const auto& counts=play.counts[d==5?6:d];values[d]=character<12?counts.characters[character]:counts.total;}add(out,t::play_counts,result_text::characters[character],values[0],values[1],values[2],values[3],values[4],values[5]);}
    add(out,t::clears,play.counts[0].clears,play.counts[1].clears,play.counts[2].clears,play.counts[3].clears,play.counts[4].clears,play.counts[6].clears);
    add(out,t::continues,play.counts[0].continues,play.counts[1].continues,play.counts[2].continues,play.counts[3].continues,play.counts[4].continues,play.counts[6].continues);
    add(out,t::practices,play.counts[0].practices,play.counts[1].practices,play.counts[2].practices,play.counts[3].practices,play.counts[4].practices,play.counts[6].practices);
    return out;
}
}
