import {createRoomMusic} from '../services/room-music';
import {createMetingProvider} from '../services/music-provider';
import { createRoomNetwork } from '../../src/launcher/room-network.mts';
import { createMultiplayerQuickChatModel } from '../../src/launcher/multiplayer-quick-chat-model.mts';
import { playQuickChatVoice } from '../../src/launcher/quick-chat-voice.mts';
import { createMultiplayerPreferenceStore } from '../../src/launcher/multiplayer-preferences.mts';
import type { MultiplayerIdentityStore } from '../../src/launcher/multiplayer-identity.mts';
import type { MultiplayerRoomSessionStore } from '../../src/launcher/multiplayer-room-session.mts';
import { createMultiplayerRoom, type MultiplayerRoomPorts } from '../services/multiplayer-room';
import type { GameSettingsModel } from '../models/game-settings';
import type { SitePreferencesModel } from '../models/site-preferences';
import type { RuntimeService } from '../services/runtime';
import {accountSession,roomProfileChangeKey} from '../services/account-session';
import {accountsEnabled, listenTogetherEnabled} from '../release-features';
import {createRoomActionAudio} from '../services/room-action-audio';

export interface RoomSessionOptions extends Omit<
  MultiplayerRoomPorts,
  'identity' | 'sessions' | 'preferences' | 'network' | 'quickChat' | 'music' | 'onActionSound' | 'online' | 'visible'
> {
  identity: MultiplayerIdentityStore;
  sessions: MultiplayerRoomSessionStore;
  storage: Storage | null;
  document: Document;
  window: Window;
  settingsModel: GameSettingsModel;
  sitePreferences: SitePreferencesModel;
  runtime(): RuntimeService | null;
  replayViewer(): boolean;
  titleOverlayOpen(): boolean;
  onMusicAudibleChange?(audible: boolean): void;
}
/** Document-lived room transport, original probe channels and shared quick-chat state.
 * Gameplay remains the existing Runtime; probe metrics never certify its path. */
