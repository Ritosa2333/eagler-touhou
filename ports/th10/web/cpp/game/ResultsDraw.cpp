#include "ResultsDraw.hpp"
#include "HighRefresh.hpp"
namespace th10 {
static u32 text_address(const char* text){return reinterpret_cast<uintptr_t>(text);}
static u32 selected_color(bool selected){return selected?0xffffff00:0xff808080;}
static float displayed_elapsed(const Timer& timer){
    if(!high_refresh::active||timer.current<=0||timer.current-timer.previous>2)return timer.fractional;
    return high_refresh::lerp(float(timer.previous),timer.fractional);
}
// 0x4224c0. The final three cells use the font's special control glyphs.
void Results::draw_name(Vec3 position,ResultsDrawEnvironment& env) const {
    const i32 length=std::strlen(env.alphabet);env.print(position,"%s",{text_address(name)});
    position.x=(Extended::from_int(static_cast<i32>(static_cast<u32>(name_length)*9u))+number(position.x)).to_float();if(name_length==8)position.x=Scalar::sub(position.x,9);
    *env.color=0xffffff00;env.print(position,"_");*env.color=0xffffffff;position={112,320,0};
    for(i32 i=0;i<length;++i){*env.color=selected_color(keyboard.selected==i);const i32 glyph=i<length-3?static_cast<std::int8_t>(env.alphabet[i]):i==length-3?129:i==length-2?127:128;env.print(position,"%c",{static_cast<u32>(glyph)});
        if(i%13==12){position.x=112;position.y=Scalar::add(position.y,16);}else position.x=Scalar::add(position.x,18);}
    *env.color=0xffffffff;
}
// 0x422660. Text state and colors are restored on every exit, including states
// where the interface draws no labels.
i32 Results::draw(ResultsDrawEnvironment& env) const {
    *env.text_mode=1;Vec3 position{};
    if(state==10||state==17){
        position={48,64,0};for(i32 i=0;i<25;++i){*env.color=selected_color(menu.selected==i);const auto* preview=previews[i];
            if(!preview)env.print(position,"No.%.2d -------- --/--/-- ------- - St-",{static_cast<u32>(i+1)});
            else{const auto* info=preview->info;const auto date=env.local_date(info->timestamp);env.print(position,"No.%.2d %s %.2d/%.2d/%.2d %s %s %s",{static_cast<u32>(i+1),text_address(info->name),static_cast<u32>(date.year%100),static_cast<u32>(date.month+1),static_cast<u32>(date.day),text_address(env.characters[info->character*3+info->shot_type]),text_address(env.difficulties[info->difficulty]),text_address(env.replay_stage_names[info->last_stage])});}
            position.y=Scalar::add(position.y,15);}*env.color=0xffffffff;
    }else if(state==11||state==18){
        position={102,224,0};if(elapsed.current<10){const auto from=Extended::from_int(menu.selected)*number(15)+number(64);position.y=((number(224)-from)*number(displayed_elapsed(elapsed))*number(.1f)+from).to_float();}
        draw_name(position,env);position.x=48;const auto* info=(*env.replay)->info;const auto date=env.local_date(info->timestamp);
        env.print(position,"No.%.2d          %.2d/%.2d/%.2d %s %s %s",{static_cast<u32>(menu.selected+1),static_cast<u32>(date.year%100),static_cast<u32>(date.month+1),static_cast<u32>(date.day),text_address(env.characters[info->character*3+info->shot_type]),text_address(env.difficulties[info->difficulty]),text_address(env.replay_stage_names[env.game->stage])});
    }else if(state==12||state==19){
        const auto selection=menu.selected;position={48,64,0};env.print(position,"            Score Ranking!!");position.x=75;position.y=(Extended::from_int(selection)*number(18)+number(96)).to_float();if(!no_rank)draw_name(position,env);
        position={48,96,0};for(i32 i=0;i<10;++i){*env.color=selected_color(i==(no_rank?-1:selection));auto& game=*env.game;auto* record=&(*env.scores)->characters[game.character*3+game.shot_type].high_scores[game.difficulty][i];
            if(!record->timestamp)env.print(position,"%2d %s %.9ld%d --/--/-- Stage -",{static_cast<u32>(i+1),text_address(record->name),static_cast<u32>(record->score),static_cast<u32>(static_cast<std::int8_t>(record->score_units))});
            else{const auto date=env.local_date(record->timestamp);record=&(*env.scores)->characters[game.character*3+game.shot_type].high_scores[game.difficulty][i];env.print(position,"%2d %s %.9ld%d %.2d/%.2d/%.2d %s",{static_cast<u32>(i+1),text_address(record->name),static_cast<u32>(record->score),static_cast<u32>(static_cast<std::int8_t>(record->score_units)),static_cast<u32>(date.year%100),static_cast<u32>(date.month+1),static_cast<u32>(date.day),text_address(env.stage_names[static_cast<std::int8_t>(record->stage)])});}
            position.y=Scalar::add(position.y,18);}*env.color=0xffffffff;
    }
    *env.text_mode=0;return 1;
}
}
