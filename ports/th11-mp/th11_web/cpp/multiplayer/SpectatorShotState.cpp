#include "SpectatorShotState.hpp"
#include "../game/ShotManager.hpp"
#include "../game/PlayerFrame.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>

namespace th11::multiplayer {
// Stage changes retain the physical shot slots. Only named scalar fields cross
// the wire; resource, target, timer and animation owners are rebound locally.
namespace spectator_shots {
using Writer=Netplay::SpectatorScalars::Writer;
using Reader=Netplay::SpectatorScalars::Reader;
static u32 last_failure=0;
inline bool Failed(u32 code){last_failure=code;return false;}
template<class IO> bool Vec(IO& io,Vec2& v){return io.Values(v.x,v.y);}
template<class IO> bool Vec(IO& io,Vec3& v){return io.Values(v.x,v.y,v.z);}
template<class IO> bool Clock(IO& io,Timer& t){return io.Values(t.previous,t.current,t.fractional,t.flags);}
template<class IO> bool Move(IO& io,Movement& m){return Vec(io,m.position)&&Vec(io,m.velocity)&&
    io.Values(m.speed,m.angle,m.radius,m.radial_velocity,m.ellipse_angle,m.ellipse_ratio,m.flags);}
template<class IO,class T,u32 N> bool Interpolate(IO& io,Interpolator<T,N>& v){
    return io.Array(v.start)&&io.Array(v.end)&&io.Array(v.initial_tangent)&&io.Array(v.final_tangent)&&
        Clock(io,v.timer)&&io.Values(v.duration,v.mode);
}
template<class IO> bool Vm(IO& io,AnmVm& v){
    if(!io.Values(v.layer,v.script_index,v.file_index,v.sprite_index,v.sprite_frame)||
       !Vec(io,v.rotation)||!Vec(io,v.angular_velocity)||!Vec(io,v.scale)||!Vec(io,v.scale_velocity)||
       !Vec(io,v.sprite_size)||!Vec(io,v.uv_offset)||!Clock(io,v.timer))return false;
    for(auto& p:v.uv)if(!Vec(io,p))return false;
    return Interpolate(io,v.position_interpolation)&&Interpolate(io,v.color_interpolation)&&
        Interpolate(io,v.alpha_interpolation)&&Interpolate(io,v.rotation_interpolation)&&
        Interpolate(io,v.scale_interpolation)&&Interpolate(io,v.color2_interpolation)&&
        Interpolate(io,v.alpha2_interpolation)&&Interpolate(io,v.scroll_x_interpolation)&&
        Interpolate(io,v.scroll_y_interpolation)&&Vec(io,v.scroll_velocity)&&
        io.Array(v.sprite_matrix.m)&&io.Array(v.transform_matrix.m)&&io.Array(v.uv_matrix.m)&&
        io.Values(v.color,v.secondary_color,v.pending_interrupt,v.reserved_37e)&&Clock(io,v.saved_timer)&&
        io.Array(v.integers)&&io.Array(v.floats)&&io.Array(v.extra_integers)&&
        Vec(io,v.script_position)&&Vec(io,v.position)&&Vec(io,v.child_position)&&
        io.Values(v.flags,v.flags2,v.reserved_40c,v.reserved_418,v.rectangle_columns,v.rectangle_rows,
            v.reserved_41e)&&io.Array(v.reserved_420)&&io.Value(v.reserved_430);
}
inline void Rebind(AnmVm& v,const float* rate){
    v.timer.rate=v.saved_timer.rate=rate;
    v.position_interpolation.timer.rate=v.color_interpolation.timer.rate=v.alpha_interpolation.timer.rate=rate;
    v.rotation_interpolation.timer.rate=v.scale_interpolation.timer.rate=v.color2_interpolation.timer.rate=rate;
    v.alpha2_interpolation.timer.rate=v.scroll_x_interpolation.timer.rate=v.scroll_y_interpolation.timer.rate=rate;
}
template<class IO> bool Spec(IO& io,ShotSpec& s){return io.Values(s.interval,s.delay,s.damage)&&
    Vec(io,s.offset)&&Vec(io,s.hitbox)&&io.Values(s.angle,s.speed,s.option,s.type,s.animation,
        s.hit_animation,s.sound,s.spawn,s.update,s.hit_callback,s.extra_callback);}
template<class IO> bool Area(IO& io,DamageArea& a){return io.Values(a.radius,a.growth,a.angle,a.angular_velocity)&&
    Vec(io,a.size)&&Move(io,a.movement)&&Clock(io,a.timer)&&
    io.Values(a.damage,a.total_damage,a.max_damage,a.period,a.flags);}
template<class IO> bool Pilot(IO& io,ShotPlayer& p){return Vec(io,p.position)&&
    io.Values(p.option_count,p.state,p.warp_state,p.power,p.power_step,p.character,p.subtype,p.weapon_mode,
        p.held,p.focused,p.bomb_active,p.enemies_present,p.special_active);}
inline u32 EnemyIndex(EnemyLink* first,const Enemy* target){
    if(!target)return 0;u32 index=1;for(auto* n=first;n;n=n->next,++index)if(n->value==target)return index;return ~0u;
}
inline Enemy* EnemyAt(EnemyLink* first,u32 index){
    if(!index)return nullptr;for(auto* n=first;n;n=n->next)if(!--index)return n->value;return nullptr;
}
inline bool WriteVmValue(Writer& w,AnmManager& owner,AnmResource& resource,const AnmVm* source){
    const bool present=source!=nullptr;if(!w.Value(present)||!present)return w.Good();
    // Player shot scripts have no native callbacks or geometry. Such an owner
    // must be explicitly supported before it can be admitted as a checkpoint.
    if(source->resource!=&resource)return Failed(2);
    if(source->geometry)return Failed(3);
    if(source->before_update||source->after_update||source->draw_callback||source->sprite_callback)return Failed(4);
    if(source->child.previous||source->child.next)return Failed(5);
    if(source->script_index<0||u32(source->script_index)>=resource.scripts.size())return Failed(6);
    const auto& bytes=resource.scripts[source->script_index].bytes;
    auto offset=[&](AnmInstruction* pc,u32& out){if(!pc){out=~0u;return true;}
        const auto address=reinterpret_cast<std::uintptr_t>(pc),base=reinterpret_cast<std::uintptr_t>(bytes.data());
        if(address<base||address-base>=bytes.size())return Failed(7);out=u32(address-base);return true;};
    u32 instruction=0,saved=0;if(!offset(source->instruction,instruction)||!offset(source->saved_instruction,saved))return false;
    auto v=*source;return w.Values(instruction,saved,owner.multiplayer_tag(*source),source->sprite!=nullptr)&&Vm(w,v);
}
inline bool WriteVm(Writer& w,AnmManager& owner,AnmResource& resource,u32 id){
    return WriteVmValue(w,owner,resource,owner.find(id));
}
inline bool ReadVm(Reader& r,AnmManager& owner,AnmResource& resource,u32& id,AnmVm* embedded=nullptr){
    bool present=false;if(!r.Value(present))return false;id=0;if(!present)return true;
    u32 instruction=0,saved=0,tag=0;bool sprite_present=false;AnmVm state{};
    if(!r.Values(instruction,saved,tag,sprite_present)||!Vm(r,state)||state.script_index<0||
       u32(state.script_index)>=resource.scripts.size()||state.layer>=31||tag>3)return false;
    auto& bytes=resource.scripts[state.script_index].bytes;
    auto valid=[&](u32 pc){return pc==~0u||(pc<=bytes.size()&&bytes.size()-pc>=sizeof(AnmInstruction));};
    if(!valid(instruction)||!valid(saved))return false;
    auto* v=embedded?embedded:owner.create(resource,state.script_index,state.file_index,state.layer);
    if(!v||v->geometry||v->child.next||v->child.previous)return false;
    v->resource=&resource;v->script_index=state.script_index;v->file_index=state.file_index;
    // Bind the resource-local sprite before applying scalar UV/matrix state.
    if(sprite_present){if(!v->bind_sprite(state.sprite_index))return false;}else v->sprite=nullptr;
    Writer fields;if(!Vm(fields,state))return false;Reader field_reader(fields.Bytes());
    if(!Vm(field_reader,*v)||!field_reader.Done())return false;
    v->script_begin=reinterpret_cast<AnmInstruction*>(bytes.data());
    v->instruction=instruction==~0u?nullptr:reinterpret_cast<AnmInstruction*>(bytes.data()+instruction);
    v->saved_instruction=saved==~0u?nullptr:reinterpret_cast<AnmInstruction*>(bytes.data()+saved);
    Rebind(*v,&owner.rate);if(tag)owner.multiplayer_tags[v]=tag;else owner.multiplayer_tags.erase(v);
    id=v->id;return true;
}
template<class IO> bool Motion(IO& io,PlayerMotionState& s){
    return Vec(io,s.position)&&io.Values(s.x,s.y,s.normal_speed,s.focus_speed,s.normal_diagonal,s.focus_diagonal)&&
        Vec(io,s.delta)&&Vec(io,s.last_delta)&&io.Values(s.dx,s.dy,s.previous_dx,s.previous_dy,s.direction,
            s.warp,s.warp_timer,s.focused,s.transition_timer,s.option_lerp,s.standing_timer,s.fast_timer,
            s.weapon_mode,s.recall_timer,s.flags,s.option_count,s.option_angle);
}
template<class IO> bool Option(IO& io,PlayerOption& o){
    if(!io.Value(o.state)||!io.Array(o.reserved04)||!Vec(io,o.polar_normal)||!Vec(io,o.polar_focus)||
       !io.Array(o.reserved2c)||!io.Values(o.target_x,o.target_y,o.x,o.y,o.normal_x,o.normal_y,o.focus_x,o.focus_y))return false;
    for(auto& mode:o.modes)if(!io.Array(mode))return false;
    return io.Array(o.reserved9c)&&io.Values(o.angle,o.reservedac)&&io.Array(o.reservedb8)&&
        io.Values(o.index,o.snap,o.flags,o.behavior,o.reservede0);
}
}

bool SpectatorShotAccess::Capture(const PlayerFrame& player,std::vector<u8>& bytes){
        using namespace spectator_shots;last_failure=1;const auto& source=player.shots;const auto& motion=player.motion;
        Writer w;auto motion_state=motion.state;
        if(!Motion(w,motion_state)||!WriteVmValue(w,source.animations,motion.player_resource,&motion.body)||
           !WriteVm(w,source.animations,motion.bullet_resource,motion.focus_animation))return false;
        for(auto option:motion.options)if(!Option(w,option)||
            !WriteVm(w,source.animations,motion.player_resource,option.animation)||
            !WriteVm(w,source.animations,motion.player_resource,option.auxiliary))return false;
        auto p=source.player;auto schedule=source.schedule.timer;
        auto converted=source.converted_laser_spec;
        const auto locked=EnemyIndex(source.world.enemies,source.locked_target);
        if(locked==~0u||!Pilot(w,p)||!Clock(w,schedule)||!w.Array(source.laser_active)||
           !w.Values(locked,source.target_locked,source.last_error)||!Spec(w,converted))return false;
        for(u32 i=0;i<p.options.size();++i){const auto& option=p.options[i];u32 alias=0;
            if(option.animation){alias=~0u;for(u32 n=0;n<motion.options.size();++n)
                if(option.animation==motion.options[n].animation)alias=n+1;}
            if(alias==~0u)alias=9; // A retained, retiring option from the old stage.
            if(!w.Values(option.x,option.y,option.angle,alias)||
               (alias==9&&!WriteVm(w,source.animations,source.animation_resource,option.animation)))return false;
        }
        for(auto area:source.damage_areas.areas)if(!Area(w,area))return false;
        // Spawn reuses physical slots without clearing every field. Preserve
        // previously used inactive slots as well as the live projectiles.
        u32 count=0;for(const auto& shot:source.shots)if(shot.state||shot.spec)++count;if(!w.Value(count))return false;
        for(u32 index=0;index<source.shots.size();++index){auto s=source.shots[index];if(!s.state&&!s.spec)continue;
            u32 group=~0u,spec_index=0;
            if(s.spec==&source.converted_laser_spec)group=~1u;
            else for(u32 g=0;g<source.resource.groups.size();++g)
                for(u32 n=0;n<source.resource.groups[g].shots.size();++n)
                    if(s.spec==&source.resource.groups[g].shots[n]){group=g;spec_index=n;}
            const auto target=EnemyIndex(source.world.enemies,s.target);
            if(group==~0u)return Failed(8);if(target==~0u)return Failed(9);
            if(group==~0u||target==~0u||!w.Values(index,group,spec_index,target,s.state,s.hit_this_frame,
               s.hit_last_frame,s.damage,s.acceleration)||!Clock(w,s.timer)||!Move(w,s.movement)||
               !WriteVm(w,source.animations,source.animation_resource,s.animation)||
               !WriteVm(w,source.animations,source.animation_resource,s.extra_animation))return false;
        }
        if(!w.Good())return false;bytes=w.Bytes();last_failure=0;return true;
    }
    bool SpectatorShotAccess::Apply(PlayerFrame& player,const std::vector<u8>& bytes){
        using namespace spectator_shots;auto& owner=player.shots;auto& motion=player.motion;Reader r(bytes);u32 ignored=0;
        if(!Motion(r,motion.state)||!ReadVm(r,owner.animations,motion.player_resource,ignored,&motion.body))return false;
        if(auto* old=owner.animations.find(motion.focus_animation))owner.animations.destroy(*old);
        if(!ReadVm(r,owner.animations,motion.bullet_resource,motion.focus_animation))return false;
        for(auto& option:motion.options){
            if(auto* old=owner.animations.find(option.animation))owner.animations.destroy(*old);
            if(auto* old=owner.animations.find(option.auxiliary))owner.animations.destroy(*old);
            if(!Option(r,option)||!ReadVm(r,owner.animations,motion.player_resource,option.animation)||
               !ReadVm(r,owner.animations,motion.player_resource,option.auxiliary))return false;
        }
        u32 locked=0;
        if(!Pilot(r,owner.player)||!Clock(r,owner.schedule.timer)||!r.Array(owner.laser_active)||
           !r.Values(locked,owner.target_locked,owner.last_error)||!Spec(r,owner.converted_laser_spec))return false;
        owner.schedule.timer.rate=&owner.animations.rate;owner.locked_target=EnemyAt(owner.world.enemies,locked);
        if(locked&&!owner.locked_target)return false;
        for(auto& option:owner.player.options){
            u32 alias=0;if(!r.Values(option.x,option.y,option.angle,alias)||alias>9)return false;
            if(alias==9){if(!ReadVm(r,owner.animations,owner.animation_resource,option.animation))return false;}
            else option.animation=alias?motion.options[alias-1].animation:0;
        }
        for(auto& area:owner.damage_areas.areas){if(!Area(r,area))return false;area.timer.rate=&owner.animations.rate;}
        owner.clear_projectiles();for(auto& s:owner.shots)s=ShotState{};
        u32 count=0,previous=0;if(!r.Value(count)||count>owner.shots.size())return false;
        for(u32 n=0;n<count;++n){u32 index=0,group=0,spec_index=0,target=0;ShotState s{};
            if(!r.Values(index,group,spec_index,target,s.state,s.hit_this_frame,s.hit_last_frame,s.damage,s.acceleration)||
               index>=owner.shots.size()||(n&&index<=previous)||s.state<0||s.state>2||
               !Clock(r,s.timer)||!Move(r,s.movement))return false;
            previous=index;s.timer.rate=&owner.animations.rate;
            if(group==~1u)s.spec=&owner.converted_laser_spec;
            else{if(group>=owner.resource.groups.size()||spec_index>=owner.resource.groups[group].shots.size())return false;
                s.spec=&owner.resource.groups[group].shots[spec_index];}
            s.target=EnemyAt(owner.world.enemies,target);if(target&&!s.target)return false;
            if(!ReadVm(r,owner.animations,owner.animation_resource,s.animation)||
               !ReadVm(r,owner.animations,owner.animation_resource,s.extra_animation))return false;
            owner.shots[index]=s;
        }
        owner.activity.invalidate();return r.Done();
    }
u32 SpectatorShotAccess::Failure(){return spectator_shots::last_failure;}
} // namespace th11::multiplayer
