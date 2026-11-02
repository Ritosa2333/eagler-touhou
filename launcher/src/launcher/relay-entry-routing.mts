import type {SiteManifestShared} from '../contracts/site-manifest.mts';

export interface RelayEntrySocket {
  readonly readyState: number;
  send(data: string): void;
  close(code?: number, reason?: string): void;
  addEventListener<K extends keyof WebSocketEventMap>(type: K, listener: (event: WebSocketEventMap[K]) => void): void;
}

/** Entries must proxy the same Relay process. This is connection establishment
 * fallback, not room replication or replay of an established connection. */
export function createRelayEntryRouting(options: {
  shared(): SiteManifestShared | undefined;
  createSocket(url: string): WebSocket;
  connectTimeoutMs?: number;
}) {
  const selected = new Map<string, string>();
  function entries() {
    const shared = options.shared();
    return [...new Set([shared?.netplayRelay, ...(shared?.netplayRelayFallbacks ?? [])]
      .filter((url): url is string => !!url))];
  }
  function resolve(base: string): string {
    const candidate = selected.get(base);
    return candidate && entries().includes(candidate) ? candidate : base;
  }
  function createSocket(value: string): RelayEntrySocket {
    const bases = entries();
    const input = new URL(value);
    const primary = bases[0];
    const matching = bases.find(base => {
      const url = new URL(base);
      return url.origin === input.origin && url.pathname === input.pathname;
    });
    if (!primary || !matching || bases.length < 2) return options.createSocket(value);
    const preferred = resolve(primary);
    const candidates = [...new Set([preferred, ...bases])];
    const output = new EventTarget();
    let socket: WebSocket | null = null, state = 0, attempt = 0;
    let timer: ReturnType<typeof setTimeout> | undefined;
    let generation = 0;
    function clear() {clearTimeout(timer); timer = undefined;}
    function closed(event: CloseEvent) {
      if (state === 3) return;
      clear(); state = 3; output.dispatchEvent(event);
    }
    function connect() {
      const id = ++generation;
      const base = candidates[attempt++];
      const url = new URL(base);
      // Role, room, run and identity remain byte-for-byte query values.
      url.search = input.search;
      url.hash = '';
      let next: WebSocket;
      const failed = () => {
        if (generation !== id || state !== 0) return;
        clear(); ++generation;
        try {socket?.close();} catch {}
        if (attempt < candidates.length) connect();
        else {
          output.dispatchEvent(new Event('error'));
          closed(new CloseEvent('close', {code: 1006}));
        }
      };
      try {next = options.createSocket(url.href); socket = next;}
      catch {failed(); return;}
      timer = setTimeout(failed, options.connectTimeoutMs ?? 2000);
      next.addEventListener('open', () => {
        if (generation !== id || state !== 0) return;
        clear(); state = 1; selected.set(primary, base);
        output.dispatchEvent(new Event('open'));
      });
      next.addEventListener('message', event => {
        if (generation === id && state === 1)
          output.dispatchEvent(new MessageEvent('message', {data: event.data, origin: event.origin}));
      });
      next.addEventListener('error', () => {
        if (generation !== id) return;
        if (state === 0) failed(); else output.dispatchEvent(new Event('error'));
      });
      next.addEventListener('close', event => {
        if (generation !== id) return;
        if (state === 0 && event.code === 1006) failed();
        else closed(new CloseEvent('close', {code: event.code, reason: event.reason, wasClean: event.wasClean}));
      });
    }
    const routed = Object.assign(output, {
      send(data: string) {
        if (state !== 1 || !socket) throw new DOMException('Relay is not open', 'InvalidStateError');
        socket.send(data);
      },
      close(code?: number, reason?: string) {
        if (state >= 2) return;
        clear(); ++generation; state = 2;
        try {socket?.close(code, reason);} finally {
          closed(new CloseEvent('close', {code: code ?? 1000, reason: reason ?? '', wasClean: true}));
        }
      },
    });
    Object.defineProperty(routed, 'readyState', {get: () => state});
    queueMicrotask(() => {if (state === 0) connect();});
    return routed as unknown as RelayEntrySocket;
  }
  return {resolve, createSocket};
}
