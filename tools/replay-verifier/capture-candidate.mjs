// Diagnostic candidate only. No original capture, golden acceptance or display
// work runs here. Every consumed input has exactly one complete logic sample.
import fs from 'node:fs';
import path from 'node:path';
import {pathToFileURL} from 'node:url';
import {startReplayStage} from '../../src/replay-session.mjs';
import {StageLoader} from '../../src/stage-loader.mjs';
import {PracticeBridge} from '../../src/practice/bridge.mjs';
import {DamageSourceBridge} from '../../src/practice/damage-sources.mjs';
import {argumentsFor,verifierPaths,prerequisite,jsonFile,sha256} from './config.mjs';
import {finiteTree,writeRecords,difference,records,validateGameplayFile} from './trace-stream.mjs';
import {loadCorpus,loadFixture,resourceIdentity} from './manifest.mjs';
function readJSON(core,method,sizeMethod){
  const ptr=core[method](),size=core[sizeMethod](),heap=core.HEAPU8;
  if(!Number.isSafeInteger(ptr)||!Number.isSafeInteger(size)||ptr<0||size<2||ptr+size>heap.byteLength)throw Error('Invalid core snapshot bounds');
  const value=JSON.parse(new TextDecoder().decode(heap.subarray(ptr,ptr+size)));finiteTree(value);return value;
}
// Generic semantic-fixture reader. The gameplay collector additionally
// requires the dedicated diagnostic ABI and build identity below.
export function state(core){return readJSON(core,'_th12_state','_th12_state_size');}
export function requireDiagnostic(core){if(core._th12_diagnostic_build?.()!==1||typeof core._th12_diagnostic_state!=='function')throw Error('Missing prerequisite: build TH12_DIAGNOSTIC=1 to a separate diagnostic Runtime');}
export async function loadDiagnostic(paths){
  const file=prerequisite(paths.runtime,'diagnostic core module'),build=jsonFile(prerequisite(path.join(path.dirname(file),'build.json'),'diagnostic build identity'));
  if(build.diagnostic!==true||build.diagnosticApi!==1)throw Error('Refusing a player/old Runtime as the diagnostic provider');
  const wasmFile=prerequisite(file.replace(/\.mjs$/,'.wasm'),'diagnostic Wasm');
  for(const artifact of [file,wasmFile]){const bytes=fs.readFileSync(artifact),pin=build.artifacts?.[path.basename(artifact)];if(!pin||pin.bytes!==bytes.length||pin.sha256!==sha256(bytes))throw Error('Diagnostic build artifact identity mismatch: '+artifact);}
  const module=await import(pathToFileURL(file).href),core=await module.default();requireDiagnostic(core);
  return {core,build,wasmSha256:sha256(fs.readFileSync(wasmFile))};
}
export function assetLoader(core,paths,pins){
  const atlases={};for(let number=1;number<=7;number++){const name=`stgenm${String(number).padStart(2,'0')}m`;if(fs.existsSync(path.join(paths.assets,name+'.anm')))atlases[name]=true;}
  return new StageLoader(core,atlases,file=>{const bytes=fs.readFileSync(path.join(paths.assets,file));if(pins){const pin=pins.find(p=>p.file===file);if(!pin||pin.bytes!==bytes.length||pin.sha256!==sha256(bytes))throw Error('Loaded resource differs from accepted identity: '+file);}return bytes;});
}
export async function initializeCandidate(number=1,{wasmBinary,paths=verifierPaths()}={}){
  let core;if(wasmBinary){const module=await import(pathToFileURL(prerequisite(paths.runtime,'diagnostic core')).href);core=await module.default({wasmBinary});requireDiagnostic(core);}
  else({core}=await loadDiagnostic(paths));
  await assetLoader(core,paths).load(number);return core;
}
export function restoreStage(core,replay,stage){return startReplayStage(core,replay,stage);}
const direct=['player','rng','ecl','enemies','bullets','items','economy'];
function tickRecord(snapshot,flow,input,motion,stageNumber,inputFrame,frame){
  if(snapshot.fixtureFlags!==0)throw Error('Partial-world fixture flags cannot participate in a gameplay trace');
  const {input:runtimeInput,stage,frame:logicFrame,...other}=snapshot;
  const row={record:'tick',frame,stageNumber,inputFrame,logicFrame,input:{recorded:input,motion:motion??null,runtime:runtimeInput}};
  for(const key of direct){if(!Object.hasOwn(other,key))throw Error('Missing core state category '+key);row[key]=other[key];delete other[key];}
  row.stage={state:stage,flow,other};finiteTree(row);return row;
}
export async function* candidateRecords(loaded,paths,commonIdentity){
  const {fixture,replay,identity:selection,completion}=loaded,{core,build,wasmSha256}=await loadDiagnostic(paths),loader=assetLoader(core,paths,loaded.resources);
  if(replay.extension)throw Error('Retail verifier rejects extended/assisted replay input');
  const practice=new PracticeBridge(core),damage=new DamageSourceBridge(core);
  yield {record:'header',schema:'th12-gameplay-trace/2',identity:{replaySha256:fixture.sha256,...commonIdentity},fixture:selection,
    provider:{kind:'candidate-diagnostic',diagnosticApi:1,wasmSha256,sourceSha256:build.sourceSha256,sampling:'after-every-complete-authoritative-tick-before-external-stage-load'}};
  let ticks=0;const stages=[];
  for(let index=0;index<replay.stages.length;index++){
    const stage=replay.stages[index],route=selection.route[index];await loader.load(stage.number);
    if(index===0){practice.reset();damage.begin({replay:true,practice:replay.practice,character:replay.character,shot:replay.shot});}
    // Selecting resources clears the practice playback flag. Restore it after
    // StageLoader, exactly as initializeReplayStage does in the player shell.
    practice.setReplay(true);
    startReplayStage(core,replay,stage);core._th12_set_full_game_mode(replay.practice?1:0);
    const damageState=damage.state();if(damageState.error&&damageState.entryResult===-1)throw Error(damageState.error);
    for(let cursor=0;cursor<route.inputTicks;cursor++){
      const input=replay.input(stage.number,cursor),motion=replay.motion(stage.number,cursor);
      // Browser Replay transport: no extra zero tick, shifted input0, late
      // resource restore, wall time, renderer, audio or display callback.
      core._th12_motion?.(motion?4:0,motion?.[0]??0,motion?.[1]??0);
      core._th12_tick(input.held,input.pressed);
      const snapshot=readJSON(core,'_th12_diagnostic_state','_th12_diagnostic_state_size');
      const flow=readJSON(core,'_th12_stage_flow','_th12_stage_flow_size');
      if((snapshot.stage?.unsupported?.length??0)||(snapshot.referenceFaults?.length??0)||(snapshot.runtimeFaults?.length??0))throw Error(`Candidate has an unsupported/faulted state at stage${stage.number} input${cursor}`);
      yield tickRecord(snapshot,flow,input,motion,stage.number,cursor,ticks++);
      // Scheduling labels such as next-stage/result can precede the retail
      // recorded tail. Keep sampling every authored input. The golden compares
      // those labels on every tick; reviewed completion owns the final state.
    }
    const flow=readJSON(core,'_th12_stage_flow','_th12_stage_flow_size'),ended=new Float32Array(core.HEAPU8.buffer,core._th12_hud(),core._th12_hud_size())[13];
    stages.push({number:stage.number,inputTicks:route.inputTicks,ended,flowStatus:flow.status});
    if(index+1<replay.stages.length&&core._th12_practice_damage_transition?.(stage.number,replay.stages[index+1].number)!==1)throw Error('Replay stage-transition transport rejected');
  }
  const footer={record:'completion',complete:true,ticks,stages,outcome:completion.outcome};
  // The outcome label is declared, not inferred from candidate survival. Actual
  // input count and every terminal state must also match original evidence.
  yield footer;
}
export async function captureCandidate(loaded,paths,identity,file){
  await writeRecords(file,candidateRecords(loaded,paths,identity));
  await validateGameplayFile(file,{identity:{replaySha256:loaded.fixture.sha256,...identity},fixture:loaded.identity,completion:loaded.completion});
  return file;
}
export async function captureAndCompare(loaded,paths,identity,file,golden){
  const expected=records(golden)[Symbol.asyncIterator]();let compared=0;
  async function* comparedRecords(){
    for await(const candidate of candidateRecords(loaded,paths,identity)){
      const next=await expected.next();if(next.done)throw Error('Candidate produced extra records');
      const left=candidate.record==='header'?{schema:next.value.schema,identity:next.value.identity,fixture:next.value.fixture}:next.value;
      const right=candidate.record==='header'?{schema:candidate.schema,identity:candidate.identity,fixture:candidate.fixture}:candidate;
      const found=difference(left,right);yield candidate;
      if(found){const error=Error('Earliest authoritative divergence');error.divergence={status:'diverged',frame:candidate.frame??compared,...found};throw error;}
      if(candidate.record==='tick')compared++;
    }
    if(!(await expected.next()).done)throw Error('Candidate cropped the expected trace');
  }
  try{
    await writeRecords(file,comparedRecords());
    await validateGameplayFile(file,{identity:{replaySha256:loaded.fixture.sha256,...identity},fixture:loaded.identity,completion:loaded.completion});
    return {status:'pass',ticks:compared};
  }catch(error){if(error.divergence)return error.divergence;throw error;}
  finally{await expected.return?.();}
}
if(process.argv[1]&&path.resolve(process.argv[1])===path.resolve(import.meta.filename)){
  try{const {values,positional}=argumentsFor(process.argv.slice(2)),paths=verifierPaths(values),corpus=loadCorpus(paths);
    if(!positional.length)throw Error('Select explicit corpus fixture ids for candidate capture');
    const fixtures=positional.map(id=>{const fixture=corpus.fixtures.find(f=>f.id===id);if(!fixture)throw Error('Unknown corpus fixture '+id);return fixture;});
    const identity=resourceIdentity(corpus,paths,fixtures);fs.mkdirSync(paths.output,{recursive:true});
    for(const fixture of fixtures){const loaded=loadFixture(corpus,fixture,paths),file=path.join(paths.output,fixture.sha256+'-'+Date.now()+'.candidate.jsonl');await captureCandidate(loaded,paths,identity,file);console.log(JSON.stringify({fixture:fixture.id,file,wholeGameOracle:false}));}
  }catch(error){console.error(JSON.stringify({status:'failed',error:error.message,wholeGameDeterminism:false}));process.exitCode=2;}
}
