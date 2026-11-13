import {test} from 'node:test';
import assert from 'node:assert/strict';
import {installFormalAudit, persistAuditCapture} from './formal-audit.mjs';
import {mkdtemp, readFile, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';

function fixture() {
  installFormalAudit();
  let audit = {frames: [], overflow: 0, terminal: null};
  let timing = {clock: 'steady-microseconds', events: [], overflow: 0};
  let originalCalls = 0;
  const module = {UTF8ToString: value => value,
    _th13_mp_audit_take() { const out = audit; audit = {...audit, frames: []}; return JSON.stringify(out); },
    _th13_mp_timing_take() { const out = timing; timing = {...timing, events: []}; return JSON.stringify(out); },
    onGameFrame() { ++originalCalls; }};
  globalThis.__installFormalAudit(module);
  return {module, state: globalThis.__formalAudit,
    setAudit: value => {audit = value;}, setTiming: value => {timing = value;},
    originalCalls: () => originalCalls};
}
test('batches stay ordered until acknowledged and the production callback remains chained', () => {
  const f=fixture();
  f.setAudit({frames:[{frame:0}], overflow:0, terminal:null});
  f.setTiming({clock:'steady-microseconds',events:[{kind:'sample',frame:0}],overflow:0});
  f.module.onGameFrame(true,1,1);
  f.setAudit({frames:[{frame:1}],overflow:0,terminal:null});
  f.state.drain(); // loading work callback
  const first=f.state.snapshot();
  assert.deepEqual(first.batches.map(row=>row.sequence),[1,2]);
  assert.deepEqual(first.batches.flatMap(row=>row.audit.frames.map(frame=>frame.frame)),[0,1]);
  assert.throws(()=>f.state.ack(1,3,2)); // Wrong acknowledgement keeps data.
  assert.deepEqual(f.state.snapshot().batches,first.batches);
  f.state.ack(1,2,2);
  assert.deepEqual(f.state.snapshot().batches,[]);
  assert.equal(f.originalCalls(),1);
});
test('cumulative overflow and terminal are recorded without resetting or duplicate terminal', () => {
  const f=fixture();
  f.setAudit({frames:[],overflow:0,terminal:{frames:2}});
  f.setTiming({clock:'steady-microseconds',events:[{frame:1}],overflow:3});
  f.state.drain(); f.state.drain();
  const rows=f.state.snapshot().batches;
  assert.equal(rows.length,2); assert.equal(rows[0].timing.overflow,3);
  assert.equal(rows[1].timing.overflow,3);
  assert.deepEqual(rows[0].audit.terminal,{frames:2}); assert.equal(rows[1].audit.terminal,null);
  f.setAudit({frames:[],overflow:0,terminal:{frames:3}});
  f.state.drain(); assert.match(f.state.errors[0],/Terminal audit changed/);
});
test('invalid native data remains a fatal drain error', () => {
  const f=fixture();
  f.setTiming({overflow:0,events:null}); f.state.drain();
  assert.match(f.state.errors[0],/Malformed native audit/);
});
test('overflow and drain error are persisted before failure and remain unacknowledged', async t => {
  const dir=await mkdtemp(join(tmpdir(),'th13-audit-test-'));
  t.after(()=>rm(dir,{recursive:true,force:true}));
  const f=fixture(), cursor={next:1,rawPending:0,errorsWritten:false};
  f.setTiming({clock:'steady-microseconds',events:[{kind:'sample',frame:0}],overflow:3});
  f.state.drain();
  const capture=f.state.snapshot(), path=join(dir,'raw.ndjson');
  await assert.rejects(persistAuditCapture(path,capture,cursor),/Timing overflow/);
  const raw=await readFile(path,'utf8');
  assert.deepEqual(JSON.parse(raw),capture.batches[0]);
  assert.deepEqual(f.state.snapshot().batches,capture.batches);
  await assert.rejects(persistAuditCapture(path,f.state.snapshot(),cursor));
  assert.equal(await readFile(path,'utf8'),raw); // failure flush writes no duplicate
  f.state.errors.push('native parse error');
  await assert.rejects(persistAuditCapture(path,f.state.snapshot(),cursor),/Audit drain errors/);
  assert.match(await readFile(path,'utf8'),/native parse error/);
  assert.deepEqual(f.state.snapshot().batches,capture.batches);
});
