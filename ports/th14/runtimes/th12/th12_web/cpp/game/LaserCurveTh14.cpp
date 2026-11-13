#include "LaserCurveTh14.hpp"
#include "Movement.hpp"
#include "Timer.hpp"
#include <cmath>
#include <cstring>
namespace th11 {
#if PORT_TITLE == 14
namespace {
float wrap(float a)noexcept{
    constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f;
    u32 loops=0;
    while(a>pi){a-=tau;if(loops++>32)break;}
    while(a<-pi){a+=tau;if(loops++>32)break;}
    return a;
}
Vec3 vector(float angle,float speed)noexcept{
    const Vec2 v=polar(angle,speed);return {v.x,v.y,0};
}
float read_float(const u8* p)noexcept{float f;std::memcpy(&f,p,4);return f;}
i32 read_int(const u8* p)noexcept{i32 i;std::memcpy(&i,p,4);return i;}
void velocity_outputs(Vec3 v,Th14CurveSample& out)noexcept{
    const float square=v.x*v.x+v.y*v.y;
    out.speed=float(std::sqrt(double(square)));
    out.angle=float(std::atan2(double(v.y),double(v.x)));
}
}
bool th14_curve_head(const Th14CurveNode& n,float time,Th14CurveSample& out)noexcept{
    const float t=time-n.start;
    if(n.mode==0){
        out.position={n.position.x+(n.direction.x*t)*n.speed,n.position.y+(n.direction.y*t)*n.speed,n.position.z+(n.direction.z*t)*n.speed};
        out.angle=n.angle;out.speed=n.speed;return true;
    }
    if(n.mode==1){
        if(n.turn<-990.f){
            const float coefficient=n.acceleration*t+n.speed*2.f,age=t+1.f;
            out.position={n.position.x+((n.direction.x*coefficient)*age)*.5f,n.position.y+((n.direction.y*coefficient)*age)*.5f,n.position.z+((n.direction.z*coefficient)*age)*.5f};
            out.speed=n.acceleration*t+n.speed;out.angle=n.angle;
        }else{
            const Vec3 initial=vector(n.angle,n.speed),acceleration=vector(n.turn,n.acceleration);
            const Vec3 v{acceleration.x+initial.x,acceleration.y+initial.y,acceleration.z+initial.z};
            out.position={n.position.x+v.x*t,n.position.y+v.y*t,n.position.z+v.z*t};
            velocity_outputs(v,out);
        }
        return true;
    }
    if(n.mode==2){
        Vec3 p=n.position;float angle=n.angle,speed=n.speed;
        const i32 steps=t>=2147483648.f||t<-2147483648.f||!std::isfinite(t)?INT32_MIN:i32(t);
        for(i32 j=0;j<steps;++j){
            const Vec3 v=vector(angle,speed);
            p={p.x+v.x,p.y+v.y,p.z+v.z};angle=wrap(angle+n.turn);speed+=n.acceleration;
        }
        const Vec3 v=vector(angle,speed);const float fraction=t-float(std::floor(double(t)));
        out.position={p.x+fraction*v.x,p.y+fraction*v.y,p.z+fraction*v.z};out.angle=angle;out.speed=speed;
        return true;
    }
    return false;
}
bool th14_curve_tail(const Th14CurveNode& n,float time,const Th14CurveSample& previous,Th14CurveSample& out)noexcept{
    if(n.mode==0){
        out.position={previous.position.x-n.direction.x*n.speed,previous.position.y-n.direction.y*n.speed,previous.position.z-n.direction.z*n.speed};
        out.angle=n.angle;out.speed=n.speed;return true;
    }
    if(n.mode==1){
        if(n.turn<-990.f){
            const float speed=previous.speed-n.acceleration;
            out.position={previous.position.x-n.direction.x*speed,previous.position.y-n.direction.y*speed,previous.position.z-n.direction.z*speed};
            // Retail uses the node's starting speed for this output, not the previous sample's speed.
            out.speed=n.speed-n.acceleration;out.angle=previous.angle;
        }else{
            const Vec3 initial=vector(previous.angle,-previous.speed),acceleration=vector(n.turn,-n.acceleration);
            const Vec3 v{acceleration.x+initial.x,acceleration.y+initial.y,acceleration.z+initial.z};
            out.position={previous.position.x+v.x,previous.position.y+v.y,previous.position.z+v.z};velocity_outputs(v,out);
        }
        return true;
    }
    if(n.mode==2){
        const float whole=float(std::floor(double(time))),fraction=time-whole;
        const Vec3 current=vector(previous.angle,previous.speed);
        const Vec3 p{previous.position.x-fraction*current.x,previous.position.y-fraction*current.y,previous.position.z-fraction*current.z};
        out.speed=previous.speed-n.acceleration;out.angle=wrap(previous.angle-n.turn);
        const Vec3 v=vector(out.angle,out.speed);const float remaining=(1.f-time)+whole;
        out.position={p.x-remaining*v.x,p.y-remaining*v.y,p.z-remaining*v.z};return true;
    }
    return false;
}
bool Th14CurvePath::initialize(const u8* bytes,const std::vector<Th14CurveNode>* inherited){
    if(!bytes||read_int(bytes+0x20)<0||(read_int(bytes+0x34c)!=0&&!inherited)||inherited==&nodes||(inherited&&inherited->empty()))return false;
    std::memcpy(request,bytes,sizeof(request));transform_index=0;nodes.clear();samples.clear();
    Vec3 position{read_float(request),read_float(request+4),read_float(request+8)};
    const float angle=read_float(request+0xc),speed=read_float(request+0x14),offset=read_float(request+0x24);
    if(offset!=0){const Vec3 v=vector(angle,offset);position={position.x+v.x,position.y+v.y,position.z+v.z};std::memcpy(request,&position,12);std::memset(request+0x24,0,4);}
    Th14CurveNode root;root.angle=wrap(angle);root.speed=speed;root.direction=vector(angle,1);root.position=position;
    if(inherited){nodes=*inherited;transform_index=99;std::memset(request+0x34c,0,4);}else nodes.push_back(root);
    Th14CurveSample initial;initial.position=position;initial.angle=angle;initial.speed=speed;
    samples.resize(u32(read_int(request+0x20)),initial);
    if(!samples.empty())samples[0].auxiliary=vector(angle,speed);
    sample(read_float(request+0x350),false);return true;
}
bool Th14CurvePath::transforms(){
    for(;transform_index<18;++transform_index){
        const u8* t=request+0x2c+transform_index*0x2c;
        const i32 opcode=read_int(t+0x20);
        if(opcode==0)return true;
        if(opcode!=4&&opcode!=8)return false;
        const float start=float(read_int(t+0x14));
        nodes.back().end=start;Th14CurveSample boundary;
        if(!th14_curve_head(nodes.back(),start,boundary))return false;
        Th14CurveNode n;n.start=start;n.mode=opcode==4?1:2;n.position=boundary.position;n.position.z=0;n.angle=boundary.angle;n.speed=boundary.speed;
        n.direction=vector(n.angle,1);n.acceleration=read_float(t);n.turn=read_float(t+4);nodes.push_back(n);
        const i32 duration=read_int(t+0x10);
        if(duration>=0){
            const float end=start+float(duration);nodes.back().end=end;
            if(!th14_curve_head(nodes.back(),end,boundary))return false;
            Th14CurveNode straight;straight.start=end;straight.position=boundary.position;straight.position.z=0;straight.angle=boundary.angle;straight.speed=boundary.speed;straight.direction=vector(straight.angle,1);nodes.push_back(straight);
        }
    }
    return true;
}
void Th14CurvePath::sample(float time,bool reset_negative)noexcept{
    bool previous=false;
    for(u32 j=0;j<samples.size();++j){
        const float t=time-float(j);
        if(reset_negative&&t<0){
            samples[j].position={read_float(request),read_float(request+4),read_float(request+8)};
            samples[j].auxiliary={};samples[j].angle=read_float(request+0xc);samples[j].speed=read_float(request+0x14);continue;
        }
        const Th14CurveNode* node=nullptr;
        for(const auto& n:nodes)if(t>=n.start&&t<n.end){node=&n;break;}
        if(node){
            if(previous&&j)th14_curve_tail(*node,t,samples[j-1],samples[j]);
            else th14_curve_head(*node,t,samples[j]);
        }
        previous=true;
    }
}
i32 Th14CurvePath::cut_circle(Vec3 center,float radius,bool protection,bool skip,std::vector<Vec3>& deliveries,bool& mark){
    deliveries.clear();mark=false;if(skip&&protection)return 0;
    // Retail uses a 256-byte stack selection buffer.
    if(samples.size()>256)return -2;
    std::vector<u8> selected(samples.size(),0);i32 count=0;const float rr=radius*radius;
    for(u32 j=0;j<samples.size();++j){
        const auto p=samples[j].position;const float dx=center.x-p.x,dy=center.y-p.y;
        if(!(dy*dy+dx*dx>rr)){selected[j]=1;++count;if(j%3==0)deliveries.push_back(p);}
    }
    if(!count)return 0;
    if(u32(count)>=samples.size()){mark=true;return count;}
    u32 start=0;while(start<selected.size()&&selected[start])++start;
    if(start==0){
        u32 first=0;while(start<selected.size()&&!selected[start]){++start;++first;}
        while(start<selected.size()&&selected[start])++start;
        if(start>=selected.size()){samples.resize(first);const i32 length=i32(samples.size());std::memcpy(request+0x20,&length,4);return count;}
    }
    samples.erase(samples.begin(),samples.begin()+start);const i32 length=i32(samples.size());std::memcpy(request+0x20,&length,4);return count;
}
i32 Th14CurvePath::cut_rectangle(Vec3 center,Vec2 size,float angle,bool protection,bool skip,Timer& timer,std::vector<Vec3>& deliveries,bool& mark,std::vector<std::array<u8,0x354>>& fragments){
    deliveries.clear();fragments.clear();mark=false;if(skip&&protection)return 0;
    const u32 length=u32(samples.size());std::vector<u8> selected(length,0);i32 count=0;
    const float s=float(std::sin(double(-angle))),c=float(std::cos(double(-angle)));
    for(u32 j=0;j<length;++j){const auto p=samples[j].position;float x=p.x-center.x,y=p.y-center.y;
        if(angle!=0){const float rx=x*c-y*s,ry=y*c+x*s;x=rx;y=ry;}
        // 442a00 selects sample centers; laser width is not added to the rectangle.
        if(!(size.x*.5f<std::fabs(x))&&!(size.y*.5f<std::fabs(y))){selected[j]=1;++count;if(j%3==0)deliveries.push_back(p);}
    }
    if(!count)return 0;if(u32(count)>=length){mark=true;return count;}
    const auto resize=[&](u32 n){samples.resize(n);const i32 value=i32(n);std::memcpy(request+0x20,&value,4);};
    u32 prefix=0;while(prefix<length&&selected[prefix])++prefix;
    if(prefix){selected.erase(selected.begin(),selected.begin()+prefix);timer.advance(-float(prefix));resize(length-prefix);if(samples.size()<4){mark=true;return count;}}
    u32 at=0;while(at<selected.size()&&!selected[at])++at;const u32 kept=at;
    while(at<selected.size()){
        while(at<selected.size()&&selected[at])++at;const u32 start=at;while(at<selected.size()&&!selected[at])++at;
        if(at-start>=4){std::array<u8,0x354> fragment{};std::memcpy(fragment.data(),request,sizeof(request));const i32 n=i32(at-start),sound=-1;const u32 inherited=1;const float time=timer.fractional-float(start);
            std::memcpy(fragment.data()+0x20,&n,4);std::memcpy(fragment.data()+0x344,&sound,4);std::memcpy(fragment.data()+0x34c,&inherited,4);std::memcpy(fragment.data()+0x350,&time,4);fragments.push_back(fragment);}
    }
    if(kept>=4)resize(kept);else mark=true;return count;
}
void th14_curve_mesh(const Th14CurveSample* samples,u32 count,float width,float top_v,float bottom_v,Vec2 origin,Th14CurveVertex* out)noexcept{
    constexpr float halfpi=1.57079637050628662109375f,pi=3.1415927410125732421875f,tau=6.283185482025146484375f;
    float u=0;
    for(u32 j=0;j<count;++j){
        for(u32 side=0;side<2;++side){
            const float turn=side?-halfpi:halfpi;
            const float current=wrap(samples[j].angle+turn);
            float angle=current;
            if(j){
                const float previous=wrap(samples[j-1].angle+turn);const float difference=previous-current;
                float delta=difference;
                if(difference>pi)delta=previous-(current+tau);
                else if(current-previous>pi)delta=previous-(current-tau);
                angle=wrap(wrap(wrap(delta)*.5f)+current);
            }
            const Vec3 v=vector(angle,width*.5f);
            out[j*2+side]={(samples[j].position.x+v.x)+origin.x,(samples[j].position.y+v.y)+origin.y,0,1,0xffffffff,u,side?bottom_v:top_v};
        }
        // Deliberately cumulative; division per point follows 441daf.
        u+=1.f/float(i32(count)-1);
    }
}
#endif
}
