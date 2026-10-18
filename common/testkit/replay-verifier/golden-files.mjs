import {createHash} from 'node:crypto';
import {createReadStream} from 'node:fs';
import {readFile} from 'node:fs/promises';
import {createInterface} from 'node:readline';
import {isAbsolute, relative, resolve} from 'node:path';
import {createGunzip} from 'node:zlib';
import {compareTraces} from './compare.mjs';

export async function sha256File(path) {
  const hash = createHash('sha256');
  for await (const chunk of createReadStream(path)) hash.update(chunk);
  return hash.digest('hex');
}
export function assetPath(root, asset) {
  const path = resolve(root, asset.path), outside = relative(resolve(root), path);
  if (outside.startsWith('..') || isAbsolute(outside)) throw Error('Golden path escapes root');
  return path;
}
export async function* readTraceFile(path) {
  const source = createReadStream(path), input = path.endsWith('.gz') ? source.pipe(createGunzip()) : source;
  const lines = createInterface({input, crlfDelay: Infinity});
  try { for await (const line of lines) if (line.trim()) yield JSON.parse(line); }
  finally { lines.close(); input.destroy(); source.destroy(); }
}
export async function verifyGoldenFiles(root, {game, suites}) {
  const manifest = JSON.parse(await readFile(resolve(root, 'manifest.json'), 'utf8'));
  if (manifest.schema !== 'eagler/replay-golden-set/v1' || manifest.game !== game)
    throw Error('Golden manifest identity mismatch');
  for (const [lane, ids] of Object.entries(suites)) {
    if (JSON.stringify(manifest.suites?.[lane]) !== JSON.stringify(ids)) throw Error('Golden suite order mismatch: ' + lane);
    for (const id of ids) {
      const asset = manifest.assets?.[id];
      if (!asset?.sha256 || !Number.isSafeInteger(asset.ticks) || asset.ticks <= 0) throw Error('Missing golden asset: ' + id);
      const path = assetPath(root, asset);
      if (await sha256File(path) !== asset.sha256) throw Error('Golden hash mismatch: ' + id);
      const records=readTraceFile(path),first=await records.next();await records.return();
      const identity=first.value?.comparisonIdentity;
      if(identity?.game!==game||identity.executableSha256!==manifest.identity?.executableSha256||identity.resourceSha256!==manifest.identity?.resourceSha256||identity.replaySha256!==asset.replaySha256||identity.stateSchema!==asset.stateSchema)
        throw Error('Golden provenance identity mismatch: '+id);
      if(!first.value?.provenance?.provider?.includes('retail-'))throw Error('Golden must be original-derived: '+id);
      const result = await compareTraces(readTraceFile(path), readTraceFile(path));
      if (result.status !== 'PASS' || result.comparedTicks !== asset.ticks) throw Error('Incomplete golden trace: ' + id);
    }
  }
  return manifest;
}
