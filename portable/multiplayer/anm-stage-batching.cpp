// Real StageDraw / AnmRenderer submission oracle; ordinary and MP use one fixture.
// The legacy loop below is frozen from StageDraw.cpp before the narrow setter edit.
// GPU rasterization and browser performance are deliberately outside this gate.
#include "../../th11_web/cpp/game/Stage.hpp"
#include "../../th11_web/cpp/game/AnmRenderer.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <type_traits>
#include <utility>
#include <vector>

namespace {
using namespace th11;
using namespace touhou::graphics;
unsigned groups=0,checks=0;
void check(bool value,const char* message){
    ++checks;if(!value){std::fprintf(stderr,"FAIL batching group %u check %u: %s\n",groups,checks,message);std::exit(1);}
}
void group(const char* name){++groups;std::printf("GROUP %u %s\n",groups,name);}
void bytes(std::vector<u8>& out,const void* source,std::size_t size){
    if(size){const auto* p=static_cast<const u8*>(source);out.insert(out.end(),p,p+size);}
}
template<class T>void append(std::vector<u8>& out,const T& value){
    static_assert(std::is_trivially_copyable<T>::value,"plain oracle value");bytes(out,&value,sizeof value);
}
void argument(std::vector<u8>& out,const Argument& a){append(out,a.source);append(out,a.complement);append(out,a.alphaOnly);}
void combiner(std::vector<u8>& out,const Combiner& c){append(out,c.operation);argument(out,c.first);argument(out,c.second);}
std::vector<u8> pipeline_bytes(const PipelineState& p){
    // Serialize every current field individually: no struct padding, omitted
    // textureFactor, tolerance, hash-only comparison or instancing relaxation.
    std::vector<u8> out;
    append(out,p.depthTest);append(out,p.depthWrite);append(out,p.blend);append(out,p.alphaTest);
    append(out,p.fog);append(out,p.rangeFog);append(out,p.dither);
    append(out,p.depthCompare);append(out,p.alphaCompare);
    append(out,p.sourceBlend);append(out,p.destinationBlend);append(out,p.blendEquation);append(out,p.cull);
    append(out,p.colorMask);append(out,p.alphaReference);append(out,p.fogColor);append(out,p.textureFactor);
    append(out,p.fogMode);append(out,p.fogNear);append(out,p.fogFar);append(out,p.fogDensity);
    combiner(out,p.color);combiner(out,p.alpha);append(out,p.textureTransform);
    append(out,p.minFilter);append(out,p.magFilter);append(out,p.addressU);append(out,p.addressV);return out;
}
struct Triangle {
    std::vector<u8> state,vertices;
    bool fog=false,depth=false;
    u32 texture=0;
    bool operator==(const Triangle& other)const{return state==other.state&&vertices==other.vertices;}
};
struct Device final:ZunGraphics {
    PipelineState state;
    u32 texture_handle=0,target=77;
    VertexLayout layout=VertexLayout::ScreenColorUv;
    GraphicsViewport viewport;
    std::array<Matrix4,4> matrices;
    std::vector<Triangle> output;
    u32 batches=0;
    Device(){
        initialize_game_pipeline();
        state.rangeFog=true;state.dither=true;state.depthCompare=Compare::LessEqual;
        state.colorMask=13;state.fogColor=0xff254367;state.fogNear=350.5f;state.fogFar=2400.25f;
        state.fogDensity=.375f;state.alphaReference=17;state.textureFactor=0x8f7391a5;
        state.addressU=Address::Mirror;state.addressV=Address::Clamp;
        for(u32 i=0;i<matrices.size();++i){matrices[i].identity();matrices[i].m[12]=float(i)*.125f;}
    }
    PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return 100+index;}
    void bind_texture(u32 handle)override{texture_handle=handle;}
    void set_layout(VertexLayout value)override{layout=value;}
    void set_matrix(MatrixKind kind,const Matrix4& value)override{matrices[u32(kind)]=value;}
    void set_viewport(const GraphicsViewport& value)override{viewport=value;}
    bool select_target(const AnmResource*,u32 index)override{target=200+index;return true;}
    bool clear_target(u32,const GraphicsViewport*)override{return true;}
    std::vector<u8> draw_state(u32 stride)const{
        auto out=pipeline_bytes(state);append(out,texture_handle);append(out,target);append(out,layout);append(out,stride);
        append(out,viewport.x);append(out,viewport.y);append(out,viewport.width);append(out,viewport.height);
        append(out,viewport.near_depth);append(out,viewport.far_depth);
        for(const auto& matrix:matrices)for(float v:matrix.m)append(out,v);return out;
    }
    void emit(Topology topology,u32 count,const void* source,u32 stride){
        check(count>0&&stride==touhou::graphics::stride(layout),"real submission has expected vertex layout/stride");
        check(topology==Topology::Triangles||topology==Topology::Strip||topology==Topology::Fan,"triangle-compatible actual submission");
        ++batches;const auto capture=draw_state(stride);const auto* raw=static_cast<const u8*>(source);
        for(u32 i=0;i<count;++i){
            u32 ids[3];
            if(topology==Topology::Triangles){ids[0]=3*i;ids[1]=3*i+1;ids[2]=3*i+2;}
            else if(topology==Topology::Fan){ids[0]=0;ids[1]=i+1;ids[2]=i+2;}
            else {ids[0]=i+(i&1);ids[1]=i+1-(i&1);ids[2]=i+2;}
            Triangle triangle;triangle.state=capture;triangle.fog=state.fog;triangle.depth=state.depthWrite;triangle.texture=texture_handle;
            for(u32 id:ids)bytes(triangle.vertices,raw+std::size_t(id)*stride,stride);
            output.push_back(std::move(triangle));
        }
    }
    void triangles(u32 count,const void* vertices,u32 stride)override{emit(Topology::Triangles,count,vertices,stride);}
    void primitives(Topology topology,u32 count,const void* vertices,u32 stride)override{emit(topology,count,vertices,stride);}
};
// Frozen reference to the original function; all rendering/geometry/visibility
// methods remain real production components, including original VM mutations.
bool legacy_draw_layer(Stage& stage,AnmRenderer& renderer,SceneCamera& camera,u32 layer){
    renderer.set_camera(camera,false);
    bool depth_write=true;
    for(auto& instance:stage.resource.instances){auto& object=stage.resource.objects[instance.object];if(object.layer!=layer)continue;
        if(!stage_visible(object,instance.position,camera,stage.state.draw_distance_squared)){instance.flags&=~1;++stage.state.culled_objects;continue;}
        object.flags|=2;
        for(const auto& primitive:object.primitives){if(primitive.type!=0)continue;auto& vm=stage.object_animations[primitive.animation];
            const u32 mode=(vm.flags>>22)&15;
            if(mode>=4){vm.position={float(double(primitive.position.x)+instance.position.x),float(double(primitive.position.y)+instance.position.y),float(double(primitive.position.z)+instance.position.z)};
                if((primitive.size.x!=0||primitive.size.y!=0)&&!vm.sprite){stage.error="stage primitive has no sprite";return false;}
                if(primitive.size.x!=0){vm.scale.x=float(double(primitive.size.x)/vm.sprite->width);vm.flags|=8;}
                if(primitive.size.y!=0){vm.scale.y=float(double(primitive.size.y)/vm.sprite->height);vm.flags|=8;}
            }
            auto& pipeline=renderer.pipeline();pipeline.fog=mode==8;
            const bool write=(vm.flags&0x800)==0;if(write!=depth_write){pipeline.depthWrite=write;depth_write=write;}
            if(renderer.draw(vm)==-2){stage.error="stage primitive draw failed";return false;}++stage.state.drawn_primitives;
        }
        instance.flags|=1;++stage.state.drawn_objects;
    }
    if(!depth_write)renderer.pipeline().depthWrite=true;return true;
}
struct Spec {u32 mode=5;bool depth=true;u32 texture=0,blend=0;bool nearest=false;};
struct Fixture {
    AnmResource file;
    AnmManager manager;
    u32 background=0xff123456;
    Stage stage;
    SceneCamera camera;
    Fixture(const std::vector<Spec>& specs):stage(manager,file,file,background,3){
        file.textures.resize(3);for(auto& t:file.textures){t.width=256;t.height=256;}
        for(u32 i=0;i<3;++i)file.sprites.push_back({i,i,16,24,32,48,.0625f,.09375f,.1875f,.28125f});
        camera.position={0,0,-600};camera.direction={0,300,600};camera.viewport={24,8,424,464,.125f,.875f};
        stage.state.draw_distance_squared=9610000;stage.state.draw_flags=1;
        StageObject object;object.id=0;object.layer=0;object.position={0,300,0};object.size={128,128,128};
        stage.object_animations.resize(specs.size());
        for(u32 i=0;i<specs.size();++i){
            const auto& spec=specs[i];auto& vm=stage.object_animations[i];vm.initialize();vm.resource=&file;
            check(vm.bind_sprite(i32(spec.texture)),"bind real authored sprite");
            vm.flags=7u|(spec.mode<<22)|(spec.blend<<4)|(spec.depth?0u:0x800u)|(spec.nearest?0x80000000u:0u);
            vm.position={100+float(i%10)*2,150,0};vm.script_position={.125f,.25f,0};vm.rotation={0,0,.03125f};
            vm.color=0x8f254367u+i*0x00010101u;vm.uv_offset={.015625f,.0078125f};
            if(spec.mode==9||spec.mode==11||spec.mode==12||spec.mode==13){
                check(manager.allocate_geometry(vm,4*sizeof(AnmVertex)),"real owned geometry allocation");
                const AnmVertex quad[]={{{100,120,.4f},1,0x8f123456,{0,0}},{{132,120,.4f},1,0xaf456789,{1,0}},{{100,168,.4f},1,0xbf6789ab,{0,1}},{{132,168,.4f},1,0xcf89abcd,{1,1}}};
                std::memcpy(vm.geometry,quad,sizeof quad);vm.integers[0]=2;
            }
            object.primitives.push_back({0,i16(i),i,{float(i%5)*2,300,0},{32,48}});
        }
        // Wrong-layer and culled instances exercise the actual Stage loop too.
        stage.resource.objects.push_back(object);
        stage.resource.instances.push_back({0,0,{0,0,0}});
        stage.resource.instances.push_back({0,1,{1000000,0,0}});
        auto other=object;other.layer=1;stage.resource.objects.push_back(std::move(other));
        stage.resource.instances.push_back({1,0,{0,0,0}});
    }
};
std::vector<u8> semantic(const Fixture& f){
    std::vector<u8> out;append(out,f.stage.state);append(out,f.camera);
    for(const auto& vm:f.stage.object_animations){append(out,vm);if(vm.geometry)bytes(out,vm.geometry,4*sizeof(AnmVertex));}
    for(const auto& o:f.stage.resource.objects)append(out,o.flags);
    for(const auto& i:f.stage.resource.instances)append(out,i.flags);
    append(out,f.stage.error.size());bytes(out,f.stage.error.data(),f.stage.error.size());return out;
}
struct Outcome {Device device;std::vector<u8> semantic;};
std::pair<Outcome,Outcome> compare_stage(const std::vector<Spec>& specs,bool initial_depth){
    Fixture fixture(specs);
    const auto initial_state=fixture.stage.state;const auto initial_vms=fixture.stage.object_animations;
    const auto initial_camera=fixture.camera;
    std::vector<u32> object_flags;for(const auto& o:fixture.stage.resource.objects)object_flags.push_back(o.flags);
    std::vector<decltype(StageInstance{}.flags)> instance_flags;for(const auto& i:fixture.stage.resource.instances)instance_flags.push_back(i.flags);
    auto run=[&](bool legacy){
        Outcome result;result.device.state.depthWrite=initial_depth;
        AnmRenderer renderer(result.device);
        const bool ok=legacy?legacy_draw_layer(fixture.stage,renderer,fixture.camera,0):fixture.stage.draw_layer(renderer,fixture.camera,0);
        check(ok,"real Stage draw succeeds");renderer.flush();result.semantic=semantic(fixture);return result;
    };
    auto old=run(true);
    fixture.stage.state=initial_state;fixture.stage.object_animations=initial_vms;fixture.camera=initial_camera;fixture.stage.error.clear();
    for(u32 i=0;i<object_flags.size();++i)fixture.stage.resource.objects[i].flags=object_flags[i];
    for(u32 i=0;i<instance_flags.size();++i)fixture.stage.resource.instances[i].flags=instance_flags[i];
    auto next=run(false);
    check(!old.device.output.empty(),"fixture really reaches graphics submissions");
    check(old.device.output.size()==next.device.output.size(),"same normalized triangle count");
    for(u32 i=0;i<old.device.output.size();++i){
        check(old.device.output[i].state==next.device.output[i].state,"every triangle preserves full device state and matrices");
        check(old.device.output[i].vertices==next.device.output[i].vertices,"every triangle preserves exact vertex bytes and order");
    }
    check(old.device.draw_state(0)==next.device.draw_state(0),"final complete device state unchanged");
    check(old.semantic==next.semantic,"Stage camera, counters, flags, complete VMs and geometry unchanged");
    std::printf("BATCHES legacy=%u candidate=%u triangles=%zu\n",old.device.batches,next.device.batches,next.device.output.size());
    return {std::move(old),std::move(next)};
}
void repeated(){
    group("real Stage projected quads coalesce without changing triangles");
    auto result=compare_stage(std::vector<Spec>(96),true);
    check(result.first.device.batches==96,"legacy flush splits each projected quad");
    check(result.second.device.batches==1,"unchanged fog/depth permits one projected batch");
    group("real Stage screen quads retain pixel rounding and order");
    std::vector<Spec> specs(24);for(auto& s:specs)s.mode=0;
    result=compare_stage(specs,true);
    check(result.first.device.batches==24&&result.second.device.batches==1,"screen quads also coalesce");
}
void transitions(){
    group("fog, depth, texture, blend, filter, WorldUv and geometry boundaries");
    const std::vector<Spec> specs={
        {5,true,0,0,false},{5,true,0,0,false},{5,false,0,0,false},{5,false,0,0,false},
        {5,false,1,0,false},{5,false,1,1,false},{5,false,1,1,true},
        {8,true,1,1,true},{8,true,1,1,true},{5,true,1,1,true},{5,true,1,1,true},
        {11,true,2,0,false},{5,true,2,0,false},{5,false,2,0,false},{5,true,2,0,false},
        {12,false,0,0,false},{5,false,0,0,false},{5,false,0,0,false}
    };
    auto result=compare_stage(specs,true);
    check(result.second.device.batches<result.first.device.batches,"adjacent unchanged states reduce batches amid transitions");
    check(result.second.device.output[14].fog,"mode8 reaches draw with fog enabled");
    check(!result.second.device.output[18].fog,"screen projection after mode8 has fog disabled");
    check(!result.second.device.output[24].depth,"fan changed device depth and same local true cache does not repair it");
    check(result.second.device.state.depthWrite,"final false cache restores device depth");
}
void depth_quirks(){
    group("foreground local depth cache starts true despite actual device false");
    auto result=compare_stage(std::vector<Spec>(12),false);
    for(const auto& tri:result.second.device.output)check(!tri.depth,"initial true requests do not enable an initially false device");
    check(!result.second.device.state.depthWrite,"local cache true leaves initially false device unchanged at exit");
    check(result.second.device.batches==1,"quirk preserved while merging repeated requests");
    group("false then true transition and final false restoration");
    result=compare_stage({{5,true},{5,false},{5,true},{5,true},{5,false}},false);
    const bool expected[]={false,false,true,true,false};
    for(u32 i=0;i<5;++i)for(u32 j=0;j<2;++j)check(result.second.device.output[2*i+j].depth==expected[i],"authored device mask at each primitive");
    check(result.second.device.state.depthWrite,"ending false cache restores true");
}
void camera_boundaries(){
    group("repeated camera selections preserve full triangle state without splitting glyphs");
    Fixture fixture({{0,true}});const auto initial=fixture.stage.object_animations[0];
    const auto run=[&](bool reference,bool changes){
        Outcome result;auto& device=result.device;AnmRenderer renderer(device);SceneCamera c=fixture.camera;auto vm=initial;
        c.mode=1;c.offset={3.125f,-.75f};
        for(unsigned i=0;i<24;++i){
            if(changes){
                if(i==4)c.viewport={32,16,384,448,0,1};
                if(i==8){renderer.invalidate();device.matrices[1].m[3]=99;}
                if(i==12)renderer.set_viewport({0,0,640,480,0,1});
                if(i==16){c.position.x+=1;c.fog.near_distance=17;c.offset.x+=.125f;}
            }
            if(reference)renderer.flush(); // frozen original setter's unconditional boundary
            renderer.set_camera(c,!(changes&&i>=18&&i<21));
            vm.script_position.x=100+float(i)*3.125f;
            check(renderer.draw_ascii_sprite(vm)==0,"actual glyph-like quad draw succeeds");
        }
        renderer.flush();append(result.semantic,c);append(result.semantic,renderer.camera);append(result.semantic,renderer.offset);
        return result;
    };
    for(bool changes:{false,true}){
        const auto reference=run(true,changes),candidate=run(false,changes);
        check(reference.device.output==candidate.device.output,"every camera-selection triangle preserves exact bytes, order and complete device state");
        check(reference.device.draw_state(0)==candidate.device.draw_state(0),"same final matrix, viewport and pipeline state");
        check(reference.semantic==candidate.semantic,"camera-side calculations, fog and offset stay authored");
#ifdef TH11_MULTIPLAYER
        check(candidate.device.batches<reference.device.batches,"redundant camera selections no longer split glyphs");
        if(!changes)check(reference.device.batches==24&&candidate.device.batches==1,"24 same-camera strings coalesce to one ordered submission");
#else
        check(candidate.device.batches==reference.device.batches,"ordinary renderer retains its original path");
#endif
    }
}
void setters(){
    group("direct helper requests flush only pending geometry whose state changes");
    Fixture fixture({{0,true}});auto& vm=fixture.stage.object_animations[0];
    auto run=[&](bool legacy){
        Device device;AnmRenderer renderer(device);renderer.set_viewport(fixture.camera.viewport);
        auto fog=[&](bool value){if(legacy)renderer.pipeline().fog=value;else renderer.set_fog(value);};
        auto depth=[&](bool value){if(legacy)renderer.pipeline().depthWrite=value;else renderer.set_depth_write(value);};
        fog(false);depth(true);check(renderer.draw(vm)==0,"first real queued quad");
        fog(false);depth(true);
        if(!legacy)check(device.batches==0,"no-op setters preserve pending batch");
        check(renderer.draw(vm)==0,"second same-state real quad");
        fog(true);
        if(!legacy)check(device.batches==1&&device.output.size()==4,"fog change flushes both old-state quads");
        check(renderer.draw(vm)==0,"new-fog queued quad");
        depth(false);
        if(!legacy)check(device.batches==2&&device.output.back().fog&&device.output.back().depth,"depth change flushes with prior depth mask");
        check(renderer.draw(vm)==0,"new-depth queued quad");renderer.flush();
        return device;
    };
    const auto old=run(true),next=run(false);
    check(old.output==next.output,"direct setters preserve full per-triangle state and exact vertices");
    check(next.batches==3&&old.batches==4,"only the repeated-state boundary disappears");
}
}
void projection_matrix_cases(){
    group("precomposed projection preserves scalar operation order and exact bytes");
    u32 random=731;auto next=[&](){random=random*1664525u+1013904223u;return random;};
    auto value=[&](){return float(i32(next()%200001)-100000)/64.f;};
    for(unsigned test=0;test<12000;++test){
        Matrix4 world,view,projection;for(auto* m:{&world,&view,&projection})for(auto& v:m->m)v=value();
        if(test%4==0){world.identity();view.identity();projection.identity();}
        if(test%7==0)projection.m[15]=0;
        GraphicsViewport viewport{next()%64,next()%64,next()%2000,next()%2000,value(),value()};
        const auto combined=GraphicsMath::multiply(GraphicsMath::multiply(world,view),projection);
        for(unsigned i=0;i<12;++i){
            const Vec3 point{value(),value(),value()};
            // Frozen original project expression, independent of project_matrix.
            const auto m=GraphicsMath::multiply(GraphicsMath::multiply(world,view),projection);
            float q[4];GraphicsMath::transform(point,m,q);
            if(std::abs(float(double(q[3])-1))>0x1p-23f){const float inverse=float(1./q[3]);for(u32 c=0;c<3;++c)q[c]=float(double(q[c])*inverse);}
            const Vec3 expected{float((double(q[0])+1)*.5*viewport.width+viewport.x),float((1.-q[1])*.5*viewport.height+viewport.y),float((double(viewport.far_depth)-viewport.near_depth)*q[2]+viewport.near_depth)};
            const auto actual=GraphicsMath::project_matrix(point,viewport,combined);
            check(std::memcmp(&expected,&actual,sizeof(Vec3))==0,"projection bytes match original, including clip-W boundaries");
        }
    }
}
int main(){
    static_assert(sizeof(void*)==4,"run the exact WASI32 component");
    repeated();transitions();depth_quirks();setters();camera_boundaries();projection_matrix_cases();
#ifdef TH11_MULTIPLAYER
    std::printf("PASS ANM Stage batching multiplayer: %u groups, %u checks\n",groups,checks);
#else
    std::printf("PASS ANM Stage batching ordinary: %u groups, %u checks\n",groups,checks);
#endif
}

