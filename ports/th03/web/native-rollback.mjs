import {packTouch,unpackTouch} from './touch-input.js';
// Bounded rollback scheduler. Snapshot ownership and boundary eligibility are
// supplied by the runtime; no remote-provided bytes are ever restored.
export class NativeRollback {
  constructor({seat,send,capture,restore,release=()=>{},step,confirm,canConfirm=()=>true,onCaptureUnavailable=()=>false,onError=()=>{},maxPrediction=6}) {
    if(![0,1].includes(seat)||!Number.isInteger(maxPrediction)||maxPrediction<1||maxPrediction>12)throw Error('Invalid rollback configuration');
    Object.assign(this,{seat,send,capture,restore,release,step,confirm,canConfirm,onCaptureUnavailable,onError,maxPrediction});
    this.frame=0;this.confirmed=0;this.nextLocal=0;this.local=new Map();this.remote=new Map();this.used=new Map();this.snapshots=new Map();this.closed=false;this.rollbacks=0;this.blocked=false;
  }
  enqueue(input) {
    if(this.closed||this.nextLocal-this.confirmed>=12)return false;
    this.validate(input);const frame=this.nextLocal++;
    this.local.set(frame,input);this.send(JSON.stringify({type:'native-rollback-input',frame,input}));return true;
  }
  submit(input,allowPrediction) {
    if(this.closed)return false;
    this.validate(input);
    if(this.frame-this.confirmed>=this.maxPrediction)return false;
    const frame=this.frame;
    if(!this.local.has(frame)){this.local.set(frame,input);this.nextLocal=Math.max(this.nextLocal,frame+1);this.send(JSON.stringify({type:'native-rollback-input',frame,input}));}
    if(!this.remote.has(frame)&&!allowPrediction)return false;
    return this.advance(allowPrediction);
  }
  advance(allowPrediction) {
    const frame=this.frame;
    if(frame-this.confirmed>=this.maxPrediction)return false;
    if(!this.local.has(frame)||(!this.remote.has(frame)&&!allowPrediction)||(this.blocked&&this.confirmed<frame))return false;
    const remote=this.remote.has(frame)?this.remote.get(frame):this.predict(frame);
    const predicted=!this.remote.has(frame), speculative=predicted||frame>this.confirmed||!this.canConfirm(frame);
    if(speculative){const snapshot=this.capture();if(snapshot==null){if(frame===this.confirmed&&this.remote.has(frame)&&this.onCaptureUnavailable(frame))return this.advance(false);return false;}this.snapshots.set(frame,snapshot);}
    const pair=this.seat===0?[this.local.get(frame),remote]:[remote,this.local.get(frame)];
    this.used.set(frame,remote);
    if(this.step(pair,frame,speculative)===false){
      if(!speculative)throw Error('Confirmed frame failed');
      const snapshot=this.snapshots.get(frame);this.restore(snapshot,frame);this.release(snapshot);this.snapshots.delete(frame);this.used.delete(frame);this.blocked=true;return false;
    }
    this.frame++;this.blocked=false;
    this.flush();return true;
  }
  predict(frame) {
    // Hold continuous controls for bounded prediction. Actual releases rewind
    // incorrect predictions; without received input the prediction limit stops
    // the match from generating indefinite fire. Never repeat one-shot inputs.
    for(let at=frame-1;at>=Math.max(0,frame-12);at--)if(this.remote.has(at)){const input=this.remote.get(at),touch=unpackTouch(input);return packTouch((input&2047)&~(16|128|256),{active:touch.active,unlimited:touch.unlimited});}
    return 0;
  }
  receive(text) {
    if(this.closed)return;
    try{
      const msg=JSON.parse(text);
      if(msg.type!=='native-rollback-input'||!Number.isSafeInteger(msg.frame)||msg.frame<0||msg.frame>this.frame+12)throw Error('Invalid rollback input');
      this.validate(msg.input);
      if(msg.frame<this.confirmed)return;
      if(this.remote.has(msg.frame)&&this.remote.get(msg.frame)!==msg.input)throw Error('Conflicting rollback input');
      this.remote.set(msg.frame,msg.input);
      if(msg.frame<this.frame && this.used.get(msg.frame)!==msg.input)this.rewind(msg.frame);
      this.flush();
    }catch(error){this.closed=true;this.onError(error);}
  }
  rewind(frame) {
    const snapshot=this.snapshots.get(frame);if(snapshot==null)throw Error('Rollback snapshot unavailable');
    const end=this.frame;this.restore(snapshot,frame);this.frame=frame;this.rollbacks++;
    for(const [key,saved] of this.snapshots)if(key>=frame){this.release(saved);this.snapshots.delete(key);}
    for(const key of [...this.used.keys()])if(key>=frame)this.used.delete(key);
    while(this.frame<end){if(!this.advance(true))break;}
  }
  flush() {
    while(this.confirmed<this.frame&&this.remote.has(this.confirmed)&&this.canConfirm(this.confirmed)){
      this.confirm?.(this.confirmed);this.local.delete(this.confirmed);this.used.delete(this.confirmed);if(this.snapshots.has(this.confirmed)){this.release(this.snapshots.get(this.confirmed));this.snapshots.delete(this.confirmed);}this.confirmed++;
    }
    for(const frame of this.remote.keys())if(frame<this.confirmed-12)this.remote.delete(frame);
  }
  validate(value){if(!Number.isSafeInteger(value)||value<0||value>2**45-1)throw Error('Invalid player input');}
  close(){this.closed=true;this.local.clear();this.remote.clear();this.used.clear();for(const saved of this.snapshots.values())this.release(saved);this.snapshots.clear();}
}
