// Diagnostic runner for a supplied native replay. No forced outcomes or inputs.
import {readFileSync,readdirSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {core,memory,root} from './helpers.mjs';
const file=resolve(process.argv[2]??resolve(root,'../[th09] 东方花映塚 (日文版)/replay/th9_udyt22.rpy'));
const c=await core(),f=c.session_create(),r=c.replay_create(),name=c.allocate(128),data=c.allocate(16000000),raw=readFileSync(file);
const text=p=>{const b=memory(c,p,512);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
const dv=()=>new DataView(c.memory.buffer),fieldMap=(fn,count)=>Array.from({length:count()},(_,i)=>Array.from({length:3},(_,j)=>dv().getUint32(fn()+i*12+j*4,true)));
const selectionFields=fieldMap(c.stage_selection_fields,c.stage_selection_field_count),motionFields=fieldMap(c.motion_fields,c.motion_field_count);
for(const n of readdirSync(resolve(root,'reference/assets'))){const b=readFileSync(resolve(root,'reference/assets',n));memory(c,name,128).fill(0);memory(c,name,128).set(Buffer.from(n));memory(c,data,b.length).set(b);c.session_file(f,name,data,b.length);}
memory(c,data,raw.length).set(raw);if(!c.replay_decode(r,data,raw.length))throw Error('Invalid replay');const decoded=c.replay_data(r),header=memory(c,decoded,0x1ec),streams=[];
for(let stage=0;stage<10;++stage){const p=dv().getUint32(decoded+32+stage*4,true);if(!p)continue;const q=dv().getUint32(decoded+72+stage*4,true);streams.push({stage,left:Array.from(memory(c,decoded+p,32)),right:Array.from(memory(c,decoded+q,32))});}
const metadata={sha256:createHash('sha256').update(raw).digest('hex'),name:Buffer.from(header.subarray(0xce,0xd7)).toString(),mode:header[0x1e4],difficulty:header[0xd7],health:[...header.subarray(0x1e6,0x1e8)],config:[...header.subarray(0xdc+0xac,0xdc+0xbc)],streams};console.log(JSON.stringify({metadata}));
const reports=[];
for(const stream of process.env.TH09_CONTINUOUS?streams.slice(0,1):streams){memory(c,data,raw.length).set(raw);if(!c.session_play(f,data,raw.length,stream.stage))throw Error(text(c.session_error(f)));const changes=[];let previous='',frames=0,minHealth=10;
 const read=()=>{const p=c.session_part(f,0,21),get=a=>dv().getInt32(p+selectionFields.find(v=>v[0]===a)[1],true);return {stage:get(0x4a7e8c),left:get(0x4a7db0),right:get(0x4a7de8),background:get(0x4a7e84),cpu:get(0x4a7df4),seed:dv().getUint16(c.session_part(f,0,6),true),health:[0,1].map(s=>dv().getInt32(c.session_part(f,s,0)+motionFields[0][1],true)),phase:c.session_value(f,0),replayFrame:c.session_value(f,2),length:c.session_value(f,3)};};
 const initial=read();console.log(JSON.stringify({initial}));
 for(let n=0;n<120000&&c.session_value(f,0)===1;++n){if(!c.session_step(f,0,0,0))throw Error(text(c.session_error(f)));const state=read();++frames;minHealth=Math.min(minHealth,state.health[0]);const key=JSON.stringify([state.stage,state.health,state.phase]);if(key!==previous){changes.push({tick:n,...state});previous=key;}if(!process.env.TH09_CONTINUOUS&&state.stage!==stream.stage)break;}
 const report={start:stream.stage,initial,frames,minHealth,final:read(),changes};reports.push(report);console.log(JSON.stringify(report));
}
mkdirSync(resolve(root,'artifacts/replay-regression'),{recursive:true});writeFileSync(resolve(root,`artifacts/replay-regression/${process.env.TH09_CONTINUOUS?'continuous':'inspection'}.json`),JSON.stringify({metadata,reports},null,2));c.session_delete(f);c.replay_delete(r);
