import { randomUUID, randomInt } from 'node:crypto';
import { mkdir, writeFile } from 'node:fs/promises';
import { resolve } from 'node:path';
import translations from '../../messages/zh-CN.json';
import { parseRoomAccountProfile } from '../../src/launcher/room-account-profile.mts';
import { QUICK_CHAT_ROWS } from '../../../common/contracts/multiplayer-quick-chat.mjs';
import {
  buildMultiplayerDiagnosticRelayUrl,
  buildMultiplayerDirectoryRelayUrl,
  buildMultiplayerGameplayRelayUrl,
  buildMultiplayerLobbyRelayUrl,
} from '../../src/launcher/multiplayer-relay-url.mts';
import { buildMultiplayerRuntimeOptions } from '../../src/launcher/multiplayer-runtime-options.mts';
import { createMultiplayerQuickChatModel } from '../../src/launcher/multiplayer-quick-chat-model.mts';
import { probeRelay } from '../../src/launcher/network-diagnostics.mts';
import { createRoomNetwork } from '../../src/launcher/room-network.mts';
import { createRoomMusic } from '../../app/services/room-music';
import { createMetingProvider, type MusicPlatform } from '../../app/services/music-provider';
import { NativeRoomAudio } from './room-audio.mts';
import { multiplayerConfigForProduct, multiplayerInputTimingPolicy } from '../../../common/contracts/product-catalog.mjs';

const product = 'th09mp';
const game = 'th09';
const catalog = multiplayerConfigForProduct(product);
const timingPolicy = multiplayerInputTimingPolicy(catalog);

const loadouts = catalog?.loadouts.map(item => ({label: translations[item.labelKey.replaceAll('.', '_')] ?? item.glyph})) ?? [];
const desktopCommands = new Set(['games', 'prepare_game', 'launch_game', 'swarm_ping', 'room_identity', 'open_account']);
const roomActions = new Set([
  'take-seat', 'stand-up', 'spectate', 'leave-spectator', 'set-name', 'set-loadout', 'set-ready',
  'settings', 'timing-settings', 'start', 'room-nudge', 'remove-player', 'move-player-to-spectator',
  'remove-spectator', 'quick-chat', 'free-chat', 'chat-report', 'room-probe', 'relay-preference',
  'listen-together', 'activity', 'resource-progress', 'th09-match',
]);

const samples = {
  rooms: [
    { code: '28160743', label: '花映塚对战', name: '月兔', seated: 1, playerCount: 2, phase: 'lobby' },
    { code: '58309216', label: '花映塚对战', name: '白玉楼', seated: 2, playerCount: 2, phase: 'playing' },
    { code: '90726158', label: '花映塚对战', name: '夜雀', seated: 1, playerCount: 2, phase: 'lobby' },
  ],
  room: {
    code: '28160743', phase: 'lobby', playerCount: 2, difficulty: 1, visibility: 'public', startSerial: 0,
    seats: [
      { name: '月兔', clientId: 'local', ready: true, nativeRuntime: true, loadout: {character: 0, shot: 0} },
      { name: '白玉楼', clientId: 'guest', ready: true, nativeRuntime: false, loadout: {character: 1, shot: 0} },
    ],
    spectators: [{name: '夜雀', clientId: 'spectator'}],
    swarmAvailable: true,
    swarmWorkers: [{id: 0, name: '本地低延迟节点', addresses: ['127.0.0.1']}],
  },
  quickChat: QUICK_CHAT_ROWS, loadouts,
  chat: [{type: 'quick-chat', name: '白玉楼', phrase: 'thanks'}, {type: 'free-chat', name: '月兔', text: '下一局也继续吧。'}],
  clientId: 'local', status: '演示数据',
};

type Envelope = Record<string, any>;
const sampleState = () => structuredClone(samples) as Envelope;

async function fixtures(directory: string) {
  await mkdir(directory, {recursive: true});
  for (const scene of ['lobby', 'room', 'game']) {
    const state = sampleState();
    state.scene = scene;
    if (scene === 'lobby') state.room = null;
    if (scene === 'game') { state.room.phase = 'playing'; state.room.startSerial = 1; state.chatOpen = true; }
    await writeFile(resolve(directory, scene + '.json'), JSON.stringify(state));
  }
}

