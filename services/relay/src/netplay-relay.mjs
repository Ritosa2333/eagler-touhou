import {TH19_VERSUS_RULES,validTh19VersusRules} from '../../../common/contracts/th19-versus-rules.mjs';
import {Th19SpectatorHistory} from './th19-spectator.mjs';
import {createRemoteNicknameService} from './account-nickname-source.mjs';
import {createNicknameService} from './nickname-service.mjs';
import {roomAccount} from '../../../common/contracts/account-nickname-ticket.mjs';
import {createAccountChatSource} from './account-chat-source.mjs';
import {createTh09RoundOutbox} from './th09-round-outbox.mjs';
import {createNicknameRequestGuard,nicknameAddressScope} from './nickname-request-guard.mjs';
import {normalizePublicNickname} from '../../../common/contracts/public-nickname.mjs';
import {TH05_INITIAL_STOCK,validTh05InitialSpares,validTh05InitialBombs} from '../../../common/contracts/th05-initial-stock.mjs';
import {
  validTh15ChapterThreshold,
  defaultTh15ChapterThreshold,
} from "../../../common/contracts/th15-chapter-reward.mjs";
import { quickChatPhrase } from '../../../common/contracts/multiplayer-quick-chat.mjs';
import { createHmac, randomUUID } from 'node:crypto';
import {createServer} from 'node:http';
import {createNetEasePlaylistHandler} from './netease-playlist.mjs';
import { createTurnConfigReader } from './turn-config.mjs';
import { createNat4CandidateExchange, readNat4PortSteps } from './nat4-candidates.mjs';
import { allocateSwarmPort, createSwarmWorkers } from './swarm-workers.mjs';
import { WebSocket, WebSocketServer } from 'ws';
import { roomProbeEnvelope } from './room-probe-policy.mjs';
import { createSwarmStandbyPlanner, rankSwarmCandidates, readRouteProbeQuality, selectSwarmRoute } from './swarm-route-quality.mjs';
import { createRtcStartup } from './rtc-startup.mjs';
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
  restrictInitialIce,
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
import {isPc98SpectatorRoom,Pc98SpectatorRelay} from './pc98-spectator.mjs';
import {SpectatorReplayTransfer} from './spectator-replay.mjs';
import { th09PlayerStats, acceptTh09MatchStats, bindTh09Duel, reconcileTh09Duel } from './th09-room-stats.mjs';
import { acceptTh19MatchStats } from './th19-room-stats.mjs';
import {
  parseMeasuredNetplayTiming,
  resolveAdonisPredictionReserve,
} from '../../../common/contracts/netplay-timing.mjs';

const nicknameService = process.env.NICKNAME_BRIDGE_SECRET
  ? createRemoteNicknameService()
  : await createNicknameService();