export function createRoomSession(options: RoomSessionOptions) {
  const { window, document } = options,
    listeners = new Set<() => void>();
  let revision = 0,
    disposed = false;
  const roomAudio = createRoomActionAudio(window as Window & typeof globalThis);
  let accountIdentity:string|null|undefined;
  let accountInvalidated = false;
  const quickChat = createMultiplayerQuickChatModel({
    send: (message) => {
      service.sendControl(message);
    },
    voice: playQuickChatVoice,
    notify: code => options.notify(options.translate(({
      'reported':'chat.reported','report-rate-limited':'chat.reportRateLimited','report-in-progress':'chat.reportInProgress',
      'chat-banned':'chat.banned','send-rate-limited':'chat.sendRateLimited','sign-in-required':'chat.signInRequired',
      'review-unavailable':'chat.reviewUnavailable',
    } as const)[code as 'reported'] ?? 'chat.failed')),
    timers: window,
  });
  const changed = () => {
    if (!disposed) {
      revision++;
      for (const listener of listeners) listener();
    }
  };
  let musicEndpoint: URL | null = null;
  if (listenTogetherEnabled) {
    try {
      const endpoint = new URL(import.meta.env?.VITE_METING_API_URL || 'https://open.motues.top/music', window.location.href);
      if (endpoint.protocol === 'https:' && !endpoint.username && !endpoint.password) musicEndpoint = endpoint;
    } catch {
      // An invalid provider setting must not prevent joining rooms.
    }
  }
  const music = musicEndpoint ? createRoomMusic({
    provider: createMetingProvider(musicEndpoint.href),
    storage: options.storage, send: message => service.sendControl(message),
    createAudio: () => document.createElement('audio'),
    onAudibleChange: options.onMusicAudibleChange,
  }) : null;
  const probes = createRoomNetwork({ send: (message) => service.sendControl(message), changed });
  const service = createMultiplayerRoom({
    ...options,
    preferences: createMultiplayerPreferenceStore({ storage: options.storage }),
    network: probes,
    quickChat: { receive: (message) => {
      if (accountInvalidated && message.type === 'chat-status' && message.registered === true) return;
      quickChat.receive(message);
    } },
    music: {receive: message => music?.receive(message)},
    onActionSound(sound) {
      const settings = options.settingsModel.getSnapshot()?.options;
      const volume = settings && 'sfxVolume' in settings && typeof settings.sfxVolume === 'number'
        ? settings.sfxVolume / 100 : 1;
      roomAudio.play(sound, volume);
    },
    online: () => window.navigator.onLine,
    visible: () => document.visibilityState === 'visible',
  });
  function sync() {
    if (disposed) return;
    const state = service.getSnapshot(),
      room = state.room,
      runtime = options.runtime()?.getSnapshot();
    if (runtime?.launched === false) service.resumeSpectator();
    if (state.product === 'th09mp' && state.spectatorRequested && room?.synced)
      options.runtime()?.spectatorRoomState(state.startSerial, room.phase === 'lobby');
    probes.update({
      localId: state.localClientId,
      peers: (room?.seats ?? [])
        .slice(0, room?.playerCount ?? 0)
        .flatMap((seat) => (seat && !seat.offline ? [seat.clientId] : [])),
      active:
        room?.synced === true &&
        state.connected &&
        state.seat != null &&
        room.phase === 'lobby' &&
        (!runtime?.launched || options.titleOverlayOpen()),
    });
    music?.update(state);
    quickChat.update({
      visible: !!room && !options.replayViewer(),
      inRoom: !!room && (runtime?.launched !== true || options.titleOverlayOpen()),
      room: room ? `${state.product}-${room.code}` : '',
      serial: state.startSerial,
      localSeat: state.seat,
      seats: room?.seats ?? [],
      spectators: room?.spectators ?? [],
      localSpectator: state.spectatorRequested,
      localClientId: state.localClientId,
      connected: state.connected,
      language: options.settingsModel.getSnapshot()?.language ?? '',
      lessMotion: options.sitePreferences.getSnapshot().lessMotion,
    });
    changed();
  }
  let previousControls = options.settingsModel.getSnapshot()?.options;
  const unsubscribeRoom = service.subscribe(sync),
    unsubscribeSettings = options.settingsModel.subscribe(() => {
      const current = options.settingsModel.getSnapshot()?.options;
      if (
        previousControls &&
        current &&
        (previousControls.touchEnabled !== current.touchEnabled ||
          previousControls.touchMovementMode !== current.touchMovementMode)
      )
        service.movementChanged();
      previousControls = current;
      sync();
    });
  const unsubscribePreferences = options.sitePreferences.subscribe(sync);
  const accountChanged = () => {
    const account = accountSession.getSnapshot();
    if (account.status === 'loading' || account.status === 'error') {
      if (accountIdentity !== undefined && !accountInvalidated) {
        accountInvalidated = true;
        quickChat.setDraft('');
        quickChat.dismiss();
        quickChat.receive({type:'chat-status',registered:false});
      }
      return;
    }
    const identity = account.status === 'member' ? account.user.username : null;
    const refresh = accountIdentity !== undefined && (accountInvalidated || identity !== accountIdentity);
    accountIdentity = identity;
    accountInvalidated = false;
    if (refresh) {
      quickChat.setDraft('');
      quickChat.dismiss();
      quickChat.receive({type:'chat-status',registered:false});
      service.refreshAccount();
    }
  };
  const unsubscribeAccount = accountsEnabled ? accountSession.subscribe(accountChanged) : () => {};
  if (accountsEnabled) accountChanged();
  const refreshRoomProfile = () => {
    if (accountSession.getSnapshot().status === 'member') service.refreshAccount();
  };
  const profileStorageChanged = (event: StorageEvent) => {
    if (event.key === roomProfileChangeKey) refreshRoomProfile();
  };
  if (accountsEnabled) {
    window.addEventListener(roomProfileChangeKey, refreshRoomProfile);
    window.addEventListener('storage', profileStorageChanged);
  }
  const pageHide = () => service.pageHide(),
    pageShow = (event: PageTransitionEvent) => service.pageShow(event.persisted);
  const visible = () => {
    if (document.visibilityState === 'visible') service.reconnect();
  };
  const unlockRoomAudio = (event: Event) => {
    if (event.isTrusted && (service.getSnapshot().room || document.body.classList.contains('lobby-page')))
      roomAudio.unlock();
  };
  const activity = (event: Event) => {
    service.noteActivity(event.isTrusted);
    unlockRoomAudio(event);
  };
  window.addEventListener('pagehide', pageHide);
  window.addEventListener('pageshow', pageShow);
  window.addEventListener('online', service.reconnect);
  document.addEventListener('visibilitychange', visible);
  document.addEventListener('pointerdown', activity, { passive: true });
  document.addEventListener('click', unlockRoomAudio);
  document.addEventListener('keydown', activity);
  return {
    service,
    network: {
      ...probes,
      subscribe(listener: () => void) {
        listeners.add(listener);
        return () => {
          listeners.delete(listener);
        };
      },
      getSnapshot: () => revision,
      retry(peer?: string) {
        probes.retry(peer);
        sync();
      },
    },
    sync,
    quickChat,
    music,
    dispose() {
      if (disposed) return;
      music?.dispose();
      roomAudio.dispose();
      quickChat.dispose();
      disposed = true;
      unsubscribeRoom();
      unsubscribeSettings();
      unsubscribePreferences();
      unsubscribeAccount();
      if (accountsEnabled) {
        window.removeEventListener(roomProfileChangeKey, refreshRoomProfile);
        window.removeEventListener('storage', profileStorageChanged);
      }
      service.dispose();
      probes.reset();
      listeners.clear();
      window.removeEventListener('pagehide', pageHide);
      window.removeEventListener('pageshow', pageShow);
      window.removeEventListener('online', service.reconnect);
      document.removeEventListener('visibilitychange', visible);
      document.removeEventListener('pointerdown', activity);
      document.removeEventListener('click', unlockRoomAudio);
      document.removeEventListener('keydown', activity);
    },
  };
}
export type RoomSession = ReturnType<typeof createRoomSession>;
