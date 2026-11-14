import {TH19_VERSUS_RULES,validTh19VersusRules} from '../../../common/contracts/th19-versus-rules.mjs';
import {Th19SpectatorHistory} from './th19-spectator.mjs';
import {createNicknameService} from './nickname-service.mjs';
import {createNicknameRequestGuard} from './nickname-request-guard.mjs';
import {normalizePublicNickname} from '../../../common/contracts/public-nickname.mjs';
import {TH05_INITIAL_STOCK,validTh05InitialSpares,validTh05InitialBombs} from '../../../common/contracts/th05-initial-stock.mjs';
import {
  validTh15ChapterThreshold,
  defaultTh15ChapterThreshold,
} from "../../../common/contracts/th15-chapter-reward.mjs";
import { quickChatPhrase } from '../../../common/contracts/multiplayer-quick-chat.mjs';
import { createHmac } from 'node:crypto';
import { createTurnConfigReader } from './turn-config.mjs';
import { WebSocket, WebSocketServer } from 'ws';
import { roomProbeEnvelope } from './room-probe-policy.mjs';
import { createRoomDirectory, publicControlMode } from './room-directory.mjs';
import { createRoomBroadcaster } from './room-broadcast.mjs';
import { startRoomStatus } from './public-status.mjs';
import {
  createRelayAbuseGuard,
  relayAbuseConfig,
  relayClientAddress,
} from './relay-abuse-guard.mjs';
import {
  RELAY_LIMITS,
  TH09_SPECTATOR_HISTORY_LIMITS,
  closeTh09HistoryAdmission,
  createRelayMessageGate,
  acceptRelayMessage,
  createBoundedRelaySender,
  relayGameplayPayload,
  normalizeRelaySignal,
  clearSpectatorHistory,
  stopSpectatorStream,
  hasPendingSpectators,
  appendSpectatorHistory,
  validRelayControlMessage,
} from './relay-flow-control.mjs';

import {
  multiplayerConfigForProduct,
  multiplayerInputTimingPolicy,
} from '../../../common/contracts/product-catalog.mjs';
import { isSpectatorFrameForRoom } from './spectator-frame.mjs';
import { COOPERATIVE_SPECTATOR_MARKERS, SpectatorStageRelay } from './spectator-stages.mjs';
import {SpectatorReplayTransfer} from './spectator-replay.mjs';
import { th09PlayerStats, acceptTh09MatchStats, bindTh09Duel, reconcileTh09Duel } from './th09-room-stats.mjs';
import { acceptTh19MatchStats } from './th19-room-stats.mjs';
import {
  parseMeasuredNetplayTiming,
  resolveAdonisPredictionReserve,
} from '../../../common/contracts/netplay-timing.mjs';

const nicknameService = await createNicknameService();
const nicknameProfiles = new Map(), nicknameRequests = new Map();
const allowNicknameRequest = createNicknameRequestGuard();
function nicknameProfile(member) {
  return nicknameProfiles.get(member) || {name:'',status:'idle',nextChangeAt:0,retryAt:0,attemptsLeft:null};
}
function publishNickname(member, profile) {
  if (nicknameProfiles.size >= 10000 && !nicknameProfiles.has(member)) nicknameProfiles.delete(nicknameProfiles.keys().next().value);
  nicknameProfiles.set(member, profile);
  const entry = roomDirectory.memberLocation(member), room = rooms.get(entry?.roomId);
  if (!room) return;
  const {clientId:client, socket} = entry;
  let changed = false;
  const seat = lobbySeatOf(room, client);
  if (seat >= 0 && room.lobby.seats[seat].name !== profile.name) {room.lobby.seats[seat].name=profile.name;changed=true;}
  if (room.lobby.spectators.has(client) && room.lobby.spectators.get(client) !== profile.name) {room.lobby.spectators.set(client,profile.name);changed=true;}
  sendLobby(socket,{type:'nickname-status',...profile});
  if (changed) broadcastLobby(room);
}
nicknameService?.onUpdate(publishNickname);
function reviewNickname(socket, member, proposed) {
  const old=nicknameProfile(member);
  if (old.status === 'queued' || (old.name && normalizePublicNickname(proposed) === old.name)) {
    sendLobby(socket,{type:'nickname-status',...old});return;
  }
  const retryAt = allowNicknameRequest(socket.relayAddress);
  if (retryAt) {sendLobby(socket,{type:'nickname-status',...old,status:'rate-limited',retryAt});return;}
  if (nicknameRequests.has(member)) {sendLobby(socket,{type:'nickname-status',...old,status:'reviewing'});return;}
  sendLobby(socket,{type:'nickname-status',...old,status:'reviewing'});
  const task=nicknameService.submit(member,socket.relayAddress,proposed)
    .then(profile=>publishNickname(member,profile))
    .catch(()=>publishNickname(member,{...old,status:'unavailable',retryAt:Date.now()+10000}))
    .finally(()=>nicknameRequests.delete(member));
  nicknameRequests.set(member,task);
}
function seatNickname(socket, member, proposed) {
  if (!nicknameService) return normalizeDisplayName(proposed);
  // A role message may still contain the legacy initial. Only set-name reviews
  // a complete candidate, after the client receives nickname-status.
  return nicknameProfile(member).name;
}

function multiplayerPolicyForRoomId(roomId) {
  const separator = roomId.indexOf('-');
  if (separator <= 0) return null;
  return multiplayerConfigForProduct(roomId.slice(0, separator));
}

const LEGACY_MULTIPLAYER_PLAYER_COUNTS = Object.freeze([2, 3]);
function multiplayerPlayerCounts(policy) {
  return Array.isArray(policy?.playerCounts) && policy.playerCounts.length
    ? policy.playerCounts
    : LEGACY_MULTIPLAYER_PLAYER_COUNTS;
}
function validPlayerCount(policy, value) {
  return Number.isInteger(value) && multiplayerPlayerCounts(policy).includes(value);
}
function defaultPlayerCount(policy) {
  return multiplayerPlayerCounts(policy)[0] ?? 2;
}

function envValue(primary, legacyNames, fallback = '') {
  const names = [primary, ...legacyNames];
  const configured = names
    .filter((name) => process.env[name] != null)
    .map((name) => [name, String(process.env[name])]);
  if (configured.length > 1 && new Set(configured.map(([, value]) => value)).size > 1) {
    throw new Error(
      `conflicting environment variables: ${configured.map(([name]) => name).join(', ')}`,
    );
  }
  return configured[0]?.[1] ?? fallback;
}

// EAGLER_NETPLAY_* is the canonical service configuration. TH07_* names are
// read-only compatibility aliases for deployments created before Multiplayer
// became a shared product profile.
const host = envValue('EAGLER_NETPLAY_RELAY_HOST', ['TH07_RELAY_HOST'], '0.0.0.0');
const port = Number.parseInt(
  envValue('EAGLER_NETPLAY_RELAY_PORT', ['TH07_RELAY_PORT'], '18142'),
  10,
);
const delayMs = Math.max(
  0,
  Number.parseInt(envValue('EAGLER_NETPLAY_RELAY_DELAY_MS', ['TH07_RELAY_DELAY_MS'], '0'), 10) || 0,
);
const jitterMs = Math.max(
  0,
  Number.parseInt(envValue('EAGLER_NETPLAY_RELAY_JITTER_MS', ['TH07_RELAY_JITTER_MS'], '0'), 10) ||
    0,
);
const dropEvery = Math.max(
  0,
  Number.parseInt(
    envValue('EAGLER_NETPLAY_RELAY_DROP_EVERY', ['TH07_RELAY_DROP_EVERY'], '0'),
    10,
  ) || 0,
);
const dropFirstInputPerEdge =
  envValue('EAGLER_NETPLAY_RELAY_DROP_FIRST_INPUT_PER_EDGE', [
    'TH07_RELAY_DROP_FIRST_INPUT_PER_EDGE',
  ]) === '1';
const dropInputLatestFrom = Math.max(
  -1,
  Number.parseInt(
    envValue(
      'EAGLER_NETPLAY_RELAY_DROP_INPUT_LATEST_FROM',
      ['TH07_RELAY_DROP_INPUT_LATEST_FROM'],
      '-1',
    ),
    10,
  ) || -1,
);
const dropInputLatestTo = Math.max(
  -1,
  Number.parseInt(
    envValue(
      'EAGLER_NETPLAY_RELAY_DROP_INPUT_LATEST_TO',
      ['TH07_RELAY_DROP_INPUT_LATEST_TO'],
      '-1',
    ),
    10,
  ) || -1,
);
const routeSkewPlayer = Number.parseInt(
  envValue('EAGLER_NETPLAY_TEST_ROUTE_SKEW_PLAYER', ['TH07_TEST_ROUTE_SKEW_PLAYER'], '-1'),
  10,
);
const routeSkewMs = Math.max(
  0,
  Number.parseInt(
    envValue('EAGLER_NETPLAY_TEST_ROUTE_SKEW_MS', ['TH07_TEST_ROUTE_SKEW_MS'], '0'),
    10,
  ) || 0,
);
// Lobby state snapshots are idempotent full-room views, so bursts of changes
// coalesce into one broadcast per tick. Without this, K connections each
// mutating state at a legal rate multiply their fan-out by every member.
const lobbyStateCoalesceMs = Math.max(
  0,
  Number.parseInt(
    envValue('EAGLER_NETPLAY_LOBBY_STATE_COALESCE_MS', ['TH07_LOBBY_STATE_COALESCE_MS'], '100'),
    10,
  ) || 100,
);
const rtcTimeoutMs = Math.max(
  1000,
  Number.parseInt(
    envValue(
      'EAGLER_NETPLAY_RTC_TIMEOUT_MS',
      ['TH07_RTC_TIMEOUT_MS', 'TH07_DIRECT_TIMEOUT_MS'],
      '4500',
    ),
    10,
  ) || 4500,
);
const allowGameplayRelayFallback = process.env.EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK === '1';
const stunUrls = envValue(
  'EAGLER_NETPLAY_STUN_URLS',
  ['TH07_STUN_URLS'],
  'stun:stun.cloudflare.com:3478',
)
  .split(',')
  .map((value) => value.trim())
  .filter(Boolean);
const turnUrls = envValue('EAGLER_NETPLAY_TURN_URLS', ['TH07_TURN_URLS'])
  .split(',')
  .map((value) => value.trim())
  .filter(Boolean);
const turnSharedSecret = envValue('EAGLER_NETPLAY_TURN_SHARED_SECRET', ['TH07_TURN_SHARED_SECRET']);
const turnUsername = envValue('EAGLER_NETPLAY_TURN_USERNAME', ['TH07_TURN_USERNAME']);
const turnCredential = envValue('EAGLER_NETPLAY_TURN_CREDENTIAL', ['TH07_TURN_CREDENTIAL']);
const readTurnConfig = createTurnConfigReader(envValue('EAGLER_NETPLAY_TURN_CONFIG_FILE', []), {
  urls: turnUrls, sharedSecret: turnSharedSecret,
});
const turnTtlSeconds = Math.max(
  60,
  Number.parseInt(
    envValue('EAGLER_NETPLAY_TURN_TTL_SECONDS', ['TH07_TURN_TTL_SECONDS'], '3600'),
    10,
  ) || 3600,
);
const lobbyReconnectGraceMs = Math.max(
  100,
  Number.parseInt(
    envValue('EAGLER_NETPLAY_LOBBY_RECONNECT_GRACE_MS', ['TH07_LOBBY_RECONNECT_GRACE_MS'], '12000'),
    10,
  ) || 12000,
);
const spectatorConnectGraceMs = Math.max(
  100,
  Number.parseInt(
    envValue(
      'EAGLER_NETPLAY_SPECTATOR_CONNECT_GRACE_MS',
      ['TH07_SPECTATOR_CONNECT_GRACE_MS'],
      '60000',
    ),
    10,
  ) || 60000,
);
const spectatorMaxBufferedBytes = Math.max(
  64 * 1024,
  Number.parseInt(
    envValue(
      'EAGLER_NETPLAY_SPECTATOR_MAX_BUFFERED_BYTES',
      ['TH07_SPECTATOR_MAX_BUFFERED_BYTES'],
      String(1024 * 1024),
    ),
    10,
  ) || 1024 * 1024,
);

for (const url of stunUrls) {
  if (!/^stuns?:/i.test(url)) throw new Error(`invalid EAGLER_NETPLAY_STUN_URLS entry: ${url}`);
}
for (const url of turnUrls) {
  if (!/^turns?:/i.test(url)) throw new Error(`invalid EAGLER_NETPLAY_TURN_URLS entry: ${url}`);
}
if (turnUrls.length && !turnSharedSecret && !(turnUsername && turnCredential)) {
  throw new Error(
    'EAGLER_NETPLAY_TURN_URLS requires EAGLER_NETPLAY_TURN_SHARED_SECRET or both static TURN credentials',
  );
}
if ((turnUsername && !turnCredential) || (!turnUsername && turnCredential)) {
  throw new Error(
    'EAGLER_NETPLAY_TURN_USERNAME and EAGLER_NETPLAY_TURN_CREDENTIAL must be configured together',
  );
}
const rooms = new Map();
// User-facing numbers are global. Runtime and signaling URLs still carry the product.
const roomCodes = new Map();
function numericRoomCode(id) {
  const code = id.slice(id.indexOf('-') + 1);
  return /^[0-9]{4,8}$/.test(code) && multiplayerPolicyForRoomId(id) ? code : '';
}
const roomStatusServer = process.env.EAGLER_NETPLAY_STATUS_PORT
  ? startRoomStatus(rooms, Number(process.env.EAGLER_NETPLAY_STATUS_PORT)) : null;
