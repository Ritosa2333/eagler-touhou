#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../stone_menu/cursor.hpp"
#include "../sprite_renderer/sprite.hpp"
namespace th20::source::help {
struct HelpInf final:runtime::CallbackOwner {
    int state;
    recovered::Timer age;
    menu::Cursor cursor;
    std::uint32_t handles[14];
    int finished;
    float x;
    sprite::AnimationFile* file;
    std::uint8_t* image_bytes;
    int substate;
    char filename[128];
    std::uint32_t image_size;
    HelpInf(); //4bee70
    ~HelpInf() override; //4bef80
};
static_assert(sizeof(HelpInf)==0x140&&offsetof(HelpInf,cursor)==0x24&&offsetof(HelpInf,handles)==0x70&&offsetof(HelpInf,filename)==0xbc);
HelpInf* controller();
HelpInf* create(); //4bfc70
int initialize(HelpInf&); //4bf990
int update(HelpInf&); //4bf0a0
void spawn(HelpInf&,int,const sprite::Vec3&); //4bfc20
void replace_texture_image(sprite::TextureRecord&,const std::uint8_t*,std::uint32_t,int format,bool container); //44dc20
void clear_texture(sprite::TextureRecord&); //449d80
}
