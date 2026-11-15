export const lobbySourceKey = 'eagler-touhou-lobby-source-v1';

export function normalizeLobbySource(value: string, launcherProtocol: string): string | null {
  if (!value.trim() || value.length > 500) return null;
  try {
    const url = new URL(value.trim());
    if (url.username || url.password || url.search || url.hash) return null;
    const isLoopback = ['localhost', '127.0.0.1', '[::1]'].includes(url.hostname);
    if (url.protocol !== 'wss:' && !(url.protocol === 'ws:' && (launcherProtocol === 'http:' || isLoopback))) return null;
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