roomStatusServer?.on('error', error => {
  console.error('Room status failed:', error.code);
  process.exit(1);
});
const abuseConfig = relayAbuseConfig();
const abuseGuard = createRelayAbuseGuard(abuseConfig);
const targetedEnvelopeMarker = 0xe7;
const sendBounded = createBoundedRelaySender();
const listenTogether = process.env.EAGLER_NETPLAY_LISTEN_TOGETHER === '1'
  ? (await import('./listen-together.mjs')).createListenTogether({
      onPublicTrackChange: () => roomDirectory.changed(),
      send(socket, message) {
        if (socket?.readyState === WebSocket.OPEN)
          sendBounded(socket, message);
      },
    })
  : null;
const lobbyBroadcaster = createRoomBroadcaster({
  snapshot: room => ({ type: 'state', room: lobbySnapshot(room) }),
  send: sendRoomMessage,
  changed: () => roomDirectory.changed(),
  coalesceMs: lobbyStateCoalesceMs,
});

function iceServersFor(roomId, runId, player) {
  const servers = [];
  if (stunUrls.length) servers.push({ urls: stunUrls });
  const activeTurn = readTurnConfig();
  const expires = Math.floor(Date.now() / 1000) + turnTtlSeconds;
  const username = `${expires}:${roomId}-${runId}-p${player}`;
  for (const node of [activeTurn, ...(activeTurn.backups ?? [])]) {
    if (!node.urls.length) continue;
    if (node.sharedSecret) {
      const credential = createHmac('sha1', node.sharedSecret).update(username).digest('base64');
      servers.push({urls: node.urls, username, credential});
    } else if (turnUsername && turnCredential) {
      servers.push({urls: node.urls, username: turnUsername, credential: turnCredential});
    }
  }
  return servers;
}

function handleDiagnosticConnection(socket) {
  const runId = `${Date.now().toString(36)}-${Math.random().toString(36).slice(2, 10)}`;
  let pingCount = 0;
  const idleTimer = setTimeout(() => socket.close(1000, 'diagnostic timeout'), 12000);
  sendBounded(
    socket,
    JSON.stringify({
      type: 'diagnostic-ready',
      protocol: 1,
      iceServers: iceServersFor('network-check', runId, 0),
    }),
  );
  socket.on('message', (data, isBinary) => {
    if (isBinary || data.length > 512) {
      socket.close(1003, 'diagnostic messages must be short text');
      return;
    }
    let message;
    try {
      message = JSON.parse(String(data));
    } catch {
      socket.close(1007, 'invalid diagnostic message');
      return;
    }
    if (
      message?.type !== 'diagnostic-ping' ||
      typeof message.nonce !== 'string' ||
      message.nonce.length > 96 ||
      ++pingCount > 3
    ) {
      socket.close(1008, 'invalid diagnostic message');
      return;
    }
    sendBounded(socket, JSON.stringify({ type: 'diagnostic-pong', nonce: message.nonce }));
  });
  socket.on('close', () => clearTimeout(idleTimer));
  socket.on('error', () => {});
}

function rejectRoomCreation(socket) {
  sendLobby(socket, {
    type: 'error',
    code: 'rate-limited',
    error: '已达到同时保留的房间数量限制，请关闭原房间后重试。',
  });
  socket.close(4008, 'room creation limit');
}

function newRoom(id) {
  const multiplayer = multiplayerPolicyForRoomId(id);
  return {
    id,
    th15: id.startsWith("th15mp-"),
    createdAt: Date.now(),
    lastActivityAt: Date.now(),
    multiplayer,
    clients: new Map(),
    lobbyClients: new Map(),
    lobbyDisconnectTimers: new Map(),
    runs: new Map(),
    lobby: {
      playerCount: defaultPlayerCount(multiplayer),
      difficulty: 1,
      ...(id.startsWith('th19mp-')?{th19Rules:[...TH19_VERSUS_RULES]}:{}),
      visibility: 'public',
      disableCheatMovement: false,
      ...(id.startsWith('th05mp-') ? { initialSpares: TH05_INITIAL_STOCK.spares, initialBombs: TH05_INITIAL_STOCK.bombs } : {}),
      ...(id.startsWith('th09mp-') ? {disableLocalChargePriority:false} : {}),
      ...(id.startsWith("th15mp-")
        ? {
            chapterThreshold: defaultTh15ChapterThreshold(
              defaultPlayerCount(multiplayer),
            ),
          }
        : {}),
      challengeMode: false,
      prankMode: false,
      inputDelay: 0,
      timingChoice: 'auto',
      startedAt: 0,
      adonisMode: multiplayerInputTimingPolicy(multiplayer).rollback ? 0 : 1,
      inputDelayAuto: false,
      predictionReserve: 2,
      timing: null,
      predictionLimit: multiplayerInputTimingPolicy(multiplayer).predictionLimit,
      settingsVersion: 1,
      phase: 'lobby',
      seats: [null, null, null],
      spectators: new Map(),
      startSerial: 0,
    },
  };
}

function getRoom(id, socket) {
  const existing = rooms.get(id);
  if (existing) return existing;
  if (!abuseGuard.createRoom(id, socket.relayAddress)) {
    rejectRoomCreation(socket);
    return null;
  }
  const room = newRoom(id);
  rooms.set(id, room);
  return room;
}

function getRun(room, runId) {
  let run = room.runs.get(runId);
  if (!run) {
    run = {
      clients: new Map(),
      signalClients: new Map(),
      rtcReady: new Set(),
      rtcFailed: false,
      playerCount: 0,
      route: null,
      ended: false,
      returnedLobbyClients: new Set(),
      routeTimer: null,
      admittedSpectators: new Set(),
      claimedSpectators: new Set(),
      spectatorClients: new Map(),
      spectatorHistory: [],
      spectatorHistoryBytes: 0,
      spectatorGraceTimer: null,
      spectatorAdmissionOpen: false,
      spectatorStopped: false,
      forwardCounters: new Map(),
      firstInputDropped: new Set(),
      inputLatestDropped: new Set(),
    };
    room.runs.set(runId, run);
  }
  return run;
}

function maybeDeleteRun(room, runId, run) {
  if (
    run.spectatorTimeline &&
    run.route &&
    run.clients.size === 0 &&
    run.signalClients.size === 0 &&
    String(room.lobby.startSerial) === String(runId)
  ) {
    run.ended = true;
    resetLobbyAfterRun(room);
  }
  if (run.clients.size !== 0 || run.signalClients.size !== 0 || run.spectatorClients.size !== 0)
    return;
  if (run.routeTimer) clearTimeout(run.routeTimer);
  if (run.spectatorGraceTimer) clearTimeout(run.spectatorGraceTimer);
  room.runs.delete(runId);
  if (String(room.lobby.startSerial) === String(runId)) resetLobbyAfterRun(room);
}

function startSpectatorGrace(roomId, room, runId, run) {
  if (run.spectatorGraceTimer) clearTimeout(run.spectatorGraceTimer);
  run.spectatorTimeline =
    roomId.startsWith('th19mp-') || process.env.EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE === '1' && roomId.startsWith('th09mp-');
  run.spectatorAdmissionOpen = true;
  run.onSpectatorAvailabilityChange = run.spectatorTimeline ? () => roomDirectory.changed() : null;
  clearSpectatorHistory(run);
  if (run.spectatorTimeline) return;
  run.spectatorGraceTimer = setTimeout(() => {
    run.spectatorGraceTimer = null;
    if (room.runs.get(runId) !== run) return;
    run.spectatorAdmissionOpen = false;
    let expired = 0;
    for (const spectatorId of [...run.admittedSpectators]) {
      if (run.claimedSpectators.has(spectatorId)) continue;
      run.admittedSpectators.delete(spectatorId);
      expired++;
    }
    clearSpectatorHistory(run);
    console.log(`SPECTATOR WINDOW CLOSE room=${roomId} run=${runId} expired=${expired}`);
    maybeDeleteRun(room, runId, run);
    maybeDeleteRoom(roomId, room);
  }, spectatorConnectGraceMs);
}

function acceptTh19SpectatorPayload(roomId,runId,room,run,payload){
 if(!roomId.startsWith('th19mp-'))return false;
 if(run.spectatorHistoryUnavailable)return true;
 try{
  const tape=run.th19Spectator??=new Th19SpectatorHistory();
  const row=tape.accept(payload);if(!row)return true;
  if(row.kind==='begin'&&!validTh19VersusRules(row.config.rules))throw Error('invalid TH19 spectator rules');
  run.spectatorTimeline=true;run.spectatorHistory=tape.history;run.spectatorHistoryBytes=tape.bytes;
  run.spectatorComplete=tape.ended;run.spectatorAdmissionOpen=!tape.ended;
  for(const [id,target] of run.spectatorClients)if(!target.spectatorReplaying)sendSpectatorPayload(target,payload,roomId,runId,id);
  if(row.kind==='begin'){
   roomDirectory.changed();
   for(const id of room.lobby.spectators.keys()){const lobby=room.lobbyClients.get(id);if(lobby)sendLobby(lobby,{type:'spectator-start',serial:room.lobby.startSerial,room:lobbySnapshot(room)});}
  }
 }catch(error){
  run.spectatorAdmissionOpen=false;run.spectatorHistoryUnavailable=true;
  for(const socket of run.spectatorClients.values())socket.close(1008,'invalid TH19 spectator stream');
  console.log(`TH19 SPECTATOR STOP room=${roomId} run=${runId} ${error.message}`);
 }
 return true;
}

function sendSpectatorPayload(socket, payload, roomId, runId, spectatorId) {
  if (socket.readyState !== WebSocket.OPEN) return false;
  if (
    !sendBounded(socket, payload, { binary: true, maxBufferedBytes: spectatorMaxBufferedBytes })
  ) {
    console.log(
      `SPECTATOR SLOW room=${roomId} run=${runId} client=${spectatorId} buffered=${socket.bufferedAmount}`,
    );
    return false;
  }
  return true;
}

function stopHostSpectatorStream(roomId, runId, run, source) {
  if (run.spectatorStopped || run.spectatorHostStopped) return;
  run.spectatorHostStopped = true;
  console.log(`SPECTATOR STOP room=${roomId} run=${runId} source=${source} ended=${run.spectatorComplete === true} viewers=${run.spectatorClients.size}`);
  // A validated cooperative End already closes admission. Existing viewers
  // still own the continuous replay prefix and may be paused far behind it.
  if (run.spectatorStages?.ended) return;
  stopSpectatorStream(run);
}

function sendSpectatorReplayFile(socket,run){
  const replay=run.spectatorNativeReplay;if(!replay?.complete)return;
  const packets=replay.chunks(),deadline=Date.now()+15000;
  const flush=()=>{
    if(socket.readyState!==WebSocket.OPEN||run.spectatorNativeReplay!==replay||Date.now()>deadline)return;
    if(socket.bufferedAmount>65536){setTimeout(flush,10);return;}
    const packet=packets.next();if(packet.done)return;
    socket.send(packet.value,{binary:true});setTimeout(flush,0);
  };flush();
}

function beginCooperativeSpectatorSession(roomId, runId, room, run, message) {
  if (!Object.hasOwn(COOPERATIVE_SPECTATOR_MARKERS, roomId.split('-')[0]) ||
      typeof message.session !== 'string' || !/^[a-f0-9]{16}$/.test(message.session) || /^0+$/.test(message.session) ||
      !Number.isInteger(message.seed) || message.seed < 0 || message.seed > 65535 ||
      !Number.isSafeInteger(message.generation) || message.generation < 0) return false;
  const previous = run.spectatorSession;
  if (previous && message.generation === previous.generation)
    return message.session === previous.session && message.seed === previous.seed;
  if (message.generation !== (previous ? previous.generation + 1 : 0) || previous?.session === message.session) return false;
  run.spectatorSession = {type: 'spectator-session', session: message.session, seed: message.seed, generation: message.generation};
  run.spectatorNativeReplay = new SpectatorReplayTransfer(message.session);
  run.spectatorStages = null; run.spectatorStageTiming = null;
  run.spectatorHistory = []; run.spectatorHistoryBytes = 0;
  run.spectatorComplete = run.spectatorStopped = run.spectatorHostStopped = run.spectatorHistoryUnavailable = false;
  run.spectatorTimeline = true; run.spectatorAdmissionOpen = false;
  if (run.spectatorGraceTimer) clearTimeout(run.spectatorGraceTimer);
  run.spectatorGraceTimer = null;
  for (const socket of run.spectatorClients.values()) {
    if (socket.spectatorReplayTimer) clearTimeout(socket.spectatorReplayTimer);
    socket.spectatorReplayEpoch = (socket.spectatorReplayEpoch || 0) + 1;
    socket.spectatorReplayTimer = null; socket.spectatorReplaying = false; socket.spectatorQueuedStages = [];
    sendSignal(socket, run.spectatorSession);
  }
  broadcastLobby(room); roomDirectory.changed();
  console.log(`SPECTATOR SESSION room=${roomId} run=${runId} generation=${message.generation}`);
  return true;
}

