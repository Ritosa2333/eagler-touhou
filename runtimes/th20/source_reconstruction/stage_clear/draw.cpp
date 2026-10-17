#include "stage_clear.hpp"
#include "data_constants.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/gameplay.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../effect_system/effect.hpp"
#include "../text_renderer/text.hpp"
namespace th20::source::stage_clear {
namespace {
void reset_text(text::Renderer& p){ //4e67e0 ordered setters
    p.color=0xffffffff;p.field_1a1c8=0xffffffff;p.shadow_color=0xff000000;p.fields_1a1d4[1]=0;p.font_width=9;p.scale_x=p.scale_y=1;p.fields_1a1d4[2]=0;p.fields_1a1d4[3]=0;p.fields_1a1d4[4]=2;p.fields_1a1d4[6]=0;p.fields_1a1d4[7]=0;p.fields_1a1d4[8]=p.fields_1a1d4[9]=1;p.rotation=0;p.fields_1a1d4[10]=0;
}
}
int draw(StageClearInf& o){
    auto& sprites=*program_entry::sprite_controller;
    if(sprite::resolve_animation_handle(sprites,o.panel_handle)&&!(gameplay::controller->game_flags&0x10u)){
        auto& r=*text::renderer;reset_text(r);auto& a=*sprite::resolve_animation_handle(sprites,o.panel_handle);r.color=(r.color&0xffffffu)|(a.base.field_490&0xff000000u);r.fields_1a1d4[4]=4;r.fields_1a1d4[8]=r.fields_1a1d4[9]=0;
        r.write_text({data::f_0056ed14,data::f_0056ec98,0},data::text_00573f50);
        auto& p=*game_session::context(0).current_player;
        const auto red=clamped_level(p,0);r.write_text({data::f_0056ed14,data::f_00573fdc,0},data::text_00573f64,red);
        const auto blue=clamped_level(p,1);r.write_text({data::f_0056ed14,data::f_00573fe0,0},data::text_00573f7c,blue);
        const auto yellow=clamped_level(p,2);r.write_text({data::f_0056ed14,data::f_00573fe4,0},data::text_00573f94,yellow);
        const auto green=clamped_level(p,3);r.write_text({data::f_0056ed14,data::f_00573fe8,0},data::text_00573fac,green);
        char buffer[256];text::format_grouped_integer(buffer,256,recovered::signed_bits(o.bonus));r.write_text({data::f_0056ed14,data::f_0056f10c,0},data::text_00573fc4,buffer);
        r.fields_1a1d4[3]=4;reset_text(r);
    }
    if(gameplay::controller)if(auto* a=sprite::find_animation(sprites,o.panel_handle)){if(!(gameplay::controller->game_flags&0x10u)&&o.state==4)effects::enable_animation_tree(*a);else sprite::hide_animation_tree(*a);}
    return 1;
}
}
