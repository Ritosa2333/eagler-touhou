// Configuration/negative-path regression only. No simulated successful compile:
// the probe compiler ALWAYS exits 42 and must never produce a PASS receipt.
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,mkdirSync,readFileSync,writeFileSync,existsSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve,dirname} from 'node:path';
import {spawnSync} from 'node:child_process';
const script=readFileSync(process.env.TH15_TEST_SCRIPT||resolve(import.meta.dirname,'visual-test.mjs'),'utf8');
function fixture(t){
 const temp=mkdtempSync(resolve(tmpdir(),'th15-sdk-config-'));t.after(()=>rmSync(temp,{recursive:true,force:true}));
 const root=resolve(temp,'ports/th15-mp'),entry=resolve(root,'portable/multiplayer/visual-test.mjs');
 const output=resolve(root,'web/artifacts/sdl-multiplayer');
 mkdirSync(dirname(entry),{recursive:true});mkdirSync(output,{recursive:true});
 writeFileSync(entry,script);writeFileSync(resolve(output,'build.json'),'{"sources":{}}');
 function sdk(name,layout='upstream',path=resolve(temp,name)){
  const compiler=resolve(path,layout,'emscripten/emcc.py');mkdirSync(dirname(compiler),{recursive:true});
  writeFileSync(compiler,`import os, sys\nprint('SDK_PROBE:${name}:' + os.environ.get('EM_CONFIG',''), flush=True)\nsys.exit(42)\n`);
  return path;
 }
 function run(extra={}){
  const env={...process.env};for(const key of ['TH15_EMSDK','EMSDK','TH15_OUTPUT','TH15_TEST_ARCHIVE'])delete env[key];
  const p=spawnSync(process.execPath,[entry,'--compile-only'],{env:{...env,...extra},encoding:'utf8',timeout:10000});
  assert.equal(p.error,undefined);assert.equal(p.status,1,'the intentional compiler failure must propagate');
  assert.equal(existsSync(resolve(output,'visual-test-compile-only.json')),false,'failed compiler must not create success receipt');
  const text=p.stdout+p.stderr;assert.doesNotMatch(text,/PASS: TH15/);return text;
 }
 return {root,sdk,run};
}
test('inherits setup-emsdk EMSDK when TH15_EMSDK is absent',t=>{
 const f=fixture(t),sdk=f.sdk('generic');const text=f.run({EMSDK:sdk});
 assert.match(text,/SDK_PROBE:generic:/);assert.ok(text.includes(resolve(sdk,'.emscripten')));
});
test('explicit TH15_EMSDK takes priority over inherited EMSDK',t=>{
 const f=fixture(t);const text=f.run({TH15_EMSDK:f.sdk('explicit'),EMSDK:f.sdk('generic')});
 assert.match(text,/SDK_PROBE:explicit:/);assert.doesNotMatch(text,/SDK_PROBE:generic:/);
});
test('invalid explicit SDK fails instead of silently using generic',t=>{
 const f=fixture(t);const text=f.run({TH15_EMSDK:resolve(f.root,'missing'),EMSDK:f.sdk('generic')});
 assert.match(text,/Set TH15_EMSDK/);assert.doesNotMatch(text,/SDK_PROBE:/);
});
test('local tools/emsdk remains the last fallback',t=>{
 const f=fixture(t);f.sdk('local','upstream',resolve(f.root,'tools/emsdk'));
 assert.match(f.run(),/SDK_PROBE:local:/);
});
test('generic install-layout SDK is accepted',t=>{
 const f=fixture(t);assert.match(f.run({EMSDK:f.sdk('install','install')}),/SDK_PROBE:install:/);
});
test('missing all SDK choices fails without a receipt',t=>{
 const f=fixture(t);assert.match(f.run(),/Set TH15_EMSDK/);
});
test('empty TH15_EMSDK retains generic fallback',t=>{
 const f=fixture(t);assert.match(f.run({TH15_EMSDK:'',EMSDK:f.sdk('empty-explicit')}),/SDK_PROBE:empty-explicit:/);
});
