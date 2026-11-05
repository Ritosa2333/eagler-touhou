#pragma once
#include <array>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <map>

namespace touhou::input {
// A sparse stream sampled at the player's movement callback, after loading.
// Each entry overrides one movement with its pre-timescale velocity. Ordinary
// keyboard frames and the original replay byte stream remain unchanged.
class MotionTrack {
public:
    struct Entry {std::uint32_t tick;float x,y;};
    struct TouchEntry {std::uint32_t tick;std::int32_t id;float x,y;std::uint32_t action;};
    struct TouchPoint {std::int32_t id;float x,y;bool released=false;};
    static_assert(sizeof(TouchEntry)==20);
    static constexpr std::uint32_t limit=600000;
    static constexpr float velocity_limit=10000000;
    std::array<std::vector<Entry>,9> stages;
    std::array<std::vector<TouchEntry>,9> touch_stages;
    std::array<std::uint32_t,9> ticks{},cursors{};
    std::array<std::uint32_t,9> touch_cursors{};
    std::map<std::int32_t,TouchPoint> record_points;
    std::map<std::int32_t,TouchPoint> playback_points;
    int playback_stage=-1;
    bool playing=false,recording=false,invalid=false,active=false,unlimited=false,cheat_movement_used=false;
    float target_x=0,target_y=0;
    void clear(){for(auto& s:stages)s.clear();for(auto& s:touch_stages)s.clear();ticks={};cursors={};touch_cursors={};record_points.clear();playback_points.clear();playback_stage=-1;playing=recording=invalid=active=unlimited=false;cheat_movement_used=false;target_x=target_y=0;}
    void begin(int stage,bool initial,bool replay,bool record){
        if(initial&&!replay)clear();playing=replay;recording=record;active=false;
        if(stage>=0&&stage<9){ticks[stage]=cursors[stage]=touch_cursors[stage]=0;if(!replay){stages[stage].clear();touch_stages[stage].clear();if(record)for(const auto& [id,point]:record_points)touch_stages[stage].push_back({0,id,point.x,point.y,0});}else{if(playback_stage<0||stage<=playback_stage)playback_points.clear();playback_stage=stage;}}
    }
    bool used()const{for(const auto& s:stages)if(!s.empty())return true;for(const auto& s:touch_stages)if(!s.empty())return true;return false;}
    void mark_cheat_movement(float x,float y){if(unlimited&&(x!=0||y!=0))cheat_movement_used=true;}
    void target(int mode,float x,float y){active=(mode==1||mode==2)&&std::isfinite(x)&&std::isfinite(y);unlimited=mode==2;if(active){target_x=x;target_y=y;}}
    void touch_event(int stage,int action,int id,float x,float y){
        if(!recording||stage<0||stage>=9||action<0||action>2||!std::isfinite(x)||!std::isfinite(y))return;
        auto found=record_points.find(id);if(action!=0&&found==record_points.end())return;
        auto& events=touch_stages[stage];if(events.size()>=limit){invalid=true;return;}events.push_back({ticks[stage],id,x,y,std::uint32_t(action)});
        if(action==0)record_points[id]={id,x,y,false};else if(action==1){found->second.x=x;found->second.y=y;}else record_points.erase(found);
    }
    void touch_cancel(int stage){
        if(!recording||stage<0||stage>=9||record_points.empty())return;auto& events=touch_stages[stage];if(events.size()>=limit){invalid=true;return;}events.push_back({ticks[stage],0,0,0,3});record_points.clear();
    }
    int replay_points(TouchPoint* points,int capacity)const{
        if(!points||capacity<=0)return 0;int count=0;for(const auto& [id,point]:playback_points){if(count>=capacity)break;points[count++]=point;}return count;
    }
    void apply_touch_events(int stage,std::uint32_t tick){
        auto& cursor=touch_cursors[stage];const auto& events=touch_stages[stage];
        for(auto it=playback_points.begin();it!=playback_points.end();)if(it->second.released)it=playback_points.erase(it);else ++it;
        while(cursor<events.size()&&events[cursor].tick<=tick){const auto& event=events[cursor++];
            if(event.action==3){playback_points.clear();continue;}
            auto found=playback_points.find(event.id);
            if(event.action==0){playback_points[event.id]={event.id,event.x,event.y,false};continue;}
            if(found==playback_points.end())continue;
            found->second.x=event.x;found->second.y=event.y;if(event.action==2)found->second.released=true;
        }
    }
    bool playback(int stage,float& x,float& y){
        if(stage<0||stage>=9)return false;auto& next=cursors[stage];const auto tick=ticks[stage]++;apply_touch_events(stage,tick);const auto& s=stages[stage];
        while(next<s.size()&&s[next].tick<tick)next++;
        if(next==s.size()||s[next].tick!=tick)return false;x=s[next].x;y=s[next].y;next++;return true;
    }
    void record(int stage,bool enabled,float x,float y){
        if(stage<0||stage>=9)return;const auto tick=ticks[stage]++;if(!recording||!enabled)return;
        if(stages[stage].size()>=limit||!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>velocity_limit||std::abs(y)>velocity_limit){invalid=true;return;}stages[stage].push_back({tick,x,y});
    }
    static std::uint32_t hash(const std::uint8_t* bytes,std::size_t size){std::uint32_t n=2166136261u;for(std::size_t i=0;i<size;i++)n=(n^bytes[i])*16777619u;return n;}
    static std::uint32_t word(const std::uint8_t* p){std::uint32_t n;std::memcpy(&n,p,4);return n;}
    bool load(const std::uint8_t* data,std::size_t size,std::uint32_t game){
        clear();if(size<24||std::memcmp(data+size-24,"THMOTION",8))return true;
        const auto* footer=data+size-24;const auto version=word(footer+12),length=word(footer+16);
        if(word(footer+8)!=game||(version!=1&&version!=2)||length>size-24||length<(version==1?36u:72u)||length>(version==1?36u+limit*12u:72u+limit*32u))return false;
        const auto* payload=footer-length;if(hash(payload,length)!=word(footer+20))return false;
        std::size_t offset=36;std::uint32_t total=0;std::array<std::vector<Entry>,9> parsed;
        for(int stage=0;stage<9;stage++){
            const auto count=word(payload+stage*4);if(count>limit-total||offset>length||count>(length-offset)/12)return false;total+=count;
            auto& s=parsed[stage];s.resize(count);if(count)std::memcpy(s.data(),payload+offset,count*12);offset+=count*12;
            for(std::uint32_t i=0;i<count;i++)if((i&&s[i].tick<=s[i-1].tick)||!std::isfinite(s[i].x)||!std::isfinite(s[i].y)||std::abs(s[i].x)>velocity_limit||std::abs(s[i].y)>velocity_limit)return false;
        }
        std::array<std::vector<TouchEntry>,9> parsed_touch;
        if(version==2){
            if(offset>length||length-offset<36)return false;const auto* counts=payload+offset;offset+=36;std::uint32_t touch_total=0;
            for(int stage=0;stage<9;stage++){
                const auto count=word(counts+stage*4);if(count>limit-touch_total||offset>length||count>(length-offset)/20)return false;touch_total+=count;
                auto& events=parsed_touch[stage];events.resize(count);if(count)std::memcpy(events.data(),payload+offset,count*20);offset+=count*20;
                for(std::uint32_t i=0;i<count;i++)if((i&&events[i].tick<events[i-1].tick)||events[i].action>3||(events[i].action!=3&&(!std::isfinite(events[i].x)||!std::isfinite(events[i].y))))return false;
            }
        }
        if(offset!=length)return false;stages=std::move(parsed);touch_stages=std::move(parsed_touch);return true;
    }
    std::vector<std::uint8_t> trailer(std::uint32_t game)const{
        std::uint32_t total=0,touch_total=0;for(const auto& s:stages)total+=s.size();for(const auto& s:touch_stages)touch_total+=s.size();if((!total&&!touch_total)||invalid||total>limit||touch_total>limit)return {};
        const std::uint32_t version=touch_total?2:1,length=36+total*12+(touch_total?36+touch_total*20:0);std::vector<std::uint8_t> bytes(length+24);std::size_t offset=36;
        for(int stage=0;stage<9;stage++){const std::uint32_t n=stages[stage].size();std::memcpy(bytes.data()+stage*4,&n,4);if(n)std::memcpy(bytes.data()+offset,stages[stage].data(),n*12);offset+=n*12;}
        if(touch_total){const auto counts=offset;offset+=36;for(int stage=0;stage<9;stage++){const std::uint32_t n=touch_stages[stage].size();std::memcpy(bytes.data()+counts+stage*4,&n,4);if(n)std::memcpy(bytes.data()+offset,touch_stages[stage].data(),n*20);offset+=n*20;}}
        const auto checksum=hash(bytes.data(),length);const std::uint32_t words[]{game,version,length,checksum};std::memcpy(bytes.data()+length,"THMOTION",8);std::memcpy(bytes.data()+length+8,words,16);return bytes;
    }
};
inline void limit_vector(float& x,float& y,float speed){const float square=x*x+y*y;if(square>speed*speed&&square>0){const float scale=speed/std::sqrt(square);x*=scale;y*=scale;}}
}
