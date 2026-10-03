import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import assert from 'node:assert/strict';

const root=resolve(import.meta.dirname,'../..');
const expectedOrdinary='1f5a9557f62f33c1243caa72a8649cd8335944665ceb70da763b8f0fe79f67e1';
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const builds={};
for(const profile of ['sdl3','multiplayer','multiplayer-fixtures']){
 const path=resolve(root,'th10_web/artifacts',profile),build=JSON.parse(readFileSync(resolve(path,'build.json')));
 const wasm=readFileSync(resolve(path,'th10-sdl.wasm')),loader=readFileSync(resolve(path,'th10-sdl.mjs'));
 assert.equal(sha(wasm),build.sha256,profile+' stale WASM');
 assert.equal(sha(loader),build.loaderSha256,profile+' stale loader');
 for(const [source,hash] of Object.entries(build.sourceFiles))assert.equal(sha(readFileSync(resolve(root,source))),hash,profile+' changed source '+source);
 const names=new Set(WebAssembly.Module.exports(new WebAssembly.Module(wasm)).map(entry=>entry.name));
 for(const name of ['multiplayer_replay_validate','multiplayer_replay_status'])assert.equal(names.has(name),profile!=='sdl3',profile+' '+name);
 for(const name of ['mp_fixture_prepare','mp_fixture_save_replay','mp_fixture_replay_seek_batch','mp_fixture_replay_probe','mp_fixture_replay_controls','mp_fixture_audio_seek_output','mp_fixture_audio_seek_tick'])assert.equal(names.has(name),profile==='multiplayer-fixtures',profile+' '+name);
 if(profile!=='multiplayer-fixtures')assert.ok(![...names].some(name=>name.startsWith('mp_fixture_')));
 builds[profile]={wasm:build.sha256,sourceDigest:build.sourceDigest,bytes:wasm.length};
}
assert.equal(builds.sdl3.wasm,expectedOrdinary,'Ordinary single-player baseline changed; investigate, do not replace the baseline');
console.log(JSON.stringify({passed:true,scope:'TH10 Replay binary isolation and unchanged ordinary WASM',builds},null,2));
