// Generated from purple shared tools (MIT). Algorithm and drawing order remain source-owned.
bool GameFPSOpt(PracticeSpeed& ctx, bool replay)
{
    static char tmpStr[32] {};
    static int fps = 0;
    static int fpsStatic = 60;
    static int fpsSlowStatic = 0;
    static int fpsFastStatic = 0;
    static int fpsDebugAcc = 0;
    static int fpsMultiplier = 0;
    static bool canFpsChangeFreely = false;
    bool clickedApply = false;
    const char* fpsMultiplierStr;

    if (fps == 0) {
        if (ctx.fps_status == 1) {
            fpsStatic = fps = 60;
            ctx.fps_replay_slow = fpsSlowStatic = 15;
            ctx.fps_replay_fast = 60;
            fpsFastStatic = 1;
            fpsDebugAcc = ctx.fps_debug_acc;
        } else if (ctx.fps_status == 2) {
            fpsStatic = fps = ctx.fps;

            if (fps <= 60)
                fpsStatic = fps = 60;
            else if (fps % 15)
                canFpsChangeFreely = true;
            else
                fpsMultiplier = (fps - 60) / 15;

            if (ctx.fps_replay_slow > 60) {
                ctx.fps_replay_slow = 60;
            }
            fpsSlowStatic = ctx.fps_replay_slow;

            if (ctx.fps_replay_fast < 60) {
                ctx.fps_replay_fast = 60;
            }
            ctx.fps_replay_fast = ctx.fps_replay_fast - ctx.fps_replay_fast % 60;
            fpsFastStatic = ctx.fps_replay_fast / 60;

            fpsDebugAcc = ctx.fps_debug_acc;
        }
    }

    if (ctx.fps_status <= 0) {
        ImGui::PushTextWrapPos();
        if (ctx.fps_status == 0)
            ImGui::TextColored(ImColor(255, 0, 0), "%s", label(practice_TH_FPS_ERR));
        else if (ctx.fps_status == -1)
            ImGui::TextColored(ImColor(255, 0, 0), "%s", label(practice_TH_FPS_UNSUPPORTED));
        ImGui::PopTextWrapPos();
        ImGui::BeginDisabled();
    }

    switch (fpsMultiplier) {
    case 1:
        fpsMultiplierStr = "x1.25 (75fps)";
        break;
    case 2:
        fpsMultiplierStr = "x1.5 (90fps)";
        break;
    case 3:
        fpsMultiplierStr = "x1.75 (105fps)";
        break;
    case 4:
        fpsMultiplierStr = "x2.0 (120fps)";
        break;
    case 5:
        fpsMultiplierStr = "x2.25 (135fps)";
        break;
    case 6:
        fpsMultiplierStr = "x2.5 (150fps)";
        break;
    case 7:
        fpsMultiplierStr = "x2.75 (165fps)";
        break;
    case 8:
        fpsMultiplierStr = "x3.0 (180fps)";
        break;
    default:
        fpsMultiplierStr = "x1.0 (60fps)";
        break;
    }

    ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x*.23f);
    if (canFpsChangeFreely) {
        ImGui::DragInt(label(practice_TH_FPS_ADJ), &fps, 1.0f, 60, 6000);
        if (!ImGui::IsItemActive())
            fps = std::clamp(fps, 60, 6000);
    } else {
        ImGui::SliderInt(label(practice_TH_FPS_ADJ), &fpsMultiplier, 0, 8, fpsMultiplierStr);
        fps = fpsMultiplier * 15 + 60;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Checkbox(label(practice_TH_FPS_FREE_ADJ), &canFpsChangeFreely)) {
        if (!canFpsChangeFreely) {
            int i = 0;
            for (; (i * 15 + 60) <= fps && i <= 8; ++i)
                ;
            fpsMultiplier = --i;
            fps = fpsMultiplier * 15 + 60;
        }
    }

    if (replay) {
        ImGui::PushItemWidth(ImGui::GetIO().DisplaySize.x*.23f);
        if (fpsFastStatic > 20) {
            sprintf(tmpStr, "infinite");
        } else {
            sprintf(tmpStr, "x%d.0 (%dfps)", fpsFastStatic, fpsFastStatic * 60);
        }

        ImGui::SliderInt("Replay slow FPS", &fpsSlowStatic, 1, 60);
        ImGui::SliderInt("Replay fast FPS", &fpsFastStatic, 1, 21, tmpStr);
        ImGui::PopItemWidth();
    }
    ImGui::Checkbox("Debug acc.", (bool*)&fpsDebugAcc);
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");

    if (fpsStatic != fps
        || fpsSlowStatic != ctx.fps_replay_slow
        || fpsFastStatic != ctx.fps_replay_fast / 60
        || fpsDebugAcc != ctx.fps_debug_acc) {
        ImGui::SameLine();
        if (ImGui::Button(label(practice_TH_ADV_OPT_APPLY))) {
            clickedApply = true;
            if (fpsStatic > fps && ctx.fps_status != 1)
                ImGui::TextUnformatted(label(practice_TH_FPS_LOWERING));
            fpsStatic = fps;
            ctx.fps_replay_slow = fpsSlowStatic;
            ctx.fps_replay_fast = fpsFastStatic * 60;
            ctx.fps_debug_acc = fpsDebugAcc;
        }
    }

    if (ctx.fps_status <= 0)
        ImGui::EndDisabled();

    ctx.fps = fpsStatic;

    

    if (clickedApply)  {
        
    }
    

    return clickedApply;
}

