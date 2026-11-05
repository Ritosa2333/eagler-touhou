// TH08 replay menu. Platform file enumeration is explicit; decoded metadata
// and stage offsets are ordinary C++ data and never executable addresses.
#include "TitleMenus.hpp"
#include "Localization.hpp"
#include <cstdio>
namespace th08 {
void TitleMenus::scan_replays(){
    i32 count=0;
    const auto add=[&](const char* path,const char* label,const std::vector<u8>& file){
        ReplayFile replay;if(!replay.decode(file.data(),file.size()))return;
        auto& metadata=state.replays[count];std::memcpy(&metadata,replay.decoded().data(),sizeof(metadata));
        std::snprintf(state.replayFilePaths[count],512,"%s",path);std::snprintf(state.replayNumbers[count],8,"%s",label);++count;
    };
    for(i32 i=1;i<=15;++i){char path[64],label[8];std::snprintf(path,sizeof(path),"./replay/th8_%02d.rpy",i);std::snprintf(label,sizeof(label),"No.%02d",i);auto file=actions.read_replay(path);if(!file.empty())add(path,label,file);}
    const auto names=actions.list_user_replays();u32 index=0;
    for(i32 attempt=0;attempt<45&&index<names.size();++attempt){
        const auto path=std::string("./replay/")+names[index];auto file=actions.read_replay(path.c_str());
        // Original FindNextFile occurs after the read-success branch. A file
        // that disappears is retried within this bounded enumeration loop.
        if(file.empty())continue;
        add(path.c_str(),"User ",file);++index;
    }
    state.replayCount=count;state.unk0xc284=0;
}
bool TitleMenus::open_replay(const char* path){
    close_replay();auto file=actions.read_replay(path);ReplayFile replay;
    if(!replay.decode(file.data(),file.size()))return false;
    std::memcpy(&selected_metadata,replay.decoded().data(),sizeof(selected_metadata));
    if(selected_metadata.shot_type>=12||selected_metadata.difficulty>=5||selected_metadata.spell_number>=222)return false;
    // THGuiRep::State(2): inspect the opened replay's PRAC parameters into the
    // candidate block without mutating the live practice run.
    if(practice)practice_replay_menu_check(*practice,file.data(),u32(file.size()));
    selected_replay=replay.decoded();state.currentReplay=&selected_metadata;return true;
}
void TitleMenus::close_replay(){state.currentReplay=nullptr;selected_replay.clear();}
i32 TitleMenus::corrupt_replay(){actions.replay_error();context.supervisor_state=-1;return 0;}
bool TitleMenus::replay_stage(i32 stage,ReplayStage& out)const{
    if(!state.currentReplay||stage<0||stage>=9)return false;const u32 offset=selected_metadata.header.stage_offsets[stage];
    if(!offset||offset>selected_replay.size()||selected_replay.size()-offset<sizeof(out))return false;
    std::memcpy(&out,selected_replay.data()+offset,sizeof(out));return true;
}
i32 TitleMenus::OnUpdateReplayMenu(){
    const auto pressed=[&](u16 mask){return (context.input.current&mask)&&((context.input.current&mask)!=(context.input.previous&mask));};
    const auto repeat=[&](u16 mask){return pressed(mask)||((context.input.current&mask)&&context.input.scrolling);};
    const auto selected_valid=[&]{return state.selectedReplay>=0&&state.selectedReplay<state.replayCount&&state.selectedReplay<60;};
    const auto stage_present=[&](i32 stage){return selected_valid()&&stage>=0&&stage<9&&state.replays[state.selectedReplay].header.stage_offsets[stage];};
    switch(state.currentScreenState){
    case 0:
        if(state.stateTimer2==0){
            // th08_rep_menu_1 / THGuiRep::State(1).
            if(practice)practice_replay_menu_reset(*practice);
            if(state.previousScreen!=TitleCurrentScreen_Replay&&actions.load_surface(0,"title/select00.png")!=0)return 0;
            SetInterruptArray(state.vms,state.vmCount,14);state.cursor=0;state.currentScreenState=0;state.stateTimer=0;state.currentHelpTextVm=nullptr;scan_replays();
        }
        if(state.stateTimer2>=8){state.currentScreenState=1;state.stateTimer=0;}break;
    case 1:
        MoveCursorVertical(state.replayCount);
        if(state.replayCount>15){
            if(repeat(64)){state.cursor-=15;if(state.cursor<0)state.cursor+=state.replayCount;actions.sound(12,0);}
            if(repeat(128)){state.cursor+=15;if(state.cursor>=state.replayCount)state.cursor-=state.replayCount;actions.sound(12,0);}
        }
        state.selectedReplay=state.cursor;if(state.stateTimer<10)break;
        if(pressed(4097)){
            if(!state.replayCount)break;if(!selected_valid())return corrupt_replay();
            actions.sound(10,0);state.currentScreenState=2;SetInterruptArray(state.vms,state.vmCount,15);state.vms[state.selectedReplay%15+80].SetInterrupt(17);
            if(!open_replay(state.replayFilePaths[state.selectedReplay]))return corrupt_replay();
            state.cursor=0;while(!stage_present(state.cursor)){++state.cursor;if(state.cursor>8)return corrupt_replay();}
            InitializeAndSetSprite(state.resultTextAnm,state.spellCardNameVms,11);
            auto& name=state.spellCardNameVms[0];name.pos={};name.anchor=3;name.fontWidth=name.fontHeight=15;
            char text[49]{};std::memcpy(text,state.replays[state.selectedReplay].spell_name,48);
            // spell_name in the replay file stays the recorded Japanese; the
            // display point looks up spells.etl (upstream spell_name#replay).
            const i32 spellNumber=state.replays[state.selectedReplay].spell_number;
            DrawTextLeft(&name,0xffffff,0,spellNumber>=0?Localization::SpellName(u32(spellNumber),text):text);name.color1.d3dColor=COLOR_WHITE;break;
        }
        if(pressed(10)){actions.sound(11,0);state.currentScreenState=4;state.stateTimer=0;SetInterruptArray(state.vms,state.vmCount,16);}break;
    case 2:{
        if(!state.currentReplay||!selected_valid())return corrupt_replay();
        const auto movement=MoveCursorVertical(9);i32 guard=0;
        if(movement<0)while(!stage_present(state.cursor)){--state.cursor;if(state.cursor<0)state.cursor=9;if(++guard>10)return corrupt_replay();}
        else if(movement>0)while(!stage_present(state.cursor)){++state.cursor;if(state.cursor>=9)state.cursor=0;if(++guard>9)return corrupt_replay();}
        state.selectedReplayStage=state.cursor;
        if(pressed(4097)){
            SetInterruptArray(state.vms,state.vmCount,19);state.vms[state.selectedReplay%15+80].SetInterrupt(17);state.currentScreenState=3;state.cursor=0;
            state.vms[108].pendingInterrupt=state.vms[109].pendingInterrupt=21;
            if(state.currentReplay->spell_number<0)state.vms[110].pendingInterrupt=21;else state.vms[110].color1.a=0;
            state.vms[state.cursor+108].pendingInterrupt=20;break;
        }
        if(pressed(10)){close_replay();state.currentScreenState=1;state.stateTimer2=0;SetInterruptArray(state.vms,state.vmCount,14);state.cursor=state.selectedReplay;}break;
    }
    case 3:
        if(!state.currentReplay||!selected_valid())return corrupt_replay();
        if(MoveCursorVertical(state.currentReplay->spell_number<0?3:2)){
            state.vms[108].pendingInterrupt=state.vms[109].pendingInterrupt=21;
            if(state.currentReplay->spell_number<0)state.vms[110].pendingInterrupt=21;else state.vms[110].color1.a=0;
            state.vms[state.cursor+108].pendingInterrupt=20;
        }
        if(pressed(4097)){
            // th08_rep_menu_3 / THGuiRep::State(3).
            if(practice)practice_replay_menu_activate(*practice);
            context.SetIsReplayWeird(true);std::snprintf(context.replayFilename,sizeof(context.replayFilename),"%s",state.replayFilePaths[state.selectedReplay]);
            context.difficulty=state.currentReplay->difficulty;context.character=state.currentReplay->shot_type;context.flags.isSpellPractice=state.currentReplay->spell_number>=0;context.currentSpellCardNumber=state.currentReplay->spell_number;
            close_replay();context.currentStage=state.selectedReplayStage;context.supervisor_state=2;context.replayMode=state.cursor;actions.stop_audio();return 0;
        }
        if(pressed(10)){state.currentScreenState=2;state.stateTimer2=0;state.cursor=state.selectedReplayStage;SetInterruptArray(state.vms,state.vmCount,15);state.vms[state.selectedReplay%15+80].SetInterrupt(17);}break;
    case 4:
        if(state.stateTimer>=30){ChangeCurrentScreen(TitleCurrentScreen_StartMenu);state.cursor=4;return 1;}break;
    }
    state.idleFrames=wrapping_add(state.idleFrames,1);state.stateTimer=wrapping_add(state.stateTimer,1);state.stateTimer2=wrapping_add(state.stateTimer2,1);return 1;
}
}
