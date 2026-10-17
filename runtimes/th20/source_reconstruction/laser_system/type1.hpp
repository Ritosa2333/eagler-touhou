#pragma once
#include "laser.hpp"
#include "../bullet_system/command.hpp"
namespace th20::source::laser {
struct Segment { //4c8240
    sprite::Vec3 position{},vector_0c{};
    sprite::Vec2 size{};
    float field_20{},angle{},angle_28{},field_2c{};
    std::uint32_t field_30{},field_34{},flags{};
};
static_assert(sizeof(Segment)==0x3c);
struct Type1Parameters { //47c030 / copy4c8e90
    sprite::Vec3 position{},velocity{};
    float angle{},angular_velocity{},length_limit{},length{},width{},growth_speed{8};
    std::int32_t delay{},grow{},sustain{},shrink{},sound{},field_44{};
    std::uint32_t handle{};
    float radial_offset{};
    std::uint32_t command_index{},type{},color{},flags{};
    std::pmr::vector<bullet::Command> commands;
    std::int32_t view_index{};
};
static_assert(sizeof(Type1Parameters)==0x74&&offsetof(Type1Parameters,commands)==0x60);
class Type1Laser final:public Laser { //4c8880 / vtable5712e8
public:
    Type1Parameters parameters; //6f8
    std::uint32_t field_76c;
    sprite::Animation animation,origin_animation; //770,d54
    Type1Laser();
    ~Type1Laser() override; //4c8c00
    int initialize(const Type1Parameters&); //4d3d70
    void set_position(const sprite::Vec3&) noexcept override; //4d7d80
    void execute_commands(); //4d12e0
    int advance(); //4d7280
    int collision_segment(Segment*); //4d5e50
    int update() override; //4cf940
    int draw() override; //4d01d0
    int finish() override {return 0;} //412540, original literal zero return
    int cancel_rectangle(const sprite::Vec3&,const sprite::Vec3&,float,std::int32_t,std::int32_t) override; //4c9f10
    int cancel_circle(const sprite::Vec3&,float,std::int32_t,std::int32_t) override; //4cabf0
    int cancel_all(std::int32_t) override; //4d2a20, collision/graze processing
    int cancel_polygon(const sprite::Vec3&,float,float,int,int,int); //4cd580
    int cancel_ellipse(const sprite::Vec3&,float,float,float,int,int); //4cc950
    int cancel_star(const sprite::Vec3&,float,float,float,int,int,int); //4cebb0
    int erase(int,int) override; //4c9580, original virtual44
    void split(const std::uint8_t*,int); //4ce220, surviving intervals become Type0
    int measure_rectangle(const sprite::Vec3&,const sprite::Vec3&,float,int,int,int&); //4cb970
};
static_assert(offsetof(Type1Laser,parameters)==0x6f8&&offsetof(Type1Laser,animation)==0x770&&sizeof(Type1Laser)==0x1338);
std::int32_t __cdecl remap_sprite(sprite::Animation*,std::int32_t); //4cf550, common concrete lasers
Type1Laser* create_type1(); //4c8130
std::uint32_t spawn_type1(Controller&,const Type1Parameters&); //4d47f0 case1
}
