#include "SakuyaState.hpp"
namespace th075 {
void select_sakuya_commands(FighterActor& a,const FighterActor& other,const SakuyaState& s){
    if(!a.animation.pattern||!a.animation.frame)return;
    const auto& sequences=a.animation.pattern->sequences;
    const i16 action=a.animation.cursor.action;
    if(action<0||u32(action)>=sequences.size())return;
    const i16 priority=i16(sequences[action].next);
    const auto mode=[&](i16 id){return id>=0&&u32(id)<sequences.size()&&sequences[id].present?i16(sequences[id].mode):i16(-32768);};

  i16 comparedPriority = 0;
  i16 targetPriority = 0;
  
  if ((((i8(a.command) == -0x23) && (double(a.motion.y) == double(0.0f))) &&
      ((a.animation.frame->flags[0] & 0x20) != 0)) &&
     (comparedPriority = priority, targetPriority = mode(0x2f0),
     comparedPriority <= targetPriority)) {
    a.reset_command_history();
    a.select_action(0x303);
  }
  else if (((i8(a.command) == -0x24) && (double(a.motion.y) == double(0.0f))) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (comparedPriority = priority, targetPriority = mode(0x2ef)
           , comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x302);
  }
  else if (((i8(a.command) == -0x2d) && (double(a.motion.y) == double(0.0f))) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (comparedPriority = priority, targetPriority = mode(0x2fb)
           , comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2fb);
  }
  else if ((((i8(a.command) == -0x2e) && (double(a.motion.y) == double(0.0f))) &&
           ((a.animation.frame->flags[0] & 0x20) != 0)) &&
          (comparedPriority = priority, targetPriority = mode(0x2fa),
          comparedPriority <= targetPriority)) {
    a.reset_command_history();
    a.select_action(0x2fa);
  }
  else if (((i8(a.command) == -0x37) && (double(a.motion.y) == double(0.0f))) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (comparedPriority = priority, targetPriority = mode(0x2e6)
           , comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2e6);
  }
  else if (((i8(a.command) == -0x38) && (double(a.motion.y) == double(0.0f))) &&
          (((a.animation.frame->flags[0] & 0x20) != 0 &&
           (comparedPriority = priority, targetPriority = mode(0x2e5)
           , comparedPriority <= targetPriority)))) {
    a.reset_command_history();
    a.select_action(0x2e5);
  }
  else if (((((i8(a.command) != 'g') || (double(a.motion.y) != double(0.0f))) ||
            (double(a.combat.spellExtent) < double(100.0f))) ||
           (((i8(a.combat.spellEnded) != '\0' || (0 < i16(a.combat.spellTimer))) ||
            (((a.animation.frame->flags[0] & 0x20) == 0 ||
             (comparedPriority = priority,
             targetPriority = mode(0x31b), targetPriority < comparedPriority)))))) ||
          ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
            (a.response == 3)) && (299 < i16(a.animation.cursor.action))))) {
    if ((((((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r')) ||
            (double(a.motion.y) != double(0.0f))) ||
           ((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f))))) ||
          (i16(a.spellChoices[1]) != 0)) ||
         ((i8(a.combat.spellPhase) != '\x01' || (i16(s.knivesActive) != 0)))) ||
        (((a.animation.frame->flags[0] & 0x20) == 0 ||
         (comparedPriority = priority, targetPriority = mode(0x2bd),
         targetPriority < comparedPriority)))) ||
       ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
         (a.response == 3)) && (299 < i16(a.animation.cursor.action))))) {
      if ((((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\x17')) ||
            ((double(a.motion.y) != double(0.0f) ||
             ((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f))))))) ||
           (i16(a.spellChoices[1]) != 1)) ||
          ((((i8(a.combat.spellPhase) != '\x01' || (0 < i16(s.timeReserve))) ||
            (i16(s.clockReserve) != 0)) ||
           (((a.animation.frame->flags[0] & 0x20) == 0 ||
            (comparedPriority = priority,
            targetPriority = mode(0x2c3), targetPriority < comparedPriority)))))) ||
         (((i16(a.animation.cursor.action) < 300 ||
           ((a.response == 0 || (a.response == 3)))) &&
          (299 < i16(a.animation.cursor.action))))) {
        if ((((((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r')) ||
                (double(a.motion.y) != double(0.0f))) ||
               ((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f))))) ||
              ((i16(a.spellChoices[1]) != 1 ||
               ((i8(a.combat.spellPhase) != '\x01' || (0 < i16(s.timeReserve))))))) ||
             (i16(s.clockReserve) != 0)) ||
            (((a.animation.frame->flags[0] & 0x20) == 0 ||
             (comparedPriority = priority,
             targetPriority = mode(0x2c2), targetPriority < comparedPriority)))) ||
           (((i16(a.animation.cursor.action) < 300 ||
             ((a.response == 0 || (a.response == 3)))) &&
            (299 < i16(a.animation.cursor.action))))) {
          if ((((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r')) ||
                (double(a.motion.y) != double(0.0f))) ||
               (((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f))) ||
                ((i16(a.spellChoices[1]) != 2 ||
                 ((i8(a.combat.spellPhase) != '\x01' ||
                  ((a.animation.frame->flags[0] & 0x20) == 0)))))))) ||
              (comparedPriority = priority,
              targetPriority = mode(0x2bf), targetPriority < comparedPriority)) ||
             ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
               (a.response == 3)) && (299 < i16(a.animation.cursor.action))))) {
            if (((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r')) ||
               ((((double(a.motion.y) != double(0.0f) ||
                  ((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f))))) ||
                 ((i16(a.spellChoices[0]) != 0 ||
                  (((i8(a.combat.spellPhase) != '\x02' ||
                    ((a.animation.frame->flags[0] & 0x20) == 0)) ||
                   (comparedPriority = priority,
                   targetPriority = mode(700), targetPriority < comparedPriority)))))) ||
                ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
                  (a.response == 3)) && (299 < i16(a.animation.cursor.action))))))) {
              if ((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\x17')) ||
                  ((((double(a.motion.y) != double(0.0f) ||
                     ((((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f)))
                       || (i16(a.spellChoices[0]) != 1)) ||
                      ((i8(a.combat.spellPhase) != '\x02' || (0 < i16(s.timeReserve)))))
                     )) || (i16(s.clockReserve) != 0)) ||
                   (((a.animation.frame->flags[0] & 0x20) == 0 ||
                    (comparedPriority = priority,
                    targetPriority = mode(0x2c1), targetPriority < comparedPriority)))))) ||
                 (((i16(a.animation.cursor.action) < 300 ||
                   ((a.response == 0 || (a.response == 3)))) &&
                  (299 < i16(a.animation.cursor.action))))) {
                if (((((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r'))
                       || (double(a.motion.y) != double(0.0f))) ||
                      (((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f)))
                       || ((i16(a.spellChoices[0]) != 1 ||
                           ((i8(a.combat.spellPhase) != '\x02' ||
                            (0 < i16(s.timeReserve))))))))) ||
                     (i16(s.clockReserve) != 0)) ||
                    (((a.animation.frame->flags[0] & 0x20) == 0 ||
                     (comparedPriority = priority,
                     targetPriority = mode(0x2c0), targetPriority < comparedPriority)))) ||
                   (((i16(a.animation.cursor.action) < 300 ||
                     ((a.response == 0 || (a.response == 3)))) &&
                    (299 < i16(a.animation.cursor.action))))) {
                  if (((((i16(a.combat.spirit) < 1) || (i8(a.command) != '\r'))
                       || (double(a.motion.y) != double(0.0f))) ||
                      ((((i16(a.combat.spellTimer) < 1 || (double(a.combat.spellExtent) < double(100.0f)))
                        || ((i16(a.spellChoices[0]) != 2 ||
                            ((i8(a.combat.spellPhase) != '\x02' ||
                             ((a.animation.frame->flags[0] & 0x20) == 0)))))) ||
                       (comparedPriority = priority,
                       targetPriority = mode(0x2be), targetPriority < comparedPriority))))
                     || ((((i16(a.animation.cursor.action) < 300 || (a.response == 0)) ||
                          (a.response == 3)) && (299 < i16(a.animation.cursor.action)))))
                  {
                    if (((('\0' < i8(a.selectedSpell)) && (double(a.motion.y) == double(0.0f))
                         ) && (((i8(a.command) == 'f' &&
                                ((i16(s.timeActive) == 0 &&
                                 ((a.animation.frame->flags[0] & 0x20) != 0)))) &&
                               (comparedPriority = priority,
                               targetPriority = mode(0x252),
                               comparedPriority <= targetPriority)))) &&
                       ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
                         (a.response != 3)) || (i16(a.animation.cursor.action) < 300)))) {
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
                        if ((0x95 < i16(a.animation.cursor.action)) &&
                           (i16(a.animation.cursor.action) < 0x9f)) {
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
                    if ((((i8(a.selectedSpell) < '\x01') ||
                         (double(a.motion.y) != double(0.0f))) ||
                        ((i8(a.command) != 'f' ||
                         (((i16(s.timeActive) != 0 ||
                           ((a.animation.frame->flags[0] & 0x20) == 0)) ||
                          (comparedPriority = priority,
                          targetPriority = mode(0x254), targetPriority < comparedPriority)))
                         ))) || ((299 < i16(a.animation.cursor.action) ||
                                 (a.reset_command_history(), double(a.motion.y) != double(0.0f))))) {
                      if (((i8(a.command) == '*') &&
                          (((double(a.motion.y) == double(0.0f) && (0 < i16(a.combat.spirit)))
                           && (((a.animation.frame->flags[0] & 0x20) != 0 &&
                               (comparedPriority = priority,
                               targetPriority = mode(0x1f8),
                               comparedPriority <= targetPriority)))))) &&
                         ((((299 < i16(a.animation.cursor.action) && (a.response != 0)) &&
                           (a.response != 3)) || (i16(a.animation.cursor.action) < 300))))
                      {
                        a.reset_command_history();
                        a.select_action(0x1fa);
                        if (double(0.0f) < double(a.motion.y)) {
                          a.select_action(0x1fd);
                        }
                      }
                      else if (((((i8(a.command) == ')') &&
                                 (double(a.motion.y) == double(0.0f))) &&
                                (0 < i16(a.combat.spirit))) &&
                               (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                (comparedPriority = priority,
                                targetPriority = mode(0x1f8),
                                comparedPriority <= targetPriority)))) &&
                              (((299 < i16(a.animation.cursor.action) &&
                                ((a.response != 0 && (a.response != 3)))
                                ) || (i16(a.animation.cursor.action) < 300)))) {
                        a.reset_command_history();
                        a.select_action(0x1f8);
                        if (double(0.0f) < double(a.motion.y)) {
                          a.select_action(0x1fc);
                        }
                      }
                      else if ((((i8(a.command) == '(') &&
                                (double(a.motion.y) == double(0.0f))) &&
                               (0 < i16(a.combat.spirit))) &&
                              ((((a.animation.frame->flags[0] & 0x20) != 0 &&
                                (comparedPriority = priority,
                                targetPriority = mode(0x1f8),
                                comparedPriority <= targetPriority)) &&
                               (((299 < i16(a.animation.cursor.action) &&
                                 ((a.response != 0 && (a.response != 3))
                                 )) || (i16(a.animation.cursor.action) < 300)))))) {
                        a.reset_command_history();
                        a.select_action(0x1f9);
                        if (double(0.0f) < double(a.motion.y)) {
                          a.select_action(0x1fb);
                        }
                      }
                      else if (((((i8(a.command) == '\x1f') &&
                                 (i16(s.timeActive) == 0)) &&
                                (0 < i16(a.combat.spirit))) &&
                               ((((a.animation.frame->flags[0] & 0x20) != 0 &&
                                 (i16(s.timeActive) == 0)) &&
                                (comparedPriority = priority,
                                targetPriority = mode(0x1f5),
                                comparedPriority <= targetPriority)))) &&
                              ((((299 < i16(a.animation.cursor.action) &&
                                 (a.response != 0)) && (a.response != 3)
                                ) || (i16(a.animation.cursor.action) < 300)))) {
                        a.reset_command_history();
                        a.select_action(0x1f5);
                        if (double(0.0f) < double(a.motion.y)) {
                          a.select_action(0x1f7);
                        }
                      }
                      else if (((((i16(a.combat.spirit) < 1) ||
                                 (i8(a.command) != '\x1e')) ||
                                (i16(s.timeActive) != 0)) ||
                               (((a.animation.frame->flags[0] & 0x20) == 0 ||
                                (comparedPriority = priority,
                                targetPriority = mode(500),
                                targetPriority < comparedPriority)))) ||
                              (((i16(a.animation.cursor.action) < 300 ||
                                ((a.response == 0 || (a.response == 3)))
                                ) && (299 < i16(a.animation.cursor.action))))) {
                        if (((((i8(a.command) == '\f') &&
                              (i16(s.timeActive) == 0)) &&
                             ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                            ((0 < i16(a.combat.spirit) &&
                             (comparedPriority = priority,
                             targetPriority = mode(0x25c),
                             comparedPriority <= targetPriority)))) &&
                           (((299 < i16(a.animation.cursor.action) &&
                             ((a.response != 0 && (a.response != 3))))
                            || (i16(a.animation.cursor.action) < 300)))) {
                          a.reset_command_history();
                          a.select_action(0x25c);
                          if (double(0.0f) < double(a.motion.y)) {
                            a.select_action(0x25e);
                          }
                        }
                        else if (((((i8(a.command) == '\v') &&
                                   (i16(s.timeActive) == 0)) &&
                                  ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                                 ((0 < i16(a.combat.spirit) &&
                                  (comparedPriority = priority,
                                  targetPriority = mode(0x260),
                                  comparedPriority <= targetPriority)))) &&
                                (((299 < i16(a.animation.cursor.action) &&
                                  ((a.response != 0 && (a.response != 3)
                                   ))) || (i16(a.animation.cursor.action) < 300)))) {
                          a.reset_command_history();
                          a.select_action(0x260);
                          if (double(0.0f) < double(a.motion.y)) {
                            a.select_action(0x262);
                          }
                        }
                        else if (((((i8(a.command) == '\n') &&
                                   (i16(s.timeActive) == 0)) &&
                                  ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                                 ((0 < i16(a.combat.spirit) &&
                                  (comparedPriority = priority,
                                  targetPriority = mode(0x25f),
                                  comparedPriority <= targetPriority)))) &&
                                (((299 < i16(a.animation.cursor.action) &&
                                  ((a.response != 0 && (a.response != 3)
                                   ))) || (i16(a.animation.cursor.action) < 300)))) {
                          a.reset_command_history();
                          a.select_action(0x25f);
                          if (double(0.0f) < double(a.motion.y)) {
                            a.select_action(0x261);
                          }
                        }
                        else if (((((i8(a.command) == '\x15') &&
                                   (double(a.motion.y) == double(0.0f))) &&
                                  (i16(s.timeActive) == 0)) &&
                                 ((((a.animation.frame->flags[0] & 0x20) != 0 &&
                                   (0 < i16(a.combat.spirit))) &&
                                  (comparedPriority = priority,
                                  targetPriority = mode(0x259),
                                  comparedPriority <= targetPriority)))) &&
                                ((((299 < i16(a.animation.cursor.action) &&
                                   (a.response != 0)) &&
                                  (a.response != 3)) ||
                                 (i16(a.animation.cursor.action) < 300)))) {
                          a.reset_command_history();
                          a.select_action(0x259);
                          if (double(0.0f) < double(a.motion.y)) {
                            a.select_action(0x25b);
                          }
                        }
                        else if ((((((i8(a.command) == '\x14') &&
                                    (double(a.motion.y) == double(0.0f))) &&
                                   (i16(s.timeActive) == 0)) &&
                                  (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                   (0 < i16(a.combat.spirit))))) &&
                                 (comparedPriority = priority,
                                 targetPriority = mode(600),
                                 comparedPriority <= targetPriority)) &&
                                ((((299 < i16(a.animation.cursor.action) &&
                                   (a.response != 0)) &&
                                  (a.response != 3)) ||
                                 (i16(a.animation.cursor.action) < 300)))) {
                          a.reset_command_history();
                          a.select_action(600);
                          if (double(0.0f) < double(a.motion.y)) {
                            a.select_action(0x25a);
                          }
                        }
                        else if (((((i8(a.command) == 'e') &&
                                   (i16(s.timeActive) == 0)) &&
                                  ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                                 ((double(a.motion.y) == double(0.0f) &&
                                  (comparedPriority = priority,
                                  targetPriority = mode(0x135),
                                  comparedPriority <= targetPriority)))) &&
                                (((299 < i16(a.animation.cursor.action) &&
                                  ((a.response != 0 && (a.response != 3)
                                   ))) || (i16(a.animation.cursor.action) < 300)))) {
                          a.reset_command_history();
                          a.select_action(0x135);
                        }
                        else if (((((i8(a.command) == 'd') &&
                                   (i16(s.timeActive) == 0)) &&
                                  (double(a.motion.y) == double(0.0f))) &&
                                 (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                  (comparedPriority = priority,
                                  targetPriority = mode(0x136),
                                  comparedPriority <= targetPriority)))) &&
                                (((299 < i16(a.animation.cursor.action) &&
                                  ((a.response != 0 && (a.response != 3)
                                   ))) || (i16(a.animation.cursor.action) < 300)))) {
                          a.reset_command_history();
                          a.select_action(0x136);
                        }
                        else if (((((i8(a.command) == '\x01') &&
                                   (i8(a.motion.facing) == '\x01')) ||
                                  (((i8(a.command) == '\x02' &&
                                    (i8(a.motion.facing) == -1)) ||
                                   ((a.controls.current.buttons[3] != 0 &&
                                    ((int)i8(a.motion.facing) * a.controls.current.horizontal == 1
                                    )))))) ||
                                 ((a.controls.current.buttons[3] == 1 &&
                                  (0 < (int)i8(a.motion.facing) * a.controls.current.horizontal)))
                                 ) && (((((double(a.motion.y) == double(0.0f) &&
                                          (i16(a.animation.cursor.action) != 0xcf)) &&
                                         ((a.animation.frame->flags[0] & 0x20) != 0))
                                        && (comparedPriority = priority,
                                           targetPriority = mode(200),
                                           comparedPriority <= targetPriority)) &&
                                       ((((299 < i16(a.animation.cursor.action) &&
                                          (a.response != 0)) &&
                                         (a.response != 3)) ||
                                        (i16(a.animation.cursor.action) < 300)))))) {
                          a.reset_command_history();
                          a.select_action(200);
                        }
                        else if ((((((i8(a.command) != '\x01') ||
                                    (i8(a.motion.facing) != '\x01')) &&
                                   (((i8(a.command) != '\x02' ||
                                     (i8(a.motion.facing) != -1)) &&
                                    (((a.controls.current.buttons[3] == 0 ||
                                      ((int)i8(a.motion.facing) * a.controls.current.horizontal !=
                                       1)) && ((a.controls.current.buttons[3] != 1 ||
                                               ((int)i8(a.motion.facing) *
                                                a.controls.current.horizontal < 1)))))))) ||
                                  ((double(a.motion.y) <= double(0.0f) ||
                                   ((((double(a.motion.y) < double(60.0f) &&
                                      (double(0.0f) < double(a.motion.velocityY))) ||
                                     ((a.animation.frame->flags[0] & 0x20) == 0)) ||
                                    ('\x01' < i8(a.movement.usedAirDashes))))))) ||
                                 (((comparedPriority = priority,
                                   targetPriority = mode(0xca),
                                   targetPriority < comparedPriority &&
                                   (priority != 99)) &&
                                  (priority != 0x31)))) ||
                                ((((i16(a.animation.cursor.action) < 300 ||
                                   (a.response == 0)) ||
                                  (a.response == 3)) &&
                                 (299 < i16(a.animation.cursor.action))))) {
                          if ((((((i8(a.command) == '\x02') &&
                                 (i8(a.motion.facing) == '\x01')) ||
                                (((i8(a.command) == '\x01' &&
                                  (i8(a.motion.facing) == -1)) ||
                                 ((a.controls.current.buttons[3] != 0 &&
                                  ((int)i8(a.motion.facing) * a.controls.current.horizontal == -1)
                                  ))))) ||
                               ((a.controls.current.buttons[3] == 1 &&
                                ((int)i8(a.motion.facing) * a.controls.current.horizontal < 0))))
                              && (((double(a.motion.y) == double(0.0f) &&
                                   ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                                  (comparedPriority = priority,
                                  targetPriority = mode(0xc9),
                                  comparedPriority <= targetPriority)))) &&
                             ((((299 < i16(a.animation.cursor.action) && (a.response != 0)
                                ) && (a.response != 3)) ||
                              (i16(a.animation.cursor.action) < 300)))) {
                            a.reset_command_history();
                            a.select_action(0xc9);
                          }
                          else if ((((i8(a.command) != '\x02') ||
                                    (i8(a.motion.facing) != '\x01')) &&
                                   ((((i8(a.command) != '\x01' ||
                                      (i8(a.motion.facing) != -1)) &&
                                     ((a.controls.current.buttons[3] == 0 ||
                                      ((int)i8(a.motion.facing) * a.controls.current.horizontal !=
                                       -1)))) &&
                                    ((a.controls.current.buttons[3] != 1 ||
                                     (-1 < (int)i8(a.motion.facing) *
                                           a.controls.current.horizontal)))))) ||
                                  (((((double(a.motion.y) <= double(0.0f) ||
                                      ((a.animation.frame->flags[0] & 0x20) == 0)) ||
                                     ('\x01' < i8(a.movement.usedAirDashes))) ||
                                    (((comparedPriority = priority,
                                      targetPriority = mode(0xcb),
                                      targetPriority < comparedPriority &&
                                      (priority != 99)) &&
                                     (priority != 0x31)))) ||
                                   ((((i16(a.animation.cursor.action) < 300 ||
                                      (a.response == 0)) ||
                                     (a.response == 3)) &&
                                    (299 < i16(a.animation.cursor.action))))))) {
                            if ((((i8(a.command) == '\x03') ||
                                 (((a.controls.current.buttons[3] != 0 &&
                                   (a.controls.current.vertical == -1)) &&
                                  (a.controls.current.horizontal == 0)))) ||
                                (((a.controls.current.buttons[3] == 1 &&
                                  (a.controls.current.vertical < -1)) &&
                                 (a.controls.current.horizontal == 0)))) &&
                               (((((a.animation.frame->flags[0] & 0x20) != 0 &&
                                  (double(a.motion.y) == double(0.0f))) &&
                                 ((comparedPriority = priority,
                                  targetPriority = mode(0xcc),
                                  comparedPriority <= targetPriority ||
                                  ((priority == 99 ||
                                   (priority == 0x31)))))) &&
                                (((299 < i16(a.animation.cursor.action) &&
                                  ((a.response != 0 && (a.response != 3)
                                   ))) || (i16(a.animation.cursor.action) < 300)))))) {
                              a.reset_command_history();
                              a.select_action(0xcc);
                            }
                            else if ((((((i8(a.command) == '\x04') ||
                                        (((a.controls.current.buttons[3] != 0 &&
                                          (a.controls.current.vertical == -1)) &&
                                         ((int)i8(a.motion.facing) * a.controls.current.horizontal
                                          < 0)))) ||
                                       (((a.controls.current.buttons[3] == 1 &&
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
                                      ((a.response != 0 &&
                                       (a.response != 3)))) ||
                                     (i16(a.animation.cursor.action) < 300)))) {
                              a.reset_command_history();
                              a.select_action(0xce);
                            }
                            else if (((((i8(a.command) == '\x05') ||
                                       (((a.controls.current.buttons[3] != 0 &&
                                         (a.controls.current.vertical == -1)) &&
                                        (0 < (int)i8(a.motion.facing) *
                                             a.controls.current.horizontal)))) ||
                                      (((a.controls.current.buttons[3] == 1 &&
                                        (a.controls.current.vertical < -1)) &&
                                       (0 < (int)i8(a.motion.facing) *
                                            a.controls.current.horizontal)))) &&
                                     ((((a.animation.frame->flags[0] & 0x20) != 0 &&
                                       (double(a.motion.y) == double(0.0f))) &&
                                      ((comparedPriority = priority,
                                       targetPriority = mode(0xcd),
                                       comparedPriority <= targetPriority ||
                                       ((priority == 99 ||
                                        (priority == 0x31))))))))
                                    && (((299 < i16(a.animation.cursor.action) &&
                                         ((a.response != 0 &&
                                          (a.response != 3)))) ||
                                        (i16(a.animation.cursor.action) < 300)))) {
                              a.reset_command_history();
                              a.select_action(0xcd);
                            }
                            else {
                              if ((double(a.motion.y) == double(0.0f)) &&
                                 ((0 < a.controls.current.vertical || (0 < a.controls.buffered.vertical))
                                 )) {
                                if (((a.controls.current.buttons[0] == 1) ||
                                    (a.controls.buffered.buttons[0] != 0)) &&
                                   ((((0 < (int)i8(a.motion.facing) *
                                           a.controls.current.horizontal ||
                                      (0 < (int)i8(a.motion.facing) *
                                           a.controls.buffered.horizontal)) &&
                                     (i16(a.animation.cursor.action) == 200)) &&
                                    ((a.animation.frame->flags[0] & 0x20) != 0)))) {
                                  a.select_action(0x188);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a2);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if (((((a.controls.current.buttons[0] == 1) ||
                                      (a.controls.buffered.buttons[0] != 0)) &&
                                     ((0 < i16(a.combat.spirit) &&
                                      ((a.animation.frame->flags[0] & 0x20) != 0))))
                                    && (((double(320.0f) < double(a.motion.x) -
                                                  double(other.motion.x) ||
                                         (double(a.motion.x) -
                                          double(other.motion.x) < -double(320.0f))) &&
                                        (comparedPriority = priority,
                                        targetPriority = mode(0x198),
                                        comparedPriority <= targetPriority)))) &&
                                   ((((299 < i16(a.animation.cursor.action) &&
                                      (a.response != 0)) &&
                                     (a.response != 3)) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x198);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a2);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((a.controls.current.buttons[0] == 1) ||
                                     (a.controls.buffered.buttons[0] != 0)) &&
                                    (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                     (comparedPriority = priority,
                                     targetPriority = mode(0x12e),
                                     comparedPriority <= targetPriority)))) &&
                                   ((((299 < i16(a.animation.cursor.action) &&
                                      (a.response != 0)) &&
                                     (a.response != 3)) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x12e);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a2);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((a.controls.current.buttons[1] == 1) ||
                                     (a.controls.buffered.buttons[0] != 0)) &&
                                    ((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal
                                     || (0 < (int)i8(a.motion.facing) *
                                             a.controls.buffered.horizontal)))) &&
                                   ((i16(a.animation.cursor.action) == 200 &&
                                    ((a.animation.frame->flags[0] & 0x20) != 0)))) {
                                  a.select_action(0x189);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a2);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((((a.controls.current.buttons[1] == 1) ||
                                       (a.controls.buffered.buttons[1] != 0)) &&
                                      (0 < i16(a.combat.spirit))) &&
                                     (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                      ((double(320.0f) < double(a.motion.x) -
                                                double(other.motion.x) ||
                                       (double(a.motion.x) -
                                        double(other.motion.x) < -double(320.0f)))))))
                                    && (comparedPriority = priority,
                                       targetPriority = mode(0x19d),
                                       comparedPriority <= targetPriority)) &&
                                   ((((299 < i16(a.animation.cursor.action) &&
                                      (a.response != 0)) &&
                                     (a.response != 3)) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x19d);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a2);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if (((((a.controls.current.buttons[1] == 1) ||
                                      (a.controls.buffered.buttons[1] != 0)) &&
                                     ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                                    (comparedPriority = priority,
                                    targetPriority = mode(0x12f),
                                    comparedPriority <= targetPriority)) &&
                                   ((((299 < i16(a.animation.cursor.action) &&
                                      (a.response != 0)) &&
                                     (a.response != 3)) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x12f);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a2);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((a.controls.current.buttons[2] == 1) ||
                                     (a.controls.buffered.buttons[2] != 0)) &&
                                    (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                     ((0 < i16(a.combat.spirit) &&
                                      (comparedPriority = priority,
                                      targetPriority = mode(0x192),
                                      comparedPriority <= targetPriority)))))) &&
                                   (((299 < i16(a.animation.cursor.action) &&
                                     ((a.response != 0 &&
                                      (a.response != 3)))) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x192);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a2);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                              }
                              if ((double(a.motion.y) == double(0.0f)) &&
                                 (a.controls.current.vertical == 0)) {
                                if ((((a.controls.current.buttons[0] == 1) ||
                                     (a.controls.buffered.buttons[0] != 0)) &&
                                    (a.controls.current.vertical == 0)) &&
                                   ((((0 < (int)i8(a.motion.facing) *
                                           a.controls.current.horizontal ||
                                      (0 < (int)i8(a.motion.facing) *
                                           a.controls.buffered.horizontal)) &&
                                     (i16(a.animation.cursor.action) == 200)) &&
                                    ((a.animation.frame->flags[0] & 0x20) != 0)))) {
                                  a.select_action(0x186);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a1);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if (((((a.controls.current.buttons[0] == 1) ||
                                      (a.controls.buffered.buttons[0] != 0)) &&
                                     ((0 < i16(a.combat.spirit) &&
                                      ((a.animation.frame->flags[0] & 0x20) != 0))))
                                    && ((((double(320.0f) < double(a.motion.x) -
                                                   double(other.motion.x) ||
                                          (double(a.motion.x) -
                                           double(other.motion.x) < -double(320.0f)))
                                         && ((0 < (int)i8(a.motion.facing) *
                                                  a.controls.current.horizontal ||
                                             (0 < (int)i8(a.motion.facing) *
                                                  a.controls.buffered.horizontal)))) &&
                                        (comparedPriority = priority,
                                        targetPriority = mode(0x197),
                                        comparedPriority <= targetPriority)))) &&
                                   ((((299 < i16(a.animation.cursor.action) &&
                                      (a.response != 0)) &&
                                     (a.response != 3)) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x197);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a1);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((((a.controls.current.buttons[0] == 1) ||
                                       (a.controls.buffered.buttons[0] != 0)) &&
                                      ((0 < i16(a.combat.spirit) &&
                                       ((a.animation.frame->flags[0] & 0x20) != 0))))
                                     && ((double(320.0f) < double(a.motion.x) -
                                                  double(other.motion.x) ||
                                         (double(a.motion.x) -
                                          double(other.motion.x) < -double(320.0f)))))
                                    && (comparedPriority = priority,
                                       targetPriority = mode(0x196),
                                       comparedPriority <= targetPriority)) &&
                                   ((((299 < i16(a.animation.cursor.action) &&
                                      (a.response != 0)) &&
                                     (a.response != 3)) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x196);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a0);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((a.controls.current.buttons[0] == 1) ||
                                     (a.controls.buffered.buttons[0] != 0)) &&
                                    ((a.controls.current.vertical == 0 &&
                                     ((0 < (int)i8(a.motion.facing) *
                                           a.controls.current.horizontal ||
                                      (0 < (int)i8(a.motion.facing) *
                                           a.controls.buffered.horizontal)))))) &&
                                   ((((a.animation.frame->flags[0] & 0x20) != 0 &&
                                     (comparedPriority = priority,
                                     targetPriority = mode(0x133),
                                     comparedPriority <= targetPriority)) &&
                                    ((((299 < i16(a.animation.cursor.action) &&
                                       (a.response != 0)) &&
                                      (a.response != 3)) ||
                                     (i16(a.animation.cursor.action) < 300)))))) {
                                  a.select_action(0x133);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a1);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if (((a.controls.current.buttons[0] == 1) ||
                                    (a.controls.buffered.buttons[0] != 0)) &&
                                   ((((a.animation.frame->flags[0] & 0x20) != 0 &&
                                     ((comparedPriority = priority,
                                      targetPriority = mode(300),
                                      comparedPriority <= targetPriority ||
                                      (i16(a.animation.cursor.action) == 300)))) &&
                                    (((299 < i16(a.animation.cursor.action) &&
                                      ((a.response != 0 &&
                                       (a.response != 3)))) ||
                                     (i16(a.animation.cursor.action) < 300)))))) {
                                  a.select_action(300);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a0);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((a.controls.current.buttons[1] == 1) ||
                                     (a.controls.buffered.buttons[0] != 0)) &&
                                    (a.controls.current.vertical == 0)) &&
                                   (((0 < (int)i8(a.motion.facing) * a.controls.current.horizontal
                                     || (0 < (int)i8(a.motion.facing) *
                                             a.controls.buffered.horizontal)) &&
                                    ((i16(a.animation.cursor.action) == 200 &&
                                     ((a.animation.frame->flags[0] & 0x20) != 0))))))
                                {
                                  a.select_action(0x187);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a1);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((a.controls.current.buttons[1] == 1) ||
                                     (a.controls.buffered.buttons[1] != 0)) &&
                                    ((a.controls.current.vertical == 0 &&
                                     (((((0 < (int)i8(a.motion.facing) *
                                              a.controls.current.horizontal ||
                                         (0 < (int)i8(a.motion.facing) *
                                              a.controls.buffered.horizontal)) &&
                                        (0 < i16(a.combat.spirit))) &&
                                       ((a.animation.frame->flags[0] & 0x20) != 0))
                                      && ((double(320.0f) < double(a.motion.x) -
                                                   double(other.motion.x) ||
                                          (double(a.motion.x) -
                                           double(other.motion.x) < -double(320.0f)))))
                                     )))) && ((comparedPriority = priority,
                                              targetPriority = mode(0x19c),
                                              comparedPriority <= targetPriority &&
                                              ((((299 < i16(a.animation.cursor.action) &&
                                                 (a.response != 0)) &&
                                                (a.response != 3)) ||
                                               (i16(a.animation.cursor.action) < 300)))))) {
                                  a.select_action(0x19c);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a1);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((((a.controls.current.buttons[1] == 1) ||
                                       (a.controls.buffered.buttons[1] != 0)) &&
                                      (0 < i16(a.combat.spirit))) &&
                                     (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                      ((double(320.0f) < double(a.motion.x) -
                                                double(other.motion.x) ||
                                       (double(a.motion.x) -
                                        double(other.motion.x) < -double(320.0f)))))))
                                    && (comparedPriority = priority,
                                       targetPriority = mode(0x19b),
                                       comparedPriority <= targetPriority)) &&
                                   ((((299 < i16(a.animation.cursor.action) &&
                                      (a.response != 0)) &&
                                     (a.response != 3)) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x19b);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a0);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if (((((a.controls.current.buttons[1] == 1) ||
                                      (a.controls.buffered.buttons[1] != 0)) &&
                                     (a.controls.current.vertical == 0)) &&
                                    (((0 < (int)i8(a.motion.facing) *
                                           a.controls.current.horizontal ||
                                      (0 < (int)i8(a.motion.facing) *
                                           a.controls.buffered.horizontal)) &&
                                     (((a.animation.frame->flags[0] & 0x20) != 0 &&
                                      (comparedPriority = priority,
                                      targetPriority = mode(0x134),
                                      comparedPriority <= targetPriority)))))) &&
                                   ((((299 < i16(a.animation.cursor.action) &&
                                      (a.response != 0)) &&
                                     (a.response != 3)) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x134);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a1);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if (((((a.controls.current.buttons[1] == 1) ||
                                      (a.controls.buffered.buttons[1] != 0)) &&
                                     ((a.animation.frame->flags[0] & 0x20) != 0)) &&
                                    (comparedPriority = priority,
                                    targetPriority = mode(0x12d),
                                    comparedPriority <= targetPriority)) &&
                                   ((((299 < i16(a.animation.cursor.action) &&
                                      (a.response != 0)) &&
                                     (a.response != 3)) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x12d);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a0);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                                if ((((a.controls.current.buttons[2] == 1) ||
                                     (a.controls.buffered.buttons[2] != 0)) &&
                                    (((a.controls.current.vertical == 0 &&
                                      (((0 < (int)i8(a.motion.facing) *
                                             a.controls.current.horizontal ||
                                        (0 < (int)i8(a.motion.facing) *
                                             a.controls.buffered.horizontal)) &&
                                       ((a.animation.frame->flags[0] & 0x20) != 0))))
                                     && ((0 < i16(a.combat.spirit) &&
                                         (comparedPriority = priority,
                                         targetPriority = mode(0x191),
                                         comparedPriority <= targetPriority)))))) &&
                                   (((299 < i16(a.animation.cursor.action) &&
                                     ((a.response != 0 &&
                                      (a.response != 3)))) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(0x191);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a1);
                                  }
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
                                     ((a.response != 0 &&
                                      (a.response != 3)))) ||
                                    (i16(a.animation.cursor.action) < 300)))) {
                                  a.select_action(400);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a0);
                                  }
                                  a.controls.buffered.clear();
                                  return;
                                }
                              }
                              if (double(0.0f) < double(a.motion.y)) {
                                if (((((((a.controls.current.buttons[0] != 1) &&
                                        (a.controls.buffered.buttons[0] == 0)) ||
                                       (i16(a.combat.spirit) < 1)) ||
                                      ((a.animation.frame->flags[0] & 0x20) == 0)) ||
                                     (((double(a.motion.x) -
                                        double(other.motion.x) <= double(400.0f) &&
                                       (-double(400.0f) <= double(a.motion.x) -
                                                  double(other.motion.x))) ||
                                      ((a.controls.current.vertical < 1 &&
                                       (a.controls.buffered.vertical < 1)))))) ||
                                    (comparedPriority = priority,
                                    targetPriority = mode(0x19a),
                                    targetPriority < comparedPriority)) ||
                                   ((((i16(a.animation.cursor.action) < 300 ||
                                      (a.response == 0)) ||
                                     (a.response == 3)) &&
                                    (299 < i16(a.animation.cursor.action))))) {
                                  if (((((a.controls.current.buttons[0] != 1) &&
                                        (a.controls.buffered.buttons[0] == 0)) ||
                                       (i16(a.combat.spirit) < 1)) ||
                                      (((a.animation.frame->flags[0] & 0x20) == 0 ||
                                       ((double(a.motion.x) -
                                         double(other.motion.x) <= double(400.0f) &&
                                        (-double(400.0f) <= double(a.motion.x) -
                                                   double(other.motion.x))))))
                                      ) || ((comparedPriority = priority,
                                            targetPriority = mode(0x199),
                                            targetPriority < comparedPriority ||
                                            ((((i16(a.animation.cursor.action) < 300 ||
                                               (a.response == 0)) ||
                                              (a.response == 3)) &&
                                             (299 < i16(a.animation.cursor.action))))))) {
                                    if (((((a.controls.current.buttons[0] == 1) ||
                                          (a.controls.buffered.buttons[0] != 0)) &&
                                         ((a.animation.frame->flags[0] & 0x20) != 0))
                                        && (comparedPriority = priority,
                                           targetPriority = mode(0x130),
                                           comparedPriority <= targetPriority)) &&
                                       ((((299 < i16(a.animation.cursor.action) &&
                                          (a.response != 0)) &&
                                         (a.response != 3)) ||
                                        (i16(a.animation.cursor.action) < 300)))) {
                                      a.select_action(0x130);
                                      if (i16(s.timeActive) != 0) {
                                        a.select_action(0x1a3);
                                      }
                                      if ((i16(s.timeActive) != 0) &&
                                         ((0 < a.controls.current.vertical ||
                                          (0 < a.controls.buffered.vertical)))) {
                                        a.select_action(0x1a4);
                                      }
                                      a.controls.buffered.clear();
                                    }
                                    else if ((((a.controls.current.buttons[1] != 1) &&
                                              (a.controls.buffered.buttons[1] == 0)) ||
                                             (((i16(a.combat.spirit) < 1 ||
                                               ((a.animation.frame->flags[0] & 0x20)
                                                == 0)) ||
                                              ((double(a.motion.x) -
                                                double(other.motion.x) <=
                                                double(480.0f) && (-double(480.0f) <= double(a.motion.x) -
                                                                    double(other.motion.x))))))) ||
                                            (((a.controls.current.vertical < 1 &&
                                              (a.controls.buffered.vertical < 1)) ||
                                             ((comparedPriority = priority,
                                              targetPriority = mode(0x19f),
                                              targetPriority < comparedPriority ||
                                              ((((i16(a.animation.cursor.action) < 300 ||
                                                 (a.response == 0)) ||
                                                (a.response == 3)) &&
                                               (299 < i16(a.animation.cursor.action))))))))) {
                                      if ((((((a.controls.current.buttons[1] != 1) &&
                                             (a.controls.buffered.buttons[1] == 0)) ||
                                            (i16(a.combat.spirit) < 1)) ||
                                           (((a.animation.frame->flags[0] & 0x20) ==
                                             0 || ((double(a.motion.x) -
                                                    double(other.motion.x) <=
                                                    double(480.0f) && (-double(480.0f) <= double(a.motion.x) -
                                                                        double(other.motion.x))))))) ||
                                          (comparedPriority = priority,
                                          targetPriority = mode(0x19e),
                                          targetPriority < comparedPriority)) ||
                                         ((((i16(a.animation.cursor.action) < 300 ||
                                            (a.response == 0)) ||
                                           (a.response == 3)) &&
                                          (299 < i16(a.animation.cursor.action))))) {
                                        if ((((a.controls.current.buttons[1] == 1) ||
                                             (a.controls.buffered.buttons[1] != 0)) &&
                                            (((a.animation.frame->flags[0] & 0x20) !=
                                              0 && (comparedPriority = priority,
                                                   targetPriority = mode(0x131),
                                                   comparedPriority <= targetPriority)))) &&
                                           ((((299 < i16(a.animation.cursor.action) &&
                                              (a.response != 0)) &&
                                             (a.response != 3)) ||
                                            (i16(a.animation.cursor.action) < 300)))) {
                                          a.select_action(0x131);
                                          if (i16(s.timeActive) != 0) {
                                            a.select_action(0x1a3);
                                          }
                                          if ((i16(s.timeActive) != 0) &&
                                             ((0 < a.controls.current.vertical ||
                                              (0 < a.controls.buffered.vertical)))) {
                                            a.select_action(0x1a4);
                                          }
                                          a.controls.buffered.clear();
                                        }
                                        else {
                                          if ((a.controls.current.buttons[2] == 1) ||
                                             (a.controls.buffered.buttons[2] != 0)) {
                                            if (((0 < a.controls.current.vertical) &&
                                                ((((a.animation.frame->flags[0] &
                                                   0x20) != 0 && (0 < i16(a.combat.spirit)))
                                                 && (comparedPriority = priority,
                                                    targetPriority = mode(0x195),
                                                    comparedPriority <= targetPriority)))) &&
                                               ((((299 < i16(a.animation.cursor.action) &&
                                                  (a.response != 0)) &&
                                                 (a.response != 3)) ||
                                                (i16(a.animation.cursor.action) < 300)))) {
                                              a.select_action(0x195);
                                              if (i16(s.timeActive) != 0) {
                                                a.select_action(0x1a4);
                                              }
                                              a.controls.buffered.clear();
                                              return;
                                            }
                                            if ((((a.animation.frame->flags[0] & 0x20) != 0) && (0 < i16(a.combat.spirit))) &&
                                               ((comparedPriority = priority
                                                , targetPriority = mode(0x194),
                                                comparedPriority <= targetPriority &&
                                                ((((299 < i16(a.animation.cursor.action) &&
                                                   (a.response != 0)) &&
                                                  (a.response != 3)) ||
                                                 (i16(a.animation.cursor.action) < 300)))))) {
                                              a.select_action(0x194);
                                              if (i16(s.timeActive) != 0) {
                                                a.select_action(0x1a3);
                                              }
                                              a.controls.buffered.clear();
                                              return;
                                            }
                                          }
                                          if ((a.controls.current.buttons[3] != 0) ||
                                             (a.controls.buffered.buttons[3] != 0)) {
                                            if ((((((a.animation.frame->flags[0] &
                                                    0x20) != 0) &&
                                                  ((i8(a.movement.usedAirDashes) < '\x02' &&
                                                   ((int)i8(a.motion.facing) *
                                                    a.controls.current.horizontal == 1)))) &&
                                                 ((double(60.0f) <= double(a.motion.y) ||
                                                  (double(a.motion.velocityY) <= double(0.0f))))) &&
                                                (((comparedPriority = priority,
                                                  targetPriority = mode(0xca),
                                                  comparedPriority <= targetPriority ||
                                                  (priority == 99
                                                  )) || (priority
                                                         == 0x31)))) &&
                                               ((((299 < i16(a.animation.cursor.action) &&
                                                  (a.response != 0)) &&
                                                 (a.response != 3)) ||
                                                (i16(a.animation.cursor.action) < 300)))) {
                                              a.reset_command_history();
                                              a.movement.usedAirDashes =
                                                   i8(a.movement.usedAirDashes) + '\x01';
                                              a.select_action(0xca);
                                              return;
                                            }
                                            if (((((a.animation.frame->flags[0] &
                                                   0x20) != 0) &&
                                                 (i8(a.movement.usedAirDashes) < '\x02')) &&
                                                (((int)i8(a.motion.facing) *
                                                  a.controls.current.horizontal == -1 &&
                                                 (((comparedPriority = priority,
                                                   targetPriority = mode(0xcb),
                                                   comparedPriority <= targetPriority ||
                                                   (priority ==
                                                    99)) || (priority == 0x31)))))) &&
                                               ((((299 < i16(a.animation.cursor.action) &&
                                                  (a.response != 0)) &&
                                                 (a.response != 3)) ||
                                                (i16(a.animation.cursor.action) < 300)))) {
                                              a.reset_command_history();
                                              a.movement.usedAirDashes =
                                                   i8(a.movement.usedAirDashes) + '\x01';
                                              a.select_action(0xcb);
                                              return;
                                            }
                                          }
                                          if ((a.controls.current.buttons[3] == 1) ||
                                             (a.controls.buffered.buttons[3] == 1)) {
                                            if (((((a.animation.frame->flags[0] &
                                                   0x20) == 0) ||
                                                 (('\x01' < i8(a.movement.usedAirDashes) ||
                                                  ((int)i8(a.motion.facing) *
                                                   a.controls.current.horizontal < 1)))) ||
                                                ((double(a.motion.y) < double(60.0f) &&
                                                 (double(0.0f) < double(a.motion.velocityY))))) ||
                                               ((((comparedPriority = priority,
                                                  targetPriority = mode(0xca),
                                                  targetPriority < comparedPriority &&
                                                  (priority != 99
                                                  )) && (priority
                                                         != 0x31)) ||
                                                ((((i16(a.animation.cursor.action) < 300 ||
                                                   (a.response == 0)) ||
                                                  (a.response == 3)) &&
                                                 (299 < i16(a.animation.cursor.action))))))) {
                                              if (((((a.animation.frame->flags[0] &
                                                     0x20) != 0) &&
                                                   (i8(a.movement.usedAirDashes) < '\x02')) &&
                                                  (((int)i8(a.motion.facing) *
                                                    a.controls.current.horizontal < 0 &&
                                                   (((comparedPriority = priority,
                                                     targetPriority = mode(0xcb),
                                                     comparedPriority <= targetPriority ||
                                                     (priority ==
                                                      99)) || (priority == 0x31)))))) &&
                                                 ((((299 < i16(a.animation.cursor.action) &&
                                                    (a.response != 0)) &&
                                                   (a.response != 3)) ||
                                                  (i16(a.animation.cursor.action) < 300)))) {
                                                a.reset_command_history();
                                                a.movement.usedAirDashes =
                                                     i8(a.movement.usedAirDashes) + '\x01';
                                                a.select_action(0xcb);
                                              }
                                            }
                                            else {
                                              a.reset_command_history();
                                              a.movement.usedAirDashes =
                                                   i8(a.movement.usedAirDashes) + '\x01';
                                              a.select_action(0xca);
                                            }
                                          }
                                        }
                                      }
                                      else {
                                        a.select_action(0x19e);
                                        if (i16(s.timeActive) != 0) {
                                          a.select_action(0x1a3);
                                        }
                                        a.controls.buffered.clear();
                                      }
                                    }
                                    else {
                                      a.select_action(0x19f);
                                      if (i16(s.timeActive) != 0) {
                                        a.select_action(0x1a4);
                                      }
                                      a.controls.buffered.clear();
                                    }
                                  }
                                  else {
                                    a.select_action(0x199);
                                    if (i16(s.timeActive) != 0) {
                                      a.select_action(0x1a3);
                                    }
                                    a.controls.buffered.clear();
                                  }
                                }
                                else {
                                  a.select_action(0x19a);
                                  if (i16(s.timeActive) != 0) {
                                    a.select_action(0x1a4);
                                  }
                                  a.controls.buffered.clear();
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
                        a.reset_command_history();
                        a.select_action(500);
                        if (double(0.0f) < double(a.motion.y)) {
                          a.select_action(0x1f6);
                        }
                      }
                    }
                    else {
                      a.select_action(0x254);
                    }
                  }
                  else {
                    a.reset_command_history();
                    a.select_action(0x2be);
                  }
                }
                else {
                  a.reset_command_history();
                  a.select_action(0x2c0);
                }
              }
              else {
                a.reset_command_history();
                a.select_action(0x2c1);
              }
            }
            else {
              a.reset_command_history();
              a.select_action(700);
            }
          }
          else {
            a.reset_command_history();
            a.select_action(0x2bf);
          }
        }
        else {
          a.reset_command_history();
          a.select_action(0x2c2);
        }
      }
      else {
        a.reset_command_history();
        a.select_action(0x2c3);
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
