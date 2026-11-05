#include "CharacterSelectScene.hpp"
#include <cstdlib>
namespace th075 {
void CharacterSelectScene::choose_character(u8 side,GameSelection& config,SelectionSettings& settings,SelectionProgress& progress,ReplayRandom& random,SceneSignals& signals){

  i8 cVar1;
  int iVar2;
  
  if (double(0.0f) < double(players[side].portraitOffset)) {
    players[side].portraitOffset = double(players[side].portraitOffset) - double(5.0f);
  }
  if (double(0.0f) < double(players[side].firstCardX)) {
    players[side].firstCardX = double(players[side].firstCardX) - double(4.0f);
  }
  if (double(0.0f) < double(players[side].firstCardY)) {
    players[side].firstCardY = double(players[side].firstCardY) - double(2.0f);
  }
  if (double(0.0f) < double(players[side].secondCardX)) {
    players[side].secondCardX = double(players[side].secondCardX) - double(4.0f);
  }
  if (double(players[side].secondCardY) <= double(0.0f)) {
    players[side].cardsHidden = 0;
  }
  else {
    players[side].secondCardY = double(players[side].secondCardY) - double(2.0f);
  }
  if (double(0.0f) < double(players[side].cardTurn)) {
    players[side].cardTurn = double(players[side].cardTurn) - double(1.0f);
  }
  if (double(players[side].cardTurn) == double(60.0f)) {
    players[side].cardTurn = 0.0f;
  }
  iVar2 = std::abs(i32(inputs[side].counts[0]));
  if (((iVar2 == 1) ||
      (iVar2 = std::abs(i32(inputs[side].counts[0])), iVar2 % 0xf == 1)) ||
     ('\0' < i8(players[side].randomTicks))) {
    if (i8(players[side].randomTicks) == '\0') {
      signals.sound(0x27);
    }
    players[side].portraitOffset = 100.0f;
    players[side].portraitSlide = 100.0f;
    players[side].firstCardX = 80.0f;
    players[side].firstCardY = 40.0f;
    players[side].secondCardX = 80.0f;
    players[side].secondCardY = 40.0f;
    players[side].previousCharacter = players[side].character;
    if (inputs[side].counts[0] < 1) {
      players[side].character = i8(players[side].character) + -1;
      if (i8(players[side].character) < '\0') {
        players[side].character = 10;
      }
      if (u8(config.mode) == '\x03') {
        if ((i8(players[side].character) == '\n') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
        if ((i8(players[side].character) == '\t') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
        if ((i8(players[side].character) == '\b') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
        if ((i8(players[side].character) == '\a') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
        if ((i8(players[side].character) == '\x06') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
      }
      else {
        if ((i8(players[side].character) == '\n') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
        if ((i8(players[side].character) == '\t') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
        if ((i8(players[side].character) == '\b') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
        if ((i8(players[side].character) == '\a') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
        if ((i8(players[side].character) == '\x06') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character = i8(players[side].character) + -1;
        }
      }
    }
    else {
      players[side].character = i8(players[side].character) + '\x01';
      if (u8(config.mode) == '\x03') {
        if ((i8(players[side].character) == '\x06') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
        if ((i8(players[side].character) == '\a') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
        if ((i8(players[side].character) == '\b') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
        if ((i8(players[side].character) == '\t') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
        if ((i8(players[side].character) == '\n') &&
           (i8(storyUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
      }
      else {
        if ((i8(players[side].character) == '\x06') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
        if ((i8(players[side].character) == '\a') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
        if ((i8(players[side].character) == '\b') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
        if ((i8(players[side].character) == '\t') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
        if ((i8(players[side].character) == '\n') &&
           (i8(duelUnlocks[(int)i8(players[side].character)]) == '\0')) {
          players[side].character =
               i8(players[side].character) + '\x01';
        }
      }
      if ('\n' < i8(players[side].character)) {
        players[side].character = 0;
      }
    }
    progress.inspect(players[0].character,config.difficulty);
  }
  if (((inputs[side].counts[2] == 1) ||
      (inputs[side].counts[5] == 1)) ||
     (i8(players[side].randomTicks) == '\x01')) {
    signals.sound(0x2d);
    config.character[side] = players[side].character;
    config.alternatePalette[side] = inputs[side].counts[5] == 1;
    players[side].phase = 1;
    players[side].cardTurn = 0.0f;
    if (((u8(config.mode) != '\x04') && (i8(players[1-(side)].phase) != '\0')) &&
       ((config.character[0] == config.character[1] &&
        (config.alternatePalette[side] == config.alternatePalette[1+(-(int)side)])))) {
      config.alternatePalette[side] = '\x01' - config.alternatePalette[side];
    }
    if (u8(config.mode) == '\x03') {
      config.alternatePalette[0] = 0;
      config.alternatePalette[1] = 0;
    }
  }
  if (inputs[side].counts[4] == 1) {
    iVar2 = random.next();
    players[side].randomTicks = (i8)(iVar2 % 10) + '\x1e';
  }
  if ('\0' < i8(players[side].randomTicks)) {
    players[side].randomTicks = i8(players[side].randomTicks) + -1;
  }
  if (inputs[side].counts[6] == 1) {
    cVar1 = progress.stages();
    if (('\0' < cVar1) && (u8(config.mode) == '\x03')) {
      signals.sound(0x2d);
      players[side].panel =
           '\x01' - (i8(players[side].panel) != '\0');
    }
    if ((u8(config.mode) != '\x03') && (u8(config.mode) != '\x05')) {
      signals.sound(0x2d);
      players[side].panel =
           '\x01' - (i8(players[side].panel) != '\0');
    }
  }
  return;

}
void CharacterSelectScene::choose_first_spell(u8 side,GameSelection& config,SelectionSettings& settings,SelectionProgress& progress,ReplayRandom& random,SceneSignals& signals){

  int iVar1;
  
  if (double(0.0f) < double(players[side].portraitOffset)) {
    players[side].portraitOffset = double(players[side].portraitOffset) - double(5.0f);
  }
  if (double(players[side].firstCardX) < double(80.0f)) {
    players[side].firstCardX = double(players[side].firstCardX) + double(4.0f);
  }
  if (double(players[side].firstCardY) < double(40.0f)) {
    players[side].firstCardY = double(players[side].firstCardY) + double(2.0f);
  }
  if (double(players[side].secondCardX) < double(80.0f)) {
    players[side].secondCardX = double(players[side].secondCardX) + double(4.0f);
  }
  if (double(players[side].secondCardY) < double(40.0f)) {
    players[side].secondCardY = double(players[side].secondCardY) + double(2.0f);
  }
  if (double(15.0f) <= double(players[side].cardTurn)) {
    if ((double(15.0f) < double(players[side].cardTurn)) &&
       (players[side].cardTurn = double(players[side].cardTurn) - double(2.0f),
       double(players[side].cardTurn) < double(15.0f))) {
      players[side].cardTurn = 0.0f;
    }
  }
  else {
    players[side].cardTurn = double(players[side].cardTurn) + double(1.0f);
  }
  if (inputs[side].counts[1] == -1) {
    signals.sound(0x27);
    players[side].spell = 0;
  }
  else {
    iVar1 = std::abs(i32(inputs[side].counts[0]));
    if (iVar1 == 1) {
      signals.sound(0x27);
      players[side].spell = 1;
    }
    else if (inputs[side].counts[1] == 1) {
      signals.sound(0x27);
      players[side].spell = 2;
    }
  }
  if (inputs[side].counts[2] == 1) {
    config.firstSpell[side] = players[side].spell;
    players[side].phase = 2;
    signals.sound(0x2d);
  }
  if (inputs[side].counts[3] == 1) {
    signals.sound(0x28);
    config.resumeStory = 0;
    players[0].panelCursor = 0;
    players[side].phase = 0;
    players[side].firstCardX = 80.0f;
    players[side].firstCardY = 40.0f;
    players[side].secondCardX = 80.0f;
    players[side].secondCardY = 40.0f;
    players[side].cardsHidden = 1;
  }
  if (inputs[side].counts[5] == 1) {
    players[side].highlightAll =
         '\x01' - (i8(players[side].highlightAll) != '\0');
  }
  if (inputs[side].counts[4] == 1) {
    iVar1 = random.next();
    config.firstSpell[side] = (i8)(iVar1 % 3);
    players[side].phase = 2;
    signals.sound(0x2d);
  }
  if (inputs[side].counts[6] == 1) {
    players[side].help =
         '\x01' - (i8(players[side].help) != '\0');
  }
  return;

}
void CharacterSelectScene::choose_second_spell(u8 side,GameSelection& config,SelectionSettings& settings,SelectionProgress& progress,ReplayRandom& random,SceneSignals& signals){

  int iVar1;
  
  if (double(0.0f) < double(players[side].portraitOffset)) {
    players[side].portraitOffset = double(players[side].portraitOffset) - double(5.0f);
  }
  if (double(players[side].firstCardX) < double(80.0f)) {
    players[side].firstCardX = double(players[side].firstCardX) + double(4.0f);
  }
  if (double(players[side].firstCardY) < double(40.0f)) {
    players[side].firstCardY = double(players[side].firstCardY) + double(2.0f);
  }
  if (double(players[side].secondCardX) < double(80.0f)) {
    players[side].secondCardX = double(players[side].secondCardX) + double(4.0f);
  }
  if (double(players[side].secondCardY) < double(40.0f)) {
    players[side].secondCardY = double(players[side].secondCardY) + double(2.0f);
  }
  if ((double(players[side].cardTurn) < double(75.0f)) &&
     (players[side].cardTurn = double(players[side].cardTurn) + double(2.0f),
     double(75.0f) < double(players[side].cardTurn))) {
    players[side].cardTurn = 75.0f;
  }
  if (inputs[side].counts[1] == -1) {
    signals.sound(0x27);
    players[side].spell = 0;
  }
  else {
    iVar1 = std::abs(i32(inputs[side].counts[0]));
    if (iVar1 == 1) {
      signals.sound(0x27);
      players[side].spell = 1;
    }
    else if (inputs[side].counts[1] == 1) {
      signals.sound(0x27);
      players[side].spell = 2;
    }
  }
  if (inputs[side].counts[2] == 1) {
    if (double(players[side].cardTurn) < double(75.0f)) {
      players[side].cardTurn = 75.0f;
    }
    config.secondSpell[side] = players[side].spell;
    players[side].phase = 4;
    if ((u8(config.mode) == '\x03') || (u8(config.mode) == '\x04')) {
      players[0].phase = 8;
      players[1].phase = 8;
    }
    signals.sound(0x2d);
  }
  if (inputs[side].counts[3] == 1) {
    signals.sound(0x28);
    players[side].phase = 1;
  }
  if (inputs[side].counts[5] == 1) {
    players[side].highlightAll =
         '\x01' - (i8(players[side].highlightAll) != '\0');
  }
  if (inputs[side].counts[4] == 1) {
    iVar1 = random.next();
    config.secondSpell[side] = (i8)(iVar1 % 3);
    players[side].phase = 4;
    if ((u8(config.mode) == '\x03') || (u8(config.mode) == '\x04')) {
      players[0].phase = 8;
      players[1].phase = 8;
    }
    signals.sound(0x2d);
  }
  if (inputs[side].counts[6] == 1) {
    players[side].help =
         '\x01' - (i8(players[side].help) != '\0');
  }
  return;

}
void CharacterSelectScene::choose_background(u8 side,GameSelection& config,SelectionSettings& settings,SelectionProgress& progress,ReplayRandom& random,SceneSignals& signals){

  int iVar1;
  u32 uVar2;
  i8 local_5;
  
  if (double(players[side].firstCardX) < double(80.0f)) {
    players[side].firstCardX = double(players[side].firstCardX) + double(4.0f);
  }
  if (double(players[side].firstCardY) < double(40.0f)) {
    players[side].firstCardY = double(players[side].firstCardY) + double(2.0f);
  }
  if (double(players[side].secondCardX) < double(80.0f)) {
    players[side].secondCardX = double(players[side].secondCardX) + double(4.0f);
  }
  if (double(players[side].secondCardY) < double(40.0f)) {
    players[side].secondCardY = double(players[side].secondCardY) + double(2.0f);
  }
  if (double(0.0f) < double(players[side].cardTurn)) {
    players[side].cardTurn = double(players[side].cardTurn) - double(1.0f);
  }
  if (double(players[side].cardTurn) == double(60.0f)) {
    players[side].cardTurn = 0.0f;
  }
  if (inputs[side].counts[3] == 1) {
    signals.sound(0x28);
    players[side].phase = 0;
    players[side].firstCardX = 80.0f;
    players[side].firstCardY = 40.0f;
    players[side].secondCardX = 80.0f;
    players[side].secondCardY = 40.0f;
    players[side].cardsHidden = 1;
  }
  if ('\x03' < i8(players[1-(side)].phase)) {
    if (i8(players[0].randomTicks) < '\x01') {
      local_5 = (0 < inputs[side].counts[0]) * '\x02' + -1;
    }
    else {
      local_5 = '\x01';
    }
    iVar1 = std::abs(i32(inputs[side].counts[0]));
    if (((iVar1 == 1) ||
        ((iVar1 = std::abs(i32(inputs[side].counts[0])), 0x3c < iVar1 &&
         (iVar1 = std::abs(i32(inputs[side].counts[0])), iVar1 % 0xf == 0)))
        ) || (('\0' < i8(players[0].randomTicks) && (side == '\0')))) {
      backgroundCursor = (i8)((i8(backgroundCursor) + 10 + (int)local_5) % 10);
      if ((i8(backgroundCursor) == '\b') && (i8(backgroundVariant) == '\x01')) {
        config.background = '\x14';
      }
      else if (i8(backgroundCursor) < '\x06') {
        config.background = i8(backgroundCursor) + i8(backgroundVariant) * '\n';
      }
      else {
        config.background = i8(backgroundCursor);
      }
      while (i8(stageUnlocks[(int)config.background]) == '\0') {
        backgroundCursor = (i8)((i8(backgroundCursor) + 10 + (int)local_5) % 10);
        if ((i8(backgroundCursor) == '\b') && (i8(backgroundVariant) == '\x01')) {
          config.background = '\x14';
        }
        else if (i8(backgroundCursor) < '\x06') {
          config.background = i8(backgroundCursor) + i8(backgroundVariant) * '\n';
        }
        else {
          config.background = i8(backgroundCursor);
        }
      }
      if ('\0' < i8(players[0].randomTicks)) {
        uVar2 = random.next();
        uVar2 = uVar2 & 0x80000001;
        if ((int)uVar2 < 0) {
          uVar2 = (uVar2 - 1 | 0xfffffffe) + 1;
        }
        backgroundVariant = (i8)uVar2;
        if ((i8(backgroundCursor) == '\b') && (i8(backgroundVariant) == '\x01')) {
          config.background = '\x14';
        }
        else if (i8(backgroundCursor) < '\x06') {
          config.background = i8(backgroundCursor) + i8(backgroundVariant) * '\n';
        }
        else {
          config.background = i8(backgroundCursor);
        }
        while (i8(stageUnlocks[(int)config.background]) == '\0') {
          uVar2 = random.next();
          uVar2 = uVar2 & 0x80000001;
          if ((int)uVar2 < 0) {
            uVar2 = (uVar2 - 1 | 0xfffffffe) + 1;
          }
          backgroundVariant = (i8)uVar2;
          if ((i8(backgroundCursor) == '\b') && (i8(backgroundVariant) == '\x01')) {
            config.background = '\x14';
          }
          else if (i8(backgroundCursor) < '\x06') {
            config.background = i8(backgroundCursor) + i8(backgroundVariant) * '\n';
          }
          else {
            config.background = i8(backgroundCursor);
          }
        }
      }
    }
    else {
      iVar1 = std::abs(i32(inputs[side].counts[1]));
      if (((iVar1 == 1) || (inputs[side].counts[5] == 1)) ||
         ((iVar1 = std::abs(i32(inputs[side].counts[1])), iVar1 % 0xf == 0
          && (iVar1 = std::abs(i32(inputs[side].counts[1])), 0x3c < iVar1))
         )) {
        iVar1 = (u32)(0 < inputs[side].counts[1]) * 2 + -1;
        uVar2 = i8(backgroundVariant) + 2 + iVar1 & 0x80000001;
        if ((int)uVar2 < 0) {
          uVar2 = (uVar2 - 1 | 0xfffffffe) + 1;
        }
        backgroundVariant = (i8)uVar2;
        if ((i8(backgroundCursor) == '\b') && (i8(backgroundVariant) == '\x01')) {
          config.background = '\x14';
        }
        else if (i8(backgroundCursor) < '\x06') {
          config.background = i8(backgroundCursor) + i8(backgroundVariant) * '\n';
        }
        else {
          config.background = i8(backgroundCursor);
        }
        while (i8(stageUnlocks[(int)config.background]) == '\0') {
          uVar2 = i8(backgroundVariant) + 2 + iVar1 & 0x80000001;
          if ((int)uVar2 < 0) {
            uVar2 = (uVar2 - 1 | 0xfffffffe) + 1;
          }
          backgroundVariant = (i8)uVar2;
          if ((i8(backgroundCursor) == '\b') && (i8(backgroundVariant) == '\x01')) {
            config.background = '\x14';
          }
          else if (i8(backgroundCursor) < '\x06') {
            config.background = i8(backgroundCursor) + i8(backgroundVariant) * '\n';
          }
          else {
            config.background = i8(backgroundCursor);
          }
        }
        signals.sound(0x27);
      }
    }
    if ((inputs[side].counts[2] == 1) ||
       (i8(players[side].randomTicks) == -1)) {
      players[0].phase = 8;
      players[1].phase = 8;
      signals.sound(0x2d);
    }
    if (inputs[side].counts[4] == 1) {
      iVar1 = random.next();
      players[0].randomTicks = (i8)(iVar1 % 10) + '\x1e';
    }
    if ((side == '\0') && ('\0' < i8(players[0].randomTicks))) {
      players[0].randomTicks = i8(players[0].randomTicks) + -1;
    }
  }
  return;

}
void CharacterSelectScene::finish_selection(u8 side,GameSelection& config,SelectionSettings& settings,SelectionProgress& progress,ReplayRandom& random,SceneSignals& signals){

  if (double(players[side].firstCardX) < double(80.0f)) {
    players[side].firstCardX = double(players[side].firstCardX) + double(4.0f);
  }
  if (double(players[side].firstCardY) < double(40.0f)) {
    players[side].firstCardY = double(players[side].firstCardY) + double(2.0f);
  }
  if (double(players[side].secondCardX) < double(80.0f)) {
    players[side].secondCardX = double(players[side].secondCardX) + double(4.0f);
  }
  if (double(players[side].secondCardY) < double(40.0f)) {
    players[side].secondCardY = double(players[side].secondCardY) + double(2.0f);
  }
  if (double(0.0f) < double(players[side].cardTurn)) {
    players[side].cardTurn = double(players[side].cardTurn) - double(1.0f);
  }
  if (double(players[side].cardTurn) == double(60.0f)) {
    players[side].cardTurn = 0.0f;
  }
  if (inputs[side].counts[3] == 1) {
    signals.sound(0x28);
    players[side].phase = 0;
    players[side].firstCardX = 80.0f;
    players[side].firstCardY = 40.0f;
    players[side].secondCardX = 80.0f;
    players[side].secondCardY = 40.0f;
    players[side].cardsHidden = 1;
  }
  return;

}
void CharacterSelectScene::player_options(u8 side,GameSelection& config,SelectionSettings& settings,SelectionProgress& progress,ReplayRandom& random,SceneSignals& signals){

  int iVar1;
  
  iVar1 = std::abs(i32(inputs[side].counts[1]));
  if (iVar1 == 1) {
    signals.sound(0x27);
    players[side].panelCursor = '\x01' - i8(players[side].panelCursor);
  }
  if (i8(players[side].panelCursor) == '\0') {
    if ((inputs[side].counts[2] == 1) ||
       (iVar1 = std::abs(i32(inputs[side].counts[0])), iVar1 == 1)) {
      signals.sound(0x2d);
      settings.easyCommands[side] = '\x01' - (settings.easyCommands[side] != '\0');
    }
  }
  else {
    if ((inputs[side].counts[2] == 1) ||
       (inputs[side].counts[0] == 1)) {
      signals.sound(0x2d);
      settings.handicap[side] = (i8)(((i8)settings.handicap[side] + 1) % 5);
    }
    if (inputs[side].counts[0] == -1) {
      signals.sound(0x2d);
      settings.handicap[side] = (i8)(((i8)settings.handicap[side] + 4) % 5);
    }
  }
  if (inputs[side].counts[6] == 1) {
    signals.sound(0x28);
    players[side].panel =
         '\x01' - (i8(players[side].panel) != '\0');
  }
  return;

}
void CharacterSelectScene::story_options(GameSelection& config,SelectionSettings& settings,SelectionProgress& progress,ReplayRandom& random,SceneSignals& signals){

  i8 cVar1;
  i8 cVar2;
  i8 cVar3;
  int iVar4;
  
  iVar4 = std::abs(i32(inputs[0].counts[1]));
  if (iVar4 == 1) {
    signals.sound(0x27);
    cVar1 = i8(players[0].panelCursor);
    iVar4 = inputs[0].counts[1];
    cVar2 = progress.stages();
    cVar3 = progress.stages();
    players[0].panelCursor = (i8)((cVar1 + iVar4 + (int)cVar2) % (int)cVar3);
  }
  if (inputs[0].counts[2] == 1) {
    signals.sound(0x2d);
    config.character[0] = players[0].character;
    config.startingStage = i8(players[0].panelCursor) + '\x01';
    config.resumeStory = 1;
    config.alternatePalette[0] = 0;
    players[0].phase = 1;
    players[0].cardTurn = 0.0f;
    players[0].panel = 0;
  }
  if ((inputs[0].counts[6] == 1) || (inputs[0].counts[3] == 1)) {
    config.startingStage = '\x01';
    signals.sound(0x28);
    players[0].panel = 0;
    players[0].panelCursor = 0;
  }
  return;

}
u16 CharacterSelectScene::advance(GameSelection& config,SelectionSettings& settings,SelectionProgress& progress,ReplayRandom& random,CharacterSelectSignals& signals){

  u8 bVar1;
  
  u32 uVar2;
  
  int local_8;
  
  ;
  signals.render_selection();
  signals.present_selection();
  if (double(5.0f) < (double)double(entryZoom)) {
    entryZoom = ((double)double(entryZoom) * double(0.97f));
  }
  rotation = ((double)double(rotation) + double(0.08f));
  if ((i8(players[0].phase) < '\x04') || (i8(players[1].phase) < '\x04')) {
    if (5 < u8(stageAlpha)) {
      stageAlpha = (i8)stageAlpha + -5;
    }
    if (double(0.0f) < (double)players[0].portraitSlide) {
      players[0].portraitSlide = ((double)players[0].portraitSlide - double(5.0f));
    }
    if (double(0.0f) < (double)players[1].portraitSlide) {
      players[1].portraitSlide = ((double)players[1].portraitSlide - double(5.0f));
    }
  }
  else {
    if ((i8)stageAlpha != -1) {
      stageAlpha = (i8)stageAlpha + '\x05';
    }
    if ((double)players[0].portraitSlide < double(50.0f)) {
      players[0].portraitSlide = ((double)players[0].portraitSlide + double(5.0f));
    }
    if ((double)players[1].portraitSlide < double(50.0f)) {
      players[1].portraitSlide = ((double)players[1].portraitSlide + double(5.0f));
    }
  }
  if ((u8(config.mode) == '\x01') || (u8(config.mode) == '\x05')) {
    if ((hardwareCancel[config.activePlayer] == 1) || (escapeHeld == 1)) {
      signals.sound(0x28);
      if (i8(players[config.activePlayer].panel) == '\0') {
        if (((i8(players[0].phase) == '\0') && (i8(players[1].phase) == '\0'))
           || (escapeHeld == 1)) {
          return 0x2202;
        }
      }
      else {
        players[config.activePlayer].panel = 0;
      }
    }
    if (inputs[(1 - config.activePlayer)].counts[3] == 1) {
      signals.sound(0x28);
      if (i8(players[1-(config.activePlayer)].panel) == '\0') {
        if (i8(players[1-(config.activePlayer)].phase) == '\0') {
          inputs[config.activePlayer].counts[3] = 1;
        }
      }
      else {
        players[1-(config.activePlayer)].panel = 0;
      }
    }
  }
  else if ((u8(config.mode) == '\x03') || (u8(config.mode) == '\x04')) {
    if (((hardwareCancel[0] == 1) && (i8(players[0].phase) == '\0')) || (escapeHeld == 1)) {
      signals.sound(0x28);
      if (i8(players[0].panel) == '\0') {
        return 0x2202;
      }
      players[0].panel = 0;
    }
    players[1].phase = 8;
  }
  else if ((hardwareCancel[0] == 1) && (i8(players[0].panel) != '\0')) {
    signals.sound(0x28);
    players[0].panel = 0;
  }
  else if ((hardwareCancel[1] == 1) && (i8(players[1].panel) != '\0')) {
    signals.sound(0x28);
    players[1].panel = 0;
  }
  else if ((((hardwareCancel[0] == 1) || (hardwareCancel[1] == 1)) || (escapeHeld == 1)) &&
          (((i8(players[0].phase) == '\0' && (i8(players[1].phase) == '\0')) ||
           (escapeHeld == 1)))) {
    signals.sound(0x28);
    return 0x2202;
  }
  for (local_8 = 0; local_8 < 2; local_8 = local_8 + 1) {
    if (i8(players[local_8].panel) == '\0') {
      bVar1 = u8(players[local_8].phase);
      if (bVar1 < 9) {


        switch(bVar1) {
        case 0:
          choose_character((u8)local_8,config,settings,progress,random,signals);
          break;
        case 1:
          choose_first_spell((u8)local_8,config,settings,progress,random,signals);
          break;
        case 2:
          choose_second_spell((u8)local_8,config,settings,progress,random,signals);
          break;
        case 4:
          choose_background((u8)local_8,config,settings,progress,random,signals);
          break;
        case 8:
          finish_selection((u8)local_8,config,settings,progress,random,signals);
        }
      }
    }
    else if ((local_8 == 0) && (u8(config.mode) == '\x03')) {
      story_options(config,settings,progress,random,signals);
    }
    else {
      player_options((u8)local_8,config,settings,progress,random,signals);
    }
  }
  if ((i8(players[0].phase) == '\b') && (i8(players[1].phase) == '\b')) {
    config.storyStage = 0;
    if (((config.character[0] == '\x06') || (config.character[1] == '\x06')) && (config.background < '\x06')) {
      config.background = config.background + '\n';
    }
    if (u8(config.mode) == '\x03') {
      config.alternatePalette[0] = 0;
      config.alternatePalette[1] = 0;
    }
    signals.fade_music(-10000,-60,1,0);
    signals.prepare_battle();
    uVar2 = 0x2201;
  }
  else {
    backgroundScroll = ((double)double(backgroundScroll) + double(1.0f));
    if (double(1024.0f) < (double)double(backgroundScroll)) {
      backgroundScroll = ((double)double(backgroundScroll) - double(1024.0f));
    }
    uVar2 = 3;
  }
  return uVar2;

}
void CharacterSelectScene::reset(GameSelection& config,SelectionProgress& progress){

  players[1].highlightAll = 1;
  players[0].highlightAll = 1;
  players[1].spell = 0;
  players[0].spell = 0;
  players[1].portraitOffset = 100.0f;
  players[0].portraitOffset = 100.0f;
  players[1].portraitSlide = 100.0f;
  players[0].portraitSlide = 100.0f;
  players[1].firstCardX = 80.0f;
  players[0].firstCardX = 80.0f;
  players[1].firstCardY = 40.0f;
  players[0].firstCardY = 40.0f;
  players[1].secondCardX = 80.0f;
  players[0].secondCardX = 80.0f;
  players[1].secondCardY = 40.0f;
  players[0].secondCardY = 40.0f;
  players[1].cardTurn = 0.0f;
  players[0].cardTurn = 0.0f;
  stageAlpha = 0;
  backgroundScroll = 0.0f;
  players[1].phase = 0;
  players[0].phase = 0;
  players[1].panel = 0;
  players[0].panel = 0;
  players[1].help = 0;
  players[0].help = 0;
  players[1].panelCursor = 0;
  players[0].panelCursor = 0;
  entryZoom = 80.0f;
  rotation = 4.712388038635254f;
  players[0].character = config.character[0];
  players[1].character = config.character[1];
  players[0].previousCharacter = (i8)((config.character[0] + 1) % 10);
  players[1].previousCharacter = (i8)((config.character[0] + 1) % 10);
  players[1].cardsHidden = 1;
  players[0].cardsHidden = 1;
  if (config.background == '\x14') {
    backgroundCursor = 8;
    backgroundVariant = 1;
  }
  else {
    backgroundCursor = config.background % '\n';
    backgroundVariant = '\t' < config.background;
  }
  progress.inspect(config.character[0],config.difficulty);
  return;

}
void CharacterSelectScene::initialize(GameSelection& config,SelectionProgress& progress,const SelectionUnlocks& unlocks){for(u32 i=0;i<15;i++){duelUnlocks[i]=unlocks.character_available(i8(i),false);storyUnlocks[i]=unlocks.character_available(i8(i),true);}for(u32 i=0;i<30;i++)stageUnlocks[i]=unlocks.background_available(i8(i));players[0].randomTicks=players[1].randomTicks=0;reset(config,progress);}
void CharacterSelectScene::poll(const std::array<SelectInput,2>& input,bool escape,const GameSelection& config){hardwareCancel={input[0].counts[3],input[1].counts[3]};inputs[0]=input[0];if(config.mode!=GameMode::Story)inputs[1]=input[1];if((config.mode==GameMode::Training||config.mode==GameMode::Versus||config.mode==GameMode::Story)&&i8(players[config.activePlayer].phase)>3){inputs[1-config.activePlayer]=inputs[config.activePlayer];inputs[config.activePlayer]={};}escapeHeld=escape?increment_wrapped(escapeHeld,1):0;}
}
