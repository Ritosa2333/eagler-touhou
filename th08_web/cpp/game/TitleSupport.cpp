// Typed data/resource operations for the TH08 title menus.
#include "TitleMenus.hpp"
namespace th08 {
bool TitleContext::IsExtraUnlockedForCharacter(i32 character)const{
    if(character>3)return true;if(character<0)return false;
    for(u32 i=0;i<4;++i)if(clears[character].without_retries[i]&0x4000)return true;return false;
}
bool TitleContext::IsSpellPracticeUnlockedForCharacter(i32 character)const{
    if(character>3)return true;if(character<0)return false;
    for(u32 i=0;i<4;++i)if(clears[character].with_retries[i]&0x8000)return true;return false;
}
bool TitleContext::IsExtraUnlocked()const{for(i32 i=0;i<4;++i)if(IsExtraUnlockedForCharacter(i))return true;return false;}
bool TitleContext::IsSpellPracticeUnlocked()const{for(i32 i=0;i<4;++i)if(IsSpellPracticeUnlockedForCharacter(i))return true;return false;}
bool TitleContext::IsExtraUnlockedWithAllTeams()const{for(i32 i=0;i<4;++i)if(!IsExtraUnlockedForCharacter(i))return false;return true;}
TitleMenus::~TitleMenus(){release();}
void TitleMenus::release(){close_replay();delete[] state.vms;state.vms=nullptr;presentation_previous.clear();presentation_help_vm=nullptr;presentation_valid=false;}
void TitleMenus::snapshot_presentation(){
    presentation_previous.resize(std::max(0,state.vmCount));for(i32 i=0;i<state.vmCount;++i)presentation_previous[i]={state.vms[i].pos,state.vms[i].pos2,state.vms[i].scriptIndex,presentation::VisualSample(state.vms[i])};
    presentation_help_vm=state.currentHelpTextVm;if(presentation_help_vm)presentation_help={presentation_help_vm->pos,presentation_help_vm->pos2,presentation_help_vm->scriptIndex,presentation::VisualSample(*presentation_help_vm)};presentation_valid=true;
}
JobResult TitleMenus::update(){
    if(state.state!=TitleScreenState_Ready)return state.state==TitleScreenState_Close?JobResult::Exit:JobResult::Continue;
    snapshot_presentation();
    i32 result=1;switch(state.currentScreen){
    case TitleCurrentScreen_StartMenu:result=OnUpdateStartMenu();break;
    case TitleCurrentScreen_Option:result=OnUpdateOptions();break;
    case TitleCurrentScreen_KeyConfig:result=OnUpdateKeyConfig();break;
    case TitleCurrentScreen_Replay:result=OnUpdateReplayMenu();break;
    case TitleCurrentScreen_DifficultySelect:case TitleCurrentScreen_DifficultySelectPractice:case TitleCurrentScreen_DifficultySelectExtra:result=OnUpdateDifficultySelect();break;
    case TitleCurrentScreen_CharacterSelect:case TitleCurrentScreen_CharacterSelectPractice:case TitleCurrentScreen_CharacterSelectExtra:case TitleCurrentScreen_CharacterSelectSpell:result=OnUpdateCharacterSelect();break;
    case TitleCurrentScreen_PracticeStageSelect:result=OnUpdatePracticeStageSelect();break;
    case TitleCurrentScreen_SpellStageSelect:result=OnUpdateSpellStageSelect();break;
    case TitleCurrentScreen_SpellCardSelect:result=OnUpdateSpellCardSelect();break;
    default:break;
    }
    execute_animations();return JobResult(result);
}
void TitleMenus::initialize_vms(AnmLoaded& file){state.titleAnm=&file;delete[] state.vms;state.vmCount=142;state.vms=new AnmVm[state.vmCount];ExecuteAnmIdxArray(state.vms,0,state.vmCount);snapshot_presentation();}
void TitleMenus::initialize_help(AnmLoaded& file){
    context.textAnm=&file;
    for(i32 i=0;i<14;++i){auto& vm=state.helpTextVms[i];vm.scriptIndex=9;executor.start(file,vm,file.scripts[9]);file.SetSprite(&vm,vm.activeSpriteIndex+i);
        file.SetSprite(&state.spellCardInfoVms[i],i+21);auto& info=state.spellCardInfoVms[i];info.pos={64,float(i*16+352+(i>4?10:0)),0};info.anchor=3;info.fontWidth=info.fontHeight=15;info.flag1=true;}
    state.currentHelpTextVm=state.helpTextVms;
}
void TitleMenus::ExecuteAnmIdxArray(AnmVm* vms,i32 first,i32 count){for(i32 i=0;i<count;++i){auto& vm=vms[i];vm.scriptIndex=i16(first+i);vm.pos={};vm.pos2={};vm.fontWidth=vm.fontHeight=15;executor.start(*state.titleAnm,vm,state.titleAnm->scripts[first+i]);vm.baseSpriteIndex=vm.activeSpriteIndex;}}
void TitleMenus::SetInterruptArray(AnmVm* vms,i32 count,i16 value){for(i32 i=0;i<count;++i)vms[i].SetInterrupt(value);}
void TitleMenus::DrawTextCentered(AnmVm* vm,u32 color,u32 outline,const char* message){text.draw(*vm,TextAlignment::Center,color,outline,message);}
void TitleMenus::DrawTextLeft(AnmVm* vm,u32 color,u32 outline,const char* message){text.draw(*vm,TextAlignment::Left,color,outline,message);}
void TitleMenus::InitializeAndSetSprite(AnmLoaded* file,AnmVm* vm,i32 index){vm->Initialize();vm->anmFile=file;file->SetSprite(vm,index);}
void TitleMenus::UnlockLastWordSpellCards(){SpellProgress(context.spells,context.clears,context.last_words).unlock_last_words();}
void TitleMenus::execute_animations(){for(i32 i=0;i<state.vmCount;++i)executor.execute(state.vms[i]);if(state.currentHelpTextVm)executor.execute(*state.currentHelpTextVm);}
void TitleMenus::ChangeCurrentScreen(TitleCurrentScreen screen){state.previousScreen=state.currentScreen;state.currentScreen=screen;state.stateTimer=state.stateTimer2=state.currentScreenState=state.idleFrames=0;}
i32 TitleMenus::SetKeyNumberSprite(AnmVm* vms,i16 key){
    if(key<0)vms[0].flag1=vms[1].flag1=false;
    else{state.titleAnm->SetSprite(vms,vms[0].baseSpriteIndex+key/10*2);state.titleAnm->SetSprite(vms+1,vms[1].baseSpriteIndex+key%10*2);vms[0].flag1=vms[1].flag1=true;}
    return 0;
}
void TitleMenus::SetKeyConfigKey(i16 old_key,i16 new_key,i32){
    auto& map=state.controllerMapping;
    for(auto* key:{&map.shotButton,&map.bombButton,&map.focusButton,&map.upButton,&map.downButton,&map.leftButton,&map.rightButton,&map.menuButton,&map.skipButton})if(*key==old_key)*key=new_key;
}
bool TitleMenus::start_demo(){
    auto file=actions.read_replay(context.replayFilename);ReplayFile replay;
    if(!replay.decode(file.data(),file.size()))return false;
    ReplayMetadata info;std::memcpy(&info,replay.decoded().data(),sizeof(info));
    i32 first=0;while(first<9&&!info.header.stage_offsets[first])++first;if(first==9)return false;
    context.SetIsReplayWeird(true);context.flags.isDemoMode=true;
    context.difficulty=info.difficulty;context.shotType=info.shot_type%2;context.character=info.shot_type;context.currentStage=first;
    context.supervisor_state=2;context.replayMode=0;context.flags.isSpellPractice=false;state.currentReplay=nullptr;return true;
}
}
