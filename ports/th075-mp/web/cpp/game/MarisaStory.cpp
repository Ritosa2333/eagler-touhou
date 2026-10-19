#include "MarisaStory.hpp"
namespace th075 {
void advance_marisa_story(FighterActor& a,FighterActor& other,StoryCutscene& s,i16& wait,StoryEnvironment& env,StoryEvents& events){

  i8 dialogueReady;
  
  switch(u8(s.step)) {
  case 0:
    env.camera.override_view(1,0.0f,(double(120.0f) - (float)(i16(wait) + -0x32) * double(0.5f)) *
                     (float)(i16(wait) < 0x123),320.0f);
    if (i16(wait) == 0) {
      s.x[0] = -192.0f;
      s.y[0] = 160.0f;
      s.x[1] = -192.0f;
      s.y[1] = 336.0f;
      s.x[2] = 0.0f;
      s.y[2] = 176.0f;
      s.x[3] = 320.0f;
      s.y[3] = 280.0f;
      events.begin_dialogue(100);
      events.show_dialogue(0);
      events.advance_dialogue();
      events.advance_dialogue();
    }
    if (0x31 < i16(wait)) {
      s.x[0] = double(s.x[0]) + double(0.25f);
      s.x[1] = double(s.x[1]) - double(0.25f);
      if (env.fadeAlpha < 5) {
        env.fadeAlpha = 0;
      }
      else {
        env.fadeAlpha = env.fadeAlpha - 5;
      }
    }
    if (i16(wait) < 0x12d) {
      if (u8(s.alpha[2]) < 0xfa) {
        s.alpha[2] = i8(s.alpha[2]) + '\x05';
      }
      else {
        s.alpha[2] = 0xff;
      }
    }
    if ((0x31 < i16(wait)) && (i16(wait) < 0x12d)) {
      if (u8(s.alpha[1]) < 0xfa) {
        s.alpha[0] = i8(s.alpha[0]) + '\x05';
        s.alpha[1] = i8(s.alpha[1]) + '\x05';
      }
      else {
        s.alpha[0] = 0xff;
        s.alpha[1] = 0xff;
      }
    }
    if (i16(wait) == 0xe6) {
      a.select_action(9);
      a.motion.velocityX = 7.75f;
      a.motion.velocityY = 18.0f;
      a.motion.accelerationY = 0.6000000238418579f;
    }
    if (300 < i16(wait)) {
      if (u8(s.alpha[0]) < 5) {
        s.alpha[0] = 0;
        s.alpha[1] = 0;
        s.alpha[2] = 0;
      }
      else {
        s.alpha[0] = i8(s.alpha[0]) + -5;
        s.alpha[1] = i8(s.alpha[1]) + -5;
        s.alpha[2] = i8(s.alpha[2]) + -5;
      }
    }
    if ((i16(a.animation.cursor.action) != 9) && (0xe6 < i16(wait))) {
      a.motion.x = 480.0f;
    }
    if (i16(wait) < 0x169) {
      wait = i16(wait) + 1;
    }
    else {
      s.step = u8(1);
      wait = 0;
    }
    break;
  case 1:
    events.begin_dialogue(0);
    s.step = u8(2);
    break;
  case 2:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      s.step = u8(3);
    }
    break;
  case 3:
    if (i16(wait) == 0) {
      other.select_action(9);
      other.motion.velocityX = 7.75f;
      other.motion.velocityY = 18.0f;
      other.motion.accelerationY = 0.6000000238418579f;
    }
    if (i16(other.animation.cursor.action) == 9) {
      wait = i16(wait) + 1;
    }
    else {
      other.motion.x = 800.0f;
      s.step = u8(4);
      wait = 0;
    }
    break;
  case 4:
    events.begin_dialogue(1);
    s.step = u8(5);
    break;
  case 5:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (i16(wait) == 0) {
        events.hide_portrait(a);
        events.hide_portrait(other);
      }
      if (i16(wait) < 0x1f) {
        wait = i16(wait) + 1;
      }
      else {
        s.alpha[3] = 0;
        a.scriptCursor.waiting = 0;
        events.show_dialogue(0);
        a.dialogueFinished = 1;
        env.camera.override_view(0,0.0f,0.0f,0.0f);
        s.step = u8(6);
        wait = 0;
      }
    }
    break;
  case 6:
    if (i16(wait) == 0) {
      events.show_portrait(a,5,1);
      events.show_portrait(other,8,0);
    }
    if (i16(wait) == 0x1e) {
      events.begin_dialogue(2);
      s.step = u8(7);
      wait = 0;
    }
    else {
      wait = i16(wait) + 1;
    }
    break;
  case 7:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      a.dialogueFinished = 1;
      s.step = u8(10);
      s.nextStage = 2;
    }
    break;
  case 10:
    env.camera.override_view(1,0.0f,(double(120.0f) - (float)(i16(wait) + -0x32) * double(0.5f)) *
                     (float)(i16(wait) < 0x123),320.0f);
    if (i16(wait) == 0) {
      s.x[0] = -192.0f;
      s.y[0] = 160.0f;
      s.x[1] = -192.0f;
      s.y[1] = 336.0f;
      s.x[2] = 0.0f;
      s.y[2] = 176.0f;
      s.x[3] = 320.0f;
      s.y[3] = 280.0f;
      events.begin_dialogue(0x65);
      events.show_dialogue(0);
      events.advance_dialogue();
      events.advance_dialogue();
    }
    if (0x31 < i16(wait)) {
      s.x[0] = double(s.x[0]) + double(0.25f);
      s.x[1] = double(s.x[1]) - double(0.25f);
      if (env.fadeAlpha < 5) {
        env.fadeAlpha = 0;
      }
      else {
        env.fadeAlpha = env.fadeAlpha - 5;
      }
    }
    if (i16(wait) < 0x12d) {
      if (u8(s.alpha[2]) < 0xfa) {
        s.alpha[2] = i8(s.alpha[2]) + '\x05';
      }
      else {
        s.alpha[2] = 0xff;
      }
    }
    if ((0x31 < i16(wait)) && (i16(wait) < 0x12d)) {
      if (u8(s.alpha[1]) < 0xfa) {
        s.alpha[0] = i8(s.alpha[0]) + '\x05';
        s.alpha[1] = i8(s.alpha[1]) + '\x05';
      }
      else {
        s.alpha[0] = 0xff;
        s.alpha[1] = 0xff;
      }
    }
    if (i16(wait) == 0xe6) {
      a.select_action(9);
      a.motion.velocityX = 7.75f;
      a.motion.velocityY = 18.0f;
      a.motion.accelerationY = 0.6000000238418579f;
    }
    if (300 < i16(wait)) {
      if (u8(s.alpha[0]) < 5) {
        s.alpha[0] = 0;
        s.alpha[1] = 0;
        s.alpha[2] = 0;
      }
      else {
        s.alpha[0] = i8(s.alpha[0]) + -5;
        s.alpha[1] = i8(s.alpha[1]) + -5;
        s.alpha[2] = i8(s.alpha[2]) + -5;
      }
    }
    if ((i16(a.animation.cursor.action) != 9) && (0xe6 < i16(wait))) {
      a.motion.x = 480.0f;
    }
    if (i16(wait) < 0x169) {
      wait = i16(wait) + 1;
    }
    else {
      s.step = u8(0xb);
      wait = 0;
    }
    break;
  case 0xb:
    events.begin_dialogue(3);
    s.step = u8(0xc);
    break;
  case 0xc:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      s.step = u8(0xd);
    }
    break;
  case 0xd:
    if (i16(wait) == 0) {
      other.select_action(9);
      other.motion.velocityX = 7.75f;
      other.motion.velocityY = 18.0f;
      other.motion.accelerationY = 0.6000000238418579f;
    }
    if (i16(other.animation.cursor.action) == 9) {
      wait = i16(wait) + 1;
    }
    else {
      other.motion.x = 800.0f;
      s.step = u8(0xe);
      wait = 0;
    }
    break;
  case 0xe:
    events.begin_dialogue(4);
    s.step = u8(0xf);
    break;
  case 0xf:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (i16(wait) == 0) {
        events.hide_portrait(a);
        events.hide_portrait(other);
      }
      if (i16(wait) < 0x1f) {
        wait = i16(wait) + 1;
      }
      else {
        s.alpha[3] = 0;
        a.scriptCursor.waiting = 0;
        events.show_dialogue(0);
        a.dialogueFinished = 1;
        env.camera.override_view(0,0.0f,0.0f,0.0f);
        s.step = u8(0x10);
        wait = 0;
      }
    }
    break;
  case 0x10:
    if (i16(wait) == 0) {
      events.show_portrait(a,3,1);
      events.show_portrait(other,8,0);
    }
    if (i16(wait) == 0x1e) {
      events.begin_dialogue(5);
      s.step = u8(0x11);
      wait = 0;
    }
    else {
      wait = i16(wait) + 1;
    }
    break;
  case 0x11:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      a.dialogueFinished = 1;
      s.step = u8(0x14);
      s.nextStage = 3;
    }
    break;
  case 0x14:
    env.camera.override_view(1,0.0f,(double(120.0f) - (float)(i16(wait) + -0x32) * double(0.5f)) *
                     (float)(i16(wait) < 0x123),320.0f);
    if (i16(wait) == 0) {
      s.x[0] = -192.0f;
      s.y[0] = 160.0f;
      s.x[1] = -192.0f;
      s.y[1] = 336.0f;
      s.x[2] = 0.0f;
      s.y[2] = 176.0f;
      s.x[3] = 320.0f;
      s.y[3] = 280.0f;
      events.begin_dialogue(0x66);
      events.show_dialogue(0);
      events.advance_dialogue();
      events.advance_dialogue();
    }
    if (0x31 < i16(wait)) {
      s.x[0] = double(s.x[0]) + double(0.25f);
      s.x[1] = double(s.x[1]) - double(0.25f);
      if (env.fadeAlpha < 5) {
        env.fadeAlpha = 0;
      }
      else {
        env.fadeAlpha = env.fadeAlpha - 5;
      }
    }
    if (i16(wait) < 0x12d) {
      if (u8(s.alpha[2]) < 0xfa) {
        s.alpha[2] = i8(s.alpha[2]) + '\x05';
      }
      else {
        s.alpha[2] = 0xff;
      }
    }
    if ((0x31 < i16(wait)) && (i16(wait) < 0x12d)) {
      if (u8(s.alpha[1]) < 0xfa) {
        s.alpha[0] = i8(s.alpha[0]) + '\x05';
        s.alpha[1] = i8(s.alpha[1]) + '\x05';
      }
      else {
        s.alpha[0] = 0xff;
        s.alpha[1] = 0xff;
      }
    }
    if (i16(wait) == 0xe6) {
      a.select_action(9);
      a.motion.velocityX = 7.75f;
      a.motion.velocityY = 18.0f;
      a.motion.accelerationY = 0.6000000238418579f;
    }
    if (300 < i16(wait)) {
      if (u8(s.alpha[0]) < 5) {
        s.alpha[0] = 0;
        s.alpha[1] = 0;
        s.alpha[2] = 0;
      }
      else {
        s.alpha[0] = i8(s.alpha[0]) + -5;
        s.alpha[1] = i8(s.alpha[1]) + -5;
        s.alpha[2] = i8(s.alpha[2]) + -5;
      }
    }
    if ((i16(a.animation.cursor.action) != 9) && (0xe6 < i16(wait))) {
      a.motion.x = 480.0f;
    }
    if (i16(wait) < 0x169) {
      wait = i16(wait) + 1;
    }
    else {
      s.step = u8(0x15);
      wait = 0;
    }
    break;
  case 0x15:
    events.begin_dialogue(6);
    s.step = u8(0x16);
    break;
  case 0x16:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      s.step = u8(0x17);
    }
    break;
  case 0x17:
    if (i16(wait) == 0) {
      other.select_action(9);
      other.motion.velocityX = 7.75f;
      other.motion.velocityY = 18.0f;
      other.motion.accelerationY = 0.6000000238418579f;
    }
    if (i16(other.animation.cursor.action) == 9) {
      wait = i16(wait) + 1;
    }
    else {
      other.motion.x = 800.0f;
      s.step = u8(0x18);
      wait = 0;
    }
    break;
  case 0x18:
    events.begin_dialogue(7);
    s.step = u8(0x19);
    break;
  case 0x19:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (i16(wait) == 0) {
        events.hide_portrait(a);
        events.hide_portrait(other);
      }
      if (i16(wait) < 0x1f) {
        wait = i16(wait) + 1;
      }
      else {
        s.alpha[3] = 0;
        a.scriptCursor.waiting = 0;
        events.show_dialogue(0);
        a.dialogueFinished = 1;
        env.camera.override_view(0,0.0f,0.0f,0.0f);
        s.step = u8(0x1a);
        wait = 0;
      }
    }
    break;
  case 0x1a:
    if (i16(wait) == 0) {
      events.show_portrait(a,0,0);
      events.show_portrait(other,8,1);
    }
    if (i16(wait) == 0x1e) {
      events.begin_dialogue(8);
      s.step = u8(0x1b);
      wait = 0;
    }
    else {
      wait = i16(wait) + 1;
    }
    break;
  case 0x1b:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      a.dialogueFinished = 1;
      s.step = u8(0x1e);
      env.completedStages = env.completedStages + '\x01';
      events.sound(0x32);
      s.nextStage = 4;
    }
    break;
  case 0x1e:
    env.camera.override_view(1,0.0f,(double(120.0f) - (float)(i16(wait) + -0x32) * double(0.5f)) *
                     (float)(i16(wait) < 0x123),320.0f);
    if (i16(wait) == 0) {
      s.x[0] = -192.0f;
      s.y[0] = 160.0f;
      s.x[1] = -192.0f;
      s.y[1] = 336.0f;
      s.x[2] = 0.0f;
      s.y[2] = 176.0f;
      s.x[3] = 320.0f;
      s.y[3] = 280.0f;
      events.begin_dialogue(0x67);
      events.show_dialogue(0);
      events.advance_dialogue();
      events.advance_dialogue();
    }
    if (0x31 < i16(wait)) {
      s.x[0] = double(s.x[0]) + double(0.25f);
      s.x[1] = double(s.x[1]) - double(0.25f);
      if (env.fadeAlpha < 5) {
        env.fadeAlpha = 0;
      }
      else {
        env.fadeAlpha = env.fadeAlpha - 5;
      }
    }
    if (i16(wait) < 0x12d) {
      if (u8(s.alpha[2]) < 0xfa) {
        s.alpha[2] = i8(s.alpha[2]) + '\x05';
      }
      else {
        s.alpha[2] = 0xff;
      }
    }
    if ((0x31 < i16(wait)) && (i16(wait) < 0x12d)) {
      if (u8(s.alpha[1]) < 0xfa) {
        s.alpha[0] = i8(s.alpha[0]) + '\x05';
        s.alpha[1] = i8(s.alpha[1]) + '\x05';
      }
      else {
        s.alpha[0] = 0xff;
        s.alpha[1] = 0xff;
      }
    }
    if (i16(wait) == 0xe6) {
      a.select_action(9);
      a.motion.velocityX = 7.75f;
      a.motion.velocityY = 18.0f;
      a.motion.accelerationY = 0.6000000238418579f;
    }
    if (300 < i16(wait)) {
      if (u8(s.alpha[0]) < 5) {
        s.alpha[0] = 0;
        s.alpha[1] = 0;
        s.alpha[2] = 0;
      }
      else {
        s.alpha[0] = i8(s.alpha[0]) + -5;
        s.alpha[1] = i8(s.alpha[1]) + -5;
        s.alpha[2] = i8(s.alpha[2]) + -5;
      }
    }
    if ((i16(a.animation.cursor.action) != 9) && (0xe6 < i16(wait))) {
      a.motion.x = 480.0f;
    }
    if (i16(wait) < 0x169) {
      wait = i16(wait) + 1;
    }
    else {
      s.step = u8(0x1f);
      wait = 0;
    }
    break;
  case 0x1f:
    events.begin_dialogue(9);
    s.step = u8(0x20);
    break;
  case 0x20:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      s.step = u8(0x21);
    }
    break;
  case 0x21:
    if (i16(wait) == 0) {
      other.select_action(9);
      other.motion.velocityX = 7.75f;
      other.motion.velocityY = 18.0f;
      other.motion.accelerationY = 0.6000000238418579f;
    }
    if (i16(other.animation.cursor.action) == 9) {
      wait = i16(wait) + 1;
    }
    else {
      other.motion.x = 800.0f;
      s.step = u8(0x22);
      wait = 0;
    }
    break;
  case 0x22:
    events.begin_dialogue(10);
    s.step = u8(0x23);
    break;
  case 0x23:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (i16(wait) == 0) {
        events.hide_portrait(a);
        events.hide_portrait(other);
      }
      if (i16(wait) < 0x1f) {
        wait = i16(wait) + 1;
      }
      else {
        s.alpha[3] = 0;
        a.scriptCursor.waiting = 0;
        events.show_dialogue(0);
        a.dialogueFinished = 1;
        env.camera.override_view(0,0.0f,0.0f,0.0f);
        s.step = u8(0x24);
        wait = 0;
      }
    }
    break;
  case 0x24:
    if (i16(wait) == 0) {
      events.show_portrait(a,0,1);
      events.show_portrait(other,8,0);
    }
    if (i16(wait) == 0x1e) {
      events.begin_dialogue(0xb);
      s.step = u8(0x25);
      wait = 0;
    }
    else {
      wait = i16(wait) + 1;
    }
    break;
  case 0x25:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      a.dialogueFinished = 1;
      s.step = u8(0x28);
      s.nextStage = 5;
    }
    break;
  case 0x28:
    env.camera.override_view(1,0.0f,(double(120.0f) - (float)(i16(wait) + -0x32) * double(0.5f)) *
                     (float)(i16(wait) < 0x123),320.0f);
    if (i16(wait) == 0) {
      s.x[0] = -192.0f;
      s.y[0] = 160.0f;
      s.x[1] = -192.0f;
      s.y[1] = 336.0f;
      s.x[2] = 0.0f;
      s.y[2] = 176.0f;
      s.x[3] = 320.0f;
      s.y[3] = 280.0f;
      events.begin_dialogue(0x68);
      events.show_dialogue(0);
      events.advance_dialogue();
      events.advance_dialogue();
    }
    if (0x31 < i16(wait)) {
      s.x[0] = double(s.x[0]) + double(0.25f);
      s.x[1] = double(s.x[1]) - double(0.25f);
      if (env.fadeAlpha < 5) {
        env.fadeAlpha = 0;
      }
      else {
        env.fadeAlpha = env.fadeAlpha - 5;
      }
    }
    if (i16(wait) < 0x12d) {
      if (u8(s.alpha[2]) < 0xfa) {
        s.alpha[2] = i8(s.alpha[2]) + '\x05';
      }
      else {
        s.alpha[2] = 0xff;
      }
    }
    if ((0x31 < i16(wait)) && (i16(wait) < 0x12d)) {
      if (u8(s.alpha[1]) < 0xfa) {
        s.alpha[0] = i8(s.alpha[0]) + '\x05';
        s.alpha[1] = i8(s.alpha[1]) + '\x05';
      }
      else {
        s.alpha[0] = 0xff;
        s.alpha[1] = 0xff;
      }
    }
    if (i16(wait) == 0xe6) {
      a.select_action(9);
      a.motion.velocityX = 7.75f;
      a.motion.velocityY = 18.0f;
      a.motion.accelerationY = 0.6000000238418579f;
    }
    if (300 < i16(wait)) {
      if (u8(s.alpha[0]) < 5) {
        s.alpha[0] = 0;
        s.alpha[1] = 0;
        s.alpha[2] = 0;
      }
      else {
        s.alpha[0] = i8(s.alpha[0]) + -5;
        s.alpha[1] = i8(s.alpha[1]) + -5;
        s.alpha[2] = i8(s.alpha[2]) + -5;
      }
    }
    if ((i16(a.animation.cursor.action) != 9) && (0xe6 < i16(wait))) {
      a.motion.x = 480.0f;
    }
    if (i16(wait) < 0x169) {
      wait = i16(wait) + 1;
    }
    else {
      s.step = u8(0x2b);
      wait = 0;
    }
    break;
  case 0x2b:
    if (i16(wait) == 0) {
      other.select_action(9);
      other.motion.velocityX = 7.75f;
      other.motion.velocityY = 18.0f;
      other.motion.accelerationY = 0.6000000238418579f;
    }
    if (i16(other.animation.cursor.action) == 9) {
      wait = i16(wait) + 1;
    }
    else {
      other.motion.x = 800.0f;
      s.step = u8(0x2c);
      wait = 0;
    }
    break;
  case 0x2c:
    events.begin_dialogue(0xc);
    s.step = u8(0x2d);
    break;
  case 0x2d:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (i16(wait) == 0) {
        events.hide_portrait(a);
        events.hide_portrait(other);
      }
      if (i16(wait) < 0x1f) {
        wait = i16(wait) + 1;
      }
      else {
        s.alpha[3] = 0;
        a.scriptCursor.waiting = 0;
        events.show_dialogue(0);
        a.dialogueFinished = 1;
        env.camera.override_view(0,0.0f,0.0f,0.0f);
        s.step = u8(0x2e);
        wait = 0;
      }
    }
    break;
  case 0x2e:
    if (env.route < '\x03') {
      if (i16(wait) < 0x78) {
        wait = i16(wait) + 1;
      }
      else {
        s.step = u8(0x46);
        s.nextStage = 8;
        a.dialogueFinished = 1;
      }
    }
    else {
      if (i16(wait) == 0) {
        events.show_portrait(a,6,0);
        events.show_portrait(other,8,1);
      }
      if (i16(wait) == 0x1e) {
        events.begin_dialogue(0xd);
        s.step = u8(0x2f);
        wait = 0;
      }
      else {
        wait = i16(wait) + 1;
      }
    }
    break;
  case 0x2f:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      a.dialogueFinished = 1;
      s.step = u8(0x32);
      s.nextStage = 6;
    }
    break;
  case 0x32:
    env.camera.override_view(1,0.0f,(double(120.0f) - (float)(i16(wait) + -0x32) * double(0.5f)) *
                     (float)(i16(wait) < 0x123),320.0f);
    if (i16(wait) == 0) {
      s.x[0] = -192.0f;
      s.y[0] = 160.0f;
      s.x[1] = -192.0f;
      s.y[1] = 336.0f;
      s.x[2] = 0.0f;
      s.y[2] = 176.0f;
      s.x[3] = 320.0f;
      s.y[3] = 280.0f;
      events.begin_dialogue(0x69);
      events.show_dialogue(0);
      events.advance_dialogue();
      events.advance_dialogue();
    }
    if (0x31 < i16(wait)) {
      s.x[0] = double(s.x[0]) + double(0.25f);
      s.x[1] = double(s.x[1]) - double(0.25f);
      if (env.fadeAlpha < 5) {
        env.fadeAlpha = 0;
      }
      else {
        env.fadeAlpha = env.fadeAlpha - 5;
      }
    }
    if (i16(wait) < 0x12d) {
      if (u8(s.alpha[2]) < 0xfa) {
        s.alpha[2] = i8(s.alpha[2]) + '\x05';
      }
      else {
        s.alpha[2] = 0xff;
      }
    }
    if ((0x31 < i16(wait)) && (i16(wait) < 0x12d)) {
      if (u8(s.alpha[1]) < 0xfa) {
        s.alpha[0] = i8(s.alpha[0]) + '\x05';
        s.alpha[1] = i8(s.alpha[1]) + '\x05';
      }
      else {
        s.alpha[0] = 0xff;
        s.alpha[1] = 0xff;
      }
    }
    if (i16(wait) == 0xe6) {
      a.select_action(9);
      a.motion.velocityX = 7.75f;
      a.motion.velocityY = 18.0f;
      a.motion.accelerationY = 0.6000000238418579f;
    }
    if (300 < i16(wait)) {
      if (u8(s.alpha[0]) < 5) {
        s.alpha[0] = 0;
        s.alpha[1] = 0;
        s.alpha[2] = 0;
      }
      else {
        s.alpha[0] = i8(s.alpha[0]) + -5;
        s.alpha[1] = i8(s.alpha[1]) + -5;
        s.alpha[2] = i8(s.alpha[2]) + -5;
      }
    }
    if ((i16(a.animation.cursor.action) != 9) && (0xe6 < i16(wait))) {
      a.motion.x = 480.0f;
    }
    if (i16(wait) < 0x169) {
      wait = i16(wait) + 1;
    }
    else {
      s.step = u8(0x33);
      wait = 0;
    }
    break;
  case 0x33:
    events.begin_dialogue(0xe);
    s.step = u8(0x34);
    break;
  case 0x34:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      s.step = u8(0x35);
      events.hide_portrait(a);
      wait = 0;
    }
    break;
  case 0x35:
    env.fadeColor = 0xffffff;
    if (i16(wait) < 0x1e) {
      wait = i16(wait) + 1;
    }
    if (i16(wait) == 0x1e) {
      if (env.fadeAlpha < 0xf0) {
        env.fadeAlpha = env.fadeAlpha + 0xf;
      }
      else {
        env.fadeAlpha = 0xff;
      }
      if (env.fadeAlpha == 0xff) {
        wait = 0x1f;
      }
    }
    if ((0x1e < i16(wait)) && (i16(wait) < 0x5a)) {
      if (i16(wait) == 0x3c) {
        events.show_dialogue(0);
        env.background = 8;
        events.reload_stage(env.background);
        other.motion.x = 800.0f;
        a.motion.x = 480.0f;
      }
      wait = i16(wait) + 1;
    }
    if (i16(wait) == 0x5a) {
      if (env.fadeAlpha < 5) {
        env.fadeAlpha = 0;
      }
      else {
        env.fadeAlpha = env.fadeAlpha - 5;
      }
      if (env.fadeAlpha == 0) {
        wait = 0;
        s.step = u8(0x36);
      }
    }
    break;
  case 0x36:
    events.begin_dialogue(0xf);
    s.step = u8(0x37);
    break;
  case 0x37:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (i16(wait) == 0) {
        events.hide_portrait(a);
        events.hide_portrait(other);
      }
      if (i16(wait) < 0x1f) {
        wait = i16(wait) + 1;
      }
      else {
        s.alpha[3] = 0;
        a.scriptCursor.waiting = 0;
        events.show_dialogue(0);
        a.dialogueFinished = 1;
        env.camera.override_view(0,0.0f,0.0f,0.0f);
        s.step = u8(0x38);
        wait = 0;
      }
    }
    break;
  case 0x38:
    if (env.route < '\x03') {
      if (i16(wait) < 0x78) {
        wait = i16(wait) + 1;
      }
      else {
        s.step = u8(0x50);
        s.nextStage = 9;
        a.dialogueFinished = 1;
      }
    }
    else {
      if (i16(wait) == 0) {
        events.show_portrait(a,1,0);
        events.show_portrait(other,8,1);
      }
      if (i16(wait) == 0x1e) {
        events.begin_dialogue(0x10);
        s.step = u8(0x39);
        wait = 0;
      }
      else {
        wait = i16(wait) + 1;
      }
    }
    break;
  case 0x39:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      a.dialogueFinished = 1;
      s.step = u8(0x3c);
      s.nextStage = 7;
    }
    break;
  case 0x3c:
    if (i16(wait) == 0) {
      events.begin_dialogue(0x6a);
      events.show_dialogue(0);
      events.advance_dialogue();
      events.advance_dialogue();
      s.x[0] = -192.0f;
      s.y[0] = 160.0f;
      s.x[1] = -192.0f;
      s.y[1] = 336.0f;
      s.x[2] = 0.0f;
      s.y[2] = 176.0f;
      s.x[3] = 320.0f;
      s.y[3] = 280.0f;
      wait = i16(wait) + 0x3c;
    }
    if (0x9f < i16(wait)) {
      s.x[0] = double(s.x[0]) + double(0.25f);
      s.x[1] = double(s.x[1]) - double(0.25f);
    }
    if ((0x77 < i16(wait)) && (i16(wait) < 0x169)) {
      if (u8(s.alpha[2]) < 0xfa) {
        s.alpha[2] = i8(s.alpha[2]) + '\x05';
      }
      else {
        s.alpha[2] = 0xff;
      }
    }
    if ((0x9f < i16(wait)) && (i16(wait) < 0x169)) {
      if (u8(s.alpha[1]) < 0xfa) {
        s.alpha[0] = i8(s.alpha[0]) + '\x05';
        s.alpha[1] = i8(s.alpha[1]) + '\x05';
      }
      else {
        s.alpha[0] = 0xff;
        s.alpha[1] = 0xff;
      }
    }
    if (0x168 < i16(wait)) {
      if (u8(s.alpha[0]) < 5) {
        s.alpha[0] = 0;
        s.alpha[1] = 0;
        s.alpha[2] = 0;
      }
      else {
        s.alpha[0] = i8(s.alpha[0]) + -5;
        s.alpha[1] = i8(s.alpha[1]) + -5;
        s.alpha[2] = i8(s.alpha[2]) + -5;
      }
    }
    if (i16(wait) == 0x1c2) {
      s.step = u8(0x3d);
      wait = 0;
    }
    else {
      wait = i16(wait) + 1;
    }
    break;
  case 0x3d:
    env.camera.override_view(1,0.0f,(float)(0x140 - i16(wait)),320.0f);
    wait = i16(wait) + 1;
    if (0x13f < i16(wait)) {
      s.step = u8(0x3e);
      wait = 0;
    }
    break;
  case 0x3e:
    env.camera.override_view(1,0.0f,0.0f,320.0f);
    events.begin_dialogue(0x11);
    s.step = u8(0x3f);
    break;
  case 0x3f:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (i16(wait) == 0) {
        events.hide_portrait(a);
        events.hide_portrait(other);
      }
      if (i16(wait) < 0x79) {
        wait = i16(wait) + 1;
      }
      else {
        s.alpha[3] = 0;
        a.scriptCursor.waiting = 0;
        events.show_dialogue(0);
        a.dialogueFinished = 1;
        env.camera.override_view(0,0.0f,0.0f,0.0f);
        s.step = u8(0x40);
        wait = 0;
        env.fadeAlpha = 0;
      }
    }
    break;
  case 0x40:
    env.fadeColor = 0xffffff;
    if (env.fadeAlpha < 0xff) {
      env.fadeAlpha = env.fadeAlpha + 1;
    }
    else {
      s.nextStage = 10;
      a.dialogueFinished = 1;
    }
    break;
  case 0x46:
    s.x[0] = 0.0f;
    s.y[0] = 0.0f;
    s.x[1] = 0.0f;
    s.y[1] = 0.0f;
    s.x[2] = 0.0f;
    s.y[2] = 0.0f;
    s.x[3] = 0.0f;
    s.y[3] = 0.0f;
    env.fadeAlpha = 0;
    events.begin_dialogue(0x6b);
    events.show_dialogue(0);
    events.advance_dialogue();
    events.advance_dialogue();
    s.step = u8(0x47);
    break;
  case 0x47:
    if (u8(s.alpha[0]) < 0xfb) {
      s.alpha[0] = i8(s.alpha[0]) + '\x05';
    }
    else {
      s.alpha[0] = 0xff;
    }
    if (i8(s.alpha[0]) == -1) {
      s.step = u8(0x48);
      wait = 0;
    }
    break;
  case 0x48:
    events.begin_dialogue(0x14);
    s.step = u8(0x49);
    break;
  case 0x49:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (u8(s.alpha[0]) < 6) {
        s.alpha[0] = 0;
      }
      else {
        s.alpha[0] = i8(s.alpha[0]) + -5;
      }
      if (i8(s.alpha[0]) == '\0') {
        events.show_dialogue(0);
        a.dialogueFinished = 1;
      }
    }
    break;
  case 0x50:
    s.x[0] = 0.0f;
    s.y[0] = 0.0f;
    s.x[1] = 0.0f;
    s.y[1] = 0.0f;
    s.x[2] = 0.0f;
    s.y[2] = 0.0f;
    s.x[3] = 0.0f;
    s.y[3] = 0.0f;
    env.fadeAlpha = 0;
    events.begin_dialogue(0x6c);
    events.show_dialogue(0);
    events.advance_dialogue();
    events.advance_dialogue();
    s.step = u8(0x51);
    break;
  case 0x51:
    if (u8(s.alpha[0]) < 0xfb) {
      s.alpha[0] = i8(s.alpha[0]) + '\x05';
    }
    else {
      s.alpha[0] = 0xff;
    }
    if (i8(s.alpha[0]) == -1) {
      s.step = u8(0x52);
      wait = 0;
    }
    break;
  case 0x52:
    events.begin_dialogue(0x15);
    s.step = u8(0x53);
    break;
  case 0x53:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (u8(s.alpha[0]) < 6) {
        s.alpha[0] = 0;
      }
      else {
        s.alpha[0] = i8(s.alpha[0]) + -5;
      }
      if (i8(s.alpha[0]) == '\0') {
        events.show_dialogue(0);
        a.dialogueFinished = 1;
      }
    }
    break;
  case 0x5a:
    s.x[0] = 0.0f;
    s.y[0] = 0.0f;
    s.x[1] = 0.0f;
    s.y[1] = 0.0f;
    s.x[2] = 0.0f;
    s.y[2] = 0.0f;
    s.x[3] = 0.0f;
    s.y[3] = 0.0f;
    env.fadeAlpha = 0;
    events.begin_dialogue(0x6d);
    events.show_dialogue(0);
    events.advance_dialogue();
    events.advance_dialogue();
    s.step = u8(0x5b);
    break;
  case 0x5b:
    s.step = u8(0x5c);
    wait = 0;
    break;
  case 0x5c:
    events.begin_dialogue(0x16);
    s.step = u8(0x5d);
    wait = 0;
    break;
  case 0x5d:
    env.fadeColor = 0;
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (i16(wait) == 0) {
        if (env.fadeAlpha < 0xfa) {
          env.fadeAlpha = env.fadeAlpha + 5;
        }
        else {
          env.fadeAlpha = 0xff;
          wait = 1;
        }
      }
      if (i16(wait) == 1) {
        events.show_dialogue(0);
        events.replace_text(false);
        s.alpha[0] = 0;
        s.alpha[1] = 0xff;
        wait = 2;
      }
      if (i16(wait) == 2) {
        if (env.fadeAlpha < 6) {
          env.fadeAlpha = 0;
          wait = 0;
          s.step = u8(0x5e);
        }
        else {
          env.fadeAlpha = env.fadeAlpha - 5;
        }
      }
    }
    break;
  case 0x5e:
    events.begin_dialogue(0x17);
    s.step = u8(0x5f);
    wait = 0;
    break;
  case 0x5f:
    env.fadeColor = 0;
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      if (i16(wait) == 0) {
        if (env.fadeAlpha < 0xfa) {
          env.fadeAlpha = env.fadeAlpha + 5;
        }
        else {
          env.fadeAlpha = 0xff;
          wait = 1;
        }
      }
      if (i16(wait) == 1) {
        events.show_dialogue(0);
        events.replace_text(true);
        s.alpha[1] = 0;
        s.alpha[2] = 0xff;
        wait = 2;
      }
      if (i16(wait) == 2) {
        if (env.fadeAlpha < 6) {
          env.fadeAlpha = 0;
          wait = 0;
          s.step = u8(0x60);
        }
        else {
          env.fadeAlpha = env.fadeAlpha - 5;
        }
      }
    }
    break;
  case 0x60:
    events.begin_dialogue(0x18);
    s.step = u8(0x61);
    break;
  case 0x61:
    dialogueReady = events.advance_dialogue();
    if (dialogueReady != '\0') {
      env.fadeColor = 0xffffff;
      dialogueReady = events.advance_dialogue();
      if (dialogueReady != '\0') {
        if (i16(wait) == 0) {
          events.stop_music();
          if (env.fadeAlpha < 0xfc) {
            env.fadeAlpha = env.fadeAlpha + 3;
          }
          else {
            env.fadeAlpha = 0xff;
            wait = 1;
          }
        }
        if (0 < i16(wait)) {
          wait = i16(wait) + 1;
        }
        if (0x3c < i16(wait)) {
          events.show_dialogue(0);
          a.dialogueFinished = 1;
        }
      }
    }
  }
  if (i16(a.scriptCursor.musicDelay) == 0) {
    events.select_music((int)i16(a.scriptCursor.musicId));
    events.start_music();
  }
  if (-1 < i16(a.scriptCursor.musicDelay)) {
    a.scriptCursor.musicDelay = i16(a.scriptCursor.musicDelay) + -1;
  }
  events.advance_portrait(a);
  events.advance_portrait(other);
  return;

}
}
