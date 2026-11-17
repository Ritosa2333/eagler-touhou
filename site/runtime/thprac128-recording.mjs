import {readThprac128Native,createThprac128Session} from './thprac128.mjs';
import {nativeReplayToFile} from './sound.mjs';
import {attachThprac128Replay} from './thprac128-replay.mjs';
export function createThprac128Recorder(module,input,identity,{seconds,isCurrent=()=>true}={}){
 const session=createThprac128Session(input.params,input.difficulty),receipt=readThprac128Native(module,session);let closed=false;
 if(typeof module?._th128_world_thprac_replay_begin!=='function'||typeof module?._th128_world_thprac_replay_clock!=='function'||typeof module?._th128_world_thprac_replay_state!=='function')throw new Error('TH128 actual practice recording runtime unavailable');
 const current=()=>{if(closed||!isCurrent())throw new DOMException('Practice recording owner closed','AbortError');};
 const native=()=>{current();const s=JSON.parse(module.UTF8ToString(module._th128_world_thprac_replay_state()));if(s.active!==true||s.generation!==receipt.generation||s.stage!==session.params.stage+1||s.rank!==session.difficulty)throw new Error('Practice recording generation/owner identity changed');return s;};
 current();if(module._th128_world_thprac_replay_begin(seconds)!==1)throw new Error(module.UTF8ToString(module._th128_world_error())||'practice recording initialization rejected');native();
 return Object.freeze({session,identity:Object.freeze({...identity}),generation:receipt.generation,
  clock(seconds,fps){native();if(module._th128_world_thprac_replay_clock(seconds,fps)!==1)throw new Error(module.UTF8ToString(module._th128_world_error())||'practice pre-tick clock rejected');},
  snapshot(){return Object.freeze(native());},
  capture(timestamp,name){const before=native();if(before.frames<1||before.clockPending)throw new Error('Practice recording has no completed input history');if(typeof timestamp!=='bigint'||timestamp<0n||timestamp>0x793406fffn||!(name instanceof Uint8Array)||name.length!==8)throw new Error('Practice Replay requires timestamp and explicit8byte name');const p=module._malloc(8);let size;try{module.HEAPU8.set(name,p);size=module._th128_world_thprac_replay_snapshot(Number(timestamp&0xffffffffn),Number(timestamp>>32n),p,8);}finally{module._free(p);}if(!Number.isSafeInteger(size)||size<292||size>16*1024*1024)throw new Error(module.UTF8ToString(module._th128_world_error())||'practice native snapshot unavailable');const pointer=module._th128_world_thprac_replay_data();if(!Number.isSafeInteger(pointer)||pointer<=0||pointer+size>module.HEAPU8.length)throw new Error('Practice native snapshot memory extent rejected');const raw=module.HEAPU8.slice(pointer,pointer+size);native();return attachThprac128Replay(nativeReplayToFile(raw),session,identity);},
  close(){closed=true;}
 });
}
