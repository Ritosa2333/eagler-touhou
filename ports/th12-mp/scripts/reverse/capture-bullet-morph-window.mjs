// Read-only candidate window; no original replay or accepted golden changes.
import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import {parseReplay} from '../../src/replay.mjs';
import {initializeCandidate,restoreStage,state} from '../../tools/replay-verifier/capture-candidate.mjs';
const root=path.resolve(import.meta.dirname,'../..');
const binary=fs.readFileSync(path.join(root,'site/runtime/core.wasm'));
const replay=parseReplay(fs.readFileSync(path.join(root,'site/replays/th12_17.rpy')));
const core=await initializeCandidate(2,{wasmBinary:binary});restoreStage(core,replay,replay.stages.find(s=>s.number===2));
const rows=[];
for(let tick=0;tick<=4446;tick++){
 const input=replay.input(2,tick);core._th12_tick(input.held,input.pressed);
 if(tick<4433)continue;
 const s=state(core),bullets=s.bullets.filter(b=>!b.friendly&&[1316,1322].includes(b.physicalSlot));
 const keys=new Set(bullets.map(b=>0x500000000+b.id));
 rows.push({inputTick:tick,bullets,animations:s.animations.nodes.filter(n=>keys.has(n.key))});
}
const output=path.join(root,'artifacts/reverse/candidate-bullet-morph-window.json');
fs.writeFileSync(output,JSON.stringify({wholeGameOracle:false,wasmSha256:crypto.createHash('sha256').update(binary).digest('hex'),rows},null,2)+'\n');
console.log(output);
