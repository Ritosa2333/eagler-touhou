import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const root=fileURLToPath(new URL('../../',import.meta.url)),out=resolve(root,'../docs/multiplayer');
const read=path=>JSON.parse(readFileSync(resolve(root,path)));
const sha=path=>createHash('sha256').update(readFileSync(resolve(root,path))).digest('hex');
const development=read('artifacts/sdl3/build.json'),release=read('artifacts/sdl-release/build.json');
const names=['session','dynamic-state','dense-a-final','sparse-b-final','dense-a2-final','world-2-4','world-6-9','world-10-12','peer-browser','transport-rtc','transport-relay','transport-rtc-release'];
const reports=Object.fromEntries(names.map(name=>{const path=`artifacts/multiplayer-tests/${name}-report.json`,r=read(path);assert.equal(r.passed,true,name);if(r.wasm)assert.equal(r.wasm,name.endsWith('-release')?release.sha256:development.sha256,name);return [name,{path,sha256:sha(path),passed:r.passed,wasm:r.wasm||r.wasmSha256,scope:r.scope,result:r.result,dense:r.dense,characters:r.characters,errors:r.errors}];}));
const lifecyclePath='artifacts/sdl3/browser/netplay/report.json',lifecycle=read(lifecyclePath);assert.equal(lifecycle.passed,true);
for(const name of ['paused','synchronized','retry','localReplayAfterMatch','reconnected'])assert.ok(lifecycle.states.some(s=>s.name===name),name);
const [a,b,a2]=['dense-a-final','sparse-b-final','dense-a2-final'].map(n=>reports[n].result);
for(const r of [a,b,a2]){assert.equal(r.checkpoints,270);assert.equal(r.recordingBoundary,1);assert.equal(r.frames,1800);}
assert.deepEqual(a.status,b.status);assert.deepEqual(a2.status,b.status);
const cost=r=>r.captureMs+r.restoreMs,denseMean=(cost(a)+cost(a2))/2;
assert.ok(cost(b)<Math.min(cost(a),cost(a2)),'Sparse capture must beat both dense runs');
const work=r=>cost(r)+r.stepMs;
const comparison={countedWorkMs:[work(a),work(b),work(a2)],countedWorkReductionPercent:100*(1-work(b)/((work(a)+work(a2))/2)),denseCaptureRestoreMs:[cost(a),cost(a2)],sparseCaptureRestoreMs:cost(b),reductionPercent:100*(1-cost(b)/denseMean),denseInitialCounterBytes:a.bytes,sparseInitialCounterBytes:b.bytes,bytesReductionPercent:100*(1-b.bytes/a.bytes),scope:'270 checkpoint/restore operations and 1620 forward ticks per run; byte counter is journal and accounted owners at snapshot start, excluding attack clones and vector/function metadata; fixed 1800-frame workload; desktop Chromium SwiftShader; excludes verification hash/image encoding and second execution'};
const scripts=['world-browser','peer-browser','transport-browser','run-tests'].map(n=>`tests/multiplayer/${n}.mjs`);
const sourceIdentity={...development.sources,...Object.fromEntries([...scripts,'tests/browser/netplay-check.mjs','sdl-runtime/shared-netplay.mjs','sdl-runtime/netplay.mjs'].map(p=>[`th09_web/${p}`,sha(p)]))};
const evidence={date:'2026-09-27',base:'3b52630',common:'ac82fa2',developmentWasm:development.sha256,releaseWasm:release.sha256,releaseBytes:release.bytes,comparison,sourceIdentity,reports,lifecycle:{path:lifecyclePath,sha256:sha(lifecyclePath),...lifecycle},limits:['Desktop Chromium SwiftShader, not physical-phone acceptance','RTC and relay are local; injected faults do not establish public TURN behavior','Audio clock progress is not an underrun/listening-quality test','Dense versus sparse compares checkpoint implementation, not FPS against old lockstep','No deployment']};
writeFileSync(resolve(out,'rollback-evidence.json'),JSON.stringify(evidence,null,2)+'\n');
const f=n=>n.toFixed(2),rows=[['A: dense',a],['B: sparse',b],['A2: dense',a2]].map(([name,r])=>`| ${name} | ${f(r.captureMs)} | ${f(r.restoreMs)} | ${f(work(r))} | ${r.bytes.toLocaleString('en-US')} | 270 / 270 |`).join('\n');
const transports=['transport-rtc','transport-relay','transport-rtc-release'].map(n=>{const r=reports[n].result;return `| ${n} | ${r.summary.map(s=>s[0]).join(' / ')} | ${r.summary.map(s=>s[2]).join(' / ')} | ${r.summary.map(s=>s[3]).join(' / ')} | ${r.spectator} | ${f(r.elapsedMs/1000)} |`;}).join('\n');
writeFileSync(resolve(out,'rollback-evidence.md'),`# TH09 rollback — measured evidence

Release WASM: \`${release.sha256}\` (${release.bytes.toLocaleString('en-US')} bytes).
Development WASM: \`${development.sha256}\`.
[Machine-readable record](rollback-evidence.json) contains report fingerprints,
source identities and result details. No deployment was performed.

## Successful optimization

Fixed WASM and identical inputs, serial A/B/A; 1,800 logical frames per run.
Every restore was checked against the original execution's state inventory and
framebuffer. Each run also passed recording chunk and motion rewind checks.

| Run | Capture total ms | Restore total ms | Capture + forward + restore ms | Initial byte counter | State/image comparisons |
| --- | ---: | ---: | ---: | ---: | ---: |
${rows}

Capture plus restore time decreased **${f(comparison.reductionPercent)}%** against
the mean of the two dense runs. Including forward execution and its first-write
capture overhead, measured work decreased **${f(comparison.countedWorkReductionPercent)}%**.
These totals cover 270 saves/restores and 1,620 forward ticks, after a 180-tick
warmup. Verification hashing, image encoding and the second execution are excluded.
This is not total game FPS. The initial journal/owner byte counter decreased
**${f(comparison.bytesReductionPercent)}%**; it excludes subsequent first writes,
attack-state clones and container/callback metadata, so it is not total snapshot
size or allocated-memory use.

Local scheduling delay is **0 frames**, compared with the former 6-frame queue
(100 ms at 60 Hz). C++ tests check same-frame input use and once-only capture.
The browser test matches independent exact-input gameplay and Replay. This does
not claim zero device-to-display delay.

## Correctness and transport

- Five 600-frame C++ network conditions: steady, jitter, spike, all fast input
  lost, and loss/reordering/duplicates. All converge with once-only captures.
- ECL/effect ownership survives destruction and reuse; all nine attack types
  restore; production adapters compile without RTTI.
- Three additional 1,800-frame character pairs with round reset: (2,4), (6,9),
  (10,12). 810 further full-state/framebuffer comparisons pass.
- Two independent browser worlds plus exact-input reference: 900 frames,
  matching audit, image, hash and 1,312-byte Replay.
- Standalone UI: pause/resume, round reset, whole-world retry, completion, local
  Replay saving, reconnect and disconnect pass.
- Real local RTC and WebSocket fallback: 25–61 ms injected one-way delay and an
  800 ms spike. RTC also drops fast input. An admitted spectator consumes
  confirmed input. Audio contexts run and their clocks advance.

| Transport | Peer frames | Corrections | Resimulated frames | Spectator frame | Elapsed s |
| --- | --- | --- | --- | ---: | ---: |
${transports}

Development runs stop at exactly 600 frames and compare all three world hashes.
The release smoke has no test-only frame limiter: endpoints can stop on different
frames, so final hashes are compared only at equal frames. Native periodic
confirmed-hash checks remain active in every run. The transport workload has a
14-second upper bound for 600 ticks, including startup and the injected outage,
to catch recovery accidentally consuming normal simulation time. Historical
pre-fix timing remains in the ignored transport-before-scheduling report.

## Limits and reproduction

Desktop Chromium/SwiftShader results do not establish physical-phone or public
TURN acceptance. Audio clock progress does not establish absence of underruns.
An outage exceeding the eight-frame prediction limit still pauses simulation.

See [migration and commands](rollback-migration.md). After reproducing the named
reports, run \`node th09_web/tests/multiplayer/summarize-evidence.mjs\`. It fails
if a report fails, a WASM identity differs, or sparse capture fails to beat both
dense runs.
`);
console.log(JSON.stringify({development:development.sha256,release:release.sha256,comparison}));
