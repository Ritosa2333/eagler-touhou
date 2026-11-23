// Fixed-allocation checkpoint adapter for a dedicated, preloaded TH128 World.
// Native GameOver can be held without opening any host files or media. Its
// logical cursor/marker follows the same ownership as the native heap.
const worldOwners=new WeakSet();

export function bindNativeRollbackWorld({module,seat,checkpointSlots,maxCheckpointBytes,beforeTick=()=>{},holdGameOver=false}){
  if(!module||worldOwners.has(module)||typeof module._th128_world_input!=='function'||
    typeof module._th128_world_tick!=='function'||typeof module._th128_world_error!=='function'||
    typeof module._th128_world_presentation_state!=='function'||typeof module.UTF8ToString!=='function'||
    typeof beforeTick!=='function'||typeof holdGameOver!=='boolean'||
    holdGameOver&&typeof module._th128_world_pause_active!=='function')throw new TypeError('A dedicated native TH128 World is required');
  if(seat!==0&&seat!==1)throw new RangeError('Invalid native World seat');
  const {HEAPU8:memory}=module;
  if(!(memory instanceof Uint8Array)||!(memory.buffer instanceof ArrayBuffer)||memory.byteOffset!==0||
    memory.byteLength!==memory.buffer.byteLength||memory.length===0)throw new Error('Unsupported native World memory');
  const {buffer,byteLength}=memory;
  if(!Number.isSafeInteger(checkpointSlots)||checkpointSlots<0||checkpointSlots>12||
    !Number.isSafeInteger(maxCheckpointBytes)||maxCheckpointBytes<0||checkpointSlots*byteLength>maxCheckpointBytes)
    throw new RangeError('Native rollback exceeds its checkpoint memory budget');
  const initial=JSON.parse(module.UTF8ToString(module._th128_world_presentation_state()));
  if(initial.frame!==0||initial.normalRecord?.bound!==false||initial.playerState!==1)
    throw new Error('Native rollback must start on a fresh player World without the single player recorder');
  worldOwners.add(module);
  const snapshots=new Map(),pool=[];
  let frame=0,closed=false,terminal=null;
  function heap(){
    if(closed)throw new Error('Native rollback World is closed');
    const view=module.HEAPU8;
    if(view.buffer!==buffer||view.byteLength!==byteLength)throw new Error('Native World grew memory beyond its checkpoint boundary');
    return view;
  }
  function checkpoint(){
    const view=heap();
    if(snapshots.size>=checkpointSlots)throw new Error('Native World checkpoint window exhausted');
    const token=Object.freeze({frame,terminal});
    const bytes=pool.pop()??new Uint8Array(byteLength);bytes.set(view);
    snapshots.set(token,bytes);return token;
  }
  function restore(token){
    const bytes=snapshots.get(token);
    if(!bytes||token.frame>frame)throw new Error('Checkpoint is outside this native World history');
    heap().set(bytes);frame=token.frame;terminal=token.terminal;
  }
  function release(token){
    const bytes=snapshots.get(token);
    if(!bytes)throw new Error('Native World checkpoint is no longer owned');
    snapshots.delete(token);pool.push(bytes);
  }
  function step(row){
    heap();const input=row?.inputs?.[seat];
    if(row?.frame!==frame||input?.player!==seat)throw new Error('Native World frame or seat differs from the multiplayer input');
    if(terminal){frame++;return;}
    const error=()=>module.UTF8ToString(module._th128_world_error());
    if(module._th128_world_input(input.held,input.pressed,input.released)!==1)throw new Error(error()||'Native World input rejected');
    beforeTick();
    const result=module._th128_world_tick();
    if(result!==1){
      const detail=error();
      if(!holdGameOver||result!==2||detail||module._th128_world_pause_active()!==1||
         JSON.parse(module.UTF8ToString(module._th128_world_presentation_state())).frame!==frame)
        throw new Error(detail||`Native World crossed a non-playing boundary (${result})`);
      // Result 2 opens the original local GameOver without consuming a World
      // frame. The peer still owns this logical pair row and may keep playing.
      terminal=Object.freeze({reason:'game-over',frame});
    }
    heap();frame++;
  }
  function close(){closed=true;snapshots.clear();pool.length=0;}
  return Object.freeze({checkpoint,restore,release,step,close,terminal:()=>terminal});
}
