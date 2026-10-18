import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {existsSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

// Sequential, symmetric ordering. Never run the CPU/GPU comparisons in parallel.
// Keep every failed run: a later pass is not permission to discard a long stall.
const root = fileURLToPath(new URL('../../', import.meta.url));
const rounds = Number(process.env.ADONIS_SERIES_ROUNDS || 1);
const prefix = process.env.ADONIS_SERIES_LABEL || `adonis-series-${new Date().toISOString().replace(/\D/g, '')}`;
assert.ok(Number.isInteger(rounds) && rounds >= 1 && rounds <= 4, 'rounds must be 1..4');
assert.match(prefix, /^[\w-]+$/);
const args = process.argv.slice(2);
assert.ok(args.every(arg => arg === '--relay' || arg === '--release'), 'only --relay/--release are supported');
const output = resolve(root, 'artifacts/multiplayer-tests');
mkdirSync(output, {recursive:true});
const order = [
  {name:'baseline', mode:0, delay:0},
  {name:'hybrid', mode:2, delay:2},
  {name:'delay', mode:1, delay:5},
  {name:'delay', mode:1, delay:5},
  {name:'hybrid', mode:2, delay:2},
  {name:'baseline', mode:0, delay:0},
];
const runs = [];
for (let round = 0; round < rounds; ++round) {
  for (let i = 0; i < order.length; ++i) {
    const policy = order[i], label = `${prefix}-${round + 1}-${i + 1}-${policy.name}`;
    assert.ok(!existsSync(resolve(output, `${label}-report.json`)) && !existsSync(resolve(output, `${label}-failure.json`)), `refusing to overwrite ${label}`);
    const child = spawnSync(process.execPath, [fileURLToPath(new URL('./transport-browser.mjs', import.meta.url)), ...args], {
      cwd:resolve(root, '..'), windowsHide:true, timeout:120000, maxBuffer:8 * 1024 * 1024,
      env:{...process.env, RUN_LABEL:label, ADONIS_MODE:String(policy.mode), INPUT_DELAY_FRAMES:String(policy.delay)},
      encoding:'utf8',
    });
    writeFileSync(resolve(output, `${label}.log`), `${child.stdout || ''}\n${child.stderr || ''}`);
    const reportPath = resolve(output, `${label}-report.json`);
    const failurePath = resolve(output, `${label}-failure.json`);
    const report = existsSync(reportPath) ? JSON.parse(readFileSync(reportPath, 'utf8')) : null;
    const failure = existsSync(failurePath) ? JSON.parse(readFileSync(failurePath, 'utf8')) : null;
    const result = report?.result;
    const run = {label,...policy,exitCode:child.status,error:child.error?.message || failure?.error || null,
      passed:child.status===0 && report?.passed===true,correctnessPassed:report?.correctnessPassed===true,
      wasm:report?.wasm || failure?.wasm || null,
      elapsedMs:result?.elapsedMs ?? null,
      setupMs:result?.setupMs ?? null,gameplayMs:result?.gameplayMs ?? null,
      gameplayCadencePassed:result?.gameplayCadencePassed ?? null,
      resimulated:result?.summary?.reduce((sum,peer)=>sum+peer[3],0) ?? null,
      snapshots:result?.adonis?.map(peer=>peer[2]) ?? null,
      callbackP95Ms:result?.measured?.slice(0,2).map(peer=>peer.callbackWorkMs.p95) ?? null,
      presentationMaxMs:result?.measured?.slice(0,2).map(peer=>peer.presentationGapMs.max) ?? null,
      comparableFinalHashes:result?.comparableFinalHashes ?? false,
      hashes:result?.hashes ?? null,
    };
    runs.push(run);
    console.log(JSON.stringify(run));
    writeFileSync(resolve(output, `${prefix}-summary.json`), JSON.stringify({
      scope:'Sequential local desktop Chromium, same fault-injection policy; delay changes applied input/workload. Not WAN/phone, phase-only attribution or input-to-photon proof.',
      route:args.includes('--relay')?'relay':'rtc',release:args.includes('--release'),rounds,
      complete:runs.length===rounds*order.length,passed:runs.every(run=>run.passed),runs,
    },null,2));
  }
}
assert.equal(new Set(runs.map(run=>run.wasm).filter(Boolean)).size, 1, 'series mixed WASM builds');
assert.ok(runs.every(run=>run.passed), 'one or more runs failed; all evidence retained');
