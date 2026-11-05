#pragma once
#include "AnmManager.hpp"
namespace th15 {
struct PopupEntry {
    std::array<u8,12> digits{};Vec3 position{};float velocity=0;u32 color=0;Timer age{0,0,0,0,0};
    u32 reserved[2]{};u8 active=0,digit_count=0,padding[2]{};i32 bonus=0;float multiplier=0;
};
static_assert(sizeof(PopupEntry)==72);
struct PopupDrawServices {virtual ~PopupDrawServices()=default;virtual bool draw_glyph(AnmVm&)=0;virtual bool draw_text(const Vec3&,u32 color,const std::string&)=0;};
class PopupManager {
    AnmManager& animations;AnmVm glyph;std::array<u64,18> presentation_generations{};u32 next_number=0,saved_next=0;std::array<PopupEntry,18> saved{};
public:
    std::array<PopupEntry,18> entries{};std::string error;
    explicit PopupManager(AnmManager& manager):animations(manager){}
    bool initialize(i32 ascii_resource=5);
    void number(const Vec3&,i32 value,u32 color);
    void bonus(const Vec3&,i32 value,u32 color,float multiplier);
    void update(float rate);bool draw(PopupDrawServices&,const Vec3& player);
    void save(){saved=entries;saved_next=next_number;}void restore(){entries=saved;next_number=saved_next;}
    void write_checkpoint_file(std::vector<u8>&)const;
    bool read_checkpoint_file(const u8*,u32);
};
}
