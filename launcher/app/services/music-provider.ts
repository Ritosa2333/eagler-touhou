import {linkAbortSignals} from './abort-signals';

export type MusicPlatform = 'netease' | 'qq';
export interface MusicRef {
  platform: MusicPlatform;
  id: string;
}
export interface MusicTrack extends MusicRef {
  name: string;
  artist: string;
  cover?: string;
  album?: string;
}
export interface MusicProvider {
  search(platform: MusicPlatform, query: string, signal: AbortSignal): Promise<MusicTrack[]>;
  playlist(platform: MusicPlatform, id: string, signal: AbortSignal): Promise<MusicTrack[]>;
  track(ref: MusicRef, signal: AbortSignal): Promise<MusicTrack>;
  stream(ref: MusicRef, signal: AbortSignal): Promise<[string, ...string[]]>;
}
export function validMusicRef(value: unknown): value is MusicRef {
  if (!value || typeof value !== 'object') return false;
  const ref = value as MusicRef;
  return (
    (ref.platform === 'netease' || ref.platform === 'qq') &&
    typeof ref.id === 'string' &&
    /^[A-Za-z0-9_-]{1,64}$/.test(ref.id)
  );
}
export function musicPlaylistId(platform: MusicPlatform, input: string): string {
  if (/^\d{1,24}$/.test(input.trim())) return input.trim();
  try {
    const url = new URL(input.trim());
    const allowed =
      platform === 'netease' ? url.hostname === 'music.163.com' : ['y.qq.com', 'i.y.qq.com'].includes(url.hostname);
    if (!allowed) throw Error();
    const id =
      new URLSearchParams(url.hash.split('?')[1] || url.search).get('id') ||
      url.searchParams.get('disstid') ||
      /\/playlist\/(\d+)/.exec(url.pathname)?.[1];
    if (id && /^\d{1,24}$/.test(id)) return id;
  } catch {}
  throw Error('invalidPlaylist');
}

export function createMetingProvider(endpoint: string, request: typeof fetch = fetch): MusicProvider {
  const resultLimit = 100;
  const server = (platform: MusicPlatform) => platform === 'qq' ? 'tencent' : 'netease';

  async function get(platform: MusicPlatform, params: Record<string, string>, signal: AbortSignal) {
    const url = new URL(endpoint);
    url.search = new URLSearchParams({ server: server(platform), ...params }).toString();
    const timeout = new AbortController();
    const lookup = linkAbortSignals(signal, timeout.signal);
    const timer = setTimeout(() => timeout.abort(), 8000);
    try {
      const response = await request(url.href, {
        signal: lookup.signal, credentials: 'omit', referrerPolicy: 'no-referrer',
      });
      if (!response.ok) throw Error('unavailable');
      return await response.json();
    } finally {
      clearTimeout(timer);
      lookup.dispose();
    }
  }
  function httpsUrl(raw: unknown) {
    if (typeof raw !== 'string') throw Error('unavailable');
    const url = new URL(raw);
    if (!['http:', 'https:'].includes(url.protocol) || url.username || url.password || url.port)
      throw Error('unavailable');
    url.protocol = 'https:';
    return url;
  }
  function track(platform: MusicPlatform, raw: unknown): MusicTrack | null {
    if (!raw || typeof raw !== 'object') return null;
    const item = raw as {
      id?: unknown; source?: unknown; name?: unknown; artist?: unknown; album?: unknown; pic_id?: unknown;
    };
    const id = typeof item.id === 'number' && Number.isSafeInteger(item.id) && item.id > 0 ? String(item.id) : item.id;
    const ref = { platform, id };
    if (item.source !== server(platform) || !validMusicRef(ref)) return null;
    const cover = platform === 'qq' && typeof item.pic_id === 'string' && /^[a-zA-Z0-9]{1,32}$/.test(item.pic_id)
      ? `https://y.gtimg.cn/music/photo_new/T002R300x300M000${item.pic_id}.jpg` : undefined;
    return {
      ...ref,
      name: typeof item.name === 'string' ? item.name.slice(0, 200) : ref.id,
      artist: Array.isArray(item.artist)
        ? item.artist.filter((name: unknown) => typeof name === 'string').join(' / ').slice(0, 300) : '',
      album: typeof item.album === 'string' ? item.album.slice(0, 200) : undefined,
      cover,
    };
  }
  function list(platform: MusicPlatform, data: unknown): MusicTrack[] {
    if (!Array.isArray(data)) throw Error('unavailable');
    const unique = new Map<string, MusicTrack>();
    for (const raw of data.slice(0, resultLimit)) {
      const item = track(platform, raw);
      if (item && !unique.has(item.id)) unique.set(item.id, item);
    }
    return [...unique.values()];
  }

  return {
    async search(platform, query, signal) {
      // Larger QQ requests fall back to the platform's default page size.
      const limit = platform === 'qq' ? 60 : resultLimit;
      return list(platform, await get(platform, { type: 'search', id: query, limit: String(limit) }, signal));
    },
    async playlist(platform, id, signal) {
      return list(platform, await get(platform, { type: 'playlist', id }, signal));
    },
    async track(ref, signal) {
      const result = track(ref.platform, await get(ref.platform, { type: 'details', id: ref.id }, signal));
      if (!result || result.id !== ref.id) throw Error('unavailable');
      if (result.cover) return result;
      try {
        const data = await get(ref.platform, { type: 'cover', id: ref.id, size: '300' }, signal);
        const url = httpsUrl(data?.url);
        const coverHost = ref.platform === 'netease'
          ? url.hostname.endsWith('.music.126.net') : url.hostname === 'y.gtimg.cn';
        if (coverHost) result.cover = url.href;
      } catch {}
      return result;
    },
    async stream(ref, signal) {
      const data = await get(ref.platform, { type: 'url', id: ref.id, br: '320' }, signal);
      if (data?.trial || data?.freeTrialInfo) throw Error('trial');
      const url = httpsUrl(data?.url);
      const mediaHost = ref.platform === 'netease'
        ? url.hostname.endsWith('.music.126.net')
        : url.hostname === 'aqqmusic.tc.qq.com' || url.hostname.endsWith('.stream.qqmusic.qq.com');
      if (!mediaHost) throw Error('unavailable');
      const urls: [string, ...string[]] = [url.href];
      const neteaseCdn = ref.platform === 'netease' && /^(m\d+)(c?)\.music\.126\.net$/.exec(url.hostname);
      if (neteaseCdn) {
        url.hostname = `${neteaseCdn[1]}${neteaseCdn[2] ? '' : 'c'}.music.126.net`;
        urls.push(url.href);
      }
      return urls;
    },
  };
}
