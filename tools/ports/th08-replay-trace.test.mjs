import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile,mkdtemp,writeFile,rm} from 'node:fs/promises';
import {join,resolve} from 'node:path';
import {tmpdir} from 'node:os';
import {runWasiCpp} from '../testing/wasi-cpp.mjs';
import {readNativeTrace} from '../../common/testkit/presentation-lab/trace-state.mjs';
import {TH08_REPLAY_TRACE_LAYOUT} from '../../ports/th08/portable/presentation-lab/replay-capture.mjs';
import {recordsFromRows,recordsFromLongJsonLines} from '../../ports/th08/tools/replay-verifier/legacy-oracle-adapter.mjs';
import {compareTraces} from '../../common/testkit/replay-verifier/compare.mjs';
import * as common from '../../common/testkit/replay-verifier/adapter.mjs';

const root=resolve(import.meta.dirname,'../..');
async function compiledTrace(t,game,mutate=value=>value){
  const directory=await mkdtemp(join(tmpdir(),'th08-native-trace-'));t.after(()=>rm(directory,{recursive:true,force:true}));
  const source=await readFile(resolve(root,`ports/${game}/web/cpp/platform/BrowserExports.cpp`),'utf8');
  const start=source.indexOf('EX("trace") const u32* browser_trace('),end=source.indexOf('\nEX(',start+1);
  assert.ok(start>=0&&end>start,'Production trace export must be selected explicitly');
  await writeFile(join(directory,'production-trace.inc'),mutate(source.slice(start,end)));
  const result=await runWasiCpp({source:resolve(import.meta.dirname,'fixtures/th08-replay-trace.cpp'),
    includes:[directory,resolve(root,`ports/${game}/web/cpp/game`),resolve(root,'common/include')],captureStdout:true});
  assert.match(result.stdout.trim(),/^[a-f0-9]{544}$/);return Buffer.from(result.stdout.trim(),'hex');
}

for(const game of ['th08','th08-mp'])test(game+' compiled production trace reaches the real capture decoder and shared state categories',async t=>{
  const raw=await compiledTrace(t,game),buffer=new ArrayBuffer(288);new Uint8Array(buffer,16).set(raw);
  const before=Buffer.from(buffer),row=readNativeTrace(buffer,16,TH08_REPLAY_TRACE_LAYOUT);
  const expected={frame:101,stage:7,gameFrames:203,controlFrames:307,flags:12,rng:401,rngCalls:509,
    player:[-123.25,456.5],playerState:3,keys:0x81,numbers:Array.from({length:228},(_,index)=>(index*17+3)&255)};
  assert.deepEqual(row,expected);assert.deepEqual(Buffer.from(buffer),before,'Trace reads must not mutate native state');
  const records=[...recordsFromRows({replaySha256:'a'.repeat(64)},[row],'compiled-production-trace',common)];
  const tick=records.find(record=>record.type==='tick');
  assert.equal(tick.replaySampleIndex,101);assert.deepEqual(tick.clocks,{replayFrame:101,gameFrames:203});
  assert.equal(tick.appliedInput,0x81);assert.equal(tick.categories.rng,common.digestCanonicalJson([401,509]));
  assert.equal(tick.categories.player,common.digestCanonicalJson([[-123.25,456.5],3]));
  assert.deepEqual(tick.scalars,{stage:7,flags:12,rngSeed:401,rngCalls:509,playerState:3});
  const comparedBytes=[...Array.from({length:72},(_,index)=>index).filter(index=>![25,26,27,42,43].includes(index)),
    ...[0x64,0x68,0x74,0x80,0x84,0x88,0x98].flatMap(at=>[at,at+1,at+2,at+3])];
  assert.equal(tick.categories.world,common.digestCanonicalJson([12,comparedBytes.map(index=>expected.numbers[index])]));
  const rows=[row,{...row,frame:102,gameFrames:204,controlFrames:308}],fixture={replaySha256:'a'.repeat(64)};
  const identity={executableSha256:'b'.repeat(64),resourceSha256:'c'.repeat(64)};
  for(const lane of ['quick','daily']){
    const trace=(values,complete)=>lane==='quick'?recordsFromRows(fixture,values,'native-state-fixture',{...common,complete}):
      recordsFromLongJsonLines({fixture,rows:values,provider:'native-state-fixture',identity,complete},common);
    const complete=await Array.fromAsync(trace(rows,true));
    assert.equal((await compareTraces(complete,trace(rows,true))).status,'PASS',lane);
    assert.equal((await compareTraces(complete,trace(rows.slice(0,1),false))).status,'INCOMPLETE',lane+' unfinished prefix');
    assert.equal((await compareTraces(complete,trace(rows.slice(0,1),true))).status,'DIVERGED',lane+' premature claimed end');
    assert.equal((await compareTraces(complete,complete.slice(0,-1))).status,'INCOMPLETE',lane+' missing end');
    const aborted=structuredClone(complete);aborted.at(-1).reason='aborted';
    const verdict=await compareTraces(complete,aborted);assert.equal(verdict.status,'DIVERGED');assert.equal(verdict.reason,'run-end');
  }
});

test('compiled mapping control detects substituting the input clock for the world clock',async t=>{
  await assert.rejects(compiledTrace(t,'th08',source=>{
    const altered=source.replace('out[2]=a.game.enemies.state.frames','out[2]=a.game.control.state.frames');
    assert.notEqual(altered,source);return altered;
  }),/compiled production fixture failed/);
});

test('native trace decoder rejects truncated buffers, unaligned pointers and invalid slot declarations',()=>{
  assert.throws(()=>readNativeTrace(new ArrayBuffer(268),0,TH08_REPLAY_TRACE_LAYOUT));
  assert.throws(()=>readNativeTrace(new ArrayBuffer(300),1,TH08_REPLAY_TRACE_LAYOUT),/aligned/);
  for(const layout of [{wordCount:1,uint32:{frame:1}},{wordCount:2,float32:{player:{at:1,length:2}}},
    {wordCount:2,bytes:{state:{at:7,length:2}}},{wordCount:2,uint32:{frame:0},float32:{frame:{at:0,length:1}}}])
    assert.throws(()=>readNativeTrace(new ArrayBuffer(16),0,layout),/Invalid native trace field/);
});
