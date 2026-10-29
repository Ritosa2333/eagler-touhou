// Port of upstream src/renderer.mjs capturePresentation/renderPresentation
// and the read-only presentation helpers it calls (stage-background.mjs,
// draw-scheduler.mjs, ascii/spell/ufo/fps/stage-clear-presentation.mjs).
// The browser read the core's state through its C ABI and JSON snapshots;
// here the same fields are read from the core directly.
#pragma once
#include "Animation.hpp"
#include "PresentationOrder.hpp"
#include "ExeTables.hpp"
#include "Renderer.hpp"
#include "TextRaster.hpp"
#include <array>
#include <list>

namespace th12host {
// ANM banks by the core's numbering: 0 bullet, 1 enemy, 2 stgenmNN,
// 3 stgenmNNm, 4..6 pl00..pl02, 7 front, 8 text, 9 stNNlogo, 10 stageNN,
// 11 ascii; 12 sig (start-up screen), 13 title, 14 title_v (menus);
// 15..18 the ending and staff roll ANM files while they play.
enum Bank:i32 {BankBullet=0,BankEnemy=1,BankStageEnemy=2,BankStageEnemyM=3,BankPlayer=4,BankFront=7,BankText=8,BankLogo=9,BankStage=10,BankAscii=11,BankSig=12,BankTitle=13,BankTitleV=14,
    BankEnding=15,BankEndingCount=4,BankCount=19};
struct Banks {
    std::array<std::shared_ptr<AnmFile>,BankCount> slots{};
    std::shared_ptr<const AnmFile> get(i32 bank)const{return bank>=0&&bank<BankCount?slots[size_t(bank)]:nullptr;}
};
// STD geometry the core does not keep: object layers, primitive kinds and
// object instances (upstream prepare-presentation.py decode_std).
struct StdGeometry {
    struct Quad {u16 kind=0,script=0;i32 slot=0;std::array<float,3> position{};std::array<float,2> size{};};
    struct Object {u8 layer=0;std::vector<Quad> quads;};
    struct Instance {u16 object=0;std::array<float,3> position{};};
    std::vector<Object> objects;std::vector<Instance> instances;
    bool parse(const std::vector<u8>& bytes,std::string& error);
};
// One FontManager submission (upstream asciiGlyphs descriptor).
struct TextDescriptor {std::string text;i32 font=0;float x=0,y=0,z=0,scale_x=1,scale_y=1;u32 color=0xffffffff;i32 align_x=1,align_y=1,draw_flag=0;};

class Scene {
public:
    Scene(Renderer& r,TextRaster& t,Banks& b,const ExeTables& e);
    struct Options {float current_fps=-1;i32 hud_age=-1;i32 score_low_digit=0;bool practice=false;bool stage_clear=false;double high_score=0;i32 recorded_fps=-1;};
    void set_geometry(i32 stage,StdGeometry g){geometry_stage=stage;geometry=std::move(g);}
    void reset_presentation();
    void reset_motion(){previous_motion.clear();current_motion.clear();}
#ifdef TH12_MULTIPLAYER
    void multiplayer_view(int local,bool visible){local_seat=local;local_visibility=visible;}
#endif
    void capture(const Options& options);
    bool continuous_motion()const{return !previous_motion.empty();}
    void render(float alpha=1,float live_fps=-1);
    // Menu/loading layers (upstream renderMenu): appearances at their own
    // positions, menu text lines, font texts, then the FPS counter.
    void render_menu(const std::vector<Appearance>& poses,float x,float y,bool overlay,float shade=0,const float* shade_bounds=nullptr,Color clear={0,0,0,1},
        const std::vector<TextDescriptor>* texts=nullptr,float fps=-1);
    // Upstream renderPauseBackdrop: the paused field (captured once into text
    // entry 1), script 79's tint over it, then the backdrop VM's own sprites.
    void render_pause_backdrop(const std::vector<Appearance>& poses,bool capture);
    void render_text(const TextDescriptor& t,const float* shake=nullptr);
    void drop_text_cache();
    AnimationSampler& sampler(){return sampler_;}
private:
    struct Dialogue {std::u32string text;std::string bytes;i32 offset=0,spacing=0,style=0;u32 color=0xd8d8d8;float width=354,height=17;u32 source_width=730,source_height=36;
        bool raster=false,menu=false,localized_spell=false;float source_x=0,source_y=2;u32 foreground=0xffffff;i32 outline[4][2]{{2,4},{-2,4},{2,0},{-2,0}};};
    struct Pose {
        enum Kind:u8 {State,NativeText,HudCounters,Rectangle,Mesh,DialogueLine,BeamTrail,Trail,Beam} kind=State;
        Appearance a;float x=0,y=0,angle=0,scale=1,alpha=1;
        bool has_priority=false;i32 priority=0;i32 clip=-1;i8 clipped=-1;bool has_order=false;i32 order=0;u32 index=0;
        u32 owner=0,id=0;bool logical=false,mesh_pass0=false;
        u64 continuity=0;
        u32 item=0; // text / dialogue / mesh index
        float bounds[4]{};u32 argb=0;u32 start=0,end=0;float width=0;
    };
    struct Triangle {i32 layer,entry,blend;bool point;Vertex v[3];float depth;u32 order=0;};
#ifdef TH12_MULTIPLAYER
    std::vector<std::array<u32,2>> visual_owners;
    int local_seat=0;bool local_visibility=false;
    void capture_multiplayer_hud(int frame);
    void apply_multiplayer_opacity();
#endif
    Renderer& gpu;TextRaster& raster;Banks& banks;const ExeTables& exe;AnimationSampler sampler_;
    StdGeometry geometry;i32 geometry_stage=0;i32 last_frame=-1,live_fps_text=-1;
    std::vector<Pose> poses;std::vector<u32> sorted;std::vector<TextDescriptor> texts;std::vector<Dialogue> dialogues;std::vector<Triangle> triangles;
    std::vector<Appearance> scratch;std::vector<Pose> logical_poses;std::vector<u32> owners;FrameLookup schedule_ids;PriorityBuckets draw_buckets,menu_buckets;std::vector<u32> menu_order;Pose shaken; // per-frame scratch
    struct ClipVertex {double cx,cy,depth,u,v;};
    std::vector<ClipVertex> clip_polygon,clip_output;std::vector<Vertex> clip_projected;
    std::vector<Vertex> fan_vertices;std::vector<u32> fan_indices;
    struct Motion {float x,y,ax,ay,rotation;i32 bank,mode;};
    std::vector<std::pair<u64,Motion>> previous_motion,current_motion;
    std::array<float,41> hud{};std::array<float,2> shake{};bool has_surface=false;Color fog{0,0,0,1};
    struct Cached {std::string key;Texture texture;};std::list<Cached> dialogue_cache,menu_cache;
    const AnmSprite* sprite(i32 bank,i32 id)const;Texture texture(i32 bank,i32 entry)const;
    void add(i32 bank,i32 script,i32 age,float x,float y,float angle,float scale,std::optional<BulletVariant> variant,const SampleOptions& sampling,float alpha,i32 priority=-1,i8 clipped=-1);
    void capture_background();
    void order();
    void draw_state(const Appearance& a,float x,float y,float angle,float scale,float alpha,const float* shake);
    void draw_dialogue(const Pose& p,const float* shake);
    void draw_text_quad(const Dialogue& d,const Appearance& a,float x,float y,float alpha,const float* shake);
    void draw_mesh(const Pose& p,const float* shake);
    void hud_text(const std::string& text,float x,float y,float scale=1,bool right=false);
    void stage_surface(i32 priority,i32 clip,bool& copied,bool& composited);
    Texture dialogue_texture(const Dialogue& d);
};
std::string grouped_number(double n);
i64 stage_clear_award(const std::array<float,41>& h,bool practice);
}
