// Candidate diagnostics only. No replay rewriting or accepted-golden output.
import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import createCore from '../../site/runtime/core.mjs';
import {restoreStage,state} from '../../tools/replay-verifier/capture-candidate.mjs';
import {parseReplay,replayInputEnded} from '../../src/replay.mjs';
const root=path.resolve(import.meta.dirname,'../..');
const output=process.argv[2]??path.join(root,'artifacts/reverse/replay-missing-before.json');
const manifest=JSON.parse(fs.readFileSync(path.join(root,'tools/replay-verifier/corpus.json')));
// Pin one binary in memory: a concurrent root build cannot silently change
// the candidate between stage entries in this diagnostic.
const wasmBinary=fs.readFileSync(path.join(root,'site/runtime/core.wasm'));
const wasmSha256=crypto.createHash('sha256').update(wasmBinary).digest('hex');
async function initializeCandidate(number){
  const c=await createCore({wasmBinary});c._th12_select_stage(number);
  const load=(name,method,index=null)=>{const bytes=fs.readFileSync(path.join(root,'site/assets',name)),pointer=c._malloc(bytes.length);try{c.HEAPU8.set(bytes,pointer);if(!(index===null?method(pointer,bytes.length):method(index,pointer,bytes.length)))throw Error('Invalid retail input '+name);}finally{c._free(pointer);}};
  const suffix=String(number).padStart(2,'0');
  for(const name of ['default.ecl',`stage${suffix}.ecl`,...(number===7?['stage07boss.ecl']:[])])load(name,c._th12_load_ecl);
  if(c._th12_load_std)load(`stage${suffix}.std`,c._th12_load_std);
  for(const [bank,name]of [[0,'bullet'],[1,'enemy'],[2,`stgenm${suffix}`],[4,'pl00'],[5,'pl01'],[6,'pl02'],[7,'front'],[8,'text'],[11,'ascii'],[9,`st${suffix}logo`],[10,`stage${suffix}`]])load(name+'.anm',c._th12_load_anm,bank);
  if(fs.existsSync(path.join(root,`site/assets/stgenm${suffix}m.anm`)))load(`stgenm${suffix}m.anm`,c._th12_load_anm,3);
  for(let index=0;index<6;index++){load(`pl0${index>>1}${index%2?'b':'a'}.sht`,c._th12_load_sht,index);load(`st${suffix}_0${index>>1}${index%2?'b':'a'}.msg`,c._th12_load_msg,index);}
  return c;
}
const rows=[];
for(const fixture of manifest.fixtures){
  const raw=fs.readFileSync(path.join(root,'site/replays',fixture.id));
  if(crypto.createHash('sha256').update(raw).digest('hex')!==fixture.sha256)throw Error('Corpus identity mismatch: '+fixture.id);
  const replay=parseReplay(raw);
  for(const stage of replay.stages){
    const c=await initializeCandidate(stage.number);restoreStage(c,replay,stage);
    const first=new Map(),previous=new Uint32Array(1024);
    let processed=0,ended=0;
    const capture=(inputFrame)=>{
      const missing=new Uint32Array(c.HEAPU8.buffer,c._th12_unsupported(),1024);
      const changes=[];
      for(let op=0;op<1024;op++)if(missing[op]>previous[op]&&!first.has(op))changes.push(op);
      if(changes.length){const snapshot=state(c);for(const opcode of changes)first.set(opcode,{opcode,inputFrame,logicFrame:snapshot.frame,count:missing[opcode],runtimeFaults:snapshot.runtimeFaults,ecl:snapshot.ecl.map(e=>({enemy:e.enemy,routine:e.routine,byteOffset:e.byteOffset,time:e.time})),enemies:snapshot.enemies.map(e=>({id:e.id,position:e.position}))});}
      previous.set(missing);
    };
    capture(-1);
    for(;processed<stage.frames;processed++){
      const input=replay.input(stage.number,processed);if(replayInputEnded(input))break;
      c._th12_tick(input.held,input.pressed);capture(processed);
      const h=new Float32Array(c.HEAPU8.buffer,c._th12_hud(),c._th12_hud_size());
      ended=h[13];if(ended){processed++;break;}
    }
    const unimplemented=Object.fromEntries([...previous].map((n,i)=>[i,n]).filter(([,n])=>n));
    const row={id:fixture.id,stage:stage.number,processed,ended,inputFrames:stage.frames,first:[...first.values()],unimplemented};rows.push(row);
    console.log(JSON.stringify({...row,first:row.first.map(({opcode,inputFrame})=>({opcode,inputFrame}))}));
    fs.writeFileSync(output,JSON.stringify({schema:'th12-replay-missing-diagnostic/1',wholeGameOracle:false,wasmSha256,fixtureCount:manifest.fixtures.length,limitations:['candidate-only replay execution; a reached end or clean unsupported count is not native Replay acceptance'],rows},null,2)+'\n');
  }
}
