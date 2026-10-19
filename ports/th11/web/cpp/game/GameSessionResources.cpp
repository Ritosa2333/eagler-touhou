#include "GameSessionResources.hpp"

namespace th11 {

namespace {
bool core_error(std::string& error,const char* message) { error=message; return false; }
}

bool GameSessionResources::load_core(GameResources& source) {
    if(core_source==&source)return true;
    error.clear(); stage_number=0;
    if(!source.open_anm("ascii.anm",core.ascii))return core_error(error,"ascii ANM");
    if(!source.open_anm("bullet.anm",core.bullet))return core_error(error,"bullet ANM");
    if(!source.open_anm("enemy.anm",core.enemy))return core_error(error,"enemy ANM");
    if(!source.open_anm("front.anm",core.front))return core_error(error,"front ANM");
    if(!source.open_anm("text.anm",core.text))return core_error(error,"text ANM");
    if(!source.open_anm("title.anm",core.title))return core_error(error,"title ANM");
    if(!source.open_anm("title_v.anm",core.title_variant))return core_error(error,"title variant ANM");
    if(!source.open_anm("pl00.anm",core.players[0]))return core_error(error,"pl00 ANM");
    if(!source.open_anm("pl01.anm",core.players[1]))return core_error(error,"pl01 ANM");
    constexpr const char* names[6]={"pl00a.sht","pl00b.sht","pl00c.sht","pl01a.sht","pl01b.sht","pl01c.sht"};
    for(u32 i=0;i<6;++i)if(!source.open_sht(names[i],core.shots[i]))return core_error(error,"SHT resource");
    if(!source.load_ecl("default.ecl",core.defaults))return core_error(error,"default ECL");
    core_source=&source;return true;
}

bool GameSessionResources::load_stage(GameResources& source,u32 number) {
    auto next=std::make_unique<StageResources>();
    if(!source.load_stage(number,*next)){error="stage resource";return false;}
    if(effects&&!effects->prepare(*next)){effects->release(*next);error="stage graphics preparation";return false;}
    retire_previous();previous_stage=std::move(stage);stage=std::move(next);
    stage_number=number;error.clear();return true;
}
void GameSessionResources::retire_previous(){if(previous_stage&&effects)effects->release(*previous_stage);previous_stage.reset();}

}
