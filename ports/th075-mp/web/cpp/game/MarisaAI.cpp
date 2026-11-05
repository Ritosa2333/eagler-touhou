#include "MarisaState.hpp"
#include <cmath>
namespace th075 {
void choose_marisa_duel_input(FighterActor& a,const FighterActor& other,const MarisaState& state,ReplayRandom& random,CharacterAIContext& context){

  float dx;
  float dy;
  i8 hurt;
  u16 duration;
  int roll;
  u32 direction;
  double distance;
  
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
      a.command = 0x32;
      return;
    }
    if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
       ((double(600.0f) < dx &&
        (((dx < double(900.0f) && (500 < i16(a.combat.spirit))) && (roll = random.next(), roll < 100)))
        ))) {
      a.command = 0x33;
      return;
    }
    if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
       ((dx < double(450.0f) && (roll = random.next(), roll < 200)))) {
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
    if (((i16(a.animation.cursor.action) == 500) ||
        ((i16(a.animation.cursor.action) == 0x1f5 || (i16(a.animation.cursor.action) == 0x135)))) &&
       (((roll = random.next(), roll < 10000 &&
         (((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))) &&
          (i16(a.spellChoices[1]) == 1)))) && (i8(a.combat.spellPhase) == '\x01')))) {
      a.command = 0xd;
      return;
    }
    if (((((((i16(a.animation.cursor.action) == 500) || (i16(a.animation.cursor.action) == 0x1f5)) ||
           ((i16(a.animation.cursor.action) == 0x135 ||
            ((i16(a.animation.cursor.action) == 0x134 || (i16(a.animation.cursor.action) == 0x133)))))) ||
          (i16(a.animation.cursor.action) == 0x20a)) ||
         (((i16(a.animation.cursor.action) == 0x20b || (i16(a.animation.cursor.action) == 0x20c)) ||
          (i16(a.animation.cursor.action) == 0x20d)))) &&
        ((roll = random.next(), roll < 10000 && (0 < i16(a.combat.spellTimer))))) &&
       ((double(100.0f) <= double(a.combat.spellExtent) &&
        ((i16(a.spellChoices[1]) == 1 && (i8(a.combat.spellPhase) == '\x02')))))) {
      a.command = 0xd;
      return;
    }
    if (((((((i16(a.animation.cursor.action) == 500) || (i16(a.animation.cursor.action) == 0x1f5)) ||
           (i16(a.animation.cursor.action) == 0x135)) ||
          ((i16(a.animation.cursor.action) == 0x136 || (i16(a.animation.cursor.action) == 0x186)))) ||
         (i16(a.animation.cursor.action) == 0x188)) &&
        ((roll = random.next(), roll < 10000 && (0 < i16(a.combat.spellTimer))))) &&
       ((double(100.0f) <= double(a.combat.spellExtent) &&
        ((i16(a.spellChoices[1]) == 2 && (i8(a.combat.spellPhase) == '\x01')))))) {
      if (i16(state.chargeCount) == 0) {
        a.command = 0xd;
        return;
      }
      roll = random.next();
      if (roll < 15000) {
        a.command = 0xd;
      }
      roll = random.next();
      if (14999 < roll) {
        return;
      }
      a.command = 0x17;
      return;
    }
    if ((((((i16(a.animation.cursor.action) == 500) || (i16(a.animation.cursor.action) == 0x1f5)) ||
          (i16(a.animation.cursor.action) == 0x135)) ||
         (((i16(a.animation.cursor.action) == 0x136 || (i16(a.animation.cursor.action) == 0x186)) ||
          (i16(a.animation.cursor.action) == 0x188)))) &&
        ((roll = random.next(), roll < 10000 && (0 < i16(a.combat.spellTimer))))) &&
       ((double(100.0f) <= double(a.combat.spellExtent) &&
        ((i16(a.spellChoices[1]) == 2 && (i8(a.combat.spellPhase) == '\x02')))))) {
      if (i16(state.chargeCount) == 0) {
        a.command = 0xd;
        return;
      }
      roll = random.next();
      if (roll < 15000) {
        a.command = 0xd;
      }
      roll = random.next();
      if (14999 < roll) {
        return;
      }
      a.command = 0x17;
      return;
    }
    if (((((i16(a.animation.cursor.action) == 500) || (i16(a.animation.cursor.action) == 0x1f5)) ||
         (i16(a.animation.cursor.action) == 0x135)) || (i16(a.animation.cursor.action) == 0x134)) &&
       (((roll = random.next(), roll < 10000 && (0 < i16(a.combat.spellTimer))) &&
        ((double(100.0f) <= double(a.combat.spellExtent) &&
         ((i16(a.spellChoices[1]) == 0 && (i8(a.combat.spellPhase) == '\x01')))))))) {
      a.command = 0xd;
      return;
    }
    if (((((i16(a.animation.cursor.action) == 500) || (i16(a.animation.cursor.action) == 0x1f5)) ||
         (i16(a.animation.cursor.action) == 0x135)) || (i16(a.animation.cursor.action) == 0x134)) &&
       (((roll = random.next(), roll < 10000 && (0 < i16(a.combat.spellTimer))) &&
        ((double(100.0f) <= double(a.combat.spellExtent) &&
         ((i16(a.spellChoices[1]) == 0 && (i8(a.combat.spellPhase) == '\x02')))))))) {
      a.command = 0xd;
      return;
    }
    if ((dx < double(60.0f)) && (roll = random.next(), roll < 10000)) {
      a.controls.current.buttons[0] = 1;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 10000)) &&
       (0xfa < i16(a.combat.spirit))) {
      duration = random.range(0,0x2d);
      a.commandTimers[0] = duration;
      a.commandTimers[1] = 1;
      a.controls.current.buttons[2] = 1;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 10000)) {
      a.command = 0x14;
      return;
    }
    if (((dx < double(200.0f)) &&
        ((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x134)))) &&
       (roll = random.next(), roll < 20000)) {
      a.command = 0x14;
      return;
    }
    if (((dx < double(100.0f)) &&
        (((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x134)) &&
         (roll = random.next(), roll < 5000)))) && (a.response == 1)) {
      a.command = 0x1f;
      return;
    }
    if (((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 20000)) && (dx < double(80.0f)))
    {
      a.controls.current.buttons[0] = 1;
      return;
    }
    if ((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 20000)) {
      a.controls.current.buttons[1] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 1000)) {
      a.controls.current.buttons[0] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 1000)) {
      a.controls.current.buttons[0] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 500)) {
      a.command = 100;
      return;
    }
    if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12d)) && (roll = random.next(), roll < 500)
       ) {
      a.command = 0x65;
      return;
    }
    if (((dx < double(150.0f)) && (i16(a.animation.cursor.action) == 0x12d)) &&
       (roll = random.next(), roll < 10000)) {
      a.controls.current.buttons[1] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 3000)) {
      a.commandTimers[1] = 2;
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
    if ((i16(a.animation.cursor.action) == 0x12f) && (roll = random.next(), roll < 500)) {
      a.command = 100;
      return;
    }
    if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12f)) && (roll = random.next(), roll < 300)
       ) {
      a.command = 0x65;
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
    if ((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 600)) {
      a.command = 100;
      return;
    }
    if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x134)) && (roll = random.next(), roll < 600)
       ) {
      a.command = 0x65;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 10000)) &&
       (300 < i16(a.combat.spirit))) {
      a.command = 0x32;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 500)) {
      a.command = 0x14;
      return;
    }
  }
  roll = random.next();
  if ((((float)(int)i16(a.combat.spirit) * double(0.8f) + double(0.0f) <= (float)roll) || (dx <= double(320.0f))) ||
     (i16(a.combat.spirit) < 0x15f)) {
    roll = random.next();
    if ((((float)(int)i16(a.combat.spirit) * double(0.5f) + double(0.0f) <= (float)roll) || (dx <= double(560.0f)))
       || ((i16(a.combat.spirit) < 0xfb || (double(100.0f) <= dy)))) {
      roll = random.next();
      if ((((float)(int)i16(a.combat.spirit) * double(0.1f) + double(0.0f) <= (float)roll) || (dx <= double(560.0f))
          ) || (i16(a.combat.spirit) < 0xfb)) {
        roll = random.next();
        if (roll < 0x32) {
          a.controls.current.vertical = 0xffffffff;
        }
        if (double(a.motion.y) == double(0.0f)) {
          roll = random.next();
          if (roll < 500) {
            a.command = 3;
            return;
          }
          roll = random.next();
          if (roll < 500) {
            a.command = 4;
            return;
          }
          if ((double(400.0f) < dx) && (roll = random.next(), roll < 500)) {
            a.command = 5;
            return;
          }
        }
        if (double(0.0f) < double(a.motion.y)) {
          roll = random.next();
          if (roll < 1000) {
            a.command = '\x02' - (i8(a.motion.facing) != -1);
            return;
          }
          roll = random.next();
          if (roll < 1000) {
            a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
            return;
          }
        }
        if ((dx <= double(120.0f)) || (roll = random.next(), 99 < roll)) {
          if ((dx <= double(120.0f)) ||
             ((double(a.motion.y) <= double(0.0f) || (roll = random.next(), 299 < roll)))) {
            if ((dx <= double(350.0f)) ||
               ((double(a.motion.y) != double(0.0f) || (roll = random.next(), 99 < roll)))) {
              if ((double(100.0f) <= dx) || (roll = random.next(), 9 < roll)) {
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
                  if ((dx < double(80.0f)) && (roll = random.next(), roll < 2000)) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.vertical = 0;
                    return;
                  }
                  if ((dx < double(140.0f)) && (roll = random.next(), roll < 2000)) {
                    a.controls.current.buttons[1] = 1;
                    roll = random.next();
                    a.controls.current.vertical = (u32)(20000 < roll);
                    if (double(other.motion.y) <= double(0.0f)) {
                      return;
                    }
                    a.controls.current.vertical = 0;
                    return;
                  }
                  if ((((dx < double(120.0f)) && (double(60.0f) < dx)) &&
                      (double(other.motion.y) == double(0.0f))) &&
                     (roll = random.next(), roll < 200)) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.horizontal = (int)i8(a.motion.facing);
                    return;
                  }
                  if (((double(60.0f) < dx) && (dx < double(240.0f))) && (roll = random.next(), roll < 400)) {
                    a.controls.current.buttons[0] = 1;
                    a.controls.current.horizontal = (int)i8(a.motion.facing);
                    return;
                  }
                  if (((double(200.0f) < dx) && (dx < double(400.0f))) && (roll = random.next(), roll < 400)) {
                    a.commandTimers[1] = 3;
                    return;
                  }
                  if ((((double(other.motion.y) == double(0.0f)) && (double(200.0f) < dx)) &&
                      (dx < double(400.0f))) && (roll = random.next(), roll < 400)) {
                    a.commandTimers[1] = 4;
                    return;
                  }
                  if (((double(other.motion.y) == double(0.0f)) && (double(300.0f) < dx)) &&
                     ((dx < double(500.0f) && (roll = random.next(), roll < 100)))) {
                    a.commandTimers[1] = 5;
                    return;
                  }
                }
                if ((double(0.0f) < double(a.motion.y)) &&
                   ((i16(other.animation.cursor.action) < 0x5d ||
                    (99 < i16(other.animation.cursor.action))))) {
                  if (((dx < double(150.0f)) &&
                      (((roll = random.next(), roll < 2000 && (dy < double(300.0f))) && (double(100.0f) < dy)))) &&
                     (double(other.motion.y) < double(a.motion.y))) {
                    a.controls.current.buttons[0] = 1;
                    return;
                  }
                  if (((dx < double(200.0f)) && (roll = random.next(), roll < 6000)) &&
                     ((dy < double(250.0f) && (double(80.0f) < dy)))) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.vertical = 0;
                    return;
                  }
                }
                roll = random.next();
                if (((2999 < roll) || (dx <= double(200.0f))) ||
                   ((i16(a.combat.spellTimer) < 1 ||
                    ((((double(a.combat.spellExtent) < double(100.0f) || (i16(a.spellChoices[1]) != 2))
                      || (i8(a.combat.spellPhase) != '\x01')) ||
                     (i16(state.chargeCount) != 0)))))) {
                  roll = random.next();
                  if ((((999 < roll) || (i16(a.combat.spellTimer) < 1)) ||
                      (double(a.combat.spellExtent) < double(100.0f))) ||
                     ((i16(a.spellChoices[1]) != 2 || (i8(a.combat.spellPhase) != '\x01'))))
                  {
                    roll = random.next();
                    if (((999 < roll) ||
                        (((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f)))
                         || (i16(a.spellChoices[1]) != 2)))) ||
                       (i8(a.combat.spellPhase) != '\x01')) {
                      roll = random.next();
                      if (((999 < roll) || (i16(a.combat.spellTimer) < 1)) ||
                         ((double(a.combat.spellExtent) < double(100.0f) ||
                          ((i16(a.spellChoices[1]) != 1 ||
                           (i8(a.combat.spellPhase) != '\x01')))))) {
                        roll = random.next();
                        if ((((int)((u32)(299 < i16(other.animation.cursor.action)) * 500
                                   + 0x32) <= roll) ||
                            (((i16(a.combat.spellTimer) < 1 ||
                              (double(a.combat.spellExtent) < double(100.0f))) ||
                             (i16(a.spellChoices[1]) != 0)))) ||
                           (i8(a.combat.spellPhase) != '\x01')) {
                          roll = random.next();
                          if ((((1999 < roll) || (dx <= double(200.0f))) ||
                              ((i16(a.combat.spellTimer) < 1 ||
                               ((double(a.combat.spellExtent) < double(100.0f) ||
                                (i16(a.spellChoices[0]) != 2)))))) ||
                             ((i8(a.combat.spellPhase) != '\x02' ||
                              (i16(state.chargeCount) != 0)))) {
                            roll = random.next();
                            if ((((1999 < roll) || (i16(a.combat.spellTimer) < 1)) ||
                                (double(a.combat.spellExtent) < double(100.0f))) ||
                               ((i16(a.spellChoices[1]) != 2 ||
                                (i8(a.combat.spellPhase) != '\x02')))) {
                              roll = random.next();
                              if ((((1999 < roll) ||
                                   ((i16(a.combat.spellTimer) < 1 ||
                                    (double(a.combat.spellExtent) < double(100.0f))))) ||
                                  (i16(a.spellChoices[1]) != 2)) ||
                                 (i8(a.combat.spellPhase) != '\x02')) {
                                roll = random.next();
                                if (((((int)((u32)(299 < i16(other.animation.cursor.action)) * 500 + 0x32) <= roll)
                                     || (i16(a.combat.spellTimer) < 1)) ||
                                    (double(a.combat.spellExtent) < double(100.0f))) ||
                                   ((i16(a.spellChoices[0]) != 0 ||
                                    (i8(a.combat.spellPhase) != '\x02')))) {
                                  roll = random.next();
                                  if ((((499 < roll) ||
                                       ((i16(a.combat.spellTimer) < 1 ||
                                        (double(a.combat.spellExtent) < double(100.0f))))) ||
                                      (i16(a.spellChoices[0]) != 1)) ||
                                     ((i8(a.combat.spellPhase) != '\x02' || (double(100.0f) <= dy)))) {
                                    if (((double(150.0f) <= dy) ||
                                        (((double(110.0f) <= dx || (roll = random.next(), 599 < roll)) ||
                                         (i16(other.animation.cursor.action) == 0x5d))))
                                       || (i16(other.animation.cursor.action) == 0x5e)) {
                                      if (((dx <= double(100.0f)) || (double(240.0f) <= dx)) ||
                                         (roll = random.next(), 199 < roll)) {
                                        if (((dx <= double(100.0f)) || (double(250.0f) <= dx)) ||
                                           ((double(other.motion.y) != double(0.0f) ||
                                            (roll = random.next(), 199 < roll)))) {
                                          if (((dx <= double(560.0f)) || (roll = random.next(), 199 < roll))
                                             || (i16(a.combat.spirit) < 0x15f)) {
                                            if (((double(250.0f) <= dy) || (dy <= double(100.0f))) ||
                                               ((double(200.0f) <= dx ||
                                                (((roll = random.next(), 99 < roll ||
                                                  (i16(other.animation.cursor.action) ==
                                                   0x5d)) ||
                                                 (i16(other.animation.cursor.action) ==
                                                  0x5e)))))) {
                                              if (((double(150.0f) <= dy) || (dx <= double(200.0f))) ||
                                                 (roll = random.next(), 999 < roll)) {
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
                                                a.command = 0x14;
                                              }
                                            }
                                            else {
                                              a.command = 0x1f;
                                            }
                                          }
                                          else {
                                            a.command = 0x33;
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
                                a.command = 0x17;
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
                      a.command = 0x17;
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
        roll = random.next();
        if (roll < 10000) {
          a.controls.current.horizontal = (int)i8(a.motion.facing);
        }
      }
    }
    else {
      a.controls.current.buttons[0] = 1;
    }
  }
  else {
    duration = random.range(0,0x2d);
    a.commandTimers[0] = duration;
    a.commandTimers[1] = 1;
    a.controls.current.buttons[2] = 1;
    roll = random.next();
    if (roll < 16000) {
      a.controls.current.horizontal = (int)i8(a.motion.facing);
    }
    roll = random.next();
    if (roll < 16000) {
      a.controls.current.vertical = 1;
    }
  }
  return;

}
void choose_marisa_story_input(FighterActor& a,const FighterActor& other,const MarisaState& state,ReplayRandom& random,CharacterAIContext& context){

  float dx;
  float dy;
  i8 hurt;
  u16 duration;
  int roll;
  u32 direction;
  double distance;
  
  distance = (double)std::abs(float(double(other.motion.x) -
                                double(a.motion.x)));
  dx = (float)distance;
  distance = (double)std::abs(float(double(other.motion.y) -
                                double(a.motion.y)));
  dy = (float)distance;
  a.commandTimers[0] = i16(a.commandTimers[0]) + -1;
  a.commandTimers[2] = i16(a.commandTimers[2]) + -1;
  if ((i16(a.animation.cursor.action) < 0x32) || (0x95 < i16(a.animation.cursor.action))) {
    if ((i16(a.commandTimers[2]) < 0) &&
       ((i8(a.phaseVariants[a.combat.spellPhase]) == '\x03' &&
        (0 < i16(a.combat.spellTimer))))) {
      a.command = 0xe6;
    }
    else if ((i16(a.commandTimers[2]) < 0) &&
            ((i8(a.phaseVariants[a.combat.spellPhase]) == '\x02' &&
             (0 < i16(a.combat.spellTimer))))) {
      roll = random.range(0,100);
      if (roll < 10) {
        a.command = 0xe1;
      }
      else {
        a.command = 0xdc;
        if (state.difficulty == '\x03') {
          a.command = 0xdd;
        }
      }
    }
    else if ((i16(a.commandTimers[2]) < 0) &&
            ((i8(a.phaseVariants[a.combat.spellPhase]) == '\x01' &&
             (0 < i16(a.combat.spellTimer))))) {
      roll = random.range(0,100);
      if (roll < 0x32) {
        a.command = 0xd2;
      }
      else {
        a.command = 0xd3;
      }
    }
    else if ((i8(a.phaseVariants[a.combat.spellPhase]) == '\0') &&
            (0 < i16(a.combat.spellTimer))) {
      a.command = 200;
    }
    else if (i16(a.commandTimers[1]) == 1) {
      if (i16(a.commandTimers[0]) < 1) {
        a.commandTimers[1] = 0;
      }
      else {
        a.controls.current.buttons[2] = 5;
      }
    }
    else {
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
            (dx < double(300.0f))) &&
           ((500 < i16(a.combat.spirit) && (roll = random.next(), roll < 100)))) {
          a.command = 0x32;
          return;
        }
        if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
           ((double(600.0f) < dx &&
            (((dx < double(900.0f) && (500 < i16(a.combat.spirit))) &&
             (roll = random.next(), roll < 100)))))) {
          a.command = 0x33;
          return;
        }
        if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
           ((dx < double(450.0f) && (roll = random.next(), roll < 200)))) {
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
        if ((dx < double(60.0f)) && (roll = random.next(), roll < 10000)) {
          a.controls.current.buttons[0] = 1;
          return;
        }
        if (((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 10000)) &&
           (0xfa < i16(a.combat.spirit))) {
          duration = random.range(0,0x2d);
          a.commandTimers[0] = duration;
          a.commandTimers[1] = 1;
          a.controls.current.buttons[2] = 1;
          return;
        }
        if ((i16(a.animation.cursor.action) == 0x133) && (roll = random.next(), roll < 10000)) {
          a.command = 0x14;
          return;
        }
        if (((dx < double(200.0f)) &&
            ((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x134)))) &&
           (roll = random.next(), roll < 20000)) {
          a.command = 0x14;
          return;
        }
        if ((dx < double(100.0f)) &&
           ((((i16(a.animation.cursor.action) == 0x12d || (i16(a.animation.cursor.action) == 0x134)) &&
             (roll = random.next(), roll < 5000)) && (a.response == 1)))) {
          a.command = 0x1f;
          return;
        }
        if (((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 20000)) &&
           (dx < double(80.0f))) {
          a.controls.current.buttons[0] = 1;
          return;
        }
        if ((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 20000)) {
          a.controls.current.buttons[1] = 1;
          a.controls.current.horizontal = (int)i8(a.motion.facing);
          return;
        }
        if ((i16(a.animation.cursor.action) == 300) && (roll = random.next(), roll < 1000)) {
          a.controls.current.buttons[0] = 1;
          a.controls.current.horizontal = (int)i8(a.motion.facing);
          return;
        }
        if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 1000)) {
          a.controls.current.buttons[0] = 1;
          a.controls.current.horizontal = (int)i8(a.motion.facing);
          return;
        }
        if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 500)) {
          a.command = 100;
          return;
        }
        if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12d)) &&
           (roll = random.next(), roll < 500)) {
          a.command = 0x65;
          return;
        }
        if (((dx < double(150.0f)) && (i16(a.animation.cursor.action) == 0x12d)) &&
           (roll = random.next(), roll < 10000)) {
          a.controls.current.buttons[1] = 1;
          a.controls.current.horizontal = (int)i8(a.motion.facing);
          return;
        }
        if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.next(), roll < 3000)) {
          a.commandTimers[1] = 2;
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
        if ((i16(a.animation.cursor.action) == 0x12f) && (roll = random.next(), roll < 500)) {
          a.command = 100;
          return;
        }
        if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12f)) &&
           (roll = random.next(), roll < 300)) {
          a.command = 0x65;
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
        if ((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 600)) {
          a.command = 100;
          return;
        }
        if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x134)) &&
           (roll = random.next(), roll < 600)) {
          a.command = 0x65;
          return;
        }
        if (((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 10000)) &&
           (300 < i16(a.combat.spirit))) {
          a.command = 0x32;
          return;
        }
        if ((i16(a.animation.cursor.action) == 0x134) && (roll = random.next(), roll < 500)) {
          a.command = 0x14;
          return;
        }
      }
      roll = random.next();
      if ((((float)(int)i16(a.combat.spirit) * double(0.8f) + double(0.0f) <= (float)roll) || (dx <= double(320.0f))
          ) || (i16(a.combat.spirit) < 0x15f)) {
        roll = random.next();
        if ((((float)(int)i16(a.combat.spirit) * double(0.5f) + double(0.0f) <= (float)roll) ||
            (dx <= double(560.0f))) || ((i16(a.combat.spirit) < 0xfb || (double(100.0f) <= dy)))) {
          roll = random.next();
          if ((((float)(int)i16(a.combat.spirit) * double(0.1f) + double(0.0f) <= (float)roll) ||
              (dx <= double(560.0f))) || (i16(a.combat.spirit) < 0xfb)) {
            roll = random.next();
            if (roll < 0x32) {
              a.controls.current.vertical = 0xffffffff;
            }
            if (double(a.motion.y) == double(0.0f)) {
              roll = random.next();
              if (roll < 500) {
                a.command = 3;
                return;
              }
              roll = random.next();
              if (roll < 500) {
                a.command = 4;
                return;
              }
              if ((double(400.0f) < dx) && (roll = random.next(), roll < 500)) {
                a.command = 5;
                return;
              }
            }
            if (double(0.0f) < double(a.motion.y)) {
              roll = random.next();
              if (roll < 1000) {
                a.command = '\x02' - (i8(a.motion.facing) != -1);
                return;
              }
              roll = random.next();
              if (roll < 1000) {
                a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
                return;
              }
            }
            if ((dx <= double(120.0f)) || (roll = random.next(), 99 < roll)) {
              if ((dx <= double(120.0f)) ||
                 ((double(a.motion.y) <= double(0.0f) || (roll = random.next(), 299 < roll)))) {
                if ((dx <= double(350.0f)) ||
                   ((double(a.motion.y) != double(0.0f) || (roll = random.next(), 99 < roll)))) {
                  if ((double(100.0f) <= dx) || (roll = random.next(), 9 < roll)) {
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
                      if ((dx < double(80.0f)) && (roll = random.next(), roll < 2000)) {
                        a.controls.current.buttons[1] = 1;
                        a.controls.current.vertical = 0;
                        return;
                      }
                      if ((dx < double(140.0f)) && (roll = random.next(), roll < 2000)) {
                        a.controls.current.buttons[1] = 1;
                        roll = random.next();
                        a.controls.current.vertical = (u32)(20000 < roll);
                        if (double(other.motion.y) <= double(0.0f)) {
                          return;
                        }
                        a.controls.current.vertical = 0;
                        return;
                      }
                      if ((((dx < double(120.0f)) && (double(60.0f) < dx)) &&
                          (double(other.motion.y) == double(0.0f))) &&
                         (roll = random.next(), roll < 200)) {
                        a.controls.current.buttons[1] = 1;
                        a.controls.current.horizontal = (int)i8(a.motion.facing);
                        return;
                      }
                      if (((double(60.0f) < dx) && (dx < double(240.0f))) && (roll = random.next(), roll < 400)) {
                        a.controls.current.buttons[0] = 1;
                        a.controls.current.horizontal = (int)i8(a.motion.facing);
                        return;
                      }
                      if (((double(200.0f) < dx) && (dx < double(400.0f))) && (roll = random.next(), roll < 400)) {
                        a.commandTimers[1] = 3;
                        return;
                      }
                      if ((((double(other.motion.y) == double(0.0f)) && (double(200.0f) < dx)
                           ) && (dx < double(400.0f))) && (roll = random.next(), roll < 400)) {
                        a.commandTimers[1] = 4;
                        return;
                      }
                      if (((double(other.motion.y) == double(0.0f)) && (double(300.0f) < dx))
                         && ((dx < double(500.0f) && (roll = random.next(), roll < 100)))) {
                        a.commandTimers[1] = 5;
                        return;
                      }
                    }
                    if ((double(0.0f) < double(a.motion.y)) &&
                       ((i16(other.animation.cursor.action) < 0x5d ||
                        (99 < i16(other.animation.cursor.action))))) {
                      if (((dx < double(150.0f)) &&
                          (((roll = random.next(), roll < 2000 && (dy < double(300.0f))) && (double(100.0f) < dy))))
                         && (double(other.motion.y) <
                             double(a.motion.y))) {
                        a.controls.current.buttons[0] = 1;
                        return;
                      }
                      if (((dx < double(200.0f)) && (roll = random.next(), roll < 6000)) &&
                         ((dy < double(250.0f) && (double(80.0f) < dy)))) {
                        a.controls.current.buttons[1] = 1;
                        a.controls.current.vertical = 0;
                        return;
                      }
                    }
                    if (((double(150.0f) <= dy) || (double(110.0f) <= dx)) ||
                       ((roll = random.next(), 599 < roll ||
                        ((i16(other.animation.cursor.action) == 0x5d ||
                         (i16(other.animation.cursor.action) == 0x5e)))))) {
                      if ((dx <= double(100.0f)) || ((double(240.0f) <= dx || (roll = random.next(), 199 < roll))))
                      {
                        if ((dx <= double(100.0f)) ||
                           (((double(250.0f) <= dx ||
                             (double(other.motion.y) != double(0.0f))) ||
                            (roll = random.next(), 199 < roll)))) {
                          if (((dx <= double(560.0f)) || (roll = random.next(), 199 < roll)) ||
                             (i16(a.combat.spirit) < 0x15f)) {
                            if (((double(250.0f) <= dy) || (dy <= double(100.0f))) ||
                               ((double(200.0f) <= dx ||
                                (((roll = random.next(), 99 < roll ||
                                  (i16(other.animation.cursor.action) == 0x5d)) ||
                                 (i16(other.animation.cursor.action) == 0x5e)))))) {
                              if (((double(150.0f) <= dy) || (dx <= double(200.0f))) ||
                                 (roll = random.next(), 999 < roll)) {
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
                                a.command = 0x14;
                              }
                            }
                            else {
                              a.command = 0x1f;
                            }
                          }
                          else {
                            a.command = 0x33;
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
            roll = random.next();
            if (roll < 10000) {
              a.controls.current.horizontal = (int)i8(a.motion.facing);
            }
          }
        }
        else {
          a.controls.current.buttons[0] = 1;
        }
      }
      else {
        duration = random.range(0,0x2d);
        a.commandTimers[0] = duration;
        a.commandTimers[1] = 1;
        a.controls.current.buttons[2] = 1;
        roll = random.next();
        if (roll < 16000) {
          a.controls.current.horizontal = (int)i8(a.motion.facing);
        }
        roll = random.next();
        if (roll < 16000) {
          a.controls.current.vertical = 1;
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