function createPort(send: (state: Envelope) => void, relay?: string, endpoint?: string, site?: string, peerConnection?: typeof RTCPeerConnection, audio?: NativeRoomAudio, musicFetch: typeof fetch = fetch) {
  let state: Envelope = relay
    ? {rooms: [], room: null, quickChat: QUICK_CHAT_ROWS, chat: [], freeChat: false, status: '正在连接', scene: 'lobby'}
    : {...sampleState(), scene: 'lobby'};
  let id = relay ? randomUUID().replaceAll('-', '') : 'local';
  let member = relay ? randomUUID().replaceAll('-', '') : id;
  state.clientId = id;
  state.loadouts = loadouts;
  state.manualDelayLimit = timingPolicy.manualDelayLimit;
  let directory: WebSocket | undefined, room: WebSocket | undefined;
  let gameStatus: Envelope | undefined;
  let roomCode = '', preferTurn = false, useSwarm = false, measuredWorkers = '';
  let attachedOrigin = '';
  let launchedSerial = 0, launchingSerial = 0, failedSerial = 0;
  let reconnectRoom: ReturnType<typeof setTimeout> | undefined, reconnectDirectory: ReturnType<typeof setTimeout> | undefined;
  let roomRetry = 0, directoryRetry = 0;
  let closed = false;
  let chatConnected = false;
  let diagnosticRunning = false;
  const canOpenAccount = !!endpoint && process.env.EAGLER_DESKTOP_ACCOUNT === '1';
  state.canOpenAccount = canOpenAccount;
  let refreshIdentity: (() => Promise<boolean>) | undefined;
  let accountPolling = false;
  let profileKeys = '';
  let profileAssets: AbortController | undefined;
  let lastRatingsAt = 0;
  let probePeer = '';
  let chatReceiptId = 0;
  let submittedText = '';
  const chatMessages: Record<string, string> = {
    reported: translations.chat_reported,
    'report-rate-limited': translations.chat_reportRateLimited,
    'report-in-progress': translations.chat_reportInProgress,
    'chat-banned': translations.chat_banned,
    'send-rate-limited': translations.chat_sendRateLimited,
    'sign-in-required': translations.chat_signInRequired,
    'review-unavailable': translations.chat_reviewUnavailable,
  };
  const chat = relay ? createMultiplayerQuickChatModel({
    send: transmit, voice: () => {}, timers: globalThis,
    notify(code) { state.chatNotice = chatMessages[code] ?? translations.chat_failed; publish(); },
  }) : null;
  const publish = () => send(state);
  const provider = audio ? createMetingProvider('https://open.motues.top/music', musicFetch) : undefined;
  const music = audio && provider ? createRoomMusic({
    provider, storage:null, createAudio:() => audio,
    onAudibleChange:audible => send({type:'room-music-audible', audible}),
    send(message) {
      if (room?.readyState !== WebSocket.OPEN) return false;
      try { transmit(message); return true; }
      catch { return false; }
    },
  }) : undefined;
  let musicSearch: AbortController | undefined;
  music?.subscribe(() => {
    const snapshot = music.getSnapshot();
    state.music = {...snapshot, offer:music.getPendingInvitation()?.track};
    publish();
  });
  function syncMusic() {
    if (!music) return;
    const info = state.room;
    const seat = info?.seats?.findIndex((entry: Envelope | null) => entry?.clientId === id) ?? -1;
    music.update({room:info, product, seat:seat < 0 ? null : seat, connected:chatConnected,
      localClientId:id, listenTogetherSupported:info?.listenTogether === true});
  }
  function resetMusicSearch() {
    musicSearch?.abort();
    musicSearch = undefined;
    delete state.musicResults;
    delete state.musicSearch;
  }
  async function searchMusic(data: Envelope) {
    if (!music || !provider || !state.room) return;
    const query = typeof data.query === 'string' ? data.query.trim().slice(0, 500) : '';
    if (!query) return;
    const platform: MusicPlatform = data.platform === 'qq' ? 'qq' : 'netease';
    resetMusicSearch();
    const request = new AbortController();
    musicSearch = request;
    state.musicSearch = '正在查询';
    publish();
    try {
      const results = await provider.search(platform, query, request.signal);
      if (musicSearch !== request) return;
      state.musicResults = results;
      state.musicSearch = results.length ? '' : '没有找到歌曲';
    } catch {
      if (musicSearch !== request) return;
      state.musicSearch = '查询失败';
    } finally {
      if (musicSearch === request) musicSearch = undefined;
    }
    publish();
  }
  const probes = relay && peerConnection ? createRoomNetwork({
    peerConnection,
    send: message => {
      if (room?.readyState !== WebSocket.OPEN) return false;
      try {
        room.send(JSON.stringify(message));
        return true;
      } catch {
        return false;
      }
    },
    changed: publishProbes,
  }) : undefined;
  function updateProbeSnapshot() {
    if (!probes || !probePeer) {
      delete state.probes;
      return;
    }
    const lanes = ['direct', 'turn'] as const;
    state.probes = Object.fromEntries(lanes.map(lane => [lane, {
      ready:probes.lossReady(probePeer, lane), metric:probes.metric(probePeer, lane), loss:probes.loss(probePeer, lane),
    }]));
  }
  function publishProbes() {
    updateProbeSnapshot();
    publish();
  }
  function syncProbes() {
    if (!probes) return;
    const seats: string[] = (state.room?.seats ?? []).flatMap((seat: Envelope | null) =>
      seat?.clientId && !seat.offline ? [seat.clientId] : []);
    const active = chatConnected && state.room?.phase === 'lobby' && state.scene !== 'game' && seats.includes(id);
    probePeer = active ? seats.find(peer => peer !== id) ?? '' : '';
    probes.update({active, localId:id, peers:seats});
    updateProbeSnapshot();
  }
  let displayedEntries = chat?.getSnapshot().entries;
  chat?.subscribe(() => {
    const current = chat.getSnapshot();
    state.freeChat = current.registered && current.bannedUntil !== -1 && current.bannedUntil <= Date.now() &&
      (current.context?.localSeat != null || current.context?.localSpectator === true);
    state.chatSending = current.sending;
    state.chatAuthor = current.authorId;
    state.accountRegistered = current.registered;
    if (current.entries !== displayedEntries) {
      displayedEntries = current.entries;
      state.chat = current.entries.map(entry => ({
        id: entry.id, name: entry.name, clientId: entry.clientId, authorId: entry.authorId,
        messageId: entry.messageId, expiredInGame: entry.expiredInGame,
        ...(entry.rating ? {rating: entry.rating.changes} :
          entry.messageId ? {text: entry.phrase.zh} : {phrase: entry.phrase.id}),
      }));
    }
    publish();
  });
  function syncChat() {
    const info = state.room;
    const seats = info?.seats ?? [];
    const spectators = info?.spectators ?? [];
    const seat = seats.findIndex((entry: Envelope | null) => entry?.clientId === id);
    chat?.update({visible: Boolean(info), room: info ? `${product}-${info.code}` : '', serial: info?.startSerial ?? 0,
      seats, spectators, localSeat: seat < 0 ? null : seat, localClientId: id,
      localSpectator: spectators.some((entry: Envelope) => entry.clientId === id),
      connected: chatConnected, inRoom: state.scene !== 'game', language: 'zh-CN'});
  }
  function syncPlayerProfiles(force = false) {
    if (!site) return;
    const profiles = (state.room?.seats ?? [])
      .map((seat: Envelope | null) => parseRoomAccountProfile(seat?.profile))
      .filter(Boolean);
    const key = JSON.stringify(profiles.map((profile: Envelope) => [profile.username, profile.avatar]));
    if (!force && key === profileKeys && Date.now() - lastRatingsAt < 12_000) return;
    if (key !== profileKeys) {
      delete state.ratings;
      delete state.ratingCards;
      delete state.avatars;
    }
    profileKeys = key;
    profileAssets?.abort();
    profileAssets = undefined;
    if (!profiles.length) return;
    const controller = new AbortController();
    profileAssets = controller;
    lastRatingsAt = Date.now();
    const signal = AbortSignal.any([controller.signal, AbortSignal.timeout(5000)]);
    const names = profiles.map((profile: Envelope) => profile.username as string);
    const ratingsUrl = new URL('/api/v1/th09/ratings', site);
    ratingsUrl.searchParams.set('cards', names.join(','));
    void (async () => {
      try {
        const response = await fetch(ratingsUrl, {signal, redirect: 'error'});
        if (!response.ok) return;
        const result = await response.json() as Envelope;
        if (controller.signal.aborted || closed) return;
        state.ratings = Object.fromEntries(names.flatMap((name: string) => {
          const entry = result.players?.[name];
          return entry && Number.isSafeInteger(entry.rating) && Number.isSafeInteger(entry.games)
            ? [[name, {rating: entry.rating, games: entry.games}]] : [];
        }));
        state.ratingCards = Object.fromEntries(names.flatMap((name: string) => {
          const entry = result.cards?.[name];
          return Array.isArray(entry?.characters) ? [[name, entry.characters.slice(0, 3).filter(
            (pair: unknown) => Array.isArray(pair) && pair.length === 2 && Number.isInteger(pair[0]) &&
              pair[0] >= 0 && pair[0] < 16 && Number.isSafeInteger(pair[1]))]] : [];
        }));
        publish();
      } catch { /* A room remains usable if the public ratings service is unavailable. */ }
    })();
    for (const {username, avatar} of profiles) {
      if (!avatar || state.avatars?.[avatar]) continue;
      const url = new URL('/api/v1/profiles/' + encodeURIComponent(username) + '/avatar/' + avatar, site);
      void (async () => {
        try {
          const response = await fetch(url, {signal, redirect: 'error'});
          const type = response.headers.get('Content-Type');
          if (!response.ok || (type !== 'image/webp' && type !== 'image/jpeg') || !response.body) return;
          const buffer = Buffer.alloc(8192);
          let size = 0;
          for await (const chunk of response.body) {
            if (size + chunk.byteLength > buffer.length) return;
            buffer.set(chunk, size);
            size += chunk.byteLength;
          }
          if (!size || controller.signal.aborted || closed) return;
          (state.avatars ??= Object.create(null))[avatar] = {
            type, data: buffer.subarray(0, size).toString('base64'),
          };
          publish();
        } catch { /* The overlay keeps the player's initial when an avatar is unavailable. */ }
      })();
    }
  }
  function accountRelay(relayUrl: string) {
    if (!endpoint || !site) return false;
    const page = new URL(site);
    const target = new URL(relayUrl);
    if (page.protocol === 'http:' && page.hostname === '127.0.0.1')
      return target.protocol === 'ws:' && target.host === page.host;
    return page.protocol === 'https:' && target.protocol === 'wss:' &&
      (target.host === page.host || target.host === 'relay.' + page.host);
  }
  function newLobbySocket(url: string) {
    // A redirected WebSocket can have a different origin from its room URL.
    // Only a direct connection to the configured account Relay receives a ticket.
    return accountRelay(url) ? new WebSocket(url, {followRedirects:false}) : new WebSocket(url);
  }
  function error(reason: unknown) {
    state.status = reason instanceof Error ? reason.message : String(reason);
    publish();
  }
  function leaveRoom(status = relay ? '已连接' : '演示数据') {
    clearTimeout(reconnectRoom);
    const prior = room;
    room = undefined;
    roomCode = '';
    refreshIdentity = undefined;
    chatConnected = false;
    probePeer = '';
    delete state.probes;
    probes?.reset();
    prior?.close(1000, 'leave room');
    state.room = null;
    syncPlayerProfiles();
    state.roomConnected = false;
    delete state.launchStage;
    delete state.runtimeError;
    delete state.finishingRun;
    state.chat = relay ? [] : sampleState().chat;
    delete state.chatReceipt;
    delete state.chatNotice;
    state.music = null;
    resetMusicSearch();
    state.preferSwarm = false;
    state.scene = 'lobby';
    state.status = status;
    syncChat();
    syncMusic();
  }
  async function desktop(command: string, args: Envelope) {
    if (!desktopCommands.has(command)) throw Error('未知桌面命令');
    if (!endpoint) {
      if (relay) throw Error('需要由桌面启动器提供本地接口');
      return command === 'games' ? [] : command === 'swarm_ping' ? [] : {};
    }
    const url = new URL(endpoint);
    if (url.protocol !== 'http:' || url.hostname !== '127.0.0.1') throw Error('桌面接口必须使用本机回环');
    const response = await fetch(url, {
      redirect: 'error', method: 'POST', headers: {Origin: 'eagler-overlay://panel', 'Content-Type': 'application/json'},
      body: JSON.stringify({command, args}), signal: AbortSignal.timeout(15000),
    });
    const json = await response.json() as Envelope;
    if (!response.ok) throw Error(json.error || '桌面操作失败');
    return json;
  }

  async function rankWorkers(workers: Envelope[]) {
    const addresses = workers.map(worker => worker.address || worker.ip || worker.addresses?.[0]);
    if (!workers.length || addresses.some(ip => !ip)) return;
    const key = JSON.stringify(workers.map((worker,index) => [worker.id, addresses[index]]));
    if (key === measuredWorkers) return;
    measuredWorkers = key;
    const activeRoom = room;
    try {
      const result = await desktop('swarm_ping', {addresses});
      if (room !== activeRoom || measuredWorkers !== key || !useSwarm) return;
      state.swarmPings = result;
      const best = result.reduce((current: number, ms: number | null, index: number) =>
        typeof ms === 'number' && (current < 0 || ms < result[current]) ? index : current, -1);
      if (best >= 0) transmit({type: 'relay-preference', swarm: true, worker: workers[best].id, automatic: true});
      publish();
    } catch (reason) {
      measuredWorkers = '';
      error(reason);
    }
  }

  function connectDirectory() {
    if (!relay || closed) return;
    clearTimeout(reconnectDirectory);
    if (directory?.readyState === WebSocket.OPEN) {
      directory.send(JSON.stringify({type: 'refresh', product}));
      return;
    }
    if (directory?.readyState === WebSocket.CONNECTING) return;
    const socket = new WebSocket(buildMultiplayerDirectoryRelayUrl(relay, member));
    directory = socket;
    socket.addEventListener('open', () => {
      if (directory !== socket) return;
      directoryRetry = 0;
      socket.send(JSON.stringify({type: 'refresh', product}));
      state.status = '已连接';
      publish();
    });
    socket.addEventListener('message', event => {
      if (directory !== socket) return;
      const data = JSON.parse(String(event.data));
      if (data.type === 'directory') {
        state.rooms = Array.isArray(data.rooms) ? data.rooms.filter((entry: Envelope) => entry.product === product).map((entry: Envelope) => ({
          ...entry, phase: entry.phase === 'lobby' ? 'lobby' : 'playing',
          seated: Array.isArray(entry.seats) ? entry.seats.filter(Boolean).length : 0,
          playerCount: entry.capacity ?? 2,
        })) : [];
        publish();
      }
    });
    socket.addEventListener('error', () => { if (directory === socket) error('大厅连接失败'); });
    socket.addEventListener('close', () => {
      if (directory !== socket || closed) return;
      directory = undefined;
      error('大厅正在重连');
      reconnectDirectory = setTimeout(connectDirectory, Math.min(1500 * 2 ** directoryRetry++, 15000));
    });
  }

  async function launch(serial: number, spectator = false) {
    const info = state.room;
    const seat = info?.seats?.findIndex((entry: Envelope | null) => entry?.clientId === id) ?? -1;
    if (!relay || !info || (!spectator && seat < 0) || !serial || launchedSerial === serial ||
        launchingSerial === serial || failedSerial === serial) return;
    const connection = room;
    const code = roomCode;
    launchingSerial = serial;
    delete state.runtimeError;
    state.launchStage = '正在检查游戏资源…';
    publish();
    try {
      gameStatus ??= await desktop('prepare_game', {game});
      if (room !== connection || roomCode !== code || launchingSerial !== serial) return;
      if (gameStatus?.missing?.length) throw Error('请先补齐游戏资源');
      const config = multiplayerConfigForProduct(product);
      if (!config) throw Error('找不到花映塚联机配置');
      const url = buildMultiplayerGameplayRelayUrl(relay, {
        product, roomCode: code, runId: serial, role: spectator ? {spectator: id} : {player: seat}, memberId: member,
        runtimeGeneration: info.runtimeGeneration || gameStatus?.roomGeneration,
      });
      const options = buildMultiplayerRuntimeOptions({
        url, player: spectator ? 0 : seat, playerCount: 2, spectator, spectatorId: spectator ? id : '',
        spectatorTimeline: info.spectatorTimeline === true, seed: Number.parseInt(code, 10) & 0xffff, difficulty: info.difficulty,
        loadouts: info.seats.slice(0, 2).map((entry: Envelope, index: number) => config.loadouts[entry?.loadout ?? index] ?? config.loadouts[0]),
        disableLocalChargePriority: info.disableLocalChargePriority === true,
        inputDelay: info.inputDelay ?? 0, adonisMode: info.adonisMode ?? 0,
        inputDelayAuto: info.inputDelayAuto === true, predictionReserve: info.predictionReserve ?? 2,
        preferTurn: preferTurn && info.swarmSelected !== true,
      }, config);
      state.launchStage = '正在启动原生联机…';
      publish();
      await desktop('launch_game', {game, room: options});
      if (room !== connection || launchingSerial !== serial) return;
      launchedSerial = serial;
      state.scene = 'game';
      syncChat();
      publish();
    } catch (reason) {
      if (room === connection && launchingSerial === serial) {
        failedSerial = serial;
        state.runtimeError = reason instanceof Error ? reason.message : String(reason);
      }
    } finally {
      if (room === connection && launchingSerial === serial) {
        launchingSerial = 0;
        delete state.launchStage;
        publish();
      }
    }
  }

  function enter(code: string, intent: 'join' | 'create', spectator = false, settings: Envelope = {}, reconnecting = false) {
    if (closed) return;
    clearTimeout(reconnectRoom);
    if (reconnecting) {
      launchingSerial = 0;
      delete state.launchStage;
    }
    if (!/^\d{4,8}$/.test(code)) throw Error('请输入 4 至 8 位房间号');
    if (!relay) {
      state.room = {...sampleState().room, ...settings, code};
      state.scene = 'room';
      publish();
      return;
    }
    room?.close(1000, 'leave room');
    refreshIdentity = undefined;
    chatConnected = false;
    state.roomConnected = false;
    probePeer = '';
    delete state.probes;
    probes?.reset(reconnecting);
    roomCode = code;
    if (!reconnecting) {
      roomRetry = 0;
      measuredWorkers = '';
      state.swarmPings = [];
      launchedSerial = launchingSerial = failedSerial = 0;
      delete state.launchStage;
      delete state.runtimeError;
      delete state.finishingRun;
      state.room = null;
      syncPlayerProfiles();
      state.chat = [];
      delete state.chatReceipt;
      delete state.chatNotice;
      state.music = null;
      resetMusicSearch();
      state.preferSwarm = false;
      state.freeChat = false;
      state.scene = 'room';
    }
    state.status = reconnecting ? '房间正在重连' : '正在进入房间';
    syncChat();
    syncMusic();
    publish();
    const {url} = buildMultiplayerLobbyRelayUrl(relay, {
      product, roomCode: code, clientId: id, memberId: member, intent, playerCount: 2,
      difficulty: settings.difficulty ?? 1, visibility: settings.visibility ?? 'public', unrated: settings.unrated === true,
    });
    const socket = newLobbySocket(url);
    room = socket;
    let entryPending = true;
    let joined = reconnecting;
    let identityRequested = false;
    let entryError = '';
    async function requestIdentity(challenge: string) {
      for (const retryDelay of [0, 350, 1100]) {
        if (retryDelay) await Bun.sleep(retryDelay);
        if (room !== socket || closed || socket.readyState !== WebSocket.OPEN) return false;
        try {
          const result = await desktop('room_identity', {challenge});
          if (room !== socket || closed || socket.readyState !== WebSocket.OPEN || typeof result.ticket !== 'string')
            return false;
          socket.send(JSON.stringify({type:'account-nickname', ticket:result.ticket}));
          return true;
        } catch { /* Transient account service errors leave quick chat available. */ }
      }
      return false;
    }
    socket.addEventListener('open', () => {
      if (room !== socket) return;
      state.status = '正在同步房间成员…';
      publish();
    });
    socket.addEventListener('message', event => {
      if (room !== socket) return;
      const data = JSON.parse(String(event.data));
      if (data.type === 'error') {
        const message = typeof data.error === 'string' && data.error ? data.error : '房间操作失败';
        if (entryPending) entryError = message;
        error(message);
        return;
      }
      if (!identityRequested && data.type === 'state' && typeof data.accountChallenge === 'string' && accountRelay(socket.url) &&
          data.accountChallenge.length > 0 && data.accountChallenge.length <= 1024) {
        identityRequested = true;
        const challenge: string = data.accountChallenge;
        refreshIdentity = () => requestIdentity(challenge);
        void refreshIdentity();
      }
      if (data.room && typeof data.room === 'object' && Array.isArray(data.room.seats)) {
        if (data.roomProbe) void probes?.receive({type:'room-probe-config', iceServers:data.roomProbe.iceServers});
        state.room = {...data.room, code, listenTogether: data.listenTogether === 2 || state.room?.listenTogether === true};
        syncPlayerProfiles();
        if (entryPending) state.status = '已连接';
        if (entryPending && data.type === 'state') {
          entryPending = false;
          joined = true;
          chatConnected = true;
          state.roomConnected = true;
          roomRetry = 0;
          if (!reconnecting && spectator) transmit({type: 'spectate', name: state.name || '东方玩家'});
          else if (!reconnecting && intent === 'create') transmit({type: 'take-seat', seat: 0, loadout: 0, name: state.name || '东方玩家', movementMode: 'joystick', touchEnabled: false, nativeRuntime: true});
        }
        if (data.room.phase === 'lobby') {
          if (state.scene !== 'game') state.scene = 'room';
          delete state.finishingRun;
          const own = data.room.seats.find((entry: Envelope | null) => entry?.clientId === id);
          useSwarm = own?.preferSwarm === true;
          state.preferSwarm = useSwarm;
          if (useSwarm) void rankWorkers(data.room.swarmWorkers ?? []);
        }
        syncChat();
        syncProbes();
        syncMusic();
      }
      if (data.type === 'start') {
        const observing = state.room?.spectators?.some((entry: Envelope) => entry.clientId === id);
        if (!observing) void launch(Number(data.serial)).catch(error);
        else transmit({type: 'spectate', name: state.name || '东方玩家'});
      }
      if (data.type === 'state' && data.room?.phase === 'starting') void launch(Number(data.room.startSerial)).catch(error);
      if (data.type === 'spectator-start') void launch(Number(data.serial), true).catch(error);
      if (data.type === 'room-action' && data.action === 'nudge' && data.outgoing !== true) error('队友提醒你准备开局');
      if (chat && ['chat-status', 'chat-history', 'quick-chat', 'free-chat', 'chat-retract', 'chat-result', 'th09-rating'].includes(data.type)) {
        const before = chat.getSnapshot();
        if (data.type === 'chat-result' && data.action === 'free-chat' && data.ok === true && before.sending) {
          state.chatReceipt = {id: ++chatReceiptId, text: submittedText};
          delete state.chatNotice;
        }
        chat.receive(data);
        if (data.type === 'th09-rating') syncPlayerProfiles(true);
      }
      if (data.type === 'listen-together') music?.receive(data);
      if (data.type === 'room-probe-config') {
        void probes?.receive(data);
        syncProbes();
      } else if (data.type === 'room-probe' && probes) void probes.receive(data);
      publish();
    });
    socket.addEventListener('error', () => {
      if (room !== socket) return;
      if (entryPending && !entryError) entryError = '等待室连接失败';
      error(entryPending ? entryError : '等待室连接失败');
    });
    socket.addEventListener('close', event => {
      if (room !== socket || closed) return;
      if (!joined || [4007, 4008, 4009, 4010].includes(event.code)) {
        leaveRoom(entryError || '等待室连接已断开');
        publish();
        return;
      }
      room = undefined;
      chatConnected = false;
      state.roomConnected = false;
      delete state.launchStage;
      syncChat();
      syncProbes();
      syncMusic();
      error('房间正在重连');
      reconnectRoom = setTimeout(() => enter(code, 'join', spectator, settings, true),
        Math.min(750 * 2 ** roomRetry++, 6000));
    });
  }

  function transmit(message: Envelope) {
    if (!roomActions.has(message.type)) throw Error('未知房间操作');
    if (relay) {
      if (room?.readyState !== WebSocket.OPEN) throw Error('等待室正在连接');
      room.send(JSON.stringify(message));
      return;
    }
    const info = state.room;
    if (!info) return;
    const seat = info.seats.findIndex((entry: Envelope | null) => entry?.clientId === id);
    const own = info.seats[seat];
    switch (message.type) {
      case 'set-ready': if (own) own.ready = message.ready; break;
      case 'set-name': if (own) own.name = message.name; break;
      case 'set-loadout': if (own) own.loadout = message.loadout; break;
      case 'take-seat': if (!info.seats[message.seat]) info.seats[message.seat] = {clientId: id, name: '月兔', nativeRuntime: true, ready: false}; break;
      case 'stand-up': if (seat >= 0) info.seats[seat] = null; break;
      case 'spectate': if (!info.spectators.some((entry: Envelope) => entry.clientId === id)) info.spectators.push({clientId: id, name: '月兔'}); break;
      case 'leave-spectator': info.spectators = info.spectators.filter((entry: Envelope) => entry.clientId !== id); break;
      case 'remove-player': if (seat === 0 && message.seat === 1) info.seats[1] = null; break;
      case 'move-player-to-spectator': if (seat === 0 && info.seats[message.seat]) {info.spectators.push(info.seats[message.seat]); info.seats[message.seat] = null;} break;
      case 'remove-spectator': info.spectators = info.spectators.filter((entry: Envelope) => entry.clientId !== message.clientId); break;
      case 'settings': if (seat === 0) Object.assign(info, message, {type: undefined}); break;
      case 'timing-settings': if (seat === 0) Object.assign(info, {
        adonisMode: message.rollbackEnabled ? 0 : 1, timingChoice: message.timingChoice,
        inputDelay: message.rollbackEnabled || message.timingChoice === 'auto' ? 0 : message.timingChoice,
        inputDelayAuto: !message.rollbackEnabled && message.timingChoice === 'auto',
      }); break;
      case 'relay-preference': info.swarmSelected = state.preferSwarm = message.swarm; break;
      case 'start': if (seat === 0 && info.seats.every((entry: Envelope) => entry?.ready)) {
        info.phase = 'playing'; info.startSerial++; state.scene = 'game';
      } break;
      case 'free-chat': case 'quick-chat':
        state.chat = [...state.chat.slice(-49), {...message, name: own?.name ?? '月兔'}]; break;
      case 'listen-together': state.music = {action: message.action}; break;
      case 'room-probe': state.status = '演示 | 直连 18 ms | TURN 31 ms'; break;
      case 'room-nudge': state.status = '已提醒'; break;
      case 'chat-report': state.status = '演示 | 已提交举报'; break;
    }
    publish();
  }

  async function action(data: Envelope) {
    switch (data.type) {
      case 'start': {
        const info = state.room;
        if (!info) return;
        const rollback = timingPolicy.rollback && info.adonisMode !== 1;
        const automatic = !rollback && timingPolicy.measuredStartup && info.timingChoice === 'auto';
        const inputDelay = rollback || automatic ? 0 : info.inputDelay ?? 0;
        transmit({type: 'start', adonisMode: rollback ? 0 : 1, inputDelay,
          inputDelayAuto: automatic, predictionReserve: 2,
          ...(catalog?.inputTiming?.sendPredictionLimit != null ? {predictionLimit: timingPolicy.predictionLimit} : {})});
        break;
      }
      case 'match-result':
        if (relay && launchedSerial && launchedSerial === state.room?.startSerial)
          transmit({...data, type: 'th09-match', serial: launchedSerial});
        break;
      case 'free-chat':
        if (chat) {
          if (!state.freeChat || chat.getSnapshot().sending || typeof data.text !== 'string') break;
          delete state.chatNotice;
          submittedText = data.text;
          chat.setDraft(data.text);
          chat.sendText();
        } else if (typeof data.text === 'string' && data.text.trim()) {
          transmit({...data, text: data.text.trim()});
          state.chatReceipt = {id: ++chatReceiptId, text: data.text};
        }
        break;
      case 'chat-report':
        if (chat) chat.reportMessage(data.messageId, data.reason);
        else transmit(data);
        break;
      case 'directory': connectDirectory(); break;
      case 'adopt-room': {
        if (!relay) break;
        const options = data.options;
        if (!options || typeof options.netplayUrl !== 'string') throw Error('启动器房间配置无效');
        const gameUrl = new URL(options.netplayUrl);
        const roomId = gameUrl.searchParams.get('room');
        const relayUrl = new URL(relay);
        if (!['ws:', 'wss:'].includes(gameUrl.protocol) || gameUrl.username || gameUrl.password ||
            !roomId?.startsWith(product + '-')) throw Error('启动器房间地址无效');
        if (attachedOrigin && attachedOrigin !== gameUrl.origin) throw Error('启动器房间地址已改变');
        if (gameUrl.origin !== relayUrl.origin || gameUrl.pathname !== relayUrl.pathname) {
          // The launcher may resolve the manifest through a configured Relay route.
          const effectiveRelay = buildMultiplayerDirectoryRelayUrl(gameUrl.href, member);
          relay = effectiveRelay;
        }
        attachedOrigin = gameUrl.origin;
        const lobbyId = options.lobbyClientId;
        const membership = gameUrl.searchParams.get('member');
        const code = roomId.slice(product.length + 1);
        const serial = Number(gameUrl.searchParams.get('run'));
        if (typeof lobbyId !== 'string' || typeof membership !== 'string' ||
            !/^[A-Za-z0-9_-]{8,64}$/.test(lobbyId) || !/^[A-Za-z0-9_-]{8,64}$/.test(membership) ||
            !/^\d{4,8}$/.test(code) || !Number.isSafeInteger(serial) || serial < 1) throw Error('启动器房间身份无效');
        const oldDirectory = directory;
        directory = undefined;
        oldDirectory?.close();
        id = lobbyId;
        member = membership;
        state.clientId = id;
        enter(code, 'join', options.netplaySpectator === true);
        launchedSerial = serial;
        state.scene = 'game';
        syncChat();
        connectDirectory();
        break;
      }
      case 'create': enter(String(randomInt(10000000,100000000)), 'create', false, data); break;
      case 'join': enter(data.code, 'join', data.spectator === true); break;
      case 'leave': leaveRoom(); break;
      case 'runtime-return':
        if (launchingSerial) failedSerial = launchingSerial;
        launchingSerial = 0;
        delete state.launchStage;
        state.finishingRun = state.room?.phase === 'starting' || state.room?.phase === 'running' &&
          state.room.seats.some((entry: Envelope | null) => entry?.clientId === id);
        state.scene = roomCode || state.room ? 'room' : 'lobby';
        if (typeof data.message === 'string' && data.message) state.runtimeError = data.message;
        else delete state.runtimeError;
        syncChat();
        syncProbes();
        break;
      case 'dismiss-runtime-error':
        delete state.runtimeError;
        break;
      case 'open-account': {
        if (!canOpenAccount) break;
        await desktop('open_account', {});
        const refresh = refreshIdentity;
        if (refresh && !accountPolling) {
          accountPolling = true;
          void (async () => {
            try {
              const deadline = Date.now() + 120_000;
              while (!closed && refreshIdentity === refresh && Date.now() < deadline) {
                if (chat?.getSnapshot().registered || await refresh()) break;
                await Bun.sleep(2500);
              }
            } finally { accountPolling = false; }
          })();
        }
        break;
      }
      case 'return-room':
        if (relay) throw Error('原生回房由当前游戏进程处理');
        state.scene = 'room'; state.room.phase = 'lobby'; break;
      case 'prefer-turn':
        preferTurn = data.enabled === true;
        if (preferTurn && useSwarm) {
          useSwarm = false;
          transmit({type: 'relay-preference', swarm: false});
        }
        break;
      case 'relay-preference':
        useSwarm = data.swarm === true;
        if (useSwarm) preferTurn = false;
        transmit(data);
        if (relay && useSwarm && data.worker === undefined) {
          measuredWorkers = '';
          void rankWorkers(state.room?.swarmWorkers ?? []);
        }
        break;
      case 'test-loss': probes?.testLoss(); break;
      case 'room-audio': audio?.receive({...data, type:data.event} as Parameters<NativeRoomAudio['receive']>[0]); return;
      case 'music-search': void searchMusic(data); return;
      case 'music-track': {
        const track = state.musicResults?.[data.index];
        if (track) music?.choose(track, [track]);
        break;
      }
      case 'music-sync': music?.requestSync(); break;
      case 'music-leave': music?.leave(); break;
      case 'music-play':
        if (music?.getSnapshot().error) music.retry();
        else music?.togglePlayback();
        break;
      case 'diagnostic': {
        if (!relay) state.status = '演示 | 直连 18 ms | TURN 31 ms';
        else if (!diagnosticRunning) {
          diagnosticRunning = true;
          const url = buildMultiplayerDiagnosticRelayUrl(relay);
          state.status = '正在检测信令往返';
          void probeRelay(url).then(({latencyMs}) => {
            if (!closed) {
              state.status = '信令往返 ' + latencyMs + ' ms';
              publish();
            }
          }, () => {
            if (!closed) error('诊断连接失败');
          }).finally(() => { diagnosticRunning = false; });
        }
        break;
      }
      case 'desktop': {
        const result = await desktop(data.command, data.args || {});
        if (data.command === 'prepare_game') gameStatus = result;
        if (data.command === 'swarm_ping') state.swarmPings = result;
        state.status = '桌面操作已完成';
        break;
      }
      case 'set-ready':
        if (relay) {
          if (data.ready) {
            const preparingRoom = room;
            gameStatus ??= await desktop('prepare_game', {game});
            if (room !== preparingRoom) return;
            if (!gameStatus?.roomGeneration || gameStatus.missing?.length) throw Error('请先补齐兼容的原生运行包');
            transmit({type: 'resource-progress', status: 'ready', stage: 'runtime', percent: 100, nativeRuntime: true});
          }
          data = {...data, runtimeGeneration: gameStatus?.roomGeneration, swarmDirect: gameStatus?.swarmDirect === true,
            nativeRuntime: true, movementMode: 'joystick', touchEnabled: false, mobileDevice: false};
        }
        transmit(data);
        break;
      case 'take-seat': transmit({...data, nativeRuntime: relay ? true : undefined}); break;
      case 'set-name': state.name = data.name; transmit(data); break;
      default: transmit(data); break;
    }
    publish();
  }
  return {action, publish, error, close() {
    closed = true;
    profileAssets?.abort();
    clearTimeout(reconnectRoom);
    clearTimeout(reconnectDirectory);
    directory?.close();
    music?.dispose();
    resetMusicSearch();
    room?.close(1000, 'leave room');
    probes?.reset();
    chat?.dispose();
  }};
}


