#pragma once
#include "laser.hpp"
#include "../bullet_system/command.hpp"
namespace th20::source::laser {
struct Type3Parameters { //48b5b0, exact50byte value/pmr vector
    sprite::Vec3 position{},vector_0c{};
    float angle{},field_1c{},field_20{},field_24{};
    std::uint32_t handle{},field_2c{},field_30{},field_34{},flags{};
    std::pmr::vector<bullet::Command> commands;
    std::int32_t view_index{};
};
static_assert(sizeof(Type3Parameters)==0x50&&offsetof(Type3Parameters,commands)==0x3c);
class Type3Laser final:public Laser { //4c8390, vtable571374
public:
    Type3Parameters parameters; //6f8
    sprite::Animation animation; //748
    std::uint32_t cursor;
    float values_d30[512];
    std::uint32_t values_1530[512];
    Type3Laser();
    ~Type3Laser() override; //4c8a40
    int initialize(const Type3Parameters&); //4d3110
    void set_parameter_flag(std::uint32_t) noexcept override; //4d7d20
    int request_delete(std::int32_t,std::int32_t) noexcept; //4c9350
    int erase(std::int32_t mode,std::int32_t keep) override{return request_delete(mode,keep);} //same original virtual44
    //These are literal zero-return original functions. Full machine evidence
    //is in evidence/, including ret argument bytes; they are not stand-ins.
    int update() override {return 0;} //412540: xor eax,eax;ret
    int draw() override {return 0;} //412540
    int finish() override {return 0;} //412540
    int cancel_rectangle(const sprite::Vec3&,const sprite::Vec3&,float,std::int32_t,std::int32_t) override {return 0;} //4c9da0
    int cancel_circle(const sprite::Vec3&,float,std::int32_t,std::int32_t) override {return 0;} //4caac0
    int cancel_all(std::int32_t) override {return 0;} //414b50
};
static_assert(offsetof(Type3Laser,parameters)==0x6f8&&offsetof(Type3Laser,animation)==0x748);
static_assert(offsetof(Type3Laser,cursor)==0xd2c&&sizeof(Type3Laser)==0x1d30);
Type3Laser* create_type3(); //4c8020
std::uint32_t spawn_type3(Controller&,const Type3Parameters&);
}
