#include "Scene.hpp"
#include "BinaryReader.hpp"
#include "RuntimeOptions.hpp"
#include "game/GameState.hpp"
#include "game/StageFlow.hpp"
#include "game/BossHud.hpp"
#include "game/CameraEffects.hpp"
#include "game/PresentationGeometry.hpp"
#include "wasm/Api.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace th12host {
namespace {
float f32(double v){return float(v);}
// TH12 1.00b draw-chain priorities installed by 45dfa0 (draw-scheduler.mjs).
constexpr i32 layer_priorities[32]{4,6,8,9,11,13,16,17,18,19,20,23,25,26,28,30,34,36,38,41,42,50,51,52,63,64,-1,-1,14,39,66,65};
i32 layer_priority(i32 layer){return layer>=0&&layer<32?layer_priorities[layer]:-1;}
Color rgba_of(u32 argb){return argb_color(argb);}
std::string width_integer(i64 v,size_t size){std::string s=std::to_string(v);return s.size()<size?std::string(size-s.size(),' ')+s:s;}
std::string precision_integer(i64 v,size_t size){std::string s=std::to_string(v<0?-v:v);if(s.size()<size)s=std::string(size-s.size(),'0')+s;return v<0?"-"+s:s;}
std::string fixed1(double v){const double n=std::floor(v*10+.5);char b[48];std::snprintf(b,sizeof b,"%.1f",n/10);return b;} // JS toFixed(1)
TextDescriptor font_text(std::string text,i32 font,float x,float y,u32 color,i32 draw_flag,float scale=1){
    TextDescriptor t;t.text=std::move(text);t.font=font;t.x=x;t.y=y;t.color=color;t.draw_flag=draw_flag;t.scale_x=t.scale_y=f32(scale);return t;
}
i32 fragment_interrupt(i32 index,i32 whole,i32 fragments){return index<whole?2:index==whole?7+std::clamp(fragments,0,5):3;}
struct Glyph {i32 id;float x,y,sx,sy;u32 color;bool point;};
// Native 4018f0/401bxx: FontManager submissions as ASCII ANM sprites.
void ascii_glyphs(const TextDescriptor& d,std::vector<Glyph>& out){
    static constexpr float metrics[6][2]{{9,14},{7,9},{7,10},{12,16},{12,16},{7,10}};
    out.clear();if(d.font<0||d.font>5)return;
    std::string text=d.text.substr(0,d.text.find('\0'));
    const float sx=d.scale_x,sy=d.scale_y,origin_x=d.x,origin_y=d.y,advance=metrics[d.font][0],height=metrics[d.font][1];
    const float ordinary=f32(double(advance)*sx);const bool large=d.font==3||d.font==4;
    float x=origin_x,y=origin_y,step=ordinary;
    if(d.align_x==0||d.align_x==2){const double factor=d.align_x==0?.5:1;
        if(d.font==0||d.font==1)x=f32(x-double(text.size())*ordinary*factor);
        else for(char c:text){const bool narrow=large?c==',':c=='.';x=f32(x-(narrow?4.0*sx:double(ordinary))*factor);}}
    if(d.align_y==0)y=f32(y-height*.5);else if(d.align_y==2)y=f32(y-height);
    for(char c:text){
        if(c=='\n'){y=f32(double(y)+double(height)*sy);x=origin_x;continue;}
        if(c==' '){x=f32(double(x)+step);continue;}
        step=ordinary;i32 id;const i32 code=u8(c);
        if(d.font==0)id=code-32;else if(d.font==1)id=code+66;
        else if(large){id=c=='/'?253:c=='.'?254:c=='s'?255:c=='*'?256:c==','?257:code+195;y=c==','?f32(double(origin_y)+3):origin_y;if(c==',')step=f32(4.0*sx);}
        else{id=c=='/'?237:c==':'?238:c=='-'?239:c=='*'?240:c=='%'?241:c=='.'?242:c=='$'?262:code+179;if(c=='.')step=f32(4.0*sx);}
        out.push_back({id,x,y,sx,sy,d.color,d.font==4||d.font==5||(d.font==0&&sx!=1)});x=f32(double(x)+step);
    }
}
// 41d0b0 checksum/decode; malformed recorded times render 999.99.
std::array<i32,2> decode_spell_real_time(i32 encoded){
    const i32 tail=encoded%100,packed=encoded/100,seconds=(packed%1000+934)%1000,hundredths=(tail+67)%100;
    return encoded/100000-22==seconds+hundredths?std::array<i32,2>{seconds,hundredths}:std::array<i32,2>{999,99};
}
std::array<float,3> sub(const std::array<float,3>& a,const std::array<float,3>& b){return {a[0]-b[0],a[1]-b[1],a[2]-b[2]};}
double dot(const std::array<double,3>& a,const std::array<double,3>& b){return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];}
std::array<double,3> cross(const std::array<double,3>& a,const std::array<double,3>& b){return {a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]};}
std::array<double,3> unit(const std::array<double,3>& a){double n=std::sqrt(dot(a,a));if(!n)n=1;return {a[0]/n,a[1]/n,a[2]/n};}
}

std::string grouped_number(double n){
    std::string s=std::to_string(i64(std::max(0.,std::round(n))));for(i64 i=i64(s.size())-3;i>0;i-=3)s.insert(size_t(i),",");return s;
}
i64 stage_clear_award(const std::array<float,41>& h,bool practice){
    const i64 stage=i64(h[39]),difficulty=i64(h[19]),power=i64(h[4]),lives=i64(h[5]),bombs=i64(h[6]);
    i64 points=i64(std::trunc(h[20]/100.0));points-=points%10;const i64 base=stage*1000000;
    if(practice)return base;
    if(stage==6){if(difficulty==4)return base+(power+lives*100)*400000+points*100;
        static constexpr i64 rate[4]{25,50,100,150};return base+(power+(bombs+lives*2)*rate[std::clamp<i64>(difficulty,0,3)])*10000+points*100;}
    return stage==7?base+(points+(power+(bombs+lives*2)*150)*100)*100:base;
}

bool StdGeometry::parse(const std::vector<u8>& b,std::string& error){
    objects.clear();instances.clear();
    const auto u16_=[&](size_t p){return p+2<=b.size()?u16(b[p]|b[p+1]<<8):u16(0xffff);};
    const auto u32_=[&](size_t p){u32 v=0;if(p+4<=b.size())std::memcpy(&v,&b[p],4);return v;};
    const auto f=[&](size_t p){float v=0;if(p+4<=b.size())std::memcpy(&v,&b[p],4);return v;};
    if(b.size()<144){error="STD too small";return false;}
    i32 slot=0;
    for(u32 i=0;i<u16_(0);++i){
        const size_t at=u32_(144+i*4);Object o;o.layer=at+2<b.size()?b[at+2]:0;size_t p=at+28;
        while(u16_(p)!=0xffff){const u16 size=u16_(p+2);if(size<28||p+size>b.size()){error="invalid STD primitive";return false;}
            Quad q;q.kind=u16_(p);q.script=u16_(p+4);q.slot=slot++;q.position={f(p+8),f(p+12),f(p+16)};q.size={f(p+20),f(p+24)};o.quads.push_back(q);p+=size;}
        objects.push_back(std::move(o));
    }
    for(size_t p=u32_(4);u16_(p)!=0xffff;p+=16){
        Instance n;n.object=u16_(p);if(n.object>=objects.size()){error="invalid STD instance object";return false;}
        n.position={f(p+4),f(p+8),f(p+12)};instances.push_back(n);
    }
    return true;
}

