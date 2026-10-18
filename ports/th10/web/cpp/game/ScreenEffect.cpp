#include "ScreenEffect.hpp"
namespace th10 {
static i32 subtract(i32 first,i32 second){const u32 bits=static_cast<u32>(first)-static_cast<u32>(second);i32 result;std::memcpy(&result,&bits,4);return result;}
void ScreenEffect::initialize() noexcept {std::memset(this,0,sizeof(*this));flags=2;}
// 0x43c970. Update callbacks are enabled immediately, at priority 14. Shakes
// have no drawing callback; all other effects draw at the caller's priority.
void ScreenEffect::start(ScreenEffectKind type,i32 frames,i32 first,i32 second,i32 third,i32 layer,ScreenEffectEnvironment& env){
    const auto index=static_cast<u32>(type);
    if(index<=8){if(type==ScreenEffectKind::RevealPlayfield)alpha=255;update_entry=(*env.chain)->add(env.update_callbacks[index],this,14,false,true,*env.callbacks);if(env.draw_callbacks[index])draw_entry=(*env.chain)->add(env.draw_callbacks[index],this,layer,true,true,*env.callbacks);}
    update_entry->notify_callback=env.delete_callback;
#ifdef TH_NATIVE_PLATFORM
    update_entry->notification=env.callbacks->resolve(env.delete_callback);
#endif
    if(!(timer_flags&1)){timer.rate=env.rate;timer_flags|=1;}timer.initialize(-1);
    duration=frames;kind=type;parameters[0]=first;parameters[1]=second;parameters[2]=third;
}
ScreenEffect* ScreenEffect::create(ScreenEffectKind type,i32 frames,i32 first,i32 second,i32 third,i32 layer,ScreenEffectEnvironment& env){auto* effect=env.allocate();if(effect){effect->initialize();effect->start(type,frames,first,second,third,layer,env);}return effect;}
void ScreenEffect::release(ScreenEffectEnvironment& env){(*env.chain)->remove_locked(update_entry,*env.callbacks);(*env.chain)->remove_locked(draw_entry,*env.callbacks);}
// 0x43c930. Begin the eight-frame recovery of a held dimming effect.
void ScreenEffect::fade_out(float* rate) noexcept {releasing=1;if(!(timer_flags&1)){timer.rate=rate;timer_flags|=1;}timer.initialize(-1);}
// 0x43bd40. Reveal effects keep ticking while the game is paused.
i32 ScreenEffect::reveal(ScreenEffectEnvironment& env){
    if(*env.quitting)return 7;
    if(duration){alpha=(number(255.0f)-number(timer.fractional)*number(255.0f)/Extended::from_int(duration)).truncate_int();if(alpha<0)alpha=0;}
    if(timer.current>=duration)return 7;timer.tick();return 1;
}
// 0x43c230. Hide effects hold their last opaque frame for two extra frames.
i32 ScreenEffect::hide(ScreenEffectEnvironment& env){
    if(*env.quitting)return 7;
    if(duration){alpha=timer.current<duration?(number(timer.fractional)*number(255.0f)/Extended::from_int(duration)).truncate_int():255;if(alpha<0)alpha=0;}
    if(timer.current>=wrapping_add(duration,2))return 7;
    if(!env.controller_flags||!(*env.controller_flags&5))timer.tick();return 1;
}
// 0x43c310. A dimming effect holds at half opacity until explicitly released.
i32 ScreenEffect::dim(){
    if(!releasing){if(duration&&timer.current<=duration)alpha=(number(timer.fractional)*number(128.0f)/Extended::from_int(duration)).truncate_int();}
    else{if(timer.current>8)return 7;alpha=subtract(128,(number(timer.fractional)*number(16.0f)).truncate_int());}
    timer.tick();return 1;
}
// 0x43c460. Flash completion returns 0 (remove callback), unlike the other
// effects' 7 (notify and destroy owner); this distinction is in the original.
i32 ScreenEffect::flash(ScreenEffectEnvironment& env){
    if(*env.quitting)return 0;
    if(timer.current<duration){const auto maximum=static_cast<u32>(parameters[1])>>24;alpha=subtract(static_cast<i32>(maximum),(Extended::from_int(maximum)*number(timer.fractional)/Extended::from_int(duration)).truncate_int());if(alpha<0)alpha=0;}
    else{alpha=0;parameters[0]=subtract(parameters[0],1);if(parameters[0]<=0)return 0;if(!(timer_flags&1)){timer.rate=env.rate;timer_flags|=1;}timer.initialize(-1);}
    timer.tick();return 1;
}
static u32 duplicated_random(Rng& random){random.next_word();const u32 word=random.next_word();return ((word<<16)|word)%3;}
static void shake_axis(float& output,const Extended& amplitude,u32 choice){output=choice==0?0.0f:(choice==1?amplitude:-amplitude).to_float();}
// 0x43c550. The inlined RNG uses its second word twice, which differs from
// the ordinary bounded RNG used by the envelope variant.
i32 ScreenEffect::shake_linear(ScreenEffectEnvironment& env){
    if(*env.quitting)return 7;timer.tick();if(timer.current>=duration)return 7;
    const auto amplitude=Extended::from_int(subtract(parameters[1],parameters[0]))*number(timer.fractional)/Extended::from_int(duration)+Extended::from_int(parameters[0]);
    shake_axis(env.camera_offset->x,amplitude,duplicated_random(*env.random));shake_axis(env.camera_offset->y,amplitude,duplicated_random(*env.random));return 1;
}
// 0x43c710. Attack, hold, release timings use signed comparisons, while the
// release ratio converts its duration and total to unsigned values.
i32 ScreenEffect::shake_envelope(ScreenEffectEnvironment& env){
    if(*env.quitting)return 7;if(!env.controller_flags||(*env.controller_flags&0x77))return 1;
    timer.tick();const i32 attack=parameters[0],hold=wrapping_add(attack,parameters[1]),end=wrapping_add(hold,parameters[2]);Extended amount;
    if(timer.current<attack)amount=number(timer.fractional)/Extended::from_int(attack);
    else if(timer.current<hold)amount=number(1.0f);
    else if(timer.current<end){auto total=Extended::from_int(end),release=Extended::from_int(parameters[2]);if(end<0)total=total+number(4294967296.0f);if(parameters[2]<0)release=release+number(4294967296.0f);amount=(total-number(timer.fractional))/release;}
    else return 7;
    const auto amplitude=number((Extended::from_int(duration)*amount).to_float());
    shake_axis(env.camera_offset->x,amplitude,env.random->bounded(3));shake_axis(env.camera_offset->y,amplitude,env.random->bounded(3));return 1;
}
i32 ScreenEffect::update(ScreenEffectEnvironment& env){switch(kind){case ScreenEffectKind::RevealScreen:case ScreenEffectKind::RevealPlayfield:return reveal(env);case ScreenEffectKind::HideScreen:case ScreenEffectKind::HidePlayfield:return hide(env);case ScreenEffectKind::DimScreen:case ScreenEffectKind::DimPlayfield:return dim();case ScreenEffectKind::FlashPlayfield:return flash(env);case ScreenEffectKind::ShakeLinear:return shake_linear(env);case ScreenEffectKind::ShakeEnvelope:return shake_envelope(env);}return 1;}
i32 ScreenEffect::draw(ScreenEffectEnvironment& env){
    const bool full=kind==ScreenEffectKind::RevealScreen||kind==ScreenEffectKind::HideScreen||kind==ScreenEffectKind::DimScreen;
    return draw_region(env,full,kind==ScreenEffectKind::RevealScreen||kind==ScreenEffectKind::HideScreen,kind==ScreenEffectKind::FlashPlayfield);
}
i32 ScreenEffect::draw_region(ScreenEffectEnvironment& env,bool full,bool set_viewport,bool flash){
    if(set_viewport)env.fullscreen_viewport();
    const auto color=flash?static_cast<u32>(parameters[1])&0xffffff:static_cast<u32>(parameters[0]);
    env.rectangle(full?ScreenRect{0,0,640,480}:ScreenRect{32,16,416,464},(static_cast<u32>(env.presentation_alpha(*this))<<24)|color);return 1;
}
// 0x43bda0 / 0x43bfa0. Four untextured vertices form a triangle strip.
void draw_screen_rectangle(const ScreenRect& rect,const u32 colors[4],AnmManager** manager,AnmRenderEnvironment& env){
    AnmRenderer{**manager,env}.flush();
    struct Vertex {Vec3 position;float reciprocal_w;u32 color;};
    const Vertex vertices[]={{{rect.left,rect.top,0},1,colors[0]},{{rect.right,rect.top,0},1,colors[1]},{{rect.left,rect.bottom,0},1,colors[2]},{{rect.right,rect.bottom,0},1,colors[3]}};
    RenderCommands(env).SetColorOp(ColorOp::SelectFirst);RenderCommands(env).SetTextureArg(TextureArg::Diffuse);RenderCommands(env).SetDestinationBlend(BlendMode::InverseSourceAlpha);env.vertex_format(Layouts::Untextured);env.draw_triangles(Primitives::Strip,2,vertices,20);
    auto& active=**manager;active.cached_draw_state[2]=active.cached_draw_state[1]=active.cached_draw_state[3]=255;active.cached_draw_state[0]=3;active.current_texture=nullptr;active.current_uv_sprite=nullptr;
    RenderCommands(env).SetColorOp(ColorOp::Modulate);RenderCommands(env).SetTextureArg(TextureArg::Texture);
}
}
