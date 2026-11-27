import {createHash} from 'node:crypto';
import {readFileSync} from 'node:fs';
import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';

const here = dirname(fileURLToPath(import.meta.url));
const common = resolve(process.env.EAGLER_COMMON_ROOT || resolve(here, '../../../../common'));
const {TraceCapture} = await import(pathToFileURL(resolve(common, 'testkit/replay-verifier/capture-file.mjs')));

export const requiredCategories = ['clocks', 'input', 'rng', 'player', 'economy', 'boss', 'entities'];

export function loadRaw(path) {
  return readFileSync(path, 'utf8').split('\n').filter(Boolean).map(line => JSON.parse(line));
}

export function framesOf(rows) {
  return rows.filter(row => row.record === true);
}

export function writeTrace(path, {identity, provider, provenance, rows}) {
  const frames = framesOf(rows);
  const end = rows.find(row => row.ended === true && row.record === false);
  const capture = new TraceCapture(path, {
    identity,
    provider,
    categories: requiredCategories,
    provenance: {...provenance, align: 'post-mainmenu-replay'},
  });
  let stage = null;
  let index = 0;
  for (const row of frames) {
    const categories = {};
    for (const key of requiredCategories) {
      categories[key] = key === 'clocks' ? logicClocks(row.categories[key]) : row.categories[key];
    }
    const nextStage = row.stageName || String(row.stageId || 'stage');
    capture.tick({
      stage: stage === null ? nextStage : nextStage,
      replaySampleIndex: index,
      appliedInput: row.appliedInput | 0,
      clocks: logicClocks(row.clocks),
      scalars: row.scalars,
      categories,
    });
    stage = nextStage;
    index++;
  }
  const complete = !!(end && end.complete && frames.length);
  capture.finish({
    complete,
    reason: end?.reason || (frames.length ? 'replay-complete' : 'no-replay-frames'),
    evidence: {samples: frames.length, align: 'post-mainmenu-replay'},
  });
  return {ticks: frames.length, complete, reason: end?.reason || null};
}

function logicClocks(clocks) {
  if (!clocks || clocks.realTimeTotal === undefined) return clocks;
  const copy = {...clocks};
  delete copy.realTimeTotal;
  return copy;
}

export function digest(value) {
  return createHash('sha256').update(JSON.stringify(value)).digest('hex').slice(0, 32);
}
