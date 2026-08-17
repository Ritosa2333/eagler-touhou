#include "Touch.hpp"

#include <SDL3/SDL_events.h>
#include <cmath>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "Controller.hpp"
#include "EaglerOptions.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"

struct FingerSlot
{
    bool active;
    SDL_FingerID id;
    f32 lastPxX;
    f32 lastPxY;
    u64 start;
    u64 end;
};

struct MenuGestureTracker
{
    bool active;
    SDL_FingerID primaryId;
    f32 startX;
    f32 startY;
    f32 currentX;
    f32 currentY;
    i32 maxFingers;
    u16 pendingButton;
};

FingerSlot g_MoveFinger;
FingerSlot g_FocusFinger;
FingerSlot g_DialogueHoldFinger;
MenuGestureTracker g_MenuGesture;

f32 g_AccumDx = 0.0f;
f32 g_AccumDy = 0.0f;

bool g_UsedThisRun = false;

// hopefully you only have 10 fingers
SDL_FingerID g_ActiveGameplayFingerIds[10];
i32 g_NumActiveGameplayFingers = 0;

bool g_BombPending = false;
bool g_PausePending = false;
bool g_BombedWithTouch = false;
i32 g_LastHostBombSerial = 0;
i32 g_LastHostEscapeSerial = 0;

bool IsGameplayTouchMode()
{
    return g_GameManager.isInMenu && !g_GameManager.isInGameMenu &&
           !g_GameManager.isInRetryMenu && !g_GameManager.isInReplay &&
           !g_GameManager.demoMode;
}

void GetWindowSize(i32 *w, i32 *h)
{
    *w = 640;
    *h = 480;
    if (g_GameWindow.window)
    {
        SDL_GetWindowSize(g_GameWindow.window, w, h);
    }
}

f32 GetSwipeThreshold();

bool ReleaseMenuGestureFinger(SDL_FingerID id)
{
    if (!g_MenuGesture.active)
    {
        return false;
    }

    const bool primaryReleased = id == g_MenuGesture.primaryId;
    const bool multiFingerGesture = g_MenuGesture.maxFingers >= 2;
    if (!primaryReleased && !multiFingerGesture)
    {
        return false;
    }

    f32 dx = g_MenuGesture.currentX - g_MenuGesture.startX;
    f32 dy = g_MenuGesture.currentY - g_MenuGesture.startY;

    if (std::abs(dx) <= GetSwipeThreshold() && std::abs(dy) <= GetSwipeThreshold())
    {
        g_MenuGesture.pendingButton =
            multiFingerGesture ? TH_BUTTON_RETURNMENU : TH_BUTTON_SELECTMENU;
    }

    // A menu gesture used to be released only by primaryId. If a two-finger
    // gesture delivered the secondary UP first and the primary UP was lost,
    // active stayed true forever and every later touch was attached to that
    // stale gesture. Any participating UP now terminates a multi-finger
    // gesture, while single-finger taps still require their primary finger.
    g_MenuGesture.active = false;
    return true;
}

void GetContainedRenderRect(f32 *outX, f32 *outY, f32 *outW, f32 *outH, f32 *outScale)
{
    i32 winW, winH;
    GetWindowSize(&winW, &winH);

    f32 sx = (f32)winW / 640.0f;
    f32 sy = (f32)winH / 480.0f;
    f32 scale = sx < sy ? sx : sy;

    f32 rw = 640.0f * scale;
    f32 rh = 480.0f * scale;
    f32 rx = ((f32)winW - rw) * 0.5f;
    f32 ry = ((f32)winH - rh) * 0.5f;

    *outX = rx;
    *outY = ry;
    *outW = rw;
    *outH = rh;
    *outScale = scale;
}

void FingerToWindowPx(const SDL_TouchFingerEvent &f, f32 *px, f32 *py)
{
    i32 winW, winH;
    GetWindowSize(&winW, &winH);

    *px = f.x * (f32)winW;
    *py = f.y * (f32)winH;
}

f32 GetSwipeThreshold()
{
    i32 winW, winH;
    GetWindowSize(&winW, &winH);
    return (f32)winH * 0.05f;
}

