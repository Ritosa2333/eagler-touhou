#include "PlayerSetup.hpp"
namespace th08 {
const PlayerResourceNames* player_resource_names(u8 character)noexcept{
    static constexpr PlayerResourceNames names[]{
        {"player00.anm","ply00a.sht","ply00as.sht"},{"player01.anm","ply01a.sht","ply01as.sht"},{"player02.anm","ply02a.sht","ply02as.sht"},{"player03.anm","ply03a.sht","ply03as.sht"},
        {"player00.anm","ply00a.sht","ply00a.sht"},{"player00.anm","ply00as.sht","ply00as.sht"},{"player01.anm","ply01a.sht","ply01a.sht"},{"player01.anm","ply01as.sht","ply01as.sht"},
        {"player02.anm","ply02a.sht","ply02a.sht"},{"player02.anm","ply02as.sht","ply02as.sht"},{"player03.anm","ply03a.sht","ply03a.sht"},{"player03.anm","ply03as.sht","ply03as.sht"}
    };return character<12?&names[character]:nullptr;
}
bool initialize_player(PlayerMotionState& motion,PlayerLifeState& life,PlayerBombState& bomb,PlayerShotsState& shots,GaugeThresholds& thresholds,const ShotProfile& human,const PlayerSetupContext& context,PlayerSetupActions& actions){
    const auto* names=player_resource_names(context.character);if(!names)return false;
    if(context.initial){if(!actions.load_shots(false,names->human)||!actions.load_shots(true,names->focused)||!actions.load_animation(names->animation))return false;}else actions.retained_animation();
    actions.animation(context.character<4||!(context.character&1)?0:5);
    auto& m=motion.movement;m.position={Scalar::div(context.extent.x,2),Scalar::sub(context.extent.y,64),.49f};
    for(auto& area:shots.regions.damaging)area.reset();for(auto& area:shots.regions.cancelling)area.reset();
    const float hit=Scalar::div(human.hit_size,2),graze=Scalar::div(human.graze_size,2),item=Scalar::div(human.item_radius,2);m.half_boxes[0]={hit,hit,5};m.half_boxes[1]={graze,graze,5};m.half_boxes[2]={item,item,5};
    m.direction=0;if(context.section_warp){life.state=0;life.timer.set(0);}else{life.state=1;life.timer.set(context.spell_practice?10:120);}// section warp: upstream th08_disable_muteki (0x44d886) spawns hittablemotion.form.reserved[0]=1;for(auto& shot:shots.shots)shot.state=0;
    shots.shooting_timer.set(-1);motion.gauge.idle.set(0);motion.form.transition.set(0);bomb.active=0;shots.option_angle=-1.5707963705062866f;m.multiplier={1,1};life.predead_count=human.deathbomb_limit;
    if(context.initial)actions.hud_interrupt(1);for(i32 i=0;i<3;++i)actions.hud_group(i,2);thresholds.configure(context.character);motion.gauge.effect=nullptr;
    for(auto& position:m.history)position=m.position;motion.form.focused=2;if(context.character>3)PlayerOptions::initialize(motion.options,context.character);
    shots.time_item_threshold=context.character>=4&&!(context.character&1)?27:40;actions.time_item_threshold(shots.time_item_threshold);return true;
}
}