Scene::Scene(Renderer& r,TextRaster& t,Banks& b,const ExeTables& e):gpu(r),raster(t),banks(b),exe(e){
    sampler_.files=[this](i32 bank){return banks.get(bank);};
}
const AnmSprite* Scene::sprite(i32 bank,i32 id)const{const auto f=banks.get(bank);return f?f->sprite(id):nullptr;}
Texture Scene::texture(i32 bank,i32 entry)const{
    // 431630: text@R entry 2 is the stage surface; entry 1 the paused field.
    if(bank==BankText&&entry==2)return gpu.stage_surface().available?const_cast<Renderer&>(gpu).stage_surface():Texture{};
    if(bank==BankText&&entry==1)return const_cast<Renderer&>(gpu).pause_surface();
    const auto f=banks.get(bank);if(!f||entry<0||u32(entry)>=f->entries.size())return {};
    return gpu.texture_for(f->entries[size_t(entry)]);
}
void Scene::reset_presentation(){sampler_.reset();last_frame=-1;previous_motion.clear();current_motion.clear();}
void Scene::drop_text_cache(){for(auto* cache:{&dialogue_cache,&menu_cache}){for(auto& c:*cache)gpu.delete_texture(c.texture.name);cache->clear();}}

void Scene::add(i32 bank,i32 script,i32 age,float x,float y,float angle,float scale,std::optional<BulletVariant> variant,const SampleOptions& sampling,float alpha,i32 priority,i8 clipped){
    scratch.clear();sampler_.sample_all(bank,script,age,scratch,variant,sampling);
    u32 child=0;
    for(const auto& a:scratch){Pose p;p.a=a;p.x=x;p.y=y;p.angle=angle;p.scale=scale;p.alpha=alpha;
        if(sampling.entity)p.continuity=(u64(1)<<62)|((sampling.entity&0xffffffff)<<30)|(u64(bank&31)<<25)|(u64(script&1023)<<15)|(child++&32767);
        if(priority>=0){p.has_priority=true;p.priority=priority;}p.clipped=clipped;p.has_order=true;p.order=i32(poses.size());poses.push_back(p);}
}

void Scene::capture_background(){
    using namespace th12;triangles.clear();
    const auto& s=stageState;const auto& c=s.camera;has_surface=!stageProgram.commands.empty();
    fog={((c.fogArgb>>16)&255)/255.f,((c.fogArgb>>8)&255)/255.f,(c.fogArgb&255)/255.f,1};
    shake=cameraEffects.offsets;
    if(!has_surface||geometry_stage!=stageNumber)return;
    if(s.flags&8)return;
    // 430a70: LookAt eye=basePosition+offset, at=basePosition+direction.
    std::array<float,3> position,target;
    for(int i=0;i<3;++i){position[i]=f32(double(c.position[i])+c.offset[i]);target[i]=f32(double(c.position[i])+c.direction[i]);}
    const auto view=sub(target,position);
    const auto z=unit({view[0],view[1],view[2]}),x=unit(cross({c.up[0],c.up[1],c.up[2]},z)),y=cross(z,x);
    const double focal=480/2/std::tan(double(c.fov)/2),center_x=13+422/2.,center_y=240;
    const double fog_start=c.fogStart,fog_end=c.fogEnd;
    const auto stage_file=banks.get(BankStage);if(!stage_file)return;
    struct P {double cx,cy,depth,u,v;};
    for(const auto& instance:geometry.instances){
        const auto& object=geometry.objects[instance.object];
        // 40396e/403d45 gates both STD object-layer groups 0..7 and 8..11.
        if(!(s.flags&1)&&object.layer<=11)continue;
        for(const auto& q:object.quads){
            if(q.kind)continue;
            const auto* node=animationScene.find(stagePrimitiveKey(q.slot));if(!node)continue;
            const auto& a=node->vm.state();
            const u32 argb=a.flags&0x10000?a.secondary:a.primary;
            const bool visible=(a.flags&3)==3&&(a.primary>>24)!=0;const float alpha=float(argb>>24)/255.f;
            const AnmSprite* sp=stage_file->sprite(a.sprite);
            if(!sp||!visible||alpha<=0)continue;
            // TH12's STD draw routine overwrites the ANM scale for every nonzero
            // primitive dimension (0x4047f8..0x404839).
            const double w=q.size[0]?double(q.size[0]):double(a.spriteWidth!=-1?a.spriteWidth:sp->w)*a.sx;
            const double h=q.size[1]?double(q.size[1]):double(a.spriteHeight!=-1?a.spriteHeight:sp->h)*a.sy;
            const double px0=double(a.x)+a.engineX+a.offsetX,py0=double(a.y)+a.engineY+a.offsetY,pz0=double(a.z)+a.engineZ+a.offsetZ;
            const double cxp=double(q.position[0])+instance.position[0]+px0,cyp=double(q.position[1])+instance.position[1]+py0,czp=double(q.position[2])+instance.position[2]+pz0;
            const double rcx=std::cos(double(a.rx)),rsx=std::sin(double(a.rx)),rcy=std::cos(double(a.ry)),rsy=std::sin(double(a.ry)),rcz=std::cos(double(a.rotation)),rsz=std::sin(double(a.rotation));
            std::vector<P> polygon;
            static constexpr double corners[4][4]{{-.5,-.5,0,0},{.5,-.5,1,0},{.5,.5,1,1},{-.5,.5,0,1}};
            for(const auto& k:corners){
                double px=k[0]*w,py=k[1]*h,pz=0,t;
                t=py*rcx-pz*rsx;pz=py*rsx+pz*rcx;py=t;
                t=px*rcy+pz*rsy;pz=-px*rsy+pz*rcy;px=t;
                t=px*rcz-py*rsz;py=px*rsz+py*rcz;px=t;
                const std::array<double,3> d{cxp+px-position[0],cyp+py-position[1],czp+pz-position[2]};
                polygon.push_back({dot(d,x),dot(d,y),dot(d,z),sp->x+k[2]*sp->w,sp->y+k[3]*sp->h});
            }
            // Clip the near plane before projection; never discard a whole tile
            // just because one corner is behind the camera.
            for(int plane=0;plane<2;++plane){
                const double limit=plane?exe.far_plane:exe.near_plane;std::vector<P> out;
                const auto keep=[&](double d){return plane?d<=0:d>=0;};
                for(size_t i=0;i<polygon.size();++i){const P& p=polygon[i];const P& n=polygon[(i+1)%polygon.size()];const bool in=keep(p.depth-limit);
                    if(in)out.push_back(p);
                    if(in!=keep(n.depth-limit)){const double t=(limit-p.depth)/(n.depth-p.depth);
                        out.push_back({p.cx+(n.cx-p.cx)*t,p.cy+(n.cy-p.cy)*t,p.depth+(n.depth-p.depth)*t,p.u+(n.u-p.u)*t,p.v+(n.v-p.v)*t});}}
                polygon.swap(out);
            }
            if(polygon.size()<3)continue;
            const auto& entry=stage_file->entries[size_t(std::clamp<i32>(sp->entry,0,i32(stage_file->entries.size())-1))];
            const Texture tex=texture(BankStage,sp->entry);if(!tex.name)continue;
            const Color color=rgba_of(argb);const bool no_fog=a.flags&0x1000;
            std::vector<Vertex> vs;bool left=true,right=true,above=true,below=true;
            for(const auto& p:polygon){
                Vertex v;v.x=float(center_x+p.cx/p.depth*focal);v.y=float(center_y-p.cy/p.depth*focal);
                v.u=float((sp->x+(p.u-sp->x)*a.uvScaleX+double(a.u)*entry.width)/tex.w);v.v=float((sp->y+(p.v-sp->y)*a.uvScaleY+double(a.v)*entry.height)/tex.h);
                v.r=color.r;v.g=color.g;v.b=color.b;v.a=color.a;v.depth=float(p.depth);
                // Original device setup selects linear vertex fog on view Z (0x451326..0x45134f).
                v.fog=no_fog?0.f:float(std::clamp((p.depth-fog_start)/(fog_end-fog_start),0.,1.));
                left&=v.x<32;right&=v.x>416;above&=v.y<16;below&=v.y>464;vs.push_back(v);
            }
            if(left||right||above||below)continue;
            for(size_t i=1;i+1<vs.size();++i){Triangle t{object.layer,sp->entry,a.blend,bool(a.flags2&2),{vs[0],vs[i],vs[i+1]},vs[0].depth+vs[i].depth+vs[i+1].depth};triangles.push_back(t);}
        }
    }
    // STD tiles share a plane; sorting also makes their few z offsets stable.
    std::stable_sort(triangles.begin(),triangles.end(),[](const Triangle& a,const Triangle& b){return a.layer!=b.layer?a.layer<b.layer:a.depth>b.depth;});
}

