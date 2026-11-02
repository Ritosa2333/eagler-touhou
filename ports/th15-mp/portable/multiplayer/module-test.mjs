import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
const root=resolve(import.meta.dirname,'../..');
const {multiplayerBuildWords,validateMultiplayerOptions,multiplayerSetupWords}=await import('../../build-eagler-multiplayer/multiplayer.mjs');
const manifest=JSON.parse(readFileSync(resolve(root,'build-eagler-multiplayer/manifest.json')));
const build=multiplayerBuildWords(manifest);assert.equal(build.length,4);
const options={netplayMode:'lan',netplayUrl:'ws://localhost/?room=th15mp-test&run=1',netplayPlayer:0,netplayPlayerCount:3,netplayDifficulty:1,netplaySeed:417,netplayLoadouts:[0,1,3].map(character=>({character,shot:0})),netplayInputDelay:2,netplayAdonisMode:1,netplayPredictionLimit:0,netplayPredictionReserve:2};
const left=await multiplayerSetupWords(options,build),right=await multiplayerSetupWords({...options,netplayPlayer:2},build);
assert.equal(left.words.length,23);assert.deepEqual(left.words.slice(3),right.words.slice(3));assert.equal(right.words[2],2);
for(const patch of [{thpracEnabled:true},{netplayPlayerCount:1},{netplayPlayer:3},{netplayAdonisMode:3},{netplayPredictionLimit:1},{netplayInputDelay:10},{netplayInputDelayAuto:true},{netplaySeed:65536},{netplayDifficulty:5},{netplayLoadouts:[0,1,4].map(character=>({character,shot:0}))},{netplayLoadouts:[0,1,3].map(character=>({character,shot:1}))}])assert.throws(()=>validateMultiplayerOptions({...options,...patch}),JSON.stringify(patch));
for(const mode of [0,2]){
 const configured=await multiplayerSetupWords({...options,netplayAdonisMode:mode,netplayPredictionLimit:12,netplayInputDelayAuto:mode===0},build);
 assert.equal(configured.words[14],mode);assert.equal(configured.words.length,23);
 if(mode===0){assert.equal(configured.words[7],0);assert.equal(configured.words[15],0);}
}
assert.throws(()=>multiplayerBuildWords({...manifest,product:'th15'}));
assert.deepEqual(validateMultiplayerOptions({replayViewer:true}),{replay:true});
assert.throws(()=>validateMultiplayerOptions({...options,replayViewer:true}));
const exports=WebAssembly.Module.exports(new WebAssembly.Module(readFileSync(resolve(root,'build-eagler-multiplayer/th15.wasm')))).map(e=>e.name);
assert.ok(exports.includes('th15_mp_start'));assert.ok(exports.includes('th15_mp_replay_load'));
assert.ok(!exports.some(n=>/^th15_probe|^th15_check_|^application_/.test(n)));
const ordinaryRoot=process.env.TH15_TEST_ORDINARY_RUNTIME||resolve(root,'build-eagler');
const ordinary=WebAssembly.Module.exports(new WebAssembly.Module(readFileSync(resolve(ordinaryRoot,'th15.wasm')))).map(e=>e.name);
assert.ok(!ordinary.some(n=>n.startsWith('th15_mp_')),'Ordinary THPrac build exposed multiplayer');
assert.notEqual(JSON.parse(readFileSync(resolve(ordinaryRoot,'manifest.json'))).features.multiplayer,true);
console.log('PASS TH15 multiplayer configuration, setup ABI, build identity and production exports');
