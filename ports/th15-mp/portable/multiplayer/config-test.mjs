import assert from 'node:assert/strict';
import {mkdtemp,copyFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join,resolve} from 'node:path';
import {pathToFileURL} from 'node:url';

// Exercise the exact source bridge, with its packaged common dependency, before
// a game build is available. The compiled module/export check is module-test.
const workspace=resolve(import.meta.dirname,'../../../..');
const temporary=await mkdtemp(join(tmpdir(),'th15-mp-config-'));
try{
 await copyFile(resolve(import.meta.dirname,'../../web/sdl-runtime/multiplayer.mjs'),join(temporary,'multiplayer.mjs'));
 await copyFile(resolve(workspace,'common/browser/adonis-calibration.mjs'),join(temporary,'adonis-calibration.mjs'));
 const {validateMultiplayerOptions,multiplayerSetupWords,multiplayerBuildWords}=await import(pathToFileURL(join(temporary,'multiplayer.mjs')));
 const manifest={game:'th15',product:'th15mp',variant:'multiplayer',features:{multiplayer:true},execution:{sha256:'12345678'.repeat(8)}};
 const build=multiplayerBuildWords(manifest);
 const base={netplayMode:'lan',netplayUrl:'ws://localhost/?room=th15mp-rollback&run=1',netplayPlayer:0,netplayPlayerCount:3,
  netplayDifficulty:1,netplaySeed:417,netplayLoadouts:[0,1,3].map(character=>({character,shot:0})),netplayPredictionReserve:2};
 for(const mode of [0,1,2])for(const seat of [0,1,2]){
  const options={...base,netplayPlayer:seat,netplayAdonisMode:mode,netplayInputDelay:mode?2:0,netplayPredictionLimit:mode===1?0:12};
  const {config,words}=await multiplayerSetupWords(options,build);
  assert.equal(config.mode,mode);assert.equal(words.length,23);assert.equal(words[0],7);assert.equal(words[2],seat);assert.equal(words[14],mode);
  assert.equal(words[7],mode?2:0);assert.equal(words[22],1500000);
 }
 const auto={...base,netplayAdonisMode:2,netplayPredictionLimit:12,netplayInputDelayAuto:true,netplayInputDelay:0};
 assert.equal((await multiplayerSetupWords(auto,build)).words[15],1);
 for(const patch of [{netplayAdonisMode:3},{netplayPredictionLimit:0},{netplayPredictionLimit:13},{netplayInputDelay:1},
  {netplaySeed:65536},{netplayPredictionReserve:3},{netplayPlayer:3},{thpracEnabled:true}]){
  assert.throws(()=>validateMultiplayerOptions({...auto,...patch}),JSON.stringify(patch));
 }
 for(const patch of [{netplayInputDelay:1},{netplayInputDelayAuto:true}])assert.throws(()=>validateMultiplayerOptions({...base,netplayAdonisMode:0,netplayPredictionLimit:12,...patch}));
 assert.throws(()=>validateMultiplayerOptions({...base,netplayAdonisMode:1,netplayPredictionLimit:12}));
 assert.deepEqual(validateMultiplayerOptions({replayViewer:true}),{replay:true});
 assert.throws(()=>multiplayerBuildWords({...manifest,product:'th15'}));
 console.log('PASS TH15 v7 direct/delay/hybrid setup, 3-seat identity, prediction budget and invalid timing rejection');
}finally{await rm(temporary,{recursive:true,force:true});}
