#pragma once
// Diagnostic-only observer. No game/ANM layout fields and no interpolation policy.
#include <cstdint>
namespace th08 { struct AnmVm; struct EffectState; struct Vec2; struct SpriteVertex; struct UntexturedVertex; struct Matrix4; class AnmRenderer; }
namespace th08::audit {
enum class Owner : uint32_t { Unknown, Title, TitleHelp, Pause, Retry, Bullet, Laser,
    Effect, Enemy, Item, Gui, Background, Spell, Player, PlayerShot, Ascii, PlayerBomb, PlayerOption, AsciiText,
    AsciiScorePopup,AsciiTimePopup };
#if defined(TH_PRESENTATION_AUDIT)
enum ScopeFlag : uint32_t { DiscreteMotion=1 };
struct Scope {
    Scope(Owner,const void*,int32_t age=0,uint32_t part=0,uint32_t flags=0);
    ~Scope();
    Scope(const Scope&)=delete;
    Scope& operator=(const Scope&)=delete;
private:
    uint32_t saved_owner,saved_object,saved_part,saved_ordinal,saved_flags; int32_t saved_age;
};
// Binary wire layout: 12 metadata words, 32 property floats, 16 sampled vertices
// of XYZ/UV/RGBA. Geometry above 16 vertices is explicitly marked sampled.
struct Record { uint32_t meta[12]{}; float values[32]{}; float vertices[16*9]{}; };
static_assert(sizeof(Record)==752);
void enable(bool);
void begin_frame(bool presentation_only,float alpha);
void end_frame();
void capture(const AnmVm&,const SpriteVertex*,uint32_t count,uint32_t kind,const Vec2* screen_shake=nullptr);
void capture_effect(const EffectState&,const SpriteVertex*,uint32_t count);
void capture_fan(const AnmVm&,const UntexturedVertex*,uint32_t count);
void capture_world(const AnmVm&,AnmRenderer&,const Matrix4& submitted_world);
uint32_t tick_id();
#endif
}
#if defined(TH_PRESENTATION_AUDIT)
#define TH08_AUDIT_SCOPE(owner,ptr,age,part) ::th08::audit::Scope th08_audit_scope{::th08::audit::Owner::owner,ptr,int32_t(age),uint32_t(part)}
#define TH08_AUDIT_SCOPE_FLAGS(owner,ptr,age,part,flags) ::th08::audit::Scope th08_audit_scope{::th08::audit::Owner::owner,ptr,int32_t(age),uint32_t(part),uint32_t(flags)}
#define TH08_AUDIT_CAPTURE(vm,vertices,count,kind) ::th08::audit::capture(vm,vertices,count,kind)
#define TH08_AUDIT_SPRITE(vm,vertices,count,shake) ::th08::audit::capture(vm,vertices,count,0,&shake)
#define TH08_AUDIT_EFFECT(effect,vertices,count) ::th08::audit::capture_effect(effect,vertices,count)
#define TH08_AUDIT_FAN(vm,vertices,count) ::th08::audit::capture_fan(vm,vertices,count)
#define TH08_AUDIT_WORLD(vm,renderer,world) ::th08::audit::capture_world(vm,renderer,world)
#define TH08_AUDIT_FRAME(only,alpha) ::th08::audit::begin_frame(only,alpha)
#define TH08_AUDIT_END() ::th08::audit::end_frame()
#else
#define TH08_AUDIT_SCOPE(owner,ptr,age,part) ((void)0)
#define TH08_AUDIT_SCOPE_FLAGS(owner,ptr,age,part,flags) ((void)0)
#define TH08_AUDIT_CAPTURE(vm,vertices,count,kind) ((void)0)
#define TH08_AUDIT_SPRITE(vm,vertices,count,shake) ((void)0)
#define TH08_AUDIT_EFFECT(effect,vertices,count) ((void)0)
#define TH08_AUDIT_FAN(vm,vertices,count) ((void)0)
#define TH08_AUDIT_WORLD(vm,renderer,world) ((void)0)
#define TH08_AUDIT_FRAME(only,alpha) ((void)0)
#define TH08_AUDIT_END() ((void)0)
#endif
