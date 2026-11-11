import {isSafeStoredZipName} from '../../package/stored-zip.mjs';

export const archiveLimit = 1536 * 1024 * 1024;

export function selectRetailArchiveEntries<T>(entries: Iterable<T>, limits: Record<string, number>,
  identify: (entry: T) => {path: string; size: number}) {
  const selected: {entry: T; path: string; size: number}[] = [];
  let total = 0, count = 0;
  for (const entry of entries) {
    if (++count > 16384) throw Error('Too many archive entries');
    const {path, size} = identify(entry);
    const name = path.split('/').at(-1)!.toLowerCase();
    if (!Object.hasOwn(limits, name)) continue;
    if (!isSafeStoredZipName(path) || /^[a-z]:/i.test(path)) throw Error('Unsafe game archive path');
    total += size;
    if (!Number.isSafeInteger(size) || size <= 0 || size > limits[name] || total > archiveLimit || selected.length >= 256)
      throw Error(name + ': game archive entry is too large');
    selected.push({entry, path, size});
  }
  return selected;
}