void Scene::capture(const Options& o){
    using namespace th12;
    poses.clear();texts.clear();dialogues.clear();
    const float* raw=th12_hud();std::copy_n(raw,hud.size(),hud.begin());
    const i32 stored=th12_score_stored();hud[3]=float(double(stored)*10);hud[40]=hud[3];
    if(o.score_low_digit){hud[3]+=float(o.score_low_digit);hud[40]+=float(o.score_low_digit);}
    // HI SCORE: the score file's best for this loadout, difficulty (or Practice stage), until beaten.
    if(o.high_score>double(hud[40]))hud[40]=float(o.high_score);
    const i32 frame=i32(hud[0]);if(frame<last_frame)reset_presentation();
    previous_motion.swap(current_motion);current_motion.clear();if(frame!=last_frame+1)previous_motion.clear();
    last_frame=frame;sampler_.begin_frame();
    capture_background();
    // Logical VM poses (128-byte ABI) and their draw schedule.
    const i32 count=th12_anm_draw();const auto* logical=static_cast<const AnimationPose*>(th12_anm_draw_ptr());
    const i32 meshes=th12_mesh_draw();
    owners.clear();owners.reserve(size_t(count+meshes));
    logical_poses.clear();logical_poses.reserve(size_t(count));
    for(i32 i=0;i<count;++i){
        const auto& r=logical[i];Pose p;auto& a=p.a;
        a.bank=r.bank;a.sprite=r.sprite;a.layer=r.layer;a.mode=r.mode;a.anchor_x=r.anchorX;a.anchor_y=r.anchorY;a.blend=r.blend;a.geometry_count=r.segments;
        a.x=0;a.y=0;a.z=r.z;a.rx=r.rx;a.ry=r.ry;a.rotation=r.rotation;a.sx=r.sx;a.sy=r.sy;a.u=r.u;a.v=r.v;a.uv_scale_x=r.uvScaleX;a.uv_scale_y=r.uvScaleY;
        a.geometry_radius=r.radius;a.geometry_width=r.width;
        const Color primary=rgba_of(r.primary),secondary=rgba_of(r.secondary),color=r.flags&0x10000?secondary:primary;
        a.r=color.r;a.g=color.g;a.b=color.b;a.alpha=color.a;a.has_primary=true;a.primary[0]=primary.r;a.primary[1]=primary.g;a.primary[2]=primary.b;a.primary[3]=primary.a;
        a.r2=secondary.r;a.g2=secondary.g;a.b2=secondary.b;a.alpha2=secondary.a;a.point=r.flags2&2;a.rotate=r.mode==1||r.mode==3;
        if(r.reserved[2]==1){std::memcpy(&a.native_width,&r.reserved[0],4);std::memcpy(&a.native_height,&r.reserved[1],4);}
        a.visible=(r.flags&3)==3;
        // Original 45c93e/45c95b tests primary alpha even when flag10000 selects secondary.
        if((a.mode==0||a.mode==1)&&primary.a==0)a.visible=false;
        p.x=r.x;p.y=r.y;p.alpha=r.alpha;p.id=r.id;p.logical=true;p.has_order=true;p.order=i;p.continuity=(u64(1)<<63)|r.id;
        owners.push_back(r.owner);logical_poses.push_back(p);
    }
    const auto* mesh_records=meshDrawState.data();
    for(i32 i=0;i<meshes;++i)owners.push_back(mesh_records[i].owner);
    std::sort(owners.begin(),owners.end());
    const auto owned=[&](u32 id){return std::binary_search(owners.begin(),owners.end(),id);};
    const auto find_logical=[&](i32 bank,i32 sprite_id)->const Pose*{for(const auto& p:logical_poses)if(p.a.bank==bank&&p.a.sprite==sprite_id)return &p;return nullptr;};
    // Logical poses, without the text VMs replaced by dialogue lines.
    for(const auto& p:logical_poses){if(p.a.bank==BankText&&((p.a.sprite>=0&&p.a.sprite<4)||p.a.sprite==7))continue;poses.push_back(p);}
    // Dialogue lines: the text VMs 0..3 carry the MSG text records.
    for(const auto& p:logical_poses){
        if(p.a.bank!=BankText||p.a.sprite<0||p.a.sprite>=4||!p.a.visible)continue;
        const auto& record=messageVM.textRecords[size_t(p.a.sprite)];if(record.bytes.empty())continue;
        const AnmSprite* sp=sprite(BankText,p.a.sprite);
        Dialogue d;d.bytes=record.bytes;d.text=decode_cp932(record.bytes);d.offset=record.offset;d.spacing=record.spacing;d.style=record.style;d.color=record.color;
        d.width=sp?sp->w:354;d.height=sp?sp->h:17;d.source_width=std::min<u32>(1024,u32(2*d.width+22));d.source_height=36;
        Pose q=p;q.kind=Pose::DialogueLine;q.item=u32(dialogues.size());dialogues.push_back(std::move(d));poses.push_back(q);
    }
    for(i32 i=0;i<meshes;++i){Pose p;p.kind=Pose::Mesh;p.item=u32(i);const auto& m=mesh_records[i];p.a.bank=m.bank;p.a.sprite=m.sprite;p.a.layer=m.layer;p.a.blend=m.blend;
        p.id=m.nodeId;p.mesh_pass0=m.pass==0;p.has_priority=true;p.priority=m.drawPriority;p.has_order=true;p.order=i;poses.push_back(p);}
    // Spell card: title line on text sprite 7, bonus and history numbers.
    const auto& spell=spellState;
    if(!spell.title.empty()){
        const std::string bytes=spell.title.substr(0,spell.title.find('\0'));
        if(!bytes.empty())for(const auto& p:logical_poses){
            if(p.a.bank!=BankText||p.a.sprite!=7||!p.a.visible||p.a.alpha<=0)continue;
            // 460800 right aligns by CP932 byte count before 44e660 doubles x.
            const auto original=decode_cp932(bytes,false);
            Dialogue d;d.bytes=bytes;d.text=translate_text(original);d.localized_spell=d.text!=original;d.offset=320-8*i32(bytes.size());d.color=0xffffff;d.width=320;d.height=17;d.source_width=666;d.source_height=36;
            d.raster=true;d.source_x=float(2*d.offset);d.source_y=2;d.foreground=0xffffff;
            Pose q=p;q.kind=Pose::DialogueLine;q.item=u32(dialogues.size());dialogues.push_back(std::move(d));poses.push_back(q);
        }
    }
    const auto push_text=[&](TextDescriptor t,i32 priority,i32 clip,i8 clipped){
        Pose p;p.kind=Pose::NativeText;p.item=u32(texts.size());texts.push_back(std::move(t));p.has_priority=true;p.priority=priority;p.clip=clip;p.clipped=clipped;
        p.has_order=true;p.order=i32(poses.size());poses.push_back(p);
    };
    if(spell.flags&1){
        // Native looks up ascii2 (sprite 261); both strings use its alpha.
        if(const Pose* history=find_logical(BankAscii,261)){
            const u32 alpha=u32(std::clamp<long>(std::lround(history->a.alpha*255),0,255)),color=(alpha<<24)|0xffffff;
            push_text(spell.flags&2?font_text(width_integer(spell.bonusDisplayed,8),2,261,51,color,1):font_text("$",2,275,51,color,1),45,-1,0);
            push_text(font_text(precision_integer(spell.loadoutRecord.captures,2)+"/"+precision_integer(spell.loadoutRecord.attempts,2),2,356,51,color,1),45,-1,0);
        }
    }
    // Boss timer hundredths (whole seconds are core VMs).
    if(bossHud.timerVisible){
        push_text(font_text(".",3,394,16,bossHud.timerColor,1),44,2,-1);
        push_text(font_text(precision_integer(bossHud.timerHundredths,2),3,402,22,bossHud.timerColor,1,.6f),44,2,-1);
    }
    // Spell result time, under the most recent front sprite 58 handle.
    if(spell.lastResult.kind!=spell_system::ResultKind::None){
        const Pose* time=nullptr;for(const auto& p:logical_poses)if(p.id==spell.resultAnimationId&&p.a.bank==BankFront&&p.a.sprite==58){time=&p;break;}
        if(time){
            const u32 alpha=u32(std::clamp<long>(std::lround(time->a.alpha*255),0,255)),white=(alpha<<24)|0xffffff;
            const i32 ticks=spell.lastResult.displayElapsedTicks,seconds=ticks/60,hundredths=(ticks%60)*100/60;
            push_text(font_text(width_integer(std::min(seconds,999),3)+".",3,222,192,white,1),44,2,-1);
            push_text(font_text(precision_integer(hundredths,2)+"s",3,266,198,white,1,.6f),44,2,-1);
            if(spell.lastResult.realTimeEncoded!=-1){const auto rt=decode_spell_real_time(spell.lastResult.realTimeEncoded);const u32 gray=(alpha<<24)|0x808080;
                push_text(font_text(width_integer(rt[0],3)+".",3,222,208,gray,1),44,2,-1);
                push_text(font_text(precision_integer(rt[1],2)+"s",3,266,214,gray,1,.6f),44,2,-1);}
        }
    }
    // UFO panel and reward texts (44a4f0/44a5d0); 44a870 at 60, 44a880 at 22.
    {
        const auto& u=ufoManager;
        const auto ufo_text=[&](TextDescriptor t){const i32 priority=t.font==3||(t.font==2&&t.draw_flag==0)?60:22;push_text(std::move(t),priority,priority==22?2:0,-1);};
        if(u.enemyId&&u.age>=20){
            ufo_text(font_text(width_integer(u.bucketA,3),3,512,260,0xffffffff,0));ufo_text(font_text(width_integer(u.bucketB,3),3,512,280,0xffffffff,0));
            ufo_text(font_text(width_integer(i64(std::trunc(double(u.fill)*100)),3)+"%",2,512,310,0xffffffff,0));
        }
        if(u.rewardDisplayFrames>0){
            TextDescriptor t=font_text("*"+fixed1(u.rewardDisplayMultiplier),4,u.rewardDisplayPosition.x,u.rewardDisplayPosition.y,u.rewardDisplayFrames%10<5?0xffffff00:0xff808080,1,2);
            t.align_x=t.align_y=0;ufo_text(t);
            if(u.rewardDisplayScore>0){TextDescriptor s=font_text(grouped_number(u.rewardDisplayScore),4,u.rewardDisplayPosition.x,f32(double(u.rewardDisplayPosition.y)+32),0xffffffff,1);s.align_x=s.align_y=0;ufo_text(s);}
        }
        if(u.enemyId&&u.age>=60)for(const auto& e:enemies)if(e->active&&e->id==u.enemyId){
            // 44a7b6 stores seconds to f32 before both truncations.
            const float seconds=f32((u.duration-u.age)/60.0);const i64 whole=i64(std::trunc(seconds)),fraction=i64(std::trunc(double(seconds)*100))%100;
            TextDescriptor t=font_text(width_integer(whole,2)+":"+precision_integer(fraction,2),2,f32(double(f32(double(e->x)+224))-12),f32(double(f32(double(e->y)+16))+20),0xc080ffc0,1);t.z=e->z;ufo_text(t);break;
        }
    }
    // Live FPS (41cc80): %2.1ffps at 588,470.
    if(o.current_fps>=0){
        const float fps=f32(o.current_fps);const std::string t=fixed1(double(fps)+.05)+"fps";
        push_text(font_text(t,1,f32(588.0-std::max<double>(0,double(t.size())-7)*7),470,fps<=30?0xff5050ff:fps<40?0xffa0a0ff:0xffffffff,0),69,0,-1);
    }
    if(o.stage_clear)if(const Pose* clear=[&]()->const Pose*{for(const auto& p:logical_poses)if(p.a.bank==BankFront&&p.a.sprite==70&&p.a.visible)return &p;return nullptr;}()){
        // 421350 binds front85; 41f040 borrows its alpha for the reward digits.
        const u32 alpha=u32(std::clamp<long>(std::lround(clear->a.alpha*255),0,255));
        std::string award=grouped_number(double(stage_clear_award(hud,o.practice)));
        TextDescriptor t=font_text(award,3,224,200,(alpha<<24)|0xffffff,0);t.align_x=t.align_y=0;push_text(t,69,0,-1);
    }
    // Stage fade (4057e0 -> 452a60): black field rectangle at draw 11.
    {
        const auto& f=fullGameFlow;bool active=false,out=false;i32 age=0;
        if(f.status==FullGameStatus::StageClear&&f.nextStage&&f.clearAge>=f.clearDelay){active=out=true;age=f.clearAge-f.clearDelay;}
        else if(f.status==FullGameStatus::NextStage){active=out=true;age=30;}
        else if(f.status==FullGameStatus::Playing&&f.entryFadeAge<30){active=true;age=f.entryFadeAge;}
        if(active){age=std::clamp(age,0,30);const u32 alpha=u32(out?255*age/30:255-255*age/30);
            Pose p;p.kind=Pose::Rectangle;p.bounds[0]=32;p.bounds[1]=16;p.bounds[2]=416;p.bounds[3]=464;p.argb=alpha<<24;p.has_priority=true;p.priority=11;p.clip=1;p.has_order=true;p.order=i32(poses.size());poses.push_back(p);}
    }
    // Legacy sprite records: bullets, items, options, the player and bomb parts.
    const i32 records=th12_draw();const auto* r=static_cast<const Sprite*>(th12_draw_ptr());
    const i32 player=BankPlayer+std::clamp(i32(hud[17]),0,2);
    for(i32 i=0;i<records;++i){
        const auto& s=r[i];if(owned(s.entityId))continue;
        SampleOptions sampling;sampling.entity=s.entityId;sampling.seed=s.entityId?(s.entityId&65535):0x1234;sampling.interrupt=s.interrupt;
        const i32 priority=s.kind==1?31:s.kind==3?27:s.kind==4?24:-1;const i8 clipped=priority>=0?1:-1;
        if(s.kind==12||s.kind==13){Pose p;p.kind=s.kind==12?Pose::BeamTrail:Pose::Trail;p.a.layer=s.kind;p.x=s.x;p.y=s.y;p.angle=s.rotation;p.scale=s.scale;
            p.width=s.alpha;p.alpha=1;p.start=u32(s.color);p.end=u32(s.animation);p.has_order=true;p.order=i32(poses.size());poses.push_back(p);}
        else if(s.kind==4||s.kind==5||s.kind==2||s.kind==7)add(s.bank>=4&&s.bank<=6?s.bank:player,s.animation,s.age,s.x,s.y,s.rotation,s.scale,std::nullopt,sampling,s.alpha,priority,clipped);
        else if(s.kind==6){if(const auto* f=sampler_.sample(s.bank>=4&&s.bank<=6?s.bank:player,s.animation,s.age,std::nullopt,sampling)){Pose p;p.kind=Pose::Beam;p.a=f->a;p.x=s.x;p.y=s.y;p.angle=s.rotation;p.scale=s.scale;
            p.alpha=s.alpha;p.has_order=true;p.order=i32(poses.size());poses.push_back(p);}}
        else if(s.kind==1){const i32 type=s.color/16;if(type>=0&&type<31){const auto& c=exe.bullets[size_t(type)].colors[size_t(s.color%16)];
            add(BankBullet,s.animation,s.age,s.x,s.y,s.rotation,s.scale,BulletVariant{c.sprite,c.spawn,c.cancel},sampling,s.alpha,priority,clipped);}}
        else if(s.kind==3)add(BankBullet,s.animation>=165?s.animation:166+s.color,s.age,s.x,s.y,s.rotation,s.scale,std::nullopt,sampling,s.alpha,priority,clipped);
        else if(s.bank>=0&&s.bank<=3&&banks.get(s.bank))add(s.bank,s.animation,s.age,s.x,s.y,s.rotation,s.scale,std::nullopt,sampling,s.alpha,priority,clipped);
    }
    // HUD: the front 0 frame, boss bar, counters, life/bomb pieces, UFO stock and difficulty.
    if(runtime_options.always_hitbox&&th12_player_state()==1&&!std::binary_search(owners.begin(),owners.end(),0xffff0020u)){
        SampleOptions marker;marker.entity=0xffff0020;marker.seed=0x1234;
        add(BankBullet,77,std::max(18,frame),hud[1]+224,hud[2]+16,0,1,std::nullopt,marker,1,34,1);
    }
    add(BankFront,0,o.hud_age>=0?o.hud_age:frame,0,0,0,1,std::nullopt,{},1,-1,0);
    if(bossHud.fill>0){
        // Native 41f350..41f439 uses solid rectangles, not an ANM texture.
        const float right=f32(330.0*bossHud.fill+41);
        const auto rect=[&](float l,float t,float rr,float b,u32 argb){Pose p;p.kind=Pose::Rectangle;p.bounds[0]=l;p.bounds[1]=t;p.bounds[2]=rr;p.bounds[3]=b;p.argb=argb;p.has_priority=true;p.priority=44;p.clip=2;p.has_order=true;p.order=i32(poses.size());poses.push_back(p);};
        rect(41,23,right,25,0xff000000);rect(40,22,f32(double(right)-1),24,0xffffffff);
        for(int i=0;i<4;++i)if(bossHud.markers[size_t(i)]!=0)rect(40,22,f32(330.0*std::min(bossHud.markers[size_t(i)],bossHud.fill)+40),24,bossHud.colors[size_t(i)]);
    }
    {Pose p;p.kind=Pose::HudCounters;p.has_priority=true;p.priority=69;p.clipped=0;p.has_order=true;p.order=i32(poses.size());poses.push_back(p);}
    for(i32 i=0;i<8;++i){
        SampleOptions life;life.interrupt=fragment_interrupt(i,i32(hud[5]),i32(hud[21]));add(BankFront,13+i,frame,0,0,0,1,std::nullopt,life,1,59,0);
        SampleOptions bomb;bomb.interrupt=fragment_interrupt(i,i32(hud[6]),i32(hud[22]));add(BankFront,21+i,frame,0,0,0,1,std::nullopt,bomb,1,59,0);
    }
    bool stock_vm=false;for(i32 i=0;i<count;++i)if(logical[i].bank==BankFront&&(logical[i].owner>>16)==0xfffe){stock_vm=true;break;}
    if(!stock_vm)for(i32 i=0;i<3;++i){const i32 color=i32(hud[23+size_t(i)]);SampleOptions s;s.interrupts={7+i,color?9+color:13,color?2:3};add(BankFront,80,frame,0,0,0,1,std::nullopt,s,1,44,1);}
    add(BankFront,69+i32(hud[19]),frame,0,0,0,1,std::nullopt,{},1,-1,0);
    order();sampler_.end_frame();
    if(!runtime_options.limit_presentation){
        current_motion.reserve(poses.size());
        for(const auto& p:poses)if(p.continuity&&p.kind==Pose::State&&p.priority<42&&p.a.mode<4&&p.a.bank!=BankStage)
            current_motion.emplace_back(p.continuity,Motion{p.x,p.y,p.a.x,p.a.y,p.a.rotation,p.a.bank,p.a.mode});
        std::sort(current_motion.begin(),current_motion.end(),[](const auto& a,const auto& b){return a.first<b.first;});
    }
}

