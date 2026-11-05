#pragma once
#include "Application.hpp"
#include "../multiplayer/BattleSnapshot.hpp"
namespace th075::sdl {
struct RollbackState {
 BattleSnapshot battle;BackgroundStage background;BattleHud hud;BattleOverlay overlay;
 ReplayStreams loadingReplay;BattleSession pendingSession;RoundRules pendingRules;
 u32 frame,now;bool recordingFinished;
 explicit RollbackState(const Application& app):battle(*app.battle,app.selection.character),background(app.battleRenderer->background),hud(app.battleRenderer->hud),overlay(app.battleRenderer->overlay),loadingReplay(app.loadingReplay),pendingSession(app.pendingSession),pendingRules(app.pendingRules),frame(app.frame),now(app.now),recordingFinished(app.recordingFinished){}
 void restore(Application& app)const{battle.restore(*app.battle);app.battleRenderer->background=background;app.battleRenderer->hud=hud;app.battleRenderer->overlay=overlay;app.loadingReplay=loadingReplay;app.pendingSession=pendingSession;app.pendingRules=pendingRules;app.frame=frame;app.now=now;app.recordingFinished=recordingFinished;}
};
}
