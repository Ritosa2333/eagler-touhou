// One fixed Wasm heap per peer, not per seat. Input edges and all seats are in native memory.
const owners=new WeakSet();
export function bindCoopStage({module:m,players,stageId,window,beforeTick,handoff,caption=()=>{}}){
 const heap=m.HEAPU8,buffer=heap.buffer,states=new Map(),pool=[];
 if(owners.has(m)||typeof m._th128_coop_tick!=='function'||!(buffer instanceof ArrayBuffer)||heap.byteOffset!==0||heap.byteLength!==buffer.byteLength||!Number.isSafeInteger(window)||window<0||window>2||window*heap.length>128*1024*1024)throw Error('Invalid co-op engine/rollback budget');
 owners.add(m);let frame=0,closed=false,fault=false,teamOver=false,cleared=false,stageStart=0,currentId=stageId,musicSerial=0;
 function check(){if(closed||fault||heap.buffer!==m.HEAPU8.buffer||heap.length!==m.HEAPU8.length)throw Error('Closed, faulted or resized co-op engine');}
 function state(){check();const native=JSON.parse(m.UTF8ToString(m._th128_coop_state()));if(!native)return null;return {...native,stageId:currentId,stageStart,stageFrame:native.frame,frame:stageStart+native.frame};}
 if(state()?.frame!==0||state().seats.length!==players)throw Error('Native co-op owner was not bound');
 function step(row){check();if(row?.frame!==frame||!Array.isArray(row.inputs)||row.inputs.length!==players)throw Error('Wrong co-op frame or seat count');
  const keys=[];for(let i=0;i<players;i++){const input=row.inputs[i];if(input?.player!==i||!Number.isSafeInteger(input.held)||input.held<0||input.held>65535||(input.held&~763))throw Error('Wrong seat order or unsupported cooperative input');keys.push(input.held);}
  if(teamOver||cleared){frame++;return cleared?{cleared:true}:{teamOver:true};}const target=m._th128_coop_next_stage();if(target){try{handoff(target);currentId=target;stageStart=frame;musicSerial=0;}catch(e){fault=true;throw e;}}
  beforeTick();const result=m._th128_coop_tick(frame-stageStart,players,keys[0],keys[1],keys[2]??0);if(result!==1&&result!==2&&result!==3){fault=true;throw Error(m.UTF8ToString(m._th128_world_error())||'Co-op stage reached an unsupported gameplay boundary');}
  frame++;teamOver=result===2;cleared=result===3;const fade=m._th128_coop_take_music_fade();if(fade< -1)throw Error('Invalid original music fade');const serial=m._th128_coop_message_music(),track=serial!==musicSerial?1:frame-stageStart===1?0:null;if(track!==null)caption(currentId,track);musicSerial=serial;const effects={...(track!==null?{musicTrack:[currentId,track]}:{}),...(fade>=0?{musicFade:fade}:{}),...(teamOver?{teamOver:true}:{}),...(cleared?{cleared:true}:{})};const count=m._th128_coop_sound_count(),ptr=m._th128_coop_sound_data();
  if(!Number.isSafeInteger(count)||count>65536||count<0||count&&(ptr<1||ptr%4||ptr+12*count>heap.length))throw Error('Invalid cooperative frame sound batch');
  if(!count)return Object.keys(effects).length?effects:undefined;
  const view=new DataView(buffer,ptr,12*count),sounds=Array.from({length:count},(_,i)=>{const at=i*12,id=view.getInt32(at,true),x=view.getFloat32(at+4,true),op=view.getInt32(at+8,true);
    if(id< -1||!Number.isFinite(x)||op<0||op>2)throw Error('Invalid cooperative sound request');return {id,x,op};});return {sounds,...effects};
 }
 function checkpoint(){check();if(states.size>=window)throw Error('Co-op checkpoint budget exhausted');const token=Object.freeze({frame,teamOver,cleared,currentId,stageStart,musicSerial}),page=pool.pop()??new Uint8Array(heap.length);page.set(heap);states.set(token,page);return token;}
 function restore(token){check();const page=states.get(token);if(!page||token.frame>frame)throw Error('Co-op checkpoint is not in this history');heap.set(page);frame=token.frame;teamOver=token.teamOver;cleared=token.cleared;currentId=token.currentId;stageStart=token.stageStart;musicSerial=token.musicSerial;}
 function release(token){const page=states.get(token);if(!page)throw Error('Unowned co-op checkpoint');states.delete(token);pool.push(page);}
 function close(){closed=true;states.clear();pool.length=0;}
 return Object.freeze({state,step,checkpoint,restore,release,close,outcome:()=>teamOver?'team-over':cleared?'cleared':null,requiresConfirmedFrame:()=>!teamOver&&m._th128_coop_next_stage()!==0});
}
