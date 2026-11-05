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
      if (!Array.isArray(value?.urls) || !value.urls.length || value.urls.length > 8
          || value.urls.some(url => typeof url !== 'string' || !/^turns?:[^\s]+$/i.test(url))
          || typeof value.sharedSecret !== 'string' || !value.sharedSecret) return current;
      current = {urls: value.urls, sharedSecret: value.sharedSecret};
    } catch {
      // Retain the last valid configuration while an atomic publication is unavailable.
    }
    return current;
  };
}
