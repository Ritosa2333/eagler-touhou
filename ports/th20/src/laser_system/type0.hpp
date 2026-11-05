#pragma once
#include "laser.hpp"
#include "../bullet_system/command.hpp"
namespace th20::source::laser {
struct Type0Parameters { //47c130, complete54byte parameter object
    sprite::Vec3 position{};
    float angle{},length{},field_14{},length_limit{},width{},speed{};
    std::uint32_t type{},color{};
    float radial_offset{};
    std::uint32_t command_index{},flags{};
    std::pmr::vector<bullet::Command> commands;
    std::int32_t field_48{},field_4c{},view_index{};
};
static_assert(sizeof(Type0Parameters)==0x54&&offsetof(Type0Parameters,commands)==0x38);
struct Segment;
class Type0Laser final:public Laser { //4c88e0,18f8
public:
    Type0Parameters parameters; //6f8
    sprite::Animation animation,origin_animation,tip_animation; //74c,d30,1314
    Type0Laser();
    ~Type0Laser() override; //4c8c40
    int initialize(const Type0Parameters&); //4d4150
    void tip_position(sprite::Vec3&) const; //4d6bd0
    void execute_commands(); //4d1470
    int advance(); //4d7750
    int collision_segment(Segment*); //4d60a0
    int update() override; //4cfa30
    int draw() override; //4d0290
    int finish() override {return 0;} //412540
    int cancel_rectangle(const sprite::Vec3&,const sprite::Vec3&,float,std::int32_t,std::int32_t) override; //4ca380
    int cancel_circle(const sprite::Vec3&,float,std::int32_t,std::int32_t) override; //4cb060
    int cancel_all(std::int32_t) override; //4d2dd0
    int accelerate(); //4d58d0
    int approach_speed(); //4d5710
    int turn_in_steps(); //4d5310
    int bounce(); //4d49e0
    int erase(int,int) override; //4c9830, original virtual44
    void split(const std::uint8_t*,int); //4ce510
    int cancel_polygon(const sprite::Vec3&,float,float,int,int,int); //4cda40
    int cancel_ellipse(const sprite::Vec3&,float,float,float,int,int); //4cce10
    int cancel_star(const sprite::Vec3&,float,float,float,int,int,int); //4cf080
    int measure_rectangle(const sprite::Vec3&,const sprite::Vec3&,float,int,int,int&); //4cbfa0
};
static_assert(offsetof(Type0Laser,animation)==0x74c&&offsetof(Type0Laser,tip_animation)==0x1314&&sizeof(Type0Laser)==0x18f8);
Type0Laser* create_type0(); //4c8190
std::uint32_t spawn_type0(Controller&,const Type0Parameters&);
}
