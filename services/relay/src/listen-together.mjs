import {randomUUID} from 'node:crypto';
import {applyMusicControl, createMusicPlayback, MUSIC_POSITION_LIMIT, MUSIC_QUEUE_LIMIT, musicPlaybackSnapshot,
  sameMusicTrack, uniqueMusicQueue} from '../../../common/contracts/music-playback.mjs';

const TYPE = 'listen-together';
const ID = /^[A-Za-z0-9_-]{1,64}$/;
const validDuration = ms => Number.isSafeInteger(ms) && ms >= 1000 && ms <= MUSIC_POSITION_LIMIT;
const currentTrack = state => state.queue[state.order[state.cursor]];

function readTrack(track) {
  if (!track || (track.platform !== 'netease' && track.platform !== 'qq') ||
      typeof track.id !== 'string' || !ID.test(track.id)) return null;
  return {platform: track.platform, id: track.id};
}
function queueTrack(value) {
  const ref=readTrack(value);
  if(!ref)return null;
  return {...ref,name:typeof value.name==='string'?value.name.slice(0,64):ref.id,artist:typeof value.artist==='string'?value.artist.slice(0,64):''};
}
function readQueue(values, fallback) {
  if(values===undefined)return [queueTrack(fallback)];
  if(!Array.isArray(values)||!values.length||values.length>MUSIC_QUEUE_LIMIT)return null;
  const tracks = values.map(queueTrack);
  if (tracks.some(track => !track)) return null;
  const queue = uniqueMusicQueue(tracks);
  return queue.some(track => sameMusicTrack(track, fallback)) ? queue : null;
}

export function createListenTogether({send, onPublicTrackChange, now = Date.now, random = Math.random}) {
  const sessions = new WeakMap();
  const budgets = new WeakMap();
  const publicTracks = new WeakMap();

  const playerIds = room => room.lobby.seats.flatMap(seat => seat ? [seat.clientId] : []);

  function snapshot(room, state, joined) {
    if (!state) return JSON.stringify({type: TYPE, session: null, serverTime: now()});
    const playback = musicPlaybackSnapshot(state);
    return JSON.stringify({
      type: TYPE,
      session: state.id,
      ...playback,
      ...(joined ? {} : {queue: [playback.queue[playback.queueIndex]], queueIndex: 0}),
      joined,
      participants: playerIds(room).filter(id => state.participants.has(id)),
      serverTime: now(),
    });
  }
  function publish(room, state) {
    scheduleTrackEnd(room, state);
    const previous = publicTracks.get(room);
    const current = state.ended ? null : currentTrack(state);
    if (previous?.name !== current?.name || previous?.artist !== current?.artist) {
      if (current) publicTracks.set(room, {name: current.name, artist: current.artist});
      else publicTracks.delete(room);
      onPublicTrackChange?.();
    }
    let playing, offer;
    for (const [id, socket] of room.lobbyClients)
      send(socket, state.participants.has(id)
        ? (playing ??= snapshot(room, state, true))
        : (offer ??= snapshot(room, state, false)));
  }

  function scheduleTrackEnd(room, state) {
    clearTimeout(state.endTimer);
    const durationMs = currentTrack(state).durationMs;
    if (!durationMs || state.ended || state.pausedAt !== null) return;
    const endsAt = state.startedAt + durationMs;
    state.endTimer = setTimeout(() => {
      if (sessions.get(room) !== state) return;
      if (now() < endsAt) scheduleTrackEnd(room, state);
      else if (applyMusicControl(state, 'ended', {durationMs}, endsAt, random)) publish(room, state);
    }, Math.max(0, endsAt - now()));
    state.endTimer.unref?.();
  }

  function sync(room, clientId) {
    const state = sessions.get(room);
    send(room.lobbyClients.get(clientId), snapshot(room, state, state?.participants.has(clientId)));
  }

  function disconnect(room, clientId) {
    sessions.get(room)?.participants.delete(clientId);
  }
  function close(room) {
    clearTimeout(sessions.get(room)?.endTimer);
    sessions.delete(room);
    publicTracks.delete(room);
  }

  function handle(room, clientId, socket, message) {
    if (room.lobbyClients.get(clientId) !== socket) return;
    const seated = playerIds(room).includes(clientId);
    const time = now();
    const state = sessions.get(room);
    // Leaving remains available after a burst of controls.
    if (message.action === 'leave' && state && message.session === state.id) {
      disconnect(room, clientId);
      sync(room, clientId);
      return;
    }
    let budget = budgets.get(socket);
    if (!budget || time >= budget.resetAt) {
      budget = {resetAt: time + 1000, count: 0};
      budgets.set(socket, budget);
    }
    if (++budget.count > 10) {
      if (budget.count === 11) send(socket, JSON.stringify({type: TYPE, error: 'rate-limited'}));
      return;
    }

    if (message.action === 'sync') {
      sync(room, clientId);
      return;
    }
    if (message.action === 'invite') {
      if (!seated || state) { sync(room, clientId); return; }
      const track = readTrack(message.track);
      if (!track) return;
      const queue=readQueue(message.queue,message.track);
      if(!queue)return;
      const {positionMs = 0, paused = false, repeat = 'off', shuffle = false} = message;
      if (!Number.isFinite(positionMs) || positionMs < 0 || positionMs > MUSIC_POSITION_LIMIT ||
          typeof paused !== 'boolean' || !['off', 'all', 'one'].includes(repeat) || typeof shuffle !== 'boolean') return;
      const active = {
        ...createMusicPlayback(track, queue, time, {positionMs, paused, repeat, shuffle}, random),
        id: randomUUID(), participants: new Set([clientId]),
      };
      if (validDuration(message.durationMs)) currentTrack(active).durationMs = message.durationMs;
      sessions.set(room, active);
      publish(room, active);
      return;
    }
    if (!state || message.session !== state.id) {
      sync(room, clientId);
      return;
    }
    if (message.action === 'join') {
      state.participants.add(clientId);
      sync(room, clientId);
      return;
    }
    if (!seated || !state.participants.has(clientId)) return;
    const {action} = message;
    if (action === 'duration') {
      const track = currentTrack(state);
      if (!track.durationMs && message.playback === state.playback && validDuration(message.durationMs)) {
        track.durationMs = message.durationMs;
        scheduleTrackEnd(room, state);
      }
      return;
    }
    if (action === 'ended') {
      if (message.playback !== state.playback || state.pausedAt !== null || state.ended) return;
      if (currentTrack(state).durationMs) return;
    } else if (message.revision !== state.revision) {
      sync(room, clientId);
      return;
    }

    let fields = message;
    if (action === 'select' || action === 'enqueue') {
      const track = queueTrack(message.track);
      if (!track) return;
      const queue = action === 'select' && message.queue !== undefined ? readQueue(message.queue, track) : undefined;
      if (queue === null) return;
      fields = {track, queue};
    }
    const changed = applyMusicControl(state, action, fields, time, random);
    if (changed === 'queue-full') send(socket, JSON.stringify({type: TYPE, error: changed}));
    if (changed !== true) return;
    publish(room,state);
  }

  return {handle, disconnect, close, sync, publicTrack: room => publicTracks.get(room)};
}
