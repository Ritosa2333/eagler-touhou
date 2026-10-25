import fs from "node:fs";
import assert from 'node:assert/strict';
import {workspacePath} from "../lib/workspace-layout.mjs";
import {runNativeCpp} from './support/native-cpp.mjs';

const read = rel => {
  const [owner, ...segments] = rel.split("/");
  return fs.readFileSync(workspacePath(owner.replace(/-eagler$/, ""), ...segments), "utf8").replaceAll("\r\n", "\n");
};
const requireText = (source, needle, label) => {
  if (!source.includes(needle)) throw new Error(`missing ${label}: ${needle}`);
};

async function assertReplayCadence(source,game,{fastCatchup}){
  const declaration=source.match(/\bconst bool preserveReplayCadence\s*=[^;]+;/)?.[0];
  const condition=source.match(/\bif\s*\(([^\n]*\bpreserveReplayCadence\b[^\n]*)\)/)?.[1];
  assert.ok(declaration&&condition,`${game}: native Replay cadence binding is missing`);
  const exercise=declaration=>runNativeCpp(`#include <initializer_list>
struct { int isInReplay=0,replay=0; } g_GameManager;
int main(){
 constexpr double baseTargetDt=1.0/60.0;
 for(bool replay:{false,true})for(bool spectatorViewing:{false,true})for(bool limitPresentationTo60:{false,true})for(double targetDt:{baseTargetDt,baseTargetDt/2}){
  g_GameManager.isInReplay=g_GameManager.replay=replay;
  ${declaration}
  const bool oneTick=(${condition});
  const bool expected=(replay||spectatorViewing||limitPresentationTo60)&&(${fastCatchup?'targetDt==baseTargetDt':'true'});
  if(oneTick!=expected)return 19;
 }
}`);
  await exercise(declaration);
  const broken=declaration.replace(/g_GameManager\.(?:isInReplay|replay)\s*!=\s*0/,'false');
  assert.notEqual(broken,declaration,`${game}: native Replay flag was not identified`);
  await assert.rejects(()=>exercise(broken),error=>error.status===19,`${game}: removing Replay ownership must fail the native policy matrix`);
}

const th06Replay = read("th06-eagler/src/ReplayManager.cpp");
const th06Priorities = read("th06-eagler/src/ChainPriorities.hpp");
const th06Window = read("th06-eagler/src/GameWindow.cpp");

requireText(th06Priorities, "TH_CHAIN_PRIO_CALC_LOW_PRIO_REPLAYMANAGER_DEMO 5", "TH06 replay injection priority 5");
requireText(th06Priorities, "TH_CHAIN_PRIO_CALC_PLAYER 7", "TH06 Player priority 7");
requireText(th06Priorities, "TH_CHAIN_PRIO_CALC_REPLAYMANAGER 15", "TH06 replay recording priority 15");
requireText(th06Replay, "g_Rng.Initialize(replayData->randomSeed)", "TH06 recorded stage RNG seed restore");
requireText(th06Replay, "g_GameManager.rank = replayData->rank", "TH06 rank restore");
requireText(th06Replay, "g_GameManager.livesRemaining = replayData->livesRemaining", "TH06 lives restore");
requireText(th06Replay, "g_GameManager.bombsRemaining = replayData->bombsRemaining", "TH06 bombs restore");
requireText(th06Replay, "g_GameManager.currentPower = replayData->power", "TH06 power restore");
await assertReplayCadence(th06Window,'th06',{fastCatchup:true});

const th07Replay = read("th07-eagler/src/ReplayManager.cpp");
const th07Window = read("th07-eagler/src/GameWindow.cpp");
const th07Game = read("th07-eagler/src/GameManager.cpp");
const th07Supervisor = read("th07-eagler/src/Supervisor.cpp");
const th07SupervisorHeader = read("th07-eagler/src/Supervisor.hpp");

requireText(th07Replay, "g_Chain.AddToCalcChain(mgr->calcChain, 5)", "TH07 replay injection priority 5");
requireText(th07Replay, "g_Chain.AddToCalcChain(mgr->rngCalcChain, 6)", "TH07 replay RNG/event recording priority 6");
requireText(th07Replay, "g_Chain.AddToCalcChain(mgr->calcChain, 16)", "TH07 replay recording priority 16");
requireText(th07Replay, "g_Rng.SetSeed(replayData->stageRngSeed)", "TH07 recorded stage RNG seed restore");
requireText(th07Replay, "*g_GameManager.defaultCfg = arg->data->data.cfg", "TH07 recorded gameplay config restore");
requireText(th07Replay, "g_GameManager.cherry = replayData->cherry", "TH07 cherry restore");
requireText(th07Replay, "g_GameManager.globals->grazeInTotal = replayData->grazeInTotal", "TH07 total graze restore");
requireText(th07Replay, "g_GameManager.globals->nextNeededPointItemsForExtend", "TH07 point-extend threshold restore");
requireText(th07Replay, "g_Supervisor.curFps = (i16) * (arg->fpsCursor + 1) & 0x7f", "TH07 saved replay FPS restore");
requireText(th07Replay, "u8 replayFps = 60;", "TH07 catch-up recording stores logical simulation FPS");
requireText(th07Replay, "if (EaglerOptions::LimitPresentationTo60())", "TH07 measured FPS is used only in original one-tick-per-picture Web mode");
requireText(th07Game, "g_Supervisor.curFps < 20", "TH07 replay lag skip consumes saved replay FPS");
requireText(th07Supervisor, "if (!g_GameManager.replay)", "TH07 playback machine FPS cannot overwrite saved replay FPS");
requireText(th07SupervisorHeader, "CheckIntegrity(const char *version, i32 exeSize, i32 exeChecksum)", "TH07 original replay executable identity API");
requireText(th07Replay, "g_Supervisor.CheckIntegrity(parsed->data.replayStr, parsed->data.exeSize", "TH07 original replay executable identity validation");
requireText(th07Replay, "if (parsed->data.cfg.slowMode)", "TH07 slow-mode replay rejection");
requireText(th07Replay, "memcmp(&g_Supervisor.cfg, &mgr->data->data.cfg", "TH07 mid-run config save guard");
await assertReplayCadence(th07Window,'th07',{fastCatchup:false});

const updateStart = th07Game.indexOf("u32 GameManager::OnUpdate(GameManager *arg)");
const drawStart = th07Game.indexOf("u32 GameManager::OnDraw(GameManager *arg)");
const drawEnd = th07Game.indexOf("void GameManager::DrawLoadingSprite", drawStart);
if (updateStart < 0 || drawStart < 0 || drawEnd < 0) throw new Error("TH07 GameManager update/draw bounds missing");
const update = th07Game.slice(updateStart, drawStart);
const draw = th07Game.slice(drawStart, drawEnd);
requireText(update, "if (arg->isInPauseMenu == 1)", "TH07 pause transition is simulation-owned");
if (draw.includes("isInPauseMenu = 2")) throw new Error("TH07 presentation must not advance pause simulation state");

console.log("th06/th07 replay determinism contract: PASS");
