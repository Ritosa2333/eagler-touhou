// C++ port of upstream src/animation.mjs: the presentation-only ANM v7
// sampler that draws what the logical core does not own as a VM (bullets,
// items, options, the HUD frame, the loading screen). It never touches the
// core's RNG. Ages are absolute: a track re-simulates from its binding when
// the requested age goes backwards.
#pragma once
#include "Anm.hpp"
#include <array>
#include <functional>
#include <memory>
#include <optional>

namespace th12host {
// One drawable ANM state, from either this sampler or a core VM pose.
struct Appearance {
    i32 bank=-1,sprite=-1,script=-1,layer=0,mode=0,anchor_x=0,anchor_y=0,blend=0,geometry_count=0;
    float x=0,y=0,z=0,rx=0,ry=0,rotation=0,sx=1,sy=1,u=0,v=0,uv_scale_x=1,uv_scale_y=1,geometry_radius=0,geometry_width=0;
    float r=1,g=1,b=1,alpha=1,r2=0,g2=0,b2=0,alpha2=0;
    bool has_primary=false;float primary[4]{1,1,1,1}; // core poses: primary colour even when colour 2 is selected
    bool visible=true,point=false,rotate=false;
    float native_width=-1,native_height=-1;
    const struct MenuText* text=nullptr; // menu line drawn on this VM (MenuAnimation::set_text)
};
// A text line a menu VM shows instead of its sprite (upstream menuText).
struct MenuText {std::u32string text;u32 color=0xffffff;float width=512,height=17;};
struct BulletVariant {i32 sprite=0,spawn=0,cancel=0;};
struct SampleOptions {
    u32 seed=0x1234;i32 interrupt=0;std::vector<i32> interrupts;bool position_offset=false;
    u64 entity=0; // identity only (upstream: the options JSON in the track key)
};

class AnimationSampler;
class Track {
public:
    struct State {
        i32 sprite=-1;float x=0,y=0,z=0,offset_x=0,offset_y=0,offset_z=0,rx=0,ry=0,rotation=0,sx=1,sy=1;
        float r=1,g=1,b=1,alpha=1,r2=0,g2=0,b2=0,alpha2=0;bool use_color2=false;
        i32 blend=0,layer=0;bool visible=true;i32 mode=0,anchor_x=0,anchor_y=0;float u=0,v=0;bool point=false;
        float uv_scale_x=1,uv_scale_y=1;i32 geometry_count=0;float geometry_radius=0,geometry_width=0;
    };
    struct Frame {Appearance a;std::vector<Frame> children;};
    Track(AnimationSampler& owner,std::shared_ptr<const AnmFile> file,i32 bank,i32 id,std::optional<BulletVariant> variant,const SampleOptions& options,u32 depth=0);
    void tick();
    const Frame& sample(i32 age);
    bool interrupt(i32 code);
    void commands_at_time();
    i32 id()const{return id_;}
    bool ended()const{return ended_;}
    u32 tick_count()const{return ticks;}
    const Frame* frame()const{return has_frame?&frame_:nullptr;}
    std::vector<std::unique_ptr<Track>>& children(){return children_;}
    State state;
private:
    struct Interpolation {std::vector<u8> keys;std::vector<double> from,to;i32 duration=0,mode=0,time=0;bool hermite=false;std::array<double,3> a{},b{};};
    struct Saved {size_t pc;i32 clock;bool stopped;};
    AnimationSampler& owner;std::shared_ptr<const AnmFile> file;const AnmScript* script=nullptr;
    i32 bank,id_;u32 depth;std::optional<BulletVariant> variant;SampleOptions options;
    size_t pc=0;i32 clock=0;u32 ticks=0;bool stopped=false,ended_=false,display_rng=false,detached=false,has_frame=false;
    u32 born=0;std::array<float,3> base{};u32 seed;
    std::vector<std::pair<double,double>> locals; // upstream Map keyed by the (int or float) variable number
    struct {float rx=0,ry=0,rotation=0,sx=0,sy=0,u=0,v=0;} velocity;
    std::vector<std::pair<u32,Interpolation>> interpolations; // insertion order, keyed by the joined key list
    std::vector<std::unique_ptr<Track>> children_;std::optional<Saved> saved;Frame frame_;
    std::optional<double> local(double key)const;void set_local(double key,double value);
    u32 random32(bool force_game=false);double random();
    double value(const AnmCommand& c,u32 i,bool real);
    void interpolate(std::vector<u8> keys,std::vector<double> to,i32 duration,i32 mode,const double* ha=nullptr,const double* hb=nullptr);
    void child(u16 op,i32 script,const AnmCommand* offset=nullptr);
    bool command(const AnmCommand& c);
    float& field(u8 key);
    friend class AnimationSampler;friend class MenuAnimation;
};

// Upstream AnimationSampler: one track per (bank, script, variant, options)
// key, dropped when a frame stops sampling it.
class AnimationSampler {
public:
    std::function<u32()> display_random32; // loading screen's own stream
    std::function<std::shared_ptr<const AnmFile>(i32 bank)> files;
    void begin_frame(){if(++generation==0){for(auto& entry:tracks)entry.second.generation=0;++generation;}}
    void end_frame();
    void reset(){tracks.clear();}
    const Track::Frame* sample(i32 bank,i32 script,i32 age,std::optional<BulletVariant> variant={},const SampleOptions& options={});
    // Root and descendants, flattened in upstream's walk order.
    void sample_all(i32 bank,i32 script,i32 age,std::vector<Appearance>& out,std::optional<BulletVariant> variant={},const SampleOptions& options={});
    static void flatten(const Track::Frame& frame,std::vector<Appearance>& out);
private:
    struct Key {i32 bank,script;bool has_variant;BulletVariant variant;u64 entity;u32 seed;i32 interrupt;std::vector<i32> interrupts;bool position_offset;
        bool operator==(const Key& o)const{return bank==o.bank&&script==o.script&&has_variant==o.has_variant&&(!has_variant||(variant.sprite==o.variant.sprite&&variant.spawn==o.variant.spawn&&variant.cancel==o.variant.cancel))&&entity==o.entity&&seed==o.seed&&interrupt==o.interrupt&&interrupts==o.interrupts&&position_offset==o.position_offset;}};
    struct KeyHash {size_t operator()(const Key& k)const;};
    struct CachedTrack {std::unique_ptr<Track> track;u64 generation;};
    std::unordered_map<Key,CachedTrack,KeyHash> tracks;
    u64 generation=1;
};

// Upstream MenuAnimation: a long-lived binding ticked by its owner. Menus
// steer it with interrupts and per-script overrides.
class MenuAnimation {
public:
    MenuAnimation(AnimationSampler& sampler,std::shared_ptr<const AnmFile> file,i32 bank,i32 script,const SampleOptions& options={});
    void tick();
    // script < 0: every VM; otherwise only VMs running that script.
    void interrupt(i32 code,bool recursive=true,bool immediate=false,i32 script=-1);
    void set_sprite(i32 script,i32 sprite);
    void set_visible(i32 script,bool visible){visibility[script]=visible;}
    void set_text(i32 script,MenuText text){texts[script]=std::move(text);}
    void set_position(i32 script,std::optional<float> x,std::optional<float> y,i32 frames=0);
    bool alive();
    void poses(std::vector<Appearance>& out)const;
    bool retiring=false;
private:
    struct Position {float from_x,from_y,to_x,to_y;bool override_x,override_y;i32 frames,age;};
    Track track;std::unordered_map<i32,bool> visibility;std::unordered_map<i32,MenuText> texts;std::unordered_map<i32,Position> positions;
};
double easing(double t,i32 mode);
}