function acceptCooperativeSpectatorPayload(roomId, runId, room, run, payload) {
  const marker = COOPERATIVE_SPECTATOR_MARKERS[roomId.split('-')[0]];
  if (marker === undefined) return false;
  const extension = payload[0] === 69 && payload[1] === marker && payload[2] === 83 && payload[3] === 86;
  if (!run.spectatorStages && !extension) {
    if (payload.length === 40 && isSpectatorFrameForRoom(roomId, payload, run.playerCount))
      run.spectatorStageTiming = Buffer.from(payload);
    return false;
  }
  try {
    if (payload.length === 40 && payload[2] === 84 && payload[3] === 77 &&
        !isSpectatorFrameForRoom(roomId, payload, run.playerCount)) throw Error('invalid spectator timing');
    const first = !run.spectatorStages;
    const stream = run.spectatorStages || new SpectatorStageRelay(marker, run.playerCount);
    if (first && run.spectatorStageTiming) stream.accept(run.spectatorStageTiming);
    const kind = stream.accept(payload);
    if (run.spectatorSession && stream.session.toString(16).padStart(16, '0') !== run.spectatorSession.session)
      throw Error('spectator session metadata mismatch');
    if (kind === 'duplicate-timing') return true;
    run.spectatorStages = stream;
    run.spectatorTimeline = true;
    run.spectatorAdmissionOpen = !stream.ended;
    run.spectatorComplete = stream.ended;
    run.spectatorHistory = stream.history;
    run.spectatorHistoryBytes = stream.bytes;
    if (run.spectatorGraceTimer) clearTimeout(run.spectatorGraceTimer);
    run.spectatorGraceTimer = null;
    run.onSpectatorAvailabilityChange = () => roomDirectory.changed();
    for (const [id, target] of run.spectatorClients)
      if (!target.spectatorReplaying) {
        if (sendSpectatorPayload(target, payload, roomId, runId, id) && first)
          sendSpectatorPayload(target, stream.bootstrap(), roomId, runId, id);
      }
      else if (kind === 'checkpoint') {
        // Retain every intervening stage for a viewer still downloading its
        // first prefix. Jumping to the newest array would leave an input gap.
        const queue = target.spectatorQueuedStages ||= [];
        queue.push(stream.history);
        if (queue.length > 32 || queue.reduce((sum, rows) => sum + rows.reduce((n, row) => n + row.length, 0), 0) > 16 * 1024 * 1024)
          target.close(1013, 'spectator stage history too slow');
      }
    if (first || stream.ended) { broadcastLobby(room); roomDirectory.changed(); }
    if (kind === 'checkpoint')
      for (const id of room.lobby.spectators.keys()) {
        const lobby = room.lobbyClients.get(id);
        if (lobby) sendLobby(lobby, {type: 'spectator-start', serial: room.lobby.startSerial, room: lobbySnapshot(room)});
      }
  } catch (error) {
    console.log(`SPECTATOR STAGE STOP room=${roomId} run=${runId} ${error.message}`);
    stopSpectatorStream(run);
  }
  return true;
}

function maybeDeleteRoom(roomId, room) {
  if (
    room.clients.size === 0 &&
    room.lobbyClients.size === 0 &&
    room.lobbyDisconnectTimers.size === 0 &&
    room.runs.size === 0 &&
    rooms.get(roomId) === room
  ) {
    lobbyBroadcaster.clear(room);
    listenTogether?.close(room);
    rooms.delete(roomId);
    const code = numericRoomCode(roomId);
    if (roomCodes.get(code) === roomId) roomCodes.delete(code);
    abuseGuard.releaseRoom(roomId);
  }
}

function removeClient(roomId, runId, player, socket) {
  const room = rooms.get(roomId);
  if (!room) return;
  const run = room.runs.get(runId);
  if (!run) return;
  if (run.clients.get(player) === socket) {
    run.clients.delete(player);
    if (run.route === 'relay' && !run.ended) {
      // Retire this non-resumable gameplay stream while the Launcher keeps
      // ownership of the lobby. RTC closes unused relay sockets normally.
      run.ended = true;
      if (run.routeTimer) clearTimeout(run.routeTimer);
      if (run.spectatorGraceTimer) clearTimeout(run.spectatorGraceTimer);
      run.routeTimer = run.spectatorGraceTimer = null;
      clearSpectatorHistory(run);
      for (const peer of [
        ...run.clients.values(),
        ...run.signalClients.values(),
        ...(run.spectatorTimeline && run.spectatorComplete ? [] : run.spectatorClients.values()),
      ]) {
        if (peer.readyState === WebSocket.OPEN)
          peer.close(1001, `player ${player + 1} left the run`);
      }
    }
  }
  maybeDeleteRun(room, runId, run);
  maybeDeleteRoom(roomId, room);
}

function sendSignal(socket, payload) {
  sendBounded(socket, JSON.stringify(payload));
}

function broadcastSignal(run, payload) {
  const message = JSON.stringify(payload);
  for (const socket of run.signalClients.values()) sendBounded(socket, message);
}

function broadcastRoute(run, payload) {
  const message = JSON.stringify(payload);
  const send = (player, socket) => {
    sendBounded(socket, message, { delayMs: player === routeSkewPlayer ? routeSkewMs : 0 });
  };
  for (const [player, socket] of run.signalClients) send(player, socket);
  for (const [player, socket] of run.clients) send(player, socket);
}

function chooseRoute(roomId, runId, room, run, route) {
  if (run.route) return;
  run.route = route;
  if (run.routeTimer) {
    clearTimeout(run.routeTimer);
    run.routeTimer = null;
  }
  if (String(room.lobby.startSerial) === String(runId) && room.lobby.phase === 'starting') {
    room.lobby.phase = 'running';
    if (roomId.startsWith('th19mp-') && run.lobbyClientIds?.length === 2) {
      // Lobby reports can arrive after the gameplay sockets have closed.
      room.th19StatsRun = {serial:room.lobby.startSerial,clientIds:[...run.lobbyClientIds],reports:[null,null]};
    }
    broadcastLobby(room);
  }
  console.log(`ROUTE room=${roomId} run=${runId} mode=${route}`);
  // Signaling is preferred, but a browser/network can leave one signaling
  // WebSocket stuck in CONNECTING while both gameplay relay sockets are live.
  // Deliver the barrier on both channels so fallback never depends on the
  // unhealthy signaling path it is meant to recover from.
  broadcastRoute(run, { type: 'route', mode: route });
}

function failRoute(roomId, runId, run, reason) {
  if (run.route || run.ended) return;
  run.route = 'failed';
  run.ended = true;
  if (run.routeTimer) clearTimeout(run.routeTimer);
  if (run.spectatorGraceTimer) clearTimeout(run.spectatorGraceTimer);
  run.routeTimer = null;
  run.spectatorGraceTimer = null;
  clearSpectatorHistory(run);
  console.log(`ROUTE room=${roomId} run=${runId} mode=failed reason=${reason}`);
  const message = JSON.stringify({type: 'route', mode: 'failed', reason});
  for (const socket of [...run.signalClients.values(), ...run.clients.values()]) {
    sendBounded(socket, message);
    if (socket.readyState === WebSocket.OPEN) socket.close(1013, reason);
  }
  for (const socket of run.spectatorClients.values()) {
    if (socket.readyState === WebSocket.OPEN) socket.close(1013, reason);
  }
}

function maybeStartRouteTimer(roomId, runId, room, run) {
  const allSignalsPresent = run.signalClients.size >= run.playerCount;
  const allRelaysPresent = run.clients.size >= run.playerCount;
  if (
    run.route ||
    run.routeTimer ||
    run.playerCount < 2 ||
    (!allSignalsPresent && !allRelaysPresent)
  )
    return;
  run.routeTimer = setTimeout(() => {
    run.routeTimer = null;
    run.rtcFailed = true;
    maybeResolveRoute(roomId, runId, room, run);
  }, rtcTimeoutMs);
}

function maybeResolveRoute(roomId, runId, room, run) {
  if (run.route || run.playerCount < 2) return;
  if (run.rtcFailed) {
    if (!allowGameplayRelayFallback && !roomId.startsWith('th05mp-')) {
      failRoute(roomId, runId, run, 'WebRTC connection unavailable');
      return;
    }
    // Relay release is a barrier too: do not let the first endpoint begin
    // frame-zero traffic until every expected relay socket and signaling
    // client is already present. Route delivery itself is asynchronous, so
    // clients also buffer relay packets during that tiny propagation window.
    if (run.clients.size === run.playerCount) chooseRoute(roomId, runId, room, run, 'relay');
    return;
  }
  if (run.signalClients.size === run.playerCount && run.rtcReady.size === run.playerCount) {
    chooseRoute(roomId, runId, room, run, 'rtc');
    return;
  }
  maybeStartRouteTimer(roomId, runId, room, run);
}

function acceptTh09PlayerReport(room, run, runId, player, message) {
  const clientId = run.lobbyClientIds?.[player];
  if (message.winner !== undefined && message.characters !== undefined &&
      run.memberIds?.has(clientId) && !run.releasedPlayers?.has(player) &&
      acceptTh09MatchStats(room, run, clientId, {...message, serial:Number(runId)}))
    broadcastLobby(room);
}

function handleSignalConnection(socket, roomId, runId, player, playerCount) {
  const room = getRoom(roomId, socket);
  if (!room) return;
  const run = getRun(room, runId);
  if (run.ended) {
    socket.close(1008, 'run already ended');
    return;
  }
  if (run.releasedPlayers?.has(player)) {
    socket.close(4008, 'membership released');
    return;
  }
  if (run.playerCount && run.playerCount !== playerCount) {
    socket.close(1008, 'player count mismatch');
    return;
  }
  run.playerCount = playerCount;
  const previous = run.signalClients.get(player);
  if (previous && previous !== socket && previous.readyState === WebSocket.OPEN)
    previous.close(1000, 'signaling reconnected');
  const existingPeers = [...run.signalClients.keys()].filter((peer) => peer !== player);
  run.signalClients.set(player, socket);
  sendSignal(socket, {
    type: 'peers',
    peers: existingPeers,
    route: run.route,
    iceServers: iceServersFor(roomId, runId, player),
    ...(roomId.startsWith('th09mp-') && run.memberIds ? {th09MatchReport:true} : {}),
  });
  for (const [peer, target] of run.signalClients) {
    if (peer !== player) sendSignal(target, { type: 'peer-join', player });
  }
  console.log(
    `SIGNAL JOIN room=${roomId} run=${runId} player=${player} peers=${run.signalClients.size}`,
  );
  maybeResolveRoute(roomId, runId, room, run);

  const acceptSignal = createRelayMessageGate({
    maxBytes: RELAY_LIMITS.controlBytes,
    messagesPerSecond: 120,
    bytesPerSecond: 256 * 1024,
  });
  socket.on('message', (data, isBinary) => {
    if (!acceptRelayMessage(socket, data, acceptSignal)) return;
    if (isBinary) {
      socket.close(1003, 'signaling expects text');
      return;
    }
    let message;
    try {
      message = JSON.parse(String(data));
    } catch {
      sendSignal(socket, { type: 'error', error: 'invalid signaling message' });
      return;
    }
    if (!validRelayControlMessage(message)) {
      socket.close(1008, 'invalid signaling message');
      return;
    }
    if (roomId.startsWith('th09mp-') && message.type === 'th09-match') {
      if (run.signalClients.get(player) === socket) acceptTh09PlayerReport(room, run, runId, player, message);
      return;
    }
    if (message.type === 'spectator-stop') {
      if (player === 0 && run.signalClients.get(player) === socket &&
          (!run.spectatorSession || message.generation === run.spectatorSession.generation)) {
        stopHostSpectatorStream(roomId, runId, run, 'host-signal');
      }
      return;
    }
    if (message.type === 'signal') {
      const to = Number(message.to);
      if (!Number.isInteger(to) || to < 0 || to >= playerCount || to === player) return;
      const target = run.signalClients.get(to);
      if (!target) return;
      const signal = normalizeRelaySignal(message);
      if (!signal) {
        socket.close(1008, 'invalid signaling payload');
        return;
      }
      sendSignal(target, {
        type: 'signal',
        from: player,
        ...signal,
      });
      return;
    }
    if (message.type === 'rtc-ready') {
      run.rtcReady.add(player);
      maybeResolveRoute(roomId, runId, room, run);
      return;
    }
    if (message.type === 'ice-restart-request' && run.route === 'rtc') {
      const to = Number(message.to);
      if (!Number.isInteger(to) || to < 0 || to >= playerCount || to === player) return;
      const target = run.signalClients.get(to);
      if (target) sendSignal(target, { type: 'ice-restart-request', from: player });
      return;
    }
    if (
      message.type === 'peer-disconnected' &&
      run.route === 'rtc' &&
      run.signalClients.get(player) === socket
    ) {
      run.disconnectedPlayers ??= new Set();
      if (!run.disconnectedPlayers.has(player)) {
        run.disconnectedPlayers.add(player);
        broadcastSignal(run, { type: 'peer-disconnected', from: player });
      }
      return;
    }
    if (message.type === 'rtc-failed') {
      run.rtcFailed = true;
      maybeResolveRoute(roomId, runId, room, run);
      return;
    }
  });

  socket.on('close', () => {
    if (run.signalClients.get(player) === socket) {
      run.signalClients.delete(player);
      run.rtcReady.delete(player);
      if (!run.route) {
        run.rtcFailed = true;
        maybeResolveRoute(roomId, runId, room, run);
      }
    }
    console.log(`SIGNAL LEAVE room=${roomId} run=${runId} player=${player}`);
    maybeDeleteRun(room, runId, run);
    maybeDeleteRoom(roomId, room);
  });
}

