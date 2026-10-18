import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,string,root} from '../../tests/cpp/helpers.mjs';
import {preloadStage} from '../../tests/cpp/stage-assets-fixture.mjs';
const c=await core(),asset=preloadStage(c,2,0),run=c.run_session_create(asset.fixture),bytes=readFileSync(resolve(root,'reference/assets/demo1.rpy')),file=c.allocate(bytes.length),out=c.allocate(64),trace=[];
try{
 memory(c,file,bytes.length).set(bytes);if(!c.run_session_load(run,2,0,file,bytes.length))throw Error(string(c,c.run_session_error(run)));
 const visual=c.run_session_world_player_field(run,10);new DataView(c.memory.buffer).setUint32(visual,123,true);new DataView(c.memory.buffer).setUint32(visual+4,0,true);
 if(!c.run_session_world_trace_enable(run))throw Error('Observer registration failed');
 for(let frame=0;frame<5818;frame++){
  if(!c.run_session_step_world(run))throw Error('frame '+frame+' '+string(c,c.run_session_error(run)));
  if(frame>=5815){const data=Array.from(new Uint32Array(c.memory.buffer,c.run_session_world_trace(run),51));c.run_session_world_state(run,out);trace.push({frame,phases:Array.from({length:17},(_,i)=>({priority:data[i*3],seed:data[i*3+1],calls:data[i*3+2]})),world:Array.from(new Uint32Array(c.memory.buffer,out,16))});}
 }
 writeFileSync(resolve(root,'artifacts/cpp/verification/world-cpp-cosmetic-phases.json'),JSON.stringify(trace,null,2));console.log(JSON.stringify(trace));
}finally{c.run_session_delete(run);asset.close();c.release(file);c.release(out);}
