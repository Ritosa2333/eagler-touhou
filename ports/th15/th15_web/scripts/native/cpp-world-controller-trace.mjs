import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,string,root} from '../../tests/cpp/helpers.mjs';
import {preloadStage} from '../../tests/cpp/stage-assets-fixture.mjs';
const specs=[{stage:3,character:1,frames:7339},{stage:2,character:0,frames:6620},{stage:1,character:2,frames:6269}],c=await core(),out=c.allocate(64),observations=[];
try{for(let index=0;index<3;index++){
 const spec=specs[index],asset=preloadStage(c,spec.stage,spec.character),run=c.run_session_create(asset.fixture),bytes=readFileSync(resolve(root,'reference/assets/demo'+index+'.rpy')),file=c.allocate(bytes.length),program=c.stage_assets_program(asset.fixture),phases=[];
 try{memory(c,file,bytes.length).set(bytes);if(!c.run_session_load(run,spec.stage,spec.character,file,bytes.length))throw Error(string(c,c.run_session_error(run)));
 let last=0,active=0,firstActive=-1;
 for(let frame=0;frame<spec.frames;frame++){
  if(!c.run_session_step_world(run))throw Error('frame '+frame+' '+string(c,c.run_session_error(run)));
  const dv=new DataView(c.memory.buffer),spell=dv.getUint32(c.run_gameplay_identity(run,10),true),flags=dv.getUint32(spell,true);if(flags&1){active++;if(firstActive<0)firstActive=frame;}
  if(frame%600===0||flags!==last||frame===spec.frames-1){const ctx=c.run_session_world_enemy_context(run,1),id=ctx?dv.getInt32(ctx+4,true):-1;const bosses=[];for(let id=1;id<500;id++){const f=c.run_session_world_enemy_flags(run,id);if(!(f&0x800000))continue;const ctx=c.run_session_world_enemy_context(run,id),ri=ctx?dv.getInt32(ctx+4,true):-1;bosses.push({id,flags:f,name:ri>=0?string(c,c.program_name(program,ri)):'none',time:ctx?dv.getFloat32(ctx,true):null,instruction:ctx?dv.getInt32(ctx+8,true):null});}phases.push({frame,flags,bosses,main:ctx?{time:dv.getFloat32(ctx,true),routine:id,name:id>=0?string(c,c.program_name(program,id)):'none',instruction:dv.getInt32(ctx+8,true)}:null});}last=flags;
 }
 observations.push({index,active,firstActive,phases});
 }finally{c.run_session_delete(run);asset.close();c.release(file);}
}writeFileSync(resolve(root,'artifacts/cpp/verification/world-cpp-controller-phases.json'),JSON.stringify(observations,null,2));console.log(JSON.stringify(observations));}finally{c.release(out);}
