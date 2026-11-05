#include "ResultScreen.hpp"
#include "SpellCatalogData.hpp"
namespace th08 {
bool ResultScreen::initialize(AnmLibrary& library,const u8* bytes,u32 size){
    auto& s=state;pending_rank=-1;
    if(s.currentState!=RESULT_SCREEN_STATE_SCORE_SAVE){
        s.resultAnm=library.get(21);s.resultTextAnm=library.get(22);auto* text_file=library.get(0);if(!s.resultAnm||!s.resultTextAnm||!text_file)return false;
        for(i32 i=0;i<72;i++){auto& vm=s.spriteVms[i];vm.pos=vm.pos2={};if(!library.start(21,i,vm,animations,false))return false;}
        auto sprite=[](AnmLoaded* file,AnmVm& vm,i32 index){vm.Initialize();vm.anmFile=file;return file->SetSprite(&vm,index)>=0;};
        if(!sprite(s.resultAnm,s.listingDividerSprite,32))return false;
        for(i32 i=0;i<28;i++){auto& vm=s.textVms[i];if(!sprite(i<14?text_file:s.resultTextAnm,vm,i<14?i+21:i-12))return false;vm.pos={};vm.anchor=3;vm.fontWidth=vm.fontHeight=15;}
    }
    s.unk0x20=0;const bool history=s.currentState!=RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME&&s.currentState!=RESULT_SCREEN_STATE_PRACTICE&&s.currentState!=RESULT_SCREEN_STATE_SPELL_PRACTICE&&s.currentState!=RESULT_SCREEN_STATE_SCORE_SAVE;
    scores.load(bytes,size,history);s.scoreDat=&scores.header;s.lastNameSavedInScore=scores.last_name_saved;
    if(s.currentState==RESULT_SCREEN_STATE_PRACTICE){
        if(u32(context.character)>=12||u32(context.stage)>=9||u32(context.difficulty)>=5)return false;
        auto& high=session.practices[context.character].high_scores[context.stage][context.difficulty];if(u32(high)<session.numbers.score)high=i32(session.numbers.score);
    }
    if(s.currentState==RESULT_SCREEN_STATE_PRACTICE||s.currentState==RESULT_SCREEN_STATE_SPELL_PRACTICE){
        s.currentState=RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION;std::memcpy(s.lastName,session.last_name.name,9);
        if(context.slow_mode||context.speedhack)std::memcpy(session.records,session.previous_records,sizeof(session.records));
    }
    for(i32 difficulty=0;difficulty<6;difficulty++)for(i32 shot=0;shot<13;shot++){
        i32 count=0;for(i32 index=0;index<spell_difficulty_counts[difficulty];index++){const auto& record=session.records[spells_by_difficulty[difficulty][index]];if(record.header.magic==fourcc('C','A','T','K')&&record.header.version==3&&(record.game.captures[shot]||record.practice.captures[shot]))++count;}s.capturedSpellCards[difficulty][shot]=count;
    }
    s.selectedSpellcardDifficulty=5;s.shotTypeCursor=s.previousShotType=12;s.updateSpellcardResults=0;s.unk_10ef8.activeSpriteIndex=-1;
    auto& all=session.statistics.counts[6];all.continues=all.clears=0;for(i32 i=0;i<5;i++){all.continues+=session.statistics.counts[i].continues;all.clears+=session.statistics.counts[i].clears;}
    if(s.currentState==RESULT_SCREEN_STATE_SCORE_SAVE){if(context.slow_mode||context.speedhack){ScoreFile file;file.decode(bytes,size);file.spells(session.records);}return true;}
    s.selectedDifficulty=1;s.selectedHighScoreCharacter=0;return true;
}
JobResult ResultScreen::update(){
    switch(state.currentState){
    case RESULT_SCREEN_STATE_PRACTICE:case RESULT_SCREEN_STATE_SPELL_PRACTICE:context.supervisor_state=1;return JobResult::Remove;
    case RESULT_SCREEN_STATE_SCORE_SAVE:return JobResult::Remove;
    case RESULT_SCREEN_STATE_INIT:set_state(RESULT_SCREEN_STATE_CHOOSING_CATEGORY);[[fallthrough]];
    case RESULT_SCREEN_STATE_CHOOSING_CATEGORY:HandleCategorySelectScreen();break;
    case RESULT_SCREEN_STATE_EXITING:
        if(state.frameTimer==1){actions.show_loading({500,440,0});interrupt_all(2);}if(state.frameTimer>=20){context.supervisor_state=1;return JobResult::Remove;}break;
    case RESULT_SCREEN_STATE_BEST_SCORES_CHOOSING_CHARACTER:HandleHighScoreCharacterSelect();break;
    case RESULT_SCREEN_STATE_BEST_SCORES_CHOOSING_DIFFICULTY:HandleHighScoreDifficultySelect();break;
    case RESULT_SCREEN_STATE_BEST_SCORES:HandleHighScoreScreen();break;
    case RESULT_SCREEN_STATE_SPELLCARDS_CHOOSING_CHARACTER:HandleSpellCardCharacterSelect();break;
    case RESULT_SCREEN_STATE_SPELLCARDS_CHOOSING_DIFFICULTY:HandleSpellCardDifficultySelect();break;
    case RESULT_SCREEN_STATE_SPELLCARDS:HandleSpellCardScreen();break;
    case RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME:HandleResultKeyboard();break;
    case RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION:case RESULT_SCREEN_STATE_CANT_SAVE_REPLAY:case RESULT_SCREEN_STATE_CHOOSING_REPLAY_FILE:case RESULT_SCREEN_STATE_WRITING_REPLAY_NAME:case RESULT_SCREEN_STATE_OVERWRITE_REPLAY_FILE:HandleReplaySaveKeyboard();break;
    case RESULT_SCREEN_STATE_STATS_SCREEN:case RESULT_SCREEN_STATE_STATS_TO_SAVE_TRANSITION:CheckConfirmButton();break;
    case RESULT_SCREEN_STATE_OTHER_STATS_SCREEN_INIT:case RESULT_SCREEN_STATE_OTHER_STATS_SCREEN:case RESULT_SCREEN_STATE_OTHER_STATS_TO_INIT_TRANSITION:
        if(HandleOtherStatsScreen()){set_state(RESULT_SCREEN_STATE_CHOOSING_CATEGORY);HandleCategorySelectScreen();}break;
    default:break;
    }
    for(auto& vm:state.spriteVms)animations.execute(vm);state.frameTimer=wrapping_add(state.frameTimer,1);return animations.invalid?JobResult::Error:JobResult::Continue;
}
}
