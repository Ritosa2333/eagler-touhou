#include "Touch.hpp"

#include <SDL3/SDL_events.h>
#include <algorithm>
#include <cmath>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "Controller.hpp"
#include "EaglerOptions.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"
#include "ReplayExtension.hpp"

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

bool g_DialogueTapPending = false;
f32 g_DialogueTapStartX = 0.0f;
f32 g_DialogueTapStartY = 0.0f;

f32 g_AccumDx = 0.0f;
f32 g_AccumDy = 0.0f;

bool g_UsedThisRun = false;
bool g_UsedCheatMovementThisRun = false;

// hopefully you only have 10 fingers
SDL_FingerID g_ActiveGameplayFingerIds[10];
i32 g_NumActiveGameplayFingers = 0;

bool g_BombPending = false;
bool g_PausePending = false;
bool g_BombedWithTouch = false;
i32 g_LastHostBombSerial = 0;
i32 g_LastHostEscapeSerial = 0;

struct DoubleTapBombTracker
{
    bool tracking;
    SDL_FingerID fingerId;
    f32 startX;
    f32 startY;
    f32 lastX;
    f32 lastY;
    u64 startTicks;
    bool movedTooFar;
    bool armed;
    f32 tapX;
    f32 tapY;
    u64 tapTicks;
};

DoubleTapBombTracker g_DoubleTapBomb;

struct ReplayRecordFingerSlot
{
    bool active;
    SDL_FingerID id;
    i32 replayId;
};

struct ReplayPlaybackFingerSlot
{
    bool active;
    bool releasedThisFrame;
    i32 id;
    f32 x;
    f32 y;
    u32 role;
    u32 flags;
};

ReplayRecordFingerSlot g_ReplayRecordFingers[10];
i32 g_NextReplayFingerId = 1;
ReplayPlaybackFingerSlot g_ReplayPlaybackFingers[10];

bool IsFinger(const FingerSlot &slot, SDL_FingerID id);
void GetWindowSize(i32 *w, i32 *h);
void GetContainedRenderRect(f32 *outX, f32 *outY, f32 *outW, f32 *outH, f32 *outScale);

static constexpr u64 DOUBLE_TAP_MAX_TAP_MS = 220;
static constexpr u64 DOUBLE_TAP_MAX_GAP_MS = 320;

void ResetReplayRecordFingerIds()
{
    for (ReplayRecordFingerSlot &slot : g_ReplayRecordFingers)
        slot = {};
    g_NextReplayFingerId = 1;
}

bool IsDialogueTouchOwner()
{
    return g_Gui.HasCurrentMsgIdx() &&
           (g_Gui.IsDialogueSkippable() || g_Gui.IsWaitingForPlayerAdvance());
}

i32 ReplayRecordFingerId(SDL_FingerID id, bool create)
{
    for (ReplayRecordFingerSlot &slot : g_ReplayRecordFingers)
    {
        if (slot.active && slot.id == id)
            return slot.replayId;
    }
    if (!create)
        return 0;
    for (ReplayRecordFingerSlot &slot : g_ReplayRecordFingers)
    {
        if (!slot.active)
        {
            slot.active = true;
            slot.id = id;
            slot.replayId = g_NextReplayFingerId++;
            return slot.replayId;
        }
    }
    return 0;
}

void ReleaseReplayRecordFingerId(SDL_FingerID id)
{
    for (ReplayRecordFingerSlot &slot : g_ReplayRecordFingers)
    {
        if (slot.active && slot.id == id)
        {
            slot = {};
            return;
        }
    }
}

u32 ReplayTouchRole(SDL_FingerID id)
{
    if (IsFinger(g_MoveFinger, id))
        return ReplayExtension::TOUCH_ROLE_MOVE;
    if (IsFinger(g_FocusFinger, id))
        return ReplayExtension::TOUCH_ROLE_FOCUS;
    if (IsFinger(g_DialogueHoldFinger, id))
        return ReplayExtension::TOUCH_ROLE_DIALOGUE;
    return ReplayExtension::TOUCH_ROLE_NONE;
}

