#include "Scene.hpp"
#ifdef TH12_MULTIPLAYER
#include "RuntimeOptions.hpp"
#include "game/GameState.hpp"
#include "game/Multiplayer.hpp"
namespace th12host {
void Scene::capture_multiplayer_hud(int frame){
    using namespace th12::multiplayer;
    const auto text=[&](std::string value,float x,float y,int font=2,int align=1,u32 color=0xffffffff,float scale=1,int priority=69,int clip=0){
        TextDescriptor t;t.text=std::move(value);t.x=x;t.y=y;t.font=font;t.align_x=align;t.color=color;t.scale_x=t.scale_y=scale;
        Pose p;p.kind=Pose::NativeText;p.item=u32(texts.size());texts.push_back(std::move(t));p.has_priority=true;p.priority=priority;p.clip=clip;p.has_order=true;p.order=int(poses.size());poses.push_back(p);
    };
    const auto piece=[](int i,int whole,int fragments){return i<whole?2:i==whole?7+std::clamp(fragments,0,5):3;};
    text(grouped_number(hud[3]),620,72,3,2);text(grouped_number(hud[40]),620,48,3,2);
    for(unsigned seat=0;seat<options.count;++seat){
        const auto& p=pilots[seat];const auto& r=p.resources;const float y=104+seat*80.f,opacity=p.ghost?.55f:1.f;
        text(std::to_string(seat+1)+"P"+(p.ghost?" GHOST":""),428,y-14,1,1,int(seat)==local_seat?0xffffff00:0xffffffff);
        SampleOptions label;label.entity=0x10000000+seat*32;
        add(BankFront,8,frame,0,y-102,0,1,std::nullopt,label,opacity,59,0);
        add(BankFront,10,frame,0,y+24-126,0,1,std::nullopt,label,opacity,59,0);
        add(BankFront,9,frame,0,y+48-150,0,1,std::nullopt,label,opacity,59,0);
        for(int i=0;i<8;++i){SampleOptions s;s.entity=0x11000000+seat*32+i;s.interrupt=piece(i,std::max(0,r.lives),r.lifeFragments);add(BankFront,13+i,frame,0,y-102,0,1,std::nullopt,s,opacity,59,0);
            s.entity+=8;s.interrupt=piece(i,r.bombs,r.bombFragments);add(BankFront,21+i,frame,0,y+24-126,0,1,std::nullopt,s,opacity,59,0);}
        const auto fraction=[](int n){const auto s=std::to_string(n%100);return s.size()<2?"0"+s:s;};
        text(std::to_string(r.power/100)+".",540,y+50,4,1,0xffff8030);text(fraction(r.power),560,y+57,4,1,0xffff8030,.6f);
        text("/"+std::to_string(r.maxPower/100)+".",574,y+50,4,1,0xffff8030);text(fraction(r.maxPower),606,y+57,4,1,0xffff8030,.6f);
        const auto taps=p.lifeTaps>=3?"L "+std::to_string(p.lifeTaps)+"/5":p.powerTaps>=3?"P "+std::to_string(p.powerTaps)+"/5":"";
        if(!taps.empty())text(taps,p.px/128.f+224,p.py/128.f,1,0,0xffffffff,1,45,2);
        if(int(seat)==local_seat&&!p.ghost){
            if(runtime_options.always_hitbox&&!std::binary_search(owners.begin(),owners.end(),p.owner()+32)){SampleOptions marker;marker.entity=p.owner()+32;add(BankBullet,77,std::max(18,frame),p.px/128.f+224,p.py/128.f+16,0,1,std::nullopt,marker,1,34,1);}
            if(local_visibility)for(const auto& b:std::initializer_list<std::array<float,4>>{{-7,-7,-3,-6},{3,-7,7,-6},{-7,6,-3,7},{3,6,7,7}}){Pose q;q.kind=Pose::Rectangle;for(int j=0;j<4;++j)q.bounds[j]=b[j]+(j%2?p.py/128.f+16:p.px/128.f+224);q.argb=0xc0ffffff;q.has_priority=true;q.priority=24;q.clip=1;poses.push_back(q);}
        }
    }
    const auto& local=pilots[local_seat>=0&&local_seat<int(options.count)?local_seat:0];const float y=104+options.count*80.f;
    add(BankFront,12,frame,0,y-174,0,1,std::nullopt,{},1,59,0);text(grouped_number(local.resources.pointValue()/10*10),620,y+2,3,2);
    add(BankFront,11,frame,0,y+24-198,0,1,std::nullopt,{},1,59,0);text(grouped_number(local.graze),620,y+26,3,2);
}
void Scene::apply_multiplayer_opacity(){
    using namespace th12::multiplayer;
    const Pilot* local=local_seat>=0&&local_seat<int(options.count)?&pilots[local_seat]:nullptr;
    visual_owners.assign(th12::multiplayerVisualOwners.begin(),th12::multiplayerVisualOwners.end());
    std::sort(visual_owners.begin(),visual_owners.end());
    for(auto& pose:poses){
        unsigned seat=MaxPlayers;
        if(pose.owner>>16==0xffff)seat=(pose.owner>>8)&255;
        else {const auto owner=std::lower_bound(visual_owners.begin(),visual_owners.end(),std::array<u32,2>{pose.owner,0});if(owner!=visual_owners.end()&&(*owner)[0]==pose.owner)seat=(*owner)[1];}
        if(seat>=options.count)continue;const auto& p=pilots[seat];
        const auto dx=local?p.px-local->px:0,dy=local?p.py-local->py:0;
        const float opacity=p.ghost?(&p==local?112:88)/255.f:&p==local?1:local&&double(dx)*dx+double(dy)*dy<48.*48*128*128?72/255.f:160/255.f;
        if(pose.kind==Pose::BeamTrail||pose.kind==Pose::Trail){const auto fade=[&](u32 c){return (c&0xffffff)|(u32(std::lround((c>>24)*opacity))<<24);};pose.start=fade(pose.start);pose.end=fade(pose.end);}
        else pose.alpha*=opacity;
    }
}
}
#endif
