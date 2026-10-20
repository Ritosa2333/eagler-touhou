#include "YoumuState.hpp"
namespace th075 {
void select_youmu_commands(FighterActor& a,const FighterActor& other,YoumuState& s){
    if(!a.animation.pattern||!a.animation.frame)return;
    const auto& sequences=a.animation.pattern->sequences;
    const i16 action=a.animation.cursor.action;
    if(action<0||u32(action)>=sequences.size())return;
    const i16 priority=i16(sequences[action].next);
    const auto mode=[&](i16 id){return id>=0&&u32(id)<sequences.size()&&sequences[id].present?i16(sequences[id].mode):i16(-32768);};

  i16 comparedPriority = 0;
  i16 targetPriority = 0;
  
  if ((((i8(a.command) == -0x1a) && (double(a.motion.y) == double(0.0f))) &&
      ((a.animation.frame->flags[0] & 0x20) != 0)) &&
     (comparedPriority = priority, targetPriority = mode(0x2f8),
     comparedPriority <= targetPriority)) {
    a.reset_command_history();
    a.select_action(0x2f8);
  }
  else if (((i8(a.command) == -0x24) && (double(a.motion.y) == double(0.0f))) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (comparedPriority = priority, targetPriority = mode(0x2f0)
           , comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2f0);
  }
  else if (((i8(a.command) == -0x2e) && (double(a.motion.y) == double(0.0f))) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (comparedPriority = priority, targetPriority = mode(0x2e9)
           , comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    if (s.difficulty == '\0') {
      a.select_action(0x2ea);
    }
    if (s.difficulty == '\x01') {
      a.select_action(0x2e9);
    }
    if (s.difficulty == '\x02') {
      a.select_action(0x2e8);
    }
    if (s.difficulty == '\x03') {
      a.select_action(0x2e7);
    }
  }
  else if ((((i8(a.command) == -0x36) && (double(a.motion.y) == double(0.0f))) &&
           ((a.animation.frame->flags[0] & 0x20) != 0)) &&
          (comparedPriority = priority, targetPriority = mode(0x2dd),
          comparedPriority <= targetPriority)) {
    a.reset_command_history();
    a.select_action(0x2dd);
  }
  else if (((i8(a.command) == -0x37) && (double(a.motion.y) == double(0.0f))) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (comparedPriority = priority, targetPriority = mode(0x2dc)
           , comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2dc);
  }
  else if (((i8(a.command) == -0x38) && (double(a.motion.y) == double(0.0f))) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (comparedPriority = priority, targetPriority = mode(0x2db)
           , comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2db);
  }
  else if (((((i8(a.command) != 'g') || (double(a.motion.y) != double(0.0f))) ||
            (double(a.combat.spellExtent) < double(100.0f))) ||
           (((i8(a.combat.spellEnded) != '\0' || (0 < i16(a.combat.spellTimer))) ||
            (((a.animation.frame->flags[0] & 0x20) == 0 ||
             (comparedPriority = priority,
             targetPriority = mode(0x31b), targetPriority < comparedPriority)))))) ||
          ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
            (a.response == 3)) && (299 < i16(a.animation.cursor.action))))) {
    if ((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r')) ||
        (double(a.motion.y) != double(0.0f))) ||
       (((((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f))) ||
          ((i16(a.spellChoices[1]) != 0 ||
           ((i8(a.combat.spellPhase) != '\x01' ||
            ((a.animation.frame->flags[0] & 0x20) == 0)))))) ||
         (comparedPriority = priority, targetPriority = mode(0x2bd),
         targetPriority < comparedPriority)) ||
        ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
          (a.response == 3)) && (299 < i16(a.animation.cursor.action))))))) {
      if (((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r')) ||
         (((double(a.motion.y) != double(0.0f) ||
           ((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f))))) ||
          (((i16(a.spellChoices[1]) != 1 ||
            (((i8(a.combat.spellPhase) != '\x01' ||
              ((a.animation.frame->flags[0] & 0x20) == 0)) ||
             (comparedPriority = priority,
             targetPriority = mode(0x2bf), targetPriority < comparedPriority)))) ||
           ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
             (a.response == 3)) && (299 < i16(a.animation.cursor.action))))))))) {
        if (((i16(a.combat.spirit) < 0xfb) || (i16(s.phantomAttacking) != 0)) ||
           ((((i8(a.command) != '\r' ||
              (((double(a.motion.y) != double(0.0f) || (i16(a.combat.spellTimer) < 1)) ||
               (double(a.combat.spellExtent) < double(100.0f))))) ||
             (((i16(a.spellChoices[1]) != 2 || (i8(a.combat.spellPhase) != '\x01')) ||
              ((i16(s.phantomTime) != 0 ||
               (((a.animation.frame->flags[0] & 0x20) == 0 ||
                (comparedPriority = priority,
                targetPriority = mode(0x2c1), targetPriority < comparedPriority)))))))) ||
            (((i16(a.animation.cursor.action) < 300 ||
              ((a.response == 0 || (a.response == 3)))) &&
             (299 < i16(a.animation.cursor.action))))))) {
          if ((((((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r')) ||
                  (double(a.motion.y) != double(0.0f))) ||
                 ((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f))))) ||
                (i16(a.spellChoices[0]) != 0)) ||
               ((i8(a.combat.spellPhase) != '\x02' ||
                ((a.animation.frame->flags[0] & 0x20) == 0)))) ||
              (comparedPriority = priority,
              targetPriority = mode(700), targetPriority < comparedPriority)) ||
             ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
               (a.response == 3)) && (299 < i16(a.animation.cursor.action))))) {
            if ((((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r')) ||
                  ((double(a.motion.y) != double(0.0f) ||
                   ((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f)))))))
                 || (i16(a.spellChoices[0]) != 1)) ||
                (((i8(a.combat.spellPhase) != '\x02' ||
                  ((a.animation.frame->flags[0] & 0x20) == 0)) ||
                 (comparedPriority = priority,
                 targetPriority = mode(0x2be), targetPriority < comparedPriority)))) ||
               ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
                 (a.response == 3)) && (299 < i16(a.animation.cursor.action))))) {
              if ((((((i16(a.combat.spirit) < 0xfb) || (i16(s.phantomAttacking) != 0)) ||
                    ((i8(a.command) != '\r' ||
                     ((((double(a.motion.y) != double(0.0f) || (i16(a.combat.spellTimer) < 1)) ||
                       (double(a.combat.spellExtent) < double(100.0f))) ||
                      ((i16(a.spellChoices[0]) != 2 || (i8(a.combat.spellPhase) != '\x02')))
                      ))))) || (i16(s.phantomTime) != 0)) ||
                  (((a.animation.frame->flags[0] & 0x20) == 0 ||
                   (comparedPriority = priority,
                   targetPriority = mode(0x2c0), targetPriority < comparedPriority)))) ||
                 (((i16(a.animation.cursor.action) < 300 ||
                   ((a.response == 0 || (a.response == 3)))) &&
                  (299 < i16(a.animation.cursor.action))))) {
                if ((((('\0' < i8(a.selectedSpell)) && (double(a.motion.y) == double(0.0f)))
                     && (i8(a.command) == 'f')) &&
                    (((a.animation.frame->flags[0] & 0x20) != 0 &&
                     (comparedPriority = priority,
                     targetPriority = mode(0x252), comparedPriority <= targetPriority)))) &&
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
                    if ((0x95 < i16(a.animation.cursor.action)) && (i16(a.animation.cursor.action) < 0x9f))
                    {
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
                if ((((i8(a.selectedSpell) < '\x01') || (double(a.motion.y) != double(0.0f)))
                    || (i8(a.command) != 'f')) ||
                   ((((a.animation.frame->flags[0] & 0x20) == 0 ||
                     (comparedPriority = priority,
                     targetPriority = mode(0x254), targetPriority < comparedPriority)) ||
                    ((299 < i16(a.animation.cursor.action) ||
                     (a.reset_command_history(), double(a.motion.y) != double(0.0f))))))) {
                  if ((((i8(a.command) == '2') && (double(a.motion.y) == double(0.0f)))
                      && ((0 < i16(a.combat.spirit) &&
                          (((a.animation.frame->flags[0] & 0x20) != 0 &&
                           (comparedPriority = priority,
                           targetPriority = mode(0x1fd), comparedPriority <= targetPriority)
                           ))))) &&
                     (((299 < i16(a.animation.cursor.action) &&
                       ((a.response != 0 && (a.response != 3)))) ||
                      (i16(a.animation.cursor.action) < 300)))) {
                    a.reset_command_history();
                    a.select_action(0x1fd);
                  }
                  else if (((((i8(a.command) == '3') &&
                             (double(a.motion.y) == double(0.0f))) &&
                            (0 < i16(a.combat.spirit))) &&
                           (((a.animation.frame->flags[0] & 0x20) != 0 &&
                            (comparedPriority = priority,
                            targetPriority = mode(0x1fe), comparedPriority <= targetPriority
                            )))) && (((299 < i16(a.animation.cursor.action) &&
                                      ((a.response != 0 &&
                                       (a.response != 3)))) ||
                                     (i16(a.animation.cursor.action) < 300)))) {
                    a.reset_command_history();
                    a.select_action(0x1fe);
                  }
                  else if (((((i8(a.command) == '\x1f') &&
                             (double(a.motion.y) == double(0.0f))) &&
                            (0 < i16(a.combat.spirit))) &&
                           (((a.animation.frame->flags[0] & 0x20) != 0 &&
                            (comparedPriority = priority,
                            targetPriority = mode(0x1f5), comparedPriority <= targetPriority
                            )))) && (((299 < i16(a.animation.cursor.action) &&
                                      ((a.response != 0 &&
                                       (a.response != 3)))) ||
                                     (i16(a.animation.cursor.action) < 300)))) {
                    a.reset_command_history();
                    a.select_action(0x1f5);
                  }
                  else if (((((i8(a.command) == '\x1e') &&
                             (double(a.motion.y) == double(0.0f))) &&
                            (0 < i16(a.combat.spirit))) &&
                           (((a.animation.frame->flags[0] & 0x20) != 0 &&
                            (comparedPriority = priority,
                            targetPriority = mode(500), comparedPriority <= targetPriority))
                           )) && (((299 < i16(a.animation.cursor.action) &&
                                   ((a.response != 0 &&
                                    (a.response != 3)))) ||
                                  (i16(a.animation.cursor.action) < 300)))) {
                    a.reset_command_history();
                    a.select_action(500);
                  }
                  else if (((((i8(a.command) == '\f') &&
                             (double(a.motion.y) == double(0.0f))) &&
                            ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                           ((0 < i16(a.combat.spirit) &&
                            (comparedPriority = priority,
                            targetPriority = mode(600), comparedPriority <= targetPriority))
                           )) && (((299 < i16(a.animation.cursor.action) &&
                                   ((a.response != 0 &&
                                    (a.response != 3)))) ||
                                  (i16(a.animation.cursor.action) < 300)))) {
                    a.reset_command_history();
                    a.select_action(600);
                  }
                  else if (((((i8(a.command) == '\x15') &&
                             (0 < i16(a.combat.spirit))) &&
                            ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                           ((double(a.motion.y) == double(0.0f) &&
                            (comparedPriority = priority,
                            targetPriority = mode(0x201), comparedPriority <= targetPriority
                            )))) && (((299 < i16(a.animation.cursor.action) &&
                                      ((a.response != 0 &&
                                       (a.response != 3)))) ||
                                     (i16(a.animation.cursor.action) < 300)))) {
                    a.reset_command_history();
                    a.select_action(0x201);
                  }
                  else if ((((i8(a.command) == '\x14') &&
                            (0 < i16(a.combat.spirit))) &&
                           ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                          (((double(a.motion.y) == double(0.0f) &&
                            (comparedPriority = priority,
                            targetPriority = mode(0x1f6), comparedPriority <= targetPriority
                            )) && (((299 < i16(a.animation.cursor.action) &&
                                    ((a.response != 0 &&
                                     (a.response != 3)))) ||
                                   (i16(a.animation.cursor.action) < 300)))))) {
                    a.reset_command_history();
                    a.select_action(0x1f6);
                  }
                  else if (((((i8(a.command) == 'e') &&
                             ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                            (double(a.motion.y) == double(0.0f))) &&
                           (comparedPriority = priority,
                           targetPriority = mode(0x135), comparedPriority <= targetPriority)
                           ) && ((((299 < i16(a.animation.cursor.action) &&
                                   (a.response != 0)) &&
                                  (a.response != 3)) ||
                                 (i16(a.animation.cursor.action) < 300)))) {
                    a.reset_command_history();
                    a.select_action(0x135);
                  }
                  else if ((((i8(a.command) == 'd') &&
                            (double(a.motion.y) == double(0.0f))) &&
                           (((a.animation.frame->flags[0] & 0x20) != 0 &&
                            (comparedPriority = priority,
                            targetPriority = mode(0x136), comparedPriority <= targetPriority
                            )))) && ((((299 < i16(a.animation.cursor.action) &&
                                       (a.response != 0)) &&
                                      (a.response != 3)) ||
                                     (i16(a.animation.cursor.action) < 300)))) {
                    a.reset_command_history();
                    a.select_action(0x136);
                  }
                  else if (((((i8(a.command) == '\x01') &&
                             (i8(a.motion.facing) == '\x01')) ||
                            ((i8(a.command) == '\x02' &&
                             (i8(a.motion.facing) == -1)))) ||
                           (((a.controls.current.buttons[3] != 0 &&
                             ((int)i8(a.motion.facing) * a.controls.current.horizontal == 1)) ||
                            ((a.controls.current.buttons[3] == 1 &&
                             (0 < (int)i8(a.motion.facing) * a.controls.current.horizontal))))))
                          && ((((double(a.motion.y) == double(0.0f) &&
                                (i16(a.animation.cursor.action) != 0xcf)) &&
                               ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                              ((comparedPriority = priority,
                               targetPriority = mode(200),
                               comparedPriority <= targetPriority &&
                               ((((299 < i16(a.animation.cursor.action) &&
                                  (a.response != 0)) &&
                                 (a.response != 3)) ||
                                (i16(a.animation.cursor.action) < 300)))))))) {
                    a.reset_command_history();
                    a.select_action(200);
                  }
                  else if ((((((i8(a.command) != '\x01') ||
                              (i8(a.motion.facing) != '\x01')) &&
                             (((i8(a.command) != '\x02' ||
                               (i8(a.motion.facing) != -1)) &&
                              ((a.controls.current.buttons[3] == 0 ||
                               ((int)i8(a.motion.facing) * a.controls.current.horizontal != 1)))))
                             ) && ((a.controls.current.buttons[3] != 1 ||
                                   ((int)i8(a.motion.facing) * a.controls.current.horizontal < 1))
                                  )) ||
                           (((double(a.motion.y) <= double(0.0f) ||
                             ((((double(a.motion.y) < double(60.0f) &&
                                (double(0.0f) < double(a.motion.velocityY))) ||
                               ((a.animation.frame->flags[0] & 0x20) == 0)) ||
                              ('\x01' < i8(a.movement.usedAirDashes))))) ||
                            (((comparedPriority = priority,
                              targetPriority = mode(0xca), targetPriority < comparedPriority
                              && (priority != 99)) &&
                             (priority != 0x31)))))) ||
                          ((((i16(a.animation.cursor.action) < 300 || (a.response == 0))
                            || (a.response == 3)) &&
                           (299 < i16(a.animation.cursor.action))))) {
                    if (((((i8(a.command) == '\x02') &&
                          (i8(a.motion.facing) == '\x01')) ||
                         ((i8(a.command) == '\x01' && (i8(a.motion.facing) == -1)
                          ))) || (((a.controls.current.buttons[3] != 0 &&
                                   ((int)i8(a.motion.facing) * a.controls.current.horizontal == -1
                                   )) || ((a.controls.current.buttons[3] == 1 &&
                                          ((int)i8(a.motion.facing) *
                                           a.controls.current.horizontal < 0)))))) &&
                       ((((double(a.motion.y) == double(0.0f) &&
                          ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                         (comparedPriority = priority,
                         targetPriority = mode(0xc9), comparedPriority <= targetPriority))
                        && ((((299 < i16(a.animation.cursor.action) && (a.response != 0))
                             && (a.response != 3)) ||
                            (i16(a.animation.cursor.action) < 300)))))) {
                      a.reset_command_history();
                      a.select_action(0xc9);
                    }
                    else if ((((((i8(a.command) != '\x02') ||
                                (i8(a.motion.facing) != '\x01')) &&
                               (((i8(a.command) != '\x01' ||
                                 (i8(a.motion.facing) != -1)) &&
                                ((a.controls.current.buttons[3] == 0 ||
                                 ((int)i8(a.motion.facing) * a.controls.current.horizontal != -1))
                                )))) && ((a.controls.current.buttons[3] != 1 ||
                                         (-1 < (int)i8(a.motion.facing) *
                                               a.controls.current.horizontal)))) ||
                             ((((double(a.motion.y) <= double(0.0f) ||
                                ((a.animation.frame->flags[0] & 0x20) == 0)) ||
                               ('\x01' < i8(a.movement.usedAirDashes))) ||
                              (((comparedPriority = priority,
                                targetPriority = mode(0xcb),
                                targetPriority < comparedPriority &&
                                (priority != 99)) &&
                               (priority != 0x31)))))) ||
                            ((((i16(a.animation.cursor.action) < 300 || (a.response == 0))
                              || (a.response == 3)) &&
                             (299 < i16(a.animation.cursor.action))))) {
                      if (((((i8(a.command) == '\x03') ||
                            (((a.controls.current.buttons[3] != 0 && (a.controls.current.vertical == -1))
                             && (a.controls.current.horizontal == 0)))) ||
                           (((a.controls.current.buttons[3] == 1 && (a.controls.current.vertical < -1)) &&
                            (a.controls.current.horizontal == 0)))) &&
                          ((((a.animation.frame->flags[0] & 0x20) != 0 &&
                            (double(a.motion.y) == double(0.0f))) &&
                           ((comparedPriority = priority,
                            targetPriority = mode(0xcc), comparedPriority <= targetPriority
                            || ((priority == 99 ||
                                (priority == 0x31)))))))) &&
                         (((299 < i16(a.animation.cursor.action) &&
                           ((a.response != 0 && (a.response != 3)))) ||
                          (i16(a.animation.cursor.action) < 300)))) {
                        a.reset_command_history();
                        a.select_action(0xcc);
                      }
                      else if ((((((i8(a.command) == '\x04') ||
                                  (((a.controls.current.buttons[3] != 0 &&
                                    (a.controls.current.vertical == -1)) &&
                                   ((int)i8(a.motion.facing) * a.controls.current.horizontal < 0))
                                  )) || (((a.controls.current.buttons[3] == 1 &&
                                          (a.controls.current.vertical < -1)) &&
                                         (0x7fffffff <
                                          (u32)((int)i8(a.motion.facing) *
                                                a.controls.current.horizontal))))) &&
                                (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                 (double(a.motion.y) == double(0.0f))))) &&
                               ((comparedPriority = priority,
                                targetPriority = mode(0xce),
                                comparedPriority <= targetPriority ||
                                ((priority == 99 ||
                                 (priority == 0x31)))))) &&
                              (((299 < i16(a.animation.cursor.action) &&
                                ((a.response != 0 && (a.response != 3)))
                                ) || (i16(a.animation.cursor.action) < 300)))) {
                        a.reset_command_history();
                        a.select_action(0xce);
                      }
                      else if ((((((i8(a.command) == '\x05') ||
                                  (((a.controls.current.buttons[3] != 0 &&
                                    (a.controls.current.vertical == -1)) &&
                                   (0 < (int)i8(a.motion.facing) * a.controls.current.horizontal))
                                  )) || (((a.controls.current.buttons[3] == 1 &&
                                          (a.controls.current.vertical < -1)) &&
                                         (0 < (int)i8(a.motion.facing) *
                                              a.controls.current.horizontal)))) &&
                                (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                 (double(a.motion.y) == double(0.0f))))) &&
                               ((comparedPriority = priority,
                                targetPriority = mode(0xcd),
                                comparedPriority <= targetPriority ||
                                ((priority == 99 ||
                                 (priority == 0x31)))))) &&
                              (((299 < i16(a.animation.cursor.action) &&
                                ((a.response != 0 && (a.response != 3)))
                                ) || (i16(a.animation.cursor.action) < 300)))) {
                        a.reset_command_history();
                        a.select_action(0xcd);
                      }
                      else {
                        if ((double(a.motion.y) == double(0.0f)) &&
                           ((0 < a.controls.current.vertical || (0 < a.controls.buffered.vertical)))) {
                          if (((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0))
                             && ((((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal
                                   || (0 < (int)i8(a.motion.facing) *
                                           a.controls.buffered.horizontal)) &&
                                  (i16(a.animation.cursor.action) == 200)) &&
                                 ((a.animation.frame->flags[0] & 0x20) != 0)))) {
                            a.select_action(0x188);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((((a.controls.current.buttons[0] == 1) ||
                                 (a.controls.buffered.buttons[0] != 0)) &&
                                ((0 < i16(a.combat.spirit) &&
                                 ((a.animation.frame->flags[0] & 0x20) != 0)))) &&
                               ((double(320.0f) < double(a.motion.x) -
                                         double(other.motion.x) ||
                                (double(a.motion.x) -
                                 double(other.motion.x) < -double(320.0f))))) &&
                              (comparedPriority = priority,
                              targetPriority = mode(0x198),
                              comparedPriority <= targetPriority)) &&
                             ((((299 < i16(a.animation.cursor.action) && (a.response != 0)
                                ) && (a.response != 3)) ||
                              (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(0x198);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0)
                               ) && (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                     (comparedPriority = priority,
                                     targetPriority = mode(0x12e),
                                     comparedPriority <= targetPriority)))) &&
                             ((((299 < i16(a.animation.cursor.action) && (a.response != 0)
                                ) && (a.response != 3)) ||
                              (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(0x12e);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[0] != 0)
                               ) && ((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal
                                     || (0 < (int)i8(a.motion.facing) *
                                             a.controls.buffered.horizontal)))) &&
                             ((i16(a.animation.cursor.action) == 200 &&
                              ((a.animation.frame->flags[0] & 0x20) != 0)))) {
                            a.select_action(0x189);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((((a.controls.current.buttons[1] == 1) ||
                                 (a.controls.buffered.buttons[1] != 0)) &&
                                (i16(s.phantomAttacking) == 0)) &&
                               (((0 < i16(a.combat.spirit) &&
                                 ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                                ((double(320.0f) < double(a.motion.x) -
                                          double(other.motion.x) ||
                                 (double(a.motion.x) -
                                  double(other.motion.x) < -double(320.0f))))))) &&
                              (comparedPriority = priority,
                              targetPriority = mode(0x19e),
                              comparedPriority <= targetPriority)) &&
                             ((((299 < i16(a.animation.cursor.action) && (a.response != 0)
                                ) && (a.response != 3)) ||
                              (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(0x19e);
                            a.controls.buffered.clear();
                            return;
                          }
                          if (((((a.controls.current.buttons[1] == 1) ||
                                (a.controls.buffered.buttons[1] != 0)) &&
                               ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                              (((comparedPriority = priority,
                                targetPriority = mode(0x12f),
                                comparedPriority <= targetPriority ||
                                (i16(a.animation.cursor.action) == 300)) ||
                               (i16(a.animation.cursor.action) == 0x12f)))) &&
                             ((((299 < i16(a.animation.cursor.action) && (a.response != 0)
                                ) && (a.response != 3)) ||
                              (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(0x12f);
                            a.controls.buffered.clear();
                            return;
                          }
                          if (((((a.controls.current.buttons[2] == 1) ||
                                (a.controls.buffered.buttons[2] != 0)) &&
                               ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                              ((0 < i16(a.combat.spirit) &&
                               (comparedPriority = priority,
                               targetPriority = mode(0x192),
                               comparedPriority <= targetPriority)))) &&
                             (((299 < i16(a.animation.cursor.action) &&
                               ((a.response != 0 && (a.response != 3))))
                              || (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(0x192);
                            a.controls.buffered.clear();
                            return;
                          }
                        }
                        if ((double(a.motion.y) == double(0.0f)) && (a.controls.current.vertical == 0))
                        {
                          if (((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0))
                             && ((a.controls.current.vertical == 0 &&
                                 ((((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal
                                    || (0 < (int)i8(a.motion.facing) *
                                            a.controls.buffered.horizontal)) &&
                                   (i16(a.animation.cursor.action) == 200)) &&
                                  ((a.animation.frame->flags[0] & 0x20) != 0)))))) {
                            a.select_action(0x186);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((((a.controls.current.buttons[0] == 1) ||
                                 (a.controls.buffered.buttons[0] != 0)) &&
                                ((a.controls.current.vertical == 0 &&
                                 ((0 < i16(a.combat.spirit) &&
                                  ((a.animation.frame->flags[0] & 0x20) != 0)))))) &&
                               ((double(320.0f) < double(a.motion.x) -
                                         double(other.motion.x) ||
                                (double(a.motion.x) -
                                 double(other.motion.x) < -double(320.0f))))) &&
                              (comparedPriority = priority,
                              targetPriority = mode(0x196),
                              comparedPriority <= targetPriority)) &&
                             ((((299 < i16(a.animation.cursor.action) && (a.response != 0)
                                ) && (a.response != 3)) ||
                              (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(0x196);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0)
                               ) && ((a.controls.current.vertical == 0 &&
                                     ((0 < (int)i8(a.motion.facing) *
                                           a.controls.current.horizontal ||
                                      (0 < (int)i8(a.motion.facing) *
                                           a.controls.buffered.horizontal)))))) &&
                             (((a.animation.frame->flags[0] & 0x20) != 0 &&
                              ((comparedPriority = priority,
                               targetPriority = mode(0x134),
                               comparedPriority <= targetPriority &&
                               ((((299 < i16(a.animation.cursor.action) &&
                                  (a.response != 0)) &&
                                 (a.response != 3)) ||
                                (i16(a.animation.cursor.action) < 300)))))))) {
                            a.select_action(0x134);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0)
                               ) && ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                             ((((comparedPriority = priority,
                                targetPriority = mode(300),
                                comparedPriority <= targetPriority ||
                                (i16(a.animation.cursor.action) == 300)) ||
                               (i16(a.animation.cursor.action) == 0x12f)) &&
                              ((((299 < i16(a.animation.cursor.action) &&
                                 (a.response != 0)) && (a.response != 3)
                                ) || (i16(a.animation.cursor.action) < 300)))))) {
                            a.select_action(300);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[0] != 0)
                               ) && (((a.controls.current.vertical == 0 &&
                                      ((0 < (int)i8(a.motion.facing) *
                                            a.controls.current.horizontal ||
                                       (0 < (int)i8(a.motion.facing) *
                                            a.controls.buffered.horizontal)))) &&
                                     (i16(a.animation.cursor.action) == 200)))) &&
                             ((a.animation.frame->flags[0] & 0x20) != 0)) {
                            a.select_action(0x187);
                            a.controls.buffered.clear();
                            return;
                          }
                          if (((((a.controls.current.buttons[1] == 1) ||
                                (a.controls.buffered.buttons[1] != 0)) &&
                               (a.controls.current.vertical == 0)) &&
                              (((i16(s.phantomAttacking) == 0 &&
                                (0 < i16(a.combat.spirit))) &&
                               (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                ((double(320.0f) < double(a.motion.x) -
                                          double(other.motion.x) ||
                                 (double(a.motion.x) -
                                  double(other.motion.x) < -double(320.0f))))))))) &&
                             ((comparedPriority = priority,
                              targetPriority = mode(0x19c),
                              comparedPriority <= targetPriority &&
                              ((((299 < i16(a.animation.cursor.action) &&
                                 (a.response != 0)) && (a.response != 3)
                                ) || (i16(a.animation.cursor.action) < 300)))))) {
                            a.select_action(0x19c);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[1] != 0)
                               ) && (((a.controls.current.vertical == 0 &&
                                      ((0 < (int)i8(a.motion.facing) *
                                            a.controls.current.horizontal ||
                                       (0 < (int)i8(a.motion.facing) *
                                            a.controls.buffered.horizontal)))) &&
                                     (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                      (comparedPriority = priority,
                                      targetPriority = mode(0x133),
                                      comparedPriority <= targetPriority)))))) &&
                             ((((299 < i16(a.animation.cursor.action) && (a.response != 0)
                                ) && (a.response != 3)) ||
                              (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(0x133);
                            a.controls.buffered.clear();
                            return;
                          }
                          if ((((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[1] != 0)
                               ) && (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                     (comparedPriority = priority,
                                     targetPriority = mode(0x12d),
                                     comparedPriority <= targetPriority)))) &&
                             ((((299 < i16(a.animation.cursor.action) && (a.response != 0)
                                ) && (a.response != 3)) ||
                              (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(0x12d);
                            a.controls.buffered.clear();
                            return;
                          }
                          if (((((a.controls.current.buttons[2] == 1) ||
                                (a.controls.buffered.buttons[2] != 0)) &&
                               ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                              ((0 < i16(a.combat.spirit) &&
                               (comparedPriority = priority,
                               targetPriority = mode(400),
                               comparedPriority <= targetPriority)))) &&
                             (((299 < i16(a.animation.cursor.action) &&
                               ((a.response != 0 && (a.response != 3))))
                              || (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(400);
                            a.controls.buffered.clear();
                            return;
                          }
                        }
                        if (double(0.0f) < double(a.motion.y)) {
                          if (((a.controls.current.buttons[0] == 1) || (a.controls.buffered.buttons[0] != 0))
                             && ((a.animation.frame->flags[0] & 0x20) != 0)) {
                            if (((((0 < i16(a.combat.spirit)) &&
                                  ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                                 ((double(320.0f) < double(a.motion.x) -
                                           double(other.motion.x) ||
                                  (double(a.motion.x) -
                                   double(other.motion.x) < -double(320.0f))))) &&
                                (comparedPriority = priority,
                                targetPriority = mode(0x19a),
                                comparedPriority <= targetPriority)) &&
                               ((((299 < i16(a.animation.cursor.action) &&
                                  (a.response != 0)) &&
                                 (a.response != 3)) ||
                                (i16(a.animation.cursor.action) < 300)))) {
                              a.select_action(0x19a);
                              a.controls.buffered.clear();
                              return;
                            }
                            comparedPriority = priority;
                            targetPriority = mode(0x130);
                            if ((comparedPriority <= targetPriority) &&
                               ((((299 < i16(a.animation.cursor.action) &&
                                  (a.response != 0)) &&
                                 (a.response != 3)) ||
                                (i16(a.animation.cursor.action) < 300)))) {
                              a.select_action(0x130);
                              a.controls.buffered.clear();
                              return;
                            }
                          }
                          if ((a.controls.current.buttons[1] == 1) || (a.controls.buffered.buttons[1] != 0))
                          {
                            if (((i16(s.phantomAttacking) == 0) &&
                                ((0 < i16(a.combat.spirit) &&
                                 ((a.animation.frame->flags[0] & 0x20) != 0)))) &&
                               ((((double(320.0f) < double(a.motion.x) -
                                           double(other.motion.x) ||
                                  (double(a.motion.x) -
                                   double(other.motion.x) < -double(320.0f))) &&
                                 (comparedPriority = priority,
                                 targetPriority = mode(0x1a0),
                                 comparedPriority <= targetPriority)) &&
                                ((((299 < i16(a.animation.cursor.action) &&
                                   (a.response != 0)) &&
                                  (a.response != 3)) ||
                                 (i16(a.animation.cursor.action) < 300)))))) {
                              a.select_action(0x1a0);
                              a.controls.buffered.clear();
                              return;
                            }
                            if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
                                (comparedPriority = priority,
                                targetPriority = mode(0x131),
                                comparedPriority <= targetPriority)) &&
                               (((299 < i16(a.animation.cursor.action) &&
                                 ((a.response != 0 && (a.response != 3))
                                 )) || (i16(a.animation.cursor.action) < 300)))) {
                              a.select_action(0x131);
                              a.controls.buffered.clear();
                              return;
                            }
                          }
                          if (((((a.controls.current.buttons[2] == 1) ||
                                (a.controls.buffered.buttons[2] != 0)) &&
                               ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                              ((0 < i16(a.combat.spirit) &&
                               (comparedPriority = priority,
                               targetPriority = mode(0x194),
                               comparedPriority <= targetPriority)))) &&
                             (((299 < i16(a.animation.cursor.action) &&
                               ((a.response != 0 && (a.response != 3))))
                              || (i16(a.animation.cursor.action) < 300)))) {
                            a.select_action(0x194);
                            a.controls.buffered.clear();
                          }
                          else {
                            if ((a.controls.current.buttons[3] != 0) || (a.controls.buffered.buttons[3] != 0)
                               ) {
                              if ((((a.animation.frame->flags[0] & 0x20) != 0) &&
                                  ((i8(a.movement.usedAirDashes) < '\x02' &&
                                   ((int)i8(a.motion.facing) * a.controls.current.horizontal == 1)
                                   ))) && (((double(60.0f) <= double(a.motion.y) ||
                                            (double(a.motion.velocityY) <= double(0.0f))) &&
                                           ((((comparedPriority = priority,
                                              targetPriority = mode(0xca),
                                              comparedPriority <= targetPriority ||
                                              (priority == 99))
                                             || (priority == 0x31)) && ((((299 < i16(a.animation.cursor.action) &&
                                                         (a.response != 0)) &&
                                                        (a.response != 3)) ||
                                                       (i16(a.animation.cursor.action) < 300)))))))) {
                                a.reset_command_history();
                                a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
                                a.select_action(0xca);
                                return;
                              }
                              if (((((a.animation.frame->flags[0] & 0x20) != 0) &&
                                   (i8(a.movement.usedAirDashes) < '\x02')) &&
                                  (((int)i8(a.motion.facing) * a.controls.current.horizontal == -1
                                   && (((comparedPriority = priority,
                                        targetPriority = mode(0xcb),
                                        comparedPriority <= targetPriority ||
                                        (priority == 99)) ||
                                       (priority == 0x31)))))) &&
                                 ((((299 < i16(a.animation.cursor.action) &&
                                    (a.response != 0)) &&
                                   (a.response != 3)) ||
                                  (i16(a.animation.cursor.action) < 300)))) {
                                a.reset_command_history();
                                a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
                                a.select_action(0xcb);
                                return;
                              }
                            }
                            if ((a.controls.current.buttons[3] == 1) || (a.controls.buffered.buttons[3] == 1)
                               ) {
                              if ((((((a.animation.frame->flags[0] & 0x20) == 0) ||
                                    (('\x01' < i8(a.movement.usedAirDashes) ||
                                     ((int)i8(a.motion.facing) * a.controls.current.horizontal < 1
                                     )))) || ((double(a.motion.y) < double(60.0f) &&
                                              (double(0.0f) < double(a.motion.velocityY))))) ||
                                  (((comparedPriority = priority,
                                    targetPriority = mode(0xca),
                                    targetPriority < comparedPriority &&
                                    (priority != 99)) &&
                                   (priority != 0x31)))) ||
                                 ((((i16(a.animation.cursor.action) < 300 ||
                                    (a.response == 0)) ||
                                   (a.response == 3)) &&
                                  (299 < i16(a.animation.cursor.action))))) {
                                if (((((a.animation.frame->flags[0] & 0x20) != 0) &&
                                     (i8(a.movement.usedAirDashes) < '\x02')) &&
                                    (((int)i8(a.motion.facing) * a.controls.current.horizontal < 0
                                     && (((comparedPriority = priority,
                                          targetPriority = mode(0xcb),
                                          comparedPriority <= targetPriority ||
                                          (priority == 99)) ||
                                         (priority == 0x31))))))
                                   && ((((299 < i16(a.animation.cursor.action) &&
                                         (a.response != 0)) &&
                                        (a.response != 3)) ||
                                       (i16(a.animation.cursor.action) < 300)))) {
                                  a.reset_command_history();
                                  a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
                                  a.select_action(0xcb);
                                }
                              }
                              else {
                                a.reset_command_history();
                                a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
                                a.select_action(0xca);
                              }
                            }
                          }
                        }
                      }
                    }
                    else {
                      a.reset_command_history();
                      a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
                      a.select_action(0xcb);
                    }
                  }
                  else {
                    a.reset_command_history();
                    a.movement.usedAirDashes = i8(a.movement.usedAirDashes) + '\x01';
                    a.select_action(0xca);
                  }
                }
                else {
                  a.select_action(0x254);
                }
              }
              else {
                a.reset_command_history();
                a.select_action(0x2c0);
              }
            }
            else {
              a.reset_command_history();
              a.select_action(0x2be);
            }
          }
          else {
            a.reset_command_history();
            a.select_action(700);
          }
        }
        else {
          a.reset_command_history();
          a.select_action(0x2c1);
        }
      }
      else {
        a.reset_command_history();
        a.select_action(0x2bf);
      }
    }
    else {
      a.reset_command_history();
      a.select_action(0x2bd);
    }
  }
  else {
    a.reset_command_history();
    a.select_action(0x31b);
  }
  return;

}
}
