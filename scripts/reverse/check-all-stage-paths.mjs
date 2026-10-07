// Candidate-only reachability diagnostic. Collision suppression is explicit;
// these results cannot be used as original-game replay acceptance or goldens.
import fs from 'node:fs';
import path from 'node:path';
import {parseReplay} from '../../src/replay.mjs';
import {initializeCandidate,restoreStage,state} from '../../tools/replay-verifier/capture-candidate.mjs';
const root=path.resolve(import.meta.dirname,'../..'),corpus=JSON.parse(fs.readFileSync(path.join(root,'tools/replay-verifier/corpus.json'))),rows=[];
for(let number=1;number<=7;number++){
  const available=corpus.fixtures.flatMap(f=>f.stages.filter(s=>s.number===number).map(s=>({file:f.id,frames:s.frames}))).sort((a,b)=>b.frames-a.frames);
  if(!available.length){rows.push({stage:number,error:'No replay supplied'});continue;}
  const selected=available[0],replay=parseReplay(fs.readFileSync(path.join(root,'site/replays',selected.file))),stage=replay.stages.find(s=>s.number===number),c=await initializeCandidate(number);
  restoreStage(c,replay,stage);c._th12_oracle_fixture(2);
  let processed=0,invalid=null;const milestones=[];
  for(;processed<stage.frames;processed++){
    const input=replay.input(number,processed);if(input.held===65535)break;c._th12_tick(input.held,input.pressed);
    if(processed%120===0){const s=state(c);if(![s.player.xFixed,s.player.yFixed,...s.economy.resources,...s.enemies.flatMap(e=>e.position),...s.bullets.flatMap(b=>b.position)].every(Number.isFinite)){invalid=processed;break;}milestones.push({frame:s.frame,enemies:s.enemies.length,bullets:s.bullets.length,lasers:s.lasers.nodes.length,spell:s.spellState.cardId,dialogue:s.message.active});}
    if(new Float32Array(c.HEAPU8.buffer,c._th12_hud(),c._th12_hud_size())[13]){processed++;break;}
  }
  const hud=[...new Float32Array(c.HEAPU8.buffer,c._th12_hud(),c._th12_hud_size())],s=state(c),missing=new Uint32Array(c.HEAPU8.buffer,c._th12_unsupported(),1024);
  const row={stage:number,replay:selected.file,inputFrames:stage.frames,processed,ended:hud[13],invalid,unimplemented:Object.fromEntries([...missing].map((n,i)=>[i,n]).filter(([,n])=>n)),remainingRoutines:s.ecl.map(e=>e.routine),referenceFaults:s.referenceFaults,runtimeFaults:s.runtimeFaults,animationUnsupported:s.animations.unsupported,milestones};rows.push(row);console.log(JSON.stringify({...row,milestones:undefined}));
}
fs.writeFileSync(path.join(root,'artifacts/reverse/all-stage-paths.json'),JSON.stringify({schema:'th12-candidate-stage-paths/1',wholeGameOracle:false,collisionSuppressed:true,rows},null,2)+'\n');
