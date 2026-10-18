#include "type2.hpp"
#include "../bullet_system/shoot.hpp"
#include "../player_entity/player.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
#include <stdexcept>
namespace th20::source::laser {
void Type2Laser::reemit(const bullet::Command& op){
    auto metadata=std::make_shared<bullet::ShotMetadata>();bullet::ShotParameters p;
    ecl::math::polar(p.position.x,p.position.y,angle,field_74);p.position.x=recovered::add32(p.position.x,position.x);p.position.y=recovered::add32(p.position.y,position.y);p.position.z=0;
    metadata->type=static_cast<std::int16_t>(op.words[4]);metadata->field_44=op.words[5];p.count=static_cast<std::int16_t>(op.words[6]);p.rows=static_cast<std::int16_t>(op.words[7]);
    p.angle=op.f(0)<=-999990?angle:(!(op.f(0)>=999990)?op.f(0):player_entity::angle_to_player(context->objects_04[0],position));p.angle_step=op.f(1);p.speed=op.f(2)<=-999990?field_7c:op.f(2);p.speed_step=op.f(3);
    ++command_index;
    if(command_index>=parameters.commands.size())throw std::out_of_range("Type2 ETEX13 requires a second command record");
    const auto& extra=parameters.commands[command_index];p.fields_00[0]=extra.words[4];p.fields_00[1]=extra.words[5];const auto erase_parent=extra.words[6];
    //4d0f78 copies exactly0x420 bytes. Original47b290 was independently run
    //32 times: the actual destination has size=capacity=2, only0x58 bytes.
    //Keep the real constructor and the preceding state changes; do not fake
    //a24-entry vector or execute heap corruption. This invalid-memory path
    //is an explicit equivalence limitation, documented inTYPE2_ETEX13.md.
    constexpr std::size_t count=0x420/sizeof(bullet::Command);
    if(parameters.commands.size()<count||metadata->commands.size()<count)
        throw std::length_error("Original Type2 ETEX13 copies 0x420 bytes beyond its 2-command destination; heap corruption is not emulated");
    std::memcpy(metadata->commands.data(),parameters.commands.data(),0x420);
    bullet::shoot(*static_cast<bullet::Controller*>(context->primary_owner),p,std::move(metadata));++command_index;if(erase_parent)erase(0,0);
}
}
