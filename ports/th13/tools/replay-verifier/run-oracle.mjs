import {spawnSync} from 'node:child_process';
import {readFileSync, mkdirSync, openSync, readSync, closeSync,
        createReadStream} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

const sha256File = path => new Promise((done, fail) => {
  const hash = createHash('sha256');
  createReadStream(path).on('data', c => hash.update(c)).on('end',
      () => done(hash.digest('hex'))).on('error', fail);
});

const here = fileURLToPath(new URL('./', import.meta.url));
const project = resolve(here, '../..');
const corpus = JSON.parse(readFileSync(resolve(here, 'corpus.json')));
const option = name => {
  const i = process.argv.indexOf(name);
  return i < 0 ? null : process.argv[i + 1];
};
const wanted = option('--case');
const lane = option('--lane') || 'all';
const ids = wanted ? [wanted]
    : lane === 'all' ? Object.values(corpus.suites).flat()
                     : corpus.suites[lane];
if (!ids) throw Error('Unknown suite: ' + lane);
const output = resolve(option('--output') || resolve(project, 'artifacts/replay-verifier'));
mkdirSync(output, {recursive: true});

const runHarness = args => {
  const run = spawnSync(process.execPath, [
    '--experimental-wasi-unstable-preview1',
    resolve(project, 'tools/run-harness.mjs'), ...args,
  ], {cwd: project, encoding: 'utf8'});
  if (run.status !== 0) throw Error('run-harness failed:\n' + run.stdout + run.stderr);
  return run.stdout.trim().split('\n').pop();
};

for (const id of ids) {
  const fixture = corpus.cases.find(c => c.id === id);
  if (!fixture) throw Error('Unknown case ' + id);
  const target = resolve(output, id + '.candidate.jsonl');
  const outerRoot = resolve(project, '../../..');
  const mounted = target.startsWith(outerRoot)
      ? target.slice(outerRoot.length) : null;
  if (!mounted) throw Error('Candidate output must live under the workspace: ' + target);
  let summary;
  if (fixture.kind === 'noinput') {
    summary = runHarness([fixture.harnessMode, '/artifacts/reference/raw', mounted,
      String(fixture.frames), 'st01.ecl']);
  } else {
    summary = runHarness([fixture.harnessMode, '/games/replays/th13_udSr01.rpy',
      '/artifacts/reference/raw', mounted]);
  }
  const fd = openSync(target, 'r');
  const head = Buffer.alloc(1 << 16);
  const got = readSync(fd, head, 0, head.length, 0);
  closeSync(fd);
  const first = head.toString('utf8', 0, got).split('\n', 1)[0];
  const frame0 = JSON.parse(first).frame;
  if (frame0 !== 0) throw Error('Candidate manifest does not start at frame 0: ' + id);
  const sha = await sha256File(target);
  console.log(JSON.stringify({id, target, framesFirst: frame0, sha256: sha,
                              summary}));
}
