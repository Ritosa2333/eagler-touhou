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
export interface MusicStream {
  urls: string[];
  duration: number;
}
export interface MusicProvider {
  search(platform: MusicPlatform, query: string, offset: number, signal: AbortSignal): Promise<MusicTrack[]>;
  playlist(platform: MusicPlatform, id: string, offset: number, signal: AbortSignal): Promise<MusicTrack[]>;
  track(ref: MusicRef, signal: AbortSignal): Promise<MusicTrack>;
  stream(ref: MusicRef, signal: AbortSignal): Promise<MusicStream>;
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

/** Only this adapter knows TuneWeave routes and response shapes. */
export function createTuneWeaveProvider(base: string, request: typeof fetch = fetch): MusicProvider {
  async function get(path: string, signal: AbortSignal) {
    const response = await request(`${base.replace(/\/$/, '')}/v1/${path}`, {
      signal,
      credentials: 'omit',
      referrerPolicy: 'no-referrer',
    });
    if (!response.ok) throw Error('unavailable');
    const body = await response.json();
    if (body?.ok !== true) throw Error('unavailable');
    return body.data;
  }
  function track(value: unknown): MusicTrack {
    if (!validMusicRef(value)) throw Error('unavailable');
    const data = value as MusicRef & {
      name?: unknown;
      artists?: { name?: unknown }[];
      album?: { name?: unknown; cover_url?: unknown };
    };
    let cover: string | undefined;
    try {
      const url = new URL(String(data.album?.cover_url));
      if (['http:', 'https:'].includes(url.protocol) && !url.username && !url.password) {
        url.protocol = 'https:';
        cover = url.href;
      }
    } catch {}
    return {
      platform: data.platform,
      id: data.id,
      name: typeof data.name === 'string' ? data.name.slice(0, 200) : data.id,
      cover,
      album: typeof data.album?.name === 'string' ? data.album.name.slice(0, 200) : undefined,
      artist: Array.isArray(data.artists)
        ? data.artists
            .map((a) => (typeof a?.name === 'string' ? a.name : ''))
            .filter(Boolean)
            .join(' / ')
            .slice(0, 300)
        : '',
    };
  }
  function list(value: unknown, wrapped = false) {
    if (!Array.isArray(value)) throw Error('unavailable');
    return value.slice(0, 30).flatMap((item) => {
      const data = wrapped ? item?.data : item;
      return validMusicRef(data) ? [track(data)] : [];
    });
  }
  const refPath = (ref: MusicRef) => `tracks/${encodeURIComponent(`${ref.platform}:${ref.id}`)}`;
  return {
    async search(platform, query, offset, signal) {
      return list(
        await get(
          `search?${new URLSearchParams({ platform, type: 'track', q: query, limit: '30', offset: String(offset) })}`,
          signal,
        ),
        true,
      );
    },
    async playlist(platform, id, offset, signal) {
      return list(
        await get(`playlists/${encodeURIComponent(`${platform}:${id}`)}/tracks?limit=30&offset=${offset}`, signal),
      );
    },
    async track(ref, signal) {
      const result = track(await get(refPath(ref), signal));
      if (result.platform !== ref.platform || result.id !== ref.id) throw Error('unavailable');
      return result;
    },
    async stream(ref, signal) {
      const data = await get(`${refPath(ref)}/stream?quality=standard&fallback=false`, signal);
      if (data?.trial != null) throw Error('trial');
      if (data?.resolved_track !== `${ref.platform}:${ref.id}`) throw Error('unavailable');
      if (Object.keys(data.headers || {}).length) throw Error('unavailable');
      const urls = [...new Set([data.url, ...(Array.isArray(data.backup_urls) ? data.backup_urls : [])])].flatMap(
        (raw) => {
          if (typeof raw !== 'string') return [];
          try {
            const url = new URL(raw);
            if (!['https:', 'http:'].includes(url.protocol) || url.username || url.password) return [];
            url.protocol = 'https:';
            return [url.href];
          } catch {
            return [];
          }
        },
      );
      if (!urls.length || !Number.isFinite(data.duration_ms) || data.duration_ms <= 0) throw Error('unavailable');
      return { urls: [...new Set(urls)].slice(0, 8), duration: data.duration_ms / 1000 };
    },
  };
}
