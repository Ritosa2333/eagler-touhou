#include "BombController.hpp"
#include "EnemyAnimations.hpp"
#include "GraphicsMath.hpp"
namespace th11 {
namespace {constexpr i32 marisa_b_scripts[]={48,50,52,54,56};}
BombController::BombController(AnmManager& a,AnmResource& r,PlayerFrame& p,BombWorld& w,u16 file,AnmResource* text):animations(a),resource(r),player(p),world(w),file_id(file),text_resource(text){}
BombController::~BombController(){if(auto* vm=animations.find(state.animation))if(vm->after_update==ring_noise){vm->after_update=nullptr;vm->reserved_418=0;}}
#ifdef TH11_MULTIPLAYER
void BombController::multiplayer_clear(){
    if(auto* vm=animations.find(state.animation))if(vm->after_update==ring_noise){vm->after_update=nullptr;vm->reserved_418=0;}
    EnemyAnimations helper(animations);helper.erase(state.animation);helper.erase(state.auxiliary);mesh.reset();state.active=0;state.animation=state.auxiliary=0;
}
#endif
bool BombController::sound(i32 id,bool positional){return world.bomb_sound(id,positional?state.position.x:0,positional);}
bool BombController::create(u32& id,i32 script,bool positioned){
    Vec3 p={float((double(state.position.x)+32)+192),float(double(state.position.y)+16),state.position.z};
    auto* vm=animations.create(resource,script,file_id,22,false,false,positioned?&p:nullptr);if(!vm)return false;id=vm->id;return true;
}
bool BombController::body(i32 script){return player.motion.body.bind_script(resource,script,file_id,&animations.rate)&&player.motion.body.update(animations)>=0;}
void BombController::invincible(i32 frames){player.state.invincibility.set(frames,&animations.rate);}
bool BombController::fail_spell(){auto& spell=player.spell;if(spell.flags&1){if(spell.elapsed>=60){spell.bonus=0;spell.flags&=~0x22u;}else if(state.active&&selection!=5)spell.flags|=0x20;}return true;}
void BombController::interrupt(u32 id,i16 label){if(auto* vm=animations.find(id))for(auto* n=&vm->child;n;n=n->next)n->value->pending_interrupt=label;}
AnmVm* BombController::child(i32 script){auto* vm=animations.find(state.animation);if(!vm){state.animation=0;return nullptr;}for(auto* n=&vm->child;n;n=n->next)if(n->value->script_index==script)return n->value;return nullptr;}
Vec3 BombController::game_position(const AnmVm& vm)const{
    const Vec3 sum={float(double(vm.script_position.x)+vm.position.x),float(double(vm.script_position.y)+vm.position.y),float(double(vm.script_position.z)+vm.position.z)};
    return {float(double(float(double(sum.x)+vm.child_position.x))-224),float(double(float(double(sum.y)+vm.child_position.y))-16),float(double(sum.z)+vm.child_position.z)};
}
i32 BombController::start(){
    if(state.active)return -1;if(!supported()){last_error=-2;return -2;}
    state.active=1;state.elapsed.set(0,&animations.rate);
    state.started_during_spell=(player.spell.flags&1)&&player.spell.elapsed>=60;
    if(!world.bomb_sound(38,player.motion.state.position.x,true)){last_error=-3;return -2;}
    state.radius=32;state.growth=10;state.position=selection==0?Vec3{0,224,0}:player.motion.state.position;
    if(selection==0){
        if(!sound(49)||!create(state.animation,35,false)||!create(state.auxiliary,36,false)||!fail_spell()||!body(26)){last_error=-4;return -2;}
        player.motion.state.flags|=2;player.motion.state.previous_dx=player.motion.state.previous_dy=0;
    }else if(selection==1){if(!sound(49)||!create(state.animation,33,true)||!fail_spell()){last_error=-4;return -2;}}
    else if(selection==2){
        if(!sound(49)||!create(state.animation,37,true)||!create_distortion()||!fail_spell()){last_error=-4;return -2;}
        auto* vm=animations.find(state.animation);vm->after_update=ring_noise;vm->reserved_418=u32(reinterpret_cast<uintptr_t>(this));
    }
    else if(selection==3){
        state.position.y=float(double(state.position.y)-32);
        if(!create(state.animation,43,true)||!create_distortion()||!fail_spell()){last_error=-4;return -2;}
        invincible(120);
    }
    else if(selection==4){if(!create(state.animation,47,true)||!fail_spell()){last_error=-4;return -2;}}
    else if(selection==5){
        state.secondary.set(0,&animations.rate);
        if(!create(state.animation,58,true)){last_error=-4;return -2;}
        player.motion.state.flags|=4;
        // Nitori's shield counts as a used Bomb only when it is hit. An
        // unused shield refunds ten power when its animation expires.
        return 0;
    }
    auto& count=world.bomb_count();count=wrapping_add(count,1);return 0;
}
i32 BombController::tick(){
    auto* vm=animations.find(state.animation);if(!vm)state.animation=0;
    if(selection==0){
        if(!vm){if(!body(0))return -2;player.motion.state.flags&=~2u;player.motion.state.previous_dx=player.motion.state.previous_dy=0;invincible(40);return -1;}
        if(state.elapsed.current>=180){
            if(state.elapsed.current==180){if(!sound(38)||!world.bomb_stop_sound(49))return -2;player.shots.damage_areas.circle(state.position,32,10,30,50,&animations.rate);}
            if(!world.bomb_cancel(state.position,state.radius,!(player.spell.flags&1),false))return -2;
            state.radius=float(double(state.radius)+state.growth);
        }return 0;
    }
    if(selection==3){
        if(!vm)return -1;
        if(state.elapsed.current>90){interrupt(state.animation,1);mesh.reset();return -1;}
        update_distortion();return world.bomb_cancel_beam(state.position,!(player.spell.flags&1))?0:-2;
    }
    if(selection==5){
        const i32 t=state.elapsed.current,s=state.secondary.current;
        const u32 shade=u32(s>1?(s*96)/30+32:t>=890?((t*3-2670)*32)/30+32:t<30?128-(t*96)/30:32);
        if(!world.bomb_background_color((((shade|0xffffff00u)<<8)|shade)<<8|shade))return -2;
        if(!vm){
            player.motion.state.flags&=~4u;
            if(!world.bomb_sound(51,player.motion.state.position.x,true)||!world.bomb_refund_power())return -2;
            return -1;
        }
        state.position=player.motion.state.position;
        EnemyAnimations helper(animations);helper.position(state.animation,state.position,true);
        if(!(player.motion.state.flags&4)||player.state.life_state==4||player.state.life_state==2){
            fail_spell();auto& count=world.bomb_count();count=wrapping_add(count,1);player.state.life_state=1;
            if(!state.secondary.current){
                state.secondary.set(1,&animations.rate);
                if(!sound(38))return -2;
                player.shots.damage_areas.circle(state.position,32,20,30,75,&animations.rate);
            }
        }
        if(state.secondary.current>0){
            player.motion.state.flags&=~4u;interrupt(state.animation,1);
            if(!world.bomb_cancel(state.position,state.radius,!(player.spell.flags&1),false))return -2;
            state.radius=float(double(state.radius)+state.growth);state.secondary.tick();invincible(40);
            if(state.secondary.current>30){helper.erase(state.animation);state.animation=0;return -1;}
        }
        return 0;
    }
    invincible(40);if(!vm){if(selection==2)mesh.reset();return -1;}
    if(selection==2){
        state.position=player.motion.state.position;EnemyAnimations helper(animations);helper.position(state.animation,state.position,true);
        if(!world.bomb_cancel(state.position,state.radius,!(player.spell.flags&1),false))return -2;
        state.radius=vm->scale.y;update_distortion();return 0;
    }
    if(selection==1){
        if(!world.bomb_cancel(state.position,96,2u|!(player.spell.flags&1),true))return -2;
        if(state.elapsed.current>=200){
            if(state.elapsed.current==200){if(!sound(38)||!world.bomb_stop_sound(49))return -2;interrupt(state.animation,1);}
            state.radius=float(double(state.radius)+state.growth);
        }return 0;
    }
    if(selection==4){
        const i32 t=state.elapsed.current;if(t%10==0&&t<50&&t>1&&!sound(38,true))return -2;
        for(const auto script:marisa_b_scripts)if(auto* part=child(script))if(!world.bomb_cancel(game_position(*part),part->scale.y,!(player.spell.flags&1),false))return -2;
        state.radius=vm->scale.y;return 0;
    }return -2;
}
bool BombController::update(){
    if(last_error)return false;if(!state.active)return true;if(!supported()){last_error=-2;return false;}
    const i32 result=state.active==1?tick():0;if(result==-2){last_error=-5;return false;}
    if(result){state.active=0;return true;}state.elapsed.tick();return true;
}
bool BombController::damage(const Vec3& p,const Vec2& size,i32& value){
    value=0;if(!state.active)return true;if(!supported())return false;
    if(selection==2||selection==3){
        const double dx=double(p.x)-state.position.x,dy=double(p.y)-state.position.y;
        const float d=float(dx*dx+dy*dy);const double radius=selection==2?state.radius:48;
        if(double(d)<radius*radius){value=selection==2?17:50;return true;}
        if(selection==3){
            const float left=float(double(state.position.x)-16),right=float(double(state.position.x)+16),top=float(double(state.position.y)-448),bottom=state.position.y;
            const float minx=float(double(p.x)-double(size.x)*.5),maxx=float(double(p.x)+double(size.x)*.5),miny=float(double(p.y)-double(size.y)*.5),maxy=float(double(p.y)+double(size.y)*.5);
            if(left<=maxx&&top<=maxy&&minx<=right&&miny<=bottom)value=8;
        }
    }
    if(selection==4)for(const auto script:marisa_b_scripts)if(auto* vm=child(script)){
        const auto center=game_position(*vm);const double dx=double(p.x)-center.x,dy=double(p.y)-center.y;
        const float d=float(dx*dx+dy*dy);if(double(d)<double(vm->scale.y)*vm->scale.y)value+=20;
    }return true;
}
bool BombController::create_distortion(){
    if(!text_resource)return false;mesh=std::make_unique<ScreenDeformation>(animations);
    if(selection==2){
        if(!mesh->initialize(*text_resource,13,13))return false;
        mesh->rectangle(float(double(state.position.x)-17),float(double(state.position.y)-17),34,34);
    }else{
        const float bottom=float(double(state.position.y)+64);
        const i32 rows=(truncate_int(bottom)+32)/32+2;
        if(rows<3||!mesh->initialize(*text_resource,4,u32(rows),true))return false;
        mesh->rectangle(float(double(state.position.x)-60),0,120,float(double(bottom)+32));
        for(u32 row=0;row<mesh->rows;++row){
            for(u32 col=0;col<4;++col){auto& p=mesh->positions[col*mesh->rows+row];const float jitter=float(double(animations.script_rng.signed_unit())*4);p.x=float(double(p.x)+jitter);}
            const u32 color=row+1<mesh->rows?0xffff8080:0xffffffff;
            mesh->vertices[mesh->rows+row].color=mesh->vertices[2*mesh->rows+row].color=color;
        }
    }return true;
}
void BombController::update_distortion(){
    if(!mesh)return;
    if(selection==3){
        for(u32 row=0;row+1<mesh->rows;++row)for(u32 col=1;col<3;++col){const u32 n=col*mesh->rows+row;
            const float jitter=float(double(animations.script_rng.signed_unit())*8);mesh->vertices[n].position.x=float(double(mesh->positions[n].x)+jitter);
        }return;
    }
    const float radius=float(double(state.radius)*.5+state.radius),extent=float(double(radius)*2+40);
    mesh->rectangle(float(double(state.position.x)-radius-20),float(double(state.position.y)-radius-20),extent,extent);
    const Vec3 center{float(double(state.position.x)+32+192),float(double(state.position.y)+16),state.position.z};const double squared=double(radius)*radius;
    for(u32 i=0;i<mesh->vertices.size();++i){auto& v=mesh->vertices[i];auto& p=mesh->positions[i];
        const Vec3 delta{float(double(p.x)-center.x),float(double(p.y)-center.y),float(double(p.z)-center.z)};
        const float distance=float(double(delta.x)*delta.x+double(delta.y)*delta.y),remaining=float(squared-distance);
        if(remaining<0)v.color&=0xffffff;
        else{
            const float strength=float(double(float(double(remaining)/squared))*64);auto direction=GraphicsMath::normalize(delta);
            direction.x=float(double(direction.x)*strength);direction.y=float(double(direction.y)*strength);
            v.color=0xffff4040;v.position.x=float(double(v.position.x)+direction.x);v.position.y=float(double(v.position.y)+direction.y);v.position.z=p.z=0;
        }
    }
}
int BombController::ring_noise(AnmVm& vm){
    auto& owner=*reinterpret_cast<BombController*>(uintptr_t(vm.reserved_418));auto* vertices=static_cast<AnmVertex*>(vm.geometry);
    const i32 count=vm.integers[0]-1;if(count<=0||!vertices)return 0;
    const float step=float(double(vm.integers[1])/count);
    for(i32 i=0;i<count*2;++i){const float dx=float(double(owner.animations.script_rng.signed_unit())*8),dy=float(double(owner.animations.script_rng.signed_unit())*8);
        vertices[i].position.x=float(double(vertices[i].position.x)+dx);vertices[i].position.y=float(double(vertices[i].position.y)+dy);
    }
    const float end=float(double(vertices[count*2-1].uv.y)+step);
    vertices[count*2]=vertices[0];vertices[count*2+1]=vertices[1];vertices[count*2].uv.y=vertices[count*2+1].uv.y=end;return 0;
}
}
