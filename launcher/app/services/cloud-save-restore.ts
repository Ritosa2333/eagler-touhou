import { sha256Hex } from '../../src/launcher/sha256.mts';
import { type CloudSaveSnapshot, downloadCloudSave } from './cloud-saves';
import { validLocalSaveNames } from './local-save-storage';
import type { RuntimeFileSession } from './runtime';

const maxArchiveBytes = 9 * 1024 * 1024,
  maxFileBytes = 4 * 1024 * 1024,
  maxTotalBytes = 8 * 1024 * 1024;
function ensureActive(signal: AbortSignal) {
  if (signal.aborted) throw new DOMException('Cancelled', 'AbortError');
}
function missingFile(error: unknown) {
  return !!error && typeof error === 'object' && 'errno' in error && error.errno === 44;
}

/** Validate names, sizes and every uncompressed SHA-256 before touching Runtime files. */
export async function readCloudSaveArchive(
  snapshot: CloudSaveSnapshot,
  signal: AbortSignal,
  allowedFiles: readonly string[],
) {
  ensureActive(signal);
  validLocalSaveNames(allowedFiles);
  const allowed = new Set(allowedFiles);
  if (
    !snapshot.files.length ||
    snapshot.files.length > 6 ||
    snapshot.files.some(
      (file) => !allowed.has(file.name) || file.bytes > maxFileBytes || !/^[a-f0-9]{64}$/.test(file.sha256),
    ) ||
    new Set(snapshot.files.map((file) => file.name)).size !== snapshot.files.length ||
    snapshot.files.reduce((sum, file) => sum + file.bytes, 0) > maxTotalBytes
  )
    throw new Error('Invalid cloud save manifest');
  const blob = await downloadCloudSave(snapshot.product, snapshot.revision, signal);
  ensureActive(signal);
  if (blob.size > maxArchiveBytes) throw new Error('Cloud save archive too large');
  const raw = new Uint8Array(await blob.arrayBuffer());
  ensureActive(signal);
  const { unzipSync } = await import('fflate'),
    seen = new Set<string>();
  let total = 0;
  const archive = unzipSync(raw, {
    filter(info) {
      const file = snapshot.files.find((item) => item.name === info.name);
      if (
        !file ||
        !allowed.has(info.name) ||
        seen.has(info.name) ||
        info.originalSize !== file.bytes ||
        file.bytes > maxFileBytes ||
        info.name.endsWith('/')
      )
        throw new Error('Invalid cloud save archive');
      total += file.bytes;
      if (total > maxTotalBytes) throw new Error('Cloud save archive too large');
      seen.add(info.name);
      return true;
    },
  });
  if (seen.size !== snapshot.files.length || Object.keys(archive).length !== snapshot.files.length)
    throw new Error('Incomplete cloud save archive');
  for (const file of snapshot.files) {
    ensureActive(signal);
    const bytes = archive[file.name];
    if (!bytes || (await sha256Hex(bytes)) !== file.sha256) throw new Error('Cloud save checksum mismatch');
  }
  ensureActive(signal);
  return archive;
}

/** Replace the complete allowed save set and restore originals if any write or sync fails. */
export async function writeCloudSaveArchive(
  access: RuntimeFileSession,
  archive: Record<string, Uint8Array>,
  signal: AbortSignal,
  allowedFiles: readonly string[],
) {
  ensureActive(signal);
  validLocalSaveNames(allowedFiles);
  const allowed = new Set(allowedFiles),
    archiveNames = Object.keys(archive);
  if (archiveNames.some((name) => !allowed.has(name)) || new Set(archiveNames).size !== archiveNames.length)
    throw new Error('Invalid cloud save paths');
  const originals = new Map<string, number[] | null>();
  for (const name of allowedFiles) {
    ensureActive(signal);
    try {
      const response = await access.send('read', { path: name });
      ensureActive(signal);
      if (!Array.isArray(response.bytes)) throw new Error('Invalid local save');
      originals.set(name, response.bytes);
    } catch (error) {
      if (missingFile(error)) originals.set(name, null);
      else throw error;
    }
  }
  const changes = allowedFiles.filter((name) => archive[name] || originals.get(name) !== null);
  const changed: string[] = [];
  try {
    for (const path of changes) {
      ensureActive(signal);
      changed.push(path);
      const bytes = archive[path];
      if (bytes) await access.send('write', { path, bytes: Array.from(bytes) });
      else await access.send('remove', { path });
      ensureActive(signal);
    }
    ensureActive(signal);
    await access.sync();
    ensureActive(signal);
  } catch (error) {
    const rollbackErrors: unknown[] = [];
    for (const path of changed) {
      const bytes = originals.get(path);
      try {
        if (bytes) await access.send('write', { path, bytes });
        else await access.send('remove', { path });
      } catch (rollbackError) {
        if (!missingFile(rollbackError)) rollbackErrors.push(rollbackError);
      }
    }
    try {
      await access.sync();
    } catch (rollbackError) {
      rollbackErrors.push(rollbackError);
    }
    if (rollbackErrors.length)
      throw new AggregateError([error, ...rollbackErrors], 'Cloud save restore failed and rollback was incomplete');
    throw error;
  }
  ensureActive(signal);
  await access.retire();
}
