#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_core/worker.hpp"
#include "../sprite_renderer/sprite.hpp"
#include <functional>
#include <list>
#include <string>
#include <string_view>
namespace th20::source::text {
struct Line {
    char text[256];
    sprite::Vec3 position;
    std::uint32_t color,blend;
    float scale_x,scale_y,rotation;
    std::uint32_t fields_120[2];
    std::int32_t font;
    std::uint32_t shadow;
    std::int32_t layer,frames;
    std::uint32_t align_x,align_y;
    Line();
};
static_assert(sizeof(Line)==0x140 && offsetof(Line,frames)==0x134);
void construct_line(Line&) noexcept;                       //46aba0
struct OwnedAnimation : sprite::Animation {
    OwnedAnimation();
    ~OwnedAnimation();
};
static_assert(sizeof(OwnedAnimation)==sizeof(sprite::Animation));
struct JobLink : scheduler::Link {JobLink(){scheduler::initialize_link(*this,nullptr);}};
struct JobList : scheduler::List {JobList(){scheduler::initialize_list(*this);}};
struct Job {
    JobLink link;
    OwnedAnimation animation;
    std::pmr::string text;
    sprite::Vec3 position;
    std::int32_t frames,layer;
    std::uint32_t fields_628[5],rectangle[4],color,field_650,field_654;
    float scale_x,scale_y;
    std::uint32_t align_x,align_y,field_668;
    float rotation;
    std::uint32_t shadow,font;
    std::uint8_t ready,padding_679[3];
    const std::uint8_t* external_ready;
    std::atomic<bool> canceled;
    std::uint8_t padding_681[3];
    Job();                                                //46ac80
    ~Job();                                               //46b0d0
};
// Job embeds a pmr::string and pending_tasks a list<function>; sizes/offsets
// past those members are MSVC x86 STL layout evidence, binding on the Windows
// build only (TH_SDL3 recompiles with libc++ layouts).
#ifndef TH_SDL3
static_assert(sizeof(Job)==0x684 && offsetof(Job,text)==0x5f8 && offsetof(Job,position)==0x614 && offsetof(Job,canceled)==0x680);
#endif
void destroy_job(Job*);                                  //46a090/46a100
Job* create_job();                                       //46a1c0
struct Point {std::int32_t x,y;};
bool rectangles_overlap(std::int32_t x,std::int32_t y,std::int32_t width,std::int32_t height,
    std::int32_t other_x,std::int32_t other_y,std::int32_t other_width,std::int32_t other_height) noexcept; //470920
Point measure_text(const char* cp932,std::int32_t font);   //4156e0
class Renderer : public runtime::CallbackOwner {
public:
    OwnedAnimation animations[3];
    Line lines[320];
    std::int32_t line_count;
    std::uint32_t color,shadow_color,field_1a1c8;
    float scale_x,scale_y;
    std::uint32_t fields_1a1d4[11];
    std::uint32_t texture_width,texture_height;
    float rotation;
    JobList jobs;
    runtime::Worker worker;
    std::list<std::function<void()>> pending_tasks;
    std::int32_t font_width;
    std::uint32_t frame_count;
    sprite::AnimationFile* animation_file;
    std::uint32_t animation_handle,loading_handle;
    scheduler::Node* additional_draw_nodes[4];
    char text_buffer[256];
    Renderer();                                          //46a8e0
    ~Renderer() override;                                //46aef0
    void enable_callbacks() override;                    //46c930
    int initialize();                                    //46c080
    void compact_lines();                                //46b7f0
    int update();                                        //46b6e0
    int draw_layer(std::int32_t);                         //46ba20
    int draw_primary_layer();                            //46b9a0
    void draw_jobs(std::int32_t);                         //46bb90
    void draw_line(const Line&);                         //46d300
    void create_loading_text(float x,float y);            //4a0aa0
    void register_job(const sprite::Vec3&,Job&);           //46d170
    Point find_atlas_position(std::int32_t width,std::int32_t height); //470680
    void enqueue_task(std::function<void()>);              //470180
    char* next_text_buffer() noexcept;                    //470620
    void commit_line(const sprite::Vec3&);                 //46ca40
    void commit_shadow_line(const sprite::Vec3&,std::uint32_t font); //46d090
    void write_float(const sprite::Vec3&,float,std::string_view suffix,std::int32_t precision); //46ce00
    void write_text(const sprite::Vec3&,const char* format,...); //46c210
    void write_text_literal(const sprite::Vec3&,const char* cp932); //46c210 after formatting
    void write_ascii_format(const sprite::Vec3&,const char* format,...); //46c990
    void write_character(const sprite::Vec3&,char);         //46cc30
    void write_integer(const sprite::Vec3&,std::int32_t);   //46cc80
    void write_padded_integer(const sprite::Vec3&,std::int32_t,std::int32_t width,char padding); //46cd40
    void write_integer_suffix(const sprite::Vec3&,std::int32_t,std::string_view); //46cd80
    void write_ascii(const sprite::Vec3&,const char*);      //46ce90
    void write_affixed_integer(const sprite::Vec3&,std::string_view,std::int32_t,std::string_view); //46cf00
    void write_prefixed_integer(const sprite::Vec3&,std::string_view,std::int32_t); //46cfa0
    void write_grouped_score(const sprite::Vec3&,std::uint64_t score,std::int32_t final_digit); //46d020
};
void format_grouped_integer(char* buffer,std::int32_t capacity,std::int64_t value); //453500
#ifndef TH_SDL3
static_assert(sizeof(std::function<void()>)==40 && sizeof(std::list<std::function<void()>>)==8);
#endif
static_assert(offsetof(Renderer,lines)==0x11bc && offsetof(Renderer,line_count)==0x1a1bc);
static_assert(offsetof(Renderer,jobs)==0x1a20c && offsetof(Renderer,worker)==0x1a224);
static_assert(offsetof(Renderer,animation_file)==0x1a244 && sizeof(Renderer)==0x1a360);
extern Renderer* renderer;                                //unique originalglobal5c0698
Renderer* create_renderer();                             //471060/46a170
void initialize_animation_sprite(sprite::AnimationFile&,sprite::Animation&,std::int32_t); //4708e0
void set_sprite(sprite::Controller&,sprite::Animation&,std::int32_t); //470ed0
void set_text_rectangle(sprite::Controller&,sprite::Animation&,std::int32_t x,std::int32_t y,
                        std::int32_t width,std::int32_t height); //470b80
SIZE write_animation_text(sprite::Controller&,sprite::Animation&,std::uint32_t foreground,
    std::uint32_t background,std::int32_t font,std::int32_t x,std::int32_t spacing,
    std::uint8_t* ready,std::function<void()> completion,const char* format,...); //44ac20/44fff0
}
