#include "type3.hpp"
#include "../sprite_renderer/pool.hpp"
#include <new>
#include <cstring>
namespace th20::source::laser {
Type3Laser::Type3Laser():cursor(0),values_d30{},values_1530{} {sprite::construct_animation(animation);}
Type3Laser::~Type3Laser(){sprite::destroy_animation_contents(animation);} //then PMR vector and base allocation
int Type3Laser::initialize(const Type3Parameters& input){
    parameters=input;state=field_24=3;position=parameters.position;select_context(parameters.view_index);field_74=parameters.field_20;angle=parameters.angle;
    field_6e8=parameters.field_2c;handle=parameters.handle;for(auto& value:values_d30)value=field_74;speed=1;cursor=0;
    sprite::reset_animation_state(animation);animation.base.flags[0]=(animation.base.flags[0]&~0xff00u)|0x100u;return 0;
}
void Type3Laser::set_parameter_flag(std::uint32_t bit) noexcept {parameters.flags=(parameters.flags&~1u)|(bit&1u);}
int Type3Laser::request_delete(std::int32_t,std::int32_t keep) noexcept {if(!keep)flags=(flags&~6u)|2u;return 0;}
Type3Laser* create_type3(){void* memory=::operator new(sizeof(Type3Laser),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Type3Laser));return new(memory)Type3Laser;}
std::uint32_t spawn_type3(Controller& owner,const Type3Parameters& p){if(recovered::signed_bits(owner.count)>511)return 0;++owner.next_handle;if(recovered::signed_bits(owner.next_handle)<0x10000)owner.next_handle=0x10000;auto* l=create_type3();if(!l)return 0;l->handle=owner.next_handle;if(l->initialize(p)<0)return 0;owner.attach(*l);return owner.next_handle;}
}
