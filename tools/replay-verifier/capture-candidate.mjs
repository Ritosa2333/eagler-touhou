// Candidate-only diagnostics. This never generates or accepts original goldens.
import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
import createCore from '../../site/runtime/core.mjs';import {parseReplay} from '../../src/replay.mjs';
const root=path.resolve(import.meta.dirname,'../..'),assets=path.join(root,'site/assets');
export function state(core){const ptr=core._th12_state(),size=core._th12_state_size();return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(ptr,ptr+size)));}
export async function initializeCandidate(number=1){const c=await createCore();c._th12_select_stage(number);
  const load=(file,method,index=null)=>{const b=fs.readFileSync(path.join(assets,file)),p=c._malloc(b.length);try{c.HEAPU8.set(b,p);if(!(index===null?method(p,b.length):method(index,p,b.length)))throw Error('Invalid retail input '+file);}finally{c._free(p);}};
  for(const file of ['default.ecl',`stage${String(number).padStart(2,'0')}.ecl`,...(number===7?['stage07boss.ecl']:[])])load(file,c._th12_load_ecl);
  if(c._th12_load_std)load(`stage${String(number).padStart(2,'0')}.std`,c._th12_load_std);
  for(const [bank,file]of [[0,'bullet'],[1,'enemy'],[2,`stgenm${String(number).padStart(2,'0')}`],[4,'pl00'],[5,'pl01'],[6,'pl02'],[7,'front'],[8,'text'],[11,'ascii'],[9,`st${String(number).padStart(2,'0')}logo`],[10,`stage${String(number).padStart(2,'0')}`]])load(file+'.anm',c._th12_load_anm,bank);
  if(fs.existsSync(path.join(assets,`stgenm${String(number).padStart(2,'0')}m.anm`)))load(`stgenm${String(number).padStart(2,'0')}m.anm`,c._th12_load_anm,3);
  for(let i=0;i<6;i++){load(`pl0${i>>1}${i%2?'b':'a'}.sht`,c._th12_load_sht,i);load(`st${String(number).padStart(2,'0')}_0${i>>1}${i%2?'b':'a'}.msg`,c._th12_load_msg,i);}return c;
}
export function restoreStage(c,replay,stage){c._th12_start(replay.character,replay.shot,replay.difficulty,stage.seed);const i=stage.initial;c._th12_set_initial(i.x,i.y,i.power,i.lives,i.bombs,i.score);c._th12_set_replay_economy(i.pointValue,i.lifeFragments,i.bombFragments,...i.ufoColors,i.rank);}
if(process.argv[1]===import.meta.filename){
  const selected=process.argv.slice(2),index=JSON.parse(fs.readFileSync(path.join(root,'tools/replay-verifier/corpus.json'))),rows=[];
  for(const fixture of index.fixtures){if(selected.length&&!selected.includes(fixture.id))continue;const replay=parseReplay(fs.readFileSync(path.join(root,'site/replays',fixture.id)));
    for(const stage of replay.stages){const c=await initializeCandidate(stage.number);restoreStage(c,replay,stage);const samples=[],misses=[];let processed=0,invalid=null;
      for(;processed<stage.frames;processed++){const input=replay.input(stage.number,processed);if(input.held===65535)break;c._th12_tick(input.held,input.pressed);const h=new Float32Array(c.HEAPU8.buffer,c._th12_hud(),c._th12_hud_size());if(![...h].every(Number.isFinite)){invalid=processed;break;}if(h[14]&32)misses.push(processed+1);if(processed%60===0){const s=state(c);samples.push({inputFrame:processed,logicFrame:s.frame,sha256:crypto.createHash('sha256').update(JSON.stringify(s)).digest('hex'),enemyCount:s.enemies.length,bullets:s.bullets.length,items:s.items.length,ufo:s.economy.ufo,rank:s.economy.resources[10]});}if(h[13]){processed++;break;}}
      const h=[...new Float32Array(c.HEAPU8.buffer,c._th12_hud(),c._th12_hud_size())],missing=new Uint32Array(c.HEAPU8.buffer,c._th12_unsupported(),1024);
      const row={id:fixture.id,stage:stage.number,inputFrames:stage.frames,processed,ended:h[13],misses,invalid,unimplemented:Object.fromEntries([...missing].map((n,i)=>[i,n]).filter(([,n])=>n)),finalHud:h,samples};rows.push(row);console.log(JSON.stringify({...row,samples:undefined,finalHud:undefined}));
    }
  }
  const report={schema:'th12-candidate-diagnostics/2',wholeGameOracle:false,wasmSha256:crypto.createHash('sha256').update(fs.readFileSync(path.join(root,'site/runtime/core.wasm'))).digest('hex'),rows};const output=path.join(root,'artifacts/candidate-latest.json');fs.mkdirSync(path.dirname(output),{recursive:true});fs.writeFileSync(output,JSON.stringify(report,null,2)+'\n');
}