void CaptureReplayTouchEvent(const SDL_TouchFingerEvent &event, u32 action)
{
    const bool create = action == ReplayExtension::TOUCH_ACTION_DOWN;
    const i32 replayId = ReplayRecordFingerId(event.fingerID, create);
    if (replayId == 0)
        return;
    const u32 flags = EaglerOptions::UnlimitedTouch() ? ReplayExtension::TOUCH_FLAG_UNLIMITED : 0;
    ReplayExtension::CaptureTouchEvent(replayId, event.x, event.y, action,
                                       ReplayTouchRole(event.fingerID), flags);
    if (action == ReplayExtension::TOUCH_ACTION_UP)
        ReleaseReplayRecordFingerId(event.fingerID);
}

bool HasReplayRecordFinger()
{
    for (const ReplayRecordFingerSlot &slot : g_ReplayRecordFingers)
        if (slot.active)
            return true;
    return false;
}

ReplayPlaybackFingerSlot *FindReplayPlaybackFinger(i32 id)
{
    for (ReplayPlaybackFingerSlot &slot : g_ReplayPlaybackFingers)
        if ((slot.active || slot.releasedThisFrame) && slot.id == id)
            return &slot;
    return nullptr;
}

ReplayPlaybackFingerSlot *AcquireReplayPlaybackFinger(i32 id)
{
    if (ReplayPlaybackFingerSlot *slot = FindReplayPlaybackFinger(id))
        return slot;
    for (ReplayPlaybackFingerSlot &slot : g_ReplayPlaybackFingers)
    {
        if (!slot.active && !slot.releasedThisFrame)
        {
            slot = {};
            slot.id = id;
            return &slot;
        }
    }
    return nullptr;
}

void MarkNonReplayableTouchUse()
{
    // Direct coordinate dragging cannot be represented in the original replay
    // input stream. Both joystick modes remain replay-owned: the ordinary
    // wheel becomes TH_BUTTON_* bits, while the free-direction wheel has its
    // analog vector recorded by ReplayExtension.
    if (!EaglerOptions::TouchMovementUsesJoystick())
        g_UsedThisRun = true;
}

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

f32 GetDoubleTapRadius()
{
    i32 winW, winH;
    GetWindowSize(&winW, &winH);
    const i32 shortest = winW < winH ? winW : winH;
    return (f32)shortest * 0.08f;
}

bool IsWithinDistance(f32 x1, f32 y1, f32 x2, f32 y2, f32 radius)
{
    const f32 dx = x1 - x2;
    const f32 dy = y1 - y2;
    return dx * dx + dy * dy <= radius * radius;
}

void ResetDoubleTapBomb()
{
    g_DoubleTapBomb = {};
}

void StartDoubleTapCandidate(SDL_FingerID id, f32 px, f32 py)
{
    if (!EaglerOptions::DoubleTapBombEnabled())
        return;
    g_DoubleTapBomb.tracking = true;
    g_DoubleTapBomb.fingerId = id;
    g_DoubleTapBomb.startX = px;
    g_DoubleTapBomb.startY = py;
    g_DoubleTapBomb.lastX = px;
    g_DoubleTapBomb.lastY = py;
    g_DoubleTapBomb.startTicks = SDL_GetTicks();
    g_DoubleTapBomb.movedTooFar = false;
}

void UpdateDoubleTapCandidate(SDL_FingerID id, f32 px, f32 py)
{
    if (!EaglerOptions::DoubleTapBombEnabled())
        return;
    if (!g_DoubleTapBomb.tracking || g_DoubleTapBomb.fingerId != id)
    {
        return;
    }

    g_DoubleTapBomb.lastX = px;
    g_DoubleTapBomb.lastY = py;
    if (!IsWithinDistance(g_DoubleTapBomb.startX, g_DoubleTapBomb.startY, px, py, GetSwipeThreshold()))
    {
        g_DoubleTapBomb.movedTooFar = true;
    }
}

