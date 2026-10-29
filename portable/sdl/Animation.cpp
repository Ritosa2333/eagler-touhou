// Line-by-line port of upstream src/animation.mjs (Track, AnimationSampler,
// MenuAnimation) and src/easing.mjs. Native evidence cited upstream: 0x455630.
#include "Animation.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>

namespace th12host {
namespace {
constexpr float Pi=3.1415927410125732f,Tau=6.2831854820251465f;
float f32(double v){return float(v);}
float normalize(float a){while(a>Pi)a=f32(double(a)-Tau);while(a<-Pi)a=f32(double(a)+Tau);return a;}
float byte(double n){return float((i64(std::trunc(n))&255)/255.0);}
i32 to_i32(double v){if(!std::isfinite(v))return 0;return i32(u32(i64(std::fmod(std::trunc(v),4294967296.0))));} // JS v|0
// Interpolation field keys.
enum Key:u8 {X,Y,Z,OX,OY,OZ,RX,RY,RZ,SX,SY,A,R,G,B,R2,G2,B2,A2,SU,SV};
u32 key_id(std::initializer_list<u8> keys){u32 id=0;for(u8 k:keys)id=id*32+k+1;return id;}
}

double easing(double t,i32 mode){
    t=std::clamp(t,0.,1.);
    if(mode>=1&&mode<=3)return std::pow(t,mode+1);
    if(mode>=4&&mode<=6)return 1-std::pow(1-t,mode-2);
    if(mode>=9&&mode<=11){const i32 p=mode-7;return t<.5?std::pow(2*t,p)/2:1-std::pow(2-2*t,p)/2;}
    if(mode>=12&&mode<=14){const i32 p=mode-10;return t<.5?(1-std::pow(1-2*t,p))/2:(std::pow(2*t-1,p)+1)/2;}
    return mode==15?0:mode==16?1:t;
}

Track::Track(AnimationSampler& o,std::shared_ptr<const AnmFile> f,i32 bank_,i32 id,std::optional<BulletVariant> v,const SampleOptions& opt,u32 d)
    :owner(o),file(std::move(f)),bank(bank_),id_(id),depth(d),variant(v),options(opt),seed(opt.seed&65535){
    script=file?file->script(id):nullptr;
    // Native binding runs initial commands before accepting a launch interrupt.
    if(!options.interrupts.empty()){commands_at_time();for(const i32 code:options.interrupts){interrupt(code);commands_at_time();}}
    else if(options.interrupt||variant){commands_at_time();interrupt(options.interrupt?options.interrupt:2);}
}
std::optional<double> Track::local(double key)const{for(const auto& [k,v]:locals)if(k==key)return v;return std::nullopt;}
void Track::set_local(double key,double value){for(auto& [k,v]:locals)if(k==key){v=value;return;}locals.emplace_back(key,value);}
u32 Track::random32(bool force_game){
    if(!force_game&&display_rng&&owner.display_random32)return owner.display_random32();
    const u32 a=((seed^0x9630)-0x6553)&65535;seed=((a<<2)|(a>>14))&65535;
    const u32 b=((seed^0x9630)-0x6553)&65535;seed=((b<<2)|(b>>14))&65535;
    return (a<<16)|b;
}
// A deterministic local display fallback (upstream: retail display-RNG
// scheduling is not verified).
double Track::random(){return random32()/4294967296.0;}
double Track::value(const AnmCommand& c,u32 i,bool real){
    const double v=real?(i<c.floats.size()?double(c.floats[i]):0.):(i<c.ints.size()?double(c.ints[i]):0.);
    if(!(c.mask&(1u<<i)))return v;
    if(const auto l=local(v))return *l;
    if(v>=10000&&v<=10009)return 0;
    // Native integer and float readers have different builtin tables.
    if(!real)return v==10022?double(i32(random32())):v;
    if(v==10010)return f32(double(f32(random()*2-1))*Pi);
    if(v==10011)return f32(random());
    if(v==10012)return f32(random()*2-1);
    if(v==10013)return state.x;if(v==10014)return state.y;if(v==10015)return state.z;
    if(v>=10016&&v<=10021)return 0; // camera position / eye offset: not supplied by the renderer
    if(v==10022)return f32(double(random32()));
    if(v==10023)return state.rx;if(v==10024)return state.ry;if(v==10025)return state.rotation;
    return v;
}
bool Track::interrupt(i32 code){
    if(!script)return false;const auto& cs=script->commands;size_t at=cs.size();
    for(size_t i=0;i<cs.size();++i)if(cs[i].op==64&&!cs[i].ints.empty()&&cs[i].ints[0]==code){at=i;break;}
    if(at==cs.size())for(size_t i=0;i<cs.size();++i)if(cs[i].op==64&&!cs[i].ints.empty()&&cs[i].ints[0]==-1){at=i;break;}
    if(at==cs.size())return false;
    saved=Saved{pc,clock,stopped};pc=at+1;clock=cs[at].time;stopped=false;ended_=false;state.visible=true;return true;
}
float& Track::field(u8 k){
    auto& s=state;
    switch(k){case X:return s.x;case Y:return s.y;case Z:return s.z;case OX:return s.offset_x;case OY:return s.offset_y;case OZ:return s.offset_z;
        case RX:return s.rx;case RY:return s.ry;case RZ:return s.rotation;case SX:return s.sx;case SY:return s.sy;case A:return s.alpha;
        case R:return s.r;case G:return s.g;case B:return s.b;case R2:return s.r2;case G2:return s.g2;case B2:return s.b2;case A2:return s.alpha2;
        case SU:return velocity.u;default:return velocity.v;}
}
void Track::interpolate(std::initializer_list<u8> keys,std::initializer_list<double> to,i32 duration,i32 mode,const double* ha,const double* hb){
    assert(keys.size()<=3&&keys.size()==to.size());
    const auto* fields=keys.begin();const auto* targets=to.begin();
    const u32 id=key_id(keys);
    if(duration<=0){for(size_t i=0;i<keys.size();++i)field(fields[i])=f32(targets[i]);
        interpolations.erase(std::remove_if(interpolations.begin(),interpolations.end(),[&](const auto& p){return p.first==id;}),interpolations.end());return;}
    Interpolation t;t.count=u8(keys.size());
    for(size_t i=0;i<keys.size();++i){t.keys[i]=fields[i];t.from[i]=field(fields[i]);t.to[i]=f32(targets[i]);}
    t.duration=duration;t.mode=mode;
    if(ha&&hb){t.hermite=true;for(int i=0;i<3;++i){t.a[i]=ha[i];t.b[i]=hb[i];}}
    // Map.set: an existing key keeps its place.
    for(auto& p:interpolations)if(p.first==id){p.second=std::move(t);return;}
    interpolations.emplace_back(id,std::move(t));
}
void Track::child(u16 op,i32 id,const AnmCommand* offset_command){
    if(depth>=12||children_.size()>=256)return;
    if(!file||!file->script(id))return;
    SampleOptions o;o.seed=seed;
    auto c=std::make_unique<Track>(owner,file,bank,id,std::nullopt,o,depth+1);const bool det=op==95||op==97;
    c->state.layer=state.layer;c->born=ticks;c->detached=det;
    c->base=det?std::array<float,3>{state.x,state.y,state.z}:std::array<float,3>{0,0,0};
    if(det){c->state.rx=state.rx;c->state.ry=state.ry;c->state.rotation=state.rotation;}
    // Native child constructor binds and updates the child immediately.
    c->tick();
    if(offset_command){const float x=f32(value(*offset_command,1,true)),y=f32(value(*offset_command,2,true));
        c->state.offset_x=x;c->state.offset_y=y;c->frame_.a.x=f32(double(c->frame_.a.x)+x);c->frame_.a.y=f32(double(c->frame_.a.y)+y);}
    children_.insert(children_.begin(),std::move(c));
}
bool Track::command(const AnmCommand& c){
    auto& s=state;const auto I=[&](u32 i){return value(c,i,false);};const auto F=[&](u32 i){return value(c,i,true);};
    const auto jump_to=[&](double offset,double time){const auto it=script->offsets.find(u32(i64(offset)));pc=it==script->offsets.end()||offset<0?SIZE_MAX:it->second;clock=to_i32(time);};
    if(c.op>=6&&c.op<=27){
        const bool real=c.op%2,three=c.op>=18;const i32 op=three?(c.op-18)/2+1:(c.op-6)/2;
        const double key=real?(c.floats.empty()?0:c.floats[0]):(c.ints.empty()?0:c.ints[0]);
        const double a=three?value(c,1,real):local(key).value_or(0),b=value(c,three?2:1,real);
        const double v=op==0?b:op==1?a+b:op==2?a-b:op==3?a*b:op==4?(b?a/b:0):(b?std::fmod(a,b):0);
        set_local(key,real?double(f32(v)):double(to_i32(v)));return false;
    }
    if(c.op>=28&&c.op<=39){
        const bool real=c.op%2;const double a=value(c,0,real),b=value(c,1,real);const i32 kind=(c.op-28)/2;
        const bool pass=kind==0?a==b:kind==1?a!=b:kind==2?a<b:kind==3?a<=b:kind==4?a>b:a>=b;
        if(pass){const double target=I(2),time=I(3);jump_to(target,time);return true;}return false;
    }
    switch(c.op){
    case 0:break;
    case 65535:case 2:ended_=true;pc=SIZE_MAX;break;
    case 1:s.visible=false;ended_=true;pc=SIZE_MAX;break;
    case 3:{const i32 n=i32(I(0));s.sprite=variant&&n>=0&&n<3?(n==0?variant->sprite:n==1?variant->spawn:variant->cancel):n;s.visible=true;break;}
    case 4:{const double target=I(0),time=I(1);jump_to(target,time);return true;}
    case 5:{const double key=c.ints.empty()?0:c.ints[0],n=local(key).value_or(0)-1;set_local(key,n);
        if(n>0){const double target=I(1),time=I(2);jump_to(target,time);return true;}break;}
    case 40:{const double key=c.ints.empty()?0:c.ints[0];const u32 range=u32(i32(I(1)));set_local(key,range?double(random32()%range):0);break;}
    case 41:{const double key=c.floats.empty()?0:c.floats[0];const double r=random();set_local(key,f32(r*F(1)));break;}
    case 42:case 43:{const double key=c.floats.empty()?0:c.floats[0];const double v=F(1);set_local(key,f32(c.op==42?std::sin(v):std::cos(v)));break;}
    case 48:{const float z=f32(F(2)),y=f32(F(1)),x=f32(F(0));if(options.position_offset){s.offset_x=x;s.offset_y=y;s.offset_z=z;}else{s.x=x;s.y=y;s.z=z;}break;}
    case 49:{const float rx=f32(F(0)),ry=f32(F(1)),rz=f32(F(2));s.rx=rx;s.ry=ry;s.rotation=rz;break;}
    case 50:{const float x=f32(F(0)),y=f32(F(1));s.sx=x;s.sy=y;break;}
    case 51:s.alpha=byte(I(0));break;
    case 52:{const float r=byte(I(0)),g=byte(I(1)),b=byte(I(2));s.r=r;s.g=g;s.b=b;break;}
    case 76:{const float r=byte(I(0)),g=byte(I(1)),b=byte(I(2));s.r2=r;s.g2=g;s.b2=b;break;}
    case 53:{const float a=f32(F(0)),b=f32(F(1)),d=f32(F(2));velocity.rx=a;velocity.ry=b;velocity.rotation=d;break;}
    case 54:{const float a=f32(F(0)),b=f32(F(1));velocity.sx=a;velocity.sy=b;break;}
    case 55:{const double to=byte(I(0));interpolate({A},{to},i32(I(1)),0);break;}
    case 56:{const double x=F(2),y=F(3),z=F(4);const i32 d=i32(I(0)),m=i32(I(1));
        interpolate(options.position_offset?std::initializer_list<u8>{OX,OY,OZ}:std::initializer_list<u8>{X,Y,Z},{x,y,z},d,m);break;}
    case 57:case 78:{const double r=byte(I(2)),g=byte(I(3)),b=byte(I(4));const i32 d=i32(I(0)),m=i32(I(1));
        interpolate(c.op==78?std::initializer_list<u8>{R2,G2,B2}:std::initializer_list<u8>{R,G,B},{r,g,b},d,m);break;}
    case 58:case 79:{const double a=byte(I(2));const i32 d=i32(I(0)),m=i32(I(1));interpolate({u8(c.op==79?A2:A)},{a},d,m);break;}
    case 59:{const double x=F(2),y=F(3),z=F(4);const i32 d=i32(I(0)),m=i32(I(1));interpolate({RX,RY,RZ},{x,y,z},d,m);break;}
    case 60:{const double x=F(2),y=F(3);const i32 d=i32(I(0)),m=i32(I(1));interpolate({SX,SY},{x,y},d,m);break;}
    case 61:s.sx=-s.sx;break;
    case 62:s.sy=-s.sy;break;
    case 63:stopped=true;return true;
    case 64:break;
    case 65:{const i32 v=i32(I(0));s.anchor_x=v&3;s.anchor_y=(u32(v)>>16)&3;break;}
    case 66:s.blend=i32(I(0))&7;break;
    case 67:s.mode=i32(I(0))&31;break;
    case 68:s.layer=i32(I(0))&255;break;
    case 69:s.visible=false;stopped=true;return true;
    case 70:velocity.u=f32(F(0));break;
    case 71:velocity.v=f32(F(0));break;
    case 72:s.visible=i32(I(0))&1;break;
    case 75:clock-=i32(I(0));break;
    case 77:s.alpha2=byte(I(0));break;
    case 80:s.use_color2=i32(I(0))&1;break;
    case 81:if(saved){pc=saved->pc;clock=saved->clock;stopped=saved->stopped;saved.reset();return true;}break;
    case 84:case 101:s.mode=c.op==84?9:13;s.geometry_count=i32(I(0));break;
    case 87:display_rng=i32(I(0))&1;break;
    case 88:case 90:case 91:case 92:case 95:child(c.op,i32(I(0)));break;
    case 89:s.point=i32(I(0))&1;break;
    case 93:case 94:{const double to=F(2);const i32 d=i32(I(0)),m=i32(I(1));interpolate({u8(c.op==93?SU:SV)},{to},d,m);break;}
    case 96:case 97:child(c.op,i32(I(0)),&c);break;
    case 100:{const double x=F(4),y=F(5),z=F(6);const i32 d=i32(I(0));const double a[3]{F(1),F(2),F(3)},b[3]{F(7),F(8),F(9)};
        interpolate(options.position_offset?std::initializer_list<u8>{OX,OY,OZ}:std::initializer_list<u8>{X,Y,Z},{x,y,z},d,8,a,b);break;}
    case 102:{const i32 n=i32(I(0));const u32 range=u32(i32(I(1)));const u32 r=random32(true);if(!range){ended_=true;break;}s.sprite=i32(u32(n)+r%range);s.visible=true;break;}
    case 103:s.mode=14;s.geometry_radius=f32(F(0));s.geometry_width=f32(F(1));break;
    case 104:case 105:s.mode=c.op==104?15:16;s.geometry_radius=f32(F(0));s.geometry_count=i32(I(1));break;
    case 108:s.mode=18;s.geometry_radius=f32(F(0));s.geometry_width=f32(F(1));break;
    case 110:s.mode=20;s.geometry_radius=f32(F(0));s.geometry_width=f32(F(1));break;
    default:break; // 73, 82, 83, 85, 86 and unknown: no presentation effect here
    }
    return false;
}
void Track::commands_at_time(){
    if(stopped||ended_||!script)return;
    const auto& cs=script->commands;
    for(u32 budget=0;budget<1000&&pc<cs.size()&&cs[pc].time<=clock;++budget){
        const size_t at=pc;const bool jumped=command(cs[at]);if(!jumped&&pc==at)++pc;if(stopped||ended_)break;
    }
}
void Track::tick(){
    const u32 now=ticks;commands_at_time();auto& s=state;
    s.rx=normalize(f32(double(s.rx)+velocity.rx));s.ry=normalize(f32(double(s.ry)+velocity.ry));s.rotation=normalize(f32(double(s.rotation)+velocity.rotation));
    s.sx=f32(double(s.sx)+velocity.sx);s.sy=f32(double(s.sy)+velocity.sy);
    s.u=f32(double(s.u)+velocity.u);s.v=f32(double(s.v)+velocity.v);
    if(s.u>=1)s.u=f32(double(s.u)-1);else if(s.u<0)s.u=f32(double(s.u)+1);
    if(s.v>=1)s.v=f32(double(s.v)-1);else if(s.v<0)s.v=f32(double(s.v)+1);
    for(size_t n=0;n<interpolations.size();){
        auto& t=interpolations[n].second;
        // TH12 interpolation helpers advance their own timer before sampling (0x458cff etc).
        const double p=std::min(1.,double(++t.time)/t.duration),e=easing(p,t.mode),e32=easing(double(f32(p)),t.mode);
        for(size_t i=0;i<t.count;++i){
            const u8 k=t.keys[i];double value;
            if(t.hermite){const double p2=p*p,p3=p2*p;value=(2*p3-3*p2+1)*t.from[i]+(-2*p3+3*p2)*t.to[i]+(p3-2*p2+p)*t.a[i]+(p3-p2)*t.b[i];}
            else value=t.from[i]+(t.to[i]-t.from[i])*e;
            if(k==R||k==G||k==B||k==R2||k==G2||k==B2){const double a=std::round(t.from[i]*255),b=std::round(t.to[i]*255);field(k)=byte(a-std::trunc((a-b)*e32));}
            else if(k==A||k==A2){const double a=std::round(t.from[i]*255),b=std::round(t.to[i]*255);field(k)=byte(std::trunc(a+(b-a)*e32));}
            else field(k)=f32(value);
        }
        if(t.time>=t.duration)interpolations.erase(interpolations.begin()+i64(n));else ++n;
    }
    Frame f;auto& a=f.a;
    a.bank=bank;a.script=id_;a.sprite=s.sprite;a.x=f32(double(s.x)+s.offset_x);a.y=f32(double(s.y)+s.offset_y);a.z=f32(double(s.z)+s.offset_z);
    a.rx=s.rx;a.ry=s.ry;a.rotation=s.rotation;a.sx=s.sx;a.sy=s.sy;
    if(s.use_color2){a.r=s.r2;a.g=s.g2;a.b=s.b2;a.alpha=s.alpha2;}else{a.r=s.r;a.g=s.g;a.b=s.b;a.alpha=s.alpha;}
    a.r2=s.r2;a.g2=s.g2;a.b2=s.b2;a.alpha2=s.alpha2;a.blend=s.blend;a.layer=s.layer;a.visible=s.visible;a.mode=s.mode;
    a.anchor_x=s.anchor_x;a.anchor_y=s.anchor_y;a.u=s.u;a.v=s.v;a.point=s.point;a.uv_scale_x=s.uv_scale_x;a.uv_scale_y=s.uv_scale_y;
    a.geometry_count=s.geometry_count;a.geometry_radius=s.geometry_radius;a.geometry_width=s.geometry_width;a.rotate=s.mode==1||s.mode==3;
    // Preserve newest-first VM ticks and display RNG consumption. Frame slots
    // follow creation order and retain their nested vector storage across ticks.
    if(children_.empty())frame_=std::move(f);
    else{frame_.a=a;tick_children(now);}
    has_frame=true;++ticks;if(!stopped&&!ended_)++clock;
}
void Track::tick_children(u32 now){
    const auto& a=frame_.a;const auto& s=state;
    size_t visible_frames=0;
    for(auto& c:children_){
        if(c->born!=now)c->tick();if(c->has_frame)++visible_frames;
    }
    frame_.children.resize(visible_frames);size_t at=0;
    for(auto it=children_.rbegin();it!=children_.rend();++it){
        const auto& c=*it;if(!c->has_frame)continue;
        const std::array<float,3> offset=c->detached?c->base:std::array<float,3>{f32(double(a.x)+c->base[0]),f32(double(a.y)+c->base[1]),f32(double(a.z)+c->base[2])};
        translate_frame(frame_.children[at++],c->frame_,offset,c->detached?nullptr:&s);
    }
    children_.erase(std::remove_if(children_.begin(),children_.end(),[](const auto& c){return c->ended_&&!c->state.visible;}),children_.end());
}
void Track::translate_frame(Frame& out,const Frame& in,const std::array<float,3>& offset,const State* parent){
    out.a=in.a;out.a.x=float(double(in.a.x)+offset[0]);out.a.y=float(double(in.a.y)+offset[1]);out.a.z=float(double(in.a.z)+offset[2]);
    // Native 45a593/45ccb5 multiply the immediate parent's scale and add its
    // rotation for rotated modes. Descendants already contain their VM parent.
    out.a.sx=in.a.sx*(parent?parent->sx:1.f);out.a.sy=in.a.sy*(parent?parent->sy:1.f);
    out.a.rotation=float(double(in.a.rotation)+((in.a.rotate||in.a.mode>=14)&&parent?parent->rotation:0.f));
    out.children.resize(in.children.size());
    for(size_t i=0;i<in.children.size();++i)translate_frame(out.children[i],in.children[i],offset,nullptr);
}
const Track::Frame& Track::sample(i32 age){
    age=std::clamp(age,0,100000);
    while(ticks<=u32(age)){
        tick();
        // A stopped VM still runs velocity, interpolation and children. Once
        // all of them settle, only its age changes. Always tick once first so
        // commands, interrupts and public state edits keep their normal order.
        if((stopped||ended_)&&children_.empty()&&frame_.children.empty()&&interpolations.empty()
            &&velocity.rx==0&&velocity.ry==0&&velocity.rotation==0&&velocity.sx==0&&velocity.sy==0&&velocity.u==0&&velocity.v==0
            &&state.u>=0&&state.u<1&&state.v>=0&&state.v<1){
            ticks=u32(age)+1;break;
        }
    }
    return frame_;
}

size_t AnimationSampler::KeyHash::operator()(const Key& k)const{
    size_t h=std::hash<i64>()(i64(k.bank)<<32^k.script);
    const auto mix=[&](u64 v){h^=std::hash<u64>()(v)+0x9e3779b97f4a7c15ull+(h<<6)+(h>>2);};
    mix(k.has_variant?u64(u32(k.variant.sprite))<<32^u32(k.variant.spawn)^(u64(u32(k.variant.cancel))<<16):0);mix(k.entity);mix(k.seed);mix(u32(k.interrupt));
    for(i32 i:k.interrupts)mix(u32(i));mix(k.position_offset);return h;
}
void AnimationSampler::end_frame(){
    for(auto it=tracks.begin();it!=tracks.end();)if(it->second.generation!=generation)it=tracks.erase(it);else ++it;
}
const Track::Frame* AnimationSampler::sample(i32 bank,i32 script,i32 age,std::optional<BulletVariant> variant,const SampleOptions& o){
    auto file=files?files(bank):nullptr;if(!file||!file->script(script))return nullptr;
    Key key{bank,script,variant.has_value(),variant.value_or(BulletVariant{}),o.entity,o.seed&65535,o.interrupt,o.interrupts,o.position_offset};
    age=std::clamp(age,0,100000);
    auto it=tracks.find(key);
    if(it==tracks.end()||i64(age)<i64(it->second.track->tick_count())-1){
        auto track=std::make_unique<Track>(*this,file,bank,script,variant,o);
        if(it==tracks.end())it=tracks.emplace(std::move(key),CachedTrack{std::move(track),generation}).first;else it->second.track=std::move(track);
    }
    it->second.generation=generation;
    return &it->second.track->sample(age);
}
void AnimationSampler::flatten(const Track::Frame& f,std::vector<Appearance>& out){
    out.push_back(f.a);for(const auto& c:f.children)flatten(c,out);
}
void AnimationSampler::sample_all(i32 bank,i32 script,i32 age,std::vector<Appearance>& out,std::optional<BulletVariant> variant,const SampleOptions& o){
    if(const auto* root=sample(bank,script,age,variant,o))flatten(*root,out);
}

MenuAnimation::MenuAnimation(AnimationSampler& sampler,std::shared_ptr<const AnmFile> file,i32 bank,i32 script,const SampleOptions& o)
    :track(sampler,std::move(file),bank,script,std::nullopt,o){track.commands_at_time();}
void MenuAnimation::tick(){track.tick();for(auto& [script,p]:positions)if(p.frames>0&&p.age<p.frames)++p.age;}
void MenuAnimation::interrupt(i32 code,bool recursive,bool immediate,i32 script){
    std::function<void(Track&)> visit=[&](Track& t){
        if(script<0||t.id()==script){t.interrupt(code);if(immediate)t.commands_at_time();}
        if(recursive)for(auto& c:t.children())visit(*c);};
    visit(track);
}
void MenuAnimation::set_sprite(i32 script,i32 sprite){
    std::function<void(Track&)> visit=[&](Track& t){if(t.id()==script)t.state.sprite=sprite;for(auto& c:t.children())visit(*c);};visit(track);
}
void MenuAnimation::set_position(i32 script,std::optional<float> x,std::optional<float> y,i32 frames){
    std::vector<Appearance> current;poses(current);float px=0,py=0;bool found=false;
    for(const auto& a:current)if(a.script==script){px=a.x;py=a.y;found=true;break;}
    if(!found){std::function<const Track*(const Track&)> find=[&](const Track& t)->const Track*{if(t.id()==script)return &t;for(const auto& c:const_cast<Track&>(t).children())if(const Track* f=find(*c))return f;return nullptr;};
        const Track* t=find(track);if(!t)return;px=t->state.x;py=t->state.y;}
    const float tx=x.value_or(px),ty=y.value_or(py);
    if(const auto it=positions.find(script);it!=positions.end()&&it->second.to_x==tx&&it->second.to_y==ty)return;
    positions[script]={px,py,tx,ty,x.has_value(),y.has_value(),frames,0};
}
bool MenuAnimation::alive(){
    std::function<bool(Track&)> visit=[&](Track& t){if(!t.ended())return true;for(auto& c:t.children())if(visit(*c))return true;return false;};
    return visit(track);
}
void MenuAnimation::poses(std::vector<Appearance>& out)const{
    const auto* root=track.frame();if(!root)return;
    std::function<void(const Track::Frame&)> visit=[&](const Track::Frame& f){
        Appearance a=f.a;
        if(const auto v=visibility.find(a.script);v!=visibility.end())a.visible=v->second&&a.visible;
        if(const auto p=positions.find(a.script);p!=positions.end()){const auto& q=p->second;const float t=q.frames>0?std::min(1.f,float(q.age)/float(q.frames)):1.f;
            if(q.override_x)a.x=q.from_x+(q.to_x-q.from_x)*t;if(q.override_y)a.y=q.from_y+(q.to_y-q.from_y)*t;}
        if(const auto m=texts.find(a.script);m!=texts.end())a.text=&m->second;
        out.push_back(a);for(const auto& c:f.children)visit(c);
    };
    visit(*root);
}
}
