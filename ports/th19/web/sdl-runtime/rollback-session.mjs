// Bounded input prediction with explicit native checkpoints and confirmed effects.
const neutral=()=>({keys:0,direct:0,x:0,y:0});
const equal=(a,b)=>a.keys===b.keys&&a.direct===b.direct&&a.x===b.x&&a.y===b.y;
const validate=i=>{if(!i||!Number.isInteger(i.keys)||i.keys<0||i.keys>2047||![0,1].includes(i.direct)||!Number.isFinite(i.x)||!Number.isFinite(i.y)||Math.abs(i.x)>512||Math.abs(i.y)>512)throw Error('Invalid input');return {...i};};
export class RollbackSession {
 constructor(core,{side=0,window=8,send=()=>{},commit=()=>{}}={}){
  if(![0,1].includes(side)||!Number.isInteger(window)||window<0||window>12)throw Error('Invalid rollback configuration');
  Object.assign(this,{core,side,window,send,commit});this.frame=0;this.confirmed=0;this.remoteEnd=0;this.dirty=null;this.rollbacks=0;this.resimulated=0;
  this.local=new Map();this.remote=new Map();this.records=new Map();this.snapshots=new Map();this.previousRemote=neutral();
 }
 receive(frame,input){
  if(!Number.isSafeInteger(frame)||frame<0||frame>this.frame+300)throw Error('Invalid input frame');input=validate(input);
  if(frame<this.confirmed)return;
  const old=this.remote.get(frame);if(old&&!equal(old,input))throw Error('Conflicting confirmed input');
  this.remote.set(frame,input);
  const used=this.records.get(frame);if(used&&!equal(used.remote,input))this.dirty=this.dirty===null?frame:Math.min(this.dirty,frame);
  while(this.remote.has(this.remoteEnd))this.remoteEnd++;
 }
 predict(frame){
  const last=this.records.get(frame-1)?.remote||this.previousRemote;
  // Repeat held controls only. Never repeat relative touch displacement or
  // invent a new bomb/pause edge while waiting for actual remote input.
  return {keys:last.keys&~(4|1024),direct:0,x:0,y:0};
 }
 execute(frame,resimulating){
  const remote=this.remote.get(frame)||this.predict(frame),local=this.local.get(frame);
  const inputs=this.side===0?[local,remote]:[remote,local];
  // A received future input is still speculative until every earlier remote
  // input is present. Never cross a resource/pause boundary on that branch.
  const speculative=frame>=this.remoteEnd;
  if(speculative&&this.core.canPredict?.(...inputs)===false)return false;
  // Confirmed frames cannot change: copy only the states needed to repair
  // prediction. This avoids deep-copying two worlds on every ordinary tick.
  if(speculative&&!this.snapshots.has(frame))this.snapshots.set(frame,this.core.save());
  // Native effects must be returned as data, never emitted during speculation.
  const effects=this.core.step(frame,...inputs,{resimulating,speculative});
  this.records.set(frame,{remote:{...remote},effects});return true;
 }
 reconcile(){
  if(this.dirty!==null){
   const begin=this.dirty;this.dirty=null;const saved=this.snapshots.get(begin);if(saved===undefined)throw Error('Missing rollback checkpoint');
   this.core.load(saved);for(const [f,s] of this.snapshots)if(f>begin){this.core.release?.(s);this.snapshots.delete(f);}
   for(let f=begin;f<this.frame;f++){
    if(!this.execute(f,true)){
     this.frame=f;
     for(const key of this.records.keys())if(key>=f)this.records.delete(key);
     break;
    }
    this.resimulated++;
   }this.rollbacks++;
  }
  const end=Math.min(this.remoteEnd,this.frame);
  while(this.confirmed<end){const f=this.confirmed,record=this.records.get(f);this.commit(f,record.effects);this.previousRemote=record.remote;
   this.core.release?.(this.snapshots.get(f));this.snapshots.delete(f);this.local.delete(f);this.remote.delete(f);this.records.delete(f);this.confirmed++;
  }
 }
 advance(input){
  this.reconcile();
  if(!this.local.has(this.frame)){const own=validate(input);this.local.set(this.frame,own);this.send(this.frame,own);}
  if(this.frame>=this.remoteEnd&&this.frame-this.confirmed>=this.window)return false;
  if(!this.execute(this.frame,false))return false;this.frame++;this.reconcile();return true;
 }
 close(){for(const s of this.snapshots.values())this.core.release?.(s);this.snapshots.clear();this.records.clear();this.local.clear();this.remote.clear();}
}