void Scene::order(){
    using namespace th12;
    // orderPresentation: schedule metadata for logical VMs and pass-0 meshes.
    const i32 count=th12_anm_schedule_count();const auto* schedule=static_cast<const AnimationDrawSchedule*>(th12_anm_schedule_ptr());
    // Schedule entries by id (the first entry for an id wins), without a hash map per frame.
    schedule_ids.clear();for(i32 i=0;i<count;++i)schedule_ids.emplace_back(schedule[i].id,u32(i));std::sort(schedule_ids.begin(),schedule_ids.end());
    const auto by_id=[&](u32 id)->const AnimationDrawSchedule*{const auto it=std::lower_bound(schedule_ids.begin(),schedule_ids.end(),std::pair<u32,u32>{id,0});
        return it!=schedule_ids.end()&&it->first==id?&schedule[it->second]:nullptr;};
    sorted.clear();sorted.reserve(poses.size());
    for(u32 index=0;index<poses.size();++index){
        Pose& p=poses[index];const AnimationDrawSchedule* m=nullptr;
        if(p.logical||p.mesh_pass0)m=by_id(p.id);
        const i32 priority=m?m->priority:p.mesh_pass0?layer_priority(p.a.layer):p.has_priority?p.priority:layer_priority(p.a.layer);
        const i32 clip=m?i32(m->clip):p.clip>=0?p.clip:p.clipped>=0?(p.clipped?2:0):priority<43?2:0;
        if(!(priority>=0))continue;
        p.priority=priority;p.order=m?i32(m->order):p.has_order?p.order:i32(index);p.index=index;p.clip=clip;sorted.push_back(index);
    }
    // Indices, not ~250-byte poses: dense patterns put thousands here.
    std::sort(sorted.begin(),sorted.end(),[this](u32 x,u32 y){const Pose& a=poses[x];const Pose& b=poses[y];return a.priority!=b.priority?a.priority<b.priority:a.order!=b.order?a.order<b.order:a.index<b.index;});
}

