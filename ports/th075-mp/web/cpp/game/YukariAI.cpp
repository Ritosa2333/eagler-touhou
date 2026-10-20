#include "YukariState.hpp"
#include <cmath>
namespace th075 {
void choose_yukari_duel_input(FighterActor& a,const FighterActor& other,const YukariState& state,ReplayRandom& random,CharacterAIContext& context){
  float dx;
  float dy;
  i8 hurt;
  u16 duration;
  int roll;
  u32 direction;
  double distance;
  u32 local_34;
  u32 local_30;
  int local_2c;
  
  distance = (double)std::abs(float(double(other.motion.x) -
                                double(a.motion.x)));
  dx = (float)distance;
  distance = (double)std::abs(float(double(other.motion.y) -
                                double(a.motion.y)));
  dy = (float)distance;
  a.commandTimers[0] = i16(a.commandTimers[0]) + -1;
  if ((0x31 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 0x96)) {
    a.commandTimers[1] = 0;
    a.commandTimers[0] = 0;
  }
  if (i16(a.commandTimers[1]) == 1) {
    if (0 < i16(a.commandTimers[0])) {
      a.controls.current.buttons[2] = 5;
      return;
    }
    a.commandTimers[1] = 0;
    return;
  }
  if (i16(a.commandTimers[1]) == 2) {
    if (i16(a.animation.cursor.action) == 0x187) {
      a.commandTimers[1] = 0;
      return;
    }
    if (i16(a.animation.cursor.action) == 200) {
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      a.controls.current.buttons[1] = 1;
      return;
    }
    if (i16(a.animation.cursor.action) != 200) {
      a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != '\x01'));
      return;
    }
  }
  if (i16(a.commandTimers[1]) == 3) {
    if (i16(a.animation.cursor.action) == 0x186) {
      a.commandTimers[1] = 0;
      return;
    }
    if (i16(a.animation.cursor.action) == 200) {
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      a.controls.current.buttons[0] = 1;
      return;
    }
    if (i16(a.animation.cursor.action) != 200) {
      a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != '\x01'));
      return;
    }
  }
  if (i16(a.commandTimers[1]) == 4) {
    if (i16(a.animation.cursor.action) == 0x188) {
      a.commandTimers[1] = 0;
      return;
    }
    if (i16(a.animation.cursor.action) == 200) {
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      a.controls.current.buttons[0] = 1;
      a.controls.current.vertical = 1;
      return;
    }
    if (i16(a.animation.cursor.action) != 200) {
      a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != '\x01'));
      return;
    }
  }
  if (i16(a.commandTimers[1]) == 5) {
    if (i16(a.animation.cursor.action) == 0x189) {
      a.commandTimers[1] = 0;
      return;
    }
    if (i16(a.animation.cursor.action) == 200) {
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      a.controls.current.buttons[1] = 1;
      a.controls.current.vertical = 1;
      return;
    }
    if (i16(a.animation.cursor.action) != 200) {
      a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != '\x01'));
      return;
    }
  }
  if (i16(a.commandTimers[1]) == 6) {
    if (0 < i16(a.commandTimers[0])) {
      a.controls.current.buttons[0] = 5;
      return;
    }
    a.commandTimers[1] = 0;
    return;
  }
  if (double(300.0f) < double(a.combat.spellExtent)) {
    if ((((double(560.0f) < dx) && (roll = random.next(), roll < 3)) && (i8(a.combat.spellEnded) == '\0'))
       && (i16(a.combat.spellTimer) < 1)) {
      a.command = 0x67;
      return;
    }
    if (((i16(other.animation.cursor.action) == 0x62) && (roll = random.next(), roll < 0x14))
       && ((i8(a.combat.spellEnded) == '\0' && (i16(a.combat.spellTimer) < 1)))) {
      a.command = 0x67;
      return;
    }
  }
  if (double(100.0f) < double(a.combat.spellExtent)) {
    if (((i16(a.combat.health) < 4000) && (double(560.0f) < dx)) &&
       ((roll = random.next(), roll < 0x32 &&
        ((i8(a.combat.spellEnded) == '\0' && (i16(a.combat.spellTimer) < 1)))))) {
      a.command = 0x67;
      return;
    }
    if ((((i16(a.combat.health) < 4000) && (roll = random.next(), roll < 5)) &&
        (i8(a.combat.spellEnded) == '\0')) && (i16(a.combat.spellTimer) < 1)) {
      a.command = 0x67;
      return;
    }
    hurt = (other.animation.cursor.action>=50&&other.animation.cursor.action<=149);
    if (((hurt != '\0') && (i16(a.combat.health) < 3000)) &&
       ((i8(a.combat.spellEnded) == '\0' && (i16(a.combat.spellTimer) < 1)))) {
      a.command = 0x67;
      return;
    }
  }
  if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) ||
     ((599 < i16(a.animation.cursor.action) && (i16(a.animation.cursor.action) < 700)))) {
    if (double(0.0f) < dx) {
      if (double(a.motion.y) == double(0.0f)) {
        roll = random.next();
        if (roll < 4000) {
          a.command = 3;
          return;
        }
        roll = random.next();
        if (roll < 4000) {
          a.command = 4;
          return;
        }
        roll = random.next();
        if (roll < 4000) {
          a.command = 5;
          return;
        }
      }
      if (double(0.0f) < double(a.motion.y)) {
        roll = random.next();
        if (roll < 4000) {
          a.command = '\x02' - (i8(a.motion.facing) != -1);
          return;
        }
        roll = random.next();
        if (roll < 4000) {
          a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
          return;
        }
      }
    }
    if ((((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
        (dx < double(300.0f))) && ((500 < i16(a.combat.spirit) && (roll = random.next(), roll < 100))))
    {
      a.command = 0x14;
      return;
    }
    if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
       ((double(600.0f) < dx && ((500 < i16(a.combat.spirit) && (roll = random.next(), roll < 100))))))
    {
      a.command = 0x15;
      return;
    }
    if ((((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
        (dx < double(300.0f))) && ((500 < i16(a.combat.spirit) && (roll = random.next(), roll < 100))))
    {
      a.command = 10;
      return;
    }
    if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
       ((double(600.0f) < dx && ((500 < i16(a.combat.spirit) && (roll = random.next(), roll < 100))))))
    {
      a.command = 0xb;
      return;
    }
    if ((((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
        (dx < double(450.0f))) && (roll = random.next(), roll < 200)) {
      a.command = 0x14;
      return;
    }
    if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
       ((double(300.0f) < dx && ((dx < double(700.0f) && (roll = random.next(), roll < 100)))))) {
      a.command = 0x15;
      return;
    }
  }
  hurt = (other.animation.cursor.action>=50&&other.animation.cursor.action<=149);
  if ((hurt != '\0') &&
     ((i16(other.animation.cursor.action) < 0x5d ||
      (99 < i16(other.animation.cursor.action))))) {
    if (((i16(a.animation.cursor.action) == 0x1f5) ||
        ((i16(a.animation.cursor.action) == 0x135 || (i16(a.animation.cursor.action) == 0x189)))) &&
       ((roll = random.next(), roll < 10000 &&
        ((((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))) &&
          (i16(a.spellChoices[1]) == 1)) && (i8(a.combat.spellPhase) == '\x01')))))) {
      a.command = 0xd;
      return;
    }
    if ((((((i16(a.animation.cursor.action) == 500) || (i16(a.animation.cursor.action) == 0x1f5)) ||
          ((i16(a.animation.cursor.action) == 0x135 ||
           ((i16(a.animation.cursor.action) == 0x134 || (i16(a.animation.cursor.action) == 0x133)))))) ||
         (i16(a.animation.cursor.action) == 0x186)) ||
        ((i16(a.animation.cursor.action) == 0x188 || (i16(a.animation.cursor.action) == 0x189)))) &&
       (((roll = random.next(), roll < 10000 &&
         (((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))) &&
          (i16(a.spellChoices[1]) == 1)))) && (i8(a.combat.spellPhase) == '\x02')))) {
      a.command = 0xd;
      return;
    }
    if (((((i16(a.animation.cursor.action) == 500) || (i16(a.animation.cursor.action) == 0x1f5)) ||
         ((i16(a.animation.cursor.action) == 0x135 ||
          ((i16(a.animation.cursor.action) == 0x136 || (i16(a.animation.cursor.action) == 0x186)))))) ||
        (i16(a.animation.cursor.action) == 0x189)) &&
       ((((roll = random.next(), roll < 10000 && (0 < i16(a.combat.spellTimer))) &&
         (double(100.0f) <= double(a.combat.spellExtent))) &&
        ((i16(a.spellChoices[1]) == 2 && (i8(a.combat.spellPhase) == '\x01')))))) {
      a.command = 0xd;
      return;
    }
    if ((((i16(a.animation.cursor.action) == 0x189) || (i16(a.animation.cursor.action) == 0x188)) ||
        ((i16(a.animation.cursor.action) == 0x1fa ||
         (((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x134)) ||
          (i16(a.animation.cursor.action) == 0x133)))))) &&
       (((roll = random.next(), roll < 10000 && (0 < i16(a.combat.spellTimer))) &&
        ((double(100.0f) <= double(a.combat.spellExtent) &&
         ((i16(a.spellChoices[1]) == 2 && (i8(a.combat.spellPhase) == '\x02')))))))) {
      a.command = 0xd;
      return;
    }
    if ((((((i16(a.animation.cursor.action) == 0x189) || (i16(a.animation.cursor.action) == 0x188)) ||
          (i16(a.animation.cursor.action) == 0x1fa)) ||
         ((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x12f)))) &&
        (roll = random.next(), roll < 10000)) &&
       (((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))) &&
        ((i16(a.spellChoices[1]) == 0 && (i8(a.combat.spellPhase) == '\x01')))))) {
      a.command = 0xd;
      return;
    }
    if (((((i16(a.animation.cursor.action) == 0x189) || (i16(a.animation.cursor.action) == 0x188)) ||
         (i16(a.animation.cursor.action) == 0x1fa)) ||
        ((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x12f)))) &&
       (((roll = random.next(), roll < 10000 &&
         ((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))))) &&
        ((i16(a.spellChoices[1]) == 0 && (i8(a.combat.spellPhase) == '\x02')))))) {
      a.command = 0xd;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 10000)) &&
       (0xfa < i16(a.combat.spirit))) {
      duration = random.range(0,0x2d);
      a.commandTimers[0] = duration;
      a.commandTimers[1] = 6;
      a.controls.current.buttons[2] = 1;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 1000)) &&
       (500 < i16(a.combat.spirit))) {
      a.command = 0x14;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 500)) {
      a.command = 100;
      return;
    }
    if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x133)) && (roll = random.next(), roll < 500)
       ) {
      a.command = 0x65;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 500)) {
      a.command = 0x28;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 200)) {
      a.command = 0x29;
      return;
    }
    if ((((dx < double(100.0f)) && (i16(a.animation.cursor.action) == 0x133)) &&
        (roll = random.next(), roll < 5000)) && (300 < i16(a.combat.spirit))) {
      duration = random.range(0,0x1e);
      a.commandTimers[0] = duration;
      a.commandTimers[1] = 1;
      a.controls.current.buttons[2] = 1;
      return;
    }
    if (((dx < double(200.0f)) &&
        ((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x134)))) &&
       (roll = random.next(), roll < 20000)) {
      a.command = 0x14;
      return;
    }
    if ((((dx < double(100.0f)) &&
         ((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x134)))) &&
        (roll = random.next(), roll < 5000)) && (a.response == 1)) {
      a.command = 0x1f;
      return;
    }
    if (((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 20000)) && (dx < double(80.0f)))
    {
      a.controls.current.buttons[0] = 1;
      return;
    }
    if ((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 100)) {
      a.controls.current.buttons[1] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 10000)) {
      a.controls.current.buttons[0] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 15000)) {
      a.controls.current.buttons[0] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12f) && (roll = random.next(), roll < 0x226)) {
      a.command = 100;
      return;
    }
    if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12f)) && (roll = random.next(), roll < 300)
       ) {
      a.command = 0x65;
      return;
    }
    if (((dx < double(100.0f)) && (i16(a.animation.cursor.action) == 0x12d)) &&
       (roll = random.next(), roll < 1000)) {
      a.command = 0x1e;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12f) && (roll = random.next(), roll < 200)) {
      a.command = 100;
      return;
    }
    if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12f)) && (roll = random.next(), roll < 200)
       ) {
      a.command = 0x65;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 1000)) {
      a.commandTimers[1] = 3;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 1000)) {
      a.commandTimers[1] = 5;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12e) && (roll = random.next(), roll < 15000)) {
      a.controls.current.buttons[0] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12e) && (roll = random.next(), roll < 1000)) {
      a.commandTimers[1] = 3;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12e) && (roll = random.next(), roll < 1000)) {
      a.commandTimers[1] = 5;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12e) && (roll = random.next(), roll < 1000)) {
      a.commandTimers[1] = 4;
      return;
    }
    if (((dx < double(100.0f)) && (i16(a.animation.cursor.action) == 0x12f)) &&
       (roll = random.next(), roll < 15000)) {
      a.controls.current.buttons[1] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if (((dx < double(100.0f)) && (i16(a.animation.cursor.action) == 0x12f)) &&
       ((roll = random.next(), roll < 5000 && (300 < i16(a.combat.spirit))))) {
      duration = random.range(0,0x1e);
      a.commandTimers[0] = duration;
      a.commandTimers[1] = 1;
      a.controls.current.buttons[2] = 1;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 10000)) &&
       (300 < i16(a.combat.spirit))) {
      duration = random.range(0,0x1e);
      a.commandTimers[0] = duration;
      a.commandTimers[1] = 1;
      a.controls.current.buttons[2] = 1;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 10000)) &&
       (300 < i16(a.combat.spirit))) {
      a.command = 10;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 200)) {
      a.command = 0x14;
      return;
    }
    if ((dx < double(60.0f)) && (roll = random.next(), roll < 10000)) {
      a.controls.current.buttons[0] = 1;
      return;
    }
  }
  roll = random.next();
  if ((((float)(int)i16(a.combat.spirit) * double(0.8f) + double(0.0f) <= (float)roll) || (dx <= double(320.0f))) ||
     (i16(a.combat.spirit) < 0x15f)) {
    roll = random.next();
    if ((((float)(int)i16(a.combat.spirit) * double(1.0f) + double(0.0f) <= (float)roll) || (dx <= double(560.0f)))
       || ((i16(a.combat.spirit) < 0xfb || (double(100.0f) <= dy)))) {
      roll = random.next();
      if ((((float)(int)i16(a.combat.spirit) * double(0.5f) + double(0.0f) <= (float)roll) || (dx <= double(560.0f))
          ) || (i16(a.combat.spirit) < 0xfb)) {
        roll = random.next();
        if (roll < 0x32) {
          a.controls.current.vertical = 0xffffffff;
        }
        if (double(a.motion.y) == double(0.0f)) {
          roll = random.next();
          if (roll < 1000) {
            a.command = 3;
            return;
          }
          roll = random.next();
          if (roll < 1000) {
            a.command = 4;
            return;
          }
          if ((double(400.0f) < dx) && (roll = random.next(), roll < 1000)) {
            a.command = 5;
            return;
          }
        }
        if (double(0.0f) < double(a.motion.y)) {
          roll = random.next();
          if (roll < 0x44c) {
            a.command = '\x02' - (i8(a.motion.facing) != -1);
            return;
          }
          roll = random.next();
          if (roll < 0x44c) {
            a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
            return;
          }
        }
        if ((dx <= double(120.0f)) || (roll = random.next(), 99 < roll)) {
          if ((dx <= double(200.0f)) ||
             ((double(a.motion.y) <= double(0.0f) || (roll = random.next(), 0x4f < roll)))) {
            if ((dx <= double(350.0f)) ||
               ((double(a.motion.y) != double(0.0f) || (roll = random.next(), 0x31 < roll)))) {
              if ((double(400.0f) <= dx) || (roll = random.next(), 99 < roll)) {
                if ((double(a.motion.y) == double(0.0f)) &&
                   ((i16(other.animation.cursor.action) < 0x5d ||
                    (99 < i16(other.animation.cursor.action))))) {
                  if ((dx < double(80.0f)) && (roll = random.next(), roll < 5000)) {
                    a.controls.current.buttons[0] = 1;
                    roll = random.next();
                    a.controls.current.vertical = (u32)(20000 < roll);
                    if (double(other.motion.y) <= double(0.0f)) {
                      return;
                    }
                    a.controls.current.vertical = 0;
                    return;
                  }
                  if ((dx < double(120.0f)) && (roll = random.next(), roll < 2000)) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.vertical = 0;
                    return;
                  }
                  if (dx < double(140.0f)) {
                    if ((dx <= double(0.0f)) || (double(200.0f) <= dx)) {
                      local_2c = 0;
                    }
                    else {
                      local_2c = 1;
                    }
                    roll = random.next();
                    if (roll < local_2c * 900 + 100) {
                      a.controls.current.buttons[1] = 1;
                      a.controls.current.vertical = 1;
                      return;
                    }
                  }
                  if ((((dx < double(120.0f)) && (double(60.0f) < dx)) &&
                      (double(other.motion.y) == double(0.0f))) &&
                     (roll = random.next(), roll < 100)) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.horizontal = (int)i8(a.motion.facing);
                    return;
                  }
                  if (((double(60.0f) < dx) && (dx < double(300.0f))) && (roll = random.next(), roll < 400)) {
                    a.controls.current.buttons[0] = 1;
                    a.controls.current.horizontal = (int)i8(a.motion.facing);
                    return;
                  }
                  if (((double(200.0f) < dx) && (dx < double(400.0f))) && (roll = random.next(), roll < 600)) {
                    a.commandTimers[1] = 3;
                    return;
                  }
                  if ((((double(other.motion.y) == double(0.0f)) && (double(100.0f) < dx)) &&
                      (dx < double(500.0f))) && (roll = random.next(), roll < 100)) {
                    a.commandTimers[1] = 2;
                    return;
                  }
                  if (((double(other.motion.y) == double(0.0f)) && (double(200.0f) < dx)) &&
                     ((dx < double(500.0f) && (roll = random.next(), roll < 600)))) {
                    a.commandTimers[1] = 4;
                    return;
                  }
                  if (((double(other.motion.y) == double(0.0f)) && (double(300.0f) < dx)) &&
                     ((dx < double(500.0f) && (roll = random.next(), roll < 0xfa)))) {
                    a.commandTimers[1] = 5;
                    return;
                  }
                }
                if ((double(0.0f) < double(a.motion.y)) &&
                   ((i16(other.animation.cursor.action) < 0x5d ||
                    (99 < i16(other.animation.cursor.action))))) {
                  if ((dx < double(300.0f)) &&
                     (((roll = random.next(), roll < 6000 && (dy < double(200.0f))) && (double(0.0f) < dy)))) {
                    a.controls.current.buttons[0] = 1;
                    return;
                  }
                  if (((dx < double(200.0f)) && (roll = random.next(), roll < 4000)) &&
                     ((dy < double(250.0f) && (double(80.0f) < dy)))) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.vertical = 0;
                    return;
                  }
                }
                roll = random.next();
                if (((0x5db < roll) || (i16(a.combat.spellTimer) < 1)) ||
                   ((double(a.combat.spellExtent) < double(100.0f) ||
                    ((i16(a.spellChoices[1]) != 2 || (i8(a.combat.spellPhase) != '\x01')))))
                   ) {
                  local_30 = (u32)(double(0.0f) < double(other.motion.y));
                  roll = random.next();
                  if (((int)(local_30 * 0x5dc + 1000) <= roll) ||
                     ((((double(350.0f) <= dx || (i16(a.combat.spellTimer) < 1)) ||
                       (double(a.combat.spellExtent) < double(100.0f))) ||
                      ((i16(a.spellChoices[1]) != 1 || (i8(a.combat.spellPhase) != '\x01')))
                      ))) {
                    roll = random.next();
                    if ((((0x5db < roll) ||
                         ((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f)))
                         )) || (i16(a.spellChoices[1]) != 0)) ||
                       (i8(a.combat.spellPhase) != '\x01')) {
                      roll = random.next();
                      if ((((499 < roll) || (double(300.0f) <= dx)) || (i16(a.combat.spellTimer) < 1))
                         || (((double(a.combat.spellExtent) < double(100.0f) ||
                              (i16(a.spellChoices[0]) != 2)) ||
                             (i8(a.combat.spellPhase) != '\x02')))) {
                        roll = random.next();
                        if (((0x5db < roll) || (i16(a.combat.spellTimer) < 1)) ||
                           ((double(a.combat.spellExtent) < double(100.0f) ||
                            ((i16(a.spellChoices[0]) != 0 ||
                             (i8(a.combat.spellPhase) != '\x02')))))) {
                          local_34 = (u32)(double(0.0f) < double(other.motion.y));
                          roll = random.next();
                          if (((int)(local_34 * 0x5dc + 500) <= roll) ||
                             ((((double(200.0f) <= dx || (i16(a.combat.spellTimer) < 1)) ||
                               (double(a.combat.spellExtent) < double(100.0f))) ||
                              ((i16(a.spellChoices[0]) != 1 ||
                               (i8(a.combat.spellPhase) != '\x02')))))) {
                            if ((((double(150.0f) <= dy) ||
                                 ((double(100.0f) <= dx || (roll = random.next(), 599 < roll)))) ||
                                (i16(other.animation.cursor.action) == 0x5d)) ||
                               (i16(other.animation.cursor.action) == 0x5e)) {
                              if (((dx <= double(100.0f)) || (double(300.0f) <= dx)) ||
                                 (roll = random.next(), 199 < roll)) {
                                if (((dx <= double(400.0f)) || (double(500.0f) <= dx)) ||
                                   ((double(other.motion.y) != double(0.0f) ||
                                    (roll = random.next(), 199 < roll)))) {
                                  if (((dx <= double(350.0f)) || (roll = random.next(), 199 < roll)) ||
                                     (i16(a.combat.spirit) < 0x15f)) {
                                    if (((dx <= double(350.0f)) || (roll = random.next(), 199 < roll)) ||
                                       (i16(a.combat.spirit) < 0x15f)) {
                                      roll = random.next();
                                      if (((roll < 0xfa) &&
                                          (i16(other.animation.cursor.action) != 0x5d)) &&
                                         (i16(other.animation.cursor.action) != 0x5e)) {
                                        a.command = 0x28;
                                      }
                                      else {
                                        roll = random.next();
                                        if (((roll < 100) &&
                                            (i16(other.animation.cursor.action) != 0x5d))
                                           && (i16(other.animation.cursor.action) != 0x5e)
                                           ) {
                                          a.command = 0x29;
                                        }
                                        else if (((((double(250.0f) <= dy) || (dy <= double(100.0f))) ||
                                                  (double(200.0f) <= dx)) ||
                                                 ((roll = random.next(), 99 < roll ||
                                                  (i16(other.animation.cursor.action) ==
                                                   0x5d)))) ||
                                                (i16(other.animation.cursor.action) ==
                                                 0x5e)) {
                                          if (((double(150.0f) <= dy) || (dx <= double(200.0f))) ||
                                             (roll = random.next(),
                                             (float)(int)i16(a.combat.spirit) * double(0.1f) <=
                                             (float)roll)) {
                                            if (((double(150.0f) <= dy) || (dx <= double(400.0f))) ||
                                               (roll = random.next(),
                                               (float)(int)i16(a.combat.spirit) * double(0.1f) <=
                                               (float)roll)) {
                                              if (context.duelWalkFrames < 1) {
                                                if (context.duelWalkFrames < 0) {
                                                  a.controls.current.horizontal = 0xffffffff;
                                                  context.duelWalkFrames = context.duelWalkFrames + 1;
                                                }
                                                else {
                                                  roll = random.next();
                                                  direction = random.next();
                                                  direction = direction & 0x80000001;
                                                  if ((int)direction < 0) {
                                                    direction = (direction - 1 | 0xfffffffe) + 1;
                                                  }
                                                  context.duelWalkFrames = (roll % 0x5a + 10) *
                                                                 (direction * -2 + 1);
                                                }
                                              }
                                              else {
                                                a.controls.current.horizontal = 1;
                                                context.duelWalkFrames = context.duelWalkFrames + -1;
                                              }
                                            }
                                            else {
                                              a.command = 0x15;
                                            }
                                          }
                                          else {
                                            a.command = 0x14;
                                          }
                                        }
                                        else {
                                          a.command = 0x1f;
                                        }
                                      }
                                    }
                                    else {
                                      a.command = 0xb;
                                    }
                                  }
                                  else {
                                    a.command = 10;
                                  }
                                }
                                else {
                                  a.command = 0x65;
                                }
                              }
                              else {
                                a.command = 100;
                              }
                            }
                            else {
                              a.command = 0x1e;
                            }
                          }
                          else {
                            a.command = 0xd;
                          }
                        }
                        else {
                          a.command = 0xd;
                        }
                      }
                      else {
                        a.command = 0xd;
                      }
                    }
                    else {
                      a.command = 0xd;
                    }
                  }
                  else {
                    a.command = 0xd;
                  }
                }
                else {
                  a.command = 0xd;
                }
              }
              else {
                a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
                a.command = u8(a.command);
              }
            }
            else {
              a.command = '\x02' - (i8(a.motion.facing) != -1);
              a.command = u8(a.command);
            }
          }
          else {
            a.command = '\x02' - (i8(a.motion.facing) != -1);
            a.command = u8(a.command);
          }
        }
        else {
          a.controls.current.horizontal = (int)i8(a.motion.facing);
          roll = random.next();
          if (roll < 18000) {
            a.controls.current.horizontal = 0;
          }
          a.controls.current.vertical = 0xffffffff;
        }
      }
      else {
        a.controls.current.buttons[1] = 1;
      }
    }
    else {
      a.commandTimers[1] = 6;
      duration = random.range(0,0x2d);
      a.commandTimers[0] = duration;
      a.controls.current.buttons[0] = 1;
      roll = random.next();
      if (roll < 2000) {
        a.controls.current.horizontal = (int)i8(a.motion.facing);
      }
      roll = random.next();
      if (roll < 2000) {
        a.controls.current.vertical = 1;
      }
    }
  }
  else {
    duration = random.range(0,0x2d);
    a.commandTimers[0] = duration;
    a.commandTimers[1] = 1;
    a.controls.current.buttons[2] = 1;
    roll = random.next();
    if (roll < 1000) {
      a.controls.current.horizontal = (int)i8(a.motion.facing);
    }
    roll = random.next();
    if (roll < 1000) {
      a.controls.current.vertical = 1;
    }
  }
  return;
}
void choose_yukari_story_input(FighterActor& a,const FighterActor& other,const YukariState& state,ReplayRandom& random,CharacterAIContext& context){
  float dx;
  float dy;
  i8 hurt;
  u16 duration;
  int roll;
  u32 direction;
  double distance;
  int local_2c;
  
  distance = (double)std::abs(float(double(other.motion.x) -
                                double(a.motion.x)));
  dx = (float)distance;
  distance = (double)std::abs(float(double(other.motion.y) -
                                double(a.motion.y)));
  dy = (float)distance;
  a.commandTimers[0] = i16(a.commandTimers[0]) + -1;
  if (((i16(a.animation.cursor.action) < 0x32) || (0x95 < i16(a.animation.cursor.action))) ||
     (i8(a.phaseVariants[a.combat.spellPhase]) == '\x04')) {
    if ((i8(a.phaseVariants[a.combat.spellPhase]) == '\x04') &&
       (0 < i16(a.combat.spellTimer))) {
      roll = random.range(0,100);
      if (roll < 0x4b) {
        a.command = 0xde;
      }
      else {
        a.command = 0xfa;
      }
    }
    else if ((((i8(a.phaseVariants[a.combat.spellPhase]) == '\x03') &&
              ((0 < i16(a.combat.spellTimer) &&
               (a.commandTimers[2] = i16(a.commandTimers[2]) + -1,
               i16(a.commandTimers[2]) < 1)))) && (double(a.motion.y) == double(0.0f))) &&
            (0xdab < i16(a.combat.health))) {
      a.command = 0xe6;
    }
    else {
      if ((i8(a.phaseVariants[a.combat.spellPhase]) == '\x02') &&
         (0 < i16(a.combat.spellTimer))) {
        a.commandTimers[2] = i16(a.commandTimers[2]) + -1;
        roll = random.range(0,100);
        if ((roll < 0x32) &&
           ((i16(a.commandTimers[2]) < 1 && (double(a.motion.y) == double(0.0f))))) {
          a.command = 0xdd;
          if (state.difficulty != '\0') {
            return;
          }
          a.command = 0xde;
          return;
        }
      }
      if ((i8(a.phaseVariants[a.combat.spellPhase]) == '\x01') &&
         (0 < i16(a.combat.spellTimer))) {
        a.commandTimers[2] = i16(a.commandTimers[2]) + -1;
        roll = random.range(0,100);
        if ((roll < 100) &&
           ((i16(a.commandTimers[2]) < 1 && (double(a.motion.y) == double(0.0f))))) {
          hurt = random.range(0,3);
          a.command = hurt + -0x2e;
          return;
        }
      }
      if ((i8(a.phaseVariants[a.combat.spellPhase]) == '\0') &&
         (0 < i16(a.combat.spellTimer))) {
        a.commandTimers[2] = i16(a.commandTimers[2]) + -1;
        roll = random.range(0,100);
        if ((roll < 100) &&
           ((i16(a.commandTimers[2]) < 1 && (double(a.motion.y) == double(0.0f))))) {
          a.command = 200;
          return;
        }
      }
      if ((i8(a.phaseVariants[a.combat.spellPhase]) == '\x03') ||
         (i8(a.phaseVariants[a.combat.spellPhase]) == '\x04')) {
        roll = random.next();
        if (roll < 0x32) {
          a.controls.current.vertical = 0xffffffff;
        }
        if (double(a.motion.y) == double(0.0f)) {
          roll = random.next();
          if (roll < 1000) {
            a.command = 3;
            return;
          }
          roll = random.next();
          if (roll < 1000) {
            a.command = 4;
            return;
          }
          if ((double(400.0f) < dx) && (roll = random.next(), roll < 1000)) {
            a.command = 5;
            return;
          }
        }
        if (double(0.0f) < double(a.motion.y)) {
          roll = random.next();
          if (roll < 0x44c) {
            a.command = '\x02' - (i8(a.motion.facing) != -1);
            return;
          }
          roll = random.next();
          if (roll < 0x44c) {
            a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
            return;
          }
        }
        if ((dx <= double(120.0f)) || (roll = random.next(), 99 < roll)) {
          if ((dx <= double(200.0f)) ||
             ((double(a.motion.y) <= double(0.0f) || (roll = random.next(), 0x4f < roll)))) {
            if ((dx <= double(350.0f)) ||
               ((double(a.motion.y) != double(0.0f) || (roll = random.next(), 0x31 < roll)))) {
              if ((double(400.0f) <= dx) || (roll = random.next(), 99 < roll)) {
                if (context.storyWalkFrames < 1) {
                  if (context.storyWalkFrames < 0) {
                    a.controls.current.horizontal = 0xffffffff;
                    context.storyWalkFrames = context.storyWalkFrames + 1;
                  }
                  else {
                    roll = random.next();
                    direction = random.next();
                    direction = direction & 0x80000001;
                    if ((int)direction < 0) {
                      direction = (direction - 1 | 0xfffffffe) + 1;
                    }
                    context.storyWalkFrames = (roll % 0x5a + 10) * (direction * -2 + 1);
                  }
                }
                else {
                  a.controls.current.horizontal = 1;
                  context.storyWalkFrames = context.storyWalkFrames + -1;
                }
              }
              else {
                a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
                a.command = u8(a.command);
              }
            }
            else {
              a.command = '\x02' - (i8(a.motion.facing) != -1);
              a.command = u8(a.command);
            }
          }
          else {
            a.command = '\x02' - (i8(a.motion.facing) != -1);
            a.command = u8(a.command);
          }
        }
        else {
          a.controls.current.horizontal = (int)i8(a.motion.facing);
          roll = random.next();
          if (roll < 18000) {
            a.controls.current.horizontal = 0;
          }
          a.controls.current.vertical = 0xffffffff;
        }
      }
      else {
        if (i16(a.commandTimers[1]) == 1) {
          if (0 < i16(a.commandTimers[0])) {
            a.controls.current.buttons[2] = 5;
            return;
          }
          a.commandTimers[1] = 0;
          return;
        }
        if (i16(a.commandTimers[1]) == 2) {
          if (i16(a.animation.cursor.action) == 0x187) {
            a.commandTimers[1] = 0;
            return;
          }
          if (i16(a.animation.cursor.action) == 200) {
            a.controls.current.horizontal = (int)i8(a.motion.facing);
            a.controls.current.buttons[1] = 1;
            return;
          }
          if (i16(a.animation.cursor.action) != 200) {
            a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != '\x01'));
            return;
          }
        }
        if (i16(a.commandTimers[1]) == 3) {
          if (i16(a.animation.cursor.action) == 0x186) {
            a.commandTimers[1] = 0;
            return;
          }
          if (i16(a.animation.cursor.action) == 200) {
            a.controls.current.horizontal = (int)i8(a.motion.facing);
            a.controls.current.buttons[0] = 1;
            return;
          }
          if (i16(a.animation.cursor.action) != 200) {
            a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != '\x01'));
            return;
          }
        }
        if (i16(a.commandTimers[1]) == 4) {
          if (i16(a.animation.cursor.action) == 0x188) {
            a.commandTimers[1] = 0;
            return;
          }
          if (i16(a.animation.cursor.action) == 200) {
            a.controls.current.horizontal = (int)i8(a.motion.facing);
            a.controls.current.buttons[0] = 1;
            a.controls.current.vertical = 1;
            return;
          }
          if (i16(a.animation.cursor.action) != 200) {
            a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != '\x01'));
            return;
          }
        }
        if (i16(a.commandTimers[1]) == 5) {
          if (i16(a.animation.cursor.action) == 0x189) {
            a.commandTimers[1] = 0;
            return;
          }
          if (i16(a.animation.cursor.action) == 200) {
            a.controls.current.horizontal = (int)i8(a.motion.facing);
            a.controls.current.buttons[1] = 1;
            a.controls.current.vertical = 1;
            return;
          }
          if (i16(a.animation.cursor.action) != 200) {
            a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != '\x01'));
            return;
          }
        }
        if (i16(a.commandTimers[1]) == 6) {
          if (i16(a.commandTimers[0]) < 1) {
            a.commandTimers[1] = 0;
          }
          else {
            a.controls.current.buttons[0] = 5;
          }
        }
        else {
          if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) ||
             ((599 < i16(a.animation.cursor.action) && (i16(a.animation.cursor.action) < 700)))) {
            if (double(0.0f) < dx) {
              if (double(a.motion.y) == double(0.0f)) {
                roll = random.next();
                if (roll < 4000) {
                  a.command = 3;
                  return;
                }
                roll = random.next();
                if (roll < 4000) {
                  a.command = 4;
                  return;
                }
                roll = random.next();
                if (roll < 4000) {
                  a.command = 5;
                  return;
                }
              }
              if (double(0.0f) < double(a.motion.y)) {
                roll = random.next();
                if (roll < 4000) {
                  a.command = '\x02' - (i8(a.motion.facing) != -1);
                  return;
                }
                roll = random.next();
                if (roll < 4000) {
                  a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1))
                  ;
                  return;
                }
              }
            }
            if ((((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
                (dx < double(300.0f))) &&
               ((500 < i16(a.combat.spirit) && (roll = random.next(), roll < 100)))) {
              a.command = 0x14;
              return;
            }
            if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
               ((double(600.0f) < dx &&
                ((500 < i16(a.combat.spirit) && (roll = random.next(), roll < 100)))))) {
              a.command = 0x15;
              return;
            }
            if ((((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
                (dx < double(300.0f))) &&
               ((500 < i16(a.combat.spirit) && (roll = random.next(), roll < 100)))) {
              a.command = 10;
              return;
            }
            if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
               ((double(600.0f) < dx &&
                ((500 < i16(a.combat.spirit) && (roll = random.next(), roll < 100)))))) {
              a.command = 0xb;
              return;
            }
            if ((((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
                (dx < double(450.0f))) && (roll = random.next(), roll < 200)) {
              a.command = 0x14;
              return;
            }
            if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
               ((double(300.0f) < dx && ((dx < double(700.0f) && (roll = random.next(), roll < 100)))))) {
              a.command = 0x15;
              return;
            }
          }
          hurt = (other.animation.cursor.action>=50&&other.animation.cursor.action<=149);
          if ((hurt != '\0') &&
             ((i16(other.animation.cursor.action) < 0x5d ||
              (99 < i16(other.animation.cursor.action))))) {
            if ((i16(a.animation.cursor.action) == 0x133) &&
               ((roll = random.next(), roll < 10000 && (0xfa < i16(a.combat.spirit))))) {
              duration = random.range(0,0x2d);
              a.commandTimers[0] = duration;
              a.commandTimers[1] = 6;
              a.controls.current.buttons[2] = 1;
              return;
            }
            if (((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 1000)) &&
               (500 < i16(a.combat.spirit))) {
              a.command = 0x14;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 500)) {
              a.command = 100;
              return;
            }
            if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x133)) &&
               (roll = random.next(), roll < 500)) {
              a.command = 0x65;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 500)) {
              a.command = 0x28;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 200)) {
              a.command = 0x29;
              return;
            }
            if ((((dx < double(100.0f)) && (i16(a.animation.cursor.action) == 0x133)) &&
                (roll = random.next(), roll < 5000)) && (300 < i16(a.combat.spirit))) {
              duration = random.range(0,0x1e);
              a.commandTimers[0] = duration;
              a.commandTimers[1] = 1;
              a.controls.current.buttons[2] = 1;
              return;
            }
            if (((dx < double(200.0f)) &&
                ((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x134)))) &&
               (roll = random.next(), roll < 20000)) {
              a.command = 0x14;
              return;
            }
            if (((dx < double(100.0f)) &&
                ((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x134)))) &&
               ((roll = random.next(), roll < 5000 && (a.response == 1)))) {
              a.command = 0x1f;
              return;
            }
            if (((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 20000)) &&
               (dx < double(80.0f))) {
              a.controls.current.buttons[0] = 1;
              return;
            }
            if ((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 100)) {
              a.controls.current.buttons[1] = 1;
              a.controls.current.horizontal = (int)i8(a.motion.facing);
              return;
            }
            if ((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 10000)) {
              a.controls.current.buttons[0] = 1;
              a.controls.current.horizontal = (int)i8(a.motion.facing);
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 15000)) {
              a.controls.current.buttons[0] = 1;
              a.controls.current.horizontal = (int)i8(a.motion.facing);
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x12f) && (roll = random.next(), roll < 0x226)) {
              a.command = 100;
              return;
            }
            if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12f)) &&
               (roll = random.next(), roll < 300)) {
              a.command = 0x65;
              return;
            }
            if (((dx < double(100.0f)) && (i16(a.animation.cursor.action) == 0x12d)) &&
               (roll = random.next(), roll < 1000)) {
              a.command = 0x1e;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x12f) && (roll = random.next(), roll < 200)) {
              a.command = 100;
              return;
            }
            if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12f)) &&
               (roll = random.next(), roll < 200)) {
              a.command = 0x65;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 1000)) {
              a.commandTimers[1] = 3;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 1000)) {
              a.commandTimers[1] = 5;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x12e) && (roll = random.next(), roll < 15000)) {
              a.controls.current.buttons[0] = 1;
              a.controls.current.horizontal = (int)i8(a.motion.facing);
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x12e) && (roll = random.next(), roll < 1000)) {
              a.commandTimers[1] = 3;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x12e) && (roll = random.next(), roll < 1000)) {
              a.commandTimers[1] = 5;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x12e) && (roll = random.next(), roll < 1000)) {
              a.commandTimers[1] = 4;
              return;
            }
            if (((dx < double(100.0f)) && (i16(a.animation.cursor.action) == 0x12f)) &&
               (roll = random.next(), roll < 15000)) {
              a.controls.current.buttons[1] = 1;
              a.controls.current.horizontal = (int)i8(a.motion.facing);
              return;
            }
            if (((dx < double(100.0f)) && (i16(a.animation.cursor.action) == 0x12f)) &&
               ((roll = random.next(), roll < 5000 && (300 < i16(a.combat.spirit))))) {
              duration = random.range(0,0x1e);
              a.commandTimers[0] = duration;
              a.commandTimers[1] = 1;
              a.controls.current.buttons[2] = 1;
              return;
            }
            if (((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 10000)) &&
               (300 < i16(a.combat.spirit))) {
              duration = random.range(0,0x1e);
              a.commandTimers[0] = duration;
              a.commandTimers[1] = 1;
              a.controls.current.buttons[2] = 1;
              return;
            }
            if (((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 10000)) &&
               (300 < i16(a.combat.spirit))) {
              a.command = 10;
              return;
            }
            if ((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 200)) {
              a.command = 0x14;
              return;
            }
            if ((dx < double(60.0f)) && (roll = random.next(), roll < 10000)) {
              a.controls.current.buttons[0] = 1;
              return;
            }
          }
          roll = random.next();
          if ((((float)(int)i16(a.combat.spirit) * double(0.8f) + double(0.0f) <= (float)roll) ||
              (dx <= double(320.0f))) || (i16(a.combat.spirit) < 0x15f)) {
            roll = random.next();
            if ((((float)(int)i16(a.combat.spirit) * double(1.0f) + double(0.0f) <= (float)roll) ||
                (dx <= double(560.0f))) || ((i16(a.combat.spirit) < 0xfb || (double(100.0f) <= dy)))) {
              roll = random.next();
              if ((((float)(int)i16(a.combat.spirit) * double(0.5f) + double(0.0f) <= (float)roll) ||
                  (dx <= double(560.0f))) || (i16(a.combat.spirit) < 0xfb)) {
                roll = random.next();
                if (roll < 0x32) {
                  a.controls.current.vertical = 0xffffffff;
                }
                if (double(a.motion.y) == double(0.0f)) {
                  roll = random.next();
                  if (roll < 1000) {
                    a.command = 3;
                    return;
                  }
                  roll = random.next();
                  if (roll < 1000) {
                    a.command = 4;
                    return;
                  }
                  if ((double(400.0f) < dx) && (roll = random.next(), roll < 1000)) {
                    a.command = 5;
                    return;
                  }
                }
                if (double(0.0f) < double(a.motion.y)) {
                  roll = random.next();
                  if (roll < 0x44c) {
                    a.command = '\x02' - (i8(a.motion.facing) != -1);
                    return;
                  }
                  roll = random.next();
                  if (roll < 0x44c) {
                    a.command =
                         '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
                    return;
                  }
                }
                if ((dx <= double(120.0f)) || (roll = random.next(), 99 < roll)) {
                  if ((dx <= double(200.0f)) ||
                     ((double(a.motion.y) <= double(0.0f) || (roll = random.next(), 0x4f < roll)))) {
                    if ((dx <= double(350.0f)) ||
                       ((double(a.motion.y) != double(0.0f) || (roll = random.next(), 0x31 < roll)))) {
                      if ((double(400.0f) <= dx) || (roll = random.next(), 99 < roll)) {
                        if ((double(a.motion.y) == double(0.0f)) &&
                           ((i16(other.animation.cursor.action) < 0x5d ||
                            (99 < i16(other.animation.cursor.action))))) {
                          if ((dx < double(80.0f)) && (roll = random.next(), roll < 5000)) {
                            a.controls.current.buttons[0] = 1;
                            roll = random.next();
                            a.controls.current.vertical = (u32)(20000 < roll);
                            if (double(other.motion.y) <= double(0.0f)) {
                              return;
                            }
                            a.controls.current.vertical = 0;
                            return;
                          }
                          if ((dx < double(120.0f)) && (roll = random.next(), roll < 2000)) {
                            a.controls.current.buttons[1] = 1;
                            a.controls.current.vertical = 0;
                            return;
                          }
                          if (dx < double(140.0f)) {
                            if ((dx <= double(0.0f)) || (double(200.0f) <= dx)) {
                              local_2c = 0;
                            }
                            else {
                              local_2c = 1;
                            }
                            roll = random.next();
                            if (roll < local_2c * 900 + 100) {
                              a.controls.current.buttons[1] = 1;
                              a.controls.current.vertical = 1;
                              return;
                            }
                          }
                          if ((((dx < double(120.0f)) && (double(60.0f) < dx)) &&
                              (double(other.motion.y) == double(0.0f))) &&
                             (roll = random.next(), roll < 100)) {
                            a.controls.current.buttons[1] = 1;
                            a.controls.current.horizontal = (int)i8(a.motion.facing);
                            return;
                          }
                          if (((double(60.0f) < dx) && (dx < double(300.0f))) && (roll = random.next(), roll < 400))
                          {
                            a.controls.current.buttons[0] = 1;
                            a.controls.current.horizontal = (int)i8(a.motion.facing);
                            return;
                          }
                          if (((double(200.0f) < dx) && (dx < double(400.0f))) && (roll = random.next(), roll < 600)
                             ) {
                            a.commandTimers[1] = 3;
                            return;
                          }
                          if ((((double(other.motion.y) == double(0.0f)) &&
                               (double(100.0f) < dx)) && (dx < double(500.0f))) &&
                             (roll = random.next(), roll < 100)) {
                            a.commandTimers[1] = 2;
                            return;
                          }
                          if (((double(other.motion.y) == double(0.0f)) &&
                              (double(200.0f) < dx)) &&
                             ((dx < double(500.0f) && (roll = random.next(), roll < 600)))) {
                            a.commandTimers[1] = 4;
                            return;
                          }
                          if (((double(other.motion.y) == double(0.0f)) &&
                              (double(300.0f) < dx)) &&
                             ((dx < double(500.0f) && (roll = random.next(), roll < 0xfa)))) {
                            a.commandTimers[1] = 5;
                            return;
                          }
                        }
                        if ((double(0.0f) < double(a.motion.y)) &&
                           ((i16(other.animation.cursor.action) < 0x5d ||
                            (99 < i16(other.animation.cursor.action))))) {
                          if ((dx < double(300.0f)) &&
                             (((roll = random.next(), roll < 6000 && (dy < double(200.0f))) && (double(0.0f) < dy)))
                             ) {
                            a.controls.current.buttons[0] = 1;
                            return;
                          }
                          if (((dx < double(200.0f)) && (roll = random.next(), roll < 4000)) &&
                             ((dy < double(250.0f) && (double(80.0f) < dy)))) {
                            a.controls.current.buttons[1] = 1;
                            a.controls.current.vertical = 0;
                            return;
                          }
                        }
                        if (((double(150.0f) <= dy) || (double(100.0f) <= dx)) ||
                           ((roll = random.next(), 599 < roll ||
                            ((i16(other.animation.cursor.action) == 0x5d ||
                             (i16(other.animation.cursor.action) == 0x5e)))))) {
                          if ((dx <= double(100.0f)) ||
                             ((double(300.0f) <= dx || (roll = random.next(), 199 < roll)))) {
                            if ((dx <= double(400.0f)) ||
                               (((double(500.0f) <= dx ||
                                 (double(other.motion.y) != double(0.0f))) ||
                                (roll = random.next(), 199 < roll)))) {
                              if (((dx <= double(350.0f)) || (roll = random.next(), 199 < roll)) ||
                                 (i16(a.combat.spirit) < 0x15f)) {
                                if (((dx <= double(350.0f)) || (roll = random.next(), 199 < roll)) ||
                                   (i16(a.combat.spirit) < 0x15f)) {
                                  roll = random.next();
                                  if (((roll < 0xfa) &&
                                      (i16(other.animation.cursor.action) != 0x5d)) &&
                                     (i16(other.animation.cursor.action) != 0x5e)) {
                                    a.command = 0x28;
                                  }
                                  else {
                                    roll = random.next();
                                    if (((roll < 100) &&
                                        (i16(other.animation.cursor.action) != 0x5d)) &&
                                       (i16(other.animation.cursor.action) != 0x5e)) {
                                      a.command = 0x29;
                                    }
                                    else if (((double(250.0f) <= dy) || (dy <= double(100.0f))) ||
                                            ((double(200.0f) <= dx ||
                                             (((roll = random.next(), 99 < roll ||
                                               (i16(other.animation.cursor.action) == 0x5d)) || (i16(other.animation.cursor.action)
                                                      == 0x5e)))))) {
                                      if (((double(150.0f) <= dy) || (dx <= double(200.0f))) ||
                                         (roll = random.next(),
                                         (float)(int)i16(a.combat.spirit) * double(0.1f) <=
                                         (float)roll)) {
                                        if (((double(150.0f) <= dy) || (dx <= double(400.0f))) ||
                                           (roll = random.next(),
                                           (float)(int)i16(a.combat.spirit) * double(0.1f) <=
                                           (float)roll)) {
                                          if (context.storyWalkFrames < 1) {
                                            if (context.storyWalkFrames < 0) {
                                              a.controls.current.horizontal = 0xffffffff;
                                              context.storyWalkFrames = context.storyWalkFrames + 1;
                                            }
                                            else {
                                              roll = random.next();
                                              direction = random.next();
                                              direction = direction & 0x80000001;
                                              if ((int)direction < 0) {
                                                direction = (direction - 1 | 0xfffffffe) + 1;
                                              }
                                              context.storyWalkFrames = (roll % 0x5a + 10) * (direction * -2 + 1);
                                            }
                                          }
                                          else {
                                            a.controls.current.horizontal = 1;
                                            context.storyWalkFrames = context.storyWalkFrames + -1;
                                          }
                                        }
                                        else {
                                          a.command = 0x15;
                                        }
                                      }
                                      else {
                                        a.command = 0x14;
                                      }
                                    }
                                    else {
                                      a.command = 0x1f;
                                    }
                                  }
                                }
                                else {
                                  a.command = 0xb;
                                }
                              }
                              else {
                                a.command = 10;
                              }
                            }
                            else {
                              a.command = 0x65;
                            }
                          }
                          else {
                            a.command = 100;
                          }
                        }
                        else {
                          a.command = 0x1e;
                        }
                      }
                      else {
                        a.command =
                             '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
                        a.command = u8(a.command);
                      }
                    }
                    else {
                      a.command = '\x02' - (i8(a.motion.facing) != -1);
                      a.command = u8(a.command);
                    }
                  }
                  else {
                    a.command = '\x02' - (i8(a.motion.facing) != -1);
                    a.command = u8(a.command);
                  }
                }
                else {
                  a.controls.current.horizontal = (int)i8(a.motion.facing);
                  roll = random.next();
                  if (roll < 18000) {
                    a.controls.current.horizontal = 0;
                  }
                  a.controls.current.vertical = 0xffffffff;
                }
              }
              else {
                a.controls.current.buttons[1] = 1;
              }
            }
            else {
              a.commandTimers[1] = 6;
              duration = random.range(0,0x2d);
              a.commandTimers[0] = duration;
              a.controls.current.buttons[0] = 1;
              roll = random.next();
              if (roll < 2000) {
                a.controls.current.horizontal = (int)i8(a.motion.facing);
              }
              roll = random.next();
              if (roll < 2000) {
                a.controls.current.vertical = 1;
              }
            }
          }
          else {
            duration = random.range(0,0x2d);
            a.commandTimers[0] = duration;
            a.commandTimers[1] = 1;
            a.controls.current.buttons[2] = 1;
            roll = random.next();
            if (roll < 1000) {
              a.controls.current.horizontal = (int)i8(a.motion.facing);
            }
            roll = random.next();
            if (roll < 1000) {
              a.controls.current.vertical = 1;
            }
          }
        }
      }
    }
  }
  else {
    if (i16(a.commandTimers[2]) < 0) {
      duration = random.range(0x1e,0x5a);
      a.commandTimers[2] = duration;
    }
    a.commandTimers[1] = 0;
    a.commandTimers[0] = 0;
    roll = random.range(0,100);
    if (roll < 0x32) {
      a.controls.current.horizontal = 1;
    }
    else {
      a.controls.current.horizontal = 0xffffffff;
    }
    roll = random.range(0,100);
    if (roll < 5) {
      a.controls.current.buttons[0] = 1;
    }
  }
  return;
}
}
