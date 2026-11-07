export const MUSIC_QUEUE_LIMIT = 100;
export const MUSIC_POSITION_LIMIT = 86_400_000;
export const sameMusicTrack = (a, b) => a?.platform === b?.platform && a?.id === b?.id;
export const uniqueMusicQueue = tracks => [...new Map(tracks.map(track => [`${track.platform}:${track.id}`, track])).values()];

const trackRef = ({platform, id}) => ({platform, id});
const currentIndex = state => state.order[state.cursor];

function reorder(state, random) {
  const current = currentIndex(state);
  const indices = state.queue.map((_, index) => index);
  if (!state.shuffle) {
    state.order = indices;
    state.cursor = current;
    return;
  }
  const others = indices.filter(index => index !== current);
  for (let index = others.length - 1; index > 0; index--) {
    const other = Math.floor(random() * (index + 1));
    [others[index], others[other]] = [others[other], others[index]];
  }
  state.order = [current, ...others];
  state.cursor = 0;
}

export function createMusicPlayback(track, queue, time, {positionMs = 0, paused = false, repeat = 'off', shuffle = false} = {}, random = Math.random) {
  const state = {
    revision: 1, track: trackRef(track), queue: [...queue], order: queue.map((_, index) => index),
    cursor: queue.findIndex(item => sameMusicTrack(item, track)), repeat, shuffle,
    startedAt: time - positionMs, pausedAt: paused ? time : null, playback: 1, ended: false,
  };
  if (shuffle) reorder(state, random);
  return state;
}

export function musicPlaybackSnapshot(state) {
  return {
    revision: state.revision, track: state.track, queue: [...state.queue], queueIndex: currentIndex(state),
    startedAt: state.startedAt, pausedAt: state.pausedAt, playback: state.playback, ended: state.ended,
    repeat: state.repeat, shuffle: state.shuffle,
    hasPrevious: state.cursor > 0 || state.repeat === 'all',
    hasNext: state.cursor < state.order.length - 1 || state.repeat === 'all',
  };
}

function start(state, index, time) {
  state.cursor = state.order.indexOf(index);
  state.track = trackRef(state.queue[index]);
  state.startedAt = time;
  state.pausedAt = null;
  state.ended = false;
  state.playback++;
}

// Callers own consent, revisions on the wire, and validation of supplied track metadata.
export function applyMusicControl(state, action, fields, time, random = Math.random) {
  if (action === 'select' || action === 'enqueue') {
    const {track, queue} = fields;
    if (!track) return false;
    if (queue && action === 'select') {
      state.queue = [...queue];
      state.order = queue.map((_, index) => index);
      state.cursor = queue.findIndex(item => sameMusicTrack(item, track));
      reorder(state, random);
    }
    let index = state.queue.findIndex(item => sameMusicTrack(item, track));
    if (index < 0) {
      if (state.queue.length >= MUSIC_QUEUE_LIMIT) return 'queue-full';
      index = state.queue.length;
      state.queue.push(track);
      state.order.push(index);
    } else if (action === 'enqueue') return false;
    if (action === 'select') start(state, index, time);
  } else if (action === 'remove') {
    const {index} = fields;
    const current = currentIndex(state);
    if (!Number.isInteger(index) || index < 0 || index >= state.queue.length || index === current) return false;
    state.queue.splice(index, 1);
    state.order = state.order.filter(item => item !== index).map(item => item > index ? item - 1 : item);
    state.cursor = state.order.indexOf(current > index ? current - 1 : current);
  } else if (action === 'pause') {
    if (state.pausedAt !== null || state.ended) return false;
    state.pausedAt = time;
  } else if (action === 'resume') {
    if (state.ended) start(state, currentIndex(state), time);
    else {
      if (state.pausedAt === null) return false;
      state.startedAt += time - state.pausedAt;
      state.pausedAt = null;
    }
  } else if (action === 'seek') {
    const {positionMs} = fields;
    if (!Number.isFinite(positionMs) || positionMs < 0 || positionMs > MUSIC_POSITION_LIMIT) return false;
    state.startedAt = time - positionMs;
    state.pausedAt = state.pausedAt !== null && !state.ended ? time : null;
    state.ended = false;
    state.playback++;
  } else if (action === 'mode') {
    const {repeat, shuffle} = fields;
    if (!['off', 'all', 'one'].includes(repeat) || typeof shuffle !== 'boolean') return false;
    if (state.repeat === repeat && state.shuffle === shuffle) return false;
    state.repeat = repeat;
    if (state.shuffle !== shuffle) {
      state.shuffle = shuffle;
      reorder(state, random);
    }
  } else if (action === 'previous' || action === 'next' || action === 'ended') {
    const position = (state.pausedAt ?? time) - state.startedAt;
    let cursor = state.cursor;
    if (action === 'previous') cursor -= position > 3000 ? 0 : 1;
    else if (action !== 'ended' || state.repeat !== 'one') cursor++;
    if (cursor >= state.order.length) {
      if (state.repeat === 'all') cursor = 0;
      else if (action === 'ended') {
        const {durationMs} = fields;
        if (!Number.isFinite(durationMs) || durationMs <= 0 || durationMs > MUSIC_POSITION_LIMIT) return false;
        state.ended = true;
        state.pausedAt = state.startedAt + durationMs;
      } else return false;
    }
    if (!state.ended || action !== 'ended') {
      if (cursor < 0) cursor = state.repeat === 'all' ? state.order.length - 1 : 0;
      start(state, state.order[cursor], time);
    }
  } else return false;
  state.revision++;
  return true;
}