const ticketKey = process.env.ACCOUNT_RELAY_SECRET;
if (ticketKey && ticketKey.length < 32) throw Error('Invalid ACCOUNT_RELAY_SECRET');
const nicknameProfiles = new Map(), nicknameRequests = new Map();
const allowNicknameRequest = createNicknameRequestGuard();
function nicknameProfile(member) {
  return nicknameProfiles.get(member) || {name:'',status:'idle',nextChangeAt:0,retryAt:0,attemptsLeft:null};
}
function publishNickname(member, profile) {
  if (nicknameProfiles.size >= 10000 && !nicknameProfiles.has(member)) nicknameProfiles.delete(nicknameProfiles.keys().next().value);
  nicknameProfiles.set(member, profile);
  const entry = roomDirectory.memberLocation(member), room = rooms.get(entry?.roomId);
  if (!room || entry.socket.accountNickname) return;
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
  if (socket.accountNickname) return socket.accountNickname;
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
const swarmTurnUrls = envValue('EAGLER_NETPLAY_SWARM_TURN_URLS', [])
  .split(',').map(value => value.trim()).filter(Boolean);
const swarmTurnSecret = envValue('EAGLER_NETPLAY_SWARM_TURN_SHARED_SECRET', []);
const swarmAvailable = swarmTurnSecret.length >= 24 && swarmTurnUrls.length > 0
  && swarmTurnUrls.every(url => /^turns?:[^\s]+$/i.test(url));
const swarmControlUrl = envValue('EAGLER_NETPLAY_SWARM_CONTROL_URL', [], 'ws://124.223.180.38:50429/core');
const swarmAccessToken = envValue('SWARM_ACCESS_TOKEN', []);
const swarmWorkers = createSwarmWorkers(swarmAvailable ? swarmControlUrl : '');
if (swarmAvailable) {
  const refreshSwarmWorkers = async () => {
    const previous = swarmWorkers.current();
    await swarmWorkers.refresh();
    const current = swarmWorkers.current();
    if (JSON.stringify(previous) !== JSON.stringify(current)) {
      for (const room of rooms.values()) if (room.lobbyClients.size) broadcastLobby(room);
    }
  };
  setInterval(() => void refreshSwarmWorkers().catch(() => {}), 30_000).unref();
}
const nat4PortSteps = readNat4PortSteps(envValue('EAGLER_NETPLAY_NAT4_PORT_STEPS', []));
const nat4Observers = [...new Set(envValue('EAGLER_NETPLAY_NAT4_OBSERVERS', []).split(',')
  .map(url => url.trim()).filter(Boolean))];
if (nat4Observers.length && (nat4Observers.length !== 3 || !nat4Observers.every(url => /^stun:[^\s]+$/i.test(url))))
  throw new Error('EAGLER_NETPLAY_NAT4_OBSERVERS requires three distinct STUN URLs');

function nat4ExchangeFor(run, addresses, send) {
  if (addresses[0] === addresses[1] || addresses.some(address => !address)) return;
  const configured = addresses.filter(address => nat4PortSteps.has(address)).length;
  if (configured === 2 || (run.rtcReplacementAllowed && nat4Observers.length))
    return createNat4CandidateExchange(addresses, nat4PortSteps, send, nat4Observers);
}
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
const accountChat=ticketKey ? createAccountChatSource({onRetract:event=>{
  for(const room of rooms.values()) {
    broadcastLobby(room,event);
    for(const socket of room.lobbyClients.values())if(socket.accountIdentity?.userId===event.authorId)
      sendLobby(socket,{type:'chat-status',registered:true,authorId:event.authorId,bannedUntil:event.bannedUntil});
  }
}}) : null;
accountChat?.start();
const th09OutboxPath = process.env.EAGLER_NETPLAY_TH09_OUTBOX;
const th09Rounds = th09OutboxPath && accountChat ? createTh09RoundOutbox({
  file: th09OutboxPath,
  send: accountChat.recordTh09Round,
  onStored({players, room: scope}, summaries, settlement) {
    if (!Array.isArray(summaries)) return;
    for (const room of rooms.values()) {
      if (!room.id.startsWith('th09mp-')) continue;
      if (settlement && room.chatScope === scope)
        broadcastLobby(room, {type: 'th09-rating', room: room.id, settlement});
      let changed = false;
      for (const socket of room.lobbyClients.values()) {
        const account = socket.accountIdentity;
        const side = account?.userId ? players.indexOf(account.userId) : -1;
        if (side < 0 || !account.profile) continue;
        const summary = summaries[side];
        const previous = account.profile.th09;
        if (summary?.games === previous?.games && summary?.wins === previous?.wins) continue;
        if (summary) account.profile.th09 = summary;
        else delete account.profile.th09;
        changed = true;
      }
      if (changed) broadcastLobby(room);
    }
  },
}) : null;
function th09RoundRecorder(room, run) {
  return th09Rounds && (round => th09Rounds.enqueue({...round, finishedAt: Date.now(),
    room: room.chatScope, rated: run.th09Rated !== false}));
}
process.once('exit', () => {
  th09Rounds?.close();
  accountChat?.close();
});
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

function iceServersFor(roomId, runId, player, swarm = false, swarmWorker, stun = stunUrls) {
  const servers = [];
  if (stun.length) servers.push({ urls: stun });
  const expires = Math.floor(Date.now() / 1000) + turnTtlSeconds;
  const identity = `${roomId}-${runId}-p${player}`;
  const username = `${expires}:${swarm && swarmWorker != null ? `swarm-${swarmWorker}/` : ''}${identity}`;
  if (swarm) {
    const credential = createHmac('sha1', swarmTurnSecret).update(username).digest('base64');
    return [...servers, {urls: swarmTurnUrls, username, credential}];
  }
  const activeTurn = readTurnConfig();
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

function newRoom(id, chatScope = 'room-' + randomUUID()) {
  const multiplayer = multiplayerPolicyForRoomId(id);
  return {
    id,
    chatScope,
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
      ...(id.startsWith('th03mp-') ? {disableFocus:true} : {}),
      ...(id.startsWith('th19mp-')?{th19Rules:[...TH19_VERSUS_RULES]}:{}),
      visibility: 'public',
      disableCheatMovement: false,
      ...(id.startsWith('th05mp-') ? { initialSpares: TH05_INITIAL_STOCK.spares, initialBombs: TH05_INITIAL_STOCK.bombs } : {}),
      unrated: false,
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
      rtcReplacementClients: new Set(),
      rtcGeneration: 0,
      rtcReplacement: null,
      rtcReplacementRequested: false,
      signalReconnectTimer: null,
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
  if (run.signalReconnectTimer && !run.ended && !run.releasedPlayers?.size) return;
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
  run.swarmSurvey?.close();
  clearTimeout(run.rtcReplacement?.timer);
  clearTimeout(run.signalReconnectTimer);
  run.rtcReplacement?.nat4?.close();
  if (run.th09Finals) {
    // The lobby and gameplay sockets close independently. Keep only the
    // settlement evidence needed to complete a report already in flight.
    const {th09Duel, th09Finals, lobbyClientIds, memberIds} = run;
    room.retiredTh09Reports = {serial: String(runId), expiresAt: Date.now() + 5000,
      run: {ended: true, th09Duel, th09Finals, lobbyClientIds, memberIds}};
  }
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
  if (run.spectatorStages?.ended || run.spectatorPc98?.ended) return;
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
    rooms.get(roomId) === room
  ) {
    for (const [runId, run] of room.runs) maybeDeleteRun(room, runId, run);
    if (room.runs.size) return;
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
      const closePeer = peer => {
        if (peer.readyState === WebSocket.OPEN)
          peer.close(1001, `player ${player + 1} left the run`);
      };
      const playerSockets = [...run.clients.values(), ...run.signalClients.values()];
      const reports = run.th09Finals;
      const latest = reports?.[player]?.last;
      // A report already on the wire from the other seat can finish this result.
      if (roomId.startsWith('th09mp-') && latest >= 0 &&
          reports[player].results.has(latest) && !reports[1 - player].results.has(latest))
        setTimeout(() => playerSockets.forEach(closePeer), 1000).unref();
      else playerSockets.forEach(closePeer);
      if (!run.spectatorTimeline || !run.spectatorComplete)
        for (const peer of run.spectatorClients.values()) closePeer(peer);
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
  const swarmPath = run.swarmDirectPlayer == null ? '' : ` desktopSwarm=${run.swarmDirectSelected ? 'selected' : 'not-selected'}`;
  console.log(`ROUTE room=${roomId} run=${runId} mode=${route}${swarmPath}`);
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
  run.rtcStartup?.close();
  run.rtcStartup = null;
  if (run.routeTimer) clearTimeout(run.routeTimer);
  if (run.spectatorGraceTimer) clearTimeout(run.spectatorGraceTimer);
  run.routeTimer = null;
  run.spectatorGraceTimer = null;
  clearSpectatorHistory(run);
  const swarm = run.swarmDirectPlayer != null
    ? (run.swarmGrantIssued ? 'desktop-with-turn-backup' : 'turn-backup')
    : run.swarm ? 'turn' : 'off';
  console.log(`ROUTE room=${roomId} run=${runId} mode=failed reason=${reason} ready=${run.rtcReady.size}/${run.playerCount} signaling=${run.signalClients.size}/${run.playerCount} swarm=${swarm}`);
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
  const waitingForSignaling = Boolean(run.lobbyClientIds && !allSignalsPresent);
  if (run.route || run.playerCount < 2 || (!allSignalsPresent && !allRelaysPresent && !waitingForSignaling)) return;
  if (run.routeTimer && run.routeTimerWaitingForSignaling === waitingForSignaling) return;
  clearTimeout(run.routeTimer);
  run.routeTimerWaitingForSignaling = waitingForSignaling;
  const timeout = waitingForSignaling ? Math.max(15000, rtcTimeoutMs) :
    run.swarmDirectPlayer != null ? Math.max(12000, rtcTimeoutMs) :
    run.nat4 ? Math.max(7500, rtcTimeoutMs) : rtcTimeoutMs;
  run.routeTimer = setTimeout(() => {
    run.routeTimer = null;
    run.rtcFailed = true;
    run.rtcFailureReason = waitingForSignaling ? 'A player did not join WebRTC signaling' : 'WebRTC connection timed out';
    maybeResolveRoute(roomId, runId, room, run);
  }, timeout);
}

function maybeResolveRoute(roomId, runId, room, run) {
  if (run.route || run.rtcStartup || run.playerCount < 2) return;
  if (run.rtcFailed) {
    if (run.initialIceRoute || !allowGameplayRelayFallback && !roomId.startsWith('th05mp-')) {
      failRoute(roomId, runId, run, run.rtcFailureReason ?? 'WebRTC connection unavailable');
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

function startRtcRouteSelection(roomId, runId, room, run) {
  if (run.route || run.rtcStartup || run.initialIceRoute || (!run.swarm && !run.rtcReplacementAllowed) || run.signalClients.size !== 2 ||
      run.playerCount !== 2 || [...run.signalClients.values()].some(peer => !peer.rtcStartup)) return;
  const turnServers = [0,1].map(player => iceServersFor(roomId, runId + '-startup', player, false, undefined, []));
  if (!turnServers.every(servers => servers.length)) return;
  clearTimeout(run.routeTimer);
  run.routeTimer = null;
  const timeout = run.swarmDirectPlayer != null ? Math.max(12000, rtcTimeoutMs)
    : Math.max(run.nat4 ? 7500 : 6000, rtcTimeoutMs);
  run.rtcStartup = createRtcStartup({
    timeout,
    primaryMayUseSwarm:run.swarm,
    openTurn() {
      for (const [player, peer] of run.signalClients)
        sendSignal(peer, {type:'rtc-standby', serial:1, phase:'measure', route:'turn', generation:0,
          iceServers:turnServers[player]});
    },
    select(serial) {
      broadcastSignal(run, {type:'rtc-startup-select', serial});
    },
    complete(serial) {
      run.rtcStartup = null;
      if (serial === 1) {
        run.swarm = false;
        run.turnMode = 'first-party';
        run.swarmDirectPlayer = undefined;
        run.surveyInitialSwarm = false;
      }
      console.log(`RTC STARTUP room=${roomId} run=${runId} choice=${serial ? 'first-party-turn' : 'primary'}`);
      chooseRoute(roomId, runId, room, run, 'rtc');
    },
    fail() {
      run.rtcStartup = null;
      run.rtcFailed = true;
      run.rtcFailureReason = 'Automatic WebRTC paths did not become ready';
      maybeResolveRoute(roomId, runId, room, run);
    },
  });
  run.startupSurveySerial = 1;
  broadcastSignal(run, {type:'rtc-startup'});
  run.rtcStartup.start();
}

function configureInitialIceChoice(run, choice) {
  run.initialIceRoute = choice;
  run.swarm = choice === 'swarm' && swarmAvailable;
  run.turnMode = choice === 'turn' ? 'first-party' : choice;
  const workers = swarmWorkers.current();
  if (run.swarm) run.swarmWorkers ??= Array(run.playerCount).fill(workers[0]?.id);
  if (!run.swarm) run.swarmDirectPlayer = undefined;
  if (swarmAvailable && workers.length) run.adaptiveSwarmWorkers ??= workers.map(node => node.id);
  run.surveyInitialSwarm = false;
}

function applyInitialIceChoice(run) {
  if (run.initialIceRoute || run.route || run.signalClients.size !== run.playerCount ||
      [...run.signalClients.values()].some(peer => !peer.hasInitialIceChoice || peer.peersAnnounced)) return;
  for (let seat = 0; seat < run.playerCount; seat++) {
    const choice = run.signalClients.get(seat).initialIceChoice;
    if (choice === 'auto') continue;
    configureInitialIceChoice(run, choice);
    break;
  }
}

function acceptTh09PlayerReport(room, run, runId, player, message) {
  const clientId = run.lobbyClientIds?.[player];
  if (message.winner !== undefined && message.characters !== undefined &&
      run.memberIds?.has(clientId) && !run.releasedPlayers?.has(player) &&
      acceptTh09MatchStats(room, run, clientId, {...message, serial:Number(runId)}, th09RoundRecorder(room, run)))
    broadcastLobby(room);
}

function failRtcReplacement(run, reason) {
  const replacement = run.rtcReplacement;
  if (!replacement) return;
  clearTimeout(replacement.timer);
  replacement.nat4?.close();
  run.rtcReplacement = null;
  run.ended = true;
  console.log(`RTC REPLACE FAILED generation=${run.rtcGeneration} reason=${reason}`);
  broadcastSignal(run, {type:'rtc-replace-failed', generation:run.rtcGeneration});
  for (const socket of run.signalClients.values()) socket.close(1013, 'RTC replacement failed');
}

async function replaceRtcTransport(run) {
  const generation = ++run.rtcGeneration;
  const replacement = {ready:new Set(), started:false, nat4:null, timer:null};
  run.rtcReplacement = replacement;
  replacement.timer = setTimeout(() => failRtcReplacement(run, 'timeout'), 15000);
  replacement.timer.unref();
  broadcastSignal(run, {type:'rtc-replacing', generation});
  let grant;
  if (run.swarmDirectPlayer != null) {
    run.swarmGrant = allocateDesktopSwarm(run);
    grant = await run.swarmGrant;
  }
  if (run.rtcReplacement !== replacement || run.ended) return;
  const addresses = [0, 1].map(seat => run.signalClients.get(seat)?.relayAddress);
  replacement.nat4 = nat4ExchangeFor(run, addresses, (from, signal) => {
      const target = run.signalClients.get(1 - from);
      if (target) sendSignal(target, {type:'signal', from, generation, ...signal});
  });
  replacement.started = true;
  for (const [seat, socket] of run.signalClients)
    sendSignal(socket, {type:'rtc-replace', generation,
      ...(seat === run.swarmDirectPlayer && grant ? {swarmGrant:grant} : {})});
}

function maybeReplaceRtcTransport(roomId, runId, run) {
  if (!run.rtcReplacementRequested || run.signalClients.size !== 2) return;
  run.rtcReplacementRequested = false;
  if (run.ended || run.rtcReplacement || run.disconnectedPlayers?.size || run.releasedPlayers?.size) return;
  if ([...run.signalClients.values()].some(socket => socket.readyState !== WebSocket.OPEN)) {
    run.rtcReplacementRequested = true;
    return;
  }
  run.swarmSurvey?.close();
  run.swarmSurvey = null;
  console.log(`RTC REPLACE room=${roomId} run=${runId} generation=${run.rtcGeneration + 1}`);
  void replaceRtcTransport(run).catch(() => failRtcReplacement(run, 'setup'));
}

function allocateDesktopSwarm(run) {
  return allocateSwarmPort(swarmControlUrl, swarmAccessToken, run.swarmWorkers[run.swarmDirectPlayer] ?? 0)
    .catch(() => undefined);
}

function handleSwarmSurvey(roomId, runId, run, player, socket, message) {
  if (run.route !== 'rtc' || run.ended || run.rtcReplacement || run.signalClients.size !== 2 ||
      run.signalClients.get(player) !== socket || run.rtcSurveyClients?.size !== 2 ||
      (message.generation ?? 0) !== run.rtcGeneration || !run.adaptiveSwarmWorkers?.length)
    return;
  if (message.type === 'rtc-standby-signal') {
    if (!Number.isSafeInteger(message.serial) || !run.swarmSurvey?.acceptsSignal(message.serial)) return;
    const signal = normalizeRelaySignal(message);
    if (!signal || (signal.description && signal.description.type !== (player === 0 ? 'offer' : 'answer'))) return;
    sendSignal(run.signalClients.get(1 - player),
      {type:'rtc-standby-signal', serial:message.serial, generation:run.rtcGeneration, ...signal});
    return;
  }
  const quality = readRouteProbeQuality(message.quality, Date.now());
  if (message.type === 'rtc-route-report') {
    if (quality || message.quality === null) run.swarmSurvey?.report(player, message.serial, quality);
    return;
  }
  if (!quality || typeof message.route !== 'string') return;
  run.swarmSurvey ??= createSwarmStandbyPlanner({
    workers:run.adaptiveSwarmWorkers,
    ordinaryTurn:iceServersFor(roomId, runId + '-standby', player, false, undefined, []).length > 0,
    initialSerial:run.startupSurveySerial,
    dispatch(update) {
      const worker = update.route?.startsWith('swarm-') ? Number(update.route.slice(6)) : undefined;
      const tag = runId + '-standby-' + update.serial;
      for (const [seat, peer] of run.signalClients) {
        const iceServers = update.route === 'direct' ? [{urls:stunUrls}] : update.route
          ? iceServersFor(roomId, tag, seat, worker != null, worker, []) : undefined;
        sendSignal(peer, {type:'rtc-standby', generation:run.rtcGeneration, ...update, ...(iceServers && {iceServers})});
      }
    },
  });
  if (run.swarmSurvey.consider(message.route, quality, run.surveyInitialSwarm))
    run.surveyInitialSwarm = false;
}

async function handleSignalConnection(socket, roomId, runId, player, playerCount) {
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
  let swarmGrant;
  if (player === run.swarmDirectPlayer) {
    swarmGrant = await run.swarmGrant;
    if (socket.readyState !== WebSocket.OPEN || rooms.get(roomId) !== room || room.runs.get(runId) !== run ||
        run.ended || run.releasedPlayers?.has(player)) {
      socket.close(4008, 'membership released');
      return;
    }
    run.swarmGrantIssued = Boolean(swarmGrant);
  }
  const previous = run.signalClients.get(player);
  if (previous && previous !== socket && previous.readyState === WebSocket.OPEN)
    previous.close(1000, 'signaling reconnected');
  run.signalClients.set(player, socket);
  socket.initialSwarmGrant = swarmGrant;
  if (run.lobbyClientIds) applyInitialIceChoice(run);
  clearTimeout(run.signalReconnectTimer);
  run.signalReconnectTimer = null;
  function forwardSignal(from, signal) {
    const target = run.signalClients.get(1 - from);
    if (target) sendSignal(target, {type: 'signal', from, ...signal});
  }
  const nat4Allowed = !run.initialIceRoute || run.initialIceRoute === 'direct';
  if (nat4Allowed && !run.nat4 && !run.route && playerCount === 2 && run.signalClients.size === 2) {
    const addresses = [0, 1].map(seat => run.signalClients.get(seat).relayAddress);
    run.nat4 = nat4ExchangeFor(run, addresses, forwardSignal);
    if (run.nat4) {
      if (run.routeTimer) {
        clearTimeout(run.routeTimer);
        run.routeTimer = null;
      }
    }
  }
  const waitForOtherChoice = run.lobbyClientIds && !run.initialIceRoute && !run.route &&
      run.signalClients.size < playerCount &&
      [...run.signalClients.values()].every(peer => peer.hasInitialIceChoice && !peer.peersAnnounced);
  if (!waitForOtherChoice) {
    const announced = [];
    for (const [seat, target] of run.signalClients) {
      if (target.peersAnnounced) {
        if (seat !== player) announced.push(target);
        continue;
      }
      const relayOnly = run.initialIceRoute === 'swarm' || run.initialIceRoute === 'turn';
      const observeStun = !relayOnly && run.rtcReplacementAllowed && nat4Observers.length > 0 && !nat4PortSteps.has(target.relayAddress);
      const stun = relayOnly ? [] : observeStun ? nat4Observers : stunUrls;
      const grant = run.swarm ? target.initialSwarmGrant : undefined;
      const unavailableSwarm = run.initialIceRoute === 'swarm' && !swarmAvailable;
      const iceServers = grant || run.initialIceRoute === 'direct' || unavailableSwarm
        ? (stun.length ? [{urls:stun}] : [])
        : iceServersFor(roomId, runId, seat, run.swarm, run.swarmWorkers?.[seat], stun);
      target.peersAnnounced = true;
      sendSignal(target, {
        type: 'peers',
        peers: [...run.signalClients.keys()].filter(peer => peer !== seat),
        route: run.route,
        iceServers,
        ...(run.turnMode ? {turnMode:run.turnMode} : {}),
        ...(relayOnly ? {initialIceRelayOnly:true} : {}),
        ...(run.initialIceRoute ? {fallbackIceServers:iceServersFor(roomId, runId + '-backup', seat)} : {}),
        ...(run.adaptiveSwarmWorkers?.length ? {adaptiveSwarm:true, initialSwarmWorker:run.swarmWorkers?.[seat],
          ...(run.surveyInitialSwarm && {surveyInitialSwarm:true})} : {}),
        ...(grant ? {swarmGrant:grant} : {}),
        ...(observeStun ? {nat4Observers} : {}),
        ...(run.rtcReplacementClients.size === 2 ? {rtcReplace:true} : {}),
        ...(roomId.startsWith('th09mp-') && run.memberIds ? {th09MatchReport:true} : {}),
      });
    }
    for (const target of announced) sendSignal(target, {type:'peer-join', player});
  }
  console.log(
    `SIGNAL JOIN room=${roomId} run=${runId} player=${player} peers=${run.signalClients.size}`,
  );
  startRtcRouteSelection(roomId, runId, room, run);
  maybeResolveRoute(roomId, runId, room, run);
  maybeReplaceRtcTransport(roomId, runId, run);

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
    if (run.rtcStartup && run.signalClients.get(player) === socket) {
      if (message.type === 'rtc-startup-quality') {
        const quality = readRouteProbeQuality(message.quality, Date.now());
        if (message.quality === null || quality)
          run.rtcStartup.report(player, message.serial, quality && {...quality, direct: message.serial === 0 && message.direct === true});
        return;
      }
      if (message.type === 'rtc-startup-selected') {
        if (message.rtcReplace === true && run.rtcReplacementAllowed) run.rtcReplacementClients.add(player);
        if (message.rtcRouteSurvey === true && run.adaptiveSwarmWorkers?.length) {
          run.rtcSurveyClients ??= new Set();
          run.rtcSurveyClients.add(player);
        }
        run.rtcStartup.acknowledge(player, message.serial);
        if (run.route === 'rtc' && run.rtcReplacementClients.size === 2)
          broadcastSignal(run, {type:'rtc-replace-capable'});
        return;
      }
      if (message.type === 'rtc-standby-signal') {
        const signal = message.serial === 1 && run.rtcStartup.acceptsTurnSignal() && normalizeRelaySignal(message);
        if (signal && (!signal.description || signal.description.type === (player === 0 ? 'offer' : 'answer')))
          sendSignal(run.signalClients.get(1 - player), {type:'rtc-standby-signal', serial:1, generation:0, ...signal});
        return;
      }
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
      if (!target || run.signalClients.get(player) !== socket) return;
      let signal = normalizeRelaySignal(message);
      if (!signal) {
        socket.close(1008, 'invalid signaling payload');
        return;
      }
      if (run.initialIceRoute && !run.route) {
        signal = restrictInitialIce(signal, run.initialIceRoute);
        if (!signal.description && !signal.candidate) return;
      }
      const observation = {...signal, observer: message.observer};
      if (message.observer != null && (!signal.candidate || signal.description || !nat4Observers.includes(message.observer)))
        return;
      if (run.rtcGeneration) {
        if (run.ended || message.generation !== run.rtcGeneration) return;
        const exchange = run.rtcReplacement?.nat4;
        if (exchange) exchange.send(player, observation);
        else if (message.observer == null) sendSignal(target, {type:'signal', from:player, generation:run.rtcGeneration, ...signal});
      } else if (message.generation != null) return;
      else if (run.nat4 && !run.route) run.nat4.send(player, observation);
      else if (message.observer == null) sendSignal(target, {
        type: 'signal',
        from: player,
        ...signal,
      });
      return;
    }
    if (['rtc-route-slow','rtc-route-report','rtc-standby-signal'].includes(message.type)) {
      handleSwarmSurvey(roomId, runId, run, player, socket, message);
      return;
    }
    if (message.type === 'rtc-ready') {
      if (run.signalClients.get(player) !== socket) return;
      if (run.rtcGeneration) {
        const replacement = run.rtcReplacement;
        if (!replacement || message.generation !== run.rtcGeneration || !replacement.started) return;
        replacement.ready.add(player);
        if (replacement.ready.size === playerCount) {
          clearTimeout(replacement.timer);
          replacement.nat4?.close();
          run.rtcReplacement = null;
          broadcastSignal(run, {type:'rtc-replaced', generation:run.rtcGeneration});
        }
        return;
      }
      if (message.rtcReplace === true && run.rtcReplacementAllowed) run.rtcReplacementClients.add(player);
      if (message.rtcRouteSurvey === true && run.adaptiveSwarmWorkers?.length) {
        run.rtcSurveyClients ??= new Set();
        run.rtcSurveyClients.add(player);
      }
      if (run.rtcReplacementClients.size === 2 && !run.route)
        broadcastSignal(run, {type:'rtc-replace-capable'});
      if (player === run.swarmDirectPlayer) run.swarmDirectSelected = message.swarmDirect === true;
      run.rtcReady.add(player);
      maybeResolveRoute(roomId, runId, room, run);
      return;
    }
    if (message.type === 'rtc-replace-request' && run.route === 'rtc' && run.signalClients.get(player) === socket &&
        !run.ended && run.rtcReplacementClients.size === 2 && !run.rtcReplacement &&
        message.generation === run.rtcGeneration && !run.disconnectedPlayers?.size && !run.releasedPlayers?.size) {
      run.rtcReplacementRequested = true;
      maybeReplaceRtcTransport(roomId, runId, run);
      return;
    }
    if (message.type === 'rtc-replace-failed' && run.signalClients.get(player) === socket &&
        message.generation === run.rtcGeneration) {
      failRtcReplacement(run, `player-${player}`);
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
      if (run.signalClients.get(player) !== socket) return;
      if (run.rtcStartup) {
        run.rtcStartup.report(player, 0, null);
        return;
      }
      run.rtcFailed = true;
      run.rtcFailureReason = 'A player reported a WebRTC connection failure';
      maybeResolveRoute(roomId, runId, room, run);
      return;
    }
  });

  socket.on('close', code => {
    if (run.signalClients.get(player) === socket) {
      if (run.rtcReplacement) failRtcReplacement(run, 'signaling closed');
      run.nat4?.close();
      run.signalClients.delete(player);
      run.swarmSurvey?.close();
      run.swarmSurvey = null;
      run.rtcReady.delete(player);
      if (code === 1006 && run.route === 'rtc' && run.rtcReplacementClients.size === 2 &&
          !run.ended && !run.signalClients.size && !run.releasedPlayers?.size && !run.disconnectedPlayers?.size) {
        run.signalReconnectTimer = setTimeout(() => {
          run.signalReconnectTimer = null;
          maybeDeleteRun(room, runId, run);
          maybeDeleteRoom(roomId, room);
        }, lobbyReconnectGraceMs);
        run.signalReconnectTimer.unref();
      }
      if (!run.route) {
        run.rtcStartup?.close();
        run.rtcStartup = null;
        run.rtcFailed = true;
        run.rtcFailureReason = 'A player left WebRTC signaling';
        maybeResolveRoute(roomId, runId, room, run);
      }
    }
    console.log(`SIGNAL LEAVE room=${roomId} run=${runId} player=${player}`);
    maybeDeleteRun(room, runId, run);
    maybeDeleteRoom(roomId, room);
  });
}

function prefersSwarm(seat, index) {
  return seat?.swarmOverride ?? (seat?.preferSwarm === true || index === 0 && seat?.nativeRuntime === true);
}

function publicRoomProfile(room, clientId) {
  return room.lobbyClients.get(clientId)?.accountIdentity?.profile;
}

function startupRoutes() {
  const turn = readTurnConfig();
  const ordinaryTurn = [turn, ...(turn.backups ?? [])].some(node =>
    node.urls.length && (node.sharedSecret || turnUsername && turnCredential));
  return ['auto', 'direct', ...(ordinaryTurn ? ['turn'] : []), ...(swarmAvailable ? ['swarm'] : [])];
}

function lobbySnapshot(room) {
  const spectators = [...room.lobby.spectators.entries()]
    .filter(([clientId]) => room.lobbyClients.has(clientId))
    .map(([clientId, name]) => ({ clientId, name, profile: publicRoomProfile(room, clientId) }));
  return {
    playerCount: room.lobby.playerCount,
    difficulty: room.lobby.difficulty,
    visibility: room.lobby.visibility,
    disableCheatMovement: room.lobby.disableCheatMovement,
    ...(room.id.startsWith('th05mp-')?{initialSpares:room.lobby.initialSpares,initialBombs:room.lobby.initialBombs}:{}),
    ...(room.id.startsWith('th03mp-') ? {disableFocus:room.lobby.disableFocus} : {}),
    ...(room.id.startsWith('th09mp-') ? {unrated: room.lobby.unrated} : {}),
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
    startupRoutes: startupRoutes(),
    ...(swarmAvailable ? {swarmAvailable: true} : {}),
    ...(swarmAvailable ? {swarmWorkers: swarmWorkers.current()} : {}),
    ...(room.lobby.phase !== 'lobby' && room.runs.get(String(room.lobby.startSerial))?.swarm
      ? {swarmSelected: true} : {}),
    ...(room.lobby.phase !== 'lobby' && room.runs.get(String(room.lobby.startSerial))?.runtimeGeneration
      ? {runtimeGeneration: room.runs.get(String(room.lobby.startSerial)).runtimeGeneration} : {}),
    startSerial: room.lobby.startSerial,
    spectators,
    spectatorCount: spectators.length,
    ...((process.env.EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE === '1' && room.id.startsWith('th09mp-')) ||
        room.runs.get(String(room.lobby.startSerial))?.spectatorStages ||
        room.runs.get(String(room.lobby.startSerial))?.spectatorPc98
      ? { spectatorTimeline: true }
      : {}),
    seats: room.lobby.seats.map((seat, index) =>
      seat
        ? {
            clientId: seat.clientId,
            name: seat.name || '',
            profile: publicRoomProfile(room, seat.clientId),
            ...(/^th(?:09|19)mp-/.test(room.id)
              ? { stats: th09PlayerStats(room, seat.clientId) }
              : {}),
            loadout: seat.loadout,
            controlMode: publicControlMode(seat),
            ...(typeof seat.touchEnabled === 'boolean' ? { touchEnabled: seat.touchEnabled } : {}),
            mobileDevice: seat.mobileDevice === true,
            ...(seat.nativeRuntime ? {nativeRuntime: true} : {}),
            ...(swarmAvailable && prefersSwarm(seat, index) ? {preferSwarm: true} : {}),
            ...(seat.startupRoute ? {startupRoute: seat.startupRoute} : {}),
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
  const next = newRoom(targetId, room.chatScope);
  const previous = room.lobby;
  next.lobby.visibility = previous.visibility;
  next.lobby.disableCheatMovement = previous.disableCheatMovement;
  next.lobby.unrated = previous.unrated;
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
  if (swarmAvailable) void swarmWorkers.refresh().then(() => {
    const room = rooms.get(roomId);
    if (room?.lobbyClients.get(clientId) === socket) broadcastLobby(room);
  }).catch(() => {});
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
  if (ticketKey) socket.accountChallenge = randomUUID();
  if (nicknameService) void nicknameService.current(memberId).then(profile=>publishNickname(memberId,nicknameRequests.has(memberId) ? {...profile,status:'reviewing'} : profile)).catch(()=>sendLobby(socket,{type:'nickname-status',...nicknameProfile(memberId),status:'unavailable'}));
  const room = getRoom(roomId, socket);
  if (!room) return;
  if (code) roomCodes.set(code, roomId);
  if (!existing && intent === 'create') {
    if(room.id.startsWith('th19mp-'))room.lobby.th19Rules=[...(initialPolicy?.th19Rules??TH19_VERSUS_RULES)];
    room.lobby.visibility = initialPolicy?.visibility === 'private' ? 'private' : 'public';
    room.lobby.disableCheatMovement = initialPolicy?.disableCheatMovement === true;
    if(room.id.startsWith('th03mp-'))room.lobby.disableFocus=initialPolicy?.disableFocus!==false;
    room.lobby.unrated = room.id.startsWith('th09mp-') && initialPolicy?.unrated === true;
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
  const swarmProbeWorkers = () => room.lobby.playerCount === 2 && swarmAvailable
    ? swarmWorkers.current().slice(0, 24) : [];
  const routeProbes = new Map();
  socket.routeProbes = routeProbes;
  sendLobby(socket, {
    type: 'state',
    room: lobbySnapshot(room),
    roomDirectory: { version: 1, controlModes: true, runtimeVersions: true, runReturn: true, globalRoomCodes: !!code },
    roomActions: 1,
    accountChallenge: socket.accountChallenge,
    ...(listenTogether ? { listenTogether: 2 } : {}),
    roomProbe: {
      swarmPings: swarmAvailable,
      iceServers: iceServersFor(roomId, 'lobby-probe', 0),
      swarm: swarmProbeWorkers().map(node => ({
        id: node.id,
        iceServers: iceServersFor(roomId, 'lobby-swarm', 0, true, node.id, []),
      })),
    },
  });
  void accountChat?.history(room.chatScope).then(history=>{
    if (room.lobbyClients.get(clientId)!==socket) return;
    const entries = history.entries.map(message=>({...message,room:room.id}));
    sendLobby(socket,{type:'chat-history',...history,room:room.id,entries});
  }).catch(()=>{});
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

    if (message.type === 'swarm-pings') {
      if (room.lobby.phase !== 'lobby' || room.lobbyClients.get(clientId) !== socket || lobbySeatOf(room, clientId) < 0 ||
          !Array.isArray(message.samples) || message.samples.length > 24) return;
      const workers = new Map(swarmProbeWorkers().map(worker => [worker.id, worker.ip]));
      const samples = new Map();
      for (const sample of message.samples) {
        if (!sample || !workers.has(sample.id) || workers.get(sample.id) !== sample.ip ||
            !Number.isFinite(sample.rtt) || sample.rtt < 0 || sample.rtt > 1500) continue;
        samples.set(sample.id, {ip:sample.ip, rtt:Math.round(sample.rtt)});
      }
      socket.swarmPings = {at:Date.now(), samples};
      return;
    }
    if (message?.type === 'room-probe' || message?.type === 'room-probe-report') {
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
      const workers = new Set(swarmProbeWorkers().map(node => 'swarm-' + node.id));
      if (message.type === 'room-probe-report') {
        if (message.to === clientId || !onlinePeers.has(message.to) || !(message.lane === 'turn' || workers.has(message.lane)))
          return;
        const quality = readRouteProbeQuality(message.quality, Date.now());
        if (quality) {
          const peer = room.lobbyClients.get(message.to);
          routeProbes.set(message.lane, {...quality, peer});
          if (typeof message.token === 'string' && /^[a-zA-Z0-9-]{1,64}$/.test(message.token))
            sendLobby(peer, {type:'room-probe-report', from:clientId, lane:message.lane, token:message.token,
              quality:{p95:quality.p95, sent:quality.sent, received:quality.received}});
        }
        return;
      }
      const envelope = roomProbeEnvelope(message, clientId, onlinePeers, workers);
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
        nativeRuntime: message.nativeRuntime === true,
        preferSwarm: swarmAvailable && message.preferSwarm === true,
        swarmOverride: previousEntry?.swarmOverride,
        swarmWorker: previousEntry?.swarmWorker,
        swarmWorkerManual: previousEntry?.swarmWorkerManual,
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
        if (run?.spectatorTimeline && (!(run.spectatorStages||run.spectatorPc98?.matchInitializers) || run.spectatorAdmissionOpen) && !run.ended && !run.spectatorStopped) {
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

    if (message.type === 'account-nickname' && accountChat) {
      const account = roomAccount(ticketKey,socket.accountChallenge,message.ticket);
      if (!account) return;
      const {name} = account;
      socket.accountChallenge = null;
      socket.accountNickname = name;
      socket.accountIdentity=account;
      const seat = lobbySeatOf(room,clientId);
      if (seat >= 0) room.lobby.seats[seat].name = name;
      if (room.lobby.spectators.has(clientId)) room.lobby.spectators.set(clientId,name);
      sendLobby(socket,{type:'nickname-status',name,status:'approved',registered:true});
      if(account.userId && account.sessionId)void accountChat.status(account).then(status=>{
        if(room.lobbyClients.get(clientId)===socket)sendLobby(socket,{type:'chat-status',registered:true,authorId:account.userId,...status});
      }).catch(()=>sendLobby(socket,{type:'chat-status',registered:false}));
      broadcastLobby(room);
      return;
    }
    if (message.type === 'set-name') {
      if (socket.accountNickname) {sendLobby(socket,{type:'nickname-status',name:socket.accountNickname,status:'approved',registered:true});return;}
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
    if (accountChat && (message.type === 'free-chat' || message.type === 'chat-report')) {
      const spectator=seat<0 && room.lobby.spectators.has(clientId),account=socket.accountIdentity;
      const fail=(code,retryAt=0)=>sendLobby(socket,{type:'chat-result',action:message.type,ok:false,code,retryAt});
      if(seat<0 && !spectator){fail('not-in-room');return;}
      const reporting=message.type==='chat-report';
      const accountIdentity=account?.userId && account.sessionId ? {userId:account.userId,sessionId:account.sessionId} : null;
      if(!reporting && !accountIdentity){fail('sign-in-required');return;}
      if(reporting ? !Number.isSafeInteger(message.messageId) || message.messageId<1 || (message.reason !== undefined && (typeof message.reason !== 'string' || !message.reason.trim() || message.reason.trim().length>500))
        :typeof message.text!=='string' || message.text.length>500 || message.serial!==room.lobby.startSerial){fail('invalid-message');return;}
      const pending=reporting?'pendingChatReport':'pendingChatSend';
      if(socket[pending]){fail(reporting?'report-in-progress':'send-rate-limited');return;}
      socket[pending]=true;
      const reporter=accountIdentity || {
        guestKey:'guest:'+createHmac('sha256',ticketKey).update('chat-report-ip:').update(nicknameAddressScope(socket.relayAddress)).digest('hex'),
        clientId,
      };
      const task=reporting?accountChat.report({...reporter,room:room.chatScope,messageId:message.messageId,...(message.reason !== undefined ? {reason:message.reason.trim()} : {})})
        :accountChat.send({...accountIdentity,room:room.chatScope,clientId,serial:room.lobby.startSerial,seat,spectator,text:message.text});
      void task.then(result=>{
        if(!reporting && result && rooms.get(room.id)===room)broadcastLobby(room,{...result,room:room.id});
        if(room.lobbyClients.get(clientId)===socket)sendLobby(socket,{type:'chat-result',action:message.type,ok:true});
      }).catch(error=>{
        if(room.lobbyClients.get(clientId)===socket)fail(error.code || 'service-unavailable',error.retryAt);
      }).finally(()=>{socket[pending]=false;});
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
    if (message.type === 'startup-route') {
      if (room.lobby.phase !== 'lobby' || !startupRoutes().includes(message.route)) return;
      if (occupant.startupRoute !== message.route) {
        occupant.startupRoute = message.route;
        broadcastLobby(room);
      }
      return;
    }
    if (message.type === 'relay-preference') {
      if (!swarmAvailable || room.lobby.phase !== 'lobby' || typeof message.swarm !== 'boolean') return;
      const automatic = message.automatic === true;
      if (automatic && (!message.swarm || !prefersSwarm(occupant, seat) ||
          (occupant.swarmWorkerManual && swarmWorkers.current().some(node => node.id === occupant.swarmWorker)))) return;
      const worker = message.swarm && swarmWorkers.current().some(node => node.id === message.worker)
        ? message.worker : undefined;
      const override = automatic ? occupant.swarmOverride : message.swarm;
      const manual = !automatic && worker != null;
      if (occupant.swarmOverride !== override || occupant.swarmWorker !== worker || occupant.swarmWorkerManual !== manual) {
        occupant.swarmOverride = override;
        occupant.swarmWorker = worker;
        occupant.swarmWorkerManual = manual;
        broadcastLobby(room);
      }
      return;
    }
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
      if (room.id.startsWith('th19mp-')) {
        if (acceptTh19MatchStats(room, clientId, message)) broadcastLobby(room);
        return;
      }
      const retired = room.retiredTh09Reports;
      const serial = String(message.serial);
      const run = room.runs.get(serial) ??
        (retired?.serial === serial && retired.expiresAt > Date.now() ? retired.run : undefined);
      if (
        (room.lobby.phase === 'running' || run?.th09Duel) &&
        (!accountChat || run?.memberIds?.get(clientId) === memberId) &&
        acceptTh09MatchStats(room, run, clientId, message, th09RoundRecorder(room, run))
      )
        broadcastLobby(room);
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
      const nativeRuntime = typeof message.nativeRuntime === 'boolean' ? message.nativeRuntime : occupant.nativeRuntime;
      if (occupant.nativeRuntime !== nativeRuntime || JSON.stringify(occupant.resource) !== JSON.stringify(resource)) {
        if (room.lobby.phase === 'lobby' && resource.status !== 'ready' &&
            (occupant.resource?.status === 'ready' || occupant.ready)) invalidateLobbyReady(room);
        occupant.resource = resource;
        occupant.nativeRuntime = nativeRuntime;
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
      occupant.nativeRuntime = message.nativeRuntime === true;
      occupant.swarmDirect = occupant.nativeRuntime && message.swarmDirect === true;
      if (typeof message.preferSwarm === 'boolean')
        occupant.preferSwarm = swarmAvailable && message.preferSwarm;
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
      if(message.disableFocus!==undefined&&(!room.id.startsWith('th03mp-')||typeof message.disableFocus!=='boolean')){
        sendLobby(socket,{type:'error',error:'invalid focus rule'});return;
      }
      const disableFocus=message.disableFocus??room.lobby.disableFocus;
      if (message.unrated !== undefined && (!room.id.startsWith('th09mp-') || typeof message.unrated !== 'boolean')) {
        sendLobby(socket,{type:'error',error:'invalid rating setting'});return;
      }
      const unrated = message.unrated ?? room.lobby.unrated;
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
        room.lobby.unrated !== unrated ||
        room.lobby.disableLocalChargePriority !== disableLocalChargePriority ||
        room.lobby.disableFocus !== disableFocus ||
        room.lobby.challengeMode !== challengeMode ||
        room.lobby.prankMode !== prankMode ||
        room.lobby.initialSpares !== initialSpares || room.lobby.initialBombs !== initialBombs ||
        (room.th15 && room.lobby.chapterThreshold !== chapterThreshold)
      ) {
        invalidateLobbyReady(room);
        if(room.id.startsWith('th19mp-'))room.lobby.th19Rules=[...th19Rules];
        room.lobby.disableCheatMovement = disableCheatMovement;
        if(room.id.startsWith('th03mp-'))room.lobby.disableFocus=disableFocus;
        room.lobby.unrated = unrated;
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
      const seatedClients = new Set(activeSeats.map((entry) => entry.clientId));
      run.playerCount = room.lobby.playerCount;
      run.rtcReplacementAllowed = activeSeats.length === 2 && activeSeats.some(seat => seat.nativeRuntime);
      run.runtimeGeneration = runtimeGeneration;
      const routes = startupRoutes();
      const fixedRoute = activeSeats.map(entry => entry.startupRoute).find(route => route !== 'auto' && routes.includes(route));
      run.swarm = swarmAvailable && activeSeats.some(prefersSwarm);
      const available = run.swarm ? swarmProbeWorkers() : [];
      let preferredPath, estimatedWorker;
      if (available.length && activeSeats.length === 2 && !activeSeats.some(seat => seat.swarmWorkerManual)) {
        const left = room.lobbyClients.get(activeSeats[0].clientId);
        const right = room.lobbyClients.get(activeSeats[1].clientId);
        const reports = (socket, peer) => new Map([...socket?.routeProbes ?? []].filter(([, report]) => report.peer === peer));
        const leftReports = reports(left,right), rightReports = reports(right,left);
        const now = Date.now();
        const ranked = rankSwarmCandidates(available, leftReports, rightReports, now, [left?.swarmPings, right?.swarmPings]);
        run.adaptiveSwarmWorkers = ranked.map(candidate => candidate.worker);
        const ordinaryTurnAvailable = iceServersFor(roomId, String(room.lobby.startSerial), 0, false, undefined, []).length > 0;
        preferredPath = selectSwarmRoute(ranked, ordinaryTurnAvailable);
        run.surveyInitialSwarm = preferredPath == null;
        if (ranked[0]?.measuredEndpoints) estimatedWorker = ranked[0].worker;
        if (preferredPath?.mode === 'turn') run.swarm = false;
      }
      run.turnMode = run.swarm ? 'swarm' : preferredPath?.mode === 'turn' ? 'first-party' : undefined;
      if (run.swarm) {
        const workerFor = (entry, seat) => prefersSwarm(entry, seat) && available.some(node => node.id === entry.swarmWorker)
          ? entry.swarmWorker : undefined;
        const preferred = activeSeats.map(workerFor);
        const defaultWorker = preferredPath?.worker ?? estimatedWorker ?? preferred.find(worker => worker != null) ?? available[0]?.id;
        run.swarmWorkers = run.adaptiveSwarmWorkers?.length ? activeSeats.map(() => defaultWorker)
          : preferred.map(worker => worker ?? defaultWorker);
        if ((!fixedRoute || fixedRoute === 'swarm') && swarmAccessToken && activeSeats.length === 2) {
          const native = activeSeats.findIndex(seat => seat.nativeRuntime && seat.swarmDirect);
          if (native !== -1) {
            run.swarmDirectPlayer = native;
            run.swarmGrant = allocateDesktopSwarm(run);
          }
        }
      }
      if (fixedRoute) configureInitialIceChoice(run, fixedRoute);
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
      if (roomId.startsWith('th09mp-')) {
        run.th09Rated = !room.lobby.unrated;
        const players = run.lobbyClientIds.map((id) => room.lobbyClients.get(id)?.accountIdentity?.userId ?? null);
        bindTh09Duel(room, run, players);
      }
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
    console.log(`SPECTATOR DENY room=${roomId} run=${runId} historyUnavailable=${!!run?.spectatorHistoryUnavailable} admitted=${!!run?.admittedSpectators.has(spectatorId)} stream=${!!run?.spectatorPc98} admissionOpen=${!!run?.spectatorAdmissionOpen} ended=${!!run?.spectatorPc98?.ended} pending=${run?.spectatorPc98?.pendingMatch} stopped=${!!run?.spectatorStopped}`);
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
    if(run.spectatorPc98){socket.spectatorQueuedPc98=[];socket.spectatorQueuedPc98Bytes=0;}
    let index = 0,
      blockedAt = 0,
      history = run.spectatorPc98?.pendingMatch ? null : run.spectatorPc98 ? [...run.spectatorHistory] : run.spectatorHistory;
    socket.spectatorSnapshotPending=history===null;
    const replay = () => {
      if (
        socket.readyState !== WebSocket.OPEN ||
        (socket.spectatorReplayEpoch || 0) !== replayEpoch ||
        run.spectatorStopped ||
        run.spectatorClients.get(spectatorId) !== socket
      )
        return;
      if(history===null){
        if(run.spectatorPc98?.pendingMatch){socket.spectatorReplayTimer=setTimeout(replay,50);return;}
        history=[...run.spectatorHistory];socket.spectatorSnapshotPending=false;
      }
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
      if (index >= history.length && run.spectatorPc98 && socket.spectatorQueuedPc98?.length){
        history=socket.spectatorQueuedPc98.splice(0);socket.spectatorQueuedPc98Bytes=0;index=0;
      } else if (index >= history.length && history !== run.spectatorHistory && !run.spectatorPc98) {
        history = run.spectatorStages ? socket.spectatorQueuedStages?.shift() : run.spectatorHistory;
        if (!history) {socket.close(1011, 'spectator stage history gap'); return;}
        index = 0;
      }
      const first = history[index];
      if (!first) {
        if (run.spectatorStages && !sendSpectatorPayload(socket, run.spectatorStages.bootstrap(), roomId, runId, spectatorId)) return;
        if (run.spectatorPc98 && !sendSpectatorPayload(socket, run.spectatorPc98.bootstrap(), roomId, runId, spectatorId)) return;
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
      if (!run.spectatorPc98 && (first.length === 46 || first.length === 126))
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

const servePlaylist = listenTogether ? createNetEasePlaylistHandler() : null;
const httpServer = createServer((request, response) => {
  if (servePlaylist && request.url?.startsWith('/_eagler/netease-playlist?')) {
    void servePlaylist(request, response);
    return;
  }
  response.writeHead(426, {'content-type': 'text/plain'});
  response.end('Upgrade Required');
});
const server = new WebSocketServer({
  server: httpServer,
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
  publicProfile: publicRoomProfile,
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
      disableFocus: url.searchParams.has('disableFocus') ? url.searchParams.get('disableFocus') === '1' : undefined,
      unrated: url.searchParams.get('unrated') === '1',
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
  // A spectator who stayed in the lobby can join after the initial grace
  // window once the confirmed PC-98 stream is available.
  const lateSpectator=url.searchParams.get('spectator');
  if(lateSpectator&&existingRun?.spectatorPc98&&existingRun.spectatorAdmissionOpen&&
    existingRoom.lobby.spectators.has(lateSpectator)&&existingRoom.lobbyClients.has(lateSpectator)&&
    memberId&&roomDirectory.memberIdFor(roomId,lateSpectator)===memberId){
    existingRun.admittedSpectators.add(lateSpectator);existingRun.memberIds.set(lateSpectator,memberId);
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
    socket.rtcStartup = url.searchParams.get('rtcStartup') === '1';
    const iceChoice = url.searchParams.get('rtcRoute');
    socket.hasInitialIceChoice = ['auto','direct','swarm','turn'].includes(iceChoice);
    socket.initialIceChoice = socket.hasInitialIceChoice ? iceChoice : 'auto';
    void handleSignalConnection(socket, roomId, runId, player, playerCount).catch(() => socket.close(1011, 'signaling unavailable'));
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
  const acceptPc98Spectator=createRelayMessageGate({maxBytes:RELAY_LIMITS.gameplayBytes,messagesPerSecond:120,bytesPerSecond:512*1024});
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
    const pc98Lane=isBinary&&player===0&&run.clients.get(player)===socket&&isPc98SpectatorRoom(roomId)&&data.length>=5&&data[0]===0xe8&&data[1]===0x50&&data[2]===0x39&&data[3]===0x38&&data[4]===0x53;
    if(pc98Lane){if(acceptPc98Spectator(data.byteLength)){stopSpectatorStream(run);delete run.spectatorPc98;return;}}
    else if (!acceptRelayMessage(socket, data, stageInitializer ? acceptStageInitializer : acceptGameplay)) return;
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
      if(pc98Lane&&payload.length>=4&&payload[0]===0x50&&payload[1]===0x39&&payload[2]===0x38&&payload[3]===0x53){
        try{
          const first=!run.spectatorPc98,stream=run.spectatorPc98||new Pc98SpectatorRelay(run.playerCount,{matchInitializers:/^th0[234]mp-/.test(roomId)}),kind=stream.accept(payload);
          if(run.spectatorGraceTimer){clearTimeout(run.spectatorGraceTimer);run.spectatorGraceTimer=null;}
          run.spectatorPc98=stream;run.spectatorTimeline=true;run.spectatorAdmissionOpen=!stream.ended&&!stream.pendingMatch;run.spectatorComplete=stream.ended;
          run.spectatorHistory=stream.history;run.spectatorHistoryBytes=stream.bytes;run.onSpectatorAvailabilityChange=()=>roomDirectory.changed();
          for(const [id,target] of run.spectatorClients){
            if(target.spectatorReplaying){
              if(target.spectatorSnapshotPending)continue;
              target.spectatorQueuedPc98Bytes=(target.spectatorQueuedPc98Bytes||0)+payload.length;
              if(target.spectatorQueuedPc98Bytes>128*1024*1024){target.close(1008,'spectator history too slow');continue;}
              (target.spectatorQueuedPc98??=[]).push(Buffer.from(payload));continue;
            }
            if(sendSpectatorPayload(target,payload,roomId,runId,id)&&first)sendSpectatorPayload(target,stream.bootstrap(),roomId,runId,id);
          }
          if(first||kind===3||kind===6||kind===7){broadcastLobby(room);roomDirectory.changed();}
          if(kind===6&&!stream.pendingMatch)for(const id of room.lobby.spectators.keys())if(!run.admittedSpectators.has(id)){
            const lobby=room.lobbyClients.get(id);if(!lobby)continue;
            run.admittedSpectators.add(id);run.memberIds.set(id,roomDirectory.memberIdFor(roomId,id));
            sendLobby(lobby,{type:'spectator-start',serial:room.lobby.startSerial,room:lobbySnapshot(room)});
          }
          if(first)for(const id of room.lobby.spectators.keys()){const lobby=room.lobbyClients.get(id);if(lobby)sendLobby(lobby,{type:'spectator-start',serial:room.lobby.startSerial,room:lobbySnapshot(room)});}
        }catch(error){console.log(`PC98 SPECTATOR STOP room=${roomId} run=${runId} ${error.message}`);stopSpectatorStream(run);delete run.spectatorPc98;}
        return;
      }
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
      // Native TH03 sends targeted JSON and requires a trusted sender seat.
      // Legacy untargeted DOSBox packets retain their existing framing.
      const outgoing = isPc98SpectatorRoom(roomId) && requestedPeer !== null && forwardedPayload[0] === 0x7b
        ? Buffer.concat([Buffer.from([0xe6, player]), forwardedPayload])
        : relayGameplayPayload(multiplayer,player,forwardedPayload);
      sendBounded(target, outgoing, { binary: true, delayMs: wait });
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
httpServer.listen(port, host);
