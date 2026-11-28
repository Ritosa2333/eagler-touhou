import {createHash} from 'node:crypto';
import {readdirSync, readFileSync, statSync} from 'node:fs';
import {join, relative, resolve} from 'node:path';

const resourceRoots = ['Image', 'CS', 'FX', 'Model', 'Story', 'Sound', 'BGM'];

export function sha256(bytes) {
  return createHash('sha256').update(bytes).digest('hex');
}

export function resourceSha256(assets) {
  const files = [];
  for (const root of resourceRoots) {
    const dir = resolve(assets, root);
    walk(dir, file => {
      if (file.endsWith('.wav') && resourceRoots.includes('BGM') && file.includes('/BGM/')) return;
      const info = statSync(file);
      files.push(`${relative(assets, file).replaceAll('\\', '/')}\t${info.size}`);
    });
  }
  files.sort();
  return sha256(files.join('\n'));
}

function walk(dir, visit) {
  let entries = [];
  try { entries = readdirSync(dir, {withFileTypes: true}); }
  catch { return; }
  for (const entry of entries) {
    const path = join(dir, entry.name);
    if (entry.isDirectory()) walk(path, visit);
    else if (entry.isFile() && !entry.name.endsWith('.ogg.wav') && !(entry.name.endsWith('.wav') && path.includes('/BGM/'))) visit(path);
  }
}

export function comparisonIdentity({assets, replay, executable}) {
  return {
    game: 'thsss',
    profile: 'retail-1.01/post-mainmenu-replay-v1',
    replaySha256: sha256(readFileSync(replay)),
    executableSha256: sha256(readFileSync(executable)),
    resourceSha256: resourceSha256(assets),
    stateSchema: 'thsss/authoritative-state/v1',
    traceCodec: 'jsonl/v1',
    digestAlgorithm: 'sha256-truncated-128/canonical-json-v1',
  };
}