void CompleteDoubleTapCandidate(SDL_FingerID id, f32 px, f32 py)
{
    if (!EaglerOptions::DoubleTapBombEnabled())
    {
        ResetDoubleTapBomb();
        return;
    }
    if (!g_DoubleTapBomb.tracking || g_DoubleTapBomb.fingerId != id)
    {
        return;
    }

    const u64 now = SDL_GetTicks();
    const bool isTap = IsGameplayTouchMode() && !g_Gui.HasCurrentMsgIdx() &&
                       now - g_DoubleTapBomb.startTicks <= DOUBLE_TAP_MAX_TAP_MS &&
                       !g_DoubleTapBomb.movedTooFar &&
                       IsWithinDistance(g_DoubleTapBomb.startX, g_DoubleTapBomb.startY, px, py, GetSwipeThreshold());
    g_DoubleTapBomb.tracking = false;
    if (!isTap)
    {
        g_DoubleTapBomb.armed = false;
        return;
    }

    g_DoubleTapBomb.armed = true;
    g_DoubleTapBomb.tapX = px;
    g_DoubleTapBomb.tapY = py;
    g_DoubleTapBomb.tapTicks = now;
}

bool TryDoubleTapBomb(f32 px, f32 py)
{
    if (!EaglerOptions::DoubleTapBombEnabled())
    {
        ResetDoubleTapBomb();
        return false;
    }
    if (!g_DoubleTapBomb.armed)
    {
        return false;
    }

    const u64 now = SDL_GetTicks();
    if (now - g_DoubleTapBomb.tapTicks > DOUBLE_TAP_MAX_GAP_MS ||
        !IsWithinDistance(g_DoubleTapBomb.tapX, g_DoubleTapBomb.tapY, px, py, GetDoubleTapRadius()))
    {
        g_DoubleTapBomb.armed = false;
        return false;
    }

    g_DoubleTapBomb.armed = false;
    g_DoubleTapBomb.tracking = false;
    g_BombPending = true;
    MarkNonReplayableTouchUse();
    return true;
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
    g_UsedCheatMovementThisRun = false;
    g_BombedWithTouch = false;
}

bool Touch::WasUsedThisRun()
{
    return g_UsedThisRun;
}

bool Touch::UsedCheatMovementThisRun()
{
    return g_UsedCheatMovementThisRun;
}

bool Touch::UsedTouchToBomb()
{
    return g_BombedWithTouch;
}

void Touch::SetReplayUsageState(bool usedThisRun, bool bombedWithTouch, bool cheatMovementUsed)
{
    g_UsedThisRun = usedThisRun;
    g_BombedWithTouch = bombedWithTouch;
    g_UsedCheatMovementThisRun = cheatMovementUsed;
}

void Touch::CancelTouches()
{
    if (HasReplayRecordFinger())
    {
        ReplayExtension::CaptureTouchCancelAll();
        ResetReplayRecordFingerIds();
    }

    ReleaseFinger(&g_MoveFinger);
    ReleaseFinger(&g_FocusFinger);
    ReleaseFinger(&g_DialogueHoldFinger);

    g_AccumDx = 0.0f;
    g_AccumDy = 0.0f;

    ClearGameplayFingers();
    g_PausePending = false;
    g_DialogueTapPending = false;
    g_BombPending = false;
    g_BombedWithTouch = false;
    g_MenuGesture = {};
    ResetDoubleTapBomb();
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
        ResetDoubleTapBomb();
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
            ResetDoubleTapBomb();
            CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_DOWN);
            return;
        }

        if (EaglerOptions::TouchBombZoneEnabled() && IsBombZone(px, py))
        {
            ResetDoubleTapBomb();
            g_BombPending = true;
            MarkNonReplayableTouchUse();
            CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_DOWN);
            return;
        }

        if (IsDialogueTouchOwner())
        {
            ResetDoubleTapBomb();
        }
        else if (TryDoubleTapBomb(px, py))
        {
            CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_DOWN);
            return;
        }
        else
        {
            // Double-tap Bomb is an independent gesture owner. Any gameplay
            // finger may become the tap candidate, including a second finger
            // while the primary move finger remains held and moving.
            StartDoubleTapCandidate(f.fingerID, px, py);
        }

        if (IsDialogueTouchOwner() && !g_DialogueHoldFinger.active)
        {
            AssignFinger(&g_DialogueHoldFinger, f.fingerID, px, py);
            g_DialogueTapStartX = px;
            g_DialogueTapStartY = py;
            MarkNonReplayableTouchUse();
            CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_DOWN);
            return;
        }

        // In joystick mode gameplay screen touches are never allowed to become
        // direct player movement/focus owners. The host wheel is the sole
        // movement owner; optional double-tap Bomb above remains independent.
        if (EaglerOptions::TouchMovementUsesJoystick())
        {
            CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_DOWN);
            return;
        }

        AddGameplayFinger(f.fingerID);
        if (g_NumActiveGameplayFingers >= 4)
        {
            g_PausePending = true;
        }

        if (!g_MoveFinger.active)
        {
            if (g_FocusFinger.active && f.fingerID == g_FocusFinger.id)
            {
                CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_DOWN);
                return;
            }

            AssignFinger(&g_MoveFinger, f.fingerID, px, py);
            MarkNonReplayableTouchUse();
            CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_DOWN);
            return;
        }

        if (EaglerOptions::TouchFocusUsesTwoFingers() && !g_FocusFinger.active && f.fingerID != g_MoveFinger.id)
        {
            AssignFinger(&g_FocusFinger, f.fingerID, px, py);
            MarkNonReplayableTouchUse();
            CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_DOWN);
            return;
        }
        CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_DOWN);
    }
}

