import {evaluateOwnedQualification,sha256Hex} from './ex-qualification.mjs';
import {OwnedEndingReceipts} from './native-ending-browser-host-v4.mjs';

function fail(message){throw new Error(message);}
function bytesEqual(a,b){return a.length===b.length&&a.every((v,i)=>v===b[i]);}
function actualClock(){const value=(BigInt(Date.now())+11644473600000n)*10000n;return{low:Number(value&0xffffffffn),high:Number(value>>32n),seconds:performance.now()/1000};}

export class OriginalExtraRunController{
  constructor({module,isCurrent,clock=actualClock,openReceipts=()=>OwnedEndingReceipts.open()}){
    if(!module||typeof isCurrent!=='function'||typeof clock!=='function'||typeof openReceipts!=='function')fail('EX requires its native module and owning lifecycle');
    for(const name of ['prepare_extra_record_profile','bind_debug_record_clock','debug_record_state','bind_debug_replay_record','bind_debug_record_driver',
      'debug_record_begin_update','debug_record_presentation','debug_record_post_present','debug_record_driver_state','debug_postrun_record',
      'bind_debug_postrun_asset','debug_postrun_retire','debug_record_size','debug_record_data','normal_postrun_state','state','tick','input','error'])
      if(typeof module['_th128_world_'+name]!=='function')fail('EX native API missing: '+name);
    Object.assign(this,{module,isCurrent,clock,openReceipts});this.phase='new';this.closed=false;this.busy=false;this.serial=0;this.frames=0;
    this.epoch=crypto.getRandomValues(new Uint32Array(1))[0]||1;
    const identity=crypto.getRandomValues(new Uint32Array(2));this.capability=(BigInt(identity[1])<<32n)|BigInt(identity[0]);if(!this.capability)this.capability=1n;
    this.input={holding:0,pressed:0,released:0};this.receipts=null;
  }
  current(){if(this.closed||!this.isCurrent())throw new DOMException('EX run owner cancelled','AbortError');}
  json(name,...args){this.current();const text=this.module.UTF8ToString(this.module['_th128_world_'+name](...args));if(!text)fail(this.error());return JSON.parse(text);}
  error(){return this.module.UTF8ToString(this.module._th128_world_error())||'EX native lifecycle rejected';}
  accepted(value){if(value!==1)fail(this.error());}
  memory(bytes,call){this.current();const p=this.module._malloc(bytes.length);if(!p)fail('EX native allocation failed');try{this.module.HEAPU8.set(bytes,p);return call(p,bytes.length);}finally{this.module._free(p);}}
  sample(){const c=this.clock();if(!Number.isInteger(c.low)||c.low<0||c.low>0xffffffff||!Number.isInteger(c.high)||c.high<0||c.high>0xffffffff||!Number.isFinite(c.seconds)||c.seconds<0)fail('EX clock sample invalid');return c;}
  async prepare({record,name,seed}){
    this.current();if(this.phase!=='new'||!Number.isInteger(seed)||seed<0||seed>65535)fail('EX requires a fresh pre-World record owner');
    const qualification=await evaluateOwnedQualification({record,name});this.current();if(!qualification.eligible)fail('EX requires the complete six-route profile');
    if(typeof name.digest!=='string'||await sha256Hex(qualification.name.bytes)!==name.digest)fail('EX independent name receipt digest differs');
    this.current();
    const profile=new Uint8Array(record.profile);if(await sha256Hex(profile)!==qualification.record.sha256)fail('EX profile changed after qualification');this.current();
    this.accepted(this.memory(profile,(p,n)=>this.module._th128_world_prepare_extra_record_profile(p,n,seed,0,this.epoch)));
    const first=this.sample();this.accepted(this.module._th128_world_bind_debug_record_clock(this.epoch,first.low,first.high,first.seconds));
    const native=this.json('debug_record_state');if(!native.prepared||native.epoch!==this.epoch||native.worldBound||native.failed)fail('EX native pre-World identity differs');
    this.profileSHA256=qualification.record.sha256;this.phase='prepared';return qualification;
  }
  bind(assets){
    this.current();if(this.phase!=='prepared'||!Array.isArray(assets)||assets.length!==4)fail('EX replay binding phase rejected');
    const banks=[0,24,25,11];for(let i=0;i<4;++i)if(assets[i].bank!==banks[i]||!(assets[i].bytes instanceof Uint8Array)||!assets[i].bytes.length)fail('EX requires original front/title/title_v/ascii assets');
    this.accepted(this.module._th128_world_bind_debug_replay_record(this.epoch));this.accepted(this.module._th128_world_bind_debug_record_driver(this.epoch));
    const key=new Uint8Array(8);new DataView(key.buffer).setBigUint64(0,this.capability,true);const bound=this.post(0,key);if(!bound.bound)fail('EX native score writer was not bound');
    for(const asset of assets)this.accepted(this.memory(asset.bytes,(p,n)=>this.module._th128_world_bind_debug_postrun_asset(this.epoch,asset.bank,p,n)));
    const record=this.json('debug_record_state'),driver=this.json('debug_record_driver_state');
    if(record.failed||!record.worldBound||!driver.bound||driver.phase!==0||driver.completedFrames!==0||driver.epoch!==this.epoch)fail('EX initial recorder/driver identity differs');
    this.generation=record.generation;this.serial=driver.lastClockSerial;this.phase='active';
  }
  queueInput(holding,pressed,released){this.current();for(const v of [holding,pressed,released])if(!Number.isInteger(v)||v<0||v>65535)fail('EX input mask invalid');this.input.holding=holding;this.input.pressed|=pressed;this.input.released|=released;}
  post(operation,packet){return this.memory(packet,(p,n)=>{const result=this.json('debug_postrun_record',operation,this.epoch,p,n);if(result.accepted!==true||result.epoch!==this.epoch)fail(this.error());return result;});}
  async step({drainAudio,render,onGameOver}){
    this.current();if(this.phase!=='active'||this.busy)fail('EX serial update unavailable');
    if(typeof drainAudio!=='function'||typeof render!=='function')fail('EX requires actual audio and presentation consumers');
    this.busy=true;
    try{
      const before=this.json('state');
      if(before.debugTerminal?.requested&&before.stageControl?.pending===16)return await this.retire();
      const first=this.sample();this.accepted(this.module._th128_world_debug_record_begin_update(this.epoch,this.serial+1,first.low,first.high,first.seconds));++this.serial;
      const input={...this.input};this.input.pressed=this.input.released=0;
      this.accepted(this.module._th128_world_input(input.holding,input.pressed,input.released));
      const advanced=this.module._th128_world_tick();
      if(advanced===2){await drainAudio();this.current();this.phase='game-over';await onGameOver?.();return{gameOver:true};}
      this.accepted(advanced);await drainAudio();this.current();
      const presentation=this.json('debug_record_presentation',this.epoch);
      if(presentation.epoch!==this.epoch||presentation.generation!==this.generation||presentation.frame!==this.frames+1||!Array.isArray(presentation.draws))fail('EX presentation owner/frame differs');
      await render(presentation);this.current();
      const last=this.sample();this.accepted(this.module._th128_world_debug_record_post_present(this.epoch,this.serial+1,presentation.frame,last.low,last.high,last.seconds));
      ++this.serial;++this.frames;return{frame:this.frames,retired:false};
    }catch(error){this.receipts?.close();this.receipts=null;if(error.name!=='AbortError')this.phase='failed';throw error;}finally{this.busy=false;}
  }
  async retire(){
    const native=this.json('state'),driver=this.json('debug_record_driver_state'),record=this.json('debug_record_state');
    if(!native.debugTerminal?.requested||native.debugTerminal.counter!==380||native.debugTerminal.selector!==23
      ||native.stageControl?.current!==7||native.stageControl.pending!==16||driver.phase!==0||driver.lastClockSerial!==this.serial
      ||driver.completedFrames!==native.frame||native.frame!==this.frames||record.clearRecordEvents!==1||record.generation!==this.generation)fail('EX terminal lacks the actual same-owner completed run');
    const first=this.sample(),last=this.sample(),packet=new Uint8Array(48),v=new DataView(packet.buffer);v.setBigUint64(0,this.capability,true);
    [this.serial+1,first.low,first.high,this.serial+2,last.low,last.high].forEach((n,i)=>v.setUint32(8+i*4,n,true));v.setFloat64(32,first.seconds,true);v.setFloat64(40,last.seconds,true);
    const prepared=this.post(1,packet);if(!prepared.prepared||prepared.acknowledged||prepared.generation!==this.generation||prepared.bytes<1||prepared.bytes>1024*1024)fail('EX statistics descriptor rejected');
    const m=this.module,n=m._th128_world_debug_record_size(),p=m._th128_world_debug_record_data();if(n!==52232)fail('EX full profile extent differs');
    const profile=m.HEAPU8.slice(p,p+n),container=m.HEAPU8.slice(prepared.pointer,prepared.pointer+prepared.bytes),payload=new Uint8Array(container.length+profile.length);payload.set(container);payload.set(profile,container.length);
    const digest=await sha256Hex(payload);this.current();this.receipts=await this.openReceipts();this.current();
    const metadata={domain:'extra-native-terminal-statistics',generation:this.generation,index:6,logicalTick:this.frames,profileSHA256:await sha256Hex(profile),descriptorDigest32:prepared.digest32};
    this.current();const receipt=await this.receipts.commit('EX-full-statistics',metadata,payload);this.current();
    if(!receipt.committed||!receipt.fullReadback||receipt.durability!=='strict-indexeddb-transaction'||receipt.bytes!==payload.length||receipt.sha256!==digest
      ||receipt.domain!==metadata.domain||receipt.generation!==this.generation||receipt.index!==6||receipt.logicalTick!==this.frames)fail('EX complete strict statistics receipt differs');
    if(!bytesEqual(profile,m.HEAPU8.slice(p,p+n))||!bytesEqual(container,m.HEAPU8.slice(prepared.pointer,prepared.pointer+prepared.bytes)))fail('EX native statistics changed during persistence');
    const ack=new Uint8Array(20),a=new DataView(ack.buffer);a.setBigUint64(0,this.capability,true);a.setUint32(8,1,true);a.setUint32(12,prepared.bytes,true);a.setUint32(16,prepared.digest32,true);
    if(!this.post(2,ack).acknowledged)fail('EX native statistics ACK rejected');
    this.accepted(m._th128_world_debug_postrun_retire(this.epoch));const returned=this.json('normal_postrun_state');
    if(!returned.present||returned.generation!==this.generation||returned.extraSealed!==true||returned.provenance!=='sealed-extra-terminal23-return')fail('EX native retirement did not produce its sealed owner');
    this.phase='retired';this.receipts.close();this.receipts=null;return{retired:true,snapshot:returned};
  }
  close(){if(this.closed)return;this.closed=true;this.receipts?.close();this.receipts=null;this.input={holding:0,pressed:0,released:0};}
}