void Scene::draw_state(const Appearance& a,float x,float y,float angle,float scale,float alpha,const float* shake_){
    if(!a.visible)return;
    if(a.mode==18||a.mode==20){
        // Native mode18 (opcode108): untextured gradient quad; right vertices primary, left secondary.
        const Color right=a.has_primary?Color{a.primary[0],a.primary[1],a.primary[2],a.primary[3]}:Color{a.r,a.g,a.b,a.alpha},left{a.r2,a.g2,a.b2,a.alpha2};
        const auto c=quad_corners(x+a.x,y+a.y,a.geometry_radius*a.sx*scale,a.geometry_width*a.sy*scale,a.rotation+angle,0,0,false);
        const Color k[4]{left,right,right,left};Vertex v[4];for(int i=0;i<4;++i)v[i]={c[i][0],c[i][1],0,0,k[i].r,k[i].g,k[i].b,k[i].a*alpha,1,0};
        static constexpr u32 idx[6]{0,1,2,0,2,3};gpu.solid_triangles(v,idx,6,a.blend);return;
    }
    if(a.mode==14){if(a.alpha<=0)return;const auto c=quad_corners(x+a.x,y+a.y,a.geometry_radius*a.sx*scale,a.geometry_width*a.sy*scale,a.rotation+angle,0,0,false);
        Vertex v[4];for(int i=0;i<4;++i)v[i]={c[i][0],c[i][1],0,0,a.r,a.g,a.b,a.alpha*alpha,1,0};static constexpr u32 idx[6]{0,1,2,0,2,3};gpu.solid_triangles(v,idx,6,a.blend);return;}
    if(a.mode==15||a.mode==16){
        const i32 n=a.geometry_count;if(n<3||n>512)return;
        const Color primary=a.has_primary?Color{a.primary[0],a.primary[1],a.primary[2],a.primary[3]}:Color{a.r,a.g,a.b,a.alpha};
        const Color center{primary.r,primary.g,primary.b,primary.a*alpha},edge{a.r2,a.g2,a.b2,a.alpha2*alpha};
        const float step=f32(6.283185307179586/n);float an=f32(a.rotation+angle);const float cx=x+a.x,cy=y+a.y;
        constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;
        if(a.mode==16){
            // Native 45d710: a diffuse line strip with count+1 perimeter points.
            if(center.a<=0||a.geometry_radius<=0)return;const float radius=f32(double(a.geometry_radius)*a.sx*scale),step16=f32(double(tau)/n);
            gpu.material(a.blend);gpu.bind(gpu.solid(),true);gpu.primitive(Renderer::LineStrip);gpu.flush();
            for(i32 i=0;i<=n;++i){gpu.push({f32(double(cx)+f32(std::cos(double(an))*radius)),f32(double(cy)+f32(std::sin(double(an))*radius)),.5f,.5f,center.r,center.g,center.b,center.a,1,0});
                an=f32(double(an)+step16);while(an>pi)an=f32(double(an)-tau);while(an<-pi)an=f32(double(an)+tau);}
            gpu.flush();return;
        }
        if(center.a<=0&&edge.a<=0)return;
        // Native 45d430: a fan, primary at the centre, secondary on count+1 rim points.
        const float radius=a.geometry_radius*a.sx*scale;std::vector<Vertex> v;v.push_back({cx,cy,0,0,center.r,center.g,center.b,center.a,1,0});
        for(i32 i=0;i<=n;++i){v.push_back({f32(double(cx)+f32(std::cos(double(an))*radius)),f32(double(cy)+f32(std::sin(double(an))*radius)),0,0,edge.r,edge.g,edge.b,edge.a,1,0});
            an=f32(double(an)+step);while(an>pi)an=f32(double(an)-tau);while(an<-pi)an=f32(double(an)+tau);}
        std::vector<u32> idx;for(i32 i=1;i<=n;++i){idx.push_back(0);idx.push_back(u32(i));idx.push_back(u32(i+1));}
        gpu.solid_triangles(v.data(),idx.data(),u32(idx.size()),a.blend);return;
    }
    const AnmSprite* s=sprite(a.bank,a.sprite);if(!s||a.alpha<=0)return;
    const Texture t=texture(a.bank,s->entry);
    Renderer::RegionOptions o;o.anchor_x=a.anchor_x;o.anchor_y=a.anchor_y;o.mode=a.mode;o.point=a.point;o.u=a.u;o.v=a.v;o.uv_scale_x=a.uv_scale_x;o.uv_scale_y=a.uv_scale_y;o.shake=shake_;
    gpu.region(t,s->x,s->y,s->w,s->h,x+a.x,y+a.y,(a.native_width!=-1?a.native_width:s->w)*scale*a.sx,(a.native_height!=-1?a.native_height:s->h)*scale*a.sy,
        a.rotate?a.rotation+angle:0,a.alpha*alpha,{a.r,a.g,a.b,1},a.blend,o);
}

