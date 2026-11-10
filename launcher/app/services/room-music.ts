import { applyMusicControl, createMusicPlayback, MUSIC_POSITION_LIMIT, MUSIC_QUEUE_LIMIT, musicPlaybackSnapshot,
  type MusicPlayback, type MusicRepeat, sameMusicTrack, uniqueMusicQueue } from '../../../common/contracts/music-playback.mjs';
import type { RoomOwnerSnapshot } from './multiplayer-room';
import { type MusicPlatform, type MusicProvider, type MusicRef, type MusicTrack, validMusicRef } from './music-provider';

export interface MusicLookup {
  platform: MusicPlatform;
  mode: 'search' | 'playlist';
  query: string;
}

const MUSIC_LOOKUP_KEY = 'eagler-touhou-music-lookups-v1';
const MUSIC_LOOKUP_LIMIT = 40;
function validLookup(value: unknown): value is MusicLookup {
  if (!value || typeof value !== 'object') return false;
  const {platform, mode, query} = value as Partial<MusicLookup>;
  return (platform === 'netease' || platform === 'qq') && typeof query === 'string' &&
    (mode === 'search' ? !!query.trim() && query.length <= 500 : mode === 'playlist' && /^\d{1,24}$/.test(query));
}

interface Session {
  session: string;
  revision: number;
  track: MusicRef;
  participants: string[];
  joined?: boolean;
  startedAt: number | null;
  pausedAt?: number | null;
  playback?: number;
  ended?: boolean;
  queue?: MusicTrack[];
  queueIndex?: number;
  repeat?: 'off' | 'all' | 'one';
  shuffle?: boolean;
  hasPrevious?: boolean;
  hasNext?: boolean;
  serverTime: number;
}
export type MusicError =
  | 'unavailable'
  | 'trial'
  | 'blocked'
  | 'conflict'
  | 'rateLimited'
  | 'invalidPlaylist'
  | 'queueFull';
