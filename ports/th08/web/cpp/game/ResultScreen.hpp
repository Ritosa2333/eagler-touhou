// Result UI state and services. Original layout names follow the MIT TH08 reference.
#pragma once
#include "ResultEnums.hpp"
#include "ScoreStore.hpp"
#include "ReplayFile.hpp"
#include "AnmLibrary.hpp"
#include "AnmText.hpp"
#include "InputController.hpp"
#include "Chain.hpp"
namespace th08 {
struct ResultState {
    ScoreHeader* scoreDat=nullptr;i32 frameTimer=0;ResultScreenState currentState=RESULT_SCREEN_STATE_INIT;
    ResultScreenState currentState2=RESULT_SCREEN_STATE_INIT;i32 menuDepth=0;ResultScreenState previousState=RESULT_SCREEN_STATE_INIT;
    i32 frameTimer2=0,cursor=0,unk0x20=0,unk0x24=0,selectedReplay=0,selectedCharacter=0,shotTypeCursor=0,previousShotType=0,updateSpellcardResults=0;
    i32 selectedHighScoreCharacter=0,spellcardPage=0,selectedDifficulty=0,selectedSpellcardDifficulty=0,cheatCodeStep=0,lastNameSavedInScore=0,exitingSpellcardResults=0;
    char lastName[9]{};i32 capturedSpellCards[6][13]{};u8 totalSeconds=0;
    AnmVm spriteVms[72],textVms[30],unk_10ef8,listingDividerSprite;
    AnmLoaded *resultAnm=nullptr,*resultTextAnm=nullptr;u32 unk0x11448=0;
    HighScore hscr;ReplayMetadata replays[15],currentReplay;
    ResultState(){reset();}
    void reset(){std::memset(this,0,sizeof(*this));}
};
static_assert(sizeof(void*)!=4||offsetof(ResultState,spriteVms)==0x1a0);
static_assert(sizeof(void*)!=4||offsetof(ResultState,hscr)==0x1144c);
struct ResultContext {
    InputFrame input;i32 character=0,difficulty=0,stage=0;u32 flags=0;
    bool slow_mode=false,speedhack=false,cheat_movement_used=false;i32 play_frames=0,human_frames=0,active_frames=1;
    float rendered_frames=1,total_frames=1;
    i32 total_game_frames=0;bool software_texturing=false;i32 supervisor_state=5;
};
struct ResultActions {
    virtual ~ResultActions()=default;
    virtual void sound(i32 index,i32 pan)=0;
    virtual void process_sounds()=0;
    virtual void export_records()=0;
    virtual void format_date(char* date)=0;
    virtual std::vector<u8> read_replay(i32 slot)=0;
    virtual void save_replay(i32 slot,const char* name)=0;
    virtual u32 now_ms()=0;
    virtual void draw_background()=0;
    virtual void show_loading(const Vec3&)=0;
};
class ResultScreen {
    friend class ResultView;
    GameplaySession& session;ScoreStore& scores;AnmExecutor& animations;TextWriter& text;ResultActions& actions;
    i32 pending_rank=-1;
    void execute(AnmVm* vm){animations.execute(*vm);}
    i32 move(i32& cursor,i32 length,u16 negative,u16 positive);
    bool scrolling(u16 mask)const{return context.input.pressed(mask)||((context.input.current&mask)&&context.input.scrolling);}
    void keyboard_navigation();void interrupt_all(i16 value);void choose_replay();void exit_replay();void sync_score();
    bool text_left(AnmVm&,const char* format,...);
public:
    ResultState state;ResultContext context;
    ResultScreen(GameplaySession& s,ScoreStore& scores,AnmExecutor& a,TextWriter& t,ResultActions& p):session(s),scores(scores),animations(a),text(t),actions(p){}
    void set_state(ResultScreenState value){state.previousState=state.currentState;state.currentState=state.currentState2=value;state.menuDepth=state.frameTimer=state.frameTimer2=state.exitingSpellcardResults=0;}
    i32 move_cursor(i32 length){return move(state.cursor,length,Up,Down);}
    i32 move_horizontal(i32 length){return move(state.cursor,length,Left,Right);}
    i32 move_shot(i32 length){return move(state.shotTypeCursor,length,Up,Down);}
    i32 HandleCategorySelectScreen();i32 HandleHighScoreDifficultySelect();i32 HandleHighScoreCharacterSelect();i32 HandleHighScoreScreen();
    i32 HandleSpellCardDifficultySelect();i32 HandleSpellCardCharacterSelect();i32 CheckConfirmButton();
    i32 HandleResultKeyboard();i32 HandleReplaySaveKeyboard();
    i32 HandleSpellCardScreen();i32 HandleOtherStatsScreen();
    bool initialize(AnmLibrary&,const u8* score,u32 size);
    JobResult update();
};
}
