// Partial world diagnostics, never a whole-game Replay golden.
import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
import {parseReplay,replayInputEnded} from '../../src/replay.mjs';
import {initializeCandidate,restoreStage,state} from '../../tools/replay-verifier/capture-candidate.mjs';
const root=path.resolve(import.meta.dirname,'../..'),argv=process.argv.slice(2);
const option=(name,fallback)=>{const index=argv.indexOf(name);if(index<0)return fallback;if(!argv[index+1]||argv[index+1].startsWith('--'))throw Error('Missing '+name);return argv[index+1];};
const id=option('--replay','demo2.rpy'),number=Number(option('--stage','1')),requested=Number(option('--frames','600'));
if(path.basename(id)!==id||!Number.isSafeInteger(number)||number<1||number>7||!Number.isSafeInteger(requested)||requested<1||requested>30000)throw Error('Invalid diagnostic replay/stage/frame selection');
const corpus=JSON.parse(fs.readFileSync(path.join(root,'tools/replay-verifier/corpus.json'))),fixture=corpus.fixtures.find(f=>f.id===id);
if(!fixture)throw Error('Replay is not in the current valid corpus: '+id);
const replayBytes=fs.readFileSync(path.join(root,'site/replays',id)),digest=crypto.createHash('sha256').update(replayBytes).digest('hex');
if(digest!==fixture.sha256)throw Error('Replay differs from the current corpus: '+id);
const replay=parseReplay(replayBytes),stage=replay.stages.find(s=>s.number===number);
if(!stage)throw Error('Replay does not contain selected stage');
const inputOnly=argv.includes('--inputs-only'),wasmPath=path.join(root,'site/runtime/core.wasm'),wasmDigest=()=>crypto.createHash('sha256').update(fs.readFileSync(wasmPath)).digest('hex'),wasmSha256=inputOnly?null:wasmDigest();
const core=inputOnly?null:await initializeCandidate(number),active=argv.includes('--active');
if(core){restoreStage(core,replay,stage);core._th12_oracle_fixture(active?0:7);}
const compact=argv.includes('--compact');
const capture=()=>{
  const snapshot=state(core);if(!compact)return snapshot;
  const keys=(object,names)=>Object.fromEntries(names.filter(key=>key in object).map(key=>[key,object[key]]));
  return {...keys(snapshot,['frame','rng','player','items','economy','stage']),
    ecl:snapshot.ecl.map(e=>keys(e,['enemy','routine','byteOffset','time','calls'])),
    enemies:snapshot.enemies.map(e=>keys(e,['id','position','absolute','relative','life','maxLife','age','phaseAge','flags','lifeFlags','variables','boss','bossSlot','immunity','bodyImmunity','invincible','active','hitbox'])),
    bullets:snapshot.bullets.map(b=>keys(b,['id','physicalSlot','friendly','position','motion','type','color','age','active','weapon','hostile']))};
};
const inputs=[],ticks=[],initial=core?capture():null;
for(let frame=0;frame<Math.min(requested,stage.frames);frame++){
  const input=replay.input(number,frame);if(replayInputEnded(input))break;
  if(inputOnly){inputs.push({frame,...input});continue;}
  core._th12_tick(input.held,input.pressed);const snapshot=capture();
  inputs.push({frame,xFixed:snapshot.player.xFixed,yFixed:snapshot.player.yFixed,power:snapshot.economy.resources[0],rank:snapshot.economy.resources[10],...input});ticks.push(snapshot);
}
const folder=path.resolve(option('--output-dir',path.join(root,'artifacts/reverse'))),suffix=option('--prefix',id.replace(/\.rpy$/,''));
if(!/^[a-zA-Z0-9_-]+$/.test(suffix))throw Error('Invalid diagnostic output prefix');
if(!inputOnly&&wasmDigest()!==wasmSha256)throw Error('Candidate Wasm changed during capture; rerun after the build completes');
const metadata={wholeGameOracle:false,inputOnly,compactCapture:compact,replay:id,replaySha256:digest,wasmSha256,character:replay.character,shot:replay.shot,difficulty:replay.difficulty,stage:number,seed:stage.seed};
fs.mkdirSync(folder,{recursive:true});
const inputOutput=path.join(folder,(active?'world-input-':'enemy-input-')+suffix+'.json'),candidateOutput=path.join(folder,(active?'world-candidate-active-':'enemy-candidate-passive-')+suffix+'.json');
fs.writeFileSync(inputOutput,JSON.stringify({...metadata,scope:inputOnly?'Original replay header and held/pressed/released triples; no candidate instance executed. Requires native player.':active?'Original held/pressed inputs; partial active-world provider, no whole-game acceptance':'Passive enemy-world fixture; damage, miss and Bomb disabled on both sides',initial:stage.initial,inputs},null,2)+'\n');
if(!inputOnly)fs.writeFileSync(candidateOutput,JSON.stringify({...metadata,initial,ticks})+'\n');
console.log(JSON.stringify({frames:inputs.length,replay:id,stage:number,wasmSha256,inputOutput,candidateOutput:inputOnly?null:candidateOutput}));
