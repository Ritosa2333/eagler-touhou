// Generated from purple shared tools (MIT). Algorithm and drawing order remain source-owned.
class THGuiTestReactionTest {
    enum TestState {
        NOT_BEGIN,
        WAIT_TIME,
        WAIT_TIME_PRESSED,
        REACT_TIME,
        TOO_EARLY,
        SHOW_RES,
    };
    enum TestType {
        PRESS,
        RELEASE
    };

public:
    THGuiTestReactionTest();
    bool GuiUpdate(bool ingame = false);
    void Reset();

private:
    PracticeCounter mTimeFreq;
    PracticeCounter mPressTime;
    PracticeCounter mWaitTime;
    int mFrameCount;

    TestState mTestState;
    TestType mTestType;
    int mTestTime;
    bool mShowProgressBar;

    int mCurTest;
    std::function<unsigned int(void)> mRndSeedGen;
    std::vector<float> mResults;
    std::vector<float> mFrameCounts;
};
THGuiTestReactionTest::THGuiTestReactionTest()
{
    practice_counter_frequency(&mTimeFreq);
    mTestState = NOT_BEGIN;
    mTestType = PRESS;
    mTestTime = 5;
    mRndSeedGen = practice_random_generator(1200u, 2800u);
    // mRndSeedGen = practice_random_generator(0u, 1u);
    mShowProgressBar = false;
    mCurTest = 0;
}
bool THGuiTestReactionTest::GuiUpdate(bool ingame)
{
    PracticeCounter curTime;
    practice_counter_now(&curTime);

    ImVec2 colorBtnSz = ImVec2(ImGui::GetWindowWidth()*0.5f, ImGui::GetTextLineHeight() * 10.0f);
    ImVec2 colorBtnPos = ImVec2(ImGui::GetWindowWidth()*0.25f, ImGui::GetTextLineHeight() * 2.0f);
    unsigned colorBtnFlag = ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip;
    ImVec4 color_BeforeTimeReact = ImVec4(1, 0.2, 0.2, 1);
    ImVec4 color_AfterTimeReact = ImVec4(0, 1, 0.2, 1);
    ImVec4 color_WaitPress = ImVec4(1, 1, 0.2, 1);

    bool isKeyPressed = ImGui::IsKeyDown(37) || ImGui::IsKeyDown(38) || ImGui::IsKeyDown(39) || ImGui::IsKeyDown(40);
    if (!ingame){
        if (ImGui::Button(label(practice_THPRAC_BACK))) {
            Reset();
            return false;
        }
    }
    switch (mTestState)
    {
    case NOT_BEGIN:
    {
        if (!ingame) {
            ImGui::SameLine();
            ImGui::TextUnformatted(label(practice_THPRAC_TOOLS_REACTION_TEST));
            ImGui::Separator();
        }
    
        ImGui::RadioButton(label(practice_THPRAC_TOOLS_REACTION_TEST_DOWN), (int*)&mTestType, PRESS);
        ImGui::SameLine();
        ImGui::RadioButton(label(practice_THPRAC_TOOLS_REACTION_TEST_UP), (int*)&mTestType, RELEASE);
    
        ImGui::Checkbox(label(practice_THPRAC_TOOLS_REACTION_SHOW_PROC_BAR), &mShowProgressBar);
    
        ImGui::DragInt(label(practice_THPRAC_TOOLS_REACTION_TEST_TIME), &mTestTime, 1.0f, 1, 20);
        ImGui::NewLine();
        if (ImGui::Button(label(practice_THPRAC_TOOLS_REACTION_TEST_BEGIN)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(16)) {
            // in win 7, due to the initialize sequence, practice_counter_frequency might get 0 value at the beginning, 
            // so query it every time before test begin to avoid this problem
            practice_counter_frequency(&mTimeFreq);
            mTestState = WAIT_TIME;
            mCurTest = 1;
            mFrameCount = 0;
            if (mTestType == PRESS) {
                mWaitTime.QuadPart = static_cast<int64_t>(mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart);
                mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
            }
            mResults = {};
        }
        ImGui::InvisibleButton("inv", colorBtnSz);
    } break;
    
    case TOO_EARLY: {
        ImGui::Text(label(practice_THPRAC_TOOLS_REACTION_TEST_TOO_EARLY));
        if (ImGui::Button(label(practice_THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(16)) // press z(90)/shift(16)
        {
            mFrameCount = 0;
            mTestState = WAIT_TIME;
            mWaitTime.QuadPart = static_cast<int64_t>(mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart);
            mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
        }
        ImGui::InvisibleButton("inv", colorBtnSz);
    } break;
    case SHOW_RES: {
        if (mResults.size() != 0 && mFrameCounts.size() != 0)
        {
            ImGui::Text("%s(%d): %.1f ms (%.1f frame)(framecount: %d)", label(practice_THPRAC_TOOLS_REACTION_TEST_RESULT), mCurTest, mResults[mResults.size() - 1], mResults[mResults.size() - 1] / 16.66667f,(int)mFrameCounts[mFrameCounts.size()-1]);
            if (mCurTest < mTestTime)
                ImGui::InvisibleButton("inv", colorBtnSz);
        }else
            ImGui::Text("%s(%d): ?????", label(practice_THPRAC_TOOLS_REACTION_TEST_RESULT), mCurTest);
        if (mCurTest == mTestTime)
        {
            float avg = 0.0f;
            float maxv = 60.0f;
            float minv = 0.0f;
            float avg_frameCnt = 0.0f;
            float maxfcnt = 10.0f;
            float minfcnt = 0.0f;
            for (auto x : mResults) {
                avg += fabsf(x);
                maxv = std::max(x, maxv);
                minv = std::min(x, minv);
            }
            for (auto x : mFrameCounts) {
                avg_frameCnt += fabsf(x);
                maxfcnt = std::max(x, maxfcnt);
                minfcnt = std::min(x, minfcnt);
            }
            avg /= (float)mTestTime;
            avg_frameCnt /= (float)mTestTime;
            ImGui::Separator();
            ImGui::Text("%s: %s, %s", label(practice_THPRAC_TOOLS_REACTION_MODE), mTestType == PRESS ? label(practice_THPRAC_TOOLS_REACTION_MODE_DOWN) : label(practice_THPRAC_TOOLS_REACTION_MODE_UP), 
                mShowProgressBar ? label(practice_THPRAC_TOOLS_REACTION_MODE_PROGRESSBAR) : label(practice_THPRAC_TOOLS_REACTION_MODE_NORMAL)
            );
            ImGui::Text("%s: %.1f ms (%.1f frame)(framecount: %.1f)", label(practice_THPRAC_TOOLS_REACTION_TEST_RESULT_AVG), avg, avg / 16.66667f, avg_frameCnt);
            
            ImGui::PlotHistogram(label(practice_THPRAC_TOOLS_REACTION_TEST_RESULT), &mResults[0], mTestTime, 0, label(practice_THPRAC_TOOLS_REACTION_TEST_RESULT), minv-10.0f, maxv+20.0f, ImVec2(0, 200.0));
            
            ImGui::PlotHistogram((std::string(label(practice_THPRAC_TOOLS_REACTION_TEST_RESULT))+"(framecount)").c_str(), &mFrameCounts[0], mTestTime, 0, (std::string(label(practice_THPRAC_TOOLS_REACTION_TEST_RESULT))+"(framecount)").c_str(), minfcnt-1.0f, maxfcnt + 10.0f, ImVec2(0, 200.0));
            if (ImGui::Button(label(practice_THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(16)) {
                mTestState = NOT_BEGIN;
            }
        } else {
            if (ImGui::Button(label(practice_THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(16)) // press z
            {
                mCurTest++;
                mTestState = WAIT_TIME;
                mFrameCount = 0;
                mWaitTime.QuadPart = static_cast<int64_t>(mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart);
                mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
            }
        }
    } break;
    
    case REACT_TIME:
    case WAIT_TIME:
    case WAIT_TIME_PRESSED:
    {
        if (mTestType == PRESS)
        {
            if (mTestState == WAIT_TIME && curTime.QuadPart >= mPressTime.QuadPart){
                mTestState = REACT_TIME;
                mFrameCount = 0;
            }
            ImGui::Text("test %d", mCurTest);
            mFrameCount++;
            if (mTestState == WAIT_TIME) {
                if (mShowProgressBar) {
                    ImGui::ProgressBar(std::clamp(std::fabs(static_cast<float>(((double)(mPressTime.QuadPart - curTime.QuadPart)) / ((double)mWaitTime.QuadPart))), 0.0f, 1.0f), ImVec2(0, 0), "waiting...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_BeforeTimeReact, colorBtnFlag, colorBtnSz);
                if (isKeyPressed){
                    if (!mShowProgressBar){
                        mTestState = TOO_EARLY;
                    } else {
                        // allow negative reaction time
                        float curTestReactionTimeMs = static_cast<float>(((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0);
                        mResults.push_back(curTestReactionTimeMs);
                        mFrameCounts.push_back(ceilf(fabsf(curTestReactionTimeMs))/16.6667f);
                        mTestState = SHOW_RES;
                    }
                }
            } else {
                if (mShowProgressBar) {
                    ImGui::ProgressBar(0.0f, ImVec2(0, 0), "press...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_AfterTimeReact, colorBtnFlag, colorBtnSz);
                if (isKeyPressed){
                    float curTestReactionTimeMs = static_cast<float>(((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0);
                    mResults.push_back(curTestReactionTimeMs);
                    mFrameCounts.push_back(static_cast<float>(mFrameCount));
                    mTestState = SHOW_RES;
                }
            }
        }else{
            if (mTestState == WAIT_TIME_PRESSED && curTime.QuadPart >= mPressTime.QuadPart) {
                mTestState = REACT_TIME;
                mFrameCount = 0;
            }
            mFrameCount++;
            ImGui::Text("test %d", mCurTest);
            if (mTestState == WAIT_TIME) {
                if (mShowProgressBar) {
                    ImGui::ProgressBar(1.0f, ImVec2(0, 0), "waiting...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_WaitPress, colorBtnFlag, colorBtnSz);
                if (isKeyPressed) {
                    mWaitTime.QuadPart = static_cast<int64_t>(mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart);
                    mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
                    mTestState = WAIT_TIME_PRESSED;
                }
            } else if(mTestState == REACT_TIME){
                if (mShowProgressBar) {
                    ImGui::ProgressBar(0.0f, ImVec2(0, 0), "press...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_AfterTimeReact, colorBtnFlag, colorBtnSz);
                if (!isKeyPressed) {
                    float curTestReactionTimeMs = static_cast<float>(((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0);
                    mResults.push_back(curTestReactionTimeMs);
                    mFrameCounts.push_back(static_cast<float>(mFrameCount));
                    mTestState = SHOW_RES;
                }
            }
            else if (mTestState == WAIT_TIME_PRESSED){
                if (mShowProgressBar) {
                    ImGui::ProgressBar(std::clamp(static_cast<float>(std::abs(((double)(mPressTime.QuadPart - curTime.QuadPart)) / ((double)mWaitTime.QuadPart))), 0.0f, 1.0f), ImVec2(0, 0), "waiting...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_BeforeTimeReact, colorBtnFlag, colorBtnSz);
                if (!isKeyPressed) {
                    if (!mShowProgressBar) {
                        mTestState = TOO_EARLY;
                    } else {
                        float curTestReactionTimeMs =  static_cast<float>(((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0);
                        mResults.push_back(curTestReactionTimeMs);
                        mFrameCounts.push_back(ceilf(fabsf(curTestReactionTimeMs)) / 16.6667f);
                        mTestState = SHOW_RES;
                    }
                }
            }
        }
    } 
    break;
    }
    return true;
}

void THGuiTestReactionTest::Reset()
{
    practice_counter_frequency(&mTimeFreq);
    mTestState = NOT_BEGIN;
    mTestType = PRESS;
    mTestTime = 5;
    mRndSeedGen = practice_random_generator(1200u, 3500u);
    mShowProgressBar = false;
    mCurTest = 0;
    mResults = {};
    mFrameCounts = {};
}


