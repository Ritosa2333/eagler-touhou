import assert from 'node:assert/strict';
import {readFileSync,appendFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import {root} from '../../th15_web/tests/cpp/helpers.mjs';
import {inspectReplay} from './inspect-replay.mjs';
import {capture,identity,row} from './observation.mjs';
import {createOriginalWorld} from './original-world.mjs';
const repo=fileURLToPath(new URL('../../',import.meta.url)),corpus=JSON.parse(readFileSync(new URL('./corpus.json',import.meta.url)));
const common=resolve(process.env.EAGLER_COMMON_ROOT||fileURLToPath(new URL('../../../eagler-common',import.meta.url)));
const {joinCompletedTraces}=await import(pathToFileURL(resolve(common,'testkit/replay-verifier/join-traces.mjs')));
const option=name=>{const i=process.argv.indexOf(name);return i<0?null:process.argv[i+1];};
const lane=option('--lane')||'all',ids=option('--case')?[option('--case')]:lane==='all'?Object.values(corpus.suites).flat():corpus.suites[lane];
const out=resolve(option('--output')||resolve(repo,'artifacts/replay-verifier/original'));
const selectedStage=option('--stage')?Number(option('--stage')):null;
if(!ids)throw Error('Unknown suite');mkdirSync(out,{recursive:true});
process.env.TH15_ORACLE_COUNT_INSTRUCTIONS='0';
for(const key of Object.keys(process.env))if(key.startsWith('TH15_WORLD_'))delete process.env[key];
for(const id of ids){
  const fixture=corpus.cases.find(f=>f.id===id);if(!fixture)throw Error('Unknown fixture');
  const path=resolve(repo,fixture.source),raw=readFileSync(path);assert.equal(createHash('sha256').update(raw).digest('hex'),fixture.replaySha256);
  const metadata=await inspectReplay(path),outputs=[];
  if(fixture.kind!=='demo'){assert.equal(metadata.clearStage,8);assert.equal(metadata.character,fixture.character);assert.equal(metadata.difficulty,fixture.difficulty);assert.deepEqual(metadata.stages.map(s=>s.stage),fixture.stages);}
  const stages=selectedStage===null?metadata.stages:metadata.stages.filter(s=>s.stage===selectedStage);assert.ok(stages.length,'Requested stage is present');
  for(const spec of stages){
    const name=fixture.kind==='demo'?id:id+'-stage'+spec.stage,output=resolve(out,name+'.original.jsonl'),rawPath=resolve(out,name+'.rows.jsonl');outputs.push(output);writeFileSync(rawPath,'');
    const stream=capture(output,identity(root,raw),'th15/retail-x86-world',{source:'existing-native-world-factory',viewport:[128,16,320,16],glyphOutput:'offline-raster-boundary',instructionBudgetHook:false});
    const w=await createOriginalWorld({...spec,character:metadata.character,difficulty:metadata.difficulty},path);let complete=false,finalState=null;
    try{for(let frame=0;frame<spec.frames;frame++){
      w.tick(frame);const words=w.state();appendFileSync(rawPath,JSON.stringify({frame,stage:spec.stage,words,flags:w.flags()})+'\n');stream.tick(row(spec.stage,frame,words));finalState=words;
      if(fixture.kind!=='demo'&&w.completed()){complete=true;break;}
      if(fixture.kind==='demo'&&frame+1===spec.frames)complete=true;
      if(frame%600===0)console.log(JSON.stringify({id,stage:spec.stage,frame,score:words[5]}));
    }
    stream.finish({complete,reason:fixture.kind==='demo'?'replay-fixture-consumed':complete?'stage-clear':'capture-incomplete',evidence:{spec,finalState,nativeFlags:w.flags()}});
    assert.ok(complete,'Original must consume the whole Demo or reach natural stage clear');
    console.log(JSON.stringify({id,stage:spec.stage,ticks:stream.ticks,complete}));
    }finally{w.close();}
  }
  if(fixture.kind!=='demo'&&selectedStage===null)await joinCompletedTraces(outputs,resolve(out,id+'.original.jsonl'));
}
