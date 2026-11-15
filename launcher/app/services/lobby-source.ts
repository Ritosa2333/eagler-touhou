export const lobbySourceKey = 'eagler-touhou-lobby-source-v1';

function isLocalRelayHost(hostname: string): boolean {
  const host = hostname.replace(/\.$/, '');
  if (host === 'localhost' || host.endsWith('.localhost') || host.endsWith('.local') || host === '[::1]') return true;
  // URL normalizes IPv4 literals before the address-space check.
  return /^(?:169\.254|172\.(?:1[6-9]|2[0-9]|3[01])|192\.168)\.\d+\.\d+$/.test(host)
    || /^(?:10|127)\.\d+\.\d+\.\d+$/.test(host)
    || /^\[(?:f[cd][0-9a-f]{2}|fe[89ab][0-9a-f]):/.test(host);
}

export function normalizeLobbySource(value: string, launcherProtocol: string): string | null {
  if (!value.trim() || value.length > 500) return null;
  try {
    const url = new URL(value.trim());
    if (url.username || url.password || url.search || url.hash) return null;
    if (url.protocol !== 'wss:' && !(url.protocol === 'ws:' && (launcherProtocol === 'http:' || isLocalRelayHost(url.hostname)))) return null;
    return url.href;
  } catch { return null; }
}

export function createLobbySource(storage: Storage | null, launcherProtocol: string) {
  let saved = '';
  try { saved = storage?.getItem(lobbySourceKey) ?? ''; } catch {}
  const relay = normalizeLobbySource(saved, launcherProtocol);
  const key = (name: string) => lobbySourceKey + ':' + relay + ':' + name;
  const scopedStorage = relay && storage ? {
    getItem: (name: string) => storage.getItem(key(name)),
    setItem: (name: string, value: string) => storage.setItem(key(name), value),
    removeItem: (name: string) => storage.removeItem(key(name)),
  } : storage;
  let member = '';
  return {
    relay, scopedStorage,
    local: relay !== null && isLocalRelayHost(new URL(relay).hostname),
    memberId() {
      if (!member) {
        try { member = scopedStorage?.getItem('member') ?? ''; } catch {}
        if (!/^[a-z0-9]{16,40}$/.test(member)) {
          member = [...crypto.getRandomValues(new Uint32Array(4))].map(n => n.toString(16).padStart(8, '0')).join('');
          try { scopedStorage?.setItem('member', member); } catch {}
        }
      }
      return member;
    },
    save(input: string) {
      const value = normalizeLobbySource(input, launcherProtocol);
      if (!storage || input.trim() && !value) return false;
      try {
        if (value) storage.setItem(lobbySourceKey, value);
        else storage.removeItem(lobbySourceKey);
        return true;
      } catch { return false; }
    },
  };
}
