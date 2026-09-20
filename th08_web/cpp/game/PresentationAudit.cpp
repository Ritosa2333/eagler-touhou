#include "PresentationAudit.hpp"
#if defined(TH_PRESENTATION_AUDIT)
#include "AnmRenderer.hpp"
#include "Presentation.hpp"
#include "GraphicsMath.hpp"
#include "EffectState.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
namespace th08::audit {
namespace {
constexpr uint32_t capacity=8192;
std::vector<Record> records;
std::vector<Record> references[2];
uint32_t reference_ticks[2]{},reference_dropped[2]{};
bool enabled=false;
uint32_t owner=0,object=0,part=0,ordinal=0,scope_flags=0,tick=0,dropped=0,pass=0;
int32_t age=0; float alpha=1;
float channel(uint32_t color,unsigned shift){return float((color>>shift)&255)/255.f;}
}
Scope::Scope(Owner o,const void* p,int32_t a,uint32_t sub,uint32_t flags)
    :saved_owner(owner),saved_object(object),saved_part(part),saved_ordinal(ordinal),saved_flags(scope_flags),saved_age(age){
    owner=uint32_t(o);object=uint32_t(reinterpret_cast<uintptr_t>(p));part=sub;age=a;ordinal=0;scope_flags|=flags;
}
Scope::~Scope(){owner=saved_owner;object=saved_object;part=saved_part;ordinal=saved_ordinal;scope_flags=saved_flags;age=saved_age;}
void enable(bool value){enabled=value;records.clear();for(auto& r:references)r.clear();reference_ticks[0]=reference_ticks[1]=0;if(enabled){records.reserve(capacity);for(auto& r:references)r.reserve(capacity);}}
uint32_t tick_id(){return tick;}
void end_frame(){if(enabled&&!pass){references[0].swap(references[1]);references[1]=records;reference_ticks[0]=reference_ticks[1];reference_ticks[1]=tick;reference_dropped[0]=reference_dropped[1];reference_dropped[1]=dropped;}}
void begin_frame(bool only,float a){
    if(!only)++tick;
    if(!enabled)return;
    records.clear();dropped=0;pass=only?1:0;alpha=a;owner=object=part=ordinal=0;age=0;
}
void capture(const AnmVm& vm,const SpriteVertex* v,uint32_t n,uint32_t kind,const Vec2* screen_shake){
    if(!enabled||!v||!n)return;
    const uint32_t sub=ordinal++;
    if(records.size()>=capacity){++dropped;return;}
    Record r;
    const auto texture=vm.loadedSprite?vm.loadedSprite->texture:0;
    const uint32_t metadata[]={owner,object,part,sub,kind,uint32_t(int32_t(vm.anmFileIndex)),uint32_t(int32_t(vm.scriptIndex)),uint32_t(int32_t(vm.activeSpriteIndex)),uint32_t(age),texture,n,(!owner?1u:0u)|(n>16?2u:0u)|(scope_flags&DiscreteMotion?1u<<21:0u)};
    std::copy(metadata,metadata+12,r.meta);
    // Explicit color-channel ownership; the unused secondary color is not
    // evidence of a visible fade. Bit 19 distinguishes this from old captures.
    r.meta[11]|=1u<<19;if(vm.flag17)r.meta[11]|=1u<<18;
    // Independent evidence of authored continuous ANM operations. These bits
    // are not inferred from whether the presentation lerp happens to exist.
    const auto timed=[&](AnmInterp property){return vm.interpEndTimers[property].current>0&&vm.interpCurrentTimers[property].current<=vm.interpEndTimers[property].current;};
    if(timed(AnmInterp_Pos))r.meta[11]|=1u<<8;
    if(timed(AnmInterp_Scale)||vm.scaleGrowth.x!=0||vm.scaleGrowth.y!=0)r.meta[11]|=1u<<9;
    if(timed(AnmInterp_Rotate)||vm.angleVel.x!=0||vm.angleVel.y!=0||vm.angleVel.z!=0)r.meta[11]|=1u<<10;
    if(vm.uvScrollVel.x!=0||vm.uvScrollVel.y!=0)r.meta[11]|=1u<<11;
    if(timed(AnmInterp_RGB1))r.meta[11]|=1u<<12;
    if(timed(AnmInterp_Alpha1))r.meta[11]|=1u<<13;
    if(timed(AnmInterp_RGB2))r.meta[11]|=1u<<14;
    if(timed(AnmInterp_Alpha2))r.meta[11]|=1u<<15;
    const float fields[]={vm.pos.x,vm.pos.y,vm.pos.z,vm.pos2.x,vm.pos2.y,vm.pos2.z,
        vm.scale.x,vm.scale.y,vm.rotation.x,vm.rotation.y,vm.rotation.z,
        vm.uvScrollPos.x,vm.uvScrollPos.y,
        float(vm.color1.r)/255,float(vm.color1.g)/255,float(vm.color1.b)/255,float(vm.color1.a)/255,
        float(vm.color2.r)/255,float(vm.color2.g)/255,float(vm.color2.b)/255,float(vm.color2.a)/255};
    std::copy(fields,fields+21,r.values);
    float left=v[0].pos.x,right=left,top=v[0].pos.y,bottom=top,cx=0,cy=0,opacity=0;
    bool finite=true;
    for(uint32_t i=0;i<n;++i){const auto& p=v[i];left=std::min(left,p.pos.x);right=std::max(right,p.pos.x);top=std::min(top,p.pos.y);bottom=std::max(bottom,p.pos.y);cx+=p.pos.x;cy+=p.pos.y;opacity+=channel(p.color,24);finite&=std::isfinite(p.pos.x)&&std::isfinite(p.pos.y)&&std::isfinite(p.pos.z);}
    r.values[21]=left;r.values[22]=top;r.values[23]=right;r.values[24]=bottom;r.values[25]=cx/n;r.values[26]=cy/n;
    if(n>=3){r.values[27]=std::hypot(v[1].pos.x-v[0].pos.x,v[1].pos.y-v[0].pos.y);r.values[28]=std::hypot(v[2].pos.x-v[0].pos.x,v[2].pos.y-v[0].pos.y);r.values[29]=std::atan2(v[1].pos.y-v[0].pos.y,v[1].pos.x-v[0].pos.x);}
    // Screen shake is an authored per-tick discontinuous offset. Keep bounds
    // in screen space, but let the analyzer remove this from object motion.
    if(screen_shake){r.meta[11]|=1u<<16;r.values[27]=screen_shake->x;r.values[28]=screen_shake->y;}
    // Number/text draw helpers may replace loadedSprite without updating the
    // VM's activeSpriteIndex. Record that actual identity, rather than calling
    // a digit change an un-interpolated UV animation. Wire word 29 is opaque.
    const uint32_t sprite_identity=uint32_t(reinterpret_cast<uintptr_t>(vm.loadedSprite));
    r.meta[11]|=1u<<20;std::memcpy(r.values+29,&sprite_identity,sizeof(sprite_identity));
    r.values[30]=opacity/n;r.values[31]=finite?1:0;
    const uint32_t used=std::min(n,16u);
    for(uint32_t i=0;i<used;++i){const auto& p=v[n<=16?i:i*(n-1)/15];float* o=r.vertices+i*9;o[0]=p.pos.x;o[1]=p.pos.y;o[2]=p.pos.z;o[3]=p.uv.x;o[4]=p.uv.y;for(uint32_t c=0;c<4;++c)o[5+c]=channel(p.color,c==0?16:c==1?8:c==2?0:24);}
    records.push_back(r);
}
void capture_effect(const EffectState& effect,const SpriteVertex* vertices,uint32_t count){
    if(!enabled)return;
    // In custom rings, ANM pos.x can be radius rather than position. Observe
    // the parameters actually consumed by EffectGeometry as a separate record.
    // This diagnostic copy is never sent to rendering or simulation.
    AnmVm parameters=effect;
    parameters.pos=effect.center;parameters.pos2={effect.height,effect.frequency,0};
    parameters.scale={effect.radius,effect.width};parameters.rotation={effect.angle,effect.angle_y,0};
    const size_t first=records.size();capture(parameters,vertices,count,5);
    if(records.size()>first){auto& flags=records.back().meta[11];flags|=1u<<17;
        if(flags&(1u<<8))flags|=1u<<9;
    }
}
void capture_fan(const AnmVm& vm,const UntexturedVertex* v,uint32_t n){
    if(!enabled)return;
    // Actual fan vertices, not an invented quadrilateral; truncation is explicit.
    std::array<SpriteVertex,1024> converted{};
    const uint32_t count=std::min(n,uint32_t(converted.size()));
    for(uint32_t i=0;i<count;++i){converted[i].pos=v[i].pos;converted[i].reciprocal_w=v[i].reciprocal_w;converted[i].color=v[i].color;}
    capture(vm,converted.data(),count,3);
    if(n>count){++dropped;if(!records.empty())records.back().meta[11]|=4;}
}
void capture_world(const AnmVm& vm,AnmRenderer& renderer,const Matrix4& world){
    if(!enabled)return;
    // Project the actual world quad using the matrices just submitted. This
    // read-only projection is screen-space evidence, not a second renderer.
    // Use the exact submitted world transform, including authored anchoring.
    // UV/fog/color shader results are deliberately NOT claimed as observed.
    Matrix4 transform=world;GraphicsMath::multiply(transform,transform,renderer.view_matrix);GraphicsMath::multiply(transform,transform,renderer.projection_matrix);
    bool projection_clip=false;
    std::array<SpriteVertex,4> v{};
    for(uint32_t i=0;i<4;++i){const auto& input=renderer.world_quad[i].pos;const float w=input.x*transform.m[0][3]+input.y*transform.m[1][3]+input.z*transform.m[2][3]+transform.m[3][3];projection_clip|=!std::isfinite(w)||w<=1e-5f;
        GraphicsMath::project(v[i].pos,input,&renderer.viewport,&renderer.projection_matrix,&renderer.view_matrix,&world);v[i].color=vm.flag17?vm.color2.d3dColor:vm.color1.d3dColor;v[i].uv=renderer.world_quad[i].uv;}
    capture(vm,v.data(),4,4);
    if(!records.empty()){records.back().meta[11]|=8; // CPU-projected, not GPU readback.
        if(projection_clip)records.back().meta[11]|=1u<<22;
    }
}
extern "C" {
__attribute__((export_name("audit_fault"))) void audit_fault(uint32_t hold){th08::presentation::audit_hold_interpolation=hold!=0;}
__attribute__((export_name("audit_enable"))) void audit_enable(uint32_t on){enable(on!=0);}
__attribute__((export_name("audit_records"))) const Record* audit_records(){return records.data();}
__attribute__((export_name("audit_count"))) uint32_t audit_count(){return records.size();}
__attribute__((export_name("audit_stride"))) uint32_t audit_stride(){return sizeof(Record);}
__attribute__((export_name("audit_dropped"))) uint32_t audit_dropped(){return dropped;}
__attribute__((export_name("audit_tick_id"))) uint32_t audit_tick_id(){return tick;}
__attribute__((export_name("audit_pass"))) uint32_t audit_pass(){return pass;}
__attribute__((export_name("audit_alpha"))) float audit_alpha(){return alpha;}
__attribute__((export_name("audit_reference"))) const Record* audit_reference(uint32_t which){return references[which?1:0].data();}
__attribute__((export_name("audit_reference_count"))) uint32_t audit_reference_count(uint32_t which){return references[which?1:0].size();}
__attribute__((export_name("audit_reference_tick"))) uint32_t audit_reference_tick(uint32_t which){return reference_ticks[which?1:0];}
__attribute__((export_name("audit_reference_dropped"))) uint32_t audit_reference_dropped(uint32_t which){return reference_dropped[which?1:0];}
}
}
#endif
