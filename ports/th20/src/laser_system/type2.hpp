#pragma once
#include "laser.hpp"
#include "../bullet_system/command.hpp"
namespace th20::source::laser {
struct Segment;
struct CurveNode { //4c82c0,3c-byte singly linked motion intervals
    CurveNode* next{};
    std::uint32_t field_04{};
    float begin{},end{};
    std::uint32_t kind{};
    sprite::Vec3 direction{},position{};
    float angle{},speed{};
    float acceleration{},angular_acceleration{};
};
static_assert(sizeof(CurveNode)==0x3c&&offsetof(CurveNode,angle)==0x2c);
struct CurveSample {sprite::Vec3 position,velocity;float angle,speed;};
static_assert(sizeof(CurveSample)==0x20);
struct Type2Parameters { //48b650
    sprite::Vec3 position{};
    float angle{},width{},speed{};
    std::uint32_t type{},color{},count{};
    float radial_offset{};
    std::uint32_t flags{};
    std::pmr::vector<bullet::Command> commands;
    std::int32_t sound{},motion_sound{};
    std::uint32_t command_index{};
    CurveNode* path{};
    float time{};
    std::uint32_t field_50{},field_54{};
    std::int32_t view_index{};
};
static_assert(sizeof(Type2Parameters)==0x5c&&offsetof(Type2Parameters,commands)==0x2c);
class Type2Laser final:public Laser { //4c8410,vtable571214
public:
    Type2Parameters parameters; //6f8
    sprite::Animation animation,origin_animation; //754,d38
    float field_131c;
    std::uint32_t field_1320,field_1324;
    CurveSample* samples; //1328
    void* geometry; //132c, count*38
    CurveNode path; //1330
    std::uint32_t padding_136c; //original tail alignment, constructor preserves
    Type2Laser();
    ~Type2Laser() override; //4c8a70
    int initialize(const Type2Parameters&); //4d3230
    int update() override; //4cf5f0
    int draw() override; //4cfbb0
    int finish() override; //4d2360
    int cancel_rectangle(const sprite::Vec3&,const sprite::Vec3&,float,std::int32_t,std::int32_t) override; //4c9ab0
    int cancel_circle(const sprite::Vec3&,float,std::int32_t,std::int32_t) override; //4ca830
    int cancel_all(std::int32_t) override; //4d2750
    int advance(); //4d6c40
    void execute_commands(); //4d0380
    int compute_segments(Segment* copy=nullptr); //4d5c00
    int accelerate(); //4d58d0
    int angular_accelerate(); //4d5580
    int turn_in_steps(); //4d50a0
    int steer(); //4d2540
    int wait_command(); //4d4660
    CurveNode* append_path(float time); //4d5a90
    void reemit(const bullet::Command&); //4d0380 case13, rejects proven original out-of-bounds copy
    int erase(int,int) override; //4c9380, original virtual44
    void split(std::uint8_t*,int); //4cdef0, mutates mask
    int cancel_polygon(const sprite::Vec3&,float,float,int,int,int); //4cd2d0
    int cancel_ellipse(const sprite::Vec3&,float,float,float,int,int); //4cc690
    int cancel_star(const sprite::Vec3&,float,float,float,int,int,int); //4ce8e0
    int measure_rectangle(const sprite::Vec3&,const sprite::Vec3&,float,int,int,int&); //4cb510
};
static_assert(offsetof(Type2Laser,animation)==0x754&&offsetof(Type2Laser,path)==0x1330&&sizeof(Type2Laser)==0x1370);
Type2Laser* create_type2(); //4c8080
std::uint32_t spawn_type2(Controller&,const Type2Parameters&); //4d47f0 type2
CurveNode* create_curve_node(); //4c7f90
std::int32_t __cdecl remap_curve_sprite(sprite::Animation*,std::int32_t); //4cf5c0, second arg unused
void sample_curve_absolute(const CurveNode&,sprite::Vec3&,float& speed,float& angle,float time); //4d6390
void sample_curve_backwards(const CurveNode&,sprite::Vec3&,float& speed,float& angle,const sprite::Vec3& previous,float previous_speed,float previous_angle,float time); //4d6830
void sample_curve_path(const CurveNode*,sprite::Vec3&,float& speed,float& angle,const sprite::Vec3& previous,float previous_speed,float previous_angle,float time,bool backwards); //4d62e0
bool curve_rectangle_point(const sprite::Vec3&,const sprite::Vec3&,float,const sprite::Vec3&); //458e90
}
