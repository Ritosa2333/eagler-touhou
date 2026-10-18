#include "EnemyDrawing.hpp"
#include "FamiliarEffectView.hpp"
#include <cassert>
#include <cstdio>
#include <vector>

using namespace th08;
namespace {
struct Draw:EnemyDrawActions {
    i32 form=-1;std::vector<AnmVm> sprites;
    Vec3 position(EclVm& enemy)override{return enemy.resolved_position;}
    float direction(EclVm& enemy)override{return enemy.direction.z;}
    void sprite(AnmVm& vm)override{sprites.push_back(vm);}
    void strip(AnmVm&,const SpriteVertex*,i32)override{assert(false);}
    i32 familiar_form()const override{return form;}
};
void layer_and_tint(){
    EclVm parent{},familiar{};familiar.parent=&parent;familiar.flags=0x100;
    familiar.animation[1].scriptIndex=familiar.animation[2].scriptIndex=-1;
    familiar.animation[0].color1.d3dColor=0xf0ffffff;
    // A human host's authored hit flash must survive a matching local view.
    familiar.animation[0].flag17=true;familiar.animation[0].color2.d3dColor=0xf0ff6080;
    EclVm* layers[4]{nullptr,nullptr,&familiar,nullptr};
    const auto color=familiar.animation[0].color2;
    Draw draw;draw.form=0;
    assert(draw_enemy_layers(layers,0,2,{},draw));assert(draw.sprites.empty());
    assert(draw_enemy_layers(layers,2,4,{},draw));assert(draw.sprites.size()==1);
    assert(draw.sprites.back().flag17&&draw.sprites.back().color2.d3dColor==color.d3dColor);
    draw.form=1;draw.sprites.clear();
    assert(draw_enemy_layers(layers,0,2,{},draw));assert(draw.sprites.size()==1);
    assert(draw.sprites.back().flag17&&u32(draw.sprites.back().color2.d3dColor)==0x782020c0);
    assert(draw_enemy_layers(layers,2,4,{},draw));assert(draw.sprites.size()==1);
    assert(familiar.flags==0x100&&familiar.animation[0].color2.d3dColor==color.d3dColor);
    assert(familiar.animation[0].flag17&&layers[2]==&familiar);

    // A youkai host must not force its non-attackable layer/tint onto P2.
    familiar.flags|=0x800;layers[0]=&familiar;layers[2]=nullptr;
    familiar.animation[0].color2.d3dColor=0x782020c0;
    draw.form=0;draw.sprites.clear();
    assert(draw_enemy_layers(layers,0,2,{},draw));assert(draw.sprites.empty());
    assert(draw_enemy_layers(layers,2,4,{},draw));assert(draw.sprites.size()==1);
    assert(!draw.sprites.back().flag17);
    draw.form=-1;draw.sprites.clear();
    assert(draw_enemy_layers(layers,0,2,{},draw));assert(draw.sprites.size()==1);
    assert(draw.sprites.back().flag17); // observers retain the authoritative view
    familiar.parent=nullptr;draw.form=0;draw.sprites.clear();
    assert(draw_enemy_layers(layers,0,2,{},draw));assert(draw.sprites.size()==1);
    assert(draw.sprites.back().flag17); // ordinary enemies are unaffected
}
struct Script {
    std::vector<u8> bytes;
    void op(i16 opcode,std::initializer_list<i32> arguments){
        AnmRawInstr instruction{};instruction.opcode=opcode;instruction.instructionSize=u16(8+4*arguments.size());
        const auto at=bytes.size();bytes.resize(at+instruction.instructionSize);
        std::memcpy(bytes.data()+at,&instruction,8);
        u32 i=0;for(auto argument:arguments)std::memcpy(bytes.data()+at+8+4*i++,&argument,4);
    }
    AnmRawInstr* data(){return reinterpret_cast<AnmRawInstr*>(bytes.data());}
};
void ring_animation(){
    Script script;
    script.op(21,{1});script.op(34,{8,0,230});script.op(36,{8,0,signed_bits(0x3f000000),signed_bits(0x3f000000)});script.op(20,{});
    script.op(21,{2});script.op(34,{8,0,70});script.op(36,{8,0,signed_bits(0x3f4ccccd),signed_bits(0x3f4ccccd)});script.op(20,{});
    script.op(21,{3});script.op(1,{});script.op(-1,{});
    AnmLoaded file{};file.rawData=script.bytes.data();
    for(u8 kind:{u8(32),u8(33)})for(i32 host_form:{0,1})for(i32 local_form:{0,1}){
        Rng host_random{};AnmExecutor host(host_random);FamiliarEffectView view;
        EffectState source{};source.Initialize();source.anmFile=&file;
        source.beginningOfScript=source.currentInstruction=script.data();
        source.active=1;source.kind=kind;source.pendingInterrupt=host_form?2:1;
        source.color1.a=0;source.scale={0,0};source.age.set(0);
        for(i32 frame=0;frame<8;++frame){
            const EffectState before=source;const Rng rng_before=host_random;const auto executed=host.executed;
            view.update(source,local_form,host.timing);
            assert(std::memcmp(&before,&source,sizeof(source))==0);
            assert(std::memcmp(&rng_before,&host_random,sizeof(host_random))==0&&host.executed==executed);
            assert(!host.execute(source));source.age.tick(host.timing);
        }
        EffectState draw=source;assert(view.apply(source,draw));
        assert(draw.color1.a==(local_form?70:230));assert(draw.scale.x==(local_form?.8f:.5f));
        assert(source.color1.a==(host_form?70:230));

        // A host-only interrupt does not restart or recolor the local ring.
        source.pendingInterrupt=host_form?1:2;
        view.update(source,local_form,host.timing);assert(!host.execute(source));source.age.tick(host.timing);
        draw=source;assert(view.apply(source,draw));assert(draw.color1.a==(local_form?70:230));
        // Local changes use the authored eight-frame transition.
        view.update(source,!local_form,host.timing);assert(!host.execute(source));source.age.tick(host.timing);
        draw=source;assert(view.apply(source,draw));assert(draw.color1.a!=70&&draw.color1.a!=230);
        for(i32 frame=1;frame<8;++frame){view.update(source,!local_form,host.timing);assert(!host.execute(source));source.age.tick(host.timing);}
        draw=source;assert(view.apply(source,draw));assert(draw.color1.a==(local_form?230:70));
        const EffectState before=source;
        presentation::begin(.5f,true,true);
        EffectState a=source,b=source;assert(view.apply(source,a)&&view.apply(source,b));
        assert(std::memcmp(&a,&b,sizeof(a))==0&&std::memcmp(&before,&source,sizeof(source))==0);
        presentation::end();
        // Rewind or reuse must restart from the restored authoritative VM.
        source.age.set(0);source.color1.a=0;source.scale={0,0};source.pendingInterrupt=host_form?2:1;
        view.update(source,local_form,host.timing);assert(!host.execute(source));source.age.tick(host.timing);
        draw=source;assert(view.apply(source,draw));assert(draw.color1.a!=70&&draw.color1.a!=230);
        source.pendingInterrupt=3;view.update(source,local_form,host.timing);source.age.tick(host.timing);
        draw=source;assert(view.apply(source,draw));assert(!draw.visible);
        source.active=0;assert(!view.apply(source,draw));
        view.reset();source.active=1;assert(!view.apply(source,draw));
    }
}
}
int main(){layer_and_tint();ring_animation();std::puts("TH08 local familiar draw layers, authored ring transitions, repeated draws and state isolation: PASS");}