async function verifyRelay(relay: string) {
  let state: Envelope = {};
  let handoff: Envelope | undefined;
  const generation = 'b'.repeat(64);
  const desktopServer = Bun.serve({
    hostname: '127.0.0.1', port: 0,
    async fetch(request) {
      if (request.headers.get('origin') !== 'eagler-overlay://panel') return new Response(null, {status: 404});
      const {command, args} = await request.json() as Envelope;
      const status = {id: game, directory: 'fixture', missing: [], roomGeneration: generation, swarmDirect: false};
      if (command === 'games') return Response.json([status]);
      if (command === 'prepare_game') return Response.json(status);
      if (command === 'swarm_ping') return Response.json([12]);
      if (command === 'launch_game') {
        handoff = args;
        return Response.json({pid: 123, overlay: 'fixture'});
      }
      return new Response(null, {status: 404});
    },
  });
  const peerConnection = await loadPeerConnection();
  const verifyMusic = process.env.TH09_VERIFY_ROOM_MUSIC === '1';
  const played: string[] = [];
  const audio = verifyMusic ? new NativeRoomAudio(command => {
    played.push(command.action);
    if (command.action === 'load') queueMicrotask(() =>
      audio!.receive({type:'loadedmetadata', generation:command.generation, duration:120}));
    if (command.action === 'play') queueMicrotask(() =>
      audio!.receive({type:'playing', generation:command.generation, position:1}));
  }) : undefined;
  const fixtureMusicFetch = (async (input: URL | RequestInfo) => {
    const query = new URL(String(input)).searchParams;
    const track = {source:'netease', id:'101', name:'测试歌曲', artist:['测试']};
    if (query.get('type') === 'url') return Response.json({url:'https://m1.music.126.net/test.mp3'});
    if (query.get('type') === 'details') return Response.json(track);
    return Response.json([track]);
  }) as typeof fetch;
  let audible = false;
  const port = createPort(value => {
    if (value.type === 'room-music-audible') audible = value.audible;
    else state = structuredClone(value);
  }, relay, desktopServer.url.href, undefined, peerConnection, audio, fixtureMusicFetch);
  const wait = async (predicate: () => boolean, what: string, timeout = 6000) => {
    const until = performance.now() + timeout;
    while (!predicate()) {
      if (performance.now() > until) throw Error(what + ': ' + String(state.status) + ' ' + JSON.stringify(state.room?.seats));
      await Bun.sleep(10);
    }
  };
  let guest: WebSocket | undefined;
  let musicOffer: Envelope | undefined;
  const sendGuest = (message: Envelope) => guest?.send(JSON.stringify(message));
  const guestNetwork = createRoomNetwork({
    peerConnection, changed() {},
    send: message => {
      if (guest?.readyState !== WebSocket.OPEN) return false;
      sendGuest(message);
      return true;
    },
  });
  try {
    await port.action({type: 'directory'});
    await wait(() => state.status === '已连接', 'directory connection');
    await port.action({type: 'diagnostic'});
    await wait(() => /^信令往返 \d+ ms$/.test(state.status), 'diagnostic relay round trip');
    await port.action({type: 'desktop', command: 'games'});
    await port.action({type: 'desktop', command: 'swarm_ping', args: {addresses: ['127.0.0.1']}});
    if (state.swarmPings?.[0] !== 12) throw Error('missing Swarm result');
    await port.action({type: 'create'});
    await wait(() => state.room?.seats[0]?.clientId === state.clientId, 'host seat');
    const code = state.room.code;
    await port.action({type: 'directory'});
    await wait(() => state.rooms.some((entry: Envelope) => entry.code === code), 'public room listing');
    const roomUrl = buildMultiplayerLobbyRelayUrl(relay, {
      product, roomCode: code, clientId: 'imgui_probe_guest', memberId: 'imgui_probe_guest_member', intent: 'join',
    });
    guest = new WebSocket(roomUrl.url);
    guest.addEventListener('message', event => {
      const data = JSON.parse(String(event.data));
      if (data.type === 'error') console.error('Guest:', data.error);
      if (data.type === 'listen-together') musicOffer = data;
      if (data.roomProbe) void guestNetwork.receive({type:'room-probe-config', iceServers:data.roomProbe.iceServers});
      if (data.room?.seats?.[1]?.clientId === 'imgui_probe_guest') {
        guestNetwork.update({active:data.room.phase === 'lobby', localId:'imgui_probe_guest', peers:[state.clientId]});
      }
      if (data.type === 'room-probe') void guestNetwork.receive(data);
    });
    guest.addEventListener('open', () => {
      sendGuest({type: 'take-seat', seat: 1, loadout: 1, name: '网页客人'});
    });
    await wait(() => state.room?.seats[1]?.name === '网页客人', 'guest seat');
    await wait(() => state.probes?.direct?.ready && guestNetwork.lossReady(state.clientId, 'direct'), 'two direct probe channels');
    await port.action({type:'test-loss'});
    await wait(() => state.probes?.direct?.loss.state === 'complete', 'direct packet loss', 12000);
    if (state.probes.direct.loss.incoming.received !== 300 || state.probes.direct.loss.outgoing.received !== 300)
      throw Error('loopback packet loss mismatch');
    if (verifyMusic) {
      await wait(() => state.music?.enabled, 'shared room music available');
      await port.action({type:'music-search', query:'测试'});
      await wait(() => state.musicResults?.[0]?.name === '测试歌曲', 'room music search');
      await port.action({type:'music-track', index:0});
      await wait(() => state.music?.status === 'playing' && audible, 'native playback and game BGM mute');
      await port.action({type:'music-sync'});
      await wait(() => state.music?.joined && musicOffer?.session && !musicOffer.joined, 'room music invitation');
      sendGuest({type:'listen-together', action:'join', session:musicOffer!.session});
      await wait(() => musicOffer?.joined === true, 'guest joined room music');
      const revision = musicOffer!.revision;
      await port.action({type:'music-play'});
      await wait(() => state.music?.status === 'paused' && !audible && Number(musicOffer?.revision) > revision &&
        musicOffer?.pausedAt != null, 'shared pause and game BGM restore');
      await port.action({type:'music-leave'});
      await wait(() => !state.music?.joined && played.includes('play') && played.includes('pause'), 'room music leave');
      console.log('PASS room music search, player events, invite, browser protocol join, pause and BGM signals');
    }
    await port.action({type: 'quick-chat', phrase: 'thanks', serial: 0});
    await wait(() => state.chat.some((entry: Envelope) => entry.phrase === 'thanks'), 'quick chat echo');
    await port.action({type: 'timing-settings', rollbackEnabled: false, timingChoice: timingPolicy.manualDelayLimit});
    await wait(() => state.room?.adonisMode === 1 && state.room?.timingChoice === timingPolicy.manualDelayLimit,
      'measured timing settings accepted');
    await port.action({type: 'set-ready', ready: true});
    sendGuest({type: 'set-ready', ready: true, runtimeGeneration: generation, movementMode: 'joystick', touchEnabled: false});
    await wait(() => state.room?.seats.slice(0, 2).every((seat: Envelope) => seat?.ready), 'both ready');
    await port.action({type: 'start', inputDelay: 0, adonisMode: 0});
    await wait(() => state.scene === 'game' && handoff?.room?.netplayPlayer === 0, 'desktop handoff');
    if (handoff!.room.netplayAdonisMode !== 1 || handoff!.room.netplayInputDelay !== timingPolicy.manualDelayLimit)
      throw Error('start must use the last accepted room timing');
    const url = new URL(handoff!.room.netplayUrl);
    if (url.searchParams.get('runtimeGeneration') !== generation || url.searchParams.get('room') !== 'th09mp-' + code)
      throw Error('incorrect runtime URL');
    // Exercise the launcher -> native lobby membership transfer against the same Relay room.
    let nativeState: Envelope = {};
    const native = createPort(value => { nativeState = structuredClone(value); }, relay, desktopServer.url.href);
    try {
      const activeUrl = handoff!.room.netplayUrl;
      const launcherLobbyId = state.clientId;
      await native.action({type: 'adopt-room', options: {netplayUrl: activeUrl, lobbyClientId: launcherLobbyId}});
      await wait(() => nativeState.room?.seats[0]?.clientId === launcherLobbyId && nativeState.scene === 'game',
        'native room adoption');
      await Bun.sleep(525);
      await native.action({type: 'quick-chat', phrase: 'thanks', serial: nativeState.room.startSerial});
      await wait(() => nativeState.chat.some((entry: Envelope) => entry.phrase === 'thanks'), 'adopted room quick chat');
      await native.action({type: 'runtime-return'});
      if (nativeState.scene !== 'room') throw Error('runtime return did not restore the room');
      await native.action({type: 'leave'});
      if (nativeState.scene !== 'lobby') throw Error('did not return to lobby');
    } finally { native.close(); }
    console.log('PASS Relay directory, room, diagnostics, direct packet loss, chat, startup, desktop commands, native handoff and return');
  } finally {
    port.close();
    guest?.close();
    guestNetwork.reset();
    desktopServer.stop(true);
  }
}

