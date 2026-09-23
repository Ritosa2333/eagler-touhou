#include "SessionRecords.hpp"
#include <algorithm>
#include <cstring>
#include <new>

namespace th10::multiplayer {
namespace {
void copy_values(ScoreData& to,const ScoreData& from){
    std::memcpy(to.characters,from.characters,sizeof(to.characters));
    to.settings=from.settings;
}
i32 read_count(const u8* bytes){i32 result;std::memcpy(&result,bytes,4);return result;}
void merge_count(i32& into,i32 before,i32 after,i32 maximum){
    const auto delta=std::max<std::int64_t>(0,std::int64_t(after)-before);
    into=i32(std::clamp<std::int64_t>(std::int64_t(into)+delta,0,maximum));
}
void merge_count(u8* into,const u8* before,const u8* after,i32 maximum){
    auto count=read_count(into);merge_count(count,read_count(before),read_count(after),maximum);
    std::memcpy(into,&count,4);
}
bool same_score(const HighScore& a,const HighScore& b){
    return a.score==b.score&&a.score_units==b.score_units&&a.stage==b.stage&&
        a.timestamp==b.timestamp&&a.slow_rate==b.slow_rate&&
        !std::memcmp(a.name,b.name,sizeof(a.name));
}
void merge_scores(HighScore (&into)[10],const HighScore (&before)[10],
                  const HighScore (&after)[10],i32 timestamp){
    bool consumed[10]{};
    for(const auto& candidate:after){
        bool existing=false;
        for(unsigned i=0;i<10;++i)if(!consumed[i]&&same_score(candidate,before[i])){
            consumed[i]=true;existing=true;break;
        }
        if(existing)continue;
        unsigned at=0;
        while(at<10&&(into[at].score>candidate.score||
              (into[at].score==candidate.score&&into[at].score_units>candidate.score_units)))++at;
        if(at==10)continue;
        for(unsigned i=9;i>at;--i)into[i]=into[i-1];
        into[at]=candidate;
        // Wall-clock dates decorate the committed local file only.
        if(!into[at].timestamp)into[at].timestamp=timestamp;
    }
}
}
bool SessionRecords::Begin(ScoreData& active,const std::int8_t* difficulties,bool readOnly){
    if(Active()||!difficulties)return false;
    std::unique_ptr<ScoreData> persistent(new(std::nothrow) ScoreData{});
    std::unique_ptr<ScoreData> checkpoint(new(std::nothrow) ScoreData{});
    if(!persistent||!checkpoint)return false;
    copy_values(*persistent,active);
    // These two codec buffers stay owned by Scores::memory. Neither snapshot
    // allocates/frees them or changes their addresses during simulation.
    persistent->file=active.file;persistent->unpacked=active.unpacked;
    for(auto& record:active.characters)record.initialize(difficulties);
    Rng metadataRandom{};
    active.settings.initialize(metadataRandom); // never the gameplay RNG
    copy_values(*checkpoint,active);
    persistent_=std::move(persistent);checkpoint_=std::move(checkpoint);
    read_only_=readOnly;return true;
}
bool SessionRecords::Checkpoint(const ScoreData& active,i32 timestamp){
    if(!Active())return false;
    if(read_only_)return true;
    for(unsigned character=0;character<7;++character){
        auto& into=persistent_->characters[character];
        const auto& before=checkpoint_->characters[character];
        const auto& after=active.characters[character];
        for(unsigned difficulty=0;difficulty<5;++difficulty)
            merge_scores(into.high_scores[difficulty],before.high_scores[difficulty],
                         after.high_scores[difficulty],timestamp);
        // Native statistics: play count, elapsed ticks, five clear counts.
        for(unsigned offset=0;offset<0x1c;offset+=4)
            merge_count(into.statistics+offset,before.statistics+offset,
                        after.statistics+offset,offset==4?215999999:99999);
        // The following 24 eight-byte practice slots hold high score and
        // unlock flags, not additive counters. Retain unrelated reserved bytes.
        for(unsigned offset=0x1c;offset<sizeof(into.statistics);offset+=8){
            if(read_count(after.statistics+offset)>read_count(into.statistics+offset))
                std::memcpy(into.statistics+offset,after.statistics+offset,4);
            into.statistics[offset+4]|=after.statistics[offset+4];
            into.statistics[offset+5]|=after.statistics[offset+5];
        }
        for(unsigned id=0;id<110;++id){
            auto& target=into.spells[id];const auto& old=before.spells[id];const auto& now=after.spells[id];
            merge_count(target.attempts,old.attempts,now.attempts,99999);
            merge_count(target.captures,old.captures,now.captures,99999);
            if(now.attempts>old.attempts&&now.name[0])std::memcpy(target.name,now.name,sizeof(target.name));
        }
    }
    for(unsigned i=0;i<sizeof(active.settings.statistics);++i)
        persistent_->settings.statistics[i]|=active.settings.statistics[i];
    if(std::memcmp(active.settings.last_name,checkpoint_->settings.last_name,9))
        std::memcpy(persistent_->settings.last_name,active.settings.last_name,9);
    copy_values(*checkpoint_,active);return true;
}
void SessionRecords::Finish(ScoreData& active){
    if(!Active())return;
    copy_values(active,*persistent_);persistent_.reset();checkpoint_.reset();read_only_=false;
}
}
