import test from 'node:test';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {runNativeCpp} from '../../launcher/tests/support/native-cpp.mjs';

test('production title-state header preserves menu ids and confirmation boundaries',async()=>{
  const result=await runNativeCpp(`#include "TitleMenuState.hpp"
#include <cassert>
#include <iostream>
int main(){
  using namespace th08;
  static_assert(TitleCurrentScreen_StartMenu==0 && TitleCurrentScreen_Option==2 &&
    TitleCurrentScreen_KeyConfig==3 && TitleCurrentScreen_DifficultySelect==4 &&
    TitleCurrentScreen_CharacterSelect==5 && TitleCurrentScreen_ShotSelect==6 &&
    TitleCurrentScreen_Replay==7 && TitleCurrentScreen_DifficultySelectPractice==8 &&
    TitleCurrentScreen_CharacterSelectPractice==9 && TitleCurrentScreen_ShotSelectPractice==10 &&
    TitleCurrentScreen_PracticeStageSelect==11 && TitleCurrentScreen_DifficultySelectExtra==12 &&
    TitleCurrentScreen_CharacterSelectExtra==13 && TitleCurrentScreen_ShotSelectExtra==14 &&
    TitleCurrentScreen_CharacterSelectSpell==15 && TitleCurrentScreen_SpellStageSelect==16 &&
    TitleCurrentScreen_SpellCardSelect==17);
  static_assert(TitleScreenState_Ready==0 && TitleScreenState_Loading==1 && TitleScreenState_Close==2);
  static_assert(TitleCurrentScreenState_Init==0 && TitleCurrentScreenState_Ready==1 &&
    TitleCurrentScreenState_Exit==2 && TitleCurrentScreenState_Changing==3);
  assert(!title_confirmation_ready(TitleCurrentScreen_StartMenu,TitleCurrentScreenState_Ready,9));
  assert(title_confirmation_ready(TitleCurrentScreen_StartMenu,TitleCurrentScreenState_Ready,10));
  assert(!title_confirmation_ready(TitleCurrentScreen_StartMenu,TitleCurrentScreenState_Init,10));
  assert(title_confirmation_ready(TitleCurrentScreen_DifficultySelect,TitleCurrentScreenState_Ready,0));
  assert(!title_confirmation_ready(TitleCurrentScreen_Option,TitleCurrentScreenState_Ready,10));
  assert(!title_confirmation_ready(99,TitleCurrentScreenState_Ready,10));
  std::cout<<"native confirmation guard boundaries checked";
}`,{includes:[resolve(import.meta.dirname,'../../ports/th08/web/cpp/game')]});
  assert.match(result,/confirmation guard boundaries checked/);
});
