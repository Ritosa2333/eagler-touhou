import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import {core,memory,root} from '../../th11_web/tests/cpp/helpers.mjs';
import {identity,candidateRow} from './observation.mjs';
const repo=fileURLToPath(new URL('../../',import.meta.url)),corpus=JSON.parse(readFileSync(new URL('./corpus.json',import.meta.url)));
const common=resolve(process.env.EAGLER_COMMON_ROOT||fileURLToPath(new URL('../../../eagler-common',import.meta.url)));
const {TraceCapture}=await import(pathToFileURL(resolve(common,'testkit/replay-verifier/capture-file.mjs')));
const option=name=>{const i=process.argv.indexOf(name);return i<0?null:process.argv[i+1];};
const lane=option('--lane')||'all',ids=option('--case')?[option('--case')]:lane==='all'?Object.values(corpus.suites).flat():corpus.suites[lane];
const out=resolve(option('--output')||resolve(repo,'artifacts/replay-verifier/candidate'));
if(!ids)throw Error('Unknown suite');
for(const id of ids){
  const fixture=corpus.cases.find(f=>f.id===id);if(!fixture)throw Error('Unknown fixture');
  const raw=readFileSync(resolve(repo,fixture.source));if(fixture.replaySha256)assert.equal(createHash('sha256').update(raw).digest('hex'),fixture.replaySha256);
  const c=await core(),rp=c.allocate(raw.length),r=c.replay_create(),archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),
    data=c.allocate(archive.length),resources=c.resources_create(),session=c.game_session_create(),render=c.comp_create();
  memory(c,rp,raw.length).set(raw);assert.equal(c.replay_open(r,rp,raw.length),1);
  const decoded=Buffer.from(memory(c,c.replay_data(r),c.replay_size(r))),stages=[];
  for(let stage=1;stage<=7;stage++){const p=c.replay_stage(r,stage);if(p){const offset=new DataView(c.memory.buffer).getUint32(p+4,true);stages.push({stage,offset,frames:decoded.readUInt32LE(offset+4)});}}
  const stream=new TraceCapture(resolve(out,id+'.candidate.jsonl'),{identity:identity(root,raw),provider:'th11/current-wasi-core',categories:['economy','player','rng','entities']});
  memory(c,data,archive.length).set(archive);assert.equal(c.resources_open(resources,data,archive.length),1);
  assert.equal(c.game_session_replay(session,resources,rp,raw.length,stages[0].stage,1),1);
  const budget=stages.reduce((sum,s)=>sum+s.frames,0)+5000,visited=[];
  let terminal=false;
  try{for(let frame=0;frame<budget;frame++){
    const stage=c.game_session_value(session,2);if(visited.at(-1)!==stage)visited.push(stage);
    assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,'Candidate update');
    const record=stages.find(s=>s.stage===stage),cursor=c.game_session_replay_frame(session);
    const input=cursor>0?decoded.subarray(record.offset+0x90+(cursor-1)*6,record.offset+0x90+cursor*6):Buffer.alloc(0);
    if(c.game_session_value(session,0)===4&&input.equals(Buffer.alloc(6,255))){terminal=true;break;}
    assert.equal(c.game_session_render(session,render),1,'Candidate draw transaction');
    stream.tick(candidateRow(c,session,frame));
    if(c.game_session_value(session,0)!==1){terminal=c.game_session_value(session,0)===4;break;}
  }
  assert.deepEqual(visited,stages.map(s=>s.stage),'Complete route');assert.ok(terminal,'Complete replay lifecycle');
  stream.finish({complete:true,reason:'replay-complete',evidence:{visited,phase:c.game_session_value(session,0)}});
  console.log(JSON.stringify({id,ticks:stream.ticks}));
  }finally{c.comp_delete(render);c.game_session_delete(session);c.resources_delete(resources);c.replay_delete(r);c.release(data);c.release(rp);}
}
