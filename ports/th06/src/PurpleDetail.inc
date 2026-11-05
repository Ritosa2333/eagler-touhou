// Generated native purple TH06 statistics tabs/tables (MIT).
        void ShowDetail()
        {
            ImGui::BeginTabBar("Detail Spell");
            {
                const char* tabs_diff[5] = { purple_THPRAC_IGI_DIFF_E[Locale()], purple_THPRAC_IGI_DIFF_N[Locale()], purple_THPRAC_IGI_DIFF_H[Locale()], purple_THPRAC_IGI_DIFF_L[Locale()], purple_THPRAC_IGI_DIFF_EX[Locale()] };
                for (int diff = 0; diff < 5; diff++) {
                    if (ImGui::BeginTabItem(tabs_diff[diff])) {
                        ImGui::BeginTabBar("Player Type");
                        const char* tabs_player[4] = { purple_THPRAC_IGI_PL_ReimuA[Locale()], purple_THPRAC_IGI_PL_ReimuB[Locale()], purple_THPRAC_IGI_PL_MarisaA[Locale()], purple_THPRAC_IGI_PL_MarisaB[Locale()] };
                        for (int pl = 0; pl < 4; pl++) {
                            if (ImGui::BeginTabItem(tabs_player[pl])) {
                                // time
                                ImGui::BeginTable((std::string(tabs_diff[diff])+tabs_player[pl]+"timetable").c_str(), 4, ImGuiTableFlags_::ImGuiTableFlags_Resizable);
                                ImGui::TableSetupColumn(purple_THPRAC_INGAMEINFO_06_GAMTIME_TOT[Locale()]);
                                ImGui::TableSetupColumn(purple_THPRAC_INGAMEINFO_06_GAMTIME_CUR[Locale()]);
                                ImGui::TableSetupColumn(purple_THPRAC_INGAMEINFO_06_GAMTIME_CHARACTER_TOT[Locale()]);
                                ImGui::TableSetupColumn(purple_THPRAC_INGAMEINFO_06_GAMTIME_ALL[Locale()]);
                                ImGui::TableHeadersRow();

                                int64_t gametime_tot = Stats().GetTotalGameTime(diff,pl);
                                int64_t gametime_cur = Stats().GetCurrentGameTime(diff, pl);
                                int64_t gametime_chartot = 0;
                                for (int i = 0; i < 5; i++)
                                    gametime_chartot += Stats().GetTotalGameTime(i, pl);
                                int64_t gametime_all = 0;
                                for (int i = 0; i < 5; i++)
                                    for (int j = 0; j < 4; j++)
                                        gametime_all += Stats().GetTotalGameTime(i, j);

                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped(GetTime_HHMMSS(gametime_tot).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped(GetTime_HHMMSS(gametime_cur).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped(GetTime_HHMMSS(gametime_chartot).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped(GetTime_HHMMSS(gametime_all).c_str());

                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(gametime_tot).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(gametime_cur).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(gametime_chartot).c_str());
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(gametime_all).c_str());

                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped("(%lld %s)", gametime_tot, purple_THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND[Locale()]);
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped("(%lld %s)", gametime_cur, purple_THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND[Locale()]);
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped("(%lld %s)", gametime_chartot, purple_THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND[Locale()]);
                                ImGui::TableNextColumn();
                                ImGui::TextWrapped("(%lld %s)", gametime_all, purple_THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND[Locale()]);

                                ImGui::EndTable();
                                // spell capture
                                ImGui::BeginTable((std::string(tabs_diff[diff])+tabs_player[pl]+"sptable").c_str(), 5, ImGuiTableFlags_::ImGuiTableFlags_Resizable);
                                ImGui::TableSetupColumn(purple_THPRAC_INGAMEINFO_06_SPELL_NAME[Locale()], 0, 100.0f);
                                ImGui::TableSetupColumn(purple_THPRAC_INGAMEINFO_06_CAPTURE_TOT[Locale()], 0, 50.0f);
                                ImGui::TableSetupColumn(purple_THPRAC_INGAMEINFO_06_TIMEOUT_TOT[Locale()], 0, 30.0f);
                                ImGui::TableSetupColumn(purple_THPRAC_INGAMEINFO_06_CAPTURE_CUR[Locale()], 0, 50.0f);
                                ImGui::TableSetupColumn(purple_THPRAC_INGAMEINFO_06_TIMEOUT_CUR[Locale()], 0, 30.0f);
                                ImGui::TableHeadersRow();

                                for (int spell = 0; spell < 65; spell++) {
                                    if (is_spell_used[diff][spell]) {
                                        ImGui::TableNextRow();
                                        ImGui::TableNextColumn();
                                        ImGui::Text("%s", th06_spells_str[Locale()][spell]);
                                        ImGui::TableNextColumn();

                                        ImGui::Text("%d/%d(%.1f%%)",
                                                    Stats().GetTotalSpellCardCount(spell, diff, pl, PurpleSave::Capture),
                                                    Stats().GetTotalSpellCardCount(spell, diff, pl, PurpleSave::Attempt),
                                            ((float)Stats().GetTotalSpellCardCount(spell, diff, pl, PurpleSave::Capture)) /
                                           std::fmax(1.0f,((float)Stats().GetTotalSpellCardCount(spell, diff, pl, PurpleSave::Attempt))) * 100.0f);
                                        ImGui::TableNextColumn();

                                        ImGui::Text("%d", Stats().GetTotalSpellCardCount(spell, diff, pl, PurpleSave::Timeout));
                                        ImGui::TableNextColumn();

                                        ImGui::Text("%d/%d(%.1f%%)",
                                            Stats().GetCurrentSpellCardCount(spell, diff, pl, PurpleSave::Capture),
                                            Stats().GetCurrentSpellCardCount(spell, diff, pl, PurpleSave::Attempt),
                                            ((float)Stats().GetCurrentSpellCardCount(spell, diff, pl, PurpleSave::Capture)) / 
                                            std::fmax(1.0f, ((float)Stats().GetCurrentSpellCardCount(spell, diff, pl, PurpleSave::Attempt))) * 100.0f);
                                        ImGui::TableNextColumn();

                                        ImGui::Text("%d", Stats().GetCurrentSpellCardCount(spell, diff, pl, PurpleSave::Timeout));
                                    }
                                }
                                ImGui::EndTable();
                                ImGui::EndTabItem();
                            }
                        }
                        ImGui::EndTabBar();
                        ImGui::EndTabItem();
                    }
                }
            }
            ImGui::EndTabBar();
        }
