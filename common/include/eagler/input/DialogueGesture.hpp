#pragma once
#include <cmath>
#include <cstdint>
#include <deque>
#include <map>
#include <cstddef>

namespace eagler::input {
// Dialogue recognition runs alongside movement, independently for each finger.
// Once a finger drags, returning to its origin cannot turn it back into a tap.
class DialogueGesture {
    struct Point {
        float x, y, last_x, last_y;
        std::uint64_t start;
        bool moved=false, navigated=false;
    };
    std::map<std::int64_t, Point> points;
    std::deque<int> directions;
    void choose(Point& p){
        if(p.navigated)return;
        const float dx=p.last_x-p.x,dy=p.last_y-p.y;
        if(std::abs(dx)>24||std::abs(dy)>24){p.navigated=true;
            directions.push_back(std::abs(dx)>std::abs(dy)?(dx>0?39:37):(dy>0?40:38));
        }
    }
public:
#if defined(TH_PRESENTATION_AUDIT)
    void audit_state(std::uint32_t& hash)const{
        const auto add=[&](const auto& value){const auto* bytes=reinterpret_cast<const std::uint8_t*>(&value);for(std::size_t i=0;i<sizeof(value);++i)hash=(hash^bytes[i])*16777619u;};
        add(points.size());for(const auto& [id,p]:points){add(id);add(p.x);add(p.y);add(p.last_x);add(p.last_y);add(p.start);add(p.moved);add(p.navigated);}
        add(directions.size());for(const auto key:directions)add(key);
    }
#endif
    void clear(){points.clear();directions.clear();}
    void erase(std::int64_t id){points.erase(id);}
    void begin(std::int64_t id,float x,float y,std::uint64_t now){points[id]={x,y,x,y,now};}
    void move(std::int64_t id,float x,float y,bool choice=false){
        auto it=points.find(id);if(it==points.end())return;
        auto& p=it->second;p.last_x=x;p.last_y=y;
        const float dx=x-p.x,dy=y-p.y;if(dx*dx+dy*dy>64)p.moved=true;
        if(choice)choose(p);
    }
    bool end(std::int64_t id,float x,float y,std::uint64_t now,bool choice=false){
        move(id,x,y,choice);auto it=points.find(id);if(it==points.end())return false;
        const bool tap=!it->second.moved&&now-it->second.start<500;
        points.erase(it);return tap;
    }
    bool skip(std::uint64_t now)const{
        for(const auto& entry:points)if(!entry.second.moved&&now-entry.second.start>=500)return true;
        return false;
    }
    // A choice follows menu convention: one direction per swipe, then lift.
    // The adapter enables this only while the original script awaits a choice.
    int navigate(){
        for(auto& entry:points)choose(entry.second);
        if(directions.empty())return 0;
        const int key=directions.front();directions.pop_front();return key;
    }
};
}
