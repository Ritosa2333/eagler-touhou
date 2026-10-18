#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_state/state.hpp"
#include "../game_session/session.hpp"
#include "../sprite_renderer/sprite.hpp"
namespace th20::source::small_score {
struct Entry {
    std::uint8_t digits[12];
    sprite::Vec3 position;
    float speed;
    std::uint32_t color;
    recovered::Timer age;
    std::uint32_t field_30,field_34;
    std::uint8_t active,length,padding_3a[2];
    std::int32_t bonus;
    float multiplier;
};
static_assert(sizeof(Entry)==0x44&&offsetof(Entry,active)==0x38);
class Environment;
class SmallScoreInf final:public runtime::CallbackOwner {
public:
    sprite::AnimationFile* file;
    std::int32_t next_slot;
    std::uint32_t field_18;
    sprite::Animation animation;
    Entry entries[18],secondary_entries[18];
    std::int32_t view_index;
    game_session::Context* context;
    SmallScoreInf(); //50fdd0
    ~SmallScoreInf() override; //50fea0
    void select_context(std::int32_t) noexcept; //510890
    int initialize(std::int32_t,Environment&); //510640
};
static_assert(sizeof(SmallScoreInf)==0xf98&&offsetof(SmallScoreInf,animation)==0x1c&&offsetof(SmallScoreInf,entries)==0x600&&offsetof(SmallScoreInf,secondary_entries)==0xac8);
enum class TextLine { no_bonus,multiplier,integer };
class Environment {
public:
    virtual ~Environment()=default;
    virtual sprite::AnimationFile& text_file()=0;
    virtual void initialize_sprite(sprite::AnimationFile&,sprite::Animation&,int)=0;
    virtual void select_view(int)=0;
    virtual void configure_layer(int layer,int view)=0;
    virtual bool fog_configuration()=0;
    virtual void disable_fog()=0;
    virtual void set_sprite(sprite::Animation&,int)=0;
    virtual float sprite_height(const sprite::Animation&)=0;
    virtual void draw_sprite(sprite::Animation&)=0;
    virtual void text_vertical_alignment(unsigned)=0; //488b00,+1a1e0
    virtual void text_horizontal_alignment(unsigned)=0; //488a00,+1a1ec
    virtual void text_color(unsigned)=0; //470ad0
    virtual void text_style(unsigned,unsigned)=0; //4b8620
    virtual void write_text(TextLine,const sprite::Vec3&,float,int)=0; //46c990
};
Environment& environment();
void construct_entry(Entry&) noexcept; //50fd50
void spawn(SmallScoreInf&,const sprite::Vec3&,int,std::uint32_t) noexcept; //510710
int update(SmallScoreInf&,Environment&); //50ff70
int draw(SmallScoreInf&,Environment&); //510110
inline int update(SmallScoreInf& owner){return update(owner,environment());}
inline int draw(SmallScoreInf& owner){return draw(owner,environment());}
SmallScoreInf* controller(int index=0) noexcept; //4bd5a0->421810
SmallScoreInf* create_controller(int index=0); //5108d0->50fd00
}
