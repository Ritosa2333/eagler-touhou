import {readFile, writeFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {performance} from 'node:perf_hooks';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';

// Isolated feasibility calibration. No sockets, browser, native game, or relay.
const plan = {
  purpose: 'Check whether thread CPU counters distinguish controlled busy work from a blocked wait, and retain aggregate meter-loop cost.',
  steps: [
    {kind: 'meter-pairs', pairs: 1000},
    {kind: 'blocked-wait', requestedMs: 50},
    {kind: 'busy-loop', requestedMs: 50},
    {kind: 'blocked-wait', requestedMs: 50},
    {kind: 'busy-loop', requestedMs: 50},
  ],
  omission: 'No warmup, trial selection, removal of adverse observations, or adaptive repetition.',
  limits: [
    'This is one Node process in the current shared host, not a real relay or game performance measurement.',
    'Thread CPU time excludes time spent descheduled or blocked; low CPU alone cannot distinguish those causes.',
    'Counter resolution and meter overhead are observed only for these finite intervals; submillisecond attribution is not promised.',
    'Busy-loop wall time can include host scheduling; no stable production speedup follows from this calibration.',
  ],
};
const startedAt = new Date().toISOString();
const sourcePath = fileURLToPath(import.meta.url);
const sourceBefore = await readFile(sourcePath);
const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');
const errors = [], observations = [];
const shared = new Int32Array(new SharedArrayBuffer(4));
const available = typeof process.threadCpuUsage === 'function';
if (available) {
  for (const step of plan.steps) {
    const wallStartMs = performance.now();
    const cpuStart = process.threadCpuUsage();
    let detail;
    if (step.kind === 'meter-pairs') {
      let last;
      for (let i = 0; i < step.pairs; ++i) {
        const before = process.threadCpuUsage();
        const after = process.threadCpuUsage();
        last = {before, after};
      }
      detail = {pairs: step.pairs, measuredCounterReads: 2 * step.pairs, last};
    } else if (step.kind === 'blocked-wait') {
      detail = {waitResult: Atomics.wait(shared, 0, 0, step.requestedMs)};
    } else {
      const deadline = performance.now() + step.requestedMs;
      let iterations = 0, accumulator = 1;
      do {
        for (let j = 0; j < 256; ++j) accumulator = Math.imul(accumulator ^ j, 1664525) + 1013904223 | 0;
        iterations += 256;
      } while (performance.now() < deadline);
      detail = {iterations, accumulator};
    }
    const cpuEnd = process.threadCpuUsage();
    const wallEndMs = performance.now();
    const wallElapsedMs = wallEndMs - wallStartMs;
    const userUs = cpuEnd.user - cpuStart.user, systemUs = cpuEnd.system - cpuStart.system;
    if (![wallElapsedMs, userUs, systemUs].every(Number.isFinite) || wallElapsedMs < 0 || userUs < 0 || systemUs < 0) errors.push('Invalid or regressing counter at observation ' + observations.length);
    observations.push({step, wallStartMs, wallEndMs, wallElapsedMs, cpuStart, cpuEnd,
      threadCpuDeltaMicroseconds: {user: userUs, system: systemUs, total: userUs + systemUs},
      threadCpuDeltaMs: (userUs + systemUs) / 1000,
      cpuFractionOfMeasuredWall: wallElapsedMs > 0 ? (userUs + systemUs) / (1000 * wallElapsedMs) : null,
      meterLoopMeanMsPerCounterReadIncludingLoop: step.kind === 'meter-pairs' ? wallElapsedMs / (step.pairs * 2) : null,
      detail});
  }
} else errors.push('process.threadCpuUsage is unavailable');
const sourceAfter = await readFile(sourcePath);
if (sha256(sourceAfter) !== sha256(sourceBefore)) errors.push('Source changed during calibration');
const report = {
  schema: 'th09-relay-thread-cpu-feasibility-v1',
  startedAt, completedAt: new Date().toISOString(),
  runtime: {node: process.version, platform: process.platform, arch: process.arch, executable: process.execPath},
  source: {path: sourcePath, bytes: sourceBefore.length, sha256: sha256(sourceBefore), stable: sha256(sourceAfter) === sha256(sourceBefore)},
  measurement: {api: 'process.threadCpuUsage', cpuUnit: 'microseconds reported by Node', wallClock: 'performance.now in this one process',
    bracket: 'wallStart, cpuStart, controlled step, cpuEnd, wallEnd; measured elapsed includes meter reads'},
  plan, available, checksPassed: errors.length === 0, errors, observations,
  deployedToRelay: false, productionCodeChanged: false,
};
const path = resolve('docs/multiplayer/rollback-relay-thread-cpu-feasibility.json');
const bytes = Buffer.from(JSON.stringify(report, null, 2) + '\n');
if (bytes.length > 16384) throw new Error('Calibration evidence exceeds declared 16 KiB cap');
await writeFile(path, bytes, {flag: 'wx'});
console.log(JSON.stringify({path, bytes: bytes.length, sha256: sha256(bytes), checksPassed: report.checksPassed,
  observations: observations.map(x => ({kind: x.step.kind, wallMs: x.wallElapsedMs, cpuMs: x.threadCpuDeltaMs,
    meanMsPerReadIncludingLoop: x.meterLoopMeanMsPerCounterReadIncludingLoop}))}));
if (errors.length) process.exitCode = 1;
