import {randomUUID} from 'node:crypto';
import {applyMusicControl, createMusicPlayback, MUSIC_POSITION_LIMIT, MUSIC_QUEUE_LIMIT, musicPlaybackSnapshot,
  sameMusicTrack, uniqueMusicQueue} from '../../../common/contracts/music-playback.mjs';

const TYPE = 'listen-together';
const ID = /^[A-Za-z0-9_-]{1,64}$/;

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

export function createListenTogether({send, now = Date.now, random = Math.random}) {
  const sessions = new WeakMap();
  const budgets = new WeakMap();

  function seats(room) {
    return new Set(room.lobby.seats.flatMap(seat => seat ? [seat.clientId] : []));
  }

  function snapshot(state) {
    return JSON.stringify(state ? {
      type: TYPE,
      session: state.id,
      ...musicPlaybackSnapshot(state),
      participants: [...state.participants],
      startedAt: state.pending ? null : state.startedAt,
      serverTime: now(),
    } : {type: TYPE, session: null, serverTime: now()});
  }
  function publish(room, audience, state) {
    const message = snapshot(state);
    for (const id of audience) send(room.lobbyClients.get(id), message);
  }

  function sync(room, clientId) {
    const state = sessions.get(room);
    if (room.lobby.seats.some(seat => seat?.clientId === clientId))
      send(room.lobbyClients.get(clientId), snapshot(state));
  }

  function end(room, state) {
    sessions.delete(room);
    publish(room, state.audience, null);
  }

  function reconcile(room) {
    const state = sessions.get(room);
    if (!state) return;
    const audience = seats(room);
    if (audience.size === state.audience.size &&
        [...audience].every(id => state.audience.has(id))) return;
    const removed = [...state.audience].filter(id => !audience.has(id));
    for (const id of removed) state.participants.delete(id);
    if (state.participants.size < (state.pending ? 1 : 2) || audience.size < 2) {
      end(room, state);
      return;
    }
    state.audience = audience;
    state.revision++;
    if (removed.length) publish(room, removed, null);
    publish(room, audience, state);
  }

  function handle(room, clientId, socket, message) {
    if (room.lobbyClients.get(clientId) !== socket) return;
    if (!room.lobby.seats.some(seat => seat?.clientId === clientId)) return;
    const time = now();
    const state = sessions.get(room);
    // Leaving remains available after a burst of controls.
    if (message.action === 'leave' && state && message.session === state.id) {
      if (!state.participants.delete(clientId)) return;
      if (state.participants.size < 2) end(room, state);
      else {
        state.revision++;
        publish(room, state.audience, state);
      }
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
      send(socket, snapshot(state));
      return;
    }
    if (message.action === 'invite') {
      if (state) { send(socket, snapshot(state)); return; }
      const track = readTrack(message.track);
      const audience = seats(room);
      if (!track || ![...audience].some(id => id !== clientId &&
          room.lobbyClients.get(id)?.readyState === 1)) return;
      const queue=readQueue(message.queue,message.track);
      if(!queue)return;
      const {positionMs = 0, paused = false, repeat = 'off', shuffle = false} = message;
      if (!Number.isFinite(positionMs) || positionMs < 0 || positionMs > MUSIC_POSITION_LIMIT ||
          typeof paused !== 'boolean' || !['off', 'all', 'one'].includes(repeat) || typeof shuffle !== 'boolean') return;
      const pending = {
        ...createMusicPlayback(track, queue, time, {positionMs, paused, repeat, shuffle}, random),
        id: randomUUID(), audience, participants: new Set([clientId]), pending: true, offeredAt: time,
      };
      sessions.set(room, pending);
      publish(room, audience, pending);
      return;
    }
    if (!state || message.session !== state.id) {
      send(socket, snapshot(state));
      return;
    }
    if (message.action === 'join') {
      if (state.participants.has(clientId)) return;
      if (state.pending &&
          ![...state.participants].some(id => room.lobbyClients.get(id)?.readyState === 1)) return;
      state.participants.add(clientId);
      if (state.pending) {
        const wait = time - state.offeredAt;
        state.startedAt += wait;
        if (state.pausedAt !== null) state.pausedAt = time;
        state.pending = false;
      }
      state.revision++;
      publish(room, state.audience, state);
      return;
    }
    if (state.pending || !state.participants.has(clientId)) return;
    const {action} = message;
    if (action === 'ended') {
      if (message.playback !== state.playback || state.pausedAt !== null || state.ended) return;
    } else if (message.revision !== state.revision) {
      send(socket, snapshot(state));
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
    publish(room,state.audience,state);
  }

  return {handle, reconcile, sync};
}
