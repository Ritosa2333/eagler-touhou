#include "card.hpp"
#include "records.hpp"
#include "data_constants.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../text_renderer/text.hpp"
namespace th20::source::card {
int draw(CardInf& o){
    if(!(o.flags&1u))return 1;
    auto* a=sprite::resolve_animation_handle(*program_entry::sprite_controller,o.info_handles[2]);if(!a)return 1;
    auto& r=*text::renderer;r.fields_1a1d4[3]=2;r.fields_1a1d4[6]=2;r.color=(r.color&0xffffffu)|(a->base.field_490&0xff000000u);
    if(!(o.flags&2u))r.write_ascii({data::f_0056fe84,data::f_0056fe78,0},"$");
    else r.write_padded_integer({data::f_0056fe80,data::f_0056fe78,0},o.bonus,8,' ');
    const unsigned mode=game_session::mode()==2?1:0;const auto* entry=record(*progress::current_profile(),o.spell_index);
    const int captures=record_count(entry,0xc0,mode),attempts=record_count(entry,0xc8,mode);const sprite::Vec3 location{data::f_0056fe88,data::f_0056fe78,0};
    if(captures<100){if(attempts<100)r.write_ascii_format(location,"%.2d/%.2d",captures,attempts);else r.write_ascii_format(location,"%.2d/99+",captures);}
    else r.write_ascii(location,"MASTER");
    r.fields_1a1d4[3]=0;r.fields_1a1d4[6]=0;r.color|=0xff000000u;return 1;
}
}