async function main() {
  const args = process.argv.slice(2);
  if (args[0] === '--fixtures') {
    await fixtures(args[1]);
  } else if (args[0] === '--verify-relay') {
    await verifyRelay(args[1]);
  } else if (args[0] === '--self-test') {
    let state: Envelope = {};
    const port = createPort(value => { state = structuredClone(value); });
    await port.action({type: 'join', code: '28160743'});
    await port.action({type: 'relay-preference', swarm: true});
    if (state.room.swarmSelected !== true) throw Error('Swarm preference was not applied');
    await port.action({type: 'start', inputDelay: 0, adonisMode: 0});
    if (state.scene !== 'game') throw Error('Room did not start');
    await port.action({type: 'quick-chat', phrase: 'thanks', serial: 1});
    await port.action({type: 'free-chat', text: '中文输入', serial: 1});
    if (state.chat.at(-1).text !== '中文输入') throw Error('Chinese chat did not arrive');
    await port.action({type: 'return-room'});
    if (state.scene !== 'room') throw Error('Return to room failed');
    await port.action({type: 'leave'});
    if (state.scene !== 'lobby') throw Error('Return to lobby failed');
    console.log('PASS directory, room, Swarm preference, start, quick/free chat, returns');
  } else if (args[0] === '--stdio') {
    const direct = process.env.EAGLER_DESKTOP_RELAY;
    const site = args[1] === '--site' ? args[2] : undefined;
    let relay = direct;
    if (!relay && site) {
      const url = new URL('/site-manifest.json', site);
      const response = await fetch(url, {signal: AbortSignal.timeout(10000)});
      if (!response.ok) throw Error('无法读取站点联机配置');
      const manifest = await response.json() as Envelope;
      relay = manifest.games?.[game]?.netplayRelay ?? manifest.shared?.netplayRelay;
    }
    if (!relay) throw Error('当前站点未提供联机地址');
    const write = (state: Envelope) => process.stdout.write(JSON.stringify(state) + '\n');
    const audio = new NativeRoomAudio(command => write({type:'room-audio', ...command}));
    const port = createPort(write, relay, process.env.EAGLER_DESKTOP_OVERLAY, site, await loadPeerConnection(), audio);
    port.publish();
    await port.action({type: 'directory'});
    const reader = Bun.stdin.stream().pipeThrough(new TextDecoderStream());
    let pending = '';
    try {
      readCommands: for await (const chunk of reader) {
        pending += chunk;
        if (pending.length > 65536) throw Error('联机操作消息过长');
        const lines = pending.split('\n');
        pending = lines.pop() ?? '';
        for (const line of lines.filter(Boolean)) {
          try {
            const action = JSON.parse(line);
            if (action.type === 'shutdown') break readCommands;
            await port.action(action);
          }
          catch (reason) { port.error(reason); }
        }
      }
    } finally { port.close(); }
  } else {
    const executable = args[0];
    if (!executable) throw Error('Usage: bun bridge.mts <th09-imgui-overlay.exe> [--relay=wss://...]');
    const arg = (name: string) => args.find(value => value.startsWith(name + '='))?.slice(name.length + 1);
    const child = Bun.spawn([executable, '--interactive'], {stdin: 'pipe', stdout: 'pipe', stderr: 'inherit'});
    const relay = arg('--relay');
    const port = createPort(state => {
      child.stdin.write(JSON.stringify(state) + '\n');
      child.stdin.flush();
    }, relay, process.env.EAGLER_DESKTOP_OVERLAY, undefined, relay ? await loadPeerConnection() : undefined);
    port.publish();
    await port.action({type: 'directory'});
    const stream = child.stdout.pipeThrough(new TextDecoderStream());
    let pending = '';
    try {
      for await (const chunk of stream) {
        pending += chunk;
        const lines = pending.split('\n');
        pending = lines.pop() ?? '';
        for (const line of lines.filter(Boolean)) {
          try { await port.action(JSON.parse(line)); }
          catch (reason) { port.error(reason); }
        }
      }
    } finally {
      port.close();
    }
    process.exitCode = await child.exited;
  }
}

await main();

async function loadPeerConnection() {
  const {RTCPeerConnection: NodePeerConnection} = await import('node-datachannel/polyfill');
  class RoomPeerConnection extends NodePeerConnection {
    constructor(config?: RTCConfiguration) {
      super({
        ...config,
        // node-datachannel inserts credentials into TURN URLs before handing
        // them to libdatachannel. Escape REST usernames and passwords first.
        iceServers: config?.iceServers?.map(server => ({
          ...server,
          username: server.username && encodeURIComponent(server.username),
          credential: server.credential && encodeURIComponent(server.credential),
        })),
      });
    }
  }
  return RoomPeerConnection as unknown as typeof RTCPeerConnection;
}
