// Names and algorithms adapted from GensokyoClub/th08, MIT license.
#include "TitleView.hpp"
#include "GameMath.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#include <array>
namespace th08 {
namespace {
constexpr const char* stages[]={"Stage1 ","Stage2 ","Stage3 ","Stage4A","Stage4B","Stage5 ","Stage6A","Stage6B","StageEX"};
constexpr const char* spell_stages_names[]={"Stage1   ","Stage2   ","Stage3   ","Stage4A  ","Stage4B  ","Stage5   ","Stage6A  ","Stage6B  ","Extra    ","Last Word"};
constexpr const char* characters[]={"Rm & Yk","Ms & Al","Sk & Rr","Ym & Yy","Reimu  ","Yukari ","Marisa ","Alice  ","Sakuya ","Remilia","Youmu  ","Yuyuko "};
constexpr const char* difficulties[]={"Easy    ","Normal  ","Hard    ","Lunatic ","Extra   "};
const char* mark(i32 captured,i32 all,i32 count){return captured>=count?"@":all>=count?"*":" ";}
float move(float value,float delta){return Scalar::add(value,delta);}
float progress(i32 count,i32 total,float previous,i32 frames){
    float target=(Extended::from_int(count)/Extended::from_int(total)).to_float();
    if(frames<50&&previous!=target)target=(((number(target)-number(previous))*Extended::from_int(frames))/number(50)+number(previous)).to_float();
    return target;
}
}
i32 TitleView::replays(){
    auto* vm=state.vms+79;add(vm->pos,"No.   Name       Date  Player   Rank");
    const i32 first=state.selectedReplay-state.selectedReplay%15;
    for(i32 i=first;i<first+15&&i<state.replayCount;++i){if(i<0||i>=60)break;++vm;const auto& replay=state.replays[i];
        ascii.state.selected=i==state.selectedReplay;ascii.state.color=ascii.state.selected?0xffffffff:0xff808080;
        // Native strings can span adjacent fields if an imported file is
        // malformed. Valid replays always terminate within these fixed fields.
        char name[9]{},date[7]{};std::memcpy(name,replay.player_name,8);std::memcpy(date,replay.date,6);
        const char* character=replay.shot_type<12?characters[replay.shot_type]:"???????";
        if(replay.spell_number<0)add(vm->pos,"%s %8s  %6s %7s  %8s",state.replayNumbers[i],name,date,character,replay.difficulty<5?difficulties[replay.difficulty]:"????????");
        else add(vm->pos,"%s %8s  %6s %7s  Spell No.%3d",state.replayNumbers[i],name,date,character,replay.spell_number+1);
    }
    if((state.currentScreenState==2||state.currentScreenState==3)&&state.currentReplay){
        reset_text();const auto& replay=*state.currentReplay;add(state.vms[78].pos,"       %2.3f%%",double(replay.lag));vm=state.vms+95;add(vm->pos,"Stage    LastScore");
        if(replay.spell_number<0){for(i32 i=0;i<9;++i){++vm;if(state.currentScreenState!=3){ascii.state.selected=i==state.selectedReplayStage;ascii.state.color=ascii.state.selected?0xffffffff:0xff808080;}
                else ascii.state.color=i==state.selectedReplayStage?0x60ffffff:0x60808080;
                ReplayStage stage;if(menus.replay_stage(i,stage))add(vm->pos,"%s %9d0",stages[i],i32(stage.end_score));else add(vm->pos,"%s ----------",stages[i]);
            }
        }else{vm+=2;add(vm->pos,"No.%.3d %9d0",replay.spell_number+1,i32(replay.spell_score));auto& name=state.spellCardNameVms[0];name.pos=vm->pos;name.pos.y=move(name.pos.y,16);renderer.draw_no_rotation(name);}
    }
    reset_text();return 1;
}
i32 TitleView::practice(){
    reset_text();Vec3 position=state.vms[141].pos;position.y=move(position.y,-96);add(position,"Stage    HI-Score");position.y=move(position.y,16);
    if(context.character<0||context.character>=12||config.difficulty>=5)return 1;
    u16 clear=context.clears[context.character].with_retries[config.difficulty];if(!clear)clear=1;if(clear&128)clear|=24;
    const auto& scores=context.practice_scores[context.character];
    for(i32 i=0;i<8;++i){ascii.state.selected=i==state.cursor;ascii.state.color=ascii.state.selected?0xffffffff:(clear&(1<<i))?0xffa0a0a0:0xff404040;
        add(position,"%s %9d0 (%3d)",stages[i],scores.high_scores[i][config.difficulty],scores.attempts[i][config.difficulty]);position.y=move(position.y,16);}
    reset_text();return 1;
}
i32 TitleView::completion(){
    const auto clear=[&](bool retries,i32 stage,i32 difficulty){return state.cursor>=0&&state.cursor<13&&difficulty>=0&&difficulty<5&&((retries?context.clears[state.cursor].with_retries:context.clears[state.cursor].without_retries)[difficulty]&(1<<stage));};
    i32 sprite=-1;if(state.stateTimer2>8){
        if(clear(false,7,config.difficulty)&&clear(true,6,config.difficulty))sprite=146;
        else if(clear(false,7,config.difficulty))sprite=148;
        else if(clear(false,7,0)||clear(false,7,1)||clear(false,7,2)||(clear(false,7,3)&&state.cursor>3))sprite=147;
        else if(clear(true,6,0)||clear(true,6,1)||clear(true,6,2)||clear(true,6,3))sprite=145;
    }
    if(sprite>=0){AnmVm local;AnmVm* vm=&state.spellCardNameVms[0];if(presentation::render_only)vm=&local;menus.InitializeAndSetSprite(state.titleAnm,vm,sprite);vm->anchor=3;vm->color1.d3dColor=0xffffffff;vm->pos={400,170,0};renderer.draw_no_rotation(*vm);}return 0;
}
i32 TitleView::spell_cards(){
    reset_text();Vec3 position{16,78,0};position.x=move(position.x,318);state.spellCardNameVms[15].pos=position;renderer.draw_no_rotation(state.spellCardNameVms[15]);position.x=move(position.x,-318);position.y=move(position.y,16);
    if(context.character<0||context.character>12||context.currentStage<0||context.currentStage>=10)return 1;
    for(i32 i=0;i<15&&i+state.currentPageSpellCardSelect*15<state.currentNumberOfSpellCards;++i){const i32 index=i+state.currentPageSpellCardSelect*15;if(index<0||index>=spell_stage_counts[context.currentStage])break;
        const i32 number=spells_by_stage[context.currentStage][index];const auto& spell=context.spells[number];const i32 shot=context.character;auto& vm=state.spellCardNameVms[i];ascii.state.color=u32(vm.color1.d3dColor);
        add(position,"%sNo.%.3d",spell.practice.captures[shot]?"@":spell.practice.captures[12]?"*":" ",number+1);position.x=move(position.x,414);ascii.state.scale_x=.8f;ascii.state.scale_y=1;
        add(position,"%3d/%3d(%3d/%3d)",spell.practice.captures[shot],spell.practice.attempts[shot],spell.game.captures[shot],spell.game.attempts[shot]);ascii.state.scale_x=ascii.state.scale_y=1;
        position.x=move(position.x,-414);vm.pos=position;vm.pos.x=move(vm.pos.x,102);renderer.draw_no_rotation(vm);position.y=move(position.y,16);
    }
    for(i32 i=0;i<7;++i)renderer.draw_no_rotation(state.spellCardInfoVms[i]);reset_text();return 1;
}
i32 TitleView::spell_stages(){
    const float saved_percentages[]{state.percentageCapturedSpellPracticePerShot,state.percentageCapturedInGamePerShot,state.percentageCapturedSpellPractice,state.percentageCapturedInGame};
    if(context.character<0||context.character>12)return 1;
    i32 totals[6]{};reset_text();Vec3 position=state.vms[141].pos;position.y=move(position.y,-152);position.x=move(position.x,-32);state.spellCardNameVms[0].pos=position;renderer.draw_no_rotation(state.spellCardNameVms[0]);position.y=move(position.y,16);
    for(i32 i=0;i<10;++i){i32 counts[6]{};ascii.state.selected=i==state.cursor;ascii.state.color=ascii.state.selected?0xffffffff:0xffa0a0a0;
        for(i32 j=0;j<spell_stage_counts[i];++j){const i32 number=spells_by_stage[i][j];const auto& spell=context.spells[number];
            for(i32 all=0;all<2;++all){const i32 shot=all?12:context.character;counts[all]+=spell.practice.captures[shot]>0;counts[2+all]+=spell.game.captures[shot]>0;
                counts[4+all]+=spell.game.attempts[shot]>0||spell.practice.attempts[shot]>0||(i>=9&&context.IsLastWordSpellCardAttempted(number));}}
        for(i32 j=0;j<6;++j)totals[j]+=counts[j];add(position,"%s%s",mark(counts[0],counts[1],spell_stage_counts[i]),spell_stages_names[i]);position.x=move(position.x,182);ascii.state.scale_x=.75f;ascii.state.scale_y=1;
        add(position,"%3d(%3d)/%3d/%3d",counts[0],counts[1],counts[5],spell_stage_counts[i]);position.x=move(position.x,-182);position.y=move(position.y,16);ascii.state.scale_x=ascii.state.scale_y=1;
    }
    position.y=move(position.y,5);ascii.state.selected=0;ascii.state.color=0xffd06060;add(position,"%sTotal",mark(totals[0],totals[1],222));position.x=move(position.x,182);ascii.state.scale_x=.75f;ascii.state.scale_y=1;
    add(position,"%3d(%3d)/%3d/%3d",totals[0],totals[1],totals[5],222);position.x=move(position.x,-182);ascii.state.scale_x=ascii.state.scale_y=1;reset_text();
    state.spellCardNameVms[1].pos={400,376,0};renderer.draw_no_rotation(state.spellCardNameVms[1]);state.spellCardNameVms[2].pos={552,426,0};renderer.draw_no_rotation(state.spellCardNameVms[2]);
    for(i32 all=0;all<2;++all){auto& practice=all?state.percentageCapturedSpellPractice:state.percentageCapturedSpellPracticePerShot;auto& game=all?state.percentageCapturedInGame:state.percentageCapturedInGamePerShot;
        practice=progress(totals[all],222,practice,state.stateTimer2);game=progress(totals[2+all],205,game,state.stateTimer2);
        const float x=all?562:530,y=all?438:420;pie({x,y,.02f},all?0x6080c0c0:0x60c0c0f0,practice,all?32:48);pie({x,y,.01f},all?0x8080c0c0:0x80c0c0f0,game,all?16:24);
        ascii.draw_percentage({all?580.0f:514.0f,all?451.0f:404.0f,.01f},(number(practice)*number(10000)).truncate_int(),0xffffffff);
        ascii.draw_percentage({all?580.0f:514.0f,all?443.0f:412.0f,.01f},(number(game)*number(10000)).truncate_int(),0x80c0c080);
    }
    if(presentation::render_only){state.percentageCapturedSpellPracticePerShot=saved_percentages[0];state.percentageCapturedInGamePerShot=saved_percentages[1];state.percentageCapturedSpellPractice=saved_percentages[2];state.percentageCapturedInGame=saved_percentages[3];}
    return 1;
}
void TitleView::pie(const Vec3& position,u32 color,float fraction,float diameter){
    UntexturedVertex vertices[64];AnmVm vm;vm.blendMode=0;vm.color1.d3dColor=-1;vm.zWriteDisabled=true;vm.flag15=false;vertices[0]={position,1,color};
    float angle=-1.5707963705062866f;const float radius=Scalar::div(diameter,2);
    for(i32 i=1;i<64;++i){const float sin=sine(angle).to_float(),cos=cosine(angle).to_float();auto& vertex=vertices[i];vertex.pos.x=(number(cos)*number(radius)-number(sin)*number(0)).to_float();vertex.pos.y=(number(sin)*number(radius)+number(cos)*number(0)).to_float();
        vertex.pos.x=move(vertex.pos.x,position.x);vertex.pos.y=move(vertex.pos.y,position.y);vertex.pos.z=position.z;vertex.reciprocal_w=1;vertex.color=color;angle=add_angle(angle,Scalar::mul(.10134170204401016f,fraction));}
    renderer.draw_fan(vm,vertices,64);
}
i32 TitleView::draw(){
    if(state.state!=TitleScreenState_Ready)return 1;renderer.current_texture=0;menus.actions.background();
    std::array<Vec3,21> saved_name_positions{};if(presentation::render_only)for(size_t i=0;i<saved_name_positions.size();++i)saved_name_positions[i]=state.spellCardNameVms[i].pos;
    for(i32 i=0;i<state.vmCount;++i){auto& source=state.vms[i];AnmVm copy;AnmVm& vm=presentation::render_only?(copy=source):source;if(!vm.loadedSprite||!vm.anmFile||!vm.anmFile->textures)continue;const Vec3 position=vm.pos;Vec3 draw_pos=vm.pos,draw_pos2=vm.pos2;
        TH08_AUDIT_SCOPE(Title,&source,source.currentTimeInScript.current,u32(i));
        if(menus.presentation_valid&&i<i32(menus.presentation_previous.size()))menus.presentation_previous[i].visual.apply(source,vm,presentation::alpha);
        if(presentation::active&&menus.presentation_valid&&i<i32(menus.presentation_previous.size())){const auto& before=menus.presentation_previous[i];if(before.script==vm.scriptIndex){draw_pos={presentation::lerp(before.pos.x,vm.pos.x),presentation::lerp(before.pos.y,vm.pos.y),presentation::lerp(before.pos.z,vm.pos.z)};draw_pos2={presentation::lerp(before.pos2.x,vm.pos2.x),presentation::lerp(before.pos2.y,vm.pos2.y),presentation::lerp(before.pos2.z,vm.pos2.z)};}}
        vm.pos={move(draw_pos.x,draw_pos2.x),move(draw_pos.y,draw_pos2.y),move(draw_pos.z,draw_pos2.z)};
        if(vm.rotation.z!=0)renderer.draw_2d(vm);else renderer.draw_no_rotation(vm);vm.pos=position;}
    if(state.currentHelpTextVm){TH08_AUDIT_SCOPE(TitleHelp,state.currentHelpTextVm,state.currentHelpTextVm->currentTimeInScript.current,0);if(presentation::active&&menus.presentation_valid&&menus.presentation_help_vm==state.currentHelpTextVm&&menus.presentation_help.script==state.currentHelpTextVm->scriptIndex){auto draw=*state.currentHelpTextVm;menus.presentation_help.visual.apply(*state.currentHelpTextVm,draw,presentation::alpha);draw.pos={presentation::lerp(menus.presentation_help.pos.x,draw.pos.x),presentation::lerp(menus.presentation_help.pos.y,draw.pos.y),presentation::lerp(menus.presentation_help.pos.z,draw.pos.z)};renderer.draw_no_rotation(draw);}else renderer.draw_no_rotation(*state.currentHelpTextVm);}
    switch(state.currentScreen){case TitleCurrentScreen_CharacterSelect:completion();break;case TitleCurrentScreen_Replay:replays();break;case TitleCurrentScreen_PracticeStageSelect:practice();break;case TitleCurrentScreen_SpellStageSelect:spell_stages();break;case TitleCurrentScreen_SpellCardSelect:spell_cards();break;default:break;}if(presentation::render_only)for(size_t i=0;i<saved_name_positions.size();++i)state.spellCardNameVms[i].pos=saved_name_positions[i];return 1;
}
}
