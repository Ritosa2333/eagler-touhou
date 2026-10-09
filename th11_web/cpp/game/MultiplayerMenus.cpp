#ifdef TH11_MULTIPLAYER
#include "PauseMenu.hpp"
#include "TitleMenu.hpp"
namespace th11 {
bool PauseMenu::begin_multiplayer_end(bool cleared){
    multiplayer_result=true;completed=cleared;practice=false;replay=false;
    transition(28);elapsed.set(0,&animations.rate);sounds.clear();action=PauseAction::None;
    auto* backdrop=animations.create(text,75,0,29,true);
    if(!backdrop){error="MP result backdrop creation failed";return false;}
    background_animation=backdrop->id;capture_requested=true;
    auto* menu=animations.create(front,cleared?103:98,5,29,true);
    if(!menu){error="MP result menu creation failed";return false;}
    menu_animation=menu->id;cursor.count=1;cursor.select(0);
    // Keep the native headline, ornament and Return to Title animation.
    // These unavailable choices have no owner or input path in an MP result.
    for(auto* node=menu->child.next;node;){auto* next=node->next;auto& vm=*node->value;
        if(vm.script_index==94||vm.script_index==96||vm.script_index==97||vm.script_index==101||vm.script_index==102)animations.destroy(vm);
        node=next;
    }
    family(menu_animation,cleared?7:8);return true;
}
bool PauseMenu::update_multiplayer_end(u32 pressed){
    sounds.clear();action=PauseAction::None;scan_requested=save_requested=recording_metadata_requested=false;
    switch(state){
    case 28:if(timer.current>=10)transition(29);break;
    case 29:
        if(pressed&0x80001){sounds.push_back(10);choose(completed?100:95);transition(30);}break;
    case 30:
        if(timer.current>=12){family(background_animation,1);family(menu_animation,1);transition(31);}break;
    case 31:
        if(timer.current>=10){action=PauseAction::Title;transition(27);}break;
    case 27:break;
    default:error="invalid MP result state";return false;
    }
    timer.tick();elapsed.tick();return true;
}
void TitleMenu::multiplayer_results(u32 pressed){
    // Original post-Ending Result screen and its timed ANM transitions, with
    // session rows supplied directly instead of mutating the ordinary table.
    switch(substate){
    case 0:
        music_request=17;result_unranked=1;cursor.select(-1);
        if(!exists(92)){create(92);create(18,193,2);}create(102);
        create(selection.character+150);create(selection.character*3+selection.partner+152);create(selection.difficulty+158);
        step(1);break;
    case 1:if(timer.current>6)step(2);break;
    case 2:if(pressed&0x80001){sounds.push_back(10);step(3);}break;
    case 3:
        if(timer.current>=6){close(102);close(selection.character+150);close(selection.character*3+selection.partner+152);close(selection.difficulty+158);step(4);}break;
    case 4:if(timer.current>=10)multiplayer_result_done=true;break;
    default:error="invalid MP clear result state";break;
    }
}
}
#endif
