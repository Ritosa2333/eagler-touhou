import {readFileSync} from 'node:fs';

export function createTurnConfigReader(path, fallback, read = readFileSync, clock = Date.now) {
  let current = fallback;
  let nextRead = 0;
  return () => {
    const now = clock();
    if (!path || now < nextRead) return current;
    nextRead = now + 1000;
    try {
      const content = read(path, 'utf8');
      if (Buffer.byteLength(content, 'utf8') > 16384) return current;
      const value = JSON.parse(content);
      const backups = value?.backups ?? [];
      if (!Array.isArray(backups) || backups.length > 3) return current;
      const nodes = [value, ...backups];
      if (!nodes.every(node => Array.isArray(node?.urls) && node.urls.length && node.urls.length <= 8
          && node.urls.every(url => typeof url === 'string' && /^turns?:[^\s]+$/i.test(url))
          && typeof node.sharedSecret === 'string' && node.sharedSecret)) return current;
      current = {urls: value.urls, sharedSecret: value.sharedSecret, ...(backups.length ? {backups} : {})};
    } catch {
      // Retain the last valid configuration while an atomic publication is unavailable.
    }
    return current;
  };
}
