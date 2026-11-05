#include "YoumuState.hpp"
#include <cmath>
namespace th075 {
void choose_youmu_duel_input(FighterActor& a,const FighterActor& other,const YoumuState& state,ReplayStreams& random,CharacterAIContext& context){
  float dx;
  float dy;
  i8 hurt;
  int iVar4;
  u32 uVar5;
  double fVar6;
  u32 local_54;
  u32 local_50;
  u32 local_4c;
  u32 local_40;
  u32 local_3c;
  u32 local_38;
  
  fVar6 = (double)std::abs(float(double(other.motion.x) -
                                double(a.motion.x)));
  dx = (float)fVar6;
  fVar6 = (double)std::abs(float(double(other.motion.y) -
                                double(a.motion.y)));
  dy = (float)fVar6;
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
    if ((((double(560.0f) < dx) && (iVar4 = random.random.next(), iVar4 < 3)) && (i8(a.combat.spellEnded) == '\0'))
       && (i16(a.combat.spellTimer) < 1)) {
      a.command = 0x67;
      return;
    }
    if (((i16(other.animation.cursor.action) == 0x62) && (iVar4 = random.random.next(), iVar4 < 0x14))
       && ((i8(a.combat.spellEnded) == '\0' && (i16(a.combat.spellTimer) < 1)))) {
      a.command = 0x67;
      return;
    }
  }
  if (double(100.0f) < double(a.combat.spellExtent)) {
    if (((i16(a.combat.health) < 4000) && (double(560.0f) < dx)) &&
       ((iVar4 = random.random.next(), iVar4 < 0x32 &&
        ((i8(a.combat.spellEnded) == '\0' && (i16(a.combat.spellTimer) < 1)))))) {
      a.command = 0x67;
      return;
    }
    if ((((i16(a.combat.health) < 4000) && (iVar4 = random.random.next(), iVar4 < 5)) &&
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
        iVar4 = random.random.next();
        if (iVar4 < 4000) {
          a.command = 3;
          return;
        }
        iVar4 = random.random.next();
        if (iVar4 < 4000) {
          a.command = 4;
          return;
        }
        iVar4 = random.random.next();
        if (iVar4 < 4000) {
          a.command = 5;
          return;
        }
      }
      if (double(0.0f) < double(a.motion.y)) {
        iVar4 = random.random.next();
        if (iVar4 < 4000) {
          a.command = '\x02' - (i8(a.motion.facing) != -1);
          return;
        }
        iVar4 = random.random.next();
        if (iVar4 < 4000) {
          a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
          return;
        }
      }
    }
    if ((((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
        (dx < double(300.0f))) && (iVar4 = random.random.next(), iVar4 < 4000)) {
      a.command = 0x14;
      return;
    }
    if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
       ((double(400.0f) < dx && ((dx < double(500.0f) && (iVar4 = random.random.next(), iVar4 < 500)))))) {
      a.command = 0x15;
      return;
    }
  }
  hurt = (other.animation.cursor.action>=50&&other.animation.cursor.action<=149);
  if ((hurt != '\0') &&
     ((i16(other.animation.cursor.action) < 0x5d ||
      (99 < i16(other.animation.cursor.action))))) {
    if ((((i16(a.animation.cursor.action) == 0x135) ||
         ((i16(a.animation.cursor.action) == 0x1f7 || (i16(a.animation.cursor.action) == 0x1f5)))) ||
        (((399 < i16(a.animation.cursor.action) && (i16(a.animation.cursor.action) < 500)) ||
         ((599 < i16(a.animation.cursor.action) && (i16(a.animation.cursor.action) < 700)))))) &&
       ((iVar4 = random.random.next(), iVar4 < 5000 &&
        ((((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))) &&
          (i16(a.spellChoices[1]) == 1)) && (i8(a.combat.spellPhase) == '\x01')))))) {
      a.command = 0xd;
      return;
    }
    if ((((((i16(a.animation.cursor.action) == 0x135) || (i16(a.animation.cursor.action) == 0x1f7)) ||
          ((i16(a.animation.cursor.action) == 0x1f5 ||
           ((399 < i16(a.animation.cursor.action) && (i16(a.animation.cursor.action) < 500)))))) ||
         ((599 < i16(a.animation.cursor.action) && (i16(a.animation.cursor.action) < 700)))) &&
        (((iVar4 = random.random.next(), iVar4 < 10000 && (0 < i16(a.combat.spellTimer))) &&
         (double(100.0f) <= double(a.combat.spellExtent))))) &&
       ((i16(a.spellChoices[1]) == 1 && (i8(a.combat.spellPhase) == '\x02')))) {
      a.command = 0xd;
      return;
    }
    if ((((((i16(a.animation.cursor.action) == 0x135) || (i16(a.animation.cursor.action) == 0x136)) ||
          ((i16(a.animation.cursor.action) == 500 ||
           (((i16(a.animation.cursor.action) == 0x1f5 || (i16(a.animation.cursor.action) == 0x1f7)) ||
            (i16(a.animation.cursor.action) == 0x186)))))) &&
         ((iVar4 = random.random.next(), iVar4 < 10000 && (0 < i16(a.combat.spellTimer))))) &&
        (double(100.0f) <= double(a.combat.spellExtent))) &&
       (((i16(a.spellChoices[1]) == 2 && (i8(a.combat.spellPhase) == '\x01')) &&
        (i16(state.phantomTime) < 1)))) {
      a.command = 0xd;
      return;
    }
    if (((((((i16(a.animation.cursor.action) == 0x135) || (i16(a.animation.cursor.action) == 0x136)) ||
           (i16(a.animation.cursor.action) == 500)) ||
          ((i16(a.animation.cursor.action) == 0x1f5 || (i16(a.animation.cursor.action) == 0x1f7)))) ||
         (i16(a.animation.cursor.action) == 0x186)) &&
        ((iVar4 = random.random.next(), iVar4 < 10000 && (0 < i16(a.combat.spellTimer))))) &&
       ((double(100.0f) <= double(a.combat.spellExtent) &&
        (((i16(a.spellChoices[1]) == 2 && (i8(a.combat.spellPhase) == '\x02')) &&
         (i16(state.phantomTime) < 1)))))) {
      a.command = 0xd;
      return;
    }
    if ((((((i16(a.animation.cursor.action) == 0x12d) || (i16(a.animation.cursor.action) == 0x133)) ||
          ((i16(a.animation.cursor.action) == 0x135 ||
           ((i16(a.animation.cursor.action) == 0x187 || (i16(a.animation.cursor.action) == 0x134)))))) ||
         (i16(a.animation.cursor.action) == 0x1f6)) &&
        (((iVar4 = random.random.next(), iVar4 < 10000 && (0 < i16(a.combat.spellTimer))) &&
         (double(100.0f) <= double(a.combat.spellExtent))))) &&
       ((i16(a.spellChoices[1]) == 0 && (i8(a.combat.spellPhase) == '\x01')))) {
      a.command = 0xd;
      return;
    }
    if ((((i16(a.animation.cursor.action) == 0x12d) || (i16(a.animation.cursor.action) == 0x133)) ||
        ((i16(a.animation.cursor.action) == 0x135 ||
         (((i16(a.animation.cursor.action) == 0x187 || (i16(a.animation.cursor.action) == 0x134)) ||
          (i16(a.animation.cursor.action) == 0x1f6)))))) &&
       (((iVar4 = random.random.next(), iVar4 < 10000 && (0 < i16(a.combat.spellTimer))) &&
        ((double(100.0f) <= double(a.combat.spellExtent) &&
         ((i16(a.spellChoices[1]) == 0 && (i8(a.combat.spellPhase) == '\x02')))))))) {
      a.command = 0xd;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x1f6) && (iVar4 = random.random.next(), iVar4 < 10000)) &&
       (dx < double(150.0f))) {
      a.command = 0x14;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x1f7) && (iVar4 = random.random.next(), iVar4 < 10000)) {
      a.command = 0x14;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x133) && (iVar4 = random.random.next(), iVar4 < 10000)) &&
       (0xfa < i16(a.combat.spirit))) {
      a.command = 0x14;
      return;
    }
    if ((((i16(a.animation.cursor.action) == 0x133) && (iVar4 = random.random.next(), iVar4 < 5000)) &&
        (dx < double(120.0f))) && (a.response == 1)) {
      a.command = 0x1e;
      iVar4 = random.random.next();
      if (17999 < iVar4) {
        return;
      }
      a.command = 0x1f;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x133) && (iVar4 = random.random.next(), iVar4 < 2000)) {
      a.command = 100;
      return;
    }
    if (((dx < double(80.0f)) && (i16(a.animation.cursor.action) == 0x133)) && (iVar4 = random.random.next(), iVar4 < 2000)
       ) {
      a.command = 0x65;
      return;
    }
    if ((((i16(a.animation.cursor.action) == 300) && (iVar4 = random.random.next(), iVar4 < 10000)) &&
        (dx < double(120.0f))) && (a.response == 1)) {
      a.command = 0x1e;
      iVar4 = random.random.next();
      if (17999 < iVar4) {
        return;
      }
      a.command = 0x1f;
      return;
    }
    if (((i16(a.animation.cursor.action) == 300) && (iVar4 = random.random.next(), iVar4 < 20000)) && (dx < double(80.0f)))
    {
      a.controls.current.buttons[0] = 1;
      return;
    }
    if ((i16(a.animation.cursor.action) == 300) && (iVar4 = random.random.next(), iVar4 < 2000)) {
      a.controls.current.buttons[1] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (iVar4 = random.random.next(), iVar4 < 15000)) {
      a.controls.current.buttons[0] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (iVar4 = random.random.next(), iVar4 < 15000)) {
      a.controls.current.buttons[1] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (iVar4 = random.random.next(), iVar4 < 0x5dc)) {
      a.command = 100;
      return;
    }
    if (((dx < double(150.0f)) && (i16(a.animation.cursor.action) == 0x12d)) &&
       (iVar4 = random.random.next(), iVar4 < 0x5dc)) {
      a.command = 0x65;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x12d) && (iVar4 = random.random.next(), iVar4 < 3000)) &&
       (0x15e < i16(a.combat.spirit))) {
      a.controls.current.buttons[2] = 1;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12d) && (iVar4 = random.random.next(), iVar4 < 8000)) {
      a.commandTimers[1] = 2;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12e) && (iVar4 = random.random.next(), iVar4 < 5000)) {
      a.controls.current.buttons[1] = 1;
      a.controls.current.horizontal = (int)i8(a.motion.facing);
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x12f) && (iVar4 = random.random.next(), iVar4 < 0x5dc)) {
      a.command = 100;
      return;
    }
    if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12f)) &&
       (iVar4 = random.random.next(), iVar4 < 0x5dc)) {
      a.command = 0x65;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x12f) && (iVar4 = random.random.next(), iVar4 < 10000)) &&
       (dx < double(80.0f))) {
      a.controls.current.buttons[1] = 1;
      a.controls.current.vertical = 1;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x12f) && (iVar4 = random.random.next(), iVar4 < 5000)) &&
       (a.response == 1)) {
      a.command = 0x1e;
      iVar4 = random.random.next();
      if (17999 < iVar4) {
        return;
      }
      a.command = 0x1f;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x134) && (iVar4 = random.random.next(), iVar4 < 10000)) &&
       (0xfa < i16(a.combat.spirit))) {
      a.command = 0x14;
      return;
    }
    if (((i16(a.animation.cursor.action) == 0x134) && (iVar4 = random.random.next(), iVar4 < 5000)) &&
       ((dx < double(120.0f) && (a.response == 1)))) {
      a.command = 0x1e;
      iVar4 = random.random.next();
      if (17999 < iVar4) {
        return;
      }
      a.command = 0x1f;
      return;
    }
    if ((i16(a.animation.cursor.action) == 0x134) && (iVar4 = random.random.next(), iVar4 < 0x9c4)) {
      a.command = 100;
      return;
    }
    if (((dx < double(80.0f)) && (i16(a.animation.cursor.action) == 0x134)) &&
       (iVar4 = random.random.next(), iVar4 < 0x9c4)) {
      a.command = 0x65;
      return;
    }
    if ((dx < double(60.0f)) && (iVar4 = random.random.next(), iVar4 < 10000)) {
      a.controls.current.buttons[0] = 1;
      return;
    }
  }
  iVar4 = random.random.next();
  if ((((float)(int)i16(a.combat.spirit) * double(0.8f) + double(0.0f) <= (float)iVar4) || (dx <= double(200.0f))) ||
     (i16(a.combat.spirit) < 0x15f)) {
    iVar4 = random.random.next();
    if (((((float)(int)i16(a.combat.spirit) * double(0.5f) + double(0.0f) <= (float)iVar4) || (dx <= double(560.0f)))
        || (i16(a.combat.spirit) < 0x2ef)) || (i16(state.phantomAttacking) != 0)) {
      iVar4 = random.random.next();
      if ((((float)(int)i16(a.combat.spirit) * double(0.5f) + double(0.0f) <= (float)iVar4) || (dx <= double(560.0f))
          ) || ((i16(a.combat.spirit) < 0x2ef || (i16(state.phantomAttacking) != 0)))) {
        iVar4 = random.random.next();
        if (iVar4 < 0x32) {
          a.controls.current.vertical = 0xffffffff;
        }
        if (double(a.motion.y) == double(0.0f)) {
          iVar4 = random.random.next();
          if ((float)iVar4 < dx / double(2.0f) + double(100.0f)) {
            a.command = 3;
            return;
          }
          iVar4 = random.random.next();
          if ((float)iVar4 < dx / double(2.0f) + double(100.0f)) {
            a.command = 4;
            return;
          }
          if ((double(400.0f) < dx) && (iVar4 = random.random.next(), (float)iVar4 < dx + double(100.0f))) {
            a.command = 5;
            return;
          }
        }
        if (double(0.0f) < double(a.motion.y)) {
          iVar4 = random.random.next();
          if (iVar4 < 0x5dc) {
            a.command = '\x02' - (i8(a.motion.facing) != -1);
            return;
          }
          iVar4 = random.random.next();
          if (iVar4 < 100) {
            a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
            return;
          }
        }
        if ((dx <= double(120.0f)) || (iVar4 = random.random.next(), 199 < iVar4)) {
          if ((dx <= double(150.0f)) ||
             ((double(a.motion.y) <= double(0.0f) || (iVar4 = random.random.next(), 499 < iVar4)))) {
            if ((dx <= double(300.0f)) ||
               ((double(a.motion.y) != double(0.0f) || (iVar4 = random.random.next(), 0x9c3 < iVar4)))) {
              if ((double(200.0f) <= dx) || (iVar4 = random.random.next(), 99 < iVar4)) {
                if ((double(a.motion.y) == double(0.0f)) &&
                   ((i16(other.animation.cursor.action) < 0x5d ||
                    (99 < i16(other.animation.cursor.action))))) {
                  if ((dx < double(70.0f)) && (iVar4 = random.random.next(), iVar4 < 4000)) {
                    a.controls.current.buttons[0] = 1;
                    return;
                  }
                  if ((dx < double(120.0f)) && (iVar4 = random.random.next(), iVar4 < 4000)) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.vertical = 0;
                    return;
                  }
                  if ((dx < double(170.0f)) && (iVar4 = random.random.next(), iVar4 < 4000)) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.vertical = 1;
                    return;
                  }
                  if ((((dx < double(80.0f)) && (double(0.0f) < dx)) &&
                      (double(other.motion.y) == double(0.0f))) &&
                     (iVar4 = random.random.next(), iVar4 < 2000)) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.horizontal = (int)i8(a.motion.facing);
                    return;
                  }
                  if (((double(100.0f) < dx) && (dx < double(400.0f))) && (iVar4 = random.random.next(), iVar4 < 100)) {
                    a.controls.current.buttons[0] = 1;
                    a.controls.current.horizontal = (int)i8(a.motion.facing);
                    return;
                  }
                  if (((double(300.0f) < dx) && (dx < double(400.0f))) && (iVar4 = random.random.next(), iVar4 < 1000)) {
                    a.commandTimers[1] = 3;
                    return;
                  }
                  if (((double(200.0f) < dx) && (dx < double(400.0f))) && (iVar4 = random.random.next(), iVar4 < 2000)) {
                    a.commandTimers[1] = 2;
                    return;
                  }
                  if (((double(other.motion.y) == double(0.0f)) && (double(200.0f) < dx)) &&
                     ((dx < double(400.0f) && (iVar4 = random.random.next(), iVar4 < 1000)))) {
                    a.commandTimers[1] = 4;
                    return;
                  }
                  if (((double(other.motion.y) == double(0.0f)) && (double(100.0f) < dx)) &&
                     ((dx < double(300.0f) && (iVar4 = random.random.next(), iVar4 < 500)))) {
                    a.commandTimers[1] = 5;
                    return;
                  }
                }
                if ((double(0.0f) < double(a.motion.y)) &&
                   ((i16(other.animation.cursor.action) < 0x5d ||
                    (99 < i16(other.animation.cursor.action))))) {
                  if (dx < double(150.0f)) {
                    local_38 = (u32)(double(0.0f) < double(other.motion.y));
                    iVar4 = random.random.next();
                    if ((iVar4 < (int)(local_38 * 4000 + 4000)) && (dy < double(200.0f))) {
                      a.controls.current.buttons[0] = 1;
                      return;
                    }
                  }
                  if ((((dx < double(150.0f)) && (iVar4 = random.random.next(), iVar4 < 6000)) && (dy < double(200.0f))) &&
                     (double(100.0f) < dy)) {
                    a.controls.current.buttons[1] = 1;
                    a.controls.current.vertical = 0;
                    return;
                  }
                }
                iVar4 = random.random.next();
                if (((((int)((u32)(i16(other.animation.cursor.action) == 0x62) * 0xb54 +
                            100) <= iVar4) || (i16(a.combat.spellTimer) < 1)) ||
                    ((double(a.combat.spellExtent) < double(100.0f) ||
                     ((i16(a.spellChoices[1]) != 2 || (i8(a.combat.spellPhase) != '\x01'))))
                    )) || (0 < i16(state.phantomTime))) {
                  local_3c = (u32)(dx < double(500.0f));
                  local_40 = (u32)(double(0.0f) < double(other.motion.y));
                  iVar4 = random.random.next();
                  if ((((iVar4 < (int)(local_3c * 1000 + 500 + local_40 * 3000)) &&
                       (0 < i16(a.combat.spellTimer))) && (double(100.0f) <= double(a.combat.spellExtent)))
                     && ((i16(a.spellChoices[1]) == 1 && (i8(a.combat.spellPhase) == '\x01')
                         ))) {
                    a.command = 0xd;
                  }
                  iVar4 = random.random.next();
                  if ((((double(3000.0f) - dx * double(4.0f) <= (float)iVar4) || (double(600.0f) <= dx)) ||
                      ((double(100.0f) <= dy ||
                       (((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f)))
                        || (i16(a.spellChoices[1]) != 0)))))) ||
                     (i8(a.combat.spellPhase) != '\x01')) {
                    iVar4 = random.random.next();
                    if ((((int)((u32)(i16(other.animation.cursor.action) == 0x62) * 0xb54
                               + 100) <= iVar4) || (i16(a.combat.spellTimer) < 1)) ||
                       (((double(a.combat.spellExtent) < double(100.0f) ||
                         ((i16(a.spellChoices[1]) != 2 ||
                          (i8(a.combat.spellPhase) != '\x02')))) ||
                        (0 < i16(state.phantomTime))))) {
                      iVar4 = random.random.next();
                      if ((((((double(3000.0f) - dx * double(10.0f) <= (float)iVar4) || (double(200.0f) <= dx)) ||
                            (double(100.0f) <= dy)) ||
                           ((i16(a.combat.spellTimer) < 1 ||
                            (double(a.combat.spellExtent) < double(100.0f))))) ||
                          (i16(a.spellChoices[1]) != 0)) ||
                         (i8(a.combat.spellPhase) != '\x01')) {
                        local_4c = (u32)(dx < double(500.0f));
                        local_50 = (u32)(double(0.0f) < double(other.motion.y));
                        iVar4 = random.random.next();
                        if (((iVar4 < (int)(local_4c * 1000 + 500 + local_50 * 3000)) &&
                            (0 < i16(a.combat.spellTimer))) &&
                           ((double(100.0f) <= double(a.combat.spellExtent) &&
                            ((i16(a.spellChoices[1]) == 1 &&
                             (i8(a.combat.spellPhase) == '\x02')))))) {
                          a.command = 0xd;
                        }
                        if ((dy < double(150.0f)) && (dx < double(110.0f))) {
                          local_54 = (u32)(double(0.0f) < double(other.motion.y));
                          iVar4 = random.random.next();
                          if ((iVar4 < (int)(local_54 * 0x9c4 + 500)) &&
                             ((i16(other.animation.cursor.action) != 0x5d &&
                              (i16(other.animation.cursor.action) != 0x5e)))) {
                            a.command = 0x1e;
                            return;
                          }
                        }
                        if ((((double(other.motion.y) != double(0.0f)) ||
                             (double(50.0f) <= dx)) || (iVar4 = random.random.next(), 499 < iVar4)) ||
                           (hurt = (other.animation.cursor.action>=50&&other.animation.cursor.action<=149), hurt != '\0')) {
                          if (((dx <= double(100.0f)) || (double(240.0f) <= dx)) ||
                             (iVar4 = random.random.next(), 199 < iVar4)) {
                            if (((double(100.0f) <= dx) ||
                                (double(other.motion.y) != double(0.0f))) ||
                               (iVar4 = random.random.next(), 199 < iVar4)) {
                              if (((dx <= double(250.0f)) || (double(350.0f) <= dx)) ||
                                 (iVar4 = random.random.next(), 299 < iVar4)) {
                                if (((double(200.0f) <= dx) || (dx <= double(100.0f))) ||
                                   (iVar4 = random.random.next(), 299 < iVar4)) {
                                  if (((dx <= double(300.0f)) ||
                                      (iVar4 = random.random.next(), dx + double(200.0f) <= (float)iVar4)) ||
                                     (i16(a.combat.spirit) < 0x15f)) {
                                    if (((dx <= double(300.0f)) ||
                                        (iVar4 = random.random.next(), dx * double(0.3f) + double(100.0f) <= (float)iVar4)) ||
                                       (i16(a.combat.spirit) < 0x259)) {
                                      if (context.duelWalkFrames < 1) {
                                        if (context.duelWalkFrames < 0) {
                                          a.controls.current.horizontal = 0xffffffff;
                                          context.duelWalkFrames = context.duelWalkFrames + 1;
                                        }
                                        else {
                                          iVar4 = random.random.next();
                                          uVar5 = random.random.next();
                                          uVar5 = uVar5 & 0x80000001;
                                          if ((int)uVar5 < 0) {
                                            uVar5 = (uVar5 - 1 | 0xfffffffe) + 1;
                                          }
                                          context.duelWalkFrames = (iVar4 % 0x5a + 10) * (uVar5 * -2 + 1);
                                        }
                                      }
                                      else {
                                        a.controls.current.horizontal = 1;
                                        context.duelWalkFrames = context.duelWalkFrames + -1;
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
                                  a.command = 0x14;
                                }
                              }
                              else {
                                a.command = 0x15;
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
                          a.command = 0x32;
                          iVar4 = random.random.next();
                          if (iVar4 < 18000) {
                            a.command = 0x33;
                          }
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
          iVar4 = random.random.next();
          if (iVar4 < 18000) {
            a.controls.current.horizontal = 0;
          }
          a.controls.current.vertical = 0xffffffff;
        }
      }
      else {
        a.controls.current.buttons[1] = 1;
        iVar4 = random.random.next();
        if (iVar4 < 15000) {
          a.controls.current.horizontal = (int)i8(a.motion.facing);
        }
      }
    }
    else {
      a.controls.current.buttons[0] = 1;
      iVar4 = random.random.next();
      if (iVar4 < 4000) {
        a.controls.current.horizontal = (int)i8(a.motion.facing);
      }
      iVar4 = random.random.next();
      if (iVar4 < 4000) {
        a.controls.current.vertical = 1;
      }
    }
  }
  else {
    a.controls.current.buttons[2] = 1;
    iVar4 = random.random.next();
    if (iVar4 < 2000) {
      a.controls.current.horizontal = (int)i8(a.motion.facing);
    }
    iVar4 = random.random.next();
    if (iVar4 < 2000) {
      a.controls.current.vertical = 1;
    }
  }
  return;
}
void choose_youmu_story_input(FighterActor& a,const FighterActor& other,const YoumuState& state,ReplayStreams& random,CharacterAIContext& context){
  float dx;
  float dy;
  i8 hurt;
  u16 duration;
  int roll;
  u32 direction;
  double distance;
  u32 local_3c;
  u32 local_38;
  
  distance = (double)std::abs(float(double(other.motion.x) -
                                double(a.motion.x)));
  dx = (float)distance;
  distance = (double)std::abs(float(double(other.motion.y) -
                                double(a.motion.y)));
  dy = (float)distance;
  a.commandTimers[0] = i16(a.commandTimers[0]) + -1;
  if ((i16(a.animation.cursor.action) < 0x32) || (0x95 < i16(a.animation.cursor.action))) {
    if ((i8(a.phaseVariants[a.combat.spellPhase]) == '\x03') &&
       (((0 < i16(a.combat.spellTimer) &&
         (a.commandTimers[2] = i16(a.commandTimers[2]) + -1,
         i16(a.commandTimers[2]) < 1)) && (double(a.motion.y) == double(0.0f))))) {
      a.command = 0xe6;
    }
    else {
      if ((i8(a.phaseVariants[a.combat.spellPhase]) == '\x02') &&
         (0 < i16(a.combat.spellTimer))) {
        a.commandTimers[2] = i16(a.commandTimers[2]) + -1;
        roll = random.random.range(0,100);
        if ((roll < 0x4b) &&
           (((i16(a.commandTimers[2]) < -0xb3 && (double(a.motion.y) == double(0.0f))) &&
            (roll = random.random.range(0,100), roll < 0x32)))) {
          a.command = 0xdc;
          return;
        }
      }
      if ((((i8(a.phaseVariants[a.combat.spellPhase]) == '\x01') &&
           (0 < i16(a.combat.spellTimer))) &&
          (a.commandTimers[2] = i16(a.commandTimers[2]) + -1,
          i16(a.commandTimers[2]) < 1)) &&
         (((double(a.motion.y) == double(0.0f) && (dx < double(240.0f))) &&
          (roll = random.range(0,100), roll < 0x1e)))) {
        a.command = 0xd2;
      }
      if ((i8(a.phaseVariants[a.combat.spellPhase]) == '\0') &&
         (0 < i16(a.combat.spellTimer))) {
        a.commandTimers[2] = i16(a.commandTimers[2]) + -1;
        roll = random.random.range(0,100);
        if ((roll < 0x4b) &&
           ((i16(a.commandTimers[2]) < 1 && (double(a.motion.y) == double(0.0f))))) {
          roll = random.random.range(0,100);
          if ((roll < 0x32) && (dx < double(240.0f))) {
            a.command = 0xc9;
            return;
          }
          roll = random.random.range(0,100);
          if (roll < 0x32) {
            roll = random.random.range(0,100);
            if (roll < 0x32) {
              a.command = 200;
              return;
            }
            a.command = 0xca;
            return;
          }
          hurt = random.random.range(0,3);
          a.command = hurt + -0x38;
        }
      }
      if (i16(a.commandTimers[1]) == 1) {
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
              roll = random.random.next();
              if (roll < 4000) {
                a.command = 3;
                return;
              }
              roll = random.random.next();
              if (roll < 4000) {
                a.command = 4;
                return;
              }
              roll = random.random.next();
              if (roll < 4000) {
                a.command = 5;
                return;
              }
            }
            if (double(0.0f) < double(a.motion.y)) {
              roll = random.random.next();
              if (roll < 4000) {
                a.command = '\x02' - (i8(a.motion.facing) != -1);
                return;
              }
              roll = random.random.next();
              if (roll < 4000) {
                a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1));
                return;
              }
            }
          }
          if ((((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
              (dx < double(300.0f))) && (roll = random.random.next(), roll < 4000)) {
            a.command = 0x14;
            return;
          }
          if (((399 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 500)) &&
             ((double(400.0f) < dx && ((dx < double(500.0f) && (roll = random.random.next(), roll < 500)))))) {
            a.command = 0x15;
            return;
          }
        }
        hurt = (other.animation.cursor.action>=50&&other.animation.cursor.action<=149);
        if ((hurt != '\0') &&
           ((i16(other.animation.cursor.action) < 0x5d ||
            (99 < i16(other.animation.cursor.action))))) {
          if ((i16(a.animation.cursor.action) == 0x1f6) &&
             ((roll = random.random.next(), roll < 10000 && (dx < double(150.0f))))) {
            a.command = 0x14;
            return;
          }
          if ((i16(a.animation.cursor.action) == 0x1f7) && (roll = random.random.next(), roll < 10000)) {
            a.command = 0x14;
            return;
          }
          if (((i16(a.animation.cursor.action) == 0x133) && (roll = random.random.next(), roll < 10000)) &&
             (0xfa < i16(a.combat.spirit))) {
            a.command = 0x14;
            return;
          }
          if (((i16(a.animation.cursor.action) == 0x133) && (roll = random.random.next(), roll < 5000)) &&
             ((dx < double(120.0f) && (a.response == 1)))) {
            a.command = 0x1e;
            roll = random.random.next();
            if (17999 < roll) {
              return;
            }
            a.command = 0x1f;
            return;
          }
          if ((i16(a.animation.cursor.action) == 0x133) && (roll = random.random.next(), roll < 2000)) {
            a.command = 100;
            return;
          }
          if (((dx < double(80.0f)) && (i16(a.animation.cursor.action) == 0x133)) &&
             (roll = random.random.next(), roll < 2000)) {
            a.command = 0x65;
            return;
          }
          if (((i16(a.animation.cursor.action) == 300) && (roll = random.random.next(), roll < 10000)) &&
             ((dx < double(120.0f) && (a.response == 1)))) {
            a.command = 0x1e;
            roll = random.random.next();
            if (17999 < roll) {
              return;
            }
            a.command = 0x1f;
            return;
          }
          if (((i16(a.animation.cursor.action) == 300) && (roll = random.random.next(), roll < 20000)) &&
             (dx < double(80.0f))) {
            a.controls.current.buttons[0] = 1;
            return;
          }
          if ((i16(a.animation.cursor.action) == 300) && (roll = random.random.next(), roll < 2000)) {
            a.controls.current.buttons[1] = 1;
            a.controls.current.horizontal = (int)i8(a.motion.facing);
            return;
          }
          if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.random.next(), roll < 15000)) {
            a.controls.current.buttons[0] = 1;
            a.controls.current.horizontal = (int)i8(a.motion.facing);
            return;
          }
          if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.random.next(), roll < 15000)) {
            a.controls.current.buttons[1] = 1;
            a.controls.current.horizontal = (int)i8(a.motion.facing);
            return;
          }
          if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.random.next(), roll < 0x5dc)) {
            a.command = 100;
            return;
          }
          if (((dx < double(150.0f)) && (i16(a.animation.cursor.action) == 0x12d)) &&
             (roll = random.random.next(), roll < 0x5dc)) {
            a.command = 0x65;
            return;
          }
          if (((i16(a.animation.cursor.action) == 0x12d) && (roll = random.random.next(), roll < 3000)) &&
             (0x15e < i16(a.combat.spirit))) {
            a.controls.current.buttons[2] = 1;
            return;
          }
          if ((i16(a.animation.cursor.action) == 0x12d) && (roll = random.random.next(), roll < 8000)) {
            a.commandTimers[1] = 2;
            return;
          }
          if ((i16(a.animation.cursor.action) == 0x12e) && (roll = random.random.next(), roll < 5000)) {
            a.controls.current.buttons[1] = 1;
            a.controls.current.horizontal = (int)i8(a.motion.facing);
            return;
          }
          if ((i16(a.animation.cursor.action) == 0x12f) && (roll = random.random.next(), roll < 0x5dc)) {
            a.command = 100;
            return;
          }
          if (((double(150.0f) < dx) && (i16(a.animation.cursor.action) == 0x12f)) &&
             (roll = random.random.next(), roll < 0x5dc)) {
            a.command = 0x65;
            return;
          }
          if (((i16(a.animation.cursor.action) == 0x12f) && (roll = random.random.next(), roll < 10000)) &&
             (dx < double(80.0f))) {
            a.controls.current.buttons[1] = 1;
            a.controls.current.vertical = 1;
            return;
          }
          if (((i16(a.animation.cursor.action) == 0x12f) && (roll = random.random.next(), roll < 5000)) &&
             (a.response == 1)) {
            a.command = 0x1e;
            roll = random.random.next();
            if (17999 < roll) {
              return;
            }
            a.command = 0x1f;
            return;
          }
          if (((i16(a.animation.cursor.action) == 0x134) && (roll = random.random.next(), roll < 10000)) &&
             (0xfa < i16(a.combat.spirit))) {
            a.command = 0x14;
            return;
          }
          if (((i16(a.animation.cursor.action) == 0x134) && (roll = random.random.next(), roll < 5000)) &&
             ((dx < double(120.0f) && (a.response == 1)))) {
            a.command = 0x1e;
            roll = random.random.next();
            if (17999 < roll) {
              return;
            }
            a.command = 0x1f;
            return;
          }
          if ((i16(a.animation.cursor.action) == 0x134) && (roll = random.random.next(), roll < 0x9c4)) {
            a.command = 100;
            return;
          }
          if (((dx < double(80.0f)) && (i16(a.animation.cursor.action) == 0x134)) &&
             (roll = random.random.next(), roll < 0x9c4)) {
            a.command = 0x65;
            return;
          }
          if ((dx < double(60.0f)) && (roll = random.random.next(), roll < 10000)) {
            a.controls.current.buttons[0] = 1;
            return;
          }
        }
        roll = random.random.next();
        if ((((float)(int)i16(a.combat.spirit) * double(0.8f) + double(0.0f) <= (float)roll) ||
            (dx <= double(200.0f))) || (i16(a.combat.spirit) < 0x15f)) {
          roll = random.random.next();
          if (((((float)(int)i16(a.combat.spirit) * double(0.5f) + double(0.0f) <= (float)roll) ||
               (dx <= double(560.0f))) || (i16(a.combat.spirit) < 0x2ef)) ||
             (i16(state.phantomAttacking) != 0)) {
            roll = random.random.next();
            if ((((float)(int)i16(a.combat.spirit) * double(0.5f) + double(0.0f) <= (float)roll) ||
                (dx <= double(560.0f))) ||
               ((i16(a.combat.spirit) < 0x2ef || (i16(state.phantomAttacking) != 0)))) {
              roll = random.random.next();
              if (roll < 0x32) {
                a.controls.current.vertical = 0xffffffff;
              }
              if (double(a.motion.y) == double(0.0f)) {
                roll = random.random.next();
                if ((float)roll < dx / double(2.0f) + double(100.0f)) {
                  a.command = 3;
                  return;
                }
                roll = random.random.next();
                if ((float)roll < dx / double(2.0f) + double(100.0f)) {
                  a.command = 4;
                  return;
                }
                if ((double(400.0f) < dx) && (roll = random.random.next(), (float)roll < dx + double(100.0f))) {
                  a.command = 5;
                  return;
                }
              }
              if (double(0.0f) < double(a.motion.y)) {
                roll = random.random.next();
                if (roll < 0x5dc) {
                  a.command = '\x02' - (i8(a.motion.facing) != -1);
                  return;
                }
                roll = random.random.next();
                if (roll < 100) {
                  a.command = '\x02' - ('\x01' - (i8(a.motion.facing) != -1))
                  ;
                  return;
                }
              }
              if ((dx <= double(120.0f)) || (roll = random.random.next(), 199 < roll)) {
                if ((dx <= double(150.0f)) ||
                   ((double(a.motion.y) <= double(0.0f) || (roll = random.random.next(), 499 < roll)))) {
                  if ((dx <= double(300.0f)) ||
                     ((double(a.motion.y) != double(0.0f) || (roll = random.random.next(), 0x9c3 < roll)))) {
                    if ((double(200.0f) <= dx) || (roll = random.random.next(), 99 < roll)) {
                      if ((double(a.motion.y) == double(0.0f)) &&
                         ((i16(other.animation.cursor.action) < 0x5d ||
                          (99 < i16(other.animation.cursor.action))))) {
                        if ((dx < double(70.0f)) && (roll = random.random.next(), roll < 4000)) {
                          a.controls.current.buttons[0] = 1;
                          return;
                        }
                        if ((dx < double(120.0f)) && (roll = random.random.next(), roll < 4000)) {
                          a.controls.current.buttons[1] = 1;
                          a.controls.current.vertical = 0;
                          return;
                        }
                        if ((dx < double(170.0f)) && (roll = random.random.next(), roll < 4000)) {
                          a.controls.current.buttons[1] = 1;
                          a.controls.current.vertical = 1;
                          return;
                        }
                        if ((((dx < double(80.0f)) && (double(0.0f) < dx)) &&
                            (double(other.motion.y) == double(0.0f))) &&
                           (roll = random.random.next(), roll < 2000)) {
                          a.controls.current.buttons[1] = 1;
                          a.controls.current.horizontal = (int)i8(a.motion.facing);
                          return;
                        }
                        if (((double(100.0f) < dx) && (dx < double(400.0f))) && (roll = random.random.next(), roll < 100))
                        {
                          a.controls.current.buttons[0] = 1;
                          a.controls.current.horizontal = (int)i8(a.motion.facing);
                          return;
                        }
                        if (((double(300.0f) < dx) && (dx < double(400.0f))) && (roll = random.random.next(), roll < 1000))
                        {
                          a.commandTimers[1] = 3;
                          return;
                        }
                        if (((double(200.0f) < dx) && (dx < double(400.0f))) && (roll = random.random.next(), roll < 2000))
                        {
                          a.commandTimers[1] = 2;
                          return;
                        }
                        if (((double(other.motion.y) == double(0.0f)) &&
                            (double(200.0f) < dx)) && ((dx < double(400.0f) && (roll = random.random.next(), roll < 1000)))
                           ) {
                          a.commandTimers[1] = 4;
                          return;
                        }
                        if (((double(other.motion.y) == double(0.0f)) &&
                            (double(100.0f) < dx)) && ((dx < double(300.0f) && (roll = random.random.next(), roll < 500))))
                        {
                          a.commandTimers[1] = 5;
                          return;
                        }
                      }
                      if ((double(0.0f) < double(a.motion.y)) &&
                         ((i16(other.animation.cursor.action) < 0x5d ||
                          (99 < i16(other.animation.cursor.action))))) {
                        if (dx < double(150.0f)) {
                          local_38 = (u32)(double(0.0f) < double(other.motion.y));
                          roll = random.random.next();
                          if ((roll < (int)(local_38 * 4000 + 4000)) && (dy < double(200.0f))) {
                            a.controls.current.buttons[0] = 1;
                            return;
                          }
                        }
                        if ((((dx < double(150.0f)) && (roll = random.random.next(), roll < 6000)) && (dy < double(200.0f))
                            ) && (double(100.0f) < dy)) {
                          a.controls.current.buttons[1] = 1;
                          a.controls.current.vertical = 0;
                          return;
                        }
                      }
                      if ((dy < double(150.0f)) && (dx < double(110.0f))) {
                        local_3c = (u32)(double(0.0f) < double(other.motion.y));
                        roll = random.random.next();
                        if ((roll < (int)(local_3c * 0x9c4 + 500)) &&
                           ((i16(other.animation.cursor.action) != 0x5d &&
                            (i16(other.animation.cursor.action) != 0x5e)))) {
                          a.command = 0x1e;
                          return;
                        }
                      }
                      if ((((double(other.motion.y) != double(0.0f)) || (double(50.0f) <= dx)
                           ) || (roll = random.random.next(), 499 < roll)) ||
                         (hurt = (other.animation.cursor.action>=50&&other.animation.cursor.action<=149), hurt != '\0')) {
                        if (((dx <= double(100.0f)) || (double(240.0f) <= dx)) || (roll = random.random.next(), 199 < roll)
                           ) {
                          if (((double(100.0f) <= dx) ||
                              (double(other.motion.y) != double(0.0f))) ||
                             (roll = random.random.next(), 199 < roll)) {
                            if (((dx <= double(250.0f)) || (double(350.0f) <= dx)) ||
                               (roll = random.random.next(), 299 < roll)) {
                              if (((double(200.0f) <= dx) || (dx <= double(100.0f))) ||
                                 (roll = random.random.next(), 299 < roll)) {
                                if (((dx <= double(300.0f)) ||
                                    (roll = random.random.next(), dx + double(200.0f) <= (float)roll)) ||
                                   (i16(a.combat.spirit) < 0x15f)) {
                                  if (((dx <= double(300.0f)) ||
                                      (roll = random.random.next(), dx * double(0.3f) + double(100.0f) <= (float)roll)) ||
                                     (i16(a.combat.spirit) < 0x259)) {
                                    if (context.storyWalkFrames < 1) {
                                      if (context.storyWalkFrames < 0) {
                                        a.controls.current.horizontal = 0xffffffff;
                                        context.storyWalkFrames = context.storyWalkFrames + 1;
                                      }
                                      else {
                                        roll = random.random.next();
                                        direction = random.random.next();
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
                                    a.command = 0xb;
                                  }
                                }
                                else {
                                  a.command = 10;
                                }
                              }
                              else {
                                a.command = 0x14;
                              }
                            }
                            else {
                              a.command = 0x15;
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
                        a.command = 0x32;
                        roll = random.random.next();
                        if (roll < 18000) {
                          a.command = 0x33;
                        }
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
                roll = random.random.next();
                if (roll < 18000) {
                  a.controls.current.horizontal = 0;
                }
                a.controls.current.vertical = 0xffffffff;
              }
            }
            else {
              a.controls.current.buttons[1] = 1;
              roll = random.random.next();
              if (roll < 15000) {
                a.controls.current.horizontal = (int)i8(a.motion.facing);
              }
            }
          }
          else {
            a.controls.current.buttons[0] = 1;
            roll = random.random.next();
            if (roll < 4000) {
              a.controls.current.horizontal = (int)i8(a.motion.facing);
            }
            roll = random.random.next();
            if (roll < 4000) {
              a.controls.current.vertical = 1;
            }
          }
        }
        else {
          a.controls.current.buttons[2] = 1;
          roll = random.random.next();
          if (roll < 2000) {
            a.controls.current.horizontal = (int)i8(a.motion.facing);
          }
          roll = random.random.next();
          if (roll < 2000) {
            a.controls.current.vertical = 1;
          }
        }
      }
    }
  }
  else {
    if (i16(a.commandTimers[2]) < 0) {
      duration = random.random.range(0x1e,0x5a);
      a.commandTimers[2] = duration;
    }
    a.commandTimers[1] = 0;
    a.commandTimers[0] = 0;
    roll = random.random.range(0,100);
    if (roll < 0x32) {
      a.controls.current.horizontal = 1;
    }
    else {
      a.controls.current.horizontal = 0xffffffff;
    }
    roll = random.random.range(0,100);
    if (roll < 5) {
      a.controls.current.buttons[0] = 1;
    }
  }
  return;
}
}