void Scene::render_text(const TextDescriptor& d,const float* shake_){
    static thread_local std::vector<Glyph> glyphs;ascii_glyphs(d,glyphs);
    for(const auto& g:glyphs){const AnmSprite* s=sprite(BankAscii,g.id);if(!s)continue;const Color c=rgba_of(g.color);
        Renderer::RegionOptions o;o.anchor_x=o.anchor_y=1;o.point=g.point;o.pixel=true;o.mode=0;o.shake=shake_;
        gpu.region(texture(BankAscii,s->entry),s->x,s->y,s->w,s->h,g.x,g.y,s->w*g.sx,s->h*g.sy,0,c.a,c,0,o);}
}
void Scene::hud_text(const std::string& text,float x,float y,float scale,bool right){
    // Original ASCII font 3 (0x401d97): 12px numeral advance, 4px comma, comma 3px lower.
    const auto advance=[&](char c){return (c==','?4.f:12.f)*scale;};
    if(right)for(char c:text)x-=advance(c);
    for(char c:text){const i32 id=c==','?257:c=='/'?253:c=='.'?254:c=='s'?255:c=='*'?256:u8(c)+195;
        if(const AnmSprite* s=sprite(BankAscii,id)){Renderer::RegionOptions o;o.anchor_x=o.anchor_y=1;o.point=true;
            gpu.region(texture(BankAscii,s->entry),s->x,s->y,s->w,s->h,x,y+(c==','?3:0),s->w*scale,s->h*scale,0,1,{1,1,1,1},0,o);}
        x+=advance(c);}
}
Texture Scene::dialogue_texture(const Dialogue& d){
    auto& cache=d.menu?menu_cache:dialogue_cache;
    std::string key(reinterpret_cast<const char*>(&d.offset),sizeof(i32)*3);key+=d.bytes;key+='|';
    key.append(reinterpret_cast<const char*>(d.text.data()),d.text.size()*sizeof(char32_t));key+='|';
    for(const float v:{d.width,d.height,d.source_x,d.source_y})key.append(reinterpret_cast<const char*>(&v),4);
    for(const u32 v:{d.color,d.source_width,d.source_height,d.foreground,u32(d.raster),u32(d.localized_spell)})key.append(reinterpret_cast<const char*>(&v),4);
    for(auto it=cache.begin();it!=cache.end();++it)if(it->key==key){cache.splice(cache.begin(),cache,it);return cache.front().texture;}
    // 44e56f..44e5c6: 32px MS Gothic (400) / MS Mincho (600); 15px bold for style 2.
    const bool serif=d.style==1;const u32 weight=d.raster?400:d.style==2?700:serif?600:400;const float size=d.raster?32:d.style==2?15:32;
    const u32 rgb=d.raster?d.foreground:((d.color&255)<<16|((d.color>>8)&255)<<8|((d.color>>16)&255));
    Canvas canvas;canvas.reset(d.source_width,d.source_height);
    const auto draw=[&](const std::u32string& text,float x,float scale_x=1){
        const float y=d.raster?d.source_y:2;
        for(const auto& off:d.outline)raster.fill(canvas,text,x+off[0]*scale_x,y+off[1]-2,serif,weight,size,0x000000,0,scale_x);
        raster.fill(canvas,text,x,y,serif,weight,size,rgb,0,scale_x);
    };
    if(d.localized_spell){
        // Match the Web text layout: right align translated titles and squeeze
        // only their horizontal extent when they exceed the 640px title line.
        const float available=float(d.source_width)-26;
        float scale_x=1,width=raster.measure(d.text,serif,weight,size);
        for(i32 i=0;i<8&&width>available;++i){
            scale_x*=std::min(.99f,available/width);
            width=raster.measure(d.text,serif,weight,size,scale_x);
        }
        draw(d.text,std::max(0.f,available-width),scale_x);
    }
    else if(d.spacing){for(size_t i=0;i<d.bytes.size();i+=2)draw(decode_cp932(d.bytes.substr(i,2)),float(2*d.offset+i64(i)*d.spacing));}
    else draw(d.text,d.raster?d.source_x:float(2*d.offset+2));
    Texture t;t.name=gpu.create_texture(canvas.width,canvas.height,canvas.rgba.data(),true);t.w=float(canvas.width);t.h=float(canvas.height);t.repeat=false;
    cache.push_front({key,t});
    if(cache.size()>(d.menu?96u:16u)){gpu.delete_texture(cache.back().texture.name);cache.pop_back();}
    return t;
}
void Scene::draw_dialogue(const Pose& p,const float* shake_){draw_text_quad(dialogues[p.item],p.a,p.x,p.y,p.alpha,shake_);}
void Scene::draw_text_quad(const Dialogue& d,const Appearance& a,float x,float y,float alpha,const float* shake_){
    if(d.text.empty()||a.alpha<=0)return;
    const Texture t=dialogue_texture(d);gpu.material(a.blend);gpu.bind(t,a.point);gpu.primitive(Renderer::Triangles);
    const Corners c=a.mode==0||a.mode==1?native_quad_corners(x,y,d.width*a.sx,d.height*a.sy,a.rotation,a.anchor_x,a.anchor_y,a.mode,shake_):quad_corners(x,y,d.width*a.sx,d.height*a.sy,a.rotation,a.anchor_x,a.anchor_y,false);
    gpu.quad(c,0,1,0,1,false,{a.r,a.g,a.b,a.alpha*alpha});
}
void Scene::draw_mesh(const Pose& p,const float* shake_){
    using namespace th12;const auto& m=meshDrawState[p.item];const bool untextured=m.bank==-1&&m.sprite==-1;
    const Texture t=untextured?gpu.solid():texture(m.bank,m.textureEntry);if(!t.name||!t.available)return;
    gpu.material(m.blend);gpu.bind(t,untextured||m.filter==1,i32(m.addressFlags));
    gpu.primitive(m.primitive==3?Renderer::LineStrip:Renderer::Triangles);
    const float ox=shake_?shake_[0]:0,oy=shake_?shake_[1]:0;const bool shift=m.primitive==4&&!untextured&&(ox||oy);
    const auto emit=[&](const custom_geometry::Vertex& v){const Color c=rgba_of(v.argb);
        gpu.push({shift?f32(double(v.x)+ox):v.x,shift?f32(double(v.y)+oy):v.y,untextured?.5f:v.u,untextured?.5f:(t.flip_y?1-v.v:v.v),c.r,c.g,c.b,c.a,v.rhw?1/v.rhw:1,0});};
    const auto* v=&meshVertexState[m.vertexOffset];const u32 n=m.vertexCount;
    // Native 45d5d0 submits each trail separately.
    if(m.primitive==3){gpu.flush();for(u32 i=0;i<n;++i)emit(v[i]);gpu.flush();return;}
    if(m.primitive==4){for(u32 i=0;i<n;++i)emit(v[i]);return;}
    for(u32 i=2;i<n;++i){if(i&1){emit(v[i-1]);emit(v[i-2]);emit(v[i]);}else{emit(v[i-2]);emit(v[i-1]);emit(v[i]);}}
}
void Scene::stage_surface(i32 priority,i32 clip,bool& copied,bool& composited){
    // 42f200 ends the background pass at draw 12; 42f3b0 copies it at 14.
    if(priority>=12&&!copied){
        gpu.copy_stage_surface();copied=true;if(!gpu.stage_surface().available)return;
        // 42f200 clears the destination target before any layer5/prio13 draw.
        gpu.flush();gpu.clip(2);gpu.clear({0,0,0,1});gpu.clip(clip);
    }
    if(priority>=14&&!composited){
        composited=true;if(!gpu.stage_surface().available)return;
        // Main script 81's held pose copies sprite 75 with blend 3.
        Renderer::RegionOptions o;o.anchor_x=o.anchor_y=1;o.mode=0;o.shake=shake.data();
        gpu.region(gpu.stage_surface(),32,16,384,448,32,16,384,448,0,1,{1,1,1,1},3,o);gpu.flush();
    }
}