bool IsBombZone(f32 px, f32 py)
{
    i32 winW, winH;
    GetWindowSize(&winW, &winH);

    f32 rx, ry, rw, rh, scale;
    GetContainedRenderRect(&rx, &ry, &rw, &rh, &scale);

    if (rx > 1.0f && (px < rx || px > (f32)winW - rx))
    {
        return true;
    }

    return px < (f32)winW * 0.15f && py > (f32)winH * 0.85f;
}

bool IsFinger(const FingerSlot &slot, SDL_FingerID id)
{
    return slot.active && slot.id == id;
}

void AssignFinger(FingerSlot *slot, SDL_FingerID id, f32 px, f32 py)
{
    slot->active = true;
    slot->id = id;
    slot->lastPxX = px;
    slot->lastPxY = py;
    slot->start = SDL_GetTicks();
    slot->end = 0;
}

void ReleaseFinger(FingerSlot *slot)
{
    slot->active = false;
    slot->end = SDL_GetTicks();
}

bool HasGameplayFinger(SDL_FingerID id)
{
    for (i32 i = 0; i < g_NumActiveGameplayFingers; i++)
    {
        if (g_ActiveGameplayFingerIds[i] == id)
        {
            return true;
        }
    }

    return false;
}

void AddGameplayFinger(SDL_FingerID id)
{
    if (HasGameplayFinger(id))
    {
        return;
    }

    if (g_NumActiveGameplayFingers < 10)
    {
        g_ActiveGameplayFingerIds[g_NumActiveGameplayFingers++] = id;
    }
}

void RemoveGameplayFinger(SDL_FingerID id)
{
    for (i32 i = 0; i < g_NumActiveGameplayFingers; i++)
    {
        if (g_ActiveGameplayFingerIds[i] == id)
        {
            g_ActiveGameplayFingerIds[i] =
                g_ActiveGameplayFingerIds[g_NumActiveGameplayFingers - 1];

            g_NumActiveGameplayFingers--;
            return;
        }
    }
}

void ClearGameplayFingers()
{
    g_NumActiveGameplayFingers = 0;
}

void ReleaseGameplayFingerState(SDL_FingerID id)
{
    RemoveGameplayFinger(id);

    if (IsFinger(g_DialogueHoldFinger, id))
    {
        ReleaseFinger(&g_DialogueHoldFinger);
    }
    if (IsFinger(g_MoveFinger, id))
    {
        ReleaseFinger(&g_MoveFinger);
        g_AccumDx = 0.0f;
        g_AccumDy = 0.0f;
    }
    if (IsFinger(g_FocusFinger, id))
    {
        ReleaseFinger(&g_FocusFinger);
    }
}

void Touch::ResetRunUsage()
{
    g_UsedThisRun = false;
    g_BombedWithTouch = false;
}

bool Touch::WasUsedThisRun()
{
    return g_UsedThisRun;
}

bool Touch::UsedTouchToBomb()
{
    return g_BombedWithTouch;
}

void Touch::CancelTouches()
{
    ReleaseFinger(&g_MoveFinger);
    ReleaseFinger(&g_FocusFinger);
    ReleaseFinger(&g_DialogueHoldFinger);

    g_AccumDx = 0.0f;
    g_AccumDy = 0.0f;

    ClearGameplayFingers();
    g_PausePending = false;
    g_BombPending = false;
    g_BombedWithTouch = false;
    g_MenuGesture = {};
}

