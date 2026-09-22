#include "TitleReplays.hpp"
#include "HighRefresh.hpp"
#include <cstring>
namespace th10 {
namespace {float displayed_elapsed(const Timer& t){if(!high_refresh::active||t.current<=0||t.current-t.previous>2)return t.fractional;return high_refresh::lerp(float(t.previous),t.fractional);}}
void replay_slot_filename(char (&filename)[12],i32 slot) noexcept {
    std::memcpy(filename,"th10_00.rpy",11);filename[11]=0;filename[5]='0'+slot/10;filename[6]='0'+slot%10;
}
// 0x4315c0. Numbered slots are followed by up to 25 user-file previews.
// The original list cursor still has 25 entries; its separate page is reset.
i32 TitleReplays::update(){
    auto& t=title;auto& env=environment;auto& m=t.menu;
    switch(t.phase){
    case 0:{
#ifdef TH_ENABLE_THPRAC
        // th10_rep_menu_1 / THGuiRep::State(1).
        env.reset_practice();
#endif
        m.item_count=25;m.select(*env.remembered_replay);*env.remembered_replay=0;
        if(!env.registry->find(t.animation_ids[94])){t.create_script(94,env);t.animation_ids[195]=env.create(**env.effects,8);}
        t.create_script(101,env);t.set_phase(1,env.rate);std::memset(t.previews,0,sizeof(t.previews));
        for(i32 slot=0;slot<25;++slot){char name[12];replay_slot_filename(name,slot+1);t.previews[slot]=env.preview(name);}
        env.make_directory("replay");env.change_directory("replay");ReplaySearchEntry found;
        const auto handle=env.find_first("th10_ud????.rpy",found);
        if(handle!=0xffffffff){for(i32 slot=25;slot<50;++slot){env.change_directory("../");t.previews[slot]=env.preview(found.filename);env.change_directory("replay");if(!env.find_next(handle,found))break;}}
        env.find_close(handle);env.change_directory("../");t.replay_page=0;
        [[fallthrough]];}
    case 1:if(t.elapsed.current>6)t.set_phase(2,env.rate);break;
    case 2:
        m.reserved=m.selected;if((*env.pressed|*env.repeated)&0x10)m.move(-1);if((*env.pressed|*env.repeated)&0x20)m.move(1);if(m.reserved!=m.selected)env.sound(12);
        if(*env.pressed&10){t.set_phase(5,env.rate);env.sound(11);}
        else if((*env.pressed&0x1001)&&t.previews[m.selected]){
            t.set_phase(4,env.rate);t.replay_index=m.selected;m.push();env.sound(10);m.item_count=7;m.select(0);
#ifdef TH_ENABLE_THPRAC
            // th10_rep_menu_2 / THGuiRep::State(2): inspect the opened replay.
            env.check_practice(t.previews[m.selected]->filename);
#endif
            for(i32 stage=0;stage<7;++stage)if(!t.previews[t.replay_index]->readers[stage+1].stage)m.disabled_items[m.disabled_count++]=stage;
            m.move(-1);m.move(1);
        }
        break;
    case 3:
        if(t.elapsed.current==2){env.hide_screen();env.show_loading(480,392);}
        if(t.elapsed.current>=32){
            t.set_screen(3,env.rate);env.game->stage=t.replay_stage+1;env.game->reserved_040=env.game->stage;*env.current_stage=env.stage_table+env.game->stage;
            env.fade_music(6);*env.pending_screen=12;std::strcpy(env.replay_filename,t.previews[t.replay_index]->filename);
            const auto* info=t.previews[t.replay_index]->info;env.game->character=info->character;env.game->shot_type=info->shot_type;env.game->difficulty=info->difficulty;
            *env.remembered_replay=t.replay_index;*env.return_screen=2;
        }break;
    case 4:
        if(t.elapsed.current<15)return 1;
        m.reserved=m.selected;if((*env.pressed|*env.repeated)&0x10)m.move(-1);if((*env.pressed|*env.repeated)&0x20)m.move(1);if(m.reserved!=m.selected)env.sound(12);
        if(*env.pressed&10){m.pop();m.item_count=25;m.disabled_count=0;t.set_phase(2,env.rate);env.sound(11);}
        else if(*env.pressed&0x1001){
#ifdef TH_ENABLE_THPRAC
            // th10_rep_menu_3 / THGuiRep::State(3): commit the practice run.
            env.activate_practice();
#endif
            t.replay_stage=m.selected;t.set_phase(3,env.rate);}break;
    case 5:
        if(t.elapsed.current>=6){
            for(auto* preview:t.previews)if(preview)env.delete_replay(preview);std::memset(t.previews,0,sizeof(t.previews));
            t.dismiss_script(101,env);t.signal_script(90,8,env);t.signal_script(91,8,env);t.dismiss_script(94,env);env.registry->interrupt(t.animation_ids[195],1);t.set_screen(2,env.rate);m.pop();
        }break;
    }
    return 1;
}
void draw_replay_description(const ReplayInfo& info,i32 slot,const Vec3& position,ResultsDrawEnvironment& env,const char* const* difficulties){
    const auto date=env.local_date(info.timestamp);const double slow=number(info.slow_rate).to_double();u32 parts[2];std::memcpy(parts,&slow,8);
    env.print(position,"No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",{static_cast<u32>(slot),reinterpret_cast<uintptr_t>(info.name),static_cast<u32>(date.year%100),static_cast<u32>(date.month+1),static_cast<u32>(date.day),static_cast<u32>(date.hour),static_cast<u32>(date.minute),reinterpret_cast<uintptr_t>(env.characters[info.character*3+info.shot_type]),reinterpret_cast<uintptr_t>(difficulties[info.difficulty]),reinterpret_cast<uintptr_t>(env.replay_stage_names[info.last_stage]),parts[0],parts[1]});
}
// 0x431ba0. Stage rows show the next stage's starting score when available;
// final stages use the overall replay result. Both preserve its units digit.
i32 draw_title_replays(const TitleMenu& t,ResultsDrawEnvironment& env,const char* const* difficulties){
    if(t.phase==2){
        Vec3 position{58,80,0};*env.text_mode=1;
        for(i32 slot=0;slot<25;++slot){
            *env.color=t.menu.selected==slot?0xffffff00:0xff808080;
            if(t.previews[slot])draw_replay_description(*t.previews[slot]->info,slot+1,position,env,difficulties);
            else env.print(position,"No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%",{static_cast<u32>(slot+1)});
            position.y=Scalar::add(position.y,15.0f);
        }
    }else if(t.phase==4){
        const auto* info=t.previews[t.replay_index]->info;Vec3 position{80,80,0};
        if(t.elapsed.current<10)position.y=((number(10.0f)-number(displayed_elapsed(t.elapsed)))*Extended::from_int(static_cast<i32>(static_cast<u32>(t.replay_index)*15u))*number(.1f)+number(80.0f)).to_float();
        *env.text_mode=1;draw_replay_description(*info,t.replay_index+1,position,env,difficulties);
        if(t.elapsed.current>=10){
            position={220,128,0};for(i32 stage=1;stage<8;++stage){
                *env.color=t.menu.selected==stage-1?0xffffff00:0xff808080;const auto* preview=t.previews[t.replay_index];const u32 name=reinterpret_cast<uintptr_t>(env.stage_names[stage]);
                if(!preview->readers[stage].stage)env.print(position,"%s  ---------",{name});
                else{const auto* next=stage<6?preview->readers[stage+1].stage:nullptr;env.print(position,"%s  %.8d%d",{name,static_cast<u32>(next?next->score:info->score),static_cast<u32>(next?next->score_units:info->score_units)});}
                position.y=Scalar::add(position.y,18.0f);
            }
        }
    }else return 1;
    *env.color=0xffffffff;*env.text_mode=0;return 1;
}
}