function lobbySnapshot(room) {
  const spectators = [...room.lobby.spectators.entries()]
    .filter(([clientId]) => room.lobbyClients.has(clientId))
    .map(([clientId, name]) => ({ clientId, name }));
  return {
    playerCount: room.lobby.playerCount,
    difficulty: room.lobby.difficulty,
    visibility: room.lobby.visibility,
    disableCheatMovement: room.lobby.disableCheatMovement,
    ...(room.id.startsWith('th05mp-')?{initialSpares:room.lobby.initialSpares,initialBombs:room.lobby.initialBombs}:{}),
    ...(room.id.startsWith('th09mp-') ? {disableLocalChargePriority:room.lobby.disableLocalChargePriority} : {}),
    ...(room.id.startsWith('th19mp-')?{th19Rules:[...room.lobby.th19Rules]}:{}),
    ...(room.th15 ? { chapterThreshold: room.lobby.chapterThreshold } : {}),
    challengeMode: room.lobby.challengeMode,
    prankMode: room.lobby.prankMode,
    inputDelay: room.lobby.inputDelay,
    timingChoice: room.lobby.timingChoice,
    startedAt: room.lobby.startedAt,
    adonisMode: room.lobby.adonisMode,
    inputDelayAuto: room.lobby.inputDelayAuto,
    predictionReserve: room.lobby.predictionReserve,
    timing: room.lobby.timing,
    predictionLimit: room.lobby.predictionLimit,
    settingsVersion: room.lobby.settingsVersion,
    // Server truth for the optional runtime capability "netplayAllowWebsocketFallback":
    // a relay route can only be selected when this deployment actually enables the
    // gameplay relay fallback. Clients mirror it so the launcher can opt the run in.
    gameplayRelayFallback: allowGameplayRelayFallback,
    phase: room.lobby.phase,
    ...(room.lobby.phase !== 'lobby' && room.runs.get(String(room.lobby.startSerial))?.runtimeGeneration
      ? {runtimeGeneration: room.runs.get(String(room.lobby.startSerial)).runtimeGeneration} : {}),
    startSerial: room.lobby.startSerial,
    spectators,
    spectatorCount: spectators.length,
    ...((process.env.EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE === '1' && room.id.startsWith('th09mp-')) ||
        room.runs.get(String(room.lobby.startSerial))?.spectatorStages
      ? { spectatorTimeline: true }
      : {}),
    seats: room.lobby.seats.map((seat) =>
      seat
        ? {
            clientId: seat.clientId,
            name: seat.name || '',
            ...(/^th(?:09|19)mp-/.test(room.id)
              ? { stats: th09PlayerStats(room, seat.clientId) }
              : {}),
            loadout: seat.loadout,
            controlMode: publicControlMode(seat),
            ...(typeof seat.touchEnabled === 'boolean' ? { touchEnabled: seat.touchEnabled } : {}),
            mobileDevice: seat.mobileDevice === true,
            resource: seat.resource || null,
            ready: !!seat.ready && seat.readyVersion === room.lobby.settingsVersion,
            ...(seat.runtimeGeneration ? {runtimeGeneration: seat.runtimeGeneration} : {}),
            readyVersion: Number(seat.readyVersion) || 0,
            offline: !room.lobbyClients.has(seat.clientId),
          }
        : null,
    ),
  };
}

function sendLobby(socket, payload) {
  sendBounded(socket, JSON.stringify(payload));
}

function sendRoomMessage(room, message) {
  for (const socket of room.lobbyClients.values()) sendBounded(socket, message);
}

function broadcastLobby(room, payload = null) {
  reconcileTh09Duel(room);
  lobbyBroadcaster.publish(room, payload);
}

function clearLobbySeat(room, clientId) {
  let changed = false;
  for (let index = 0; index < room.lobby.seats.length; index++) {
    if (room.lobby.seats[index]?.clientId !== clientId) continue;
    room.lobby.seats[index] = null;
    changed = true;
  }
  return changed;
}

function lobbySeatOf(room, clientId) {
  return room.lobby.seats.findIndex((seat) => seat?.clientId === clientId);
}

function validLoadout(room, value) {
  const loadoutCount = room.multiplayer?.loadouts?.length ?? 6;
  return Number.isInteger(value) && value >= 0 && value < loadoutCount;
}

function normalizeDisplayName(value) {
  // Strip all Unicode Bidi_Control characters and the existing invisible
  // controls before trimming/counting, so avatar initials stay visible too.
  return [
    ...String(value || '')
      .replace(
        /[\u0000-\u001f\u007f\u061c\u200b-\u200f\u202a-\u202e\u2060-\u2064\u2066-\u2069\ufeff]/g,
        '',
      )
      .trim(),
  ]
    .slice(0, 12)
    .join('');
}

function invalidateLobbyReady(room) {
  room.lobby.settingsVersion++;
  for (const seat of room.lobby.seats) {
    if (!seat) continue;
    seat.ready = false;
    seat.readyVersion = 0;
  }
}

function resetLobbyAfterRun(room) {
  if (room.lobby.phase === 'lobby') return;
  room.lobby.phase = 'lobby';
  roomDirectory.activity(room);
  invalidateLobbyReady(room);
  broadcastLobby(room);
}

function sendRoomProduct(socket, roomId, code, spectator = false) {
  sendLobby(socket, {type: 'room-product', product: roomId.split('-')[0], code, spectator});
  socket.close(4007, 'room product changed');
}

function reserveRoomRole(room, clientId) {
  const timer = setTimeout(() => {
    if (room.lobbyDisconnectTimers.get(clientId) !== timer) return;
    room.lobbyDisconnectTimers.delete(clientId);
    if (!room.lobbyClients.has(clientId)) {
      const seatChanged = clearLobbySeat(room, clientId);
      const spectatorChanged = room.lobby.spectators.delete(clientId);
      if (seatChanged) invalidateLobbyReady(room);
      if (seatChanged || spectatorChanged) broadcastLobby(room);
    }
    maybeDeleteRoom(room.id, room);
  }, lobbyReconnectGraceMs);
  room.lobbyDisconnectTimers.set(clientId, timer);
}

function switchLobbyProduct(room, product) {
  const code = numericRoomCode(room.id);
  if (!code || typeof product !== 'string' || !multiplayerConfigForProduct(product)) return false;
  const targetId = `${product}-${code}`;
  if (rooms.has(targetId) || !abuseGuard.moveRoom(room.id, targetId)) return false;
  const next = newRoom(targetId);
  const previous = room.lobby;
  next.lobby.visibility = previous.visibility;
  next.lobby.disableCheatMovement = previous.disableCheatMovement;
  next.lobby.challengeMode = previous.challengeMode && next.multiplayer.gameplay === 'cooperative';
  next.lobby.startSerial = previous.startSerial;
  if (validPlayerCount(next.multiplayer, previous.playerCount)) next.lobby.playerCount = previous.playerCount;
  if (next.th15) next.lobby.chapterThreshold = defaultTh15ChapterThreshold(next.lobby.playerCount);
  next.lobby.difficulty = Math.min(previous.difficulty, Math.max(0, next.multiplayer.difficulties.length - 1));
  next.removedMembers = room.removedMembers;
  next.removedClients = room.removedClients;
  next.lobby.spectators = new Map(previous.spectators);
  previous.seats.forEach((seat, index) => {
    if (!seat) return;
    if (index >= next.lobby.playerCount) next.lobby.spectators.set(seat.clientId, seat.name);
    else next.lobby.seats[index] = {...seat, loadout: 0, ready: false, readyVersion: 0, resource: null, runtimeGeneration: undefined};
  });

  const peers = [...room.lobbyClients];
  const clientIds = new Set([...room.lobbyClients.keys(), ...room.lobbyDisconnectTimers.keys()]);
  // Publish the new route and reserve each role before closing the old sockets.
  rooms.set(targetId, next);
  roomCodes.set(code, targetId);
  roomDirectory.moveMembers(room.id, targetId, clientIds);
  for (const id of clientIds) reserveRoomRole(next, id);
  for (const timer of room.lobbyDisconnectTimers.values()) clearTimeout(timer);
  room.lobbyDisconnectTimers.clear();
  room.lobbyClients.clear();
  previous.seats.fill(null);
  previous.spectators.clear();
  for (const [id, peer] of peers) sendRoomProduct(peer, targetId, code, next.lobby.spectators.has(id));
  for (const run of room.runs.values()) {
    if (run.routeTimer) clearTimeout(run.routeTimer);
    if (run.spectatorGraceTimer) clearTimeout(run.spectatorGraceTimer);
    clearSpectatorHistory(run);
    for (const peers of [run.clients, run.signalClients, run.spectatorClients])
      for (const peer of peers.values()) peer.close(1001, 'room product changed');
  }
  room.runs.clear();
  maybeDeleteRoom(room.id, room);
  return true;
}