void Touch::FingerDown(const SDL_TouchFingerEvent &f)
{
    if (!EaglerOptions::TouchEnabled())
    {
        return;
    }

    // Browser/SDL touch backends can legitimately synthesize a new DOWN for an
    // id whose previous UP/CANCELED was lost. Match SDL's own duplicate-touch
    // recovery semantics by clearing any stale role held by this id first.
    if (HasGameplayFinger(f.fingerID) || IsFinger(g_MoveFinger, f.fingerID) ||
        IsFinger(g_FocusFinger, f.fingerID) || IsFinger(g_DialogueHoldFinger, f.fingerID))
    {
        ReleaseGameplayFingerState(f.fingerID);
    }

    f32 px, py;
    FingerToWindowPx(f, &px, &py);

    if (!IsGameplayTouchMode())
    {
        if (!g_MenuGesture.active)
        {
            g_MenuGesture.active = true;
            g_MenuGesture.primaryId = f.fingerID;
            g_MenuGesture.startX = px;
            g_MenuGesture.startY = py;
            g_MenuGesture.currentX = px;
            g_MenuGesture.currentY = py;
            g_MenuGesture.maxFingers = 1;
        }
        else
        {
            g_MenuGesture.maxFingers++;
        }
    }
    else
    {
        if (g_PausePending)
        {
            return;
        }

        if (EaglerOptions::TouchBombZoneEnabled() && IsBombZone(px, py))
        {
            g_BombPending = true;
            g_UsedThisRun = true;
            return;
        }

        AddGameplayFinger(f.fingerID);
        if (g_NumActiveGameplayFingers >= 4)
        {
            g_PausePending = true;
        }

        if (g_Gui.HasCurrentMsgIdx() && !g_DialogueHoldFinger.active)
        {
            AssignFinger(&g_DialogueHoldFinger, f.fingerID, px, py);
            g_UsedThisRun = true;
            return;
        }

        if (!g_MoveFinger.active)
        {
            if (g_FocusFinger.active && f.fingerID == g_FocusFinger.id)
            {
                return;
            }

            AssignFinger(&g_MoveFinger, f.fingerID, px, py);
            g_UsedThisRun = true;
            return;
        }

        if (EaglerOptions::TouchFocusUsesTwoFingers() && !g_FocusFinger.active && f.fingerID != g_MoveFinger.id)
        {
            AssignFinger(&g_FocusFinger, f.fingerID, px, py);
            g_UsedThisRun = true;
            return;
        }
    }
}

void Touch::FingerUp(const SDL_TouchFingerEvent &f)
{
    if (!EaglerOptions::TouchEnabled())
    {
        // Never leave a latched gameplay role behind merely because touch was
        // disabled between DOWN and UP.
        ReleaseGameplayFingerState(f.fingerID);
        return;
    }
    f32 px, py;
    FingerToWindowPx(f, &px, &py);

    // Cleanup must be keyed by the finger that ended, not by the game mode at
    // the instant the UP arrives. A state transition between DOWN and UP used
    // to route the event into the menu branch and leave move/focus latched.
    ReleaseGameplayFingerState(f.fingerID);

    if (!IsGameplayTouchMode())
    {
        ReleaseMenuGestureFinger(f.fingerID);
    }
}

void Touch::FingerMotion(const SDL_TouchFingerEvent &f)
{
    if (!EaglerOptions::TouchEnabled())
    {
        return;
    }
    f32 px, py;
    FingerToWindowPx(f, &px, &py);

    if (!IsGameplayTouchMode())
    {
        if (g_MenuGesture.active && f.fingerID == g_MenuGesture.primaryId)
        {
            g_MenuGesture.currentX = px;
            g_MenuGesture.currentY = py;
        }
    }
    else
    {
        if (IsFinger(g_DialogueHoldFinger, f.fingerID))
        {
            g_DialogueHoldFinger.lastPxX = px;
            g_DialogueHoldFinger.lastPxY = py;
        }

        if (IsFinger(g_MoveFinger, f.fingerID))
        {
            f32 rx, ry, rw, rh, scale;
            GetContainedRenderRect(&rx, &ry, &rw, &rh, &scale);

            f32 dxPx = px - g_MoveFinger.lastPxX;
            f32 dyPx = py - g_MoveFinger.lastPxY;

            if (scale > 0.0f)
            {
                g_AccumDx += dxPx / scale;
                g_AccumDy += dyPx / scale;
            }
            g_MoveFinger.lastPxX = px;
            g_MoveFinger.lastPxY = py;
        }

        if (IsFinger(g_FocusFinger, f.fingerID))
        {
            g_FocusFinger.lastPxX = px;
            g_FocusFinger.lastPxY = py;
        }
    }
}

