#pragma once

#ifdef TH_ENABLE_THPRAC

#include <cstdint>

#include <imgui.h>

union SDL_Event;

namespace ThpracImGui
{
enum class Locale
{
    ZhCN,
    EnUS,
    JaJP,
};

enum class TextId
{
    Menu,
    Original,
    Custom,
    Mode,
    Stage,
    Extra,
    Warp,
    None,
    StagePortion,
    StagePortionN,
    MidBoss,
    EndBoss,
    NonSpell,
    SpellCard,
    Frame,
    FakeShot,
    ReimuA,
    ReimuB,
    MarisaA,
    MarisaB,
    Chapter,
    FirstHalf,
    SecondHalf,
    Dialog,
    Phase,
    Normal,
    Rage,
    Life,
    Bomb,
    Score,
    Power,
    Graze,
    Point,
    Rank,
    RankLock,
    Resume,
    Exit,
    Restart,
    Settings,
    Invincible,
    InfLives,
    InfBombs,
    InfPower,
    TimeLock,
    AutoBomb,
    EverlastingBgm,
    TrackerMiss,
    TrackerBomb,
};

bool Initialize();
void Shutdown();
bool IsInitialized();
Locale GetLocale();
void RequestLocale(Locale locale);
const char *Text(TextId id);
// Feed exactly one fixed-tick input sample.  Upstream thprac's TH06 Gen1
// bridge exposes rising edges, followed by the game's delayed 8-frame repeat;
// it never gives Dear ImGui a continuously-held direction every render frame.
void SetGameInput(std::uint16_t buttons, bool sampled);
void SetGameNavEnabled(bool enabled);
bool InputPressed(std::uint16_t button);

// Feed platform mouse-wheel events. Mouse position/buttons are sampled from
// SDL once per fixed ImGui frame so they stay in sync with the game's actual
// client area and the 640x480 logical render surface.
void ProcessEvent(const SDL_Event &event);

// The game owns the fixed 640x480 logical surface.  The platform backend
// deliberately does not read a separate window or request a server resource.
void BeginFrame(float deltaSeconds);
bool IsFrameOpen();
void EndFrame();
const ImDrawData *GetDrawData();

bool DebugLifecycleSelfTest();
bool DebugInputSamplingSelfTest();
} // namespace ThpracImGui

#endif
