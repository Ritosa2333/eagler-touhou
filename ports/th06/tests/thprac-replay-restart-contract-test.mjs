import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const read = rel => fs.readFileSync(path.join(root, rel), 'utf8').replaceAll('\r\n', '\n');

const practice = read('src/PracticeRuntime.cpp');
const supervisor = read('src/Supervisor.cpp');
const replay = read('src/ReplayManager.cpp');

function requireContract(condition, message) {
    if (!condition) throw new Error(message);
}

const restartHelperStart = practice.indexOf('static void ResetTouchReplayForPracticeRestart()');
const restartHelperEnd = practice.indexOf('\n}', restartHelperStart);
const restartHelper = practice.slice(restartHelperStart, restartHelperEnd);
requireContract(restartHelperStart >= 0, 'thprac restart helper is missing');
requireContract(restartHelper.includes('g_ResetReplayOnReinit = true;'),
    'thprac restart helper does not mark the next REINIT as a fresh Replay attempt');

const reinitAssignments = [...practice.matchAll(/g_Supervisor\.curState = SUPERVISOR_STATE_GAMEMANAGER_REINIT;/g)];
requireContract(reinitAssignments.length === 3,
    `expected exactly three thprac REINIT restart paths, found ${reinitAssignments.length}`);
for (const match of reinitAssignments) {
    const preceding = practice.slice(Math.max(0, match.index - 900), match.index);
    requireContract(preceding.includes('ResetTouchReplayForPracticeRestart();'),
        'a thprac REINIT path can restart without marking a fresh Replay attempt');
}

const reinitStart = supervisor.indexOf('case SUPERVISOR_STATE_GAMEMANAGER_REINIT:');
const reinitEnd = supervisor.indexOf('case SUPERVISOR_STATE_GAMEMANAGER_RESTART:', reinitStart);
const reinit = supervisor.slice(reinitStart, reinitEnd);
requireContract(reinitStart >= 0 && reinitEnd > reinitStart, 'Supervisor REINIT case is missing');
const cutGame = reinit.indexOf('GameManager::CutChain();');
const consume = reinit.indexOf('PracticeRuntime::ConsumeReplayResetOnReinit()');
const discardReplay = reinit.indexOf('ReplayManager::SaveReplay(NULL, NULL);');
const registerGame = reinit.indexOf('GameManager::RegisterChain()');
requireContract(cutGame >= 0 && consume > cutGame && discardReplay > consume && registerGame > discardReplay,
    'fresh-attempt Replay teardown must occur after old GameManager teardown and before re-registration');
requireContract(/if \(PracticeRuntime::ConsumeReplayResetOnReinit\(\)\)\s*\n\s*ReplayManager::SaveReplay\(NULL, NULL\);/.test(reinit),
    'ordinary stage REINIT must not unconditionally discard the multi-stage Replay recording');

const registerStart = replay.indexOf('ZunResult ReplayManager::RegisterChain');
const registerEnd = replay.indexOf('ChainCallbackResult ReplayManager::OnUpdate', registerStart);
const registerChain = replay.slice(registerStart, registerEnd);
requireContract(registerChain.includes('if (g_ReplayManager == NULL)'),
    'ReplayManager no longer distinguishes fresh recorder creation from reuse');
requireContract(registerChain.includes('AddedCallback(g_ReplayManager);'),
    'test premise drifted: reused ordinary ReplayManager no longer takes AddedCallback stage-progression path');

const addedStart = replay.indexOf('ZunResult ReplayManager::AddedCallback(');
const addedEnd = replay.indexOf('ZunResult ReplayManager::AddedCallbackDemo', addedStart);
const added = replay.slice(addedStart, addedEnd);
requireContract(added.includes('mgr->replayData->stageReplayData[g_GameManager.currentStage - 2]'),
    'test premise drifted: ReplayManager reuse no longer assumes previous-stage progression');

console.log('TH06 thprac Replay restart contract: PASS fresh-attempt recorder reset is isolated from stage progression');
