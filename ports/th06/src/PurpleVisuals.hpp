// Generated from purple TH06 (MIT). Native drawing order and formulas.
#pragma once
#include "BulletManager.hpp"
#include "EnemyManager.hpp"
#include "Player.hpp"
#include "Supervisor.hpp"
namespace PracticeRuntime {
    inline void RenderPurpleHitbox(ImDrawList* p, bool enabled)
    {
        // show bullet hitbox
        if (enabled) {
            const bool is_practice = g_GameManager.isInPracticeMode;
            const int gameState = g_Supervisor.curState;
            const bool is_rep = g_GameManager.isInReplay;
            if (((is_practice) || (is_rep) || (PracticeRuntime::AdvancedActive())) && gameState == SUPERVISOR_STATE_GAMEMANAGER) {
                p->PushClipRect({ 32.0f, 16.0f }, { 416.0f, 464.0f });
                ImVec2 stage_pos = { 32.0f, 16.0f };
                ImVec2 plpos1{g_Player.hitboxTopLeft.x,g_Player.hitboxTopLeft.y};
                ImVec2 plpos2{g_Player.hitboxBottomRight.x,g_Player.hitboxBottomRight.y};
                float plhit = plpos2.x - plpos1.x;

                // bullet hitbox
                for (int i = 0; i < 640; i++) {
                    const auto& bullet = g_BulletManager.bullets[i];
                    if ((bullet.state != 1) || (bullet.state == 0)) {
                        continue;
                    }
                    ImVec2 pos{bullet.pos.x,bullet.pos.y};
                    ImVec2 hit{bullet.sprites.grazeSize.x,bullet.sprites.grazeSize.y};

                    ImVec2 p1 = { pos.x - hit.x * 0.5f - plhit * 0.5f + stage_pos.x, pos.y - hit.y * 0.5f - plhit * 0.5f + stage_pos.y };
                    ImVec2 p2 = { pos.x + hit.x * 0.5f + plhit * 0.5f + stage_pos.x, pos.y + hit.y * 0.5f + plhit * 0.5f + stage_pos.y };
                    p->AddRectFilled(p1, p2, 0x88002288);
                    p->AddRect(p1, p2, 0xFFFFFFFF, 0.0f);
                }

                // laser hitbox
                for (int i = 0; i < 64; i++) {
                    const auto& laser = g_BulletManager.lasers[i];
                    const bool is_used = laser.inUse;
                    if (is_used) {
                        ImVec2 pos{laser.pos.x,laser.pos.y};
                        float angle = laser.angle;
                        float quat_width = laser.width * 0.5f * 0.5f;
                        float half_width_pl = plhit * 0.5f;
                        float start_ofs = laser.startOffset;
                        float end_ofs = laser.endOffset;
                        int state = laser.state;

                        int start_time_graze = laser.hitboxStartTime;
                        int end_time_graze = laser.hitboxEndDelay;
                        int time_cur_state = laser.timer.current;
                        float sub_frame = laser.timer.subFrame;
                        if (state == 0) {
                            int state_change_time_hit = laser.startTime;
                            float l2 = 0.0f;
                            if (time_cur_state <= state_change_time_hit - std::max(30, state_change_time_hit)) {
                                l2 = 1.2f * 0.5f;
                            } else {
                                l2 = quat_width * ((float)time_cur_state + sub_frame) / (float)state_change_time_hit;
                            }
                            float mid = (start_ofs + end_ofs) * 0.5f;
                            start_ofs = mid - l2;
                            end_ofs = mid + l2;
                        }
                        if (state == 2) {
                            int state_change_time_disappear = laser.despawnDuration;
                            float l2 = 0.0f;
                            if (state_change_time_disappear > 0) {
                                l2 = quat_width - quat_width * ((float)time_cur_state + sub_frame) / (float)state_change_time_disappear;
                            }
                            float mid = (start_ofs + end_ofs) * 0.5f;
                            start_ofs = mid - l2;
                            end_ofs = mid + l2;
                        }
                        if (state == 1 || (state == 0 && time_cur_state >= start_time_graze) || (state == 2 && time_cur_state < end_time_graze)) {
                            float c = cosf(angle);
                            float s = sinf(angle);
                            ImVec2 hitpos[4] = {
                                { start_ofs - half_width_pl, -quat_width - half_width_pl },
                                { end_ofs + half_width_pl, -quat_width - half_width_pl },
                                { end_ofs + half_width_pl, quat_width + half_width_pl },
                                { start_ofs - half_width_pl, quat_width + half_width_pl }
                            };
                            auto RotPos = [](ImVec2 p, float c, float s) -> ImVec2 {
                                return { p.x * c - p.y * s, p.x * s + p.y * c };
                            };
                            for (int j = 0; j < 4; j++) {
                                hitpos[j] = RotPos(hitpos[j], c, s);
                                hitpos[j].x += pos.x + stage_pos.x;
                                hitpos[j].y += pos.y + stage_pos.y;
                            }
                            p->AddQuad(hitpos[0], hitpos[1], hitpos[2], hitpos[3], 0xFFFF0000);
                            p->AddQuadFilled(hitpos[0], hitpos[1], hitpos[2], hitpos[3], 0x88882200);
                        }
                    }
                }

                // enemy hitbox
                for (int i = 0; i < 0x100; i++) {
                    const auto& enemy = g_EnemyManager.enemies[i];
                    if (enemy.flags.isSlotOccupied) {
                        if (enemy.flags.hasBeenInBounds && enemy.flags.isCollidable && enemy.flags.isInteractable) {
                            ImVec2 pos{enemy.position.x,enemy.position.y};
                            ImVec2 hit{enemy.hitboxDimensions.x,enemy.hitboxDimensions.y};
                            hit.x *= 0.666666f;
                            hit.y *= 0.666666f;
                            ImVec2 p1 = { pos.x - hit.x * 0.5f - plhit * 0.5f + stage_pos.x, pos.y - hit.y * 0.5f - plhit * 0.5f + stage_pos.y };
                            ImVec2 p2 = { pos.x + hit.x * 0.5f + plhit * 0.5f + stage_pos.x, pos.y + hit.y * 0.5f + plhit * 0.5f + stage_pos.y };
                            p->AddRectFilled(p1, p2, 0xAAFF5500);
                            p->AddRect(p1, p2, 0xFFFFFFFF);
                        }
                    }
                }
                 p->PopClipRect();
            }
        }
    }
}