export interface RoomMusicSnapshot {
  enabled: boolean;
  seated: boolean;
  inRoom: boolean;
  connected: boolean;
  session: Session | null;
  joined: boolean;
  syncState: 'solo' | 'offered' | 'shared';
  selected: MusicTrack | null;
  lookups: MusicLookup[];
  current: MusicTrack | null;
  status: 'idle' | 'loading' | 'playing' | 'paused' | 'ended';
  error: MusicError | null;
  position: number;
  seekTarget: number | null;
  duration: number;
  volume: number;
}
export const LAST_MUSIC_KEY = 'eagler-touhou-last-music-v1';
export function createRoomMusic({
  provider,
  storage,
  send,
  createAudio,
  onAudibleChange,
  clock = performance.now.bind(performance),
}: {
  provider: MusicProvider;
  storage: Pick<Storage, 'getItem' | 'setItem'> | null;
  send(message: Record<string, unknown>): boolean;
  createAudio(): HTMLAudioElement;
  onAudibleChange?(audible: boolean): void;
  clock?: () => number;
}) {
  let selected: MusicTrack | null = null;
  let lookups: MusicLookup[] = [];
  try {
    const item = JSON.parse(storage?.getItem(LAST_MUSIC_KEY) || 'null');
    if (validMusicRef(item)) {
      const saved = item as Partial<MusicTrack>;
      selected = {
        ...item,
        name: String(saved.name || item.id).slice(0, 200),
        artist: String(saved.artist || '').slice(0, 300),
      };
    }
  } catch {}
  try {
    const saved = JSON.parse(storage?.getItem(MUSIC_LOOKUP_KEY) || '[]');
    if (Array.isArray(saved)) lookups = saved.filter(validLookup).slice(0, MUSIC_LOOKUP_LIMIT);
  } catch {}
  let snapshot: RoomMusicSnapshot = {
    enabled: false,
    seated: false,
    inRoom: false,
    connected: false,
    session: null,
    joined: false,
    syncState: 'solo',
    selected,
    lookups,
    current: null,
    status: 'idle',
    error: null,
    position: 0,
    seekTarget: null,
    duration: 0,
    volume: 0.5,
  };
  const listeners = new Set<() => void>();
  let roomKey = '',
    localId = '',
    suppressed = '',
    playbackKey = '',
    endedPlayback = -1,
    disposed = false;
  let personal: MusicPlayback<MusicTrack> | null = null;
  let remote: Session | null = null;
  let transportSerial = 0;
  let audio: HTMLAudioElement | null = null,
    controller: AbortController | null = null;
  let producingAudio = false;
  let audible = false;
  let details: AbortController | null = null;
  let ticker: ReturnType<typeof setInterval> | null = null;
  let anchor = { server: 0, local: 0 },
    pendingTrack: MusicRef | null = null;
  let pendingSeek: {key: string; revision: number; sent: number; latest: number} | null = null;
  let seekTimeout: ReturnType<typeof setTimeout> | null = null;
  function patch(value: Partial<RoomMusicSnapshot>) {
    if (disposed) return;
    snapshot = { ...snapshot, ...value };
    for (const listener of listeners) listener();
  }
  function stop() {
    clearSeek();
    controller?.abort();
    controller = null;
    playbackKey = '';
    endedPlayback = -1;
    stopTicker();
    quiet();
    if (audio) {
      audio.pause();
      audio.removeAttribute('src');
      audio.load();
    }
  }
  function stopTicker() {
    if (ticker !== null) clearInterval(ticker);
    ticker = null;
  }
  const trackKey = (state: Session) => `${state.session}:${state.track.platform}:${state.track.id}`;
  const mediaKey = (state: Session) => `${state.track.platform}:${state.track.id}`;
  const personalSnapshot = (state: MusicPlayback<MusicTrack>): Session => ({
    ...musicPlaybackSnapshot(state), session: 'personal', participants: [], serverTime: clock(),
  });
  const wireTrack = (track: MusicTrack) => ({
    platform: track.platform,
    id: track.id,
    name: track.name.slice(0, 64),
    artist: track.artist.slice(0, 64),
  });
  function command(action: string, fields: Record<string, unknown> = {}) {
    return (
      !disposed &&
      snapshot.enabled &&
      snapshot.inRoom &&
      snapshot.connected &&
      send({ type: 'listen-together', action, ...fields })
    );
  }
  function control(action: string, fields: Record<string, unknown> = {}) {
    const state = snapshot.session;
    if (!state || disposed) return false;
    if (snapshot.joined)
      return snapshot.seated && command(action, { session: state.session, revision: state.revision, ...fields });
    if (!personal || !snapshot.inRoom) return false;
    const time = clock();
    if (snapshot.status === 'playing' && personal.pausedAt === null && audio)
      personal.startedAt = time - audio.currentTime * 1000;
    const changed = applyMusicControl(personal, action, fields, time);
    if (changed === 'queue-full') patch({ error: 'queueFull' });
    if (changed !== true) return false;
    applyState(personalSnapshot(personal), false);
    return true;
  }
  function clearSeek() {
    pendingSeek = null;
    if (seekTimeout !== null) clearTimeout(seekTimeout);
    seekTimeout = null;
    if (snapshot.seekTarget !== null) patch({ seekTarget: null });
  }
  function sendSeek(positionMs: number) {
    const state = snapshot.session;
    if (!state || !control('seek', { positionMs })) return false;
    pendingSeek = { key: trackKey(state), revision: state.revision, sent: positionMs, latest: positionMs };
    patch({ seekTarget: positionMs / 1000 });
    seekTimeout = setTimeout(() => { clearSeek(); command('sync'); }, 2500);
    return true;
  }
  function finished() {
    stopTicker();
    quiet();
    audio?.pause();
    patch({ status: 'ended', position: snapshot.duration });
    const state = snapshot.session;
    if (state && !state.ended && state.playback !== undefined && endedPlayback !== state.playback) {
      endedPlayback = state.playback;
      control('ended', { playback: state.playback, durationMs: Math.round(snapshot.duration * 1000) });
    }
  }
  function reportDuration() {
    const {session, joined, seated, duration} = snapshot;
    if (joined && seated && session?.playback !== undefined && duration)
      command('duration', {session: session.session, playback: session.playback, durationMs: Math.round(duration * 1000)});
  }
  function progress() {
    const start = snapshot.session?.startedAt;
    const time = snapshot.session?.pausedAt ?? anchor.server + clock() - anchor.local;
    return start == null ? 0 : Math.max(0, (time - start) / 1000);
  }
  function reportAudible() {
    const next = producingAudio && !!audio && !audio.paused && !audio.ended && !audio.muted && audio.volume > 0;
    if (next === audible) return;
    audible = next;
    onAudibleChange?.(audible);
  }
  function sounding() { producingAudio = true; reportAudible(); }
  function quiet() { producingAudio = false; reportAudible(); }
  // Buffering can resume at any moment. Keep the game BGM muted until playback actually stops.
  const quietEvents = ['pause', 'ended', 'emptied', 'error'] as const;
  function playerAudio() {
    if (audio) return audio;
    audio = createAudio();
    audio.preload = 'auto';
    audio.volume = snapshot.volume;
    audio.addEventListener('playing', sounding);
    audio.addEventListener('volumechange', reportAudible);
    for (const event of quietEvents) audio.addEventListener(event, quiet);
    return audio;
  }
  function prime() {
    const player = playerAudio();
    // Reserve playback permission during the user's gesture.
    if (!player.src) void player.play().catch(() => {});
  }
  function load(player: HTMLAudioElement, url: string, signal: AbortSignal): Promise<void> {
    return new Promise((resolve, reject) => {
      function cleanup() {
        clearTimeout(timeout);
        player.removeEventListener('loadedmetadata', ready);
        player.removeEventListener('error', failed);
        signal.removeEventListener('abort', aborted);
      }
      function ready() {
        cleanup();
        resolve();
      }
      function failed() {
        cleanup();
        reject(Error('unavailable'));
      }
      function aborted() {
        cleanup();
        reject(new DOMException('Cancelled', 'AbortError'));
      }
      const timeout = setTimeout(failed, 15000);
      player.addEventListener('loadedmetadata', ready);
      player.addEventListener('error', failed);
      signal.addEventListener('abort', aborted, { once: true });
      if (signal.aborted) {
        aborted();
        return;
      }
      player.src = url;
      player.load();
    });
  }
  async function applyTransport(task: AbortController, key: string) {
    const player = audio;
    const serial = ++transportSerial;
    const isCurrent = () => controller === task && serial === transportSerial && !task.signal.aborted && !disposed;
    if (!player || !isCurrent()) return;
    stopTicker();
    if (snapshot.session?.ended || progress() >= snapshot.duration) {
      finished();
      return;
    }
    player.currentTime = progress();
    if (snapshot.session?.pausedAt != null) {
      player.pause();
      quiet();
      patch({ status: 'paused', position: player.currentTime });
      return;
    }
    if (player.paused) {
      patch({ status: 'loading' });
      await player.play();
      if (!isCurrent()) return;
      if (snapshot.session?.pausedAt != null) {
        player.pause();
        quiet();
        patch({ status: 'paused', position: progress() });
        return;
      }
      if (snapshot.joined) player.currentTime = progress();
    }
    patch({ status: 'playing', position: player.currentTime });
    let ticks = 0;
    ticker = setInterval(() => {
      if (!isCurrent()) return;
      if (player.error) {
        stop();
        playbackKey = key;
        patch({ status: 'idle', error: 'unavailable' });
        return;
      }
      if (pendingSeek) return;
      if (player.ended || (snapshot.joined && progress() >= snapshot.duration)) {
        finished();
        return;
      }
      if (snapshot.joined && !player.paused && Math.abs(player.currentTime - progress()) > 0.35) player.currentTime = progress();
      patch({ position: player.currentTime });
      if (snapshot.joined && ++ticks % 10 === 0) command('sync');
    }, 1000);
  }
  function playbackFailed(task: AbortController, key: string, error: unknown) {
    if (controller !== task || disposed) return;
    stop();
    playbackKey = key;
    const code = error instanceof Error ? error.message : '';
    patch({
      status: 'idle',
      error:
        code === 'trial'
          ? 'trial'
          : code === 'blocked' || (error instanceof Error && error.name === 'NotAllowedError')
            ? 'blocked'
            : 'unavailable',
    });
  }
  async function play(state: Session) {
    stop();
    const key = mediaKey(state);
    playbackKey = key;
    const task = new AbortController();
    controller = task;
    const timeout = setTimeout(() => {
      if (controller === task) {
        stop();
        playbackKey = key;
        patch({ status: 'idle', error: 'unavailable' });
      }
    }, 30000);
    task.signal.addEventListener('abort', () => clearTimeout(timeout), { once: true });
    patch({ status: 'loading', error: null, position: 0, duration: 0 });
    const isCurrent = () => !task.signal.aborted && !disposed && controller === task;
    try {
      const urls = await provider.stream(state.track, task.signal);
      if (!isCurrent()) return;
      const player = playerAudio();
      for (const [index, url] of urls.entries()) {
        try {
          await load(player, url, task.signal);
          break;
        } catch (error) {
          if (!isCurrent()) return;
          if (index === urls.length - 1) throw error;
        }
      }
      if (!isCurrent()) return;
      const duration = player.duration;
      if (!Number.isFinite(duration) || duration <= 0 || duration * 1000 > MUSIC_POSITION_LIMIT)
        throw Error('unavailable');
      patch({ duration });
      reportDuration();
      if (!snapshot.joined && personal?.pausedAt === null) {
        // A personal track starts when it is ready. Shared tracks follow the room clock.
        personal.startedAt += clock() - anchor.local;
        const ready = personalSnapshot(personal);
        anchor = { server: ready.serverTime, local: ready.serverTime };
        patch({ session: ready });
      }
      await applyTransport(task, key);
    } catch (error) {
      playbackFailed(task, key, error);
    } finally {
      clearTimeout(timeout);
    }
  }
  function receive(message: Record<string, unknown>) {
    if (!snapshot.enabled || !snapshot.inRoom || !snapshot.connected) return;
    if (message.error === 'rate-limited' || message.error === 'queue-full') {
      clearSeek();
      if (snapshot.joined) patch({ error: message.error === 'queue-full' ? 'queueFull' : 'rateLimited' });
      return;
    }
    if (message.session === null) {
      remote = null;
      pendingTrack = null;
      if (snapshot.joined) restorePersonal();
      else patch({ syncState: 'solo' });
      return;
    }
    if (
      typeof message.session !== 'string' ||
      (message.joined !== undefined && typeof message.joined !== 'boolean') ||
      !Number.isSafeInteger(message.revision) ||
      !validMusicRef(message.track) ||
      !Array.isArray(message.participants) ||
      message.participants.length > 3 ||
      !message.participants.every((id) => typeof id === 'string') ||
      !Number.isFinite(message.serverTime) ||
      !(message.startedAt === null || Number.isFinite(message.startedAt)) ||
      !(message.pausedAt == null || Number.isFinite(message.pausedAt))
    )
      return;
    if (
      message.queue !== undefined &&
      (!Array.isArray(message.queue) ||
        !message.queue.length ||
        message.queue.length > MUSIC_QUEUE_LIMIT ||
        !message.queue.every(validMusicRef) ||
        !Number.isInteger(message.queueIndex) ||
        Number(message.queueIndex) < 0 ||
        Number(message.queueIndex) >= message.queue.length)
    )
      return;
    if (message.playback !== undefined && (!Number.isSafeInteger(message.playback) || Number(message.playback) < 1))
      return;
    if (message.repeat !== undefined && !['off', 'all', 'one'].includes(String(message.repeat))) return;
    if (message.shuffle !== undefined && typeof message.shuffle !== 'boolean') return;
    if (message.ended !== undefined && typeof message.ended !== 'boolean') return;
    const state = message as unknown as Session;
    if (remote?.session === state.session && remote.revision > state.revision) return;
    remote = state;
    const participating = state.session !== suppressed && (state.joined ?? state.participants.includes(localId));
    if (participating && state.startedAt !== null) {
      if (!snapshot.joined && personal) {
        const time = clock();
        const position = audio && snapshot.duration ? audio.currentTime : snapshot.position;
        personal.startedAt = time - position * 1000;
        personal.pausedAt = time;
      }
      const conflict = pendingTrack && !sameMusicTrack(pendingTrack, state.track);
      pendingTrack = null;
      applyState(state, true);
      if (conflict) patch({ error: 'conflict' });
    } else if (snapshot.joined) restorePersonal();
    else patch({ syncState: 'offered' });
  }

  function applyState(state: Session, joined: boolean) {
    const previous = snapshot.session;
    const restarted = previous?.session !== state.session ||
      (state.playback !== undefined
        ? previous.playback !== state.playback
        : previous.startedAt !== state.startedAt && previous.pausedAt == null && state.pausedAt == null);
    const pauseChanged = (previous?.pausedAt != null) !== (state.pausedAt != null);
    if (restarted || pauseChanged) endedPlayback = -1;
    if (joined || restarted || pauseChanged) anchor = { server: state.serverTime, local: clock() };
    patch({ session: state, joined, ...(joined ? { syncState: 'shared' as const } : {}) });
    let deferSeekTransport = false;
    if (pendingSeek) {
      if (!joined || pendingSeek.key !== trackKey(state)) clearSeek();
      else if (state.revision > pendingSeek.revision) {
        const {sent, latest} = pendingSeek;
        pendingSeek = null;
        if (seekTimeout !== null) clearTimeout(seekTimeout);
        seekTimeout = null;
        // Serialize seek commands against relay revisions. Rapid input only
        // replaces the pending target instead of sending conflicting commands.
        if (latest !== sent) deferSeekTransport = sendSeek(latest);
        if (!deferSeekTransport) {
          patch({ position: Math.min(snapshot.duration, progress()) });
          clearSeek();
        }
      }
    }

    if (!sameMusicTrack(snapshot.current, state.track)) {
      details?.abort();
      details = null;
      const label = state.queue?.[state.queueIndex ?? 0];
      patch({ current: label || { ...state.track, name: state.track.id, artist: '' } });
      if (joined || !label?.cover) resolveDetails(state.track);
    }
    if (deferSeekTransport) return;
    const key = mediaKey(state);
    if (playbackKey !== key || (restarted && snapshot.status === 'idle')) void play(state);
    else if (state.ended) finished();
    else if (controller && (restarted || pauseChanged || (state.pausedAt != null && snapshot.status !== 'paused'))) {
      const task = controller;
      // A pending stream reads the latest transport state when it finishes loading.
      if (snapshot.duration > 0 && (snapshot.status !== 'loading' || audio?.readyState)) {
        void applyTransport(task, key).catch((error) => playbackFailed(task, key, error));
      }
    }
  }
  function resolveDetails(track: MusicRef) {
    const request = new AbortController();
    details = request;
    const timeout = setTimeout(() => request.abort(), 15000);
    request.signal.addEventListener('abort', () => clearTimeout(timeout), { once: true });
    void provider
      .track(track, request.signal)
      .then((track) => {
        if (details === request && !request.signal.aborted) patch({ current: track });
      })
      .catch(() => {})
      .finally(() => clearTimeout(timeout));
  }
  function restorePersonal() {
    clearSeek();
    patch({ joined: false, syncState: remote ? 'offered' : 'solo', error: null });
    if (personal) applyState(personalSnapshot(personal), false);
    else {
      stop();
      details?.abort();
      details = null;
      patch({ session: null, current: null, status: 'idle', position: 0, duration: 0 });
    }
  }
  function leave() {
    if (!remote || !snapshot.joined) return;
    suppressed = remote.session;
    command('leave', { session: remote.session });
    restorePersonal();
  }
  function reset() {
    personal = null;
    remote = null;
    suppressed = '';
    pendingTrack = null;
    details?.abort();
    details = null;
    stop();
    patch({ session: null, joined: false, syncState: 'solo', current: null, status: 'idle', error: null, position: 0, duration: 0 });
  }
  return {
    provider,
    subscribe(listener: () => void) {
      listeners.add(listener);
      return () => {
        listeners.delete(listener);
      };
    },
    getSnapshot: () => snapshot,
    getPendingInvitation: () =>
      snapshot.syncState === 'offered' && remote && remote.session !== suppressed &&
        !(remote.joined ?? remote.participants.includes(localId)) ? remote : null,
    declineInvitation() {
      if (!remote || snapshot.joined) return;
      suppressed = remote.session;
      patch({syncState: 'offered'});
    },
    receive,
    rememberLookup(entry: MusicLookup) {
      if (disposed || !validLookup(entry)) return;
      const query = entry.query.trim().toLowerCase();
      const lookups = [entry, ...snapshot.lookups.filter(item =>
        item.platform !== entry.platform || item.mode !== entry.mode || item.query.trim().toLowerCase() !== query,
      )].slice(0, MUSIC_LOOKUP_LIMIT);
      try { storage?.setItem(MUSIC_LOOKUP_KEY, JSON.stringify(lookups)); } catch {}
      patch({ lookups });
    },
    update(room: RoomOwnerSnapshot) {
      const key = room.room ? `${room.product}:${room.room.code}` : '';
      const inRoom = !!room.room;
      const seated = room.seat != null && inRoom;
      const gainedSeat = seated && !snapshot.seated;
      const enabled = room.listenTogetherSupported === true;
      const requestState = enabled && inRoom && room.connected &&
        (key !== roomKey || !snapshot.enabled || !snapshot.connected);
      localId = room.localClientId;
      if (key !== roomKey) reset();
      else if ((!room.connected || !enabled) && remote) {
        remote = null;
        pendingTrack = null;
        if (snapshot.joined) restorePersonal();
        else patch({ syncState: 'solo' });
      }
      roomKey = key;
      if (!seated && snapshot.seated) clearSeek();
      if (
        snapshot.inRoom !== inRoom ||
        snapshot.enabled !== enabled ||
        snapshot.seated !== seated ||
        snapshot.connected !== room.connected
      )
        patch({ inRoom, enabled, seated, connected: room.connected });
      if (requestState) command('sync');
      if (gainedSeat) reportDuration();
    },
    choose(track: MusicTrack, queue?: MusicTrack[]) {
      if (disposed || !snapshot.inRoom || (snapshot.joined && !snapshot.seated) || !validMusicRef(track)) return;
      const list = queue && uniqueMusicQueue(queue.filter(validMusicRef)).slice(0, MUSIC_QUEUE_LIMIT);
      if (list && !list.some(item => sameMusicTrack(item, track))) return;
      prime();
      let selected: boolean;
      if (snapshot.joined) {
        selected = control('select', { track: wireTrack(track), ...(list ? {queue: list.map(wireTrack)} : {}) });
        if (selected) pendingTrack = track;
      } else if (!personal) {
        personal = createMusicPlayback(track, list || [track], clock());
        applyState(personalSnapshot(personal), false);
        selected = true;
      } else selected = control('select', { track, ...(list ? {queue: list} : {}) });
      if (!selected) return;
      try {
        storage?.setItem(LAST_MUSIC_KEY, JSON.stringify(track));
      } catch {}
      patch({ selected: track, error: null });
    },
    requestSync() {
      if (disposed || !snapshot.enabled || !snapshot.inRoom || !snapshot.connected || snapshot.joined) return;
      if (remote) {
        prime();
        suppressed = '';
        command('join', { session: remote.session });
        return;
      }
      if (!snapshot.seated || !personal || !snapshot.duration || snapshot.status === 'loading' || snapshot.error) return;
      const position = Math.min(audio?.currentTime ?? snapshot.position, Math.max(0, snapshot.duration - 0.05));
      command('invite', {
        track: wireTrack(personal.queue[personal.order[personal.cursor]]),
        queue: personal.queue.map(wireTrack),
        positionMs: Math.min(MUSIC_POSITION_LIMIT, Math.round(position * 1000)),
        durationMs: Math.round(snapshot.duration * 1000),
        paused: personal.pausedAt !== null, repeat: personal.repeat, shuffle: personal.shuffle,
      });
    },
    leave,
    retry() {
      if (disposed || !snapshot.session || !snapshot.inRoom) return;
      prime();
      void play(snapshot.session);
    },
    togglePlayback() {
      const state = snapshot.session;
      if (!state || !snapshot.inRoom || snapshot.status === 'loading') return;
      control(state.pausedAt == null && !state.ended && snapshot.status !== 'ended' ? 'pause' : 'resume');
    },
    seek(seconds: number) {
      if (disposed || !snapshot.inRoom || (snapshot.joined && !snapshot.seated) ||
        !snapshot.duration || !Number.isFinite(seconds)) return false;
      const positionMs = Math.round(Math.max(0, Math.min(snapshot.duration - 0.05, seconds)) * 1000);
      if (!snapshot.joined) return control('seek', { positionMs });
      if (pendingSeek) {
        pendingSeek.latest = positionMs;
        patch({ seekTarget: positionMs / 1000 });
        return true;
      }
      return sendSeek(positionMs);
    },
    previous() {
      control('previous');
    },
    next() {
      control('next');
    },
    setRepeat(repeat: MusicRepeat) {
      control('mode', { repeat, shuffle: snapshot.session?.shuffle ?? false });
    },
    setShuffle(shuffle: boolean) {
      control('mode', { shuffle, repeat: snapshot.session?.repeat ?? 'off' });
    },
    enqueue(track: MusicTrack) {
      if (disposed || !snapshot.inRoom || !validMusicRef(track)) return;
      if (!snapshot.joined && !personal) {
        personal = createMusicPlayback(track, [track], clock(), {paused: true});
        applyState(personalSnapshot(personal), false);
      } else control('enqueue', { track: snapshot.joined ? wireTrack(track) : track });
    },
    remove(index: number) {
      control('remove', { index });
    },
    setVolume(volume: number) {
      if (disposed || !Number.isFinite(volume)) return;
      const value = Math.max(0, Math.min(1, volume));
      if (audio) audio.volume = value;
      reportAudible();
      patch({ volume: value });
    },
    dispose() {
      if (remote && snapshot.joined) command('leave', { session: remote.session });
      reset();
      audio?.removeEventListener('playing', sounding);
      audio?.removeEventListener('volumechange', reportAudible);
      for (const event of quietEvents) audio?.removeEventListener(event, quiet);
      audio = null;
      disposed = true;
      listeners.clear();
    },
  };
}
export type RoomMusic = ReturnType<typeof createRoomMusic>;
