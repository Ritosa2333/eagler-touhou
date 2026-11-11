const INVALID=0xffffffff;
let modulePromise;
export async function loadRollbackCore() {
  modulePromise ??= fetch(new URL('./rollback-core.wasm',import.meta.url)).then(async response=>{
    if(!response.ok)throw Error('无法加载公共回滚核心');
    return WebAssembly.compile(await response.arrayBuffer());
  });
  return modulePromise;
}
export class Pc98RollbackCore {
  constructor(module,{players,slot,limit,held,directions,directionFrames}) {
    const wasi={clock_time_get:()=>0,fd_close:()=>0,fd_seek:()=>0,fd_write:()=>0};
    this.api=new WebAssembly.Instance(module,{wasi_snapshot_preview1:wasi}).exports;
    this.api._initialize();
    this.pointer=this.api.pc98_rollback_create(players,slot,limit,held,directions,directionFrames);
    if(!this.pointer)throw Error('公共回滚核心初始化失败');
    this.players=players;this.slot=slot;
  }
  submit(slot,frame,value){
    const args=[this.pointer,frame,value.buttons,value.mode,value.x,value.y,value.flags];
    if(slot===this.slot){if(!this.api.pc98_rollback_local(...args))throw Error('公共回滚核心拒绝本机输入');return;}
    const result=this.api.pc98_rollback_remote(this.pointer,slot,...args.slice(1));
    if(result>=4)throw Error('公共回滚核心拒绝远端输入：'+result);
  }
  prepare(frame){
    const a=this.api,p=this.pointer;if(!a.pc98_rollback_prepare(p,frame))return null;
    return Array.from({length:this.players},(_,slot)=>({buttons:a.pc98_rollback_buttons(p,slot),mode:a.pc98_rollback_mode(p,slot),x:a.pc98_rollback_x(p,slot),y:a.pc98_rollback_y(p,slot),flags:a.pc98_rollback_flags(p,slot)}));
  }
  commit(frame){if(!this.api.pc98_rollback_commit(this.pointer,frame))throw Error('公共回滚核心无法提交输入');}
  get dirty(){const value=this.api.pc98_rollback_dirty(this.pointer)>>>0;return value===INVALID?null:value;}
  rewind(frame){if(!this.api.pc98_rollback_rewind(this.pointer,frame))throw Error('公共回滚核心无法恢复输入历史');}
  retire(slot,frame){if(!this.api.pc98_rollback_retire(this.pointer,slot,frame))throw Error('公共回滚核心无法切换离线玩家');}
  refresh(){this.api.pc98_rollback_refresh(this.pointer);}
  discard(frame){this.api.pc98_rollback_discard(this.pointer,frame);}
  commitInputs(frame,values,predictedMask){
    if(!this.api.pc98_rollback_prepare(this.pointer,frame))throw Error('公共回滚核心无法准备已执行帧');
    for(let slot=0;slot<values.length;slot++){const v=values[slot];this.api.pc98_rollback_set_decision(this.pointer,slot,v.buttons,v.mode,v.x,v.y,v.flags,(predictedMask>>slot)&1);}
    this.commit(frame);this.refresh();
  }
  close(){this.api.pc98_rollback_destroy(this.pointer);this.pointer=0;}
}
