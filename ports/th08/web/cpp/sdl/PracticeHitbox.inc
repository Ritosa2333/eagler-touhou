// Generated from purple shared tools (MIT), following TH15.
static_assert(offsetof(BulletState,position)==0xd44 && offsetof(BulletState,sprites)+offsetof(BulletTemplate,hitbox)==0xd34);
static_assert(offsetof(BulletState,since_fired)+offsetof(Timer,current)==0xd88);
static_assert(offsetof(LaserState,position)==0x548 && offsetof(LaserState,in_use)==0x584 && offsetof(LaserState,timer)+offsetof(Timer,current)==0x590 && offsetof(LaserState,state)==0x598);
    void RenderBtHitbox(ImDrawList* p, BrowserRuntime& runtime)
    {
        // show bullet hitbox
        if (runtime.app.session.practice.show_bullet_hitbox) {
            u32 mode = runtime.app.game.globals.game_flags;
            if (((mode & 0x1) || (mode & 0x8)) && runtime.app.in_game()) {
                p->PushClipRect({ 32.0f, 16.0f }, { 416.0f, 464.0f });

                const auto& bounds = runtime.app.game.player_state.motion.movement.bounds;
                ImVec2 plpos1{bounds[0].x,bounds[0].y};
                ImVec2 plpos2{bounds[1].x,bounds[1].y};
                float plhit = plpos2.x - plpos1.x;
                ImVec2 stage_pos = { 32.0f, 16.0f };

                // bullet
                for (int i = 0; i < 0x600; i++) {
                    const auto& bullet = runtime.app.game.projectile_pool.bullets[i];
                    if (u16(bullet.since_fired.current) == 0)
                        continue;
                    ImVec2 pos{bullet.position.x,bullet.position.y};
                    ImVec2 hit{bullet.sprites.hitbox.x,bullet.sprites.hitbox.y};

                    ImVec2 p1 = { pos.x - hit.x * 0.5f - plhit * 0.5f + stage_pos.x, pos.y - hit.y * 0.5f - plhit * 0.5f + stage_pos.y };
                    ImVec2 p2 = { pos.x + hit.x * 0.5f + plhit * 0.5f + stage_pos.x, pos.y + hit.y * 0.5f + plhit * 0.5f + stage_pos.y };
                    p->AddRectFilled(p1, p2, 0x88002288);
                    p->AddRect(p1, p2, 0xFFFFFFFF, 0.0f);
                }
                // laser hitbox
                for (int i = 0; i < 256; i++) {
                    const auto& laser = runtime.app.game.projectile_pool.lasers[i];
                    int is_used = laser.in_use;
                    if (is_used) {
                        ImVec2 pos{laser.position.x,laser.position.y};
                        float angle = laser.angle;
                        float quat_width = laser.width * 0.5f * 0.5f;
                        float half_width_pl = plhit * 0.5f;
                        float start_ofs = laser.start_offset;
                        float end_ofs = laser.end_offset;
                        if (start_ofs < 0.0f) {
                            float mid = (start_ofs + end_ofs) * 0.5f;
                            float l2 = (start_ofs - end_ofs) * 0.7f * 0.5f;
                            start_ofs = mid - l2;
                            end_ofs = mid + l2;
                        }
                        int state = laser.state;

                        int start_time_graze = laser.hitbox_start;
                        int end_time_graze = laser.hitbox_stop;
                        int time_cur_state = laser.timer.current;
                        float sub_frame = laser.timer.fraction;
                        if (state == 0) {
                            int state_change_time_hit = laser.start;
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
                            int state_change_time_disappear = laser.stop;
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
                            p->AddQuad(hitpos[0], hitpos[1], hitpos[2], hitpos[3], 0xFFFFFF00);
                            p->AddQuadFilled(hitpos[0], hitpos[1], hitpos[2], hitpos[3], 0x88002288);
                        }
                    }
                }
                p->PopClipRect();
            }
        }
    }
