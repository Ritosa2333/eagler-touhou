#include "OptionsScene.hpp"
namespace th075 {
u16 OptionsScene::advance(GameSettings& settings,OptionsSignals& events){

  int iVar1;
  u32 uVar2;
  u32 uVar3;
  
  
  events.render_options();
  events.present_options();
  if (vertical < '\0') {
    events.sound(0x27);
    selected = (i16)selected + -1;
    iVar1 = selected;
    if ((i16)iVar1 < 0) {
      selected = 6;
    }
    events.description(u8(selected));
  }
  if ('\0' < vertical) {
    events.sound(0x27);
    selected = (i16)selected + 1;
    iVar1 = selected;
    if (6 < (i16)iVar1) {
      selected = 0;
    }
    events.description(u8(selected));
  }
  switch((i16)selected) {
  case 0:
    if ((horizontal != '\0') || (confirm != '\0')) {
      events.sound(0x27);
      uVar2 = (int)settings.difficulty + (int)horizontal + 4 +
              (int)confirm & 0x80000003;
      if ((int)uVar2 < 0) {
        uVar2 = (uVar2 - 1 | 0xfffffffc) + 1;
      }
      settings.difficulty = (i8)uVar2;
    }
    break;
  case 1:
    if ((horizontal != '\0') || (confirm != '\0')) {
      events.sound(0x2d);
      settings.animatedBackground = '\x01' - (settings.animatedBackground != '\0');
    }
    break;
  case 2:
    if ((horizontal != '\0') || (confirm != '\0')) {
      events.sound(0x2d);
      settings.renderEveryFrame = '\x01' - (settings.renderEveryFrame != '\0');
    }
    break;
  case 3:
    if ((horizontal != '\0') || (confirm != '\0')) {
      events.sound(0x2d);
      settings.recoveryMarkers = '\x01' - (settings.recoveryMarkers != '\0');
    }
    break;
  case 4:
    if ('\0' < horizontal) {
      settings.musicVolume = settings.musicVolume + '\x05';
      if ('d' < settings.musicVolume) {
        settings.musicVolume = 'd';
      }
      events.music_volume(settings.musicVolume);
      events.start_music(0);
    }
    if (horizontal < '\0') {
      settings.musicVolume = settings.musicVolume + -5;
      if (settings.musicVolume < '\0') {
        settings.musicVolume = '\0';
      }
      events.music_volume(settings.musicVolume);
      events.start_music(0);
    }
    if (confirm != '\0') {
      events.sound(0x2d);
    }
    break;
  case 5:
    if (soundWait == 0) {
      events.sound(0x28);
      soundWait = 0x3c;
    }
    soundWait = soundWait + -1;
    if ('\0' < horizontal) {
      settings.soundVolume = settings.soundVolume + '\x05';
      if ('d' < settings.soundVolume) {
        settings.soundVolume = 'd';
      }
      events.sound_volume(settings.soundVolume);
    }
    if (horizontal < '\0') {
      settings.soundVolume = settings.soundVolume + -5;
      if (settings.soundVolume < '\0') {
        settings.soundVolume = '\0';
      }
      events.sound_volume(settings.soundVolume);
    }
    if (confirm != '\0') {
      events.sound(0x2d);
    }
    break;
  case 6:
    if (confirm != '\0') {
      events.sound(0x2d);
      events.save_settings();
      return 0x2202;
    }
  }
  if ((i8)alpha != -1) {
    alpha = (i8)alpha + '\x19';
  }
  if ((i8)cancel == '\x01') {
    events.sound(0x28);
    events.save_settings();
    uVar3 = 0x2202;
  }
  else {
    uVar3 = 9;
  }
  return uVar3;

}
void OptionsScene::poll(const MenuInput& input){horizontal=vertical=confirm=cancel=0;if(input.vertical[0]==1||input.vertical[1]==1)vertical=1;if(input.vertical[0]==-1||input.vertical[1]==-1)vertical=-1;if(input.horizontal[0]==1||input.horizontal[1]==1)horizontal=1;if(input.horizontal[0]==-1||input.horizontal[1]==-1)horizontal=-1;if(input.confirm[0]==1||input.confirm[1]==1)confirm=1;if(input.cancel[0]==1||input.cancel[1]==1||input.escape)cancel=1;}
}