void Touch::FingerUp(const SDL_TouchFingerEvent &f)
{
    if (!EaglerOptions::TouchEnabled())
    {
        // Never leave a latched gameplay role behind merely because touch was
        // disabled between DOWN and UP.
        CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_UP);
        ResetDoubleTapBomb();
        ReleaseGameplayFingerState(f.fingerID);
        return;
    }
    f32 px, py;
    FingerToWindowPx(f, &px, &py);

    // Dialogue touch has two intentionally separate gestures:
    //   short stationary tap -> one Z edge (advance one dialogue pause)
    //   hold >= 500 ms       -> SKIP while held
    // Check the dialogue owner before generic cleanup releases this finger.
    if (IsFinger(g_DialogueHoldFinger, f.fingerID) && IsDialogueTouchOwner())
    {
        const u64 held = SDL_GetTicks() - g_DialogueHoldFinger.start;
        const f32 dx = px - g_DialogueTapStartX;
        const f32 dy = py - g_DialogueTapStartY;
        const f32 threshold = GetSwipeThreshold();
        if (held < 500 && std::abs(dx) <= threshold && std::abs(dy) <= threshold)
        {
            g_DialogueTapPending = true;
        }
    }

    // Cleanup must be keyed by the finger that ended, not by the game mode at
    // the instant the UP arrives. A state transition between DOWN and UP used
    // to route the event into the menu branch and leave move/focus latched.
    CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_UP);
    CompleteDoubleTapCandidate(f.fingerID, px, py);
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
        UpdateDoubleTapCandidate(f.fingerID, px, py);

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
                const f32 sensitivity = EaglerOptions::TouchSensitivity();
                g_AccumDx += dxPx / scale * sensitivity;
                g_AccumDy += dyPx / scale * sensitivity;
            }
            g_MoveFinger.lastPxX = px;
            g_MoveFinger.lastPxY = py;
        }

        if (IsFinger(g_FocusFinger, f.fingerID))
        {
            g_FocusFinger.lastPxX = px;
            g_FocusFinger.lastPxY = py;
        }

        CaptureReplayTouchEvent(f, ReplayExtension::TOUCH_ACTION_MOTION);
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

    if (g_DialogueTapPending)
    {
        if (g_Gui.HasCurrentMsgIdx())
        {
            buttons |= TH_BUTTON_SHOOT;
        }
        g_DialogueTapPending = false;
    }

    const i32 hostBombSerial = EaglerOptions::TouchBombSerial();
    if (hostBombSerial != g_LastHostBombSerial)
    {
        g_LastHostBombSerial = hostBombSerial;
        if (IsGameplayTouchMode())
        {
            g_BombPending = true;
            MarkNonReplayableTouchUse();
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

    if (!IsDialogueTouchOwner() && g_DialogueHoldFinger.active)
    {
        if (!g_MoveFinger.active)
        {
            AssignFinger(&g_MoveFinger, g_DialogueHoldFinger.id, g_DialogueHoldFinger.lastPxX,
                         g_DialogueHoldFinger.lastPxY);
        }
        ReleaseFinger(&g_DialogueHoldFinger);
    }
    else if (g_DialogueHoldFinger.active && g_Gui.IsDialogueSkippable())
    {
        u64 held = SDL_GetTicks() - g_DialogueHoldFinger.start;

        if (held >= 500)
        {
            buttons |= TH_BUTTON_SKIP;
        }
    }

    // keep firing for a bit after release
    if (EaglerOptions::TouchFireEnabled() && IsGameplayTouchMode() && !g_Gui.HasCurrentMsgIdx())
    {
        buttons |= TH_BUTTON_SHOOT;
        MarkNonReplayableTouchUse();
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

bool Touch::GetFreeJoystickVector(f32 *x, f32 *y)
{
    if (!EaglerOptions::TouchEnabled() || !EaglerOptions::TouchMovementIsFreeJoystick() ||
        !IsGameplayTouchMode())
    {
        *x = 0.0f;
        *y = 0.0f;
        return false;
    }

    constexpr f32 AXIS_MAX = 32767.0f;
    *x = std::clamp((f32)EaglerOptions::TouchJoystickX() / AXIS_MAX, -1.0f, 1.0f);
    *y = std::clamp((f32)EaglerOptions::TouchJoystickY() / AXIS_MAX, -1.0f, 1.0f);
    if (*x == 0.0f && *y == 0.0f)
    {
        return false;
    }

    MarkNonReplayableTouchUse();
    return true;
}

bool Touch::GetPlayerDelta(f32 *dx, f32 *dy)
{
    if (!EaglerOptions::TouchEnabled() || EaglerOptions::TouchMovementUsesJoystick() || !g_MoveFinger.active)
    {
        *dx = 0.0f;
        *dy = 0.0f;
        return false;
    }

    *dx = g_AccumDx;
    *dy = g_AccumDy;

    if (EaglerOptions::UnlimitedTouch() && (*dx != 0.0f || *dy != 0.0f))
    {
        g_UsedCheatMovementThisRun = true;
    }

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

void Touch::BeginReplayTouchFrame()
{
    for (ReplayPlaybackFingerSlot &slot : g_ReplayPlaybackFingers)
    {
        if (slot.releasedThisFrame)
            slot = {};
    }
}

void Touch::ApplyReplayTouchEvent(i32 fingerId, f32 x, f32 y, u32 action, u32 role, u32 flags)
{
    if (action == ReplayExtension::TOUCH_ACTION_CANCEL_ALL)
    {
        for (ReplayPlaybackFingerSlot &slot : g_ReplayPlaybackFingers)
        {
            if (slot.active)
            {
                slot.active = false;
                slot.releasedThisFrame = true;
            }
        }
        return;
    }

    ReplayPlaybackFingerSlot *slot = AcquireReplayPlaybackFinger(fingerId);
    if (!slot)
        return;

    if (action == ReplayExtension::TOUCH_ACTION_DOWN)
    {
        slot->active = true;
        slot->releasedThisFrame = false;
        slot->x = x;
        slot->y = y;
        slot->role = role;
        slot->flags = flags;
        return;
    }

    if (action == ReplayExtension::TOUCH_ACTION_MOTION)
    {
        slot->active = true;
        slot->releasedThisFrame = false;
        slot->x = x;
        slot->y = y;
        slot->role = role;
        slot->flags = flags;
        return;
    }

    if (action == ReplayExtension::TOUCH_ACTION_UP)
    {
        slot->x = x;
        slot->y = y;
        slot->role = role;
        slot->flags = flags;
        slot->active = false;
        slot->releasedThisFrame = true;
    }
}

i32 Touch::GetReplayTouchPoints(ReplayTouchPoint *points, i32 capacity)
{
    if (!points || capacity <= 0)
        return 0;
    i32 count = 0;
    for (const ReplayPlaybackFingerSlot &slot : g_ReplayPlaybackFingers)
    {
        if ((!slot.active && !slot.releasedThisFrame) || count >= capacity)
            continue;
        points[count].x = slot.x * 640.0f;
        points[count].y = slot.y * 480.0f;
        count++;
    }
    return count;
}

void Touch::ResetReplayTouch()
{
    for (ReplayPlaybackFingerSlot &slot : g_ReplayPlaybackFingers)
        slot = {};
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
