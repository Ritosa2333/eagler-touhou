#include "AliceState.hpp"
namespace th075 {
void select_alice_commands(FighterActor& a,const FighterActor& other,const AliceState& s){
    if(!a.animation.pattern||!a.animation.frame)return;
    const auto& sequences=a.animation.pattern->sequences;
    const i16 action=a.animation.cursor.action;
    if(action<0||u32(action)>=sequences.size())return;
    const i16 priority=i16(sequences[action].next);
    const auto mode=[&](i16 id){return id>=0&&u32(id)<sequences.size()&&sequences[id].present?i16(sequences[id].mode):i16(-32768);};

  i16 comparedPriority = 0;
  i16 targetPriority = 0;
  
  if ((((i8(a.command) == -0x33) && (double(a.motion.y) == double(0.0f))) &&
      ((a.animation.frame->flags[0] & 0x20) != 0)) &&
     (comparedPriority = priority, targetPriority = mode(0x2d0),
     comparedPriority <= targetPriority)) {
    a.reset_command_history();
    a.select_action(0x2d0);
    return;
  }
  if (((i8(a.command) == -0x24) && (double(a.motion.y) == double(0.0f))) &&
     (((a.animation.frame->flags[0] & 0x20) != 0 &&
      (comparedPriority = priority, targetPriority = mode(0x2ef),
      comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2ef);
    return;
  }
  if (((i8(a.command) == -0x2e) && (double(a.motion.y) == double(0.0f))) &&
     (((a.animation.frame->flags[0] & 0x20) != 0 &&
      (comparedPriority = priority, targetPriority = mode(0x2e5),
      comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2e5);
    return;
  }
  if ((((i8(a.command) == -0x38) && (double(a.motion.y) == double(0.0f))) &&
      ((a.animation.frame->flags[0] & 0x20) != 0)) &&
     (comparedPriority = priority, targetPriority = mode(0x2db),
     comparedPriority <= targetPriority)) {
    a.reset_command_history();
    a.select_action(0x2db);
    return;
  }
  if (((i8(a.command) == -0x37) && (double(a.motion.y) == double(0.0f))) &&
     (((a.animation.frame->flags[0] & 0x20) != 0 &&
      (comparedPriority = priority, targetPriority = mode(0x2dc),
      comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2dc);
    return;
  }
  if (((i8(a.command) == -0x36) && (double(a.motion.y) == double(0.0f))) &&
     (((a.animation.frame->flags[0] & 0x20) != 0 &&
      (comparedPriority = priority, targetPriority = mode(0x2dd),
      comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2dd);
    return;
  }
  if (((((i8(a.command) == 'g') && (double(a.motion.y) == double(0.0f))) &&
       (double(100.0f) <= double(a.combat.spellExtent))) &&
      (((i8(a.combat.spellEnded) == '\0' && (i16(a.combat.spellTimer) < 1)) &&
       (((a.animation.frame->flags[0] & 0x20) != 0 &&
        (comparedPriority = priority, targetPriority = mode(0x31b),
        comparedPriority <= targetPriority)))))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x31b);
    return;
  }
  if ((((('\0' < i8(a.selectedSpell)) && (double(a.motion.y) == double(0.0f))) &&
       (i8(a.command) == 'f')) &&
      (((a.animation.frame->flags[0] & 0x20) != 0 &&
       (comparedPriority = priority, targetPriority = mode(0x252),
       comparedPriority <= targetPriority)))) &&
     (((299 < i16(a.animation.cursor.action) &&
       ((a.response != 0 && (a.response != 3)))) ||
      (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    if (double(a.motion.y) == double(0.0f)) {
      if (299 < i16(a.animation.cursor.action)) {
        a.select_action(0x253);
        return;
      }
      a.select_action(0x252);
      return;
    }
    if (double(0.0f) < double(a.motion.y)) {
      if ((0x95 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 0x9f)) {
        a.select_action(599);
        return;
      }
      if (299 < i16(a.animation.cursor.action)) {
        a.select_action(0x256);
        return;
      }
      a.select_action(0x255);
      return;
    }
  }
  if (((((('\0' < i8(a.selectedSpell)) && (double(a.motion.y) == double(0.0f))) &&
        (i8(a.command) == 'f')) &&
       (((a.animation.frame->flags[0] & 0x20) != 0 &&
        (comparedPriority = priority, targetPriority = mode(0x254),
        comparedPriority <= targetPriority)))) && (i16(a.animation.cursor.action) < 300)) &&
     (a.reset_command_history(), double(a.motion.y) == double(0.0f))) {
    a.select_action(0x254);
    return;
  }
  if ((((((0 < i16(a.combat.spirit)) && (i8(a.command) == '\r')) &&
        ((double(a.motion.y) == double(0.0f) &&
         (((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))) &&
          (i16(a.spellChoices[1]) == 0)))))) &&
       ((i8(a.combat.spellPhase) == '\x01' &&
        ((a.animation.frame->flags[0] & 0x20) != 0)))) &&
      (comparedPriority = priority, targetPriority = mode(0x2bf),
      comparedPriority <= targetPriority)) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x2bf);
    return;
  }
  if (((((((0 < i16(a.combat.spirit)) && (i16(s.dollPhase) == 0)) &&
         (i8(a.command) == '\r')) &&
        ((double(a.motion.y) == double(0.0f) && (0 < i16(a.combat.spellTimer))))) &&
       ((double(100.0f) <= double(a.combat.spellExtent) &&
        ((i16(a.spellChoices[1]) == 1 && (i8(a.combat.spellPhase) == '\x01')))))) &&
      ((a.animation.frame->flags[0] & 0x20) != 0)) &&
     ((comparedPriority = priority, targetPriority = mode(0x2c0),
      comparedPriority <= targetPriority &&
      ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
        (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))))) {
    a.reset_command_history();
    a.select_action(0x2c0);
    return;
  }
  if ((((0 < i16(a.combat.spirit)) && (i8(a.command) == '\r')) &&
      ((((double(a.motion.y) == double(0.0f) &&
         ((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))))) &&
        (i16(a.spellChoices[1]) == 2)) &&
       (((i8(a.combat.spellPhase) == '\x01' &&
         ((a.animation.frame->flags[0] & 0x20) != 0)) &&
        (comparedPriority = priority, targetPriority = mode(0x2c1),
        comparedPriority <= targetPriority)))))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x2c1);
    return;
  }
  if (((0 < i16(a.combat.spirit)) && (i8(a.command) == '\r')) &&
     (((double(a.motion.y) == double(0.0f) &&
       (((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))) &&
        (i16(a.spellChoices[0]) == 0)))) &&
      (((i8(a.combat.spellPhase) == '\x02' &&
        ((a.animation.frame->flags[0] & 0x20) != 0)) &&
       ((comparedPriority = priority, targetPriority = mode(700),
        comparedPriority <= targetPriority &&
        ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
          (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))))))))) {
    a.reset_command_history();
    a.select_action(700);
    return;
  }
  if (((((((0 < i16(a.combat.spirit)) && (i16(s.dollPhase) == 0)) &&
         (i8(a.command) == '\r')) &&
        ((double(a.motion.y) == double(0.0f) && (0 < i16(a.combat.spellTimer))))) &&
       ((double(100.0f) <= double(a.combat.spellExtent) &&
        ((i16(a.spellChoices[0]) == 1 && (i8(a.combat.spellPhase) == '\x02')))))) &&
      (((a.animation.frame->flags[0] & 0x20) != 0 &&
       (comparedPriority = priority, targetPriority = mode(0x2bd),
       comparedPriority <= targetPriority)))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x2bd);
    return;
  }
  if (((0 < i16(a.combat.spirit)) && (i8(a.command) == '\r')) &&
     ((((double(a.motion.y) == double(0.0f) &&
        ((0 < i16(a.combat.spellTimer) && (double(100.0f) <= double(a.combat.spellExtent))))) &&
       ((i16(a.spellChoices[0]) == 2 &&
        (((i8(a.combat.spellPhase) == '\x02' &&
          ((a.animation.frame->flags[0] & 0x20) != 0)) &&
         (comparedPriority = priority, targetPriority = mode(0x2be),
         comparedPriority <= targetPriority)))))) &&
      ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
        (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))))) {
    a.reset_command_history();
    a.select_action(0x2be);
    return;
  }
  if ((((0 < i16(a.combat.spirit)) && (i8(a.command) == '\x16')) &&
      ((double(a.motion.y) == double(0.0f) &&
       ((((a.animation.frame->flags[0] & 0x20) != 0 &&
         (0 < i16(a.combat.spirit))) &&
        (comparedPriority = priority, targetPriority = mode(0x260),
        comparedPriority <= targetPriority)))))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x260);
    return;
  }
  if ((((0 < i16(a.combat.spirit)) && (i8(a.command) == '\x15')) &&
      ((double(a.motion.y) == double(0.0f) &&
       ((((a.animation.frame->flags[0] & 0x20) != 0 &&
         (0 < i16(a.combat.spirit))) &&
        (comparedPriority = priority, targetPriority = mode(0x25f),
        comparedPriority <= targetPriority)))))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x25f);
    return;
  }
  if ((((0 < i16(a.combat.spirit)) && (i8(a.command) == '\x14')) &&
      ((double(a.motion.y) == double(0.0f) &&
       ((((a.animation.frame->flags[0] & 0x20) != 0 &&
         (0 < i16(a.combat.spirit))) &&
        (comparedPriority = priority, targetPriority = mode(0x25e),
        comparedPriority <= targetPriority)))))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x25e);
    return;
  }
  if ((((0 < i16(a.combat.spirit)) && (i8(a.command) == '\x1f')) &&
      ((double(a.motion.y) == double(0.0f) &&
       (((a.animation.frame->flags[0] & 0x20) != 0 &&
        (comparedPriority = priority, targetPriority = mode(0x265),
        comparedPriority <= targetPriority)))))) &&
     (((299 < i16(a.animation.cursor.action) &&
       ((a.response != 0 && (a.response != 3)))) ||
      (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x265);
    return;
  }
  if (((((0 < i16(a.combat.spirit)) && (i8(a.command) == '\x1e')) &&
       (double(a.motion.y) == double(0.0f))) &&
      (((a.animation.frame->flags[0] & 0x20) != 0 &&
       (comparedPriority = priority, targetPriority = mode(0x264),
       comparedPriority <= targetPriority)))) &&
     (((299 < i16(a.animation.cursor.action) &&
       ((a.response != 0 && (a.response != 3)))) ||
      (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x264);
    return;
  }
  if (((((i8(a.command) == '\f') && (0 < i16(a.combat.spirit))) &&
       ((a.animation.frame->flags[0] & 0x20) != 0)) &&
      ((double(a.motion.y) == double(0.0f) &&
       (comparedPriority = priority, targetPriority = mode(0x25a),
       comparedPriority <= targetPriority)))) &&
     (((299 < i16(a.animation.cursor.action) &&
       ((a.response != 0 && (a.response != 3)))) ||
      (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x25a);
    return;
  }
  if (((((i8(a.command) == '\v') && (0 < i16(a.combat.spirit))) &&
       ((a.animation.frame->flags[0] & 0x20) != 0)) &&
      ((double(a.motion.y) == double(0.0f) &&
       (comparedPriority = priority, targetPriority = mode(0x259),
       comparedPriority <= targetPriority)))) &&
     (((299 < i16(a.animation.cursor.action) &&
       ((a.response != 0 && (a.response != 3)))) ||
      (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x259);
    return;
  }
  if (((((i8(a.command) == '\n') && (0 < i16(a.combat.spirit))) &&
       ((a.animation.frame->flags[0] & 0x20) != 0)) &&
      ((double(a.motion.y) == double(0.0f) &&
       (comparedPriority = priority, targetPriority = mode(600),
       comparedPriority <= targetPriority)))) &&
     (((299 < i16(a.animation.cursor.action) &&
       ((a.response != 0 && (a.response != 3)))) ||
      (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(600);
    return;
  }
  if ((((i8(a.command) == 'e') &&
       ((a.animation.frame->flags[0] & 0x20) != 0)) &&
      (double(a.motion.y) == double(0.0f))) &&
     ((comparedPriority = priority, targetPriority = mode(0x135),
      comparedPriority <= targetPriority &&
      ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
        (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))))) {
    a.reset_command_history();
    a.select_action(0x135);
    return;
  }
  if ((((i8(a.command) == 'd') && (double(a.motion.y) == double(0.0f))) &&
      (((a.animation.frame->flags[0] & 0x20) != 0 &&
       (comparedPriority = priority, targetPriority = mode(0x136),
       comparedPriority <= targetPriority)))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0x136);
    return;
  }
  if (((((((i8(a.command) == '\x01') && (i8(a.motion.facing) == '\x01')) ||
         ((i8(a.command) == '\x02' && (i8(a.motion.facing) == -1)))) ||
        ((a.controls.current.buttons[3] != 0 &&
         ((int)i8(a.motion.facing) * a.controls.current.horizontal == 1)))) ||
       ((a.controls.current.buttons[3] == 1 &&
        (0 < (int)i8(a.motion.facing) * a.controls.current.horizontal)))) &&
      ((((double(a.motion.y) == double(0.0f) && (i16(a.animation.cursor.action) != 0xcf)) &&
        ((a.animation.frame->flags[0] & 0x20) != 0)) &&
       (comparedPriority = priority, targetPriority = mode(200),
       comparedPriority <= targetPriority)))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(200);
    return;
  }
  if ((((((i8(a.command) == '\x01') && (i8(a.motion.facing) == '\x01')) ||
        (((i8(a.command) == '\x02' && (i8(a.motion.facing) == -1)) ||
         ((a.controls.current.buttons[3] != 0 &&
          ((int)i8(a.motion.facing) * a.controls.current.horizontal == 1)))))) ||
       ((a.controls.current.buttons[3] == 1 &&
        (0 < (int)i8(a.motion.facing) * a.controls.current.horizontal)))) &&
      ((((double(0.0f) < double(a.motion.y) &&
         (((double(60.0f) <= double(a.motion.y) || (double(a.motion.velocityY) <= double(0.0f))) &&
          ((a.animation.frame->flags[0] & 0x20) != 0)))) &&
        (i8(a.movement.usedAirDashes) < '\x02')) &&
       (((comparedPriority = priority, targetPriority = mode(0xca),
         comparedPriority <= targetPriority || (priority == 99))
        || ((priority == 0x31 ||
            (priority == 0x27)))))))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
    a.select_action(0xca);
    return;
  }
  if (((((((i8(a.command) == '\x02') && (i8(a.motion.facing) == '\x01')) ||
         ((i8(a.command) == '\x01' && (i8(a.motion.facing) == -1)))) ||
        ((a.controls.current.buttons[3] != 0 &&
         ((int)i8(a.motion.facing) * a.controls.current.horizontal == -1)))) ||
       ((a.controls.current.buttons[3] == 1 &&
        ((int)i8(a.motion.facing) * a.controls.current.horizontal < 0)))) &&
      (((double(a.motion.y) == double(0.0f) &&
        ((a.animation.frame->flags[0] & 0x20) != 0)) &&
       (comparedPriority = priority, targetPriority = mode(0xc9),
       comparedPriority <= targetPriority)))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0xc9);
    return;
  }
  if (((((i8(a.command) == '\x02') && (i8(a.motion.facing) == '\x01')) ||
       ((i8(a.command) == '\x01' && (i8(a.motion.facing) == -1)))) ||
      (((a.controls.current.buttons[3] != 0 &&
        ((int)i8(a.motion.facing) * a.controls.current.horizontal == -1)) ||
       ((a.controls.current.buttons[3] == 1 &&
        ((int)i8(a.motion.facing) * a.controls.current.horizontal < 0)))))) &&
     (((((double(0.0f) < double(a.motion.y) &&
         ((a.animation.frame->flags[0] & 0x20) != 0)) &&
        (i8(a.movement.usedAirDashes) < '\x02')) &&
       (((comparedPriority = priority, targetPriority = mode(0xcb),
         comparedPriority <= targetPriority || (priority == 99))
        || ((priority == 0x31 ||
            (priority == 0x27)))))) &&
      ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
        (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))))) {
    a.reset_command_history();
    a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
    a.select_action(0xcb);
    return;
  }
  if ((((((i8(a.command) == '\x03') ||
         (((a.controls.current.buttons[3] != 0 && (a.controls.current.vertical == -1)) &&
          (a.controls.current.horizontal == 0)))) ||
        (((a.controls.current.buttons[3] == 1 && (a.controls.current.vertical < -1)) &&
         (a.controls.current.horizontal == 0)))) &&
       (((a.animation.frame->flags[0] & 0x20) != 0 &&
        (double(a.motion.y) == double(0.0f))))) &&
      ((comparedPriority = priority, targetPriority = mode(0xcc),
       comparedPriority <= targetPriority ||
       (((priority == 99 ||
         (priority == 0x31)) ||
        (priority == 0x27)))))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0xcc);
    return;
  }
  if ((((i8(a.command) == '\x04') ||
       ((((a.controls.current.buttons[3] != 0 && (a.controls.current.vertical == -1)) &&
         ((int)i8(a.motion.facing) * a.controls.current.horizontal < 0)) ||
        (((a.controls.current.buttons[3] == 1 && (a.controls.current.vertical < -1)) &&
         (0x7fffffff < (u32)((int)i8(a.motion.facing) * a.controls.current.horizontal))))))) &&
      (((a.animation.frame->flags[0] & 0x20) != 0 &&
       (double(a.motion.y) == double(0.0f))))) &&
     ((((comparedPriority = priority, targetPriority = mode(0xce),
        comparedPriority <= targetPriority ||
        ((priority == 99 ||
         (priority == 0x31)))) ||
       (priority == 0x27)) &&
      ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
        (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))))) {
    a.reset_command_history();
    a.select_action(0xce);
    return;
  }
  if ((((i8(a.command) == '\x05') ||
       ((((a.controls.current.buttons[3] != 0 && (a.controls.current.vertical == -1)) &&
         (0 < (int)i8(a.motion.facing) * a.controls.current.horizontal)) ||
        (((a.controls.current.buttons[3] == 1 && (a.controls.current.vertical < -1)) &&
         (0 < (int)i8(a.motion.facing) * a.controls.current.horizontal)))))) &&
      ((((a.animation.frame->flags[0] & 0x20) != 0 &&
        (double(a.motion.y) == double(0.0f))) &&
       ((comparedPriority = priority, targetPriority = mode(0xcd),
        comparedPriority <= targetPriority ||
        (((priority == 99 ||
          (priority == 0x31)) ||
         (priority == 0x27)))))))) &&
     ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
       (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
    a.reset_command_history();
    a.select_action(0xcd);
    return;
  }
  if ((double(a.motion.y) == double(0.0f)) &&
     ((0 < a.controls.current.vertical || (0 < a.controls.buffered.vertical)))) {
    if (((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0)) &&
       ((((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal ||
          (0 < (int)i8(a.motion.facing) * a.controls.buffered.horizontal)) &&
         (i16(a.animation.cursor.action) == 200)) &&
        ((a.animation.frame->flags[0] & 0x20) != 0)))) {
      a.select_action(0x188);
      a.controls.buffered.clear();
      return;
    }
    if ((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0)) {
      if ((((0 < i16(a.combat.spirit)) &&
           ((a.animation.frame->flags[0] & 0x20) != 0)) &&
          (((double(460.0f) < double(a.motion.x) - double(other.motion.x) ||
            (double(a.motion.x) - double(other.motion.x) < -double(460.0f))) &&
           (comparedPriority = priority, targetPriority = mode(0x19a)
           , comparedPriority <= targetPriority)))) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x19a);
        a.controls.buffered.clear();
        return;
      }
      if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
          (comparedPriority = priority, targetPriority = mode(0x12e),
          comparedPriority <= targetPriority)) &&
         (((299 < i16(a.animation.cursor.action) &&
           ((a.response != 0 && (a.response != 3)))) ||
          (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x12e);
        a.controls.buffered.clear();
        return;
      }
    }
    if (((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[0] != 0)) &&
       (((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal ||
         (0 < (int)i8(a.motion.facing) * a.controls.buffered.horizontal)) &&
        ((i16(a.animation.cursor.action) == 200 &&
         ((a.animation.frame->flags[0] & 0x20) != 0)))))) {
      a.select_action(0x189);
      a.controls.buffered.clear();
      return;
    }
    if ((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[1] != 0)) {
      if (((0 < i16(a.combat.spirit)) &&
          ((a.animation.frame->flags[0] & 0x20) != 0)) &&
         ((((double(320.0f) < double(a.motion.x) - double(other.motion.x) ||
            (double(a.motion.x) - double(other.motion.x) < -double(320.0f))) &&
           (comparedPriority = priority, targetPriority = mode(0x1a0)
           , comparedPriority <= targetPriority)) &&
          ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
            (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))))) {
        a.select_action(0x1a0);
        a.controls.buffered.clear();
        return;
      }
      if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
          (comparedPriority = priority, targetPriority = mode(0x12f),
          comparedPriority <= targetPriority)) &&
         (((299 < i16(a.animation.cursor.action) &&
           ((a.response != 0 && (a.response != 3)))) ||
          (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x12f);
        a.controls.buffered.clear();
        return;
      }
    }
    if ((((a.controls.current.buttons[2] == 1) || (a.controls.buffered.buttons[2] != 0)) &&
        (0 < i16(a.combat.spirit))) &&
       ((((a.animation.frame->flags[0] & 0x20) != 0 &&
         (comparedPriority = priority, targetPriority = mode(0x192),
         comparedPriority <= targetPriority)) &&
        (((299 < i16(a.animation.cursor.action) &&
          ((a.response != 0 && (a.response != 3)))) ||
         (i16(a.animation.cursor.action) < 300)))))) {
      a.select_action(0x192);
      a.controls.buffered.clear();
      return;
    }
  }
  if ((double(a.motion.y) == double(0.0f)) && (a.controls.current.vertical == 0)) {
    if (((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0)) &&
       ((a.controls.current.vertical == 0 &&
        ((((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal ||
           (0 < (int)i8(a.motion.facing) * a.controls.buffered.horizontal)) &&
          (i16(a.animation.cursor.action) == 200)) &&
         ((a.animation.frame->flags[0] & 0x20) != 0)))))) {
      a.select_action(0x186);
      a.controls.buffered.clear();
      return;
    }
    if (((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0)) &&
       ((a.controls.current.vertical == 0 &&
        ((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal ||
         (0 < (int)i8(a.motion.facing) * a.controls.buffered.horizontal)))))) {
      if (((0 < i16(a.combat.spirit)) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (((double(480.0f) < double(a.motion.x) - double(other.motion.x) ||
             (double(a.motion.x) - double(other.motion.x) < -double(480.0f)))
            && (comparedPriority = priority,
               targetPriority = mode(0x198), comparedPriority <= targetPriority)))))) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x198);
        a.controls.buffered.clear();
        return;
      }
      if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
          (comparedPriority = priority, targetPriority = mode(0x133),
          comparedPriority <= targetPriority)) &&
         (((299 < i16(a.animation.cursor.action) &&
           ((a.response != 0 && (a.response != 3)))) ||
          (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x133);
        a.controls.buffered.clear();
        return;
      }
    }
    if ((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0)) {
      if ((((0 < i16(a.combat.spirit)) &&
           ((a.animation.frame->flags[0] & 0x20) != 0)) &&
          (((double(480.0f) < double(a.motion.x) - double(other.motion.x) ||
            (double(a.motion.x) - double(other.motion.x) < -double(480.0f))) &&
           (comparedPriority = priority, targetPriority = mode(0x198)
           , comparedPriority <= targetPriority)))) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x198);
        a.controls.buffered.clear();
        return;
      }
      if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
          (comparedPriority = priority, targetPriority = mode(300),
          comparedPriority <= targetPriority)) &&
         (((299 < i16(a.animation.cursor.action) &&
           ((a.response != 0 && (a.response != 3)))) ||
          (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(300);
        a.controls.buffered.clear();
        return;
      }
    }
    if ((((((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[0] != 0)) &&
          (a.controls.current.vertical == 0)) &&
         ((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal ||
          (0 < (int)i8(a.motion.facing) * a.controls.buffered.horizontal)))) &&
        (i16(a.animation.cursor.action) == 200)) &&
       ((a.animation.frame->flags[0] & 0x20) != 0)) {
      a.select_action(0x187);
      a.controls.buffered.clear();
      return;
    }
    if (((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[1] != 0)) &&
       ((a.controls.current.vertical == 0 &&
        ((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal ||
         (0 < (int)i8(a.motion.facing) * a.controls.buffered.horizontal)))))) {
      if (((0 < i16(a.combat.spirit)) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (((double(320.0f) < double(a.motion.x) - double(other.motion.x) ||
             (double(a.motion.x) - double(other.motion.x) < -double(320.0f)))
            && (comparedPriority = priority,
               targetPriority = mode(0x19f), comparedPriority <= targetPriority)))))) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x19f);
        a.controls.buffered.clear();
        return;
      }
      if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
          (comparedPriority = priority, targetPriority = mode(0x134),
          comparedPriority <= targetPriority)) &&
         (((299 < i16(a.animation.cursor.action) &&
           ((a.response != 0 && (a.response != 3)))) ||
          (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x134);
        a.controls.buffered.clear();
        return;
      }
    }
    if ((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[1] != 0)) {
      if (((0 < i16(a.combat.spirit)) &&
          ((a.animation.frame->flags[0] & 0x20) != 0)) &&
         ((((double(320.0f) < double(a.motion.x) - double(other.motion.x) ||
            (double(a.motion.x) - double(other.motion.x) < -double(320.0f))) &&
           (comparedPriority = priority, targetPriority = mode(0x19e)
           , comparedPriority <= targetPriority)) &&
          ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
            (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))))) {
        a.select_action(0x19e);
        a.controls.buffered.clear();
        return;
      }
      if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
          ((comparedPriority = priority, targetPriority = mode(0x12d)
           , comparedPriority <= targetPriority || (i16(a.animation.cursor.action) == 0x12d)))) &&
         (((299 < i16(a.animation.cursor.action) &&
           ((a.response != 0 && (a.response != 3)))) ||
          (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x12d);
        a.controls.buffered.clear();
        return;
      }
    }
    if ((((((a.controls.current.buttons[2] == 1) || (a.controls.buffered.buttons[2] != 0)) &&
          (a.controls.current.vertical == 0)) &&
         ((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal ||
          (0 < (int)i8(a.motion.facing) * a.controls.buffered.horizontal)))) &&
        (((a.animation.frame->flags[0] & 0x20) != 0 &&
         ((0 < i16(a.combat.spirit) &&
          (comparedPriority = priority, targetPriority = mode(0x191),
          comparedPriority <= targetPriority)))))) &&
       (((299 < i16(a.animation.cursor.action) &&
         ((a.response != 0 && (a.response != 3)))) ||
        (i16(a.animation.cursor.action) < 300)))) {
      a.select_action(0x191);
      a.controls.buffered.clear();
      return;
    }
    if (((((a.controls.current.buttons[2] == 1) || (a.controls.buffered.buttons[2] != 0)) &&
         ((a.animation.frame->flags[0] & 0x20) != 0)) &&
        ((0 < i16(a.combat.spirit) &&
         (comparedPriority = priority, targetPriority = mode(400),
         comparedPriority <= targetPriority)))) &&
       (((299 < i16(a.animation.cursor.action) &&
         ((a.response != 0 && (a.response != 3)))) ||
        (i16(a.animation.cursor.action) < 300)))) {
      a.select_action(400);
      a.controls.buffered.clear();
      return;
    }
  }
  if (double(0.0f) < double(a.motion.y)) {
    if ((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0)) {
      if (((((0 < i16(a.combat.spirit)) &&
            ((a.animation.frame->flags[0] & 0x20) != 0)) &&
           (((double(520.0f) < double(a.motion.x) - double(other.motion.x) ||
             (double(a.motion.x) - double(other.motion.x) < -double(520.0f)))
            && ((0 < a.controls.current.vertical || (0 < a.controls.buffered.vertical)))))) &&
          (comparedPriority = priority, targetPriority = mode(0x19d),
          comparedPriority <= targetPriority)) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x19d);
        a.controls.buffered.clear();
        return;
      }
      if (((((0 < i16(a.combat.spirit)) &&
            ((a.animation.frame->flags[0] & 0x20) != 0)) &&
           ((double(520.0f) < double(a.motion.x) - double(other.motion.x) ||
            (double(a.motion.x) - double(other.motion.x) < -double(520.0f)))))
          && (comparedPriority = priority,
             targetPriority = mode(0x19c), comparedPriority <= targetPriority)) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x19c);
        a.controls.buffered.clear();
        return;
      }
      if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
          (comparedPriority = priority, targetPriority = mode(0x130),
          comparedPriority <= targetPriority)) &&
         (((299 < i16(a.animation.cursor.action) &&
           ((a.response != 0 && (a.response != 3)))) ||
          (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x130);
        a.controls.buffered.clear();
        return;
      }
    }
    if ((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[1] != 0)) {
      if ((((0 < i16(a.combat.spirit)) &&
           ((a.animation.frame->flags[0] & 0x20) != 0)) &&
          (((double(400.0f) < double(a.motion.x) - double(other.motion.x) ||
            (double(a.motion.x) - double(other.motion.x) < -double(400.0f))) &&
           (comparedPriority = priority, targetPriority = mode(0x1a2)
           , comparedPriority <= targetPriority)))) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x1a2);
        a.controls.buffered.clear();
        return;
      }
      if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
          (comparedPriority = priority, targetPriority = mode(0x131),
          comparedPriority <= targetPriority)) &&
         (((299 < i16(a.animation.cursor.action) &&
           ((a.response != 0 && (a.response != 3)))) ||
          (i16(a.animation.cursor.action) < 300)))) {
        a.select_action(0x131);
        a.controls.buffered.clear();
        return;
      }
    }
    if (((((a.controls.current.buttons[2] == 1) || (a.controls.buffered.buttons[2] != 0)) &&
         ((0 < a.controls.current.vertical || (0 < a.controls.buffered.vertical)))) &&
        ((((a.animation.frame->flags[0] & 0x20) != 0 &&
          (0 < i16(a.combat.spirit))) &&
         (comparedPriority = priority, targetPriority = mode(0x194),
         comparedPriority <= targetPriority)))) &&
       ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
         (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
      a.select_action(0x194);
      a.controls.buffered.clear();
      return;
    }
    if ((((a.controls.current.buttons[2] == 1) || (a.controls.buffered.buttons[2] != 0)) &&
        (((a.animation.frame->flags[0] & 0x20) != 0 &&
         ((0 < i16(a.combat.spirit) &&
          (comparedPriority = priority, targetPriority = mode(0x197),
          comparedPriority <= targetPriority)))))) &&
       (((299 < i16(a.animation.cursor.action) &&
         ((a.response != 0 && (a.response != 3)))) ||
        (i16(a.animation.cursor.action) < 300)))) {
      a.select_action(0x197);
      a.controls.buffered.clear();
      return;
    }
    if ((a.controls.current.buttons[3] != 0) || (a.controls.buffered.buttons[3] != 0)) {
      if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
          ((i8(a.movement.usedAirDashes) < '\x02' &&
           ((int)i8(a.motion.facing) * a.controls.current.horizontal == 1)))) &&
         ((((double(60.0f) <= double(a.motion.y) || (double(a.motion.velocityY) <= double(0.0f))) &&
           (((comparedPriority = priority,
             targetPriority = mode(0xca), comparedPriority <= targetPriority ||
             (priority == 99)) ||
            (priority == 0x31)))) &&
          ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
            (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))))) {
        a.reset_command_history();
        a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
        a.select_action(0xca);
        return;
      }
      if (((((a.animation.frame->flags[0] & 0x20) != 0) &&
           (i8(a.movement.usedAirDashes) < '\x02')) &&
          (((int)i8(a.motion.facing) * a.controls.current.horizontal == -1 &&
           (((comparedPriority = priority,
             targetPriority = mode(0xcb), comparedPriority <= targetPriority ||
             (priority == 99)) ||
            (priority == 0x31)))))) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.reset_command_history();
        a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
        a.select_action(0xcb);
        return;
      }
    }
    if ((a.controls.current.buttons[3] == 1) || (a.controls.buffered.buttons[3] == 1)) {
      if ((((((a.animation.frame->flags[0] & 0x20) != 0) &&
            ((i8(a.movement.usedAirDashes) < '\x02' &&
             (0 < (int)i8(a.motion.facing) * a.controls.current.horizontal)))) &&
           ((double(60.0f) <= double(a.motion.y) || (double(a.motion.velocityY) <= double(0.0f))))) &&
          (((comparedPriority = priority, targetPriority = mode(0xca)
            , comparedPriority <= targetPriority ||
            (priority == 99)) ||
           (priority == 0x31)))) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.reset_command_history();
        a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
        a.select_action(0xca);
        return;
      }
      if (((((a.animation.frame->flags[0] & 0x20) != 0) &&
           (i8(a.movement.usedAirDashes) < '\x02')) &&
          (((int)i8(a.motion.facing) * a.controls.current.horizontal < 0 &&
           (((comparedPriority = priority,
             targetPriority = mode(0xcb), comparedPriority <= targetPriority ||
             (priority == 99)) ||
            (priority == 0x31)))))) &&
         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
           (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
        a.reset_command_history();
        a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
        a.select_action(0xcb);
      }
    }
  }
  return;

}
}