function handleLobbyConnection(socket, roomId, clientId, memberId, intent, initialPolicy) {
  const code = numericRoomCode(roomId);
  const current = roomCodes.get(code);
  if (current && current !== roomId) {
    if (intent === 'create') {
      sendLobby(socket, {type: 'error', error: '房间号已被使用，请重新创建。'});
      socket.close(4007, 'room unavailable');
    } else {
      const spectator = roomDirectory.memberIdFor(current, clientId) === memberId &&
        rooms.get(current)?.lobby.spectators.has(clientId);
      sendRoomProduct(socket, current, code, spectator);
    }
    return;
  }
  if (
    initialPolicy?.chapterThreshold !== undefined &&
    (!roomId.startsWith("th15mp-") ||
      !validTh15ChapterThreshold(initialPolicy.chapterThreshold))
  ) {
    sendLobby(socket, {
      type: "error",
      error: "invalid chapter reward threshold",
    });
    socket.close(4007, "invalid settings");
    return;
  }
  if(initialPolicy?.th19Rules!==undefined&&(!roomId.startsWith('th19mp-')||!validTh19VersusRules(initialPolicy.th19Rules))){sendLobby(socket,{type:'error',error:'invalid TH19 versus rules'});socket.close(4007,'invalid settings');return;}
  const existing = rooms.get(roomId);
  if (!existing && intent !== 'join' && !abuseGuard.canCreateRoom(socket.relayAddress)) {
    rejectRoomCreation(socket);
    return;
  }
  if (
    (intent === 'join' && !existing) ||
    (intent === 'create' &&
      existing &&
      !existing.lobbyClients.has(clientId) &&
      lobbySeatOf(existing, clientId) < 0)
  ) {
    sendLobby(socket, {
      type: 'error',
      error: intent === 'join' ? '房间已关闭，请返回大厅刷新。' : '房间号已被使用，请重新创建。',
    });
    socket.close(4007, 'room unavailable');
    return;
  }
  if (!roomDirectory.admit(socket, roomId, clientId, memberId)) return;
  if (nicknameService) void nicknameService.current(memberId).then(profile=>publishNickname(memberId,nicknameRequests.has(memberId) ? {...profile,status:'reviewing'} : profile)).catch(()=>sendLobby(socket,{type:'nickname-status',...nicknameProfile(memberId),status:'unavailable'}));
  const room = getRoom(roomId, socket);
  if (!room) return;
  if (code) roomCodes.set(code, roomId);
  if (!existing && intent === 'create') {
    if(room.id.startsWith('th19mp-'))room.lobby.th19Rules=[...(initialPolicy?.th19Rules??TH19_VERSUS_RULES)];
    room.lobby.visibility = initialPolicy?.visibility === 'private' ? 'private' : 'public';
    room.lobby.disableCheatMovement = initialPolicy?.disableCheatMovement === true;
    if(room.id.startsWith('th09mp-'))room.lobby.disableLocalChargePriority=initialPolicy?.disableLocalChargePriority===true;
    const modesSupported = room.multiplayer?.gameplay === 'cooperative';
    room.lobby.challengeMode = modesSupported && initialPolicy?.challengeMode === true;
    room.lobby.prankMode = false;
    if (validPlayerCount(room.multiplayer, initialPolicy?.playerCount))
      room.lobby.playerCount = initialPolicy.playerCount;
    if (room.th15)
      room.lobby.chapterThreshold =
        initialPolicy?.chapterThreshold ??
        defaultTh15ChapterThreshold(room.lobby.playerCount);
    const difficultyMax = Math.max(0, (room.multiplayer?.difficulties?.length ?? 6) - 1);
    if (Number.isInteger(initialPolicy?.difficulty))
      room.lobby.difficulty = Math.max(0, Math.min(difficultyMax, initialPolicy.difficulty));
  }
  const pendingDisconnect = room.lobbyDisconnectTimers.get(clientId);
  if (pendingDisconnect) {
    clearTimeout(pendingDisconnect);
    room.lobbyDisconnectTimers.delete(clientId);
  }
  const previous = room.lobbyClients.get(clientId);
  if (previous && previous !== socket && previous.readyState === WebSocket.OPEN)
    previous.close(1000, 'lobby reconnected');
  room.lobbyClients.set(clientId, socket);
  console.log(`LOBBY JOIN room=${roomId} client=${clientId} peers=${room.lobbyClients.size}`);
  sendLobby(socket, {
    type: 'state',
    room: lobbySnapshot(room),
    roomDirectory: { version: 1, controlModes: true, runtimeVersions: true, runReturn: true, globalRoomCodes: !!code },
    roomActions: 1,
    ...(listenTogether ? { listenTogether: 2 } : {}),
    roomProbe: { iceServers: iceServersFor(roomId, 'lobby-probe', 0) },
  });
  listenTogether?.sync(room, clientId);
  if (pendingDisconnect) broadcastLobby(room);
  let probeBudget = 0,
    probeWindow = Date.now();
  const acceptLobby = createRelayMessageGate({
    maxBytes: RELAY_LIMITS.controlBytes,
    messagesPerSecond: 60,
    bytesPerSecond: 256 * 1024,
  });

  socket.on('message', (data, isBinary) => {
    if (room.lobbyClients.get(clientId) !== socket) return;
    if (!acceptRelayMessage(socket, data, acceptLobby)) return;
    if (isBinary) {
      socket.close(1003, 'lobby expects text frames');
      return;
    }
    let message;
    try {
      message = JSON.parse(String(data));
    } catch {
      sendLobby(socket, { type: 'error', error: 'invalid lobby message' });
      return;
    }
    if (!validRelayControlMessage(message)) {
      socket.close(1008, 'invalid lobby message');
      return;
    }
    if (listenTogether && message.type === 'listen-together') {
      listenTogether.handle(room, clientId, socket, message);
      return;
    }
    if (message.type === 'activity') {
      if (lobbySeatOf(room, clientId) >= 0 || room.lobby.spectators.has(clientId))
        roomDirectory.activity(room);
      return;
    }
    if (
      [
        'take-seat',
        'stand-up',
        'spectate',
        'leave-spectator',
        'set-name',
        'set-loadout',
        'set-ready',
        'settings',
        'start',
        'remove-player',
        'move-player-to-spectator',
        'remove-spectator',
      ].includes(message.type)
    )
      roomDirectory.activity(room);

    if (message?.type === 'room-probe') {
      if (
        room.lobby.phase !== 'lobby' ||
        room.lobbyClients.get(clientId) !== socket ||
        lobbySeatOf(room, clientId) < 0
      )
        return;
      if (Date.now() - probeWindow > 10000) {
        probeWindow = Date.now();
        probeBudget = 0;
      }
      if (++probeBudget > 180) return;
      const onlinePeers = new Set(
        room.lobby.seats
          .slice(0, room.lobby.playerCount)
          .filter((seat) => seat && room.lobbyClients.has(seat.clientId))
          .map((seat) => seat.clientId),
      );
      const envelope = roomProbeEnvelope(message, clientId, onlinePeers);
      if (envelope) sendLobby(room.lobbyClients.get(message.to), envelope);
      return;
    }

    if (message.type === 'take-seat') {
      if (room.lobby.phase !== 'lobby') {
        sendLobby(socket, { type: 'state', room: lobbySnapshot(room) });
        return;
      }
      const seat = Number(message.seat);
      if (
        !Number.isInteger(seat) ||
        seat < 0 ||
        seat >= room.lobby.playerCount ||
        !validLoadout(room, Number(message.loadout))
      ) {
        sendLobby(socket, { type: 'error', error: 'invalid seat or loadout' });
        return;
      }
      const occupant = room.lobby.seats[seat];
      if (
        room.lobby.disableCheatMovement &&
        !['touch', 'joystick', 'joystick-free'].includes(message.movementMode)
      ) {
        sendLobby(socket, {
          type: 'error',
          code: 'movement-policy',
          seat,
          error: '必须调整你的作弊移动方式',
        });
        return;
      }
      if (occupant && occupant.clientId !== clientId) {
        sendLobby(socket, { type: 'error', error: `P${seat + 1} 已被占用` });
        sendLobby(socket, { type: 'state', room: lobbySnapshot(room) });
        return;
      }
      const previousSeat = lobbySeatOf(room, clientId);
      const previousEntry = previousSeat >= 0 ? room.lobby.seats[previousSeat] : null;
      const previousLoadout = previousEntry?.loadout ?? null;
      const changesMatchConfig =
        previousSeat !== seat || previousLoadout !== Number(message.loadout);
      const preserveReady =
        !changesMatchConfig &&
        previousEntry?.ready === true &&
        previousEntry.readyVersion === room.lobby.settingsVersion;
      if (changesMatchConfig) invalidateLobbyReady(room);
      clearLobbySeat(room, clientId);
      room.lobby.spectators.delete(clientId);
      const currentRun = room.runs.get(String(room.lobby.startSerial));
      if (currentRun && !currentRun.claimedSpectators.has(clientId))
        currentRun.admittedSpectators.delete(clientId);
      room.lobby.seats[seat] = {
        clientId,
        name: seatNickname(socket, memberId, message.name),
        loadout: Number(message.loadout),
        movementMode: message.movementMode,
        touchEnabled: typeof message.touchEnabled === 'boolean' ? message.touchEnabled : undefined,
        mobileDevice: message.mobileDevice === true,
        ready: preserveReady,
        runtimeGeneration: previousEntry?.runtimeGeneration,
        readyVersion: preserveReady ? room.lobby.settingsVersion : 0,
        resource: previousEntry?.resource || null,
      };
      broadcastLobby(room);
      if (previousSeat !== seat) sendRoomMessage(room, JSON.stringify({type: 'room-action', action: 'seat'}));
      return;
    }

    if (message.type === 'stand-up') {
      if (room.lobby.phase !== 'lobby') {
        sendLobby(socket, { type: 'error', error: '本局已经开始' });
        return;
      }
      if (clearLobbySeat(room, clientId)) {
        invalidateLobbyReady(room);
        broadcastLobby(room);
      }
      return;
    }

    if (message.type === 'spectate') {
      if (room.lobby.phase !== 'lobby' && lobbySeatOf(room, clientId) >= 0) {
        sendLobby(socket, { type: 'error', error: '对局中的玩家不能切换为观众' });
        return;
      }
      const seatChanged = clearLobbySeat(room, clientId);
      const spectatorChanged = !room.lobby.spectators.has(clientId);
      room.lobby.spectators.set(clientId, seatNickname(socket, memberId, message.name));
      if (seatChanged) invalidateLobbyReady(room);
      if (seatChanged || spectatorChanged) broadcastLobby(room);
      if (room.lobby.phase !== 'lobby') {
        const run = room.runs.get(String(room.lobby.startSerial));
        if (run?.spectatorTimeline && (!run.spectatorStages || run.spectatorAdmissionOpen) && !run.ended && !run.spectatorStopped) {
          run.admittedSpectators.add(clientId);
          run.memberIds.set(clientId, roomDirectory.memberIdFor(roomId, clientId));
          sendLobby(socket, {
            type: 'spectator-start',
            serial: room.lobby.startSerial,
            room: lobbySnapshot(room),
          });
          return;
        }
        sendLobby(socket, {type: 'spectator-waiting', serial: room.lobby.startSerial, room: lobbySnapshot(room)});
      }
      return;
    }

    if (message.type === 'leave-spectator') {
      const changed = room.lobby.spectators.delete(clientId);
      for (const run of room.runs.values()) {
        // Retire this membership's receiver before a new admission can claim
        // it. The old socket's delayed close must not release its successor.
        run.admittedSpectators.delete(clientId);
        run.claimedSpectators.delete(clientId);
        const receiver = run.spectatorClients.get(clientId);
        run.spectatorClients.delete(clientId);
        receiver?.close(1000, 'spectator left');
      }
      if (changed) broadcastLobby(room);
      return;
    }

    if (message.type === 'set-name') {
      if (nicknameService) {reviewNickname(socket,memberId,message.name);return;}
      const name = normalizeDisplayName(message.name);
      const playerSeat = lobbySeatOf(room, clientId);
      if (playerSeat >= 0) room.lobby.seats[playerSeat].name = name;
      else if (room.lobby.spectators.has(clientId)) room.lobby.spectators.set(clientId, name);
      else {
        sendLobby(socket, { type: 'error', error: '请先加入玩家席或旁观' });
        return;
      }
      broadcastLobby(room);
      return;
    }

    const seat = lobbySeatOf(room, clientId);
    if (message.type === 'run-returned') {
      const run = room.runs.get(String(message.serial));
      if (!Number.isSafeInteger(message.serial) || message.serial <= 0 ||
          message.serial !== room.lobby.startSerial || room.lobby.phase === 'lobby' ||
          !run?.lobbyClientIds || run.lobbyClientIds[seat] !== clientId ||
          run.memberIds?.get(clientId) !== memberId || roomDirectory.memberIdFor(roomId, clientId) !== memberId) return;
      run.returnedLobbyClients.add(clientId);
      // Logical return is independent of the transports that still echo the
      // final input ACK and serve completed history to slower spectators.
      if (run.lobbyClientIds.every(id => run.returnedLobbyClients.has(id))) resetLobbyAfterRun(room);
      return;
    }
    if (message.type === 'switch-product') {
      if (seat !== 0 || room.lobby.phase !== 'lobby') return;
      if (!switchLobbyProduct(room, message.product))
        sendLobby(socket, {type: 'error', error: '暂时无法更换作品，请稍后再试。'});
      return;
    }
    if (message.type === 'room-nudge') {
      if (seat <= 0 || room.lobby.phase !== 'lobby' || !room.lobby.seats[seat]?.ready) return;
      const peers = room.lobby.seats
        .filter(entry => entry && entry.clientId !== clientId)
        .map(entry => room.lobbyClients.get(entry.clientId))
        .filter(peer => peer?.readyState === WebSocket.OPEN);
      if (!peers.length) return;
      const now = Date.now();
      if (now < (room.lobby.nextNudgeAt ?? 0)) {
        sendLobby(socket, {type: 'room-action', action: 'nudge-limited'});
        return;
      }
      room.lobby.nextNudgeAt = now + 5000;
      sendLobby(socket, {type: 'room-action', action: 'nudge', fromSeat: seat, outgoing: true});
      for (const peer of peers) sendLobby(peer, {type: 'room-action', action: 'nudge', fromSeat: seat});
      return;
    }
    if (message.type === 'quick-chat') {
      const spectator = seat < 0 && room.lobby.spectators.has(clientId);
      if (
        (seat < 0 && !spectator) ||
        message.serial !== room.lobby.startSerial ||
        !quickChatPhrase(message.phrase)
      )
        return;
      const now = Date.now();
      if (now - (socket.lastQuickChatAt ?? 0) < 500) return;
      socket.lastQuickChatAt = now;
      broadcastLobby(room, {
        type: 'quick-chat',
        room: roomId,
        serial: room.lobby.startSerial,
        seat,
        ...(spectator ? { spectator: true } : {}),
        clientId,
        phrase: message.phrase,
      });
      return;
    }
    if (message.type === 'resources-changed' && (seat >= 0 || room.lobby.spectators.has(clientId))) {
      if (room.lobby.phase === 'lobby') {
        invalidateLobbyReady(room);
        if (seat >= 0) room.lobby.seats[seat].resource = {status: 'preparing', stage: 'package', percent: null};
        broadcastLobby(room);
      }
      return;
    }
    if (seat < 0) {
      sendLobby(socket, { type: 'error', error: '请先选择 P 位' });
      return;
    }
    const occupant = room.lobby.seats[seat];
    if (message.type === 'timing-settings') {
      const timing = multiplayerInputTimingPolicy(room.multiplayer), choice = message.timingChoice;
      if (seat !== 0 || room.lobby.phase !== 'lobby' || !room.multiplayer?.inputTiming ||
          typeof message.rollbackEnabled !== 'boolean' || (message.rollbackEnabled && !timing.rollback) ||
          !(choice === 'auto' || (Number.isInteger(choice) && choice >= 0 && choice <= timing.manualDelayLimit))) {
        sendLobby(socket, {type: 'error', error: 'invalid timing settings'}); return;
      }
      const mode = message.rollbackEnabled ? 0 : 1;
      if (room.lobby.adonisMode !== mode || room.lobby.timingChoice !== choice) {
        invalidateLobbyReady(room);
        room.lobby.adonisMode = mode;
        room.lobby.timingChoice = choice;
        room.lobby.inputDelay = choice === 'auto' || message.rollbackEnabled ? 0 : choice;
        room.lobby.inputDelayAuto = !message.rollbackEnabled && timing.measuredStartup && choice === 'auto';
        room.lobby.timing = null;
        broadcastLobby(room);
      }
      return;
    }
    if (message.type === 'th09-match') {
      const changed = room.id.startsWith('th19mp-')
        ? acceptTh19MatchStats(room, clientId, message)
        : room.lobby.phase === 'running' &&
          acceptTh09MatchStats(room, room.runs.get(String(message.serial)), clientId, message);
      if (changed) broadcastLobby(room);
      return;
    }

    if (message.type === 'resource-progress') {
      if (
        !['preparing', 'ready', 'failed', 'cancelled', 'importing'].includes(message.status) ||
        !['package', 'runtime'].includes(message.stage)
      )
        return;
      const rawPercent = message.percent === null ? null : Number(message.percent);
      if (
        rawPercent !== null &&
        (!Number.isFinite(rawPercent) || rawPercent < 0 || rawPercent > 100)
      )
        return;
      const resource = {
        ...Object.fromEntries(['basePercent', 'runtimePercent'].filter(key => key in message).map(key => [key,
          typeof message[key] === 'number' && Number.isFinite(message[key]) && message[key] >= 0 && message[key] <= 100 ? Math.round(message[key]) : null])),
        status: message.status,
        stage: message.stage,
        percent:
          message.status === 'ready' ? 100 : rawPercent === null ? null : Math.round(rawPercent),
      };
      if (JSON.stringify(occupant.resource) !== JSON.stringify(resource)) {
        if (room.lobby.phase === 'lobby' && resource.status !== 'ready' &&
            (occupant.resource?.status === 'ready' || occupant.ready)) invalidateLobbyReady(room);
        occupant.resource = resource;
        broadcastLobby(room);
      }
      return;
    }

    if (message.type === 'move-player-to-spectator') {
      const targetSeat = Number(message.seat);
      const target = room.lobby.seats[targetSeat];
      if (
        seat !== 0 || room.lobby.phase !== 'lobby' ||
        !Number.isInteger(targetSeat) || targetSeat < 1 ||
        targetSeat >= room.lobby.playerCount || !target ||
        target.clientId !== message.clientId ||
        room.lobbyClients.get(target.clientId)?.readyState !== WebSocket.OPEN
      ) {
        sendLobby(socket, { type: 'error', error: '无法将该玩家移到观众席，请刷新房间状态后重试。' });
        return;
      }
      clearLobbySeat(room, target.clientId);
      room.lobby.spectators.set(target.clientId, target.name || '');
      invalidateLobbyReady(room);
      broadcastLobby(room);
      return;
    }
    if (message.type === 'remove-player') {
      const targetSeat = Number(message.seat);
      const target = room.lobby.seats[targetSeat];
      if (
        seat !== 0 ||
        room.lobby.phase !== 'lobby' ||
        !Number.isInteger(targetSeat) ||
        targetSeat < 1 ||
        targetSeat >= room.lobby.playerCount ||
        !target ||
        target.clientId !== message.clientId ||
        !roomDirectory.evict(roomId, target.clientId)
      ) {
        sendLobby(socket, { type: 'error', error: '无法移除该玩家，请刷新房间状态后重试。' });
      }
      return;
    }
    if (message.type === 'remove-spectator') {
      if (
        seat !== 0 ||
        room.lobby.phase !== 'lobby' ||
        typeof message.clientId !== 'string' ||
        !room.lobby.spectators.has(message.clientId) ||
        !roomDirectory.evict(roomId, message.clientId)
      ) {
        sendLobby(socket, { type: 'error', error: '无法移除该旁观者，请刷新房间状态后重试。' });
      }
      return;
    }

    if (
      room.lobby.phase !== 'lobby' &&
      ['set-loadout', 'set-ready', 'settings', 'movement'].includes(message.type)
    ) {
      sendLobby(socket, { type: 'error', error: '本局已经开始' });
      return;
    }

    if (message.type === 'set-loadout') {
      const loadout = Number(message.loadout);
      if (!validLoadout(room, loadout)) {
        sendLobby(socket, { type: 'error', error: 'invalid loadout' });
        return;
      }
      if (occupant.loadout !== loadout) {
        invalidateLobbyReady(room);
        occupant.loadout = loadout;
      }
      broadcastLobby(room);
      return;
    }
    if (message.type === 'movement') {
      if (
        occupant.movementMode !== message.movementMode ||
        occupant.touchEnabled !== message.touchEnabled ||
        occupant.mobileDevice !== (message.mobileDevice === true)
      ) {
        occupant.movementMode = message.movementMode;
        occupant.touchEnabled =
          typeof message.touchEnabled === 'boolean' ? message.touchEnabled : undefined;
        occupant.mobileDevice = message.mobileDevice === true;
        occupant.ready = false;
        occupant.readyVersion = 0;
        broadcastLobby(room);
      }
      return;
    }
    if (message.type === 'set-ready') {
      // Versioned clients opt this room into exact immutable-program agreement.
      // An old client with no identity cannot join a versioned start.
      const generation = message.runtimeGeneration;
      const versioned = generation !== undefined || room.lobby.seats.some(entry => entry?.runtimeGeneration);
      if (message.ready && versioned &&
          (typeof generation !== 'string' || !/^[a-f0-9]{64}$/.test(generation) ||
           room.lobby.seats.some(entry => entry && entry !== occupant && entry.ready && entry.runtimeGeneration !== generation))) {
        occupant.ready = false;
        occupant.readyVersion = 0;
        sendLobby(socket, {type:'error', code:'runtime-version', error:'联机程序版本不一致，请双方取消准备后重新准备，旧版页面请刷新。'});
        sendLobby(socket, {type:'state', room:lobbySnapshot(room)});
        broadcastLobby(room);
        return;
      }
      if (message.ready && occupant.resource && occupant.resource.status !== 'ready') {
        sendLobby(socket, {type: 'state', room: lobbySnapshot(room)});
        return;
      }
      if (
        room.lobby.disableCheatMovement &&
        message.ready &&
        !['touch', 'joystick', 'joystick-free'].includes(message.movementMode)
      ) {
        sendLobby(socket, {
          type: 'error',
          code: 'movement-policy',
          seat,
          error: '必须调整你的作弊移动方式',
        });
        return;
      }
      occupant.movementMode = message.movementMode;
      occupant.touchEnabled =
        typeof message.touchEnabled === 'boolean' ? message.touchEnabled : undefined;
      occupant.mobileDevice = message.mobileDevice === true;
      const wasReady = occupant.ready && occupant.readyVersion === room.lobby.settingsVersion;
      occupant.ready = !!message.ready;
      if (message.ready) occupant.runtimeGeneration = generation;
      occupant.readyVersion = occupant.ready ? room.lobby.settingsVersion : 0;
      broadcastLobby(room);
      if (occupant.ready && !wasReady) sendRoomMessage(room, JSON.stringify({type: 'room-action', action: 'ready'}));
      return;
    }
    if (message.type === 'settings') {
      if (seat !== 0) {
        sendLobby(socket, { type: 'error', error: '只有 P1 可以修改房间设置' });
        return;
      }
      if (room.lobby.phase !== 'lobby') {
        sendLobby(socket, { type: 'state', room: lobbySnapshot(room) });
        return;
      }
      if(message.th19Rules!==undefined&&(!room.id.startsWith('th19mp-')||!validTh19VersusRules(message.th19Rules))){sendLobby(socket,{type:'error',error:'invalid TH19 versus rules'});return;}
      const th19Rules=message.th19Rules??room.lobby.th19Rules;
      const playerCount = Number(message.playerCount);
      if (!validPlayerCount(room.multiplayer, playerCount)) {
        sendLobby(socket, { type: 'error', error: 'invalid player count' });
        return;
      }
      const difficultyMax = Math.max(0, (room.multiplayer?.difficulties?.length ?? 6) - 1);
      const difficulty = Math.max(0, Math.min(difficultyMax, Number(message.difficulty) || 0));
      if(message.initialSpares!==undefined&&(!room.id.startsWith('th05mp-')||!validTh05InitialSpares(message.initialSpares))||message.initialBombs!==undefined&&(!room.id.startsWith('th05mp-')||!validTh05InitialBombs(message.initialBombs))){sendLobby(socket,{type:'error',error:'invalid TH05 initial stocks'});return;}
      const initialSpares=message.initialSpares??room.lobby.initialSpares,initialBombs=message.initialBombs??room.lobby.initialBombs;
      if (
        message.chapterThreshold !== undefined &&
        (!room.th15 || !validTh15ChapterThreshold(message.chapterThreshold))
      ) {
        sendLobby(socket, {
          type: "error",
          error: "invalid chapter reward threshold",
        });
        return;
      }
      const chapterThreshold = room.th15
        ? (message.chapterThreshold ??
          (playerCount !== room.lobby.playerCount
            ? defaultTh15ChapterThreshold(playerCount)
            : room.lobby.chapterThreshold))
        : undefined;
      const visibility =
        message.visibility === 'private' || message.visibility === 'public'
          ? message.visibility
          : room.lobby.visibility;
      const disableCheatMovement =
        typeof message.disableCheatMovement === 'boolean'
          ? message.disableCheatMovement
          : room.lobby.disableCheatMovement;
      if(message.disableLocalChargePriority !== undefined &&
         (!room.id.startsWith('th09mp-') || typeof message.disableLocalChargePriority !== 'boolean')){
        sendLobby(socket,{type:'error',error:'invalid local charge priority setting'});return;
      }
      const disableLocalChargePriority=message.disableLocalChargePriority ?? room.lobby.disableLocalChargePriority;
      const modesSupported = room.multiplayer?.gameplay === 'cooperative';
      const challengeMode =
        modesSupported &&
        (typeof message.challengeMode === 'boolean'
          ? message.challengeMode
          : room.lobby.challengeMode);
      // Prank mode is temporarily unavailable for every product.
      const prankMode = false;
      room.lobby.visibility = visibility;
      if (
        (room.id.startsWith('th19mp-')&&JSON.stringify(room.lobby.th19Rules)!==JSON.stringify(th19Rules)) ||
        room.lobby.playerCount !== playerCount ||
        room.lobby.difficulty !== difficulty ||
        room.lobby.disableCheatMovement !== disableCheatMovement ||
        room.lobby.disableLocalChargePriority !== disableLocalChargePriority ||
        room.lobby.challengeMode !== challengeMode ||
        room.lobby.prankMode !== prankMode ||
        room.lobby.initialSpares !== initialSpares || room.lobby.initialBombs !== initialBombs ||
        (room.th15 && room.lobby.chapterThreshold !== chapterThreshold)
      ) {
        invalidateLobbyReady(room);
        if(room.id.startsWith('th19mp-'))room.lobby.th19Rules=[...th19Rules];
        room.lobby.disableCheatMovement = disableCheatMovement;
        if(room.id.startsWith('th09mp-'))room.lobby.disableLocalChargePriority=disableLocalChargePriority;
        room.lobby.challengeMode = challengeMode;
        room.lobby.prankMode = prankMode;
        if(room.id.startsWith('th05mp-')){room.lobby.initialSpares=initialSpares;room.lobby.initialBombs=initialBombs;}
        if (room.th15) room.lobby.chapterThreshold = chapterThreshold;
        room.lobby.playerCount = playerCount;
        room.lobby.difficulty = difficulty;
        for (let index = playerCount; index < room.lobby.seats.length; index++)
          room.lobby.seats[index] = null;
      }
      broadcastLobby(room);
      return;
    }
    if (message.type === 'timing-result') {
      const timing = parseMeasuredNetplayTiming(message.timing);
      const timingPolicy = multiplayerInputTimingPolicy(room.multiplayer);
      if (
        seat !== 0 ||
        !timingPolicy.measuredStartup ||
        (!timingPolicy.rollback && timing?.adonisMode !== 1) ||
        room.lobby.phase === 'lobby' ||
        message.serial !== room.lobby.startSerial ||
        !timing ||
        timing.route === 'spectator' ||
        timing.adonisMode !== room.lobby.adonisMode ||
        timing.automatic !== room.lobby.inputDelayAuto ||
        timing.predictionReserve !==
          (timing.adonisMode === 2
            ? resolveAdonisPredictionReserve(
                timing.fullDelay,
                room.lobby.predictionReserve,
                timing.automatic,
              )
            : 0) ||
        (!timing.automatic && timing.inputDelay !== room.lobby.inputDelay)
      ) {
        sendLobby(socket, { type: 'error', error: 'invalid measured timing result' });
        return;
      }
      if (room.lobby.timing && JSON.stringify(room.lobby.timing) !== JSON.stringify(timing)) {
        sendLobby(socket, { type: 'error', error: 'measured timing is immutable for this run' });
        return;
      }
      // Display only: native peers have already committed this choice. This
      // message neither configures their cores nor changes a live D queue.
      room.lobby.timing = timing;
      room.lobby.inputDelay = timing.inputDelay;
      broadcastLobby(room);
      return;
    }
    if (message.type === 'start') {
      if (seat !== 0) {
        sendLobby(socket, { type: 'error', error: '只有 P1 可以开始游戏' });
        return;
      }
      if (room.lobby.phase !== 'lobby') {
        sendLobby(socket, { type: 'state', room: lobbySnapshot(room) });
        return;
      }
      const activeSeats = room.lobby.seats.slice(0, room.lobby.playerCount);
      const runtimeGeneration = activeSeats.find(entry => entry?.runtimeGeneration)?.runtimeGeneration;
      if (runtimeGeneration && activeSeats.some(entry => entry?.runtimeGeneration !== runtimeGeneration)) {
        sendLobby(socket, {type:'error', code:'runtime-version', error:'联机程序版本不一致，请双方取消准备后重新准备，旧版页面请刷新。'});
        return;
      }
      if (
        room.lobby.disableCheatMovement &&
        activeSeats.some(
          (entry) => entry && !['touch', 'joystick', 'joystick-free'].includes(entry.movementMode),
        )
      ) {
        sendLobby(socket, { type: 'error', error: '仍有玩家需要调整作弊移动方式' });
        return;
      }
      if (
        activeSeats.some(
          (entry) =>
            !entry ||
            !room.lobbyClients.has(entry.clientId) ||
            !entry.ready ||
            (entry.resource && entry.resource.status !== 'ready') ||
            entry.readyVersion !== room.lobby.settingsVersion,
        )
      ) {
        sendLobby(socket, { type: 'error', error: '仍有玩家未在线、未入座或未对当前设置准备' });
        return;
      }
      const inputDelay = message.inputDelay === undefined ? 0 : Number(message.inputDelay);
      const adonisMode = message.adonisMode === undefined ? 0 : Number(message.adonisMode);
      const inputDelayAuto = message.inputDelayAuto ?? false;
      const predictionReserve = message.predictionReserve ?? 2;
      const timingPolicy = multiplayerInputTimingPolicy(room.multiplayer);
      const adonisSupported = timingPolicy.measuredStartup;
      const configurablePredictionLimit = room.multiplayer?.inputTiming?.sendPredictionLimit;
      const predictionLimit =
        configurablePredictionLimit != null
          ? message.predictionLimit === undefined
            ? configurablePredictionLimit
            : Number(message.predictionLimit)
          : room.lobby.predictionLimit;
      if (
        !Number.isInteger(adonisMode) ||
        adonisMode < 0 ||
        adonisMode > 2 ||
        typeof inputDelayAuto !== 'boolean' ||
        (inputDelayAuto && (!adonisSupported || !adonisMode || inputDelay !== 0)) ||
        !Number.isInteger(predictionReserve) ||
        predictionReserve < 1 ||
        predictionReserve > 2 ||
        (message.predictionReserve !== undefined && !adonisSupported) ||
        (adonisMode === 1 && !room.multiplayer?.inputTiming) ||
        (adonisMode === 2 && !adonisSupported) ||
        (!timingPolicy.rollback &&
          (adonisMode !== 1 ||
            (message.predictionLimit !== undefined && message.predictionLimit !== 0))) ||
        !Number.isInteger(inputDelay) ||
        inputDelay < 0 ||
        inputDelay > timingPolicy.manualDelayLimit ||
        (configurablePredictionLimit != null &&
          (!Number.isInteger(predictionLimit) ||
            predictionLimit < (timingPolicy.rollback ? 1 : 0) ||
            predictionLimit > configurablePredictionLimit))
      ) {
        sendLobby(socket, { type: 'error', error: 'invalid input timing' });
        return;
      }
      const rollback = !!room.multiplayer && timingPolicy.rollback && adonisMode !== 1;
      room.lobby.inputDelay = rollback ? 0 : inputDelay;
      room.lobby.adonisMode = rollback ? 0 : adonisMode;
      room.lobby.inputDelayAuto = rollback ? false : inputDelayAuto;
      room.lobby.predictionReserve = predictionReserve;
      room.lobby.timing = null;
      if (room.th15) room.lobby.predictionLimit = rollback ? 12 : 0;
      else if (configurablePredictionLimit != null) room.lobby.predictionLimit = predictionLimit;
      room.lobby.phase = 'starting';
      room.lobby.startedAt = Date.now();
      room.lobby.startSerial++;
      const run = getRun(room, String(room.lobby.startSerial));
      run.lobbyClientIds = activeSeats.map((entry) => entry.clientId);
      bindTh09Duel(room, run);
      const seatedClients = new Set(activeSeats.map((entry) => entry.clientId));
      run.playerCount = room.lobby.playerCount;
      run.runtimeGeneration = runtimeGeneration;
      run.admittedSpectators = new Set(
        [...room.lobby.spectators.keys()].filter(
          (id) => room.lobbyClients.has(id) && !seatedClients.has(id),
        ),
      );
      // Bind this run to the admitted members, not reusable public client IDs.
      run.memberIds = new Map(
        [...seatedClients, ...run.admittedSpectators].map((id) => [
          id,
          roomDirectory.memberIdFor(roomId, id),
        ]),
      );
      clearSpectatorHistory(run);
      startSpectatorGrace(roomId, room, String(room.lobby.startSerial), run);
      const snapshot = lobbySnapshot(room);
      snapshot.spectatorCount = run.admittedSpectators.size;
      broadcastLobby(room, { type: 'start', serial: room.lobby.startSerial, room: snapshot });
      return;
    }
    sendLobby(socket, { type: 'error', error: 'unknown lobby message' });
  });

  socket.on('close', (code, reason) => {
    if (room.lobbyClients.get(clientId) === socket) {
      room.lobbyClients.delete(clientId);
      listenTogether?.disconnect(room, clientId);
      const spectatorChanged = room.lobby.spectators.delete(clientId);
      const deliberateLeave = code === 1000 && String(reason || '') === 'leave room';
      if (deliberateLeave) {
        roomDirectory.depart(clientId, socket);
        const seatChanged = clearLobbySeat(room, clientId);
        if (seatChanged) invalidateLobbyReady(room);
        if (seatChanged || spectatorChanged) broadcastLobby(room);
      } else if (lobbySeatOf(room, clientId) >= 0) {
        reserveRoomRole(room, clientId);
        // Keep the seat visible during a short network transition. Clients can
        // show it as reconnecting instead of presenting an empty room.
        broadcastLobby(room);
      } else if (spectatorChanged) {
        broadcastLobby(room);
      }
    }
    console.log(`LOBBY LEAVE room=${roomId} client=${clientId}`);
    maybeDeleteRoom(roomId, room);
  });
  socket.on('error', (error) => {
    console.error(`LOBBY SOCKET room=${roomId} client=${clientId} ${error.message}`);
  });
}

