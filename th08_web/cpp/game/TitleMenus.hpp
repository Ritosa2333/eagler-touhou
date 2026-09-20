// Source-level TH08 title menu recovery. GensokyoClub/th08 reference: MIT.
#pragma once
#include "PresentationVisual.hpp"
#include "AnmText.hpp"
#include "AnmExecutor.hpp"
#include "InputController.hpp"
#include "ReplayFile.hpp"
#include "ScoreFile.hpp"
#include "Chain.hpp"
#include "SpellProgress.hpp"
#include "PracticeConfig.hpp"
#include <string>
namespace th08 {
enum TitleCurrentScreen
{
    TitleCurrentScreen_StartMenu = 0,
    TitleCurrentScreen_Option = 2,
    TitleCurrentScreen_KeyConfig = 3,
    TitleCurrentScreen_DifficultySelect = 4,
    TitleCurrentScreen_CharacterSelect = 5,
    TitleCurrentScreen_ShotSelect = 6, /* Leftover from PCB */
    TitleCurrentScreen_Replay = 7,
    TitleCurrentScreen_DifficultySelectPractice = 8,
    TitleCurrentScreen_CharacterSelectPractice = 9,
    TitleCurrentScreen_ShotSelectPractice = 10, /* Leftover from PCB */
    TitleCurrentScreen_PracticeStageSelect = 11,
    TitleCurrentScreen_DifficultySelectExtra = 12,
    TitleCurrentScreen_CharacterSelectExtra = 13,
    TitleCurrentScreen_ShotSelectExtra = 14,      /* Leftover from PCB */
    TitleCurrentScreen_CharacterSelectSpell = 15, /* Seems to be an earlier feature, the current scene
                                                   * is never set to this value, but it checks against
                                                   * this value.
                                                   */
    TitleCurrentScreen_SpellStageSelect = 16,
    TitleCurrentScreen_SpellCardSelect = 17,
};

enum TitleScreenState
{
    TitleScreenState_Ready = 0,
    TitleScreenState_Loading = 1,
    TitleScreenState_Close = 2,
};

enum TitleCurrentScreenState
{
    TitleCurrentScreenState_Init = 0,
    TitleCurrentScreenState_Ready = 1,
    TitleCurrentScreenState_Exit = 2,
    TitleCurrentScreenState_Changing = 3,
};


struct TitleControllerMapping {
    i16 shotButton,bombButton,focusButton,menuButton,upButton,downButton,leftButton,rightButton,skipButton;
};
static_assert(sizeof(TitleControllerMapping)==18);
struct TitleState {
    i32 cursor;
    i32 currentPageSpellCardSelect;
    i32 cursor2;
    i32 currentScreenState;
    i32 stateTimer;
    i32 unk0x14;
    u8 padding[80];

    TitleCurrentScreen previousScreen;
    u32 practiceState;

    char replayFilePaths[60][512];
    char replayNumbers[60][8];
    ReplayMetadata replays[60];
    ReplayMetadata *currentReplay;
    i32 unk0xc284;
    i32 replayCount;
    i32 selectedReplay;
    i32 selectedReplayStage;
    i32 idleFrames;
    i32 currentNumberOfSpellCards;
    i32 unk0xc29c;
    float percentageCapturedSpellPracticePerShot;
    float percentageCapturedInGamePerShot;
    float percentageCapturedSpellPractice;
    float percentageCapturedInGame;

    AnmLoaded *titleAnm;
    AnmLoaded *resultTextAnm;
    AnmVm *vms;
    AnmVm *currentHelpTextVm;
    AnmVm helpTextVms[14];
    AnmVm spellCardNameVms[21];
    AnmVm spellCardInfoVms[14];
    i32 vmCount;

    TitleCurrentScreen currentScreen;
    i32 stateTimer2;
    i32 startMenuIdleFrames;
    TitleScreenState state;
    ChainElement *calcChain;
    ChainElement *drawChain;
    TitleControllerMapping controllerMapping;
    GameConfiguration currentGameConfig;

