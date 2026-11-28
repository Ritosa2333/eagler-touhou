#pragma once

#include "../game/PauseFrame.hpp"

namespace th13::multiplayer {

enum class TerminalHandoff { NotReady, Finished, Failed };

template<class App, class Session>
TerminalHandoff complete_terminal_handoff(App& app, Session& session) {
    if (!session.Simulating() || !app.pause || !session.AllFramesCommitted())
        return TerminalHandoff::NotReady;
    const auto screen = app.pause->state.screen;
    if (screen != PauseScreen::ExtraEnd && screen != PauseScreen::GameOver &&
        screen != PauseScreen::BGameOver)
        return TerminalHandoff::NotReady;
    if (!app.commit_pending_multiplayer_audio()) {
        app.fail(app.audio_device.error);
        return TerminalHandoff::Failed;
    }
    return app.present() ? TerminalHandoff::Finished : TerminalHandoff::Failed;
}

}