function handleSpectatorConnection(socket, roomId, runId, spectatorId, playerCount) {
  const room = rooms.get(roomId);
  const run = room?.runs.get(runId);
  if (
    !room ||
    !run ||
    run.spectatorHistoryUnavailable ||
    run.playerCount !== playerCount ||
    !run.admittedSpectators.has(spectatorId)
  ) {
    socket.close(1008, 'spectator was not admitted within the join window');
    return;
  }
  if (run.claimedSpectators.has(spectatorId)) {
    socket.close(1008, 'spectators cannot reconnect midgame');
    return;
  }
  run.claimedSpectators.add(spectatorId);
  run.spectatorClients.set(spectatorId, socket);
  if (run.spectatorSession) sendSignal(socket, run.spectatorSession);
  sendSpectatorReplayFile(socket,run);
  if (run.spectatorTimeline) {
    // Ordered bounded replay, then live delivery. Live packets remain in the
    // same retained tape until the replay cursor catches its append frontier.
    socket.spectatorReplaying = true;
    const replayEpoch = socket.spectatorReplayEpoch || 0;
    let index = 0,
      blockedAt = 0,
      history = run.spectatorHistory;
    const replay = () => {
      if (
        socket.readyState !== WebSocket.OPEN ||
        (socket.spectatorReplayEpoch || 0) !== replayEpoch ||
        run.spectatorStopped ||
        run.spectatorClients.get(spectatorId) !== socket
      )
        return;
      if (socket.bufferedAmount > 64 * 1024) {
        blockedAt ||= Date.now();
        if (Date.now() - blockedAt > 15000) {
          socket.close(1008, 'spectator history too slow');
          return;
        }
        socket.spectatorReplayTimer = setTimeout(replay, 10);
        return;
      }
      blockedAt = 0;
      if (index >= history.length && history !== run.spectatorHistory) {
        history = run.spectatorStages ? socket.spectatorQueuedStages?.shift() : run.spectatorHistory;
        if (!history) {socket.close(1011, 'spectator stage history gap'); return;}
        index = 0;
      }
      const first = history[index];
      if (!first) {
        if (run.spectatorStages && !sendSpectatorPayload(socket, run.spectatorStages.bootstrap(), roomId, runId, spectatorId)) return;
        socket.spectatorReplaying = false;
        return;
      }
      if (run.spectatorStages) {
        // Keep legacy input packets individually decodable by native peers.
        // Yield after a bounded burst instead of one timer per logical frame.
        for (let sent = 0; sent < 64 && history[index] && socket.bufferedAmount <= 64 * 1024; ++sent)
          if (!sendSpectatorPayload(socket, history[index++], roomId, runId, spectatorId)) return;
        socket.spectatorReplayTimer = setTimeout(replay, 0);
        return;
      }
      const batch = [first];
      index++;
      if (first.length === 46 || first.length === 126)
        while (batch.length < 64 && history[index]?.length === first.length) batch.push(history[index++]);
      if (!sendSpectatorPayload(socket, Buffer.concat(batch), roomId, runId, spectatorId)) return;
      socket.spectatorReplayTimer = setTimeout(replay, 0);
    };
    replay();
  } else
    for (const payload of run.spectatorHistory) {
      if (!sendSpectatorPayload(socket, payload, roomId, runId, spectatorId)) break;
    }
  if (!hasPendingSpectators(run)) clearSpectatorHistory(run);
  console.log(`SPECTATOR JOIN room=${roomId} run=${runId} client=${spectatorId}`);
  socket.on('message', () => socket.close(1008, 'spectators are receive-only'));
  socket.on('close', () => {
    if (socket.spectatorReplayTimer) clearTimeout(socket.spectatorReplayTimer);
    if (run.spectatorTimeline && run.spectatorClients.get(spectatorId) === socket)
      run.claimedSpectators.delete(spectatorId);
    if (run.spectatorClients.get(spectatorId) === socket) run.spectatorClients.delete(spectatorId);
    console.log(`SPECTATOR LEAVE room=${roomId} run=${runId} client=${spectatorId}`);
    maybeDeleteRun(room, runId, run);
    maybeDeleteRoom(roomId, room);
  });
  socket.on('error', (error) => {
    console.error(
      `SPECTATOR SOCKET room=${roomId} run=${runId} client=${spectatorId} ${error.message}`,
    );
  });
}