void Scene::render(float alpha){
    gpu.begin();gpu.clip(1);gpu.fog_color(fog.r,fog.g,fog.b);gpu.clear(fog);
    gpu.primitive(Renderer::Triangles);
    for(const auto& t:triangles){const Texture tex=texture(BankStage,t.entry);if(!tex.name)continue;gpu.material(t.blend);gpu.bind(tex,t.point);for(const auto& v:t.v)gpu.push(v);}
    gpu.flush();
    i32 current_clip=1;bool copied=false,composited=false;const float rounded[2]{f32(shake[0]),f32(shake[1])};const bool shaking=rounded[0]||rounded[1];
    for(const u32 at:sorted){
        const Pose& captured=poses[at];Pose interpolated;const Pose* presented=&captured;
        if(alpha<1&&captured.continuity&&captured.kind==Pose::State){
            const auto old=std::lower_bound(previous_motion.begin(),previous_motion.end(),captured.continuity,[](const auto& item,u64 key){return item.first<key;});
            if(old!=previous_motion.end()&&old->first==captured.continuity){
                const auto& m=old->second;const auto& p=captured;
                // Identity is persistent; jumps such as Murasa's teleport use the
                // destination immediately. Presentation never writes core state.
                const float dx=p.x+p.a.x-m.x-m.ax,dy=p.y+p.a.y-m.y-m.ay;
                if(m.bank==p.a.bank&&m.mode==p.a.mode&&dx*dx+dy*dy<4096){
                    const auto mix=[alpha](float a,float b){return a+(b-a)*alpha;};
                    interpolated=p;interpolated.x=mix(m.x,p.x);interpolated.y=mix(m.y,p.y);
                    interpolated.a.x=mix(m.ax,p.a.x);interpolated.a.y=mix(m.ay,p.a.y);
                    interpolated.a.rotation=m.rotation+std::remainder(p.a.rotation-m.rotation,6.283185307179586f)*alpha;
                    presented=&interpolated;
                }
            }
        }
        const Pose& original=*presented;
        if(has_surface)stage_surface(original.priority,current_clip,copied,composited);
        const Pose* use=&original;const float* pose_shake=nullptr;
        if(shaking&&original.priority<42&&original.kind!=Pose::Mesh){
            // Native sprite corners apply shake at submission (modes 0/1, font
            // text); shakePresentation moves other positioned poses, except
            // modes >= 14, which submit already formed vertices.
            const bool appearance=original.kind==Pose::State||original.kind==Pose::Beam||original.kind==Pose::DialogueLine;
            if(original.kind==Pose::NativeText||(appearance&&(original.a.mode==0||original.a.mode==1)))pose_shake=rounded;
            else if((appearance&&original.a.mode<14)||original.kind==Pose::Trail||original.kind==Pose::BeamTrail){
                shaken=original;shaken.x=f32(double(original.x)+rounded[0]);shaken.y=f32(double(original.y)+rounded[1]);use=&shaken;}
        }
        const Pose& p=*use;
        if(p.clip!=current_clip){gpu.clip(p.clip);current_clip=p.clip;}
        switch(p.kind){
        case Pose::NativeText:render_text(texts[p.item],pose_shake);break;
        case Pose::HudCounters:{
            const auto& h=hud;hud_text(grouped_number(h[3]),620,72,1,true);hud_text(grouped_number(h[40]),620,48,1,true);
            hud_text(std::to_string(i64(std::floor(h[4]/100.0)))+".",540,152);hud_text(precision_integer(i64(std::round(double(h[4])))%100,2),560,159,.6f);
            hud_text("/4.",574,152);hud_text("00",606,159,.6f);
            hud_text(grouped_number(std::floor(h[20]/100.0/10)*10),620,176,1,true);hud_text(grouped_number(h[7]),620,200,1,true);break;}
        case Pose::Rectangle:{const Color c=rgba_of(p.argb);const float* b=p.bounds;
            Vertex v[4]{{b[0],b[1],0,0,c.r,c.g,c.b,c.a,1,0},{b[2],b[1],0,0,c.r,c.g,c.b,c.a,1,0},{b[2],b[3],0,0,c.r,c.g,c.b,c.a,1,0},{b[0],b[3],0,0,c.r,c.g,c.b,c.a,1,0}};
            static constexpr u32 idx[6]{0,1,2,0,2,3};gpu.solid_triangles(v,idx,6,0);break;}
        case Pose::Mesh:draw_mesh(p,p.priority<42?rounded:nullptr);break;
        case Pose::DialogueLine:draw_dialogue(p,pose_shake);break;
        case Pose::BeamTrail:{
            // Native profile3/script210: sprite 405, blend 1, edges offset 12px (46a0ee..46a153).
            const AnmSprite* s=sprite(BankBullet,405);if(!s)break;const Texture t=texture(BankBullet,s->entry);if(!t.name)break;
            gpu.material(1);gpu.bind(t,false);gpu.primitive(Renderer::Triangles);
            const double dx=std::cos(double(p.angle))*p.scale/2,dy=std::sin(double(p.angle))*p.scale/2,nx=-std::sin(double(p.angle))*p.width/2,ny=std::cos(double(p.angle))*p.width/2;
            const Color a=rgba_of(p.start),b=rgba_of(p.end);
            const float pts[4][2]{{float(p.x-dx-nx),float(p.y-dy-ny)},{float(p.x-dx+nx),float(p.y-dy+ny)},{float(p.x+dx+nx),float(p.y+dy+ny)},{float(p.x+dx-nx),float(p.y+dy-ny)}};
            const Color cs[4]{a,a,b,b};const float uv[4][2]{{s->x/t.w,0},{(s->x+s->w)/t.w,0},{(s->x+s->w)/t.w,1/32.f},{s->x/t.w,1/32.f}};
            for(int i:{0,1,2,0,2,3})gpu.push({pts[i][0],pts[i][1],uv[i][0],uv[i][1],cs[i].r,cs[i].g,cs[i].b,cs[i].a,1,0});break;}
        case Pose::Trail:{
            const double dx=std::cos(double(p.angle))*p.scale/2,dy=std::sin(double(p.angle))*p.scale/2;
            gpu.material(0);gpu.bind(gpu.solid(),true);gpu.primitive(Renderer::Lines);
            const Color a=argb_color(p.start,p.alpha),b=argb_color(p.end,p.alpha);
            gpu.push({float(p.x-dx),float(p.y-dy),.5f,.5f,a.r,a.g,a.b,a.a,1,0});gpu.push({float(p.x+dx),float(p.y+dy),.5f,.5f,b.r,b.g,b.b,b.a,1,0});break;}
        case Pose::Beam:{const AnmSprite* s=sprite(p.a.bank,p.a.sprite);if(!s)break;
            Renderer::RegionOptions o;o.mode=1;o.shake=pose_shake;
            gpu.region(texture(p.a.bank,s->entry),s->x,s->y,s->w,s->h,p.x,p.y-448*p.scale/2,448*p.scale,14*p.a.sy,-3.14159265358979/2,p.a.alpha*p.alpha,{p.a.r,p.a.g,p.a.b,1},p.a.blend,o);break;}
        default:draw_state(p.a,p.x,p.y,p.angle,p.scale,p.alpha,pose_shake);break;
        }
    }
    gpu.flush();gpu.clip(0);
}

