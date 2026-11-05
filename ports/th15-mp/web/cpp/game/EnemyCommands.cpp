#include "EnemyCommands.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
namespace {
void scalar_curve(ScalarInterpolation& curve,i32 duration,i32 mode,float start,float end){curve.start[0]=start;curve.end[0]=end;curve.control1=curve.control2={};curve.duration=duration;curve.mode=mode;curve.timer.set(0);}
void shape_curve(Vec2Interpolation& curve,i32 duration,i32 mode,std::array<float,2> start,std::array<float,2> end){curve.start=start;curve.end=end;curve.control1=curve.control2={};curve.duration=duration;curve.mode=mode;curve.timer.set(0);}
float mirror_angle(float value){return normalize_angle(float(1.57079637050628662109375f-normalize_angle(float(value-1.57079637050628662109375f))));}
void restart_motion(MotionState& motion){if((motion.flags&15)==4)motion.origin=motion.position;motion.advance();}
}
int EnemyCommands::execute(EclContext& context,u16 opcode){
    if(opcode==522||opcode==537||opcode==538||opcode==539){
        const auto* instruction=context.instruction;if(!instruction||instruction->length<32){context.error="Truncated spell declaration";return -2;}
        const u32 size=instruction->argument<u32>(3);if(!size||size>instruction->length-32){context.error="Invalid encoded spell title";return -2;}
        const auto* source=reinterpret_cast<const u8*>(instruction)+32;std::string title;u8 key=0x77,step=7;bool terminated=false;
        for(u32 i=0;i<size;i++){const u8 value=source[i]^key;key=u8(key+step);step=u8(step+0x10);if(!terminated){if(!value)terminated=true;else title.push_back(char(value));}}
        if(!terminated||title.size()>=64){context.error="Spell title outside original range";return -2;}
        SpellStartRequest request;request.name=title;request.survival=opcode==522;
        if(!context.integer(0,instruction->argument<i32>(0),request.id))return -2;
        if(opcode!=522)request.id=wrapping_add(request.id,wrapping_add(world.difficulty,opcode==537?0:opcode==538?-1:-2));
        if(!context.integer(2,instruction->argument<i32>(2),request.requested_bonus)||!context.integer(1,instruction->argument<i32>(1),request.duration))return -2;
        if(!host||!host->begin_spell(request)){context.error="Enemy spell start failed";return -2;}enemy.life_flags|=1;enemy.life_budget=wrapping_sub(wrapping_mul(enemy.life,8),enemy.life);enemy.age_timer.set(0);return 0;
    }
    if(opcode==613){if(!host||!host->cancel_all_bullets(0)||!host->cancel_all_lasers(1,false)){context.error="Enemy full-field projectile cancellation failed";return -2;}return 0;}
    if(opcode==545){if(!host||!host->clear_lasers_with_rewards()){context.error="Enemy full-field laser clear failed";return -2;}return 0;}
    if(opcode==630){const auto* instruction=context.instruction;i32 label=0;if(!instruction||instruction->length<20||!context.integer(0,instruction->argument<i32>(0),label))return -2;if(!host||!host->background_interrupt(label)){context.error="Enemy background interrupt service unavailable";return -2;}return 0;}
    if(opcode==615||opcode==616||opcode==712){
        const auto* instruction=context.instruction;float first=0,second=0;if(!instruction||instruction->length<20||!context.floating(0,instruction->argument<float>(0),first))return -2;
        if(!host){context.error="Enemy cancellation host unavailable";return -2;}
        if(opcode==712){if(instruction->length<24||!context.floating(1,instruction->argument<float>(1),second))return -2;auto* vm=visuals.host?visuals.host->find(enemy.animation_handles[0]):nullptr;
            if(!vm){context.error="Enemy rectangle cancellation animation unavailable";return -2;}if(!host->cancel_bullets_rectangle(enemy.motion.position,{first,second},vm->variables.rotation.z,1)){context.error="Enemy rectangle cancellation failed";return -2;}
        }else{const i32 reward=opcode==615?1:0;if(!host->cancel_bullets_circle(enemy.motion.position,first,reward,false)||!host->cancel_lasers_circle(enemy.motion.position,first,reward,true)){context.error="Enemy circle cancellation failed";return -2;}}return 0;
    }
    // These target-build cases branch straight to the dispatch epilogue.
    if(opcode==901||opcode==902)return 0;
    if(opcode==302||opcode==303||opcode==306||opcode==313||opcode==316||opcode==317||opcode==318||opcode==319||opcode==320||opcode==322||(opcode>=325&&opcode<=333)||opcode==335||opcode==336||opcode==550||opcode==552)return visuals.execute(context,opcode);
    if(opcode>=600&&opcode<=641&&opcode!=629&&opcode!=632&&opcode!=633)return bullets.execute(context,opcode);
    if(opcode==700||opcode==701)return bullets.execute(context,opcode);
    if(opcode>=702&&opcode<=714)return lasers.execute(context,opcode);
    const auto* instruction=context.instruction;if(!instruction){context.error="Enemy instruction unavailable";return -2;}bool ok=true;
    auto arg=[&](u32 index){float value=0;if(20+index*4>instruction->length){context.error="Truncated enemy movement arguments";ok=false;}else if(!context.floating(index,instruction->argument<float>(index),value))ok=false;return value;};
    auto integer_arg=[&](u32 index){i32 value=0;if(20+index*4>instruction->length){context.error="Truncated enemy movement arguments";ok=false;}else if(!context.integer(index,instruction->argument<i32>(index),value))ok=false;return value;};
    switch(opcode){
        case 307:case 308:case 337:{EnemyCommandHost::EffectRequest request;if(opcode==337){const float y=arg(3),x=arg(2);request.position={float(enemy.motion.position.x+x),float(enemy.motion.position.y+y),float(enemy.motion.position.z+0.f)};request.rotation=arg(4);request.script=integer_arg(1);request.resource=integer_arg(0);}else if(opcode==307){request.script=integer_arg(1);request.resource=integer_arg(0);request.position=enemy.motion.position;}else{request.resource=integer_arg(0);request.script=integer_arg(1);}if(!ok)return -2;if(!host||!host->effect(request)){context.error="Enemy independent effect service unavailable";return -2;}break;}
        case 300:case 301:case 304:case 305:case 309:case 310:case 311:case 312:case 321:{
            if(opcode>=309&&opcode<=312){if(!spawning){context.error="Enemy spawn host unavailable";return -2;}if(spawning->boss_exists(0))break;}
            if(world.enemy_count>=world.enemy_control)break;
            if(instruction->length<20){context.error="Truncated enemy spawn name";return -2;}
            const u32 name_size=instruction->argument<u32>(0);const auto* bytes=reinterpret_cast<const u8*>(instruction);
            if(!name_size||name_size%4||name_size>instruction->length-20||!std::memchr(bytes+20,0,name_size)||u64(20)+name_size+20>instruction->length){context.error="Invalid enemy spawn arguments";return -2;}
            EnemySpawnRequest request;request.routine=reinterpret_cast<const char*>(bytes+20);const u32 offset=20+name_size;
            auto number=[&](u32 index){float literal,value=0;std::memcpy(&literal,bytes+offset+(index-1)*4,4);if(!context.floating(index,literal,value))ok=false;return value;};
            auto value=[&](u32 index){i32 literal,result=0;std::memcpy(&literal,bytes+offset+(index-1)*4,4);if(!context.integer(index,literal,result))ok=false;return result;};
            request.position={number(1),number(2),0};
            if(opcode==300||opcode==304||opcode==309||opcode==311||opcode==321){request.position.x=float(request.position.x+enemy.motion.position.x);request.position.y=float(request.position.y+enemy.motion.position.y);}
            request.mirrored=opcode==304||opcode==305||opcode==311||opcode==312;
            if(enemy.flags&0x80000){request.position.x=-request.position.x;request.mirrored=!request.mirrored;}
            request.life=value(3);request.score=value(4);request.item=value(5);request.integers=enemy.integer_registers;request.floats=enemy.float_registers;request.temporary=enemy.temporary_registers;request.parent=enemy.id;
            if(!ok)return -2;if(!spawning||!spawning->spawn_at_rate(request,context.current_interpolation_rate())){const auto* detail=spawning?spawning->failure():nullptr;context.error=detail?*detail:"Enemy creation failed";return -2;}break;
        }
        case 400:case 402:{auto& target=opcode==400?enemy.absolute:enemy.relative;const float x=arg(0),y=arg(1);if(!ok)return -2;if(double(x)>-999999)target.position.x=x;if(double(y)>-999999)target.position.y=y;target.flags&=~15u;enemy.motion.position={float(enemy.relative.position.x+enemy.absolute.position.x),float(enemy.relative.position.y+enemy.absolute.position.y),float(enemy.relative.position.z+enemy.absolute.position.z)};enemy.recompose();break;}
        case 416:case 417:{auto& target=opcode==416?enemy.absolute:enemy.relative;const float x=arg(0),y=arg(1),z=arg(2);if(!ok)return -2;target.position={float(x+target.position.x),float(y+target.position.y),float(z+target.position.z)};enemy.recompose();break;}
        case 401:case 403:case 436:case 437:case 434:case 435:case 438:case 439:{
            const bool absolute=opcode==401||opcode==436||opcode==434||opcode==438,axes=opcode==434||opcode==435||opcode==438||opcode==439,delta=opcode>=436;auto& motion=absolute?enemy.absolute:enemy.relative;auto& interpolation=absolute?enemy.absolute_position:enemy.relative_position;float x=arg(axes?3:2),y=arg(axes?4:3);const i32 duration=integer_arg(0);if(!ok)return -2;if(duration<=0){interpolation.duration=0;break;}
            if(delta){x=enemy.flags&0x80000?float(motion.position.x-x):float(motion.position.x+x);y=float(motion.position.y+y);}interpolation.duration=integer_arg(0);interpolation.control1=interpolation.control2={};if(axes){interpolation.flags|=1;interpolation.axis_modes[0]=integer_arg(1);interpolation.axis_modes[1]=integer_arg(2);}else{interpolation.mode=integer_arg(1);interpolation.flags&=~1u;}interpolation.start={motion.position.x,motion.position.y,motion.position.z};interpolation.end={double(x)>-999999?x:motion.position.x,double(y)>-999999?y:motion.position.y,0};interpolation.timer.set(0);motion.flags&=~15u;break;
        }
        case 425:case 426:{auto& motion=opcode==425?enemy.absolute:enemy.relative;auto& interpolation=opcode==425?enemy.absolute_position:enemy.relative_position;const float x=arg(3),y=arg(4),c1x=arg(1),c1y=arg(2),c2x=arg(5),c2y=arg(6);const i32 duration=integer_arg(0);if(!ok)return -2;interpolation.begin(duration,8,{motion.position.x,motion.position.y,motion.position.z},{double(x)>-999999?x:motion.position.x,double(y)>-999999?y:motion.position.y,0});interpolation.control1={c1x,c1y,0};interpolation.control2={c2x,c2y,0};motion.flags&=~15u;break;}
        case 404:case 406:case 428:case 430:{auto& target=opcode==404||opcode==428?enemy.absolute:enemy.relative;float angle=arg(0);const float speed=arg(1);if(!ok)return -2;if(double(angle)>-999999){if((enemy.flags&0x80000)&&(opcode==404||opcode==406)){angle=normalize_angle(float(angle-1.57079637050628662109375f));angle=normalize_angle(float(1.57079637050628662109375f-angle));}target.set_angle(angle);}if(double(speed)>-999999)target.speed=speed;target.flags&=~15u;break;}
        case 405:case 407:case 429:case 431:case 441:case 443:{
            const bool only_angle=opcode==441||opcode==443,absolute=opcode==405||opcode==429||opcode==441;auto& motion=absolute?enemy.absolute:enemy.relative;auto& angle_curve=absolute?enemy.absolute_angle:enemy.relative_angle;auto& speed_curve=absolute?enemy.absolute_speed:enemy.relative_speed;
            float angle=arg(2),speed=only_angle?0:arg(3);const i32 duration=integer_arg(0);if(!ok)return -2;if(duration<=0){angle_curve.duration=0;if(!only_angle)speed_curve.duration=0;break;}
            const i32 mode=integer_arg(1);const bool mirrored=(enemy.flags&0x80000)&&(opcode==405||opcode==407||only_angle);
            if(mode==7){if(double(angle)>-999999){if(mirrored)angle=-angle;}else angle=0;if(!only_angle&&!(double(speed)>-999999))speed=0;}
            else{if(double(angle)>-999999){if(mirrored)angle=mirror_angle(angle);}else angle=motion.angle;if(!only_angle&&!(double(speed)>-999999))speed=motion.speed;}
            float start=motion.angle;if(!(std::fabs(float(start-angle))<3.1415927410125732421875f)){
                if(only_angle){if(angle>start)angle=float(angle+6.283185482025146484375f);else start=float(start+6.283185482025146484375f);}
                else{if(angle>start)start=float(start+6.283185482025146484375f);else angle=float(angle+6.283185482025146484375f);}
            }
            scalar_curve(angle_curve,integer_arg(0),mode,start,angle);if(!only_angle)scalar_curve(speed_curve,integer_arg(0),mode,motion.speed,speed);motion.flags&=~15u;break;
        }
        case 440:case 442:{auto& motion=opcode==440?enemy.absolute:enemy.relative;float angle=arg(0);if(!ok)return -2;if(enemy.flags&0x80000)angle=mirror_angle(angle);motion.set_angle(angle);motion.flags&=~15u;break;}
        case 444:case 446:{auto& motion=opcode==444?enemy.absolute:enemy.relative;const float speed=arg(0);if(!ok)return -2;if(double(speed)>-999999)motion.speed=speed;motion.flags&=~15u;break;}
        case 445:case 447:{auto& motion=opcode==445?enemy.absolute:enemy.relative;auto& curve=opcode==445?enemy.absolute_speed:enemy.relative_speed;float speed=arg(2);const i32 duration=integer_arg(0);if(!ok)return -2;if(duration<=0){curve.duration=0;break;}const i32 mode=integer_arg(1);if(!ok)return -2;if(!(double(speed)>-999999))speed=mode==7?0:motion.speed;scalar_curve(curve,integer_arg(0),mode,motion.speed,speed);if(!ok)return -2;motion.flags&=~15u;break;}
        case 412:case 413:{
            if(!world.random){context.error="Enemy random steering state unavailable";return -2;}
            auto& random=*world.random;constexpr float pi=3.1415927410125732421875f,half_pi=1.57079637050628662109375f;float angle;
            const float x=enemy.motion.position.x,quarter_width=float(enemy.bound_size.x*.25f);
            if(float(enemy.bound_center.x-quarter_width)>x)angle=float(float(random.signed_unit()*pi)/3.f);
            else if(x>float(enemy.bound_center.x+quarter_width))angle=normalize_angle(float(float(float(random.signed_unit()*pi)/3.f)+pi));
            else{angle=float(float(random.signed_unit()*pi)*.25f);const u16 word=random.next16();if(world.player_position.x>x?word%3==0:word%3!=0)angle=float(angle+pi);}
            const float quarter_height=float(enemy.bound_size.y*.25f),y=enemy.motion.position.y;
            if(float(enemy.bound_center.y-quarter_height)>y)angle=std::fabs(angle);else if(y>float(enemy.bound_center.y+quarter_height))angle=-std::fabs(angle);
            if(std::fabs(float(angle+half_pi))<.05000000074505806f)angle=angle<-half_pi?-1.6207963228225708f:-1.5207964181900024f;
            else if(double(std::fabs(float(angle-half_pi)))<.05)angle=angle<half_pi?1.5207964181900024f:1.6207963228225708f;
            const i32 mode=integer_arg(1);const float speed=arg(2);const i32 duration=integer_arg(0);if(!ok)return -2;
            auto& motion=opcode==412?enemy.absolute:enemy.relative;auto& angle_curve=opcode==412?enemy.absolute_angle:enemy.relative_angle;auto& speed_curve=opcode==412?enemy.absolute_speed:enemy.relative_speed;
            scalar_curve(angle_curve,duration,mode,angle,angle);const i32 speed_duration=integer_arg(0);if(!ok)return -2;
            // The original writes the angle easing twice and retains the speed
            // interpolator's previous easing mode. Preserve that side effect.
            scalar_curve(speed_curve,speed_duration,speed_curve.mode,speed,0);motion.flags&=~15u;break;
        }
        case 408:case 410:case 420:case 422:{
            auto& motion=opcode==408||opcode==420?enemy.absolute:enemy.relative;const float angle=arg(0),speed=arg(1),radius=arg(2),velocity=arg(3),axis=opcode>=420?arg(4):0,scale=opcode>=420?arg(5):0;if(!ok)return -2;
            if((motion.flags&15)!=2)motion.velocity=motion.position;if(double(angle)>-999999)motion.set_angle(angle);if(double(speed)>-999999)motion.speed=speed;if(double(radius)>-999999)motion.radius=radius;if(double(velocity)>-999999)motion.angular_velocity=velocity;
            if(opcode>=420){if(double(axis)>-999999)motion.axis_angle=normalize_angle(normalize_angle(axis));if(double(scale)>-999999)motion.axis_scale=scale;motion.flags=(motion.flags&0xfffffff3u)|3;}else motion.flags=(motion.flags&0xfffffff2u)|2;
            restart_motion(motion);enemy.recompose();break;
        }
        case 409:case 411:case 421:case 423:{
            const bool absolute=opcode==409||opcode==421,ellipse=opcode>=421;auto& motion=absolute?enemy.absolute:enemy.relative;auto& speed_curve=absolute?enemy.absolute_speed:enemy.relative_speed;auto& shape=absolute?enemy.absolute_shape:enemy.relative_shape;auto& axis_curve=absolute?enemy.absolute_wave:enemy.relative_wave;
            float speed=arg(2),radius=arg(3),velocity=arg(4),axis=ellipse?arg(5):0,scale=ellipse?arg(6):0;
            if(!(double(speed)>-999999))speed=motion.speed;if(!(double(radius)>-999999))radius=motion.radius;if(!(double(velocity)>-999999))velocity=motion.angular_velocity;if(ellipse){if(!(double(axis)>-999999))axis=motion.axis_angle;if(!(double(scale)>-999999))scale=motion.axis_scale;}
            const i32 duration=integer_arg(0),mode=integer_arg(1);if(!ok)return -2;if(duration<=0){speed_curve.duration=shape.duration=0;if(ellipse)axis_curve.duration=0;break;}
            scalar_curve(speed_curve,duration,mode,motion.speed,speed);shape_curve(shape,duration,mode,{motion.radius,motion.angular_velocity},{radius,velocity});if(ellipse)shape_curve(axis_curve,duration,mode,{motion.axis_angle,motion.axis_scale},{axis,scale});
            if(ellipse){motion.velocity=motion.position;motion.flags=(motion.flags&0xfffffff3u)|3;}else motion.flags=(motion.flags&0xfffffff2u)|2;restart_motion(motion);enemy.recompose();break;
        }
        case 432:case 433:{const i32 id=integer_arg(0);if(!ok)return -2;auto* target=world.lookup(u32(id));if(!target){context.error="Enemy movement target unavailable";return -2;}(opcode==432?enemy.absolute:enemy.relative).position=target->motion.position;break;}
        case 414:case 415:if(!world.boss){context.error="Enemy movement boss target unavailable";return -2;}else(opcode==414?enemy.absolute:enemy.relative).position=world.boss->motion.position;break;
        case 424:{i32 value;if(instruction->length<20||!context.integer(0,instruction->argument<i32>(0),value)){context.error="Enemy mirror argument unavailable";return -2;}enemy.flags=(enemy.flags&~0x80000u)|(u32(value)<<19&0x80000u);break;}
        case 504:{const float x=arg(0),y=arg(1),width=arg(2),height=arg(3);if(!ok)return -2;enemy.flags|=0x20000;enemy.bound_center={x,y};enemy.bound_size={width,height};
            // Purple 42b261 runs once after the four authored arguments.
            // This advanced hook applies outside Practice too.
            if(world.practice&&world.practice->enabled&&world.practice->force_boss_move_down){
                practice_boss_range(&enemy.bound_center.y,&enemy.bound_size.y,world.practice->boss_move_down_range);
            }
            break;}
        case 505:enemy.flags&=~0x20000u;break;
        case 500:enemy.hitbox={arg(0),arg(1)};if(!enemy.chapter_contribution){enemy.chapter_contribution=1;world.chapter_total=wrapping_add(world.chapter_total,1);}break;
        case 501:enemy.hurtbox={arg(0),arg(1)};break;
        case 502:case 503:{const u32 flags=u32(integer_arg(0));if(!ok)return -2;if(opcode==502)enemy.flags|=flags;else enemy.flags&=~flags;const bool paused=(enemy.flags&32)!=0;if(paused==(opcode==502)){for(u32 handle:enemy.animation_handles)if(handle&&(!host||!host->pause_animation(handle,paused))){context.error="Enemy animation pause host unavailable";return -2;}}break;}
        case 506:enemy.item_drops={};break;
        case 507:{const i32 index=integer_arg(0),value=integer_arg(1);if(!ok)return -2;if(index<0||index>16){context.error="Enemy drop index outside resource range";return -2;}if(index==0)enemy.primary_drop=value;else enemy.item_drops[index-1]=value;break;}
        case 508:enemy.drop_radius={arg(0),arg(1)};break;
        case 509:if((world.mode_flags&48)!=32&&(!host||!host->drop_items(enemy))){context.error="Enemy item-drop service unavailable";return -2;}break;
        case 510:enemy.primary_drop=integer_arg(0);break;
        case 511:{const i32 life=integer_arg(0);if(!ok)return -2;enemy.life=enemy.initial_life=enemy.phase_life=life;enemy.life_budget=wrapping_sub(wrapping_mul(life,8),life);if(enemy.flags&0x800000)enemy.flags|=0x40000000;break;}
        case 512:{const i32 slot=integer_arg(0);if(!ok)return -2;if(world.practice&&world.practice->enabled)world.practice->lock_timer.reset();world.manager_flags&=~1u;if(slot<0){if(enemy.flags&0x800000){if(enemy.boss_slot<0||enemy.boss_slot>=3){context.error="Enemy Boss slot outside range";return -2;}world.boss_ids[enemy.boss_slot]=0;}enemy.flags&=~0x800000u;}else{if(slot>=3){context.error="Enemy Boss slot outside range";return -2;}enemy.flags|=0x800000;world.boss_ids[slot]=enemy.id;enemy.boss_slot=slot;}world.refresh_boss();break;}
        case 513:enemy.age_timer.set(0);break;
        case 514:case 521:case 556:{
            const auto read_name=[&](u32 offset,std::string& out){if(offset>=instruction->length){context.error="Truncated enemy script name";return false;}const char* data=reinterpret_cast<const char*>(instruction)+offset;u32 length=0;while(offset+length<instruction->length&&data[length])length++;if(offset+length==instruction->length||length>=64){context.error="Invalid enemy script name";return false;}out.assign(data,length);return true;};
            if(opcode==556){if(!read_name(20,enemy.death_script))return -2;break;}
            if(opcode==514){
                if(world.practice&&world.practice->enabled)world.practice->lock_timer.reset();
                const i32 time=integer_arg(2),life=((world.mode_flags&48)==32&&(enemy.flags&0x800000))?0:integer_arg(1),slot=integer_arg(0);if(!ok)return -2;if(slot<0||slot>=8){context.error="Enemy interrupt index outside resource range";return -2;}auto& interrupt=enemy.interrupts[slot];interrupt.life=life;
                if(life>=0){interrupt.time=time;if((world.mode_flags&48)==32&&(enemy.flags&0x800000)){interrupt.script="BossDead";interrupt.timeout_script="BossEscape";}else{if(!read_name(32,interrupt.script))return -2;interrupt.timeout_script=interrupt.script;}}
            }else{const i32 slot=integer_arg(0);if(!ok)return -2;if(slot<0||slot>=8){context.error="Enemy interrupt index outside resource range";return -2;}if(!read_name(24,enemy.interrupts[slot].timeout_script))return -2;}break;
        }
        case 515:enemy.collision_timer.set(integer_arg(0));break;
        case 516:{const i32 id=integer_arg(0);if(!ok)return -2;if(!host||!host->sound(id,enemy.motion.position)){context.error="Enemy sound host unavailable";return -2;}break;}
        case 517:{ScreenNudgeSpec s; s.last=integer_arg(2);s.first=integer_arg(1);s.duration=integer_arg(0);if(!ok)return -2;if(!host||!host->nudge(s)){context.error="Enemy screen nudge service unavailable";return -2;}break;}
        case 518:{const i32 id=integer_arg(0);if(!ok)return -2;if(!host||!host->scene_message(id)||!host->cancel_all_bullets(0)||!host->cancel_all_lasers(0,false)||!spawning||!spawning->clear_field(false)){context.error="Enemy message and scene clear failed";return -2;}break;}
        case 519:{bool complete=false;if(!host||!host->message_complete(complete)){context.error="Enemy message status unavailable";return -2;}if(!complete)return -1;break;}
        case 520:if(world.boss_at(0)||world.boss_at(1)||world.boss_at(2))return -1;break;
        case 523:if(!host||!host->finish_spell()){context.error="Enemy spell finish failed";return -2;}enemy.life_flags&=~1u;break;
        case 524:{const i32 chapter=integer_arg(0);if(!ok)return -2;world.request_chapter(chapter);enemy.chapter=chapter;enemy.chapter_contribution=0;break;}
        case 525:case 571:if(!spawning||!spawning->clear_field(opcode==571)){context.error="Enemy scene clear failed";return -2;}break;
        // Retail 0x42b275 squares the evaluated float into enemy +0x4074;
        // bullet emission copies that value to its minimum-distance gate.
        case 526:{const float distance=arg(0);if(!ok)return -2;enemy.minimum_bullet_distance_squared=float(distance*distance);break;}
        case 527:{const u32 color=u32(integer_arg(2));const float fraction=float(arg(1)/float(enemy.initial_life));const i32 index=integer_arg(0);if(!ok)return -2;if(!host||!host->boss_segment(enemy.boss_slot,index,fraction,color)){context.error="Boss health segment service unavailable";return -2;}break;}
        case 540:{const i32 count=integer_arg(0);if(!ok)return -2;if(!host||!host->boss_segments(count)){context.error="Boss health display service unavailable";return -2;}break;}
        case 541:enemy.invulnerability_timer.set(integer_arg(0));break;
        case 542:world.spell_flags()|=8;break;
        case 544:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&~0x8000000u)|(value<<27&0x8000000u);break;}
        case 546:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&~0x10000000u)|(value<<28&0x10000000u);const i32 script=integer_arg(1);if(!ok)return -2;enemy.flags&=~0x20000001u;enemy.inactive_animation=script;enemy.normal_animation=enemy.animation_base;break;}
        case 547:{const float value=arg(0);if(!ok)return -2;if(!world.frame_rate){context.error="Enemy shared frame clock unavailable";return -2;}*world.frame_rate=value;break;}
        case 548:{std::array<i32,4> delays;for(u32 i=0;i<4;i++)delays[i]=integer_arg(i);if(!ok)return -2;const i32 index=world.difficulty>=0&&world.difficulty<=2?world.difficulty:3;context.time=float(context.time-float(delays[index]));break;}
        case 549:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&0x7fffffffu)|(value<<31);break;}
        case 553:enemy.hit_sound=integer_arg(0);break;
        case 554:if(!host||!host->stage_logo()){context.error="Stage logo service unavailable";return -2;}break;
        case 555:{const u32 id=u32(integer_arg(1));if(!ok)return -2;auto* destination=context.integer_destination(0);if(!destination){context.error="Enemy lookup destination unavailable";return -2;}*destination=world.find(id)?1:0;break;}
        case 557:{const i32 duration=integer_arg(0),mode=integer_arg(1);const u32 color=u32(integer_arg(2));const float far_value=arg(4),near_value=arg(3);if(!ok)return -2;StageFog target;target.set(color,near_value,far_value);if(!host||!host->background_fog(duration,mode,target)){context.error="Enemy background fog service unavailable";return -2;}break;}
        case 558:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&~0x80000u)|(value<<19&0x80000u);break;}
        case 559:world.enemy_control=integer_arg(0);break;
        case 563:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&~0x1000u)|(value<<12&0x1000u);break;}
        case 564:enemy.rotation_angle=arg(0);break;
        case 565:enemy.damage_multiplier=arg(0);break;
        case 568:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.life_flags=(enemy.life_flags&~1u)|(value&1u);break;}
        case 569:{const i32 previous=enemy.chapter_contribution;if(previous==0||previous==1){const i32 value=integer_arg(0);if(!ok)return -2;enemy.chapter_contribution=value;if(previous==1)world.chapter_total=wrapping_add(wrapping_add(world.chapter_total,-1),value);else if(value>0)world.chapter_total=wrapping_add(world.chapter_total,value);}break;}
        case 570:if(enemy.chapter_contribution&&enemy.chapter==world.current_chapter){world.chapter_defeated=wrapping_add(world.chapter_defeated,enemy.chapter_contribution);enemy.chapter_contribution=0;}break;
        case 535:{std::array<i32,4> values;for(u32 i=0;i<4;i++)values[i]=integer_arg(i+1);if(!ok)return -2;auto* destination=context.integer_destination(0);if(!destination){context.error="Enemy difficulty integer destination unavailable";return -2;}*destination=values[world.difficulty>=0&&world.difficulty<3?world.difficulty:3];break;}
        case 536:{std::array<float,4> values;for(u32 i=0;i<4;i++)values[i]=arg(i+1);if(!ok)return -2;auto* destination=context.float_destination(0);if(!destination){context.error="Enemy difficulty float destination unavailable";return -2;}*destination=values[world.difficulty>=0&&world.difficulty<3?world.difficulty:3];break;}
        case 427:enemy.absolute.position={float(enemy.absolute.position.x+enemy.relative.position.x),float(enemy.absolute.position.y+enemy.relative.position.y),float(enemy.absolute.position.z+enemy.relative.position.z)};enemy.relative.position={};enemy.absolute.speed=enemy.relative.speed=0;enemy.absolute.velocity=enemy.relative.velocity={};enemy.absolute.flags&=~15u;enemy.relative.flags&=~15u;enemy.absolute_position.duration=enemy.relative_position.duration=0;enemy.absolute_angle.duration=enemy.relative_angle.duration=enemy.absolute_speed.duration=enemy.relative_speed.duration=0;enemy.absolute_shape.duration=enemy.relative_shape.duration=enemy.absolute_wave.duration=enemy.relative_wave.duration=0;break;
        case 800:{const u32 identifier=u32(integer_arg(0));if(!ok)return -2;if(!world.lookup(identifier))break;const u32 length=instruction->argument<u32>(1);if(instruction->length<25||!length||length>instruction->length-24||!std::memchr(reinterpret_cast<const u8*>(instruction)+24,0,length)){context.error="Invalid enemy replacement routine";return -2;}const std::string name(reinterpret_cast<const char*>(instruction)+24);if(!spawning||!spawning->switch_enemy_routine(identifier,name)){const auto* cause=spawning?spawning->failure():nullptr;context.error=cause?*cause:"Enemy routine replacement unavailable";return -2;}break;}
        case 629:{if(enemy.distortion.active&&(!host||!host->retire_distortion(enemy))){context.error="Enemy distortion retirement unavailable";return -2;}enemy.distortion.active=false;const float radius=arg(0);enemy.distortion.target=radius;enemy.distortion.radius=16;enemy.distortion.color=u32(integer_arg(1));enemy.distortion.phase_x=enemy.distortion.phase_y=0;if(!ok)return -2;if(radius>0){if(!host||!host->begin_distortion(enemy)){context.error="Enemy distortion creation unavailable";return -2;}enemy.distortion.active=true;}break;}
        case 632:{const i32 rule=integer_arg(0);if(!ok)return -2;if(rule<0||rule>4){context.error="Enemy update callback not reconstructed: "+std::to_string(rule);return -2;}enemy.update_rule=EnemyUpdateRule(rule);enemy.callback_state=0;break;}
        case 633:{const i32 rule=integer_arg(0);if(!ok)return -2;if(rule<0||rule>2){context.error="Enemy damage callback outside original table";return -2;}enemy.damage_rule=EnemyDamageRule(rule);break;}
        case 1001:case 1002:enemy.script_control[opcode-1001]=integer_arg(0);break;
        default:context.error="Unimplemented enemy ECL opcode "+std::to_string(opcode);return -2;
    }return ok?0:-2;
}
}
