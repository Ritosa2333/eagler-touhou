#pragma once

#include "../game/PauseFrame.hpp"

namespace th13::multiplayer {

enum class TerminalHandoff { NotReady, Finished, Failed };

// Called after the receive pump, before sampling another gameplay input.
// present() owns the existing confirmed terminal transition and its draw.
template<class App, class Session>
TerminalHandoff complete_terminal_handoff(App& app, Session& session) {
    if (!session.Simulating() || !app.pause || !session.AllFramesCommitted())
        return TerminalHandoff::NotReady;
    const auto screen = app.pause->state.screen;
    if (screen != PauseScreen::ExtraEnd && screen != PauseScreen::GameOver &&
        screen != PauseScreen::BGameOver)
        return TerminalHandoff::NotReady;
    // Pump may just have retired the final predicted frame. Flush its
    // confirmed audio before present can release the world or apply a pending
    // destination. This is the same ordered drain used by Application::commit.
    if (!app.commit_pending_multiplayer_audio()) {
        app.fail(app.audio_device.error);
        return TerminalHandoff::Failed;
    }
    return app.present() ? TerminalHandoff::Finished : TerminalHandoff::Failed;
}

}  // namespace th13::multiplayer