void Scene::render_menu(const std::vector<Appearance>& list,float x,float y,bool overlay,float shade,const float* bounds,Color clear,const std::vector<TextDescriptor>* texts,float fps){
    if(!overlay)gpu.begin(clear);
    gpu.clip(0);gpu.fog_color(0,0,0);
    if(shade>0){const float b[4]{bounds?bounds[0]:0,bounds?bounds[1]:0,bounds?bounds[2]:640,bounds?bounds[3]:480};
        Vertex v[4]{{b[0],b[1],0,0,0,0,0,shade,1,0},{b[2],b[1],0,0,0,0,0,shade,1,0},{b[2],b[3],0,0,0,0,0,shade,1,0},{b[0],b[3],0,0,0,0,0,shade,1,0}};
        static constexpr u32 idx[6]{0,1,2,0,2,3};gpu.solid_triangles(v,idx,6,0);}
    // orderPresentation on their layer priorities, stable.
    std::vector<u32> order_;for(u32 i=0;i<list.size();++i)if(layer_priority(list[i].layer)>=0)order_.push_back(i);
    std::stable_sort(order_.begin(),order_.end(),[&](u32 a,u32 b){return layer_priority(list[a].layer)<layer_priority(list[b].layer);});
    for(u32 i:order_){
        const auto& a=list[i];
        if(!a.text){draw_state(a,x,y,0,1,1,nullptr);continue;}
        if(!a.visible||a.alpha<=0)continue;
        // A menu line: 32px MS Gothic at half size with a black outline.
        Dialogue d;d.text=a.text->text;d.menu=d.raster=true;d.width=a.text->width;d.height=a.text->height;d.source_width=u32(d.width*2);d.source_height=u32(d.height*2);
        d.source_x=0;d.source_y=0;d.foreground=a.text->color;const i32 outline[4][2]{{2,2},{-2,2},{2,-2},{-2,-2}};std::memcpy(d.outline,outline,sizeof outline);
        draw_text_quad(d,a,x+a.x,y+a.y,1,nullptr);
    }
    if(texts)for(const auto& t:*texts)render_text(t);
    if(fps>=0){
        // 41cc80: %2.1ffps at 588,470 (fps-presentation.mjs).
        const std::string t=fixed1(double(fps)+.05)+"fps";
        render_text(font_text(t,1,f32(588.0-std::max<double>(0,double(t.size())-7)*7),470,fps<=30?0xff5050ff:fps<40?0xffa0a0ff:0xffffffff,0));
    }
    gpu.flush();
}
void Scene::render_pause_backdrop(const std::vector<Appearance>& list,bool capture){
    gpu.flush();if(capture)gpu.capture_pause_field();gpu.clip(0);gpu.fog_color(0,0,0);
    // 432260 copies the invisible script 79 VM's RGB to the captured field's tint.
    for(const auto& a:list)if(a.script==79){
        Vertex v[4]{{32,16,0,0,a.r,a.g,a.b,1,1,0},{416,16,0,0,a.r,a.g,a.b,1,1,0},{416,464,0,0,a.r,a.g,a.b,1,1,0},{32,464,0,0,a.r,a.g,a.b,1,1,0}};
        static constexpr u32 idx[6]{0,1,2,0,2,3};gpu.solid_triangles(v,idx,6,5);break;}
    for(const auto& a:list)if(a.script!=79)draw_state(a,0,0,0,1,1,nullptr);
    gpu.flush();
}
}