    TitleState(){reset();}
    void reset(){std::memset(this,0,sizeof(*this));}
};
static_assert(sizeof(void*)!=4||offsetof(TitleState,titleAnm)==0xc2b0);
struct TitleGameFlags {
    u32 isPracticeMode:1,isDemoMode:1,isActive:1,isReplay:1,unk4:1,gameState:2,unk7:2,unk9:1,unk10:1,isGoingToFinalB:2,unk13:1,isSpellPractice:1,isExtraUnlocked:1,isSpellPracticeUnlocked:1,isExtraUnlockedWithAllTeams:1,reserved:14;
};
struct TitleContext {
    TitleGameFlags flags{};
    i32 supervisor_state=1,supervisor_previous=0,difficulty=1,character=0,shotType=0,currentStage=0,replayMode=0,demoFrameCount=0;
    u8 currentDemoReplay=0;u8 padding25=0;i16 lastKeyChanged=32;
    i32 bgmVolume=100,sfxVolume=100;
    InputFrame input;u8 controller_state[128]{};
    char replayFilename[512]{};
    ClearRecord clears[13];PlayRecord play;
    SpellRecord spells[222];LastWords last_words;AnmLoaded* textAnm=nullptr;
    i32 currentSpellCardNumber=0;
    PracticeRecord practice_scores[12];bool software_texturing=false;
    bool IsReplay()const{return flags.isReplay;}
    void SetIsReplayWeird(bool value){flags.isReplay=value;}
    bool IsPracticeMode()const{return flags.isPracticeMode;}
    bool IsSpellPractice()const{return flags.isSpellPractice;}
    bool IsPhantasmUnlocked()const{return false;}
    bool IsExtraUnlockedForCharacter(i32 character)const;
    bool IsSpellPracticeUnlockedForCharacter(i32 character)const;
    bool IsExtraUnlocked()const;
    bool IsSpellPracticeUnlocked()const;
    bool IsExtraUnlockedWithAllTeams()const;
    bool HasSpellCardBeenEncountered(i32 number,i32 shot)const{return number>=0&&number<222&&shot>=0&&shot<=12&&(spells[number].game.attempts[shot]||spells[number].practice.attempts[shot]);}
    bool IsLastWordSpellCardAttempted(i32 number)const{return number<205?HasSpellCardBeenEncountered(number,12):number<222&&last_words.unlocked[number-205]==number;}
};
struct TitleActions {
    virtual ~TitleActions()=default;
    virtual i32 load_surface(i32 slot,const char* path)=0;
    virtual void sound(i32 index,i32 pan)=0;
    virtual void process_sounds()=0;
    virtual void play_music(i32 track,i32 mode)=0;
    virtual void load_music(i32 track,const char* path)=0;
    virtual void stop_audio()=0;
    virtual void midi_reset()=0;
    virtual void apply_volume()=0;
    virtual std::vector<u8> read_replay(const char* path)=0;
    virtual std::vector<std::string> list_user_replays()=0;
    virtual void replay_error()=0;
    virtual void background()=0;
};
class TitleMenus {
    friend class TitleView;
    friend class TitleFlow;
public:
    TitleState state;
    // Owned by GameplaySession; lets the replay menu drive the THGuiRep
    // State(1/2/3) practice-parameter lifecycle.
    PracticeState* practice=nullptr;
    TitleMenus(TitleContext& context,GameConfiguration& config,AnmExecutor& executor,TextWriter& text,TitleActions& actions)
      :context(context),config(config),executor(executor),text(text),actions(actions){}
    ~TitleMenus();
    void initialize_vms(AnmLoaded& file);
    void initialize_help(AnmLoaded& file);
    i32 OnUpdateStartMenu();
    i32 OnUpdateOptions();
    i32 OnUpdateKeyConfig();
    i32 OnUpdateDifficultySelect();
    i32 OnUpdateCharacterSelect();
    i32 OnUpdatePracticeStageSelect();
    i32 OnUpdateSpellStageSelect();
    i32 OnUpdateSpellCardSelect();
    i32 OnUpdateReplayMenu();
    void UnlockLastWordSpellCards();
    void FormatSpellCardInfo();
    i32 MoveCursorVertical(i32 length);
    i32 MoveCursorHorizontal(i32 length);
    void ChangeCurrentScreen(TitleCurrentScreen screen);
    i32 SetKeyNumberSprite(AnmVm* vms,i16 key);
    void SetKeyConfigKey(i16 old_key,i16 new_key,i32 unused);
    void execute_animations();
    JobResult update();
    void release();
    const std::vector<u8>& current_replay_bytes()const{return selected_replay;}
    bool replay_stage(i32 stage,ReplayStage& out)const;
private:
    TitleContext& context;GameConfiguration& config;AnmExecutor& executor;TextWriter& text;TitleActions& actions;
    struct PresentationVm {Vec3 pos{},pos2{};i16 script=-1;presentation::VisualSample visual;};
    std::vector<PresentationVm> presentation_previous;AnmVm* presentation_help_vm=nullptr;PresentationVm presentation_help{};bool presentation_valid=false;
    void snapshot_presentation();
    void SetInterruptArray(AnmVm* vms,i32 count,i16 value);
    void ExecuteAnmIdxArray(AnmVm* vms,i32 first,i32 count);
    void DrawTextCentered(AnmVm* vm,u32 color,u32 outline,const char* message);
    void DrawTextLeft(AnmVm* vm,u32 color,u32 outline,const char* message);
    void DrawTextFormatted(AnmVm* vm,const char* format,...);
    void InitializeAndSetSprite(AnmLoaded* file,AnmVm* vm,i32 index);
    bool start_demo();
    ReplayMetadata selected_metadata;
    std::vector<u8> selected_replay;
    void scan_replays();
    bool open_replay(const char* path);
    void close_replay();
    i32 corrupt_replay();
};
}
