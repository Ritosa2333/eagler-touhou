#include "EnemyState.hpp"
#include "Trace.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
void EnemyState::initialize(Enemy* owner,const float* rate)noexcept{
    std::memset(this,0,sizeof(*this));script_owner=owner;
    hitbox=collision_box={24,24};boss_slot=-1;facing=1;manager_link.value=owner;
    drops.spread={32,32};
    lifetime.set(0,rate);damage_immunity.set(0,rate);collision_immunity.set(0,rate);
    for(auto& interrupt:interrupts)interrupt.health=interrupt.time=-1;
#if PORT_TITLE == 16
    th16_phase_timer.set(0,rate); // 41b6d2..41b721.
    // Retail41ab15..41ab47 defaults before the constructor's first frame.
    th16_time_drop_timer.set(60,rate);th16_time_drop_duration=60;th16_time_drop_minimum=1;drops.counts[15]=10;
#endif
#if PORT_TITLE == 14
    th14_bomb_damage_ratio=1;
    damage_reserved=-1; // 422b5b: negative selects the automatic hit sound.
    for(auto& parent:animation_parents)parent=-1;
    timer_2bc.set(0,rate);
#endif
}
namespace {
Vec3 add(Vec3 a,Vec3 b){return{float(double(a.x)+b.x),float(double(a.y)+b.y),float(double(a.z)+b.z)};}
Vec3 subtract(Vec3 a,Vec3 b){return{float(double(a.x)-b.x),float(double(a.y)-b.y),float(double(a.z)-b.z)};}
void angle_step(Movement& movement,Vec2Interpolator& interpolation,const float* rate){
    if(!interpolation.duration)return;const Vec2 value=sample(interpolation,rate);
    if(!(movement.flags&1))movement.angle=normalize_angle(value.x);movement.speed=value.y;
}
void radius_step(Movement& movement,Vec2Interpolator& interpolation,const float* rate){
    if(!interpolation.duration)return;const Vec2 value=sample(interpolation,rate);movement.radius=value.x;movement.radial_velocity=value.y;
}
void velocity_step(Movement& movement,Vec3Interpolator& interpolation,const float* rate){
    if(interpolation.duration)movement.velocity=subtract(sample(interpolation,rate),movement.position);else movement.update_velocity();
}
float clamp_axis(float value,float center,float width){const double half=double(width)*.5,low=double(center)-half,high=double(center)+half;return value<low?float(low):value>high?float(high):value;}
float player_aim_angle(const Vec3& origin,const Vec3& player)noexcept{
#if PORT_TITLE == 14
    // 44ee00 returns +pi/2 when both binary32 displacements are zero.
    // The shared TH11 atan2 helper deliberately retains its zero result.
    const float dx=float(player.x-origin.x),dy=float(player.y-origin.y);
    if(dx==0.f&&dy==0.f)return 1.5707963705062866f;
#endif
    return aim_angle(origin,player);
}
}
// 0x411610: the coordinate sum is rounded before the displacement subtraction.
void EnemyState::combine_movement()noexcept{
#if PORT_TITLE == 14 || PORT_TITLE == 16
    current.velocity=subtract(add(absolute.position,relative.position),current.position);current.update();
    if(flags&0x20000){
        auto clamp=[](float v,float c,float w){const float h=float(w*.5f),lo=float(c-h),hi=float(c+h);return v<lo?lo:v>hi?hi:v;};
        current.position.x=clamp(current.position.x,clamp_center.x,clamp_size.x);current.position.y=clamp(current.position.y,clamp_center.y,clamp_size.y);
        absolute.position=subtract(current.position,relative.position);
    }
    return;
#endif
    current.velocity=subtract(add(relative.position,absolute.position),current.position);current.update();
    // The original gates this clamp with `test byte ptr [esi+0x16ba],1` at 0x413768, i.e. bit 16 of the
    // flags word at 0x16b8, so the mask is 0x10000. This field is that word: the case bodies test
    // [ebx+0x16b8] with 0x40000, 0x400000, 0x2000000, 0x200 and 0x80000, the same values the port uses,
    // and 0x416b6f's 0x40000 sits inside the 0x131 body that the port guards with flags&0x40000. 0x2000
    // was not one of the masks the original tests anywhere.
    if(flags&0x10000){current.position.x=clamp_axis(current.position.x,clamp_center.x,clamp_size.x);current.position.y=clamp_axis(current.position.y,clamp_center.y,clamp_size.y);absolute.position=subtract(current.position,relative.position);}
}
// Movement/visibility prefix of 0x411750, before script and collision updates.
i32 EnemyState::update_movement(const float* rate,const Vec3& camera_delta)noexcept{
#if PORT_TITLE == 16
    previous=current;
    // Own retail41bb50, scalar4171c0, position4258b0 and mode tables
    // 402ff0/403110. The 40000 guard belongs to the outer41d2e0 frame.
    // Radius bands and movement mode4 remain explicit gaps.
    if(absolute_angle.duration||relative_angle.duration||absolute_radius.duration||relative_radius.duration||
        absolute_ellipse.duration||relative_ellipse.duration){trace_point=9026;return -2;}
    for(u32 i=0;i<4;++i)if(th16_motion_interpolation[i].duration){
        auto& m=i<2?absolute:relative;if(!(i&1)&&((m.flags&15)==2||(m.flags&15)==3))continue;
        const float value=enemy_th16_motion_sample(th16_motion_interpolation[i],rate);
        if(i&1)m.speed=value;else m.angle=normalize_angle(normalize_angle(value));
    }
    const bool absolute_interpolated=absolute_position.duration!=0,relative_interpolated=relative_position.duration!=0;
    if(absolute_interpolated)absolute.velocity=subtract(enemy_th16_position_sample(absolute_position,rate),absolute.position);
    if(relative_interpolated)relative.velocity=subtract(enemy_th16_position_sample(relative_position,rate),relative.position);
    auto position_step=[&](Movement& m,bool interpolated)->bool{
        const u32 nibble=m.flags&0xfu;
        if(nibble==0u){if(!interpolated){const Vec2 p=polar(m.angle,float(m.speed * *rate));m.velocity.x=p.x;m.velocity.y=p.y;}m.update();return true;}
        if(nibble==1u){m.snap_position();return true;}
        if(nibble==2u){
            m.angle=normalize_angle(float(m.angle+float(m.speed * *rate)));
            m.radius=float(m.radius+float(m.radial_velocity * *rate));
            const Vec2 p=polar(m.angle,m.radius);
            m.position={float(double(m.velocity.x)+p.x),float(double(m.velocity.y)+p.y),float(double(m.velocity.z)+0.)};
            m.snap_position();return true;
        }
        if(nibble==3u){
            if(!interpolated){m.radius=float(m.radius+float(m.radial_velocity * *rate));m.angle=normalize_angle(float(m.angle+float(m.speed * *rate)));}
            enemy_th16_ellipse_position(m);return true;
        }
        return false;};
    if(!position_step(absolute,absolute_interpolated)){trace_point=9027;return -2;}
    if(flags&0x4000000)relative.position=add(relative.position,camera_delta);
    if(!position_step(relative,relative_interpolated)){trace_point=9028;return -2;}
    combine_movement();return 0;
#endif
#if PORT_TITLE == 14
    if(flags&0x40000)return 0;flags|=0x40000;previous=current;
    // 0x4230d0 opens with the current-to-previous copy, then the interpolator gate at 0x423142
    // (`cmp [edi+0x3f0],0`, the +0x28 duration of the first Vec2 band) and the per-record nibble
    // test at 0x42314f (`[edi+0xc8]&0xf`, compared against 2 and 3 before the normalize loop at
    // 0x42316a). What each nibble does per frame is the same table the commands use: 0x4160 for the
    // nibble and 0x3df0 for the walk. The measured rules (tools/tmp/fit2.py over the oracle's
    // mode-table frames) are: nibble 0 recomputes velocity.x/velocity.y from polar(angle,
    // speed*rate) and integrates; nibble 1 only snaps; nibble 2 walks angle by speed*rate and
    // radius by radial_velocity*rate and bakes position = velocity + polar(angle, radius). The camera
    // delta of 0x4d9294 lands on the relative record between the two (the port's mode-zero path
    // already fixed that order), and combine_movement closes the frame.
    // TH14 4230d0 leaves the +510/+554 ellipse interpolation bands dormant.
    if(absolute_angle.duration||relative_angle.duration)return -2;
    for(u32 i=0;i<4;++i)if(motion_interpolation[i].duration){
        auto& m=i<2?absolute:relative;if(!(i&1)&&((m.flags&15)==2||(m.flags&15)==3))continue;
        const float value=enemy_th14_motion_sample(motion_interpolation[i],rate);
        if(i&1)m.speed=value;else m.angle=normalize_angle(normalize_angle(value));
    }
    if(absolute_radius.duration){const auto r=enemy_th14_radius_sample(absolute_radius,rate);absolute.radius=r.x;absolute.radial_velocity=r.y;}
    if(relative_radius.duration){const auto r=enemy_th14_radius_sample(relative_radius,rate);relative.radius=r.x;relative.radial_velocity=r.y;}
    const bool absolute_interpolated=absolute_position.duration!=0,relative_interpolated=relative_position.duration!=0;
    if(absolute_interpolated)absolute.velocity=subtract(enemy_th14_position_sample(absolute_position,rate,(th14_absolute_position_modes.flags&1)?th14_absolute_position_modes.axis:nullptr),absolute.position);
    if(relative_interpolated)relative.velocity=subtract(enemy_th14_position_sample(relative_position,rate,(th14_relative_position_modes.flags&1)?th14_relative_position_modes.axis:nullptr),relative.position);
    auto position_step=[&](Movement& m,Th14WaveMovement& wave,bool interpolated)->bool{
        const u32 nibble=m.flags&0xfu;
        if(nibble==0u){if(!interpolated){const Vec2 p=polar(m.angle,float(m.speed * *rate));m.velocity.x=p.x;m.velocity.y=p.y;}m.update();return true;}
        if(nibble==1u){m.snap_position();return true;}
        if(nibble==2u){
            if(!interpolated){
                m.angle=normalize_angle(float(m.angle+float(m.speed * *rate)));
                m.radius=float(m.radius+float(m.radial_velocity * *rate));
            }
            const Vec2 p=polar(m.angle,m.radius);
            m.position={float(double(m.velocity.x)+p.x),float(double(m.velocity.y)+p.y),float(double(m.velocity.z)+0.)};
            m.snap_position();return true;
        }
        if(nibble==3u){
            if(!interpolated){m.radius=float(m.radius+float(m.radial_velocity * *rate));m.angle=normalize_angle(float(m.angle+float(m.speed * *rate)));}
            enemy_th14_ellipse_position(m);return true;
        }
        if(nibble==4u){
            if(!interpolated){
                wave.phase=normalize_angle(float(wave.phase+float(m.radial_velocity * *rate)));
                const Vec2 p=polar(m.ellipse_angle,float(m.speed * *rate));m.velocity={p.x,p.y,0};
            }
            enemy_th14_wave_position(m,wave,rate);return true;
        }
        return false;};
    if(!position_step(absolute,th14_absolute_wave,absolute_interpolated))return -2;
    if(flags&0x4000000)relative.position=add(relative.position,camera_delta);
    if(!position_step(relative,th14_relative_wave,relative_interpolated))return -2;
    combine_movement();return 0;
#endif
    if(flags&0x4000)return 0;flags|=0x4000;previous=current;
    angle_step(absolute,absolute_angle,rate);radius_step(absolute,absolute_radius,rate);
    angle_step(relative,relative_angle,rate);radius_step(relative,relative_radius,rate);
    velocity_step(absolute,absolute_position,rate);velocity_step(relative,relative_position,rate);
    absolute.update();if(flags&0x400000)relative.position=add(relative.position,camera_delta);relative.update();combine_movement();
    const double half_x=double(visual_size.x)*.5,half_y=double(visual_size.y)*.5;
    if(double(current.position.x)+half_x< -192||double(current.position.x)-half_x>192){if((flags&0x1000)&&!(flags&4))return -1;}
    else if(double(current.position.y)+half_y<0||double(current.position.y)-half_y>448){if((flags&0x1000)&&!(flags&8))return -1;}
    else flags|=0x1000;
    return 0;
}
float aim_angle(const Vec3& origin,const Vec3& player)noexcept{
    // The original has no zero-vector branch: 0x4088c0 loads its two arguments and calls the CRT's atan2 at
    // 0x4937aa, and 0x208 measures atan2(0, 0) as 0 rather than as a quarter turn. This function used to
    // return pi/2 there, which is a divergence the 0x208 comparison caught: with the point exactly at the
    // player position the original's store held 0 and the port's held 1.5707964. Every caller shares this
    // helper - the -9989, -9956 and -9955 getters and the item attraction - so the branch is removed rather
    // than worked around at the one call site that happened to be measured.
    const float dx=float(double(player.x)-origin.x),dy=float(double(player.y)-origin.y);
    return float(std::atan2(double(dy),double(dx)));
}
float position_distance(const Vec3& a,const Vec3& b)noexcept{const double dx=double(a.x)-b.x,dy=double(a.y)-b.y;return float(std::sqrt(double(float(dx*dx+dy*dy))));}
i32* EnemyGlobals::integer_reference(i32 id){
#if PORT_TITLE == 16
    if(id>=-9926&&id<=-9923)return environment.th16_shared_integers+(id+9926);
    if(id>=-9943&&id<=-9940)return (environment.boss?environment.boss:&enemy)->integers+(id+9943);
#endif
#if PORT_TITLE == 14
    if(id>=-9926&&id<=-9923)return environment.th14_shared_integers+(id+9926);
    if(id>=-9943&&id<=-9940)return (environment.boss?environment.boss:&enemy)->integers+(id+9943);
#endif
    if(id>=-9985&&id<=-9982)return enemy.integers+(id+9985);
    if(id>=-9949&&id<=-9947)return environment.shared_integers+(id+9949);
    if(id>=-9943&&id<=-9940)return environment.boss?environment.boss->integers+(id+9943):nullptr;
    return nullptr;
}
float* EnemyGlobals::float_reference(i32 id){
#if PORT_TITLE == 16
    if(id>=-9922&&id<=-9915)return environment.th16_shared_floats+(id+9922);
    if(id>=-9939&&id<=-9936)return (environment.boss?environment.boss:&enemy)->floats+(id+9939);
    switch(id){case -9995:return &enemy.absolute.position.x;case -9994:return &enemy.absolute.position.y;
    case -9993:return &enemy.relative.position.x;case -9992:return &enemy.relative.position.y;default:break;}
#endif
#if PORT_TITLE == 14
    if(id>=-9922&&id<=-9915)return environment.th14_shared_floats+(id+9922);
    if(id>=-9939&&id<=-9936)return (environment.boss?environment.boss:&enemy)->floats+(id+9939);
    switch(id){case -9995:return &enemy.absolute.position.x;case -9994:return &enemy.absolute.position.y;
    case -9993:return &enemy.relative.position.x;case -9992:return &enemy.relative.position.y;default:break;}
#endif
    if(id>=-9981&&id<=-9978)return enemy.floats+(id+9981);
    if(id>=-9939&&id<=-9936)return environment.boss?environment.boss->floats+(id+9939):nullptr;
    if(id>=-9935&&id<=-9932)return enemy.extra_floats+(id+9935);
    return nullptr;
}
#if PORT_TITLE == 14
i32 EnemyEnvironment::th14_targetable_enemy_count()const noexcept{
    // Retail4219b0 walks the live manager list on every ECL -9908 read.
    i32 result=0;
    for(auto* node=th14_enemy_list?*th14_enemy_list:nullptr;node;node=node->next){
        const auto& state=node->value->state;
        if(!(state.flags&0x31)&&state.damage_immunity.current<=0)++result;
    }
    return result;
}
#endif
double EnemyGlobals::floating(i32 id){
#if PORT_TITLE == 16
    if(id>=-9926&&id<=-9923)return float(environment.th16_shared_integers[id+9926]);
    if(!environment.boss&&((id>=-9943&&id<=-9940)||(id>=-9939&&id<=-9936)))return 0;
#endif
#if PORT_TITLE == 14
    if(id>=-9926&&id<=-9923)return float(environment.th14_shared_integers[id+9926]);
    if(id==-9908)return float(environment.th14_targetable_enemy_count());
    if(id==-9906)return (enemy.flags>>19)&1u;
    // Retail reads return zero with no Boss, although writable references
    // fall back to this enemy's storage (42b11e / 42bd32).
    if(!environment.boss&&((id>=-9943&&id<=-9940)||(id>=-9939&&id<=-9936)))return 0;
#endif
    if constexpr(kPortTitle==14||kPortTitle==16)if(id==-9907)return float(environment.spell_practice_id);
    if(auto* p=integer_reference(id))return *p;if(auto* p=float_reference(id))return *p;
#if PORT_TITLE == 14
    if(environment.th14_random_read&&(id==-10000||id==-9999||id==-9998||id==-9987))environment.th14_random_read(id,enemy.instance_id,environment.rng().call_count());
#endif
    switch(id){
    case -10000:{const u32 value=environment.rng().next32();return kPortTitle==14?value&0x7fffffffu:value;}
    case -9999:return environment.rng().unit();
    case -9998:return float(double(environment.rng().signed_unit())*3.1415927410125732421875);
    case -9987:return environment.rng().signed_unit();
    case -9997:case -9977:return enemy.current.position.x;case -9996:case -9976:return enemy.current.position.y;
    case -9995:case -9975:return enemy.absolute.position.x;case -9994:case -9974:return enemy.absolute.position.y;
    case -9993:case -9973:return enemy.relative.position.x;case -9992:case -9972:return enemy.relative.position.y;
    case -9991:case -9965:return environment.player_position.x;case -9990:case -9964:return environment.player_position.y;
    case -9989:return player_aim_angle(enemy.current.position,environment.player_position);
    // TH14 42b352 reads enemy+14b4: phase timer fractional, not lifetime.
    case -9988:return kPortTitle==14?enemy.timer_2bc.fractional:enemy.lifetime.fractional;
    case -9986:return (enemy.flags>>(kPortTitle==14?24:20))&1;
    case -9971:return enemy.absolute.angle;case -9970:return enemy.relative.angle;
    case -9969:return enemy.absolute.speed;case -9968:return enemy.relative.speed;
    case -9967:return enemy.absolute.radius;case -9966:return enemy.relative.radius;
    case -9963:return environment.boss?environment.boss->current.position.x:0;
    case -9962:return environment.boss?environment.boss->current.position.y:0;
    case -9960:return environment.rank;case -9959:return environment.difficulty;
    case -9958:return float(std::atan2(double(enemy.current.velocity.y),double(enemy.current.velocity.x)));
    case -9957:return 1;case -9956:return player_aim_angle(enemy.absolute.position,environment.player_position);
    case -9955:return player_aim_angle(enemy.relative.position,environment.player_position);case -9954:return enemy.health;
    case -9953:case -9952:case -9951:case -9950:return environment.difficulty==id+9953;
    case -9946:return environment.enemy_count;case -9945:return signed_bits(u32(environment.character)*2+u32(environment.subtype));
    case -9944:return position_distance(enemy.current.position,environment.player_position);
    default:return 0;
    }
}
i32 EnemyGlobals::integer(i32 id){
#if PORT_TITLE == 16
    if(!environment.boss&&((id>=-9943&&id<=-9940)||(id>=-9939&&id<=-9936)))return 0;
#endif
#if PORT_TITLE == 14
    if(!environment.boss&&((id>=-9943&&id<=-9940)||(id>=-9939&&id<=-9936)))return 0;
    if(id==-9908)return environment.th14_targetable_enemy_count();
    if(id==-9909)return signed_bits(enemy.parent_instance_id); // 42ad9c, object +11e8.
#endif
    if constexpr(kPortTitle==14||kPortTitle==16)if(id==-9907)return environment.spell_practice_id;
    if(auto* p=integer_reference(id))return *p;
    switch(id){
    case -10000:
#if PORT_TITLE == 14
        if(environment.th14_random_read)environment.th14_random_read(id,enemy.instance_id,environment.rng().call_count());
#endif
        {const u32 value=environment.rng().next32();return signed_bits(kPortTitle==14?value&0x7fffffffu:value);}
    case -9998:case -9989:case -9956:case -9955:return 0;
    // TH14 42aa00 reads enemy+14b0: phase timer current.
    case -9988:return kPortTitle==14?enemy.timer_2bc.current:enemy.lifetime.current;
    case -9961:return environment.animation_sprite?environment.animation_sprite(enemy.animations[0],environment.animation_user):0;
    default:return truncate_int(floating(id));
    }
}
}
