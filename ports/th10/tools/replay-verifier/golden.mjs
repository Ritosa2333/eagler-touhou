import {createHash} from 'node:crypto';
import {createReadStream} from 'node:fs';
import {readFile} from 'node:fs/promises';
import {createInterface} from 'node:readline';
import {dirname, isAbsolute, join, relative, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createGunzip} from 'node:zlib';

const here = dirname(fileURLToPath(import.meta.url));
export const defaultGoldenRoot = join(here, 'golden');

function stream(path) {
  const input = createReadStream(path);
  return path.endsWith('.gz') ? input.pipe(createGunzip()) : input;
}

export async function readJsonMaybeGzip(path) {
  const chunks = [];
  for await (const chunk of stream(path)) chunks.push(chunk);
  return JSON.parse(Buffer.concat(chunks).toString('utf8'));
}

export async function* readJsonLinesMaybeGzip(path) {
  const input = stream(path);
  const lines = createInterface({input, crlfDelay: Infinity});
  try {
    for await (const line of lines) if (line.trim()) yield JSON.parse(line);
  } finally {
    lines.close();
    input.destroy();
  }
}

async function sha256(path) {
  const hash = createHash('sha256');
  for await (const chunk of createReadStream(path)) hash.update(chunk);
  return hash.digest('hex');
}

export function assetPath(goldenRoot, asset) {
  const path = resolve(goldenRoot, asset.path);
  const outside = relative(resolve(goldenRoot), path);
  if (outside.startsWith('..') || isAbsolute(outside)) throw Error(`Golden asset escapes its root: ${asset.path}`);
  return path;
}

export async function loadGoldenManifest(goldenRoot = defaultGoldenRoot, {verify = true} = {}) {
  const root = resolve(goldenRoot);
  const manifest = JSON.parse(await readFile(join(root, 'manifest.json'), 'utf8'));
  if (manifest.schema !== 'eagler/replay-golden-set/v1' || manifest.game !== 'th10')
    throw Error('Unsupported TH10 golden manifest');
  if (!manifest.identity?.executableSha256 || !manifest.identity?.resourceSha256)
    throw Error('Golden manifest is missing original identity');
  for (const suite of ['quick', 'daily']) {
    if (!Array.isArray(manifest.suites?.[suite]) || !manifest.suites[suite].length)
      throw Error(`Golden manifest is missing the ${suite} suite`);
  }
  if (verify) {
    for (const [id, asset] of Object.entries(manifest.assets ?? {})) {
      const actual = await sha256(assetPath(root, asset));
      if (actual !== asset.sha256) throw Error(`Golden asset hash mismatch for ${id}: ${actual}`);
    }
  }
  return {root, manifest};
}
