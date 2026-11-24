#pragma once
#include <algorithm>
#include <optional>

namespace soew {
inline constexpr int practice_shadow_extent=9; // shot_box draws through right/bottom + 8, inclusive.
struct PracticePoint { int x,y; };
struct PracticeBounds {
    int left,top,right,bottom;
    bool contains(PracticePoint p)const{
        return p.x>=left&&p.x<right&&p.y>=top&&p.y<bottom;
    }
};
struct PracticeWindow {
    PracticePoint offset{0,0};
    PracticeBounds bounds(PracticeBounds base)const{
        return {base.left+offset.x,base.top+offset.y,base.right+offset.x,base.bottom+offset.y};
    }
    PracticePoint local(PracticePoint p)const{return {p.x-offset.x,p.y-offset.y};}
    void move(PracticeBounds base,PracticePoint to,int shadow=0){
        offset={std::clamp(to.x,-base.left,640-base.right-shadow),
                std::clamp(to.y,-base.top,400-base.bottom-shadow)};
    }
};
struct PracticeDrag {
    PracticeWindow *window=nullptr;
    PracticePoint anchor{0,0};
    PracticeBounds base{};
    int shadow=0;
    void begin(PracticeWindow &target,PracticeBounds bounds,PracticePoint p,int padding=0){
        window=&target;anchor=target.local(p);base=bounds;shadow=padding;
    }
    void move(PracticePoint p)const{window->move(base,{p.x-anchor.x,p.y-anchor.y},shadow);}
    void end(){window=nullptr;}
};

inline std::optional<PracticePoint> practice_pointer(PracticeBounds view,PracticePoint client,bool captured){
    const int w=view.right-view.left,h=view.bottom-view.top;
    if(!w||!h||(!captured&&!view.contains(client)))return std::nullopt;
    return PracticePoint{int(640LL*(client.x-view.left)/w),int(400LL*(client.y-view.top)/h)};
}
}