u16 Touch::GetButtonBits()
{
    if (!EaglerOptions::TouchEnabled())
    {
        return 0;
    }
    u16 buttons = 0;

    g_BombedWithTouch = false;

    const i32 hostBombSerial = EaglerOptions::TouchBombSerial();
    if (hostBombSerial != g_LastHostBombSerial)
    {
        g_LastHostBombSerial = hostBombSerial;
        if (IsGameplayTouchMode())
        {
            g_BombPending = true;
            g_UsedThisRun = true;
        }
    }

    const i32 hostEscapeSerial = EaglerOptions::TouchEscapeSerial();
    if (hostEscapeSerial != g_LastHostEscapeSerial)
    {
        g_LastHostEscapeSerial = hostEscapeSerial;
        buttons |= TH_BUTTON_MENU;
    }

    if (!IsGameplayTouchMode())
    {
        if (g_MenuGesture.pendingButton != 0)
        {
            buttons |= g_MenuGesture.pendingButton;
            g_MenuGesture.pendingButton = 0;
        }

        if (g_MenuGesture.active)
        {
            f32 dx = g_MenuGesture.currentX - g_MenuGesture.startX;
            f32 dy = g_MenuGesture.currentY - g_MenuGesture.startY;
            f32 threshold = GetSwipeThreshold();

            if (std::abs(dx) > threshold || std::abs(dy) > threshold)
            {
                if (std::abs(dx) > std::abs(dy))
                {
                    buttons |= (dx > 0) ? TH_BUTTON_RIGHT : TH_BUTTON_LEFT;
                }
                else
                {
                    buttons |= (dy > 0) ? TH_BUTTON_DOWN : TH_BUTTON_UP;
                }
            }
        }
    }

    if (!g_Gui.HasCurrentMsgIdx() && g_DialogueHoldFinger.active)
    {
        if (!g_MoveFinger.active)
        {
            AssignFinger(&g_MoveFinger, g_DialogueHoldFinger.id, g_DialogueHoldFinger.lastPxX,
                         g_DialogueHoldFinger.lastPxY);
        }
        ReleaseFinger(&g_DialogueHoldFinger);
    }
    else if (g_DialogueHoldFinger.active)
    {
        u64 held = SDL_GetTicks() - g_DialogueHoldFinger.start;

        if (held >= 500)
        {
            buttons |= TH_BUTTON_SKIP;
        }
    }

    // keep firing for a bit after release
    if (EaglerOptions::TouchFireEnabled() && IsGameplayTouchMode())
    {
        buttons |= TH_BUTTON_SHOOT;
        g_UsedThisRun = true;
    }

    if ((EaglerOptions::TouchFocusUsesTwoFingers() && g_FocusFinger.active) ||
        (EaglerOptions::TouchFocusButtonEnabled() && IsGameplayTouchMode()))
    {
        buttons |= TH_BUTTON_FOCUS;
    }

    if (g_BombPending)
    {
        buttons |= TH_BUTTON_BOMB;
        g_BombPending = false;
        g_BombedWithTouch = true;
    }

    if (g_PausePending)
    {
        buttons |= TH_BUTTON_MENU;
        Touch::CancelTouches();
    }

    return buttons;
}

bool Touch::IsFocus()
{
    return EaglerOptions::TouchEnabled() &&
           ((EaglerOptions::TouchFocusUsesTwoFingers() && g_FocusFinger.active) ||
            (EaglerOptions::TouchFocusButtonEnabled() && IsGameplayTouchMode()));
}

bool Touch::IsUnlimited()
{
    return EaglerOptions::TouchEnabled() && EaglerOptions::UnlimitedTouch();
}

bool Touch::GetPlayerDelta(f32 *dx, f32 *dy)
{
    if (!EaglerOptions::TouchEnabled() || !g_MoveFinger.active)
    {
        *dx = 0.0f;
        *dy = 0.0f;
        return false;
    }

    *dx = g_AccumDx;
    *dy = g_AccumDy;

    return true;
}

void Touch::SetPlayerDelta(f32 dx, f32 dy)
{
    g_AccumDx = dx;
    g_AccumDy = dy;
}

void Touch::ConsumePlayerDelta(f32 dx, f32 dy)
{
    g_AccumDx -= dx;
    g_AccumDy -= dy;
}

