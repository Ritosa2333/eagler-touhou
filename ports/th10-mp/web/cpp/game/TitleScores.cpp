#include "TitleScores.hpp"
#include <cstring>
namespace th10 {
// 0x4088c0 and 0x42c8c0. Music unlocks are separate from these flags.
i32 count_spells(const std::int8_t* difficulties,i32 difficulty) noexcept {
    i32 count=0;for(i32 i=0;i<110;++i)if(difficulties[i]==difficulty)++count;return count;
}
void unlock_score_records(ScoreData& score) noexcept {
    std::memset(score.settings.statistics+1,1,16);
    for(i32 character=0;character<6;++character)
        for(i32 slot=0;slot<24;++slot)score.characters[character].statistics[0x21+slot*8]=1;
}
// 0x432690. Discovery is shared by all characters; capture/attempt counts
// come from the selected shot. Original names are padded in bytes, not glyphs.
i32 TitleScores::fill_spell_list(){
    auto& t=title;auto& env=environment;i32 spell=0;
    const i32 skip=(t.tertiary.selected-1)*10;
    for(i32 skipped=0;skipped<skip&&spell<110;++spell)
        if(env.spell_difficulties[spell]==t.secondary.selected)++skipped;
    t.reserved_2ac=0;
    for(i32 row=0;row<10;++row){
        while(spell<110&&env.spell_difficulties[spell]!=t.secondary.selected)++spell;
        if(spell>=110){env.print(env.registry->find_and_clear(t.animation_ids[196+row]),0xffffffff," ");continue;}
        const auto& shared=(*env.scores)->characters[6].spells[spell];
        const auto& record=(*env.scores)->characters[t.menu.selected].spells[spell];
        if(!shared.attempts){
            auto* vm=env.registry->find_and_clear(t.animation_ids[196+row]);
            env.print(vm,0x808080,env.unknown_spell_format,{static_cast<u32>(spell+1),static_cast<u32>(record.captures),static_cast<u32>(record.attempts)});
        }else{
            char name[168];const auto length=std::strlen(shared.name);std::memcpy(name,shared.name,length);
            const auto padded=length<42?42:length;if(length<42)std::memset(name+length,' ',42-length);name[padded]=0;
            auto* vm=env.registry->find_and_clear(t.animation_ids[196+row]);
            env.print(vm,record.captures?0xffff80:0xefefef,"No.%3d %s %4d/%4d",{static_cast<u32>(spell+1),reinterpret_cast<uintptr_t>(name),static_cast<u32>(record.captures),static_cast<u32>(record.attempts)});
        }
        ++spell;++t.reserved_2ac;
    }
    return 0;
}
void TitleScores::update_unlock_code(){
    auto& env=environment;
    if(*env.pressed&0x160b){*env.unlock_cursor=0;*env.unlock_elapsed=0;}
    std::memcpy(env.previous_keyboard,env.keyboard,256);
    if(env.read_keyboard()){
        for(i32 i=0;i<256;++i)env.pressed_keyboard[i]=(env.previous_keyboard[i]^env.keyboard[i])&env.keyboard[i];
        if(*env.unlock_cursor>=22){unlock_score_records(**env.scores);env.sound(44);*env.unlock_cursor=0;}
        else if(env.pressed_keyboard[env.unlock_sequence[*env.unlock_cursor]]&0x80){++*env.unlock_cursor;*env.unlock_elapsed=0;}
        else{u8 pressed=0;for(i32 i=0;i<57;++i)pressed|=env.pressed_keyboard[i];if(pressed&0x80)*env.unlock_cursor=0;}
    }
    *env.unlock_elapsed=wrapping_add(*env.unlock_elapsed,1);
    if(*env.unlock_elapsed>300){*env.unlock_cursor=0;*env.unlock_elapsed=0;}
}
// 0x431ee0. Page zero is the score table; later pages contain ten spells each.
i32 TitleScores::update(){
    auto& t=title;auto& env=environment;auto& main=t.menu;auto& difficulty=t.secondary;auto& page=t.tertiary;
    constexpr i32 decorations[]={168,169,170,171,165,166,167,172};
    switch(t.phase){
    case 0:
        main.item_count=6;main.select(0);difficulty.item_count=5;difficulty.select(1);difficulty.wrap=1;
        page.item_count=(count_spells(env.spell_difficulties,difficulty.selected)+9)/10+1;page.select(0);page.wrap=1;
        if(!env.registry->find(t.animation_ids[94])){t.create_script(94,env);t.animation_ids[195]=env.create(**env.effects,8);}
        t.create_script(102,env);t.set_phase(1,env.rate);t.create_script(152+main.selected/3,env);t.create_script(154+main.selected,env);t.create_script(160+difficulty.selected,env);
        for(auto script:decorations)t.create_script(script,env);
        [[fallthrough]];
    case 1:if(t.elapsed.current>6)t.set_phase(2,env.rate);break;
    case 2:
        main.reserved=main.selected;difficulty.reserved=difficulty.selected;page.reserved=page.selected;
        if((*env.pressed|*env.repeated)&0x10){difficulty.move(-1);env.interrupt_immediately(t.animation_ids[170],2);}
        if((*env.pressed|*env.repeated)&0x20){difficulty.move(1);env.interrupt_immediately(t.animation_ids[171],2);}
        if(difficulty.reserved!=difficulty.selected){
            env.sound(12);t.dismiss_script(160+difficulty.reserved,env);t.create_script(160+difficulty.selected,env);
            // The original fills the new difficulty before updating page count.
            if(page.selected>0){page.select(1);fill_spell_list();}
            page.item_count=(count_spells(env.spell_difficulties,difficulty.selected)+9)/10+1;
        }
        if((*env.pressed|*env.repeated)&0x40){main.move(-1);env.interrupt_immediately(t.animation_ids[168],2);}
        if((*env.pressed|*env.repeated)&0x80){main.move(1);env.interrupt_immediately(t.animation_ids[169],2);}
        if(main.reserved!=main.selected){
            env.sound(12);
            if(main.reserved/3!=main.selected/3){t.dismiss_script(152+main.reserved/3,env);t.create_script(152+main.selected/3,env);}
            t.dismiss_script(154+main.reserved,env);t.create_script(154+main.selected,env);if(page.selected>0)fill_spell_list();
        }
        if(*env.pressed&0x1001){
            if(!page.selected)for(i32 row=0;row<10;++row)t.animation_ids[196+row]=env.create(**env.rows,23+row);
            page.move(1);
            if(!page.selected)for(i32 row=0;row<10;++row)env.registry->interrupt(t.animation_ids[196+row],1);
            else fill_spell_list();
            env.sound(10);
        }
        if(difficulty.selected==4&&main.selected==2)update_unlock_code();
        if(*env.pressed&10){
            t.set_phase(3,env.rate);env.sound(11);t.dismiss_script(160+difficulty.selected,env);t.dismiss_script(154+main.selected,env);t.dismiss_script(152+main.selected/3,env);
            for(auto script:decorations)t.dismiss_script(script,env);
            for(i32 row=0;row<10;++row)env.registry->interrupt(t.animation_ids[196+row],1);
        }
        break;
    case 3:
        if(t.elapsed.current>=6){t.dismiss_script(102,env);t.signal_script(90,8,env);t.signal_script(91,8,env);t.dismiss_script(94,env);env.registry->interrupt(t.animation_ids[195],1);t.set_screen(2,env.rate);main.pop();}break;
    }
    return 1;
}
void draw_score_entry(i32 character,i32 difficulty,i32 row,const Vec3& position,ResultsDrawEnvironment& env){
            auto* record=&(*env.scores)->characters[character].high_scores[difficulty][row];
            ReplayDate date{};if(record->timestamp){date=env.local_date(record->timestamp);record=&(*env.scores)->characters[character].high_scores[difficulty][row];}
            const double slow=number(record->slow_rate).to_double();u32 parts[2];std::memcpy(parts,&slow,8);
            const u32 name=reinterpret_cast<uintptr_t>(record->name),score=static_cast<u32>(record->score),units=static_cast<u32>(static_cast<std::int8_t>(record->score_units));
            if(!record->timestamp)env.print(position,"%2d  %s  %9ld%d  ----/--/-- --:--  Stage -  ---%%",{static_cast<u32>(row+1),name,score,units,parts[0],parts[1]});
            else env.print(position,"%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%",{static_cast<u32>(row+1),name,score,units,static_cast<u32>(date.year+1900),static_cast<u32>(date.month+1),static_cast<u32>(date.day),static_cast<u32>(date.hour),static_cast<u32>(date.minute),reinterpret_cast<uintptr_t>(env.stage_names[static_cast<std::int8_t>(record->stage)]),parts[0],parts[1]});
}
// 0x4329f0. Preserve date conversion, signed score digits, the original
// gradient, and all floating-point varargs (including the unused default row).
i32 draw_title_scores(const TitleMenu& title,ResultsDrawEnvironment& env){
    if(title.phase!=2)return 1;
    const i32 difficulty=title.secondary.selected;*env.text_mode=1;
    if(!title.tertiary.selected){
        Vec3 position{48,160,0};
        for(i32 row=0;row<10;++row){
            const u32 shade=255-row*16;*env.color=0xff0000ff|(shade<<16)|(shade<<8);
            draw_score_entry(title.menu.selected,difficulty,row,position,env);
            position.y=Scalar::add(position.y,18.0f);
        }
    }
    *env.color=0xffffffff;Vec3 position{328,378,0};u32 count;
    std::memcpy(&count,(*env.scores)->characters[title.menu.selected].statistics,4);env.print(position,"    %5d",{count});
    i32 frames;std::memcpy(&frames,(*env.scores)->characters[title.menu.selected].statistics+4,4);position.y=396;
    env.print(position,"%3d:%.2d:%.2d",{static_cast<u32>(frames/216000),static_cast<u32>((frames/3600)%60),static_cast<u32>((frames/60)%60)});
    std::memcpy(&count,(*env.scores)->characters[title.menu.selected].statistics+8+difficulty*4,4);position.y=414;env.print(position,"    %5d",{count});
    *env.color=0xffffffff;*env.text_mode=0;return 1;
}
}
