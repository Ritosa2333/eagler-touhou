import {Lockstep} from '../lockstep.js';
import {TICK_MS} from '../frame-queue.js';
const same=(a,b)=>JSON.stringify(a)===JSON.stringify(b);
// Page owns the queue and transport; a single awaited transaction owns guest
// execution. Never commit audio/hash/history from a branch invalidated in flight.
export class AsyncLockstep extends Lockstep {
  constructor(options){super(options);this.execute=options.execute;this.replaySlice=options.replaySlice;this.confirmQueued=options.confirmQueued;}
  planReplay(){
    const cursor=Object.create(this.queue),plans=[];
    let previous=[...this.checkpoints].at(-1);
    for(;cursor.frame<this.replayUntil&&plans.length<4;cursor.frame++){
      const inputs=cursor.peek();if(!inputs)break;
      const frame=cursor.frame,capture=cursor.active&&frame>=cursor.confirmed&&(previous===undefined||frame-previous>=this.snapshotStride);
      plans.push({frame,inputs,capture,hash:(frame+1)%120===0,end:frame+1===this.replayUntil});
      if(capture)previous=frame;
    }
    return plans;
  }
  reconcile(){
    const q=this.queue,remote=q.slot^1;q.dirty=null;
    for(const [at,pair]of q.used)if(!same(pair[remote],q.predicted(remote,at)))q.dirty=q.dirty===null?at:Math.min(q.dirty,at);
    return q.dirty;
  }
  async confirmFrames(){
    if(this.queue.dirty!==null)return false;
    const prefix=Math.min(this.queue.frame,this.queue.confirmed),preceding=[...this.checkpoints].filter(frame=>frame<=prefix).at(-1);
    const retainFrom=this.snapshotStride===1?prefix:(preceding??prefix);
    if(this.lastConfirmation?.prefix!==prefix||this.lastConfirmation?.retainFrom!==retainFrom){
      // The worker's serial queue applies this before the next execute/restore.
      // Legacy callers can still require an acknowledged confirmation.
      if(this.confirmQueued)this.confirmQueued(prefix,retainFrom);
      else await this.confirm?.(prefix,retainFrom);
      if(!this.running)return false;
      this.costs.confirmCallbacks++;this.lastConfirmation={prefix,retainFrom};
      this.recorder?.confirm(prefix,this.queue.dirty!==null);
    }else this.costs.skippedConfirmations++;
    for(const frame of this.checkpoints)if(frame<retainFrom)this.checkpoints.delete(frame);
    for(const [frame,hash]of this.pendingHashes)if(frame<=prefix){
      this.hashes.set(frame,hash);this.network.sendPacket({type:'check',frame,hash});this.compare(frame);this.pendingHashes.delete(frame);
    }
    // Receipt may advance q.confirmed during the awaited RPC. Only prune the
    // prefix acknowledged by the worker, retaining newly dirty executed history.
    this.queue.prune(prefix);return this.queue.dirty===null;
  }
  async pump(now=performance.now()){
    if(!this.running||this.pumping)return;this.pumping=true;
    let replayProgress=false,replayWaiting=false;
    try{
      now=Math.max(now,this.last);this.budget+=now-this.last;this.last=now;
      this.budget=this.budget>=TICK_MS*4?TICK_MS:Math.min(this.budget,TICK_MS*2);
      if(this.queue.waitReason)this.budget=Math.min(this.budget,TICK_MS);
      const started=performance.now();let steps=0;
      const confirm=async()=>{const at=performance.now();const result=await this.confirmFrames();this.costs.confirm+=performance.now()-at;return result;};
      const captureLocal=(frontier=this.queue.frame)=>{if(this.queue.needsCapture(frontier)){const packet=this.queue.capture(this.network.capture(),frontier);if(packet)this.network.sendPacket(packet);}};
      const simulate=async(replay,repairing=false)=>{
        const pair=this.nextInputs(repairing);if(!pair)return false;
        const q=this.queue,frame=q.frame,previous=[...this.checkpoints].at(-1);
        const capture=q.active&&frame>=q.confirmed&&(previous===undefined||frame-previous>=this.snapshotStride),hash=(frame+1)%120===0;
        const at=performance.now();const result=await this.execute(pair,{frame,replay,capture,hash});
        if(!this.running)return false;
        this.costs.simulate+=performance.now()-at;
        if(capture){this.checkpoints.add(frame);this.costs.captures++;this.costs.capture+=result?.captureMs||0;}
        q.commit(pair);this.costs.steps++;this.reconcile();
        if(hash){this.pendingHashes.set(q.frame,result.hash);this.costs.hash+=result.hashMs||0;}
        this.recorder?.stage(frame,{inputs:pair,offlineMask:0,proof:result.proof,hash:hash?result.hash:null});
        steps++;this.lastProgress=performance.now();return true;
      };
      if(this.replayUntil!==null&&this.queue.frame<this.replayUntil&&this.budget>=TICK_MS)captureLocal(this.replayUntil);
      if(this.queue.dirty!==null)this.reconcile();
      if(this.queue.dirty!==null){
        if(this.budget>=TICK_MS&&(this.replayUntil===null||this.queue.frame>=this.replayUntil))captureLocal();
        const dirty=this.queue.dirty,from=this.snapshotStride===1?dirty:[...this.checkpoints].filter(frame=>frame<=dirty).at(-1);
        if(from===undefined)throw Error('回滚检查点已过期');
        this.replayUntil=Math.max(this.replayUntil??0,this.queue.frame);
        const at=performance.now(),restored=await this.restore(from);if(!this.running)return;
        this.costs.restore+=performance.now()-at;this.costs.restores++;this.queue.rewind(restored);this.reconcile();this.lastConfirmation=null;
        for(const frame of this.checkpoints)if(frame>=restored)this.checkpoints.delete(frame);
        for(const frame of this.pendingHashes.keys())if(frame>restored)this.pendingHashes.delete(frame);
      }
      if(this.queue.dirty!==null)return;
      if(!await confirm()||!this.running)return;
      while(this.replayUntil!==null&&this.queue.frame<this.replayUntil&&steps<4&&(steps===0||performance.now()-started<6)){
        if(this.replaySlice){
          const plans=this.planReplay();
          if(!plans.length){this.nextInputs(true);replayWaiting=true;break;}
          const at=performance.now(),outcome=await this.replaySlice(this.queue.frame,plans);if(!this.running)return;
          if(!Number.isInteger(outcome?.applied)||outcome.applied<1||outcome.applied>plans.length||!Array.isArray(outcome.records)||outcome.records.length!==outcome.applied)throw Error('TH03 回滚补算返回边界异常');
          for(let index=0;index<outcome.applied;index++){
            const plan=plans[index],record=outcome.records[index];
            if(record.frame!==plan.frame)throw Error('TH03 回滚补算记录帧异常');
            if(plan.capture){this.checkpoints.add(plan.frame);this.costs.captures++;this.costs.capture+=record.captureMs||0;}
            this.queue.commit(plan.inputs);steps++;this.costs.steps++;this.costs.replays++;
            if(plan.hash){this.pendingHashes.set(plan.frame+1,record.hash);this.costs.hash+=record.hashMs||0;}
            this.recorder?.stage(plan.frame,{inputs:plan.inputs,offlineMask:0,proof:record.proof,hash:plan.hash?record.hash:null});
          }
          this.costs.simulate+=performance.now()-at;this.lastProgress=performance.now();this.reconcile();replayProgress=true;
          if(this.queue.dirty!==null||!await confirm()||!this.running)return;
          if(outcome.applied<plans.length)break;
          continue;
        }
        if(!await simulate(this.queue.frame+1<this.replayUntil,true)){replayWaiting=true;break;}this.costs.replays++;replayProgress=true;
        if(this.queue.dirty!==null||!await confirm()||!this.running)return;
      }
      if(this.replayUntil!==null&&this.queue.frame>=this.replayUntil)this.replayUntil=null;
      while(this.replayUntil===null&&this.budget>=TICK_MS&&steps<4&&(steps===0||performance.now()-started<6)){
        captureLocal();if(!await simulate(false))break;this.budget-=TICK_MS;
        if(this.queue.dirty!==null||!await confirm()||!this.running)return;
      }
      if(!this.running)return;
      const wait=performance.now()-this.lastProgress;
      if(wait>15000&&['window','prediction-lead'].includes(this.queue.waitReason))throw Error('联机等待对端输入超时，请双方检查网络连接');
      if(wait>600)this.onState?.(`等待另一端第 ${this.queue.frame} 帧输入`);
      else if(steps&&this.queue.frame%120===0)this.onState?.(`帧同步运行中 · ${this.queue.frame}`);
      if(this.replayUntil===null)this.present?.();
    }catch(error){this.fail(error);}finally{
      this.pumping=false;
      if(this.running){this.schedule();this.wake(this.queue.dirty!==null?0:this.replayUntil!==null?(replayProgress&&!replayWaiting?0:16):Math.max(1,TICK_MS-this.budget-(performance.now()-this.last)));}
    }
  }
}
