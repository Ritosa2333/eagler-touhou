#pragma once
#include <SDL3/SDL_events.h>
#include <cstdint>
#include <eagler/overlay/ConnectionStatus.hpp>
#if TH09_RMLUI_OVERLAY
#include <string>
#endif
namespace Th09Overlay {
bool Open(SDL_Window* window);
void Close();
bool HandleEvent(const SDL_Event& event);
bool CapturesKeyboard();
bool RoomMusicAudible();
bool NeedsPresentation();
void Composite(int x, int y, int width, int height);
bool ShowLobby();
enum class RoomPhase { idle, connecting, synchronizing, selection, playing, loadingSpectator, spectating, results };
void SetRoomPhase(RoomPhase phase, unsigned completed = 0, unsigned total = 0);
void SetTransportRecovering(bool recovering);
void SetConnectionStatus(ConnectionStatus status);
void SetEscapeAvailable(bool available);
bool TakeEscapeRequest();
void FinishRoom(const char* error, bool showRoom = true);
void AdoptRoom(const char* options);
void ReportMatch(std::uint32_t start, std::uint32_t frames, unsigned winner, unsigned first, unsigned second);
bool TakeReturnRequest();
#if TH09_RMLUI_OVERLAY
std::string TakeRoomStart();
const char* InitialRouteChoice();
void SetRouteSwitch(const char* from, int fromMs, const char* to, int toMs, bool switching);
bool TakeRouteSwitchRequest();
#endif
}