#ifdef TH_DEV_TOOLS
bool Touch::DebugStateSelfTest()
{
    Touch::CancelTouches();

    g_MoveFinger = {true, 101, 10.0f, 20.0f, 0, 0};
    g_FocusFinger = {true, 202, 30.0f, 40.0f, 0, 0};
    g_DialogueHoldFinger = {true, 303, 50.0f, 60.0f, 0, 0};
    g_ActiveGameplayFingerIds[0] = 101;
    g_ActiveGameplayFingerIds[1] = 202;
    g_ActiveGameplayFingerIds[2] = 303;
    g_NumActiveGameplayFingers = 3;
    g_AccumDx = 7.0f;
    g_AccumDy = -5.0f;

    ReleaseGameplayFingerState(202);
    const bool focusReleasedOnly =
        !g_FocusFinger.active && g_MoveFinger.active && g_DialogueHoldFinger.active &&
        g_NumActiveGameplayFingers == 2 && !HasGameplayFinger(202);

    ReleaseGameplayFingerState(101);
    const bool moveReleasedAndDeltaCleared =
        !g_MoveFinger.active && g_AccumDx == 0.0f && g_AccumDy == 0.0f &&
        g_NumActiveGameplayFingers == 1 && !HasGameplayFinger(101);

    ReleaseGameplayFingerState(303);
    const bool dialogueReleased =
        !g_DialogueHoldFinger.active && g_NumActiveGameplayFingers == 0;

    g_MenuGesture = {true, 401, 100.0f, 100.0f, 100.0f, 100.0f, 2, 0};
    const bool secondaryEndedTwoFingerMenuGesture =
        ReleaseMenuGestureFinger(402) && !g_MenuGesture.active &&
        g_MenuGesture.pendingButton == TH_BUTTON_RETURNMENU;

    g_MenuGesture = {true, 501, 120.0f, 120.0f, 120.0f, 120.0f, 1, 0};
    const bool unrelatedFingerDoesNotEndSingleFingerGesture =
        !ReleaseMenuGestureFinger(502) && g_MenuGesture.active && g_MenuGesture.pendingButton == 0;
    const bool primaryEndsSingleFingerGesture =
        ReleaseMenuGestureFinger(501) && !g_MenuGesture.active &&
        g_MenuGesture.pendingButton == TH_BUTTON_SELECTMENU;

    Touch::CancelTouches();
    return focusReleasedOnly && moveReleasedAndDeltaCleared && dialogueReleased &&
           secondaryEndedTwoFingerMenuGesture && unrelatedFingerDoesNotEndSingleFingerGesture &&
           primaryEndsSingleFingerGesture;
}
#endif

#ifdef __EMSCRIPTEN__
// SDL only reports touches that begin on its canvas. The Web shell forwards
// touches from the surrounding letterbox through these equivalent entry points.
extern "C" EMSCRIPTEN_KEEPALIVE void TouhouAuxTouchDown(i32 id, f32 x, f32 y)
{
    if (!EaglerOptions::TouchEnabled())
    {
        return;
    }

    SDL_TouchFingerEvent event = {};
    event.fingerID = static_cast<SDL_FingerID>(id);
    event.x = x;
    event.y = y;
    Touch::FingerDown(event);
}

extern "C" EMSCRIPTEN_KEEPALIVE void TouhouAuxTouchMotion(i32 id, f32 x, f32 y)
{
    SDL_TouchFingerEvent event = {};
    event.fingerID = static_cast<SDL_FingerID>(id);
    event.x = x;
    event.y = y;
    Touch::FingerMotion(event);
}

extern "C" EMSCRIPTEN_KEEPALIVE void TouhouAuxTouchUp(i32 id, f32 x, f32 y)
{
    SDL_TouchFingerEvent event = {};
    event.fingerID = static_cast<SDL_FingerID>(id);
    event.x = x;
    event.y = y;
    Touch::FingerUp(event);
}

extern "C" EMSCRIPTEN_KEEPALIVE void TouhouAuxTouchCancelAll()
{
    Touch::CancelTouches();
}
#endif
