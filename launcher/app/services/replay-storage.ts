import {idbfsBytes, openIdbfsDatabase, type IdBfsEntry} from './idbfs';
import type {ProductId} from '../../src/contracts/product-catalog.mts';
import {isReplayFilePath} from '../../src/launcher/replay-files.mts';
import {isTh09ReplayDirectory, parseTh09ReplayMatch, type Th09ReplayMatch} from '../../src/launcher/th09-replay-match.mts';
import {motionReplayName} from '../../src/launcher/replay-motion.mts';
import {replayStorageLocation} from '../../src/launcher/replay-storage-policy.mts';
import {exportReplayName as th09ReplayName} from '../../../ports/th09/web/sdl-runtime/motion-replay.mjs';

export interface StoredReplay {path: string; size: number; bytes: Uint8Array; modifiedAt?: number; replayMatch?: Th09ReplayMatch}
export interface ReplayStorageSelection {path?: string; directory?: string; language?: string}
export interface ReplayStorageReader {read(product: ProductId, selection?: ReplayStorageSelection): Promise<StoredReplay[]>}
/** Compatibility reader for Emscripten IDBFS v21. Runtime remains the sole writer.
 * A missing database is never created; an unknown schema fails without fallback. */
export function createReplayStorageReader(environment: () => {indexedDB: IDBFactory; keyRange: typeof IDBKeyRange}): ReplayStorageReader {
  return {async read(product, selection = {}) {
    const {game, database: databaseName, root, validName, sharedExtensionSlot} = replayStorageLocation(product, selection.language);
    const extendedReplayName = game === 'th09' ? th09ReplayName : motionReplayName;
    const validPath = (path: string) => isReplayFilePath(path) && validName(path.slice(path.lastIndexOf('/') + 1));
    if (selection.path && !validPath(selection.path)) throw new Error('录像路径无效');
    if (selection.directory && !isTh09ReplayDirectory(selection.directory)) throw new Error('录像文件夹无效');
    const {indexedDB, keyRange} = environment();
    if (!indexedDB || !keyRange) throw new Error('浏览器无法读取本地录像存储');
    const database = await openIdbfsDatabase(indexedDB, databaseName);
    if (!database) return [];
    try {
      return await new Promise<StoredReplay[]>((resolve, reject) => {
        const transaction = database.transaction('FILE_DATA', 'readonly'), store = transaction.objectStore('FILE_DATA');
        const records = new Map<string, IdBfsEntry>();
        // Limit downloads to their file/folder. List contents stay local to this
        // transaction and are released after the model extracts row metadata.
        const prefix = root + '/' + (selection.directory ?? 'replay') + '/';
        const request = selection.path ? null : store.openCursor(keyRange.bound(prefix, prefix.slice(0, -1) + '0', false, true));
        if (selection.path) {
          const physical = root + '/' + (sharedExtensionSlot ? selection.path.replace(/\.rpyx$/i, '.rpy') : selection.path);
          for (const path of new Set([physical, root + '/' + selection.path, physical.replace(/\.rpyx?$/i, '.json')])) {
            const get = store.get(path); get.onsuccess = () => {if (get.result) records.set(path, get.result);};
          }
        } else if (request) request.onsuccess = () => {const cursor = request.result; if (cursor) {if (typeof cursor.key === 'string') records.set(cursor.key, cursor.value); cursor.continue();}};
        transaction.onerror = transaction.onabort = () => reject(transaction.error ?? new Error('读取录像失败'));
        transaction.oncomplete = () => {
          try {
            const files: StoredReplay[] = [];
            for (const [storedPath, entry] of records) {
              const relative = storedPath.slice(root.length + 1);
              if (!storedPath.startsWith(root + '/replay/') || !validPath(relative) || (entry.mode & 0xf000) !== 0x8000) continue;
              const bytes = idbfsBytes(entry.contents);
              if (!bytes || bytes.length > 64 * 1024 * 1024) throw new Error('本地录像内容无效');
              const path = product === 'th10mp' && relative.endsWith('.rpy') ? relative + 'x' : sharedExtensionSlot ? extendedReplayName(relative, bytes, Number(game.slice(2))) : relative;
              if (selection.path && path !== selection.path || selection.directory && path.slice(0, path.lastIndexOf('/')) !== selection.directory) continue;
              let match: Th09ReplayMatch | null = null;
              const sidecar = idbfsBytes(records.get(storedPath.replace(/\.rpyx?$/i, '.json'))?.contents);
              if (product === 'th09mp' && sidecar && sidecar.length <= 16384) {try {match = parseTh09ReplayMatch(JSON.parse(new TextDecoder().decode(sidecar)));} catch {}}
              const modifiedAt = Number(entry.timestamp);
              files.push({path, bytes, size: bytes.length, ...(Number.isFinite(modifiedAt) && modifiedAt > 0 && modifiedAt <= 8.64e15 ? {modifiedAt} : {}), ...(match ? {replayMatch: match} : {})});
            }
            resolve(files);
          } catch (error) {reject(error);}
        };
      });
    } finally {database.close();}
  }};
}
