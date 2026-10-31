import test,{before,after} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {mkdtempSync,rmSync,readFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve,join} from 'node:path';
import {recordedReplayFps} from '../src/replay.mjs';

let directory,program;
before(()=>{
  directory=mkdtempSync(join(tmpdir(),'th12-sdl-replay-'));
  program=join(directory,process.platform==='win32'?'replay.exe':'replay');
  const source=resolve(import.meta.dirname,'../portable/sdl');
  execFileSync(process.env.CXX||'g++',['-std=c++17','-O2','-D_GLIBCXX_ASSERTIONS','-I',source,
    resolve(import.meta.dirname,'native/sdl-replay.cpp'),
    ...['Replay.cpp','ReplayExtension.cpp','Archive.cpp'].map(name=>join(source,name)),'-o',program],
    {encoding:'utf8',windowsHide:true,timeout:60000});
});
after(()=>{if(directory)rmSync(directory,{recursive:true,force:true});});
const run=mode=>execFileSync(program,[mode],{encoding:'utf8',windowsHide:true,timeout:30000});

test('SDL replay rejects every incomplete first-stage header and a truncated second header',()=>{
  assert.match(run('bounds'),/valid recording round-trip passed/);
});

test('SDL recorded FPS agrees with original playback cursor reads and the existing Web lookup',()=>{
  const actual=run('fps').trim().split(/\s+/).map(Number);
  const samples=Uint8Array.of(60,59,58,57),stage={frames:91,fpsOffset:0,fpsCount:4};
  assert.deepEqual(actual,Array.from({length:92},(_,cursor)=>recordedReplayFps(samples,stage,cursor)));
  const original=JSON.parse(readFileSync(new URL('fixtures/replay-input-order-original.json',import.meta.url)));
  for(const read of original.records[0].steps)assert.equal(actual[Math.max(0,read.stageCursor)],read.fps,read.step);
});
