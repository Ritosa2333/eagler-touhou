import {MUSIC_QUEUE_LIMIT} from '../../../common/contracts/music-playback.mjs';

const origin = 'https://music.163.com';
const numericId = /^[1-9][0-9]{0,23}$/;
const cacheTime = 120_000;

export async function readNetEasePlaylist(id, fetcher = fetch) {
  if (!numericId.test(id)) throw new TypeError('Invalid playlist ID');
  const signal = AbortSignal.timeout(7000);
  async function read(path, fields) {
    const url = new URL(path, origin);
    url.search = new URLSearchParams(fields);
    const response = await fetcher(url, {signal, redirect: 'error', headers: {referer: origin + '/'}});
    if (!response.ok) throw new Error('NetEase playlist unavailable');
    const data = await response.json();
    if (data?.code !== 200) throw new Error('NetEase playlist unavailable');
    return data;
  }
  const {playlist} = await read('/api/v6/playlist/detail', {id});
  if (!Array.isArray(playlist?.trackIds)) throw new Error('NetEase playlist unavailable');
  const ids = [...new Set(playlist.trackIds.map(track => track?.id)
    .filter(id => Number.isSafeInteger(id) && id > 0))].slice(0, MUSIC_QUEUE_LIMIT);
  if (!ids.length) return [];
  const {songs} = await read('/api/v3/song/detail', {c: JSON.stringify(ids.map(id => ({id})))});
  if (!Array.isArray(songs)) throw new Error('NetEase songs unavailable');
  const byId = new Map(songs.filter(song => song && typeof song.name === 'string').map(song => [song.id, song]));
  return ids.flatMap(id => {
    const song = byId.get(id);
    if (!song) return [];
    return [{id, source: 'netease', name: song.name.slice(0, 200),
      artist: Array.isArray(song.ar) ? song.ar.flatMap(artist => typeof artist?.name === 'string' ? [artist.name] : []).slice(0, 20) : [],
      album: typeof song.al?.name === 'string' ? song.al.name.slice(0, 200) : undefined}];
  });
}

export function createNetEasePlaylistHandler(read = readNetEasePlaylist) {
  const pending = new Map(), cache = new Map();
  return async (request, response) => {
    const reply = (status, value) => {
      response.writeHead(status, {'content-type': 'application/json',
        'access-control-allow-origin': '*', 'cache-control': status === 200 ? 'public, max-age=120' : 'no-store'});
      response.end(JSON.stringify(value));
    };
    if (request.method !== 'GET') {reply(405, null);return;}
    const id = new URL(request.url, 'http://localhost').searchParams.get('id');
    if (id === null || !numericId.test(id)) {reply(400, null);return;}
    const cached = cache.get(id);
    if (cached?.until > Date.now()) {reply(200, cached.tracks);return;}
    let lookup = pending.get(id);
    if (!lookup) {
      if (pending.size >= 4) {reply(503, null);return;}
      lookup = read(id).then(tracks => {
        cache.delete(id);
        if (cache.size >= 128) cache.delete(cache.keys().next().value);
        cache.set(id, {tracks, until: Date.now() + cacheTime});
        return tracks;
      }).finally(() => pending.delete(id));
      pending.set(id, lookup);
    }
    try {reply(200, await lookup);} catch {reply(502, null);}
  };
}
