#include "score.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/dispatch.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../text_renderer/text.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/platform_window.hpp"
namespace th20::source::small_score {
namespace {
class GameEnvironment final:public Environment {
public:
    sprite::AnimationFile& text_file() override{return *text::renderer->animation_file;}
    void initialize_sprite(sprite::AnimationFile& file,sprite::Animation& animation,int index) override{text::initialize_animation_sprite(file,animation,index);}
    void select_view(int index) override{program_entry::sprite_controller->field_6c4=index;}
    void configure_layer(int layer,int view) override{sprite::configure_animation_layer(*program_entry::sprite_controller,layer,view);}
    bool fog_configuration() override{return (program_entry::graphics_state.configuration.flags&4u)!=0;}
    void disable_fog() override{platform_window::disable_fog(program_entry::graphics_state);}
    void set_sprite(sprite::Animation& animation,int index) override{text::set_sprite(*program_entry::sprite_controller,animation,index);}
    float sprite_height(const sprite::Animation& animation) override{return sprite::current_sprite(*program_entry::sprite_controller,animation).extent_4c;}
    void draw_sprite(sprite::Animation& animation) override{sprite::draw_axis_aligned_sprite(*program_entry::sprite_controller,animation,false);}
    void text_vertical_alignment(unsigned value) override{text::renderer->fields_1a1d4[3]=value;}
    void text_horizontal_alignment(unsigned value) override{text::renderer->fields_1a1d4[6]=value;}
    void text_color(unsigned value) override{text::renderer->color=value;}
    void text_style(unsigned a,unsigned b) override{text::renderer->fields_1a1d4[8]=a;text::renderer->fields_1a1d4[9]=b;}
    void write_text(TextLine line,const sprite::Vec3& position,float multiplier,int value) override{
        switch(line){case TextLine::no_bonus:text::renderer->write_ascii_format(position,"NO BONUS");break;case TextLine::multiplier:text::renderer->write_ascii_format(position,"BONUS %.1f",static_cast<double>(multiplier));break;case TextLine::integer:text::renderer->write_ascii_format(position,"%d",value);break;}
    }
};
}
Environment& environment(){static GameEnvironment host;return host;}
}