function releaseMemberTransports(roomId, clientId) {
  const room = rooms.get(roomId);
  if (!room) return;
  for (const run of room.runs.values()) {
    const player = run.lobbyClientIds?.indexOf(clientId) ?? -1;
    if (player >= 0) {
      (run.releasedPlayers ??= new Set()).add(player);
      run.clients.get(player)?.close(4008, 'membership released');
      run.signalClients.get(player)?.close(4008, 'membership released');
    }
    run.admittedSpectators.delete(clientId);
    run.spectatorClients.get(clientId)?.close(4008, 'membership released');
  }
}

const server = new WebSocketServer({
  host,
  port,
  perMessageDeflate: false,
  maxPayload: RELAY_LIMITS.messageBytes,
  verifyClient(info, done) {
    const allowed = abuseGuard.allowHandshake(
      relayClientAddress(info.req, abuseConfig.trustedProxies),
    );
    done(allowed, allowed ? undefined : 429, allowed ? undefined : 'Too Many Requests');
  },
});
const abuseMaintenance = setInterval(() => abuseGuard.sweep(), 60_000);
abuseMaintenance.unref();
const roomDirectory = createRoomDirectory({
  publicNames: !!nicknameService,
  publicTrack: listenTogether?.publicTrack,
  rooms,
  clearSeat: clearLobbySeat,
  invalidateReady: invalidateLobbyReady,
  broadcast: broadcastLobby,
  maybeDelete: maybeDeleteRoom,
  releaseTransports: releaseMemberTransports,
});
server.on('close', () => {
  void nicknameService?.close();
  roomDirectory.close();
  clearInterval(abuseMaintenance);
  for (const room of rooms.values()) {
    lobbyBroadcaster.clear(room);
    listenTogether?.close(room);
  }
});

