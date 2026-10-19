#include "ResultScreen.hpp"
#include "ReplayText.hpp"
namespace th08 {
namespace {
constexpr char alphabet[]="ABCDEFGHIJKLMNOP" "QRSTUVWXYZ.,:;_@" "abcdefghijklmnop" "qrstuvwxyz+-/*=%" "0123456789#!?'\"$" "(){}[]<>&\\|~^ --";
static_assert(sizeof(alphabet)==97);
void copy_name(char* destination,const char* source){u32 size=0;while(size<8&&source[size]){destination[size]=source[size];size++;}destination[size]=0;}
bool valid_replay(const ReplayMetadata& replay){return replay.header.magic==fourcc('T','8','R','P')&&(replay.header.version&0xfff)==6;}
}
void ResultScreen::interrupt_all(i16 value){for(auto& vm:state.spriteVms)vm.pendingInterrupt=value;}
void ResultScreen::keyboard_navigation(){
    auto& key=state.selectedCharacter;
    if(scrolling(Up)){do{key-=16;if(key<0)key+=96;}while(alphabet[key]==' ');actions.sound(12,0);}
    if(scrolling(Down)){do{key+=16;if(key>=96)key-=96;}while(alphabet[key]==' ');actions.sound(12,0);}
    if(scrolling(Left)){do{--key;if(key%16==15)key+=16;if(key<0)key=15;}while(alphabet[key]==' ');actions.sound(12,0);}
    if(scrolling(Right)){do{++key;if(key%16==0)key-=16;}while(alphabet[key]==' ');actions.sound(12,0);}
}
void ResultScreen::sync_score(){if(pending_rank>=0&&u32(state.selectedDifficulty)<5&&u32(state.selectedHighScoreCharacter)<12){auto& list=scores.list(state.selectedDifficulty,state.selectedHighScoreCharacter);if(u32(pending_rank)<list.size())list[pending_rank]=state.hscr;}}
i32 ResultScreen::HandleResultKeyboard(){
    auto& s=state;auto finish=[&](){s.currentState=RESULT_SCREEN_STATE_STATS_SCREEN;s.frameTimer=0;interrupt_all(2);copy_name(s.lastName,s.hscr.name);copy_name(session.last_name.name,s.lastName);sync_score();return 0;};
    if(context.slow_mode||context.speedhack){s.currentState=RESULT_SCREEN_STATE_STATS_SCREEN;s.frameTimer=0;std::memcpy(session.records,session.previous_records,sizeof(session.records));return 0;}
    if(s.frameTimer==0){
        if(u32(context.character)>=12||u32(context.difficulty)>=5)return 0;
        s.selectedHighScoreCharacter=context.character;s.selectedDifficulty=context.difficulty;interrupt_all(context.difficulty+3);
        text.draw(s.textVms[0],TextAlignment::Center,0xffffff,0,replay_text::characters[context.character]);s.textVms[0].color1.a=255;
        session.history.play_frames=context.play_frames;session.history.humanity=(Extended::from_int(context.human_frames)/Extended::from_int(context.active_frames)*number(10000)).truncate_int();
        s.hscr=session.history;s.hscr.score=session.numbers.score;s.hscr.retries=session.numbers.retries;s.hscr.header.version=4;s.hscr.header.magic=fourcc('H','S','C','R');s.hscr.stage=context.flags&16?99:context.stage;s.hscr.header.unknown=1;
        copy_name(s.hscr.name,session.last_name.name);actions.format_date(s.hscr.date);
        const auto fraction=number(context.rendered_frames)/number(context.total_frames)-number(.5f);float rate=(fraction+fraction).to_float();if(rate<0)rate=0;else if(rate>=1)rate=1;s.hscr.lag=context.cheat_movement_used?100.f:((number(1)-number(rate))*number(100)).to_float();
        pending_rank=scores.link(s.hscr,s.selectedDifficulty,s.selectedHighScoreCharacter);if(pending_rank>=10)return finish();
        s.cursor=0;if(s.lastNameSavedInScore)s.selectedCharacter=95;s.lastName[0]=0;
    }
    if(s.frameTimer<10)return 0;keyboard_navigation();
    if(scrolling(Shoot|Enter)){
        const i32 index=s.cursor>=8?7:s.cursor;
        if(s.selectedCharacter<94)s.hscr.name[index]=alphabet[s.selectedCharacter];else if(s.selectedCharacter==94)s.hscr.name[index]=' ';else{actions.sound(11,0);return finish();}
        if(s.cursor<8&&++s.cursor==8)s.selectedCharacter=95;actions.sound(10,0);
    }
    if(scrolling(Menu|Bomb)){const i32 index=s.cursor>=8?7:s.cursor;if(s.cursor>0){--s.cursor;s.hscr.name[index]=s.hscr.name[s.cursor]=' ';}actions.sound(11,0);}
    if(context.input.pressed(Menu)){actions.sound(11,0);return finish();}sync_score();return 0;
}
void ResultScreen::choose_replay(){actions.sound(10,0);actions.process_sounds();state.currentState=RESULT_SCREEN_STATE_CHOOSING_REPLAY_FILE;interrupt_all(12);state.frameTimer=0;for(i32 i=0;i<15;i++){const auto data=actions.read_replay(i);ReplayFile file;if(file.decode(data.data(),data.size()))std::memcpy(&state.replays[i],file.decoded().data(),sizeof(ReplayMetadata));}}
void ResultScreen::exit_replay(){state.frameTimer=0;actions.sound(11,0);state.currentState=RESULT_SCREEN_STATE_EXITING;interrupt_all(2);}
i32 ResultScreen::HandleReplaySaveKeyboard(){
    auto& s=state;auto color_choices=[&](){auto& a=s.spriteVms[50].color1.d3dColor;auto& b=s.spriteVms[51].color1.d3dColor;a=(a&0xff000000)|(s.cursor?0x606060:0xff6060);b=(b&0xff000000)|(s.cursor?0xff6060:0x606060);};
    switch(s.currentState){
    case RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION:
        if(s.frameTimer==10){const i16 value=context.slow_mode||context.speedhack?19:session.numbers.retries?14:11;
            // Original starts four entries into a 72-VM array, so the last four
            // interrupts target the adjacent text VMs. Express this without UB.
            for(i32 i=4;i<76;i++)(i<72?s.spriteVms[i]:s.textVms[i-72]).pendingInterrupt=value;
            if(value!=11)s.currentState=RESULT_SCREEN_STATE_CANT_SAVE_REPLAY;s.cursor=0;
        }
        color_choices();if(s.frameTimer<12)return 0;move_horizontal(2);
        if(context.input.pressed(Menu|Bomb)||context.input.pressed(Menu)){exit_replay();break;}
        if(context.input.pressed(Shoot|Enter)){if(s.cursor==0)choose_replay();else exit_replay();}break;
    case RESULT_SCREEN_STATE_CANT_SAVE_REPLAY:
        if(s.frameTimer<20)return 0;if(context.input.pressed(Shoot|Enter)||context.input.pressed(Menu|Bomb))exit_replay();break;
    case RESULT_SCREEN_STATE_CHOOSING_REPLAY_FILE:
        if(s.frameTimer==0)for(i32 i=0;i<15;i++){const auto data=actions.read_replay(i);ReplayFile file;if(file.decode(data.data(),data.size()))std::memcpy(&s.replays[i],file.decoded().data(),sizeof(ReplayMetadata));}
        if(s.frameTimer<20)return 0;move_cursor(15);s.selectedReplay=s.cursor;
        if(context.input.pressed(Shoot|Enter)){
            actions.sound(10,0);s.selectedReplay=s.cursor;s.frameTimer=0;actions.format_date(s.currentReplay.date);s.currentReplay.spell_score=session.numbers.score;
            const bool occupied=valid_replay(s.replays[s.cursor]);interrupt_all(occupied?13:17);s.spriteVms[56+s.selectedReplay].pendingInterrupt=16;s.currentState=occupied?RESULT_SCREEN_STATE_OVERWRITE_REPLAY_FILE:RESULT_SCREEN_STATE_WRITING_REPLAY_NAME;
            s.cursor=0;s.selectedCharacter=s.lastNameSavedInScore?95:0;
        }
        if(context.input.pressed(Menu|Bomb)){actions.sound(11,0);s.currentState=RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION;interrupt_all(2);s.frameTimer=0;}break;
    case RESULT_SCREEN_STATE_WRITING_REPLAY_NAME:
        if(s.frameTimer<30)return 0;keyboard_navigation();
        if(scrolling(Shoot|Enter)){
            actions.sound(10,0);actions.process_sounds();const i32 index=s.cursor>=8?7:s.cursor;
            if(s.selectedCharacter<94)s.lastName[index]=alphabet[s.selectedCharacter];else if(s.selectedCharacter==94)s.lastName[index]=' ';
            else{actions.save_replay(s.selectedReplay,s.lastName);s.frameTimer=0;s.currentState=RESULT_SCREEN_STATE_EXITING;interrupt_all(2);copy_name(session.last_name.name,s.lastName);}
            if(s.cursor<8&&++s.cursor==8)s.selectedCharacter=95;
        }
        if(scrolling(Menu|Bomb)){const i32 index=s.cursor>=8?7:s.cursor;if(s.cursor>0){--s.cursor;s.lastName[index]=s.lastName[s.cursor]=' ';}actions.sound(11,0);}
        if(context.input.pressed(Menu))choose_replay();break;
    case RESULT_SCREEN_STATE_OVERWRITE_REPLAY_FILE:
        color_choices();if(s.frameTimer<20)return 0;move_horizontal(2);
        if(context.input.pressed(Menu|Bomb)||context.input.pressed(Menu)){choose_replay();break;}
        if(context.input.pressed(Shoot|Enter)){s.frameTimer=0;if(s.cursor==0){interrupt_all(17);s.spriteVms[56+s.selectedReplay].pendingInterrupt=16;s.currentState=RESULT_SCREEN_STATE_WRITING_REPLAY_NAME;}else choose_replay();}break;
    default:break;
    }return 0;
}
}
