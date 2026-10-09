// Generated purple ABTestRender (MIT), LF source sha256 2b412168cd8d71dd7ef8e1f79a1a5d33cf64f983792cbc9d0f4ccbc56e60ce03.
// Typed globals, per-run counter, C++17 formatting and side-label bounds are adapted.
    void practice_ab_test_result(ApplicationState& app)
    {
        auto& t = app.practice.ab_result_frames;
        const auto& globals = app.scene()->battle.enemy_world;
        const int m9923 = globals.integer_registers[3];
        if (m9923 == 6) {
            t++;
            float intp = (t >= 90) ? 1.0f : t / 90.0f;
            auto& io = ImGui::GetIO();
            auto szx = io.DisplaySize.x;
            auto szy = io.DisplaySize.y;
            ImVec2 mid_st = { 64.0f + 384.0f, 32.0f + 448.0f };
            mid_st.x *= szx / 1280.0f;
            mid_st.y *= szy / 960.0f;
            ImVec2 sz = { 380.0f * 2.0f, 600.0f * intp };
            sz.x *= szx / 1280.0f;
            sz.y *= szy / 960.0f;

            ImGui::SetNextWindowPos({ mid_st.x - sz.x * 0.5f, mid_st.y - sz.y * 0.5f });
            ImGui::SetNextWindowSize(sz);
            ImGui::SetNextWindowBgAlpha(0.8f);
            ImGui::Begin("##res", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
            if (t >= 90) {
                std::array<float,5> result_origs = {
                    globals.float_registers[7],
                    globals.float_registers[6],
                    globals.float_registers[5],
                    globals.float_registers[4],
                    globals.float_registers[3]
                };
                auto scores = practice_ab_scores(result_origs);
                float avg_score = 0.0f;
                for (int i = 0; i < 6; i++)
                    avg_score += scores[i];
                avg_score /= 6.0f;

                const char* abtest_rank;
                if (avg_score < 0.2f)
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_10);
                else if (avg_score < 0.36f)
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_9);
                else if (avg_score < 0.54f)
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_8);
                else if (avg_score < 0.72f)
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_7);
                else if (avg_score < 0.9f)
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_6);
                else if (avg_score < 0.95f)
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_5);
                else if (avg_score < 0.98f)
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_4);
                else if (avg_score < 0.985f)
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_3);
                else if (avg_score < 0.99f)
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_2);
                else
                    abtest_rank = label(practice_TH15_AB_TEST_RES_RANK_1);

                ImGui::Text("%s %s", label(practice_TH15_AB_TEST_RES), abtest_rank);
                ImGui::Text(label(practice_TH15_AB_TEST_RES_DESC));

                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 csz = ImGui::GetContentRegionAvail();
                ImVec2 cmid = { p0.x + csz.x * 0.5f, p0.y + csz.y * 0.5f };
                ImVec2 p1 = { cmid.x + csz.x * 0.5f, cmid.y + csz.y * 0.5f };
                float hheight = csz.y * 0.75f * 0.5f;

                ImDrawList* pw = ImGui::GetWindowDrawList();
                ImVec4 color1 = ImGui::GetStyleColorVec4(ImGuiCol_::ImGuiCol_ChildBg);
                color1.w = 0.1f;
                pw->AddRectFilled(p0, p1, ImGui::ColorConvertFloat4ToU32(color1));
                pw->AddRect(p0, p1, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_::ImGuiCol_Text)));
                pw->PushClipRect(p0, p1);
                {
                    ImVec2 points[6];
                    // radar
                    for (int i = 0; i < 6; i++) {
                        pw->AddLine(cmid, { cmid.x + hheight * cosf(i * 3.1415927410125732421875f * 2.0f / 6.0f - 1.57079f), cmid.y + hheight * sinf(i * 3.1415927410125732421875f * 2.0f / 6.0f - 1.57079f) }, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_::ImGuiCol_Text)), 2.0f);
                    }
                    // lines
                    constexpr float radiuses_ranks[] = {
                        0.18f / 0.9f * 0.5f,
                        0.36f / 0.9f * 0.5f,
                        0.54f / 0.9f * 0.5f,
                        0.72f / 0.9f * 0.5f,
                        0.5f, 0.65f, 0.85f, 1.0f
                    };
                    for (int i = 0; i < 8; i++) {
                        float ra = radiuses_ranks[i] * hheight;
                        for (int j = 0; j < 6; j++) {
                            points[j] = { cmid.x + ra * cosf(j * 3.1415927410125732421875f * 2.0f / 6.0f - 1.57079f), cmid.y + ra * sinf(j * 3.1415927410125732421875f * 2.0f / 6.0f - 1.57079f) };
                        }
                        pw->AddPolyline(points, 6, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_::ImGuiCol_Text)), ImDrawFlags_::ImDrawFlags_Closed, i == 7 ? 2.0f : 1.0f);
                    }
                    // scores
                    auto MInterpolation = [](float t, float a, float b) {
                        if (t < 0.0f) {
                            return a;
                        } else if (t < 0.5) {
                            float k = (b - a) * 2.0f;
                            return k * t * t + a;
                        } else if (t < 1.0f) {
                            float k = (b - a) * 2.0f;
                            t = t - 1.0f;
                            return -k * t * t + b;
                        }
                        return b;
                    };
                    float t2 = MInterpolation((t - 90.0f) / 90.0f, 0.0f, 1.0f);
                    for (int i = 0; i < 6; i++)
                        scores[i] *= t2;
                    // animation
                    for (int i = 0; i < 6; i++) {
                        float radius = scores[i];
                        if (scores[i] < 0.9f) {
                            radius = scores[i] / 0.9f * 0.5f;
                        } else if (scores[i] < 0.95f) {
                            radius = scores[i] * 3.0f - 2.2f;
                        } else if (scores[i] < 0.98f) {
                            radius = scores[i] * 6.66667f - 5.683333f;
                        } else {
                            radius = scores[i] * 7.5f - 6.5f;
                        }
                        points[i] = { cmid.x + hheight * radius * cosf(i * 3.1415927410125732421875f * 2.0f / 6.0f - 1.57079f), cmid.y + hheight * radius * sinf(i * 3.1415927410125732421875f * 2.0f / 6.0f - 1.57079f) };
                    }
                    for (int i = 0; i < 6; i++) {
                        auto col = ImGui::GetStyleColorVec4(ImGuiCol_PlotHistogram);
                        col.w = 0.75f;
                        pw->AddTriangleFilled(points[i], points[(i + 1) % 6], cmid, ImGui::ColorConvertFloat4ToU32(col));
                    }
                    pw->AddPolyline(points, 6, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_::ImGuiCol_PlotLines)), ImDrawFlags_::ImDrawFlags_Closed, 1.0f);

                    // text
                    const char* chars[] = {
                        label(practice_TH15_AB_TEST_REACTION),
                        label(practice_TH15_AB_TEST_MOV),
                        label(practice_TH15_AB_TEST_RET),
                        label(practice_TH15_AB_TEST_HORIZON),
                        label(practice_TH15_AB_TEST_PREC),
                        label(practice_TH15_AB_TEST_LUCK)
                    };
                    for (int i = 0; i < 6; i++) {
                        const char* rank = "E";
                        if (scores[i] < 0.18f)
                            rank = "F";
                        else if (scores[i] < 0.36f)
                            rank = "E";
                        else if (scores[i] < 0.54f)
                            rank = "D";
                        else if (scores[i] < 0.72f)
                            rank = "C";
                        else if (scores[i] < 0.9f)
                            rank = "B";
                        else if (scores[i] < 0.95f)
                            rank = "A"; // 0.9-0.95
                        else if (scores[i] < 0.98f)
                            rank = "S"; // 0.95-0.98
                        else if (scores[i] < 0.99f)
                            rank = "SS"; // 0.98-0.99
                        else
                            rank = "???"; // 0.99+
                        char formatted[256]; std::snprintf(formatted, sizeof formatted, "%s:%s(%3.1f)", chars[i], rank, scores[i] * 100.0f);
                        std::string text = formatted;
                        auto sz_text = ImGui::CalcTextSize(text.c_str());
                        float h2 = hheight;
                        if (i == 0 || i == 3)
                            h2 = hheight + sz_text.y;
                        else
                            h2 = hheight + sz_text.x * 0.85f;
                        ImVec2 pos = { cmid.x + h2 * cosf(i * 3.1415927410125732421875f * 2.0f / 6.0f - 1.57079f), cmid.y + h2 * sinf(i * 3.1415927410125732421875f * 2.0f / 6.0f - 1.57079f) };
                        pos.x -= sz_text.x * 0.5f;
                        if (i != 0 && i != 3) {
                            const float inset = ImGui::GetStyle().FramePadding.x * 2.0f;
                            const float left = p0.x + inset;
                            const float right = std::max(left, p1.x - inset - sz_text.x);
                            pos.x = std::clamp(pos.x, left, right);
                        }
                        pos.y -= sz_text.y * 0.5f;
                        pw->AddText(pos, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_::ImGuiCol_Text)), text.c_str());
                    }
                }
                pw->PopClipRect();
            }
            ImGui::End();
        } else {
            t = 0;
        }
    }