server.on('connection', (socket, request) => {
  socket.relayAddress = relayClientAddress(request, abuseConfig.trustedProxies);
  if (!abuseGuard.trackSocket(socket.relayAddress, socket)) {
    socket.close(4008, 'connection limit');
    return;
  }
  roomDirectory.track(socket);
  const url = new URL(request.url || '/', `ws://${request.headers.host || 'localhost'}`);
  const memberId = url.searchParams.get('member') || '';
  if (url.searchParams.get('directory') === '1') {
    roomDirectory.connect(socket, /^[A-Za-z0-9_-]{8,64}$/.test(memberId) ? memberId : '');
    return;
  }
  if (url.searchParams.get('diagnostic') === '1') {
    handleDiagnosticConnection(socket);
    return;
  }
  const roomId = url.searchParams.get('room') || '';
  const roomCode = numericRoomCode(roomId);
  if (/^[A-Za-z0-9_-]{1,64}$/.test(roomId) &&
      !abuseGuard.allowRoomLookup(socket.relayAddress, roomCode || roomId)) {
    sendLobby(socket, { type: 'error', code: 'rate-limited', error: '房间查询过于频繁，请稍后再试。' });
    socket.close(1008, 'room lookup rate exceeded');
    return;
  }
  const runId = url.searchParams.get('run') || '0';
  const lobbyClient = url.searchParams.get('lobby') || '';
  if (/^[A-Za-z0-9_-]{1,64}$/.test(roomId) && /^[A-Za-z0-9_-]{8,64}$/.test(lobbyClient)) {
    if (!/^[A-Za-z0-9_-]{8,64}$/.test(memberId) || memberId === lobbyClient) {
      socket.close(1008, 'private member identity required');
      return;
    }
    handleLobbyConnection(socket, roomId, lobbyClient, memberId, url.searchParams.get('intent'), {
      visibility: url.searchParams.get('visibility'),
      disableCheatMovement: url.searchParams.get('disableCheatMovement') === '1',
      disableLocalChargePriority: url.searchParams.get('disableLocalChargePriority') === '1',
      th19Rules: url.searchParams.has('th19Rules') ? (()=>{try{return JSON.parse(url.searchParams.get('th19Rules'));}catch{return null;}})() : undefined,
      chapterThreshold: url.searchParams.has('chapterThreshold')
        ? Number(url.searchParams.get('chapterThreshold'))
        : undefined,
      challengeMode: url.searchParams.get('challengeMode') === '1',
      prankMode: url.searchParams.get('prankMode') === '1',
      playerCount: Number(url.searchParams.get('players')),
      difficulty: url.searchParams.has('difficulty')
        ? Number(url.searchParams.get('difficulty'))
        : undefined,
    });
    return;
  }
  if (roomCodes.has(roomCode) && roomCodes.get(roomCode) !== roomId) {
    socket.close(1008, 'room product changed');
    return;
  }
  const existingRoom = rooms.get(roomId);
  const existingRun = existingRoom?.runs.get(runId);
  if (existingRun?.runtimeGeneration && url.searchParams.get('runtimeGeneration') !== existingRun.runtimeGeneration) {
    socket.close(1008, 'runtime version mismatch');
    return;
  }
  if (
    existingRun?.memberIds ||
    existingRoom?.lobbyClients.size ||
    existingRoom?.lobby.seats.some(Boolean)
  ) {
    const clientId =
      url.searchParams.get('spectator') ||
      existingRun?.lobbyClientIds?.[Number(url.searchParams.get('player'))];
    if (
      !memberId ||
      !clientId ||
      existingRun?.memberIds?.get(clientId) !== memberId ||
      roomDirectory.memberIdFor(roomId, clientId) !== memberId
    ) {
      socket.close(1008, 'run membership required');
      return;
    }
  }
  const spectator = url.searchParams.get('spectator') || '';
  const multiplayer = multiplayerPolicyForRoomId(roomId);
  const defaultCount = defaultPlayerCount(multiplayer);
  const spectatorPlayerCount = Number.parseInt(
    url.searchParams.get('players') || String(defaultCount),
    10,
  );
  if (spectator) {
    if (
      !/^[A-Za-z0-9_-]{1,64}$/.test(roomId) ||
      !/^[A-Za-z0-9_-]{1,64}$/.test(runId) ||
      !/^[A-Za-z0-9_-]{8,64}$/.test(spectator) ||
      !validPlayerCount(multiplayer, spectatorPlayerCount)
    ) {
      socket.close(1008, 'invalid spectator');
      return;
    }
    handleSpectatorConnection(socket, roomId, runId, spectator, spectatorPlayerCount);
    return;
  }
  const player = Number.parseInt(url.searchParams.get('player') || '-1', 10);
  const playerCount = Number.parseInt(url.searchParams.get('players') || String(defaultCount), 10);
  const signaling = url.searchParams.get('signal') === '1';
  if (
    !/^[A-Za-z0-9_-]{1,64}$/.test(roomId) ||
    !/^[A-Za-z0-9_-]{1,64}$/.test(runId) ||
    !Number.isInteger(player) ||
    player < 0 ||
    player >= playerCount ||
    !validPlayerCount(multiplayer, playerCount)
  ) {
    socket.close(1008, 'invalid room or player');
    return;
  }

  if (signaling) {
    handleSignalConnection(socket, roomId, runId, player, playerCount);
    return;
  }

  const room = getRoom(roomId, socket);
  if (!room) return;
  const run = getRun(room, runId);
  if (run.ended) {
    socket.close(1008, 'run already ended');
    return;
  }
  if (run.clients.has(player)) {
    socket.close(1008, 'player slot already occupied');
    return;
  }
  if (run.releasedPlayers?.has(player)) {
    socket.close(4008, 'membership released');
    return;
  }
  if (run.playerCount && run.playerCount !== playerCount) {
    socket.close(1008, 'player count mismatch');
    return;
  }
  // Relay fallback must establish its own stream size without signaling.
  run.playerCount = playerCount;
  run.clients.set(player, socket);
  if (roomId.startsWith('th09mp-') && run.memberIds && url.searchParams.get('th09MatchReport') === '1')
    sendSignal(socket, {type:'th09-match-report'});
  console.log(`JOIN room=${roomId} run=${runId} player=${player} peers=${run.clients.size}`);
  maybeResolveRoute(roomId, runId, room, run);

  const acceptGameplay = createRelayMessageGate({
    maxBytes: RELAY_LIMITS.gameplayBytes,
    messagesPerSecond: 600,
    bytesPerSecond: 512 * 1024,
  });
  // Sparse cooperative stage owners can exceed the ordinary 16 KiB input
  // packet cap. Their optional lane has its own bounded byte/message budget.
  const acceptStageInitializer = createRelayMessageGate({
    maxBytes: RELAY_LIMITS.messageBytes,
    messagesPerSecond: 10,
    bytesPerSecond: 256 * 1024,
  });
  const acceptReplayFile=createRelayMessageGate({maxBytes:32788,messagesPerSecond:120,bytesPerSecond:4*1024*1024});
  socket.on('message', (data, isBinary) => {
    if(isBinary&&data[0]===0xe9){
      if(player!==0||run.clients.get(player)!==socket||!run.spectatorNativeReplay)return;
      if(acceptReplayFile(data.byteLength))return;
      const replay=run.spectatorNativeReplay;
      if(replay.accept(data)&&replay.complete){
        for(const viewer of run.spectatorClients.values())sendSpectatorReplayFile(viewer,run);
      }
      return;
    }
    const marker = COOPERATIVE_SPECTATOR_MARKERS[roomId.split('-')[0]];
    const stageInitializer = isBinary && player === 0 && marker !== undefined &&
      data.length >= 9 && data[0] === 0xe8 && data[1] === 69 && data[2] === marker &&
      data[3] === 83 && data[4] === 86 && data[5] === 1 && data[6] === 1;
    if (!acceptRelayMessage(socket, data, stageInitializer ? acceptStageInitializer : acceptGameplay)) return;
    if (!isBinary) {
      let message; try { message = JSON.parse(String(data)); } catch {}
      if (roomId.startsWith('th09mp-') && message?.type === 'th09-match') {
        if (run.clients.get(player) === socket) acceptTh09PlayerReport(room, run, runId, player, message);
        return;
      }
      if (player === 0 && run.clients.get(player) === socket && message?.type === 'spectator-session' &&
          beginCooperativeSpectatorSession(roomId, runId, room, run, message)) return;
      socket.close(1003, 'binary frames only');
      return;
    }
    const incoming = data;
    if (incoming.length >= 2 && incoming[0] === 0xe8) {
      if (
        player === 0 &&
        run.clients.get(player) === socket &&
        incoming.equals(Buffer.from([0xe8, 0x53, 0x54, 0x4f, 0x50, 1]))
      ) {
        stopHostSpectatorStream(roomId, runId, run, 'host-upload');
        return;
      }
      if (run.spectatorStopped) return;
      if (player !== 0) return;
      const payload = incoming.subarray(1);
      if(acceptTh19SpectatorPayload(roomId,runId,room,run,payload))return;
      if (acceptCooperativeSpectatorPayload(roomId, runId, room, run, payload)) return;
      if (!run.spectatorAdmissionOpen && run.spectatorClients.size === 0 && !run.spectatorHistoryUnavailable) return;
      const th09Stream = roomId.startsWith('th09mp-') && run.playerCount === 2;
      const ending =
        th09Stream &&
        payload.length === 24 &&
        [84, 57, 83, 69, 1, 0, 2, 0].every((v, i) => payload[i] === v);
      const beginning =
        th09Stream &&
        ((payload.length === 74 && payload[4] === 1) || (payload.length === 102 && payload[4] === 2)) &&
        [84, 57, 66, 71].every((v, i) => payload[i] === v) &&
        payload[5] === 0 && payload[6] === 2 && payload[7] === 0;
      const roundStart =
        th09Stream &&
        payload.length === 28 &&
        [84, 57, 82, 68, 1, 0, 2, 0].every((v, i) => payload[i] === v);
      if (
        !ending &&
        !beginning &&
        !roundStart &&
        !isSpectatorFrameForRoom(roomId, payload, run.playerCount)
      )
        return;
      if (run.spectatorTimeline && payload.length === 40)
        run.spectatorTiming = Buffer.from(payload);
      if (beginning) {
        const frame = payload.readUInt32LE(8),
          build = payload.subarray(12, 24);
        if (
          frame < (run.spectatorNextFrame || 0) ||
          (run.spectatorBuild && !run.spectatorComplete && frame !== (run.spectatorNextFrame || 0)) ||
          (run.spectatorBuild && !run.spectatorBuild.equals(build))
        ) {
          stopSpectatorStream(run);
          return;
        }
        run.spectatorBuild ||= Buffer.from(build);
        run.spectatorNextFrame = frame;
        run.spectatorRound = 0;
        run.spectatorComplete = false;
        // A complete initializer permits bounded per-match history even when
        // the same player connections keep running through native Retry.
        run.spectatorHistory = run.spectatorTiming ? [run.spectatorTiming] : [];
        run.spectatorHistoryBytes = run.spectatorTiming?.length || 0;
        if (run.spectatorHistoryUnavailable) {
          run.spectatorHistoryUnavailable = false;
          run.spectatorAdmissionOpen = true;
          run.onSpectatorAvailabilityChange?.();
        }
      }
      if (th09Stream && (payload.length === 46 || payload.length === 126 || ending || roundStart)) {
        const frame = payload.readUInt32LE(8),
          build = payload.subarray(12, 24);
        if (
          frame !== (run.spectatorNextFrame || 0) ||
          (run.spectatorBuild && !run.spectatorBuild.equals(build))
        ) {
          stopSpectatorStream(run);
          return;
        }
        run.spectatorBuild ||= Buffer.from(build);
        if (roundStart) {
          const round = payload.readUInt32LE(24);
          if (run.spectatorComplete || round <= (run.spectatorRound || 0) || round > 1024) {
            stopSpectatorStream(run);
            return;
          }
          run.spectatorRound = round;
        } else if (ending) run.spectatorComplete = true;
        else {
          if (run.spectatorComplete) return;
          run.spectatorNextFrame = frame + 1;
        }
      }
      if (
        run.spectatorAdmissionOpen &&
        (run.spectatorTimeline || hasPendingSpectators(run)) &&
        !appendSpectatorHistory(
          run,
          payload,
          run.spectatorTimeline
            ? TH09_SPECTATOR_HISTORY_LIMITS
            : RELAY_LIMITS,
          th09Stream && run.spectatorTimeline ? closeTh09HistoryAdmission : stopSpectatorStream,
        )
      ) {
        if (run.spectatorStopped) return;
      }
      for (const [spectatorId, target] of run.spectatorClients)
        if (!target.spectatorReplaying)
          sendSpectatorPayload(target, payload, roomId, runId, spectatorId);
      if (beginning)
        for (const id of room.lobby.spectators.keys()) {
          const lobby = room.lobbyClients.get(id);
          if (lobby)
            sendLobby(lobby, {
              type: 'spectator-start',
              serial: room.lobby.startSerial,
              room: lobbySnapshot(room),
            });
        }
      return;
    }
    if (!allowGameplayRelayFallback && !roomId.startsWith('th05mp-')) return;
    let requestedPeer = null;
    let forwardedPayload = incoming;
    if (incoming.length >= 2 && incoming[0] === targetedEnvelopeMarker) {
      requestedPeer = incoming[1];
      forwardedPayload = incoming.subarray(2);
      if (
        !Number.isInteger(requestedPeer) ||
        requestedPeer < 0 ||
        requestedPeer >= playerCount ||
        requestedPeer === player
      ) {
        socket.close(1008, 'invalid relay target');
        return;
      }
    }
    for (const [peer, target] of run.clients) {
      if (peer === player || target.readyState !== WebSocket.OPEN) continue;
      if (requestedPeer != null && peer !== requestedPeer) continue;
      const key = `${roomId}:${runId}:${player}->${peer}`;
      const sequence = (run.forwardCounters.get(key) || 0) + 1;
      run.forwardCounters.set(key, sequence);
      const looksLikeNetplayInput =
        forwardedPayload.length >= 6 &&
        forwardedPayload[0] === 0x45 &&
        forwardedPayload[2] === 0x4e &&
        forwardedPayload[3] === 0x50 &&
        forwardedPayload[5] === 1;
      if (dropFirstInputPerEdge && looksLikeNetplayInput && !run.firstInputDropped.has(key)) {
        run.firstInputDropped.add(key);
        continue;
      }
      if (
        looksLikeNetplayInput &&
        dropInputLatestFrom >= 0 &&
        dropInputLatestTo >= dropInputLatestFrom &&
        forwardedPayload.length >= 28
      ) {
        const latestFrame = forwardedPayload.readUInt32LE(24);
        if (latestFrame >= dropInputLatestFrom && latestFrame <= dropInputLatestTo) {
          const frameDropKey = `${key}:${latestFrame}`;
          if (!run.inputLatestDropped.has(frameDropKey)) {
            run.inputLatestDropped.add(frameDropKey);
            continue;
          }
        }
      }
      if (dropEvery > 0 && sequence % dropEvery === 0) continue;
      const spread = jitterMs > 0 ? ((sequence * 17) % (jitterMs * 2 + 1)) - jitterMs : 0;
      const wait = Math.max(0, delayMs + spread);
      sendBounded(target, relayGameplayPayload(multiplayer,player,forwardedPayload), { binary: true, delayMs: wait });
    }
  });

  socket.on('close', () => {
    removeClient(roomId, runId, player, socket);
    console.log(`LEAVE room=${roomId} run=${runId} player=${player}`);
  });
  socket.on('error', (error) => {
    console.error(`SOCKET room=${roomId} player=${player} ${error.message}`);
  });
});

server.on('listening', () => {
  const dropRange =
    dropInputLatestFrom >= 0 && dropInputLatestTo >= dropInputLatestFrom
      ? `${dropInputLatestFrom}-${dropInputLatestTo}`
      : 'off';
  console.log(
    `Eagler Touhou netplay relay listening ws://${host}:${server.address().port} delay=${delayMs} jitter=${jitterMs} dropEvery=${dropEvery} dropFirstInput=${dropFirstInputPerEdge ? 1 : 0} dropInputLatest=${dropRange}`,
  );
});

server.on('error', (error) => {
  console.error((error && error.stack) || error);
  process.exitCode = 1;
});
