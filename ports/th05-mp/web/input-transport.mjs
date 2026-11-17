// Canonical source; tools/sync_input_transport.mjs copies this into each title.
const WIRE='pc98-input/1',RETRY_MS=32,REPAIR_MS=80;
const MAX_PENDING=256,MAX_BYTES=262144,PACKET_BYTES=1150,REPAIR_BYTES=16384;
const states=new WeakMap();
const encoder=new TextEncoder();
const bytes=value=>encoder.encode(value).length;
const integer=value=>Number.isSafeInteger(value)&&value>=0;
const isInput=text=>{
  const value=JSON.parse(text);
  return (value.type==='input'&&Number.isSafeInteger(value.frame))||value.type==='frame';
};

// Preserve control/input causality across two lanes. Inputs within one control
// epoch can arrive out of order; commands never pass their preceding inputs.
export class InputTransport {
  constructor(label,{now=()=>performance.now(),repeat=(fn,ms)=>setInterval(fn,ms),cancel=id=>clearInterval(id)}={}){
    Object.assign(this,{label,now,repeat,cancel});
    this.kind='loss-resilient';this.ordered=false;this.maxRetransmits=0;this.maxPacketLifeTime=null;
    this.readyState='connecting';this.control=null;this.fast=null;this.remoteHello=false;this.early=[];
    this.sentInput=0;this.receivedInput=0;this.deliveredInput=0;
    this.sentControl=0;this.receivedControl=0;this.deliveredControl=0;
    this.pending=new Map();this.incoming=new Map();this.received=new Set();this.delivered=new Set();this.controls=new Map();
    this.pendingBytes=0;this.nextRetry=0;this.lastProgress=now();this.lastRepair=now();this.lastAckRepair=now();
    this.ackDirty=false;this.ackRepair=false;this.draining=false;this.timer=null;
    this.stats={fastSent:0,fastReceived:0,reliableRepairs:0,ackRepairs:0,duplicates:0,retransmitted:0,received:0};
  }
  get bufferedAmount(){return (this.control?.bufferedAmount||0)+(this.fast?.bufferedAmount||0);}
  get transportStats(){return {...this.stats,pending:this.pending.size,pendingBytes:this.pendingBytes,acknowledged:this.sentInput-this.pending.size,receivedThrough:this.receivedInput};}
  bind(channel,fast){
    const field=fast?'fast':'control';
    if(this[field]||channel.label!==(fast?this.label+'-fast':this.label))throw Error('Duplicate or unknown input transport lane');
    if(fast?(channel.ordered!==false||channel.maxRetransmits!==0):
      (channel.ordered!==true||channel.maxRetransmits!=null||channel.maxPacketLifeTime!=null))throw Error('Incompatible input transport lane');
    this[field]=channel;
    let opened=false;
    channel.onopen=()=>this.guard(()=>{
      if(opened)return;opened=true;
      if(!fast)channel.send(JSON.stringify({wire:WIRE,kind:'hello'}));
      this.open();
    });
    channel.onmessage=event=>this.guard(()=>this.receive(event.data,fast));
    channel.onerror=event=>this.fail(event.error||Error('Input transport lane failed'));
    channel.onclose=()=>this.close();
    if(channel.readyState==='open')channel.onopen();
  }
  guard(fn){if(this.readyState==='closed')return;try{fn();}catch(error){this.fail(error);}}
  fail(error){if(this.readyState==='closed')return;try{this.onerror?.({error});}finally{this.close();}}
  open(){
    if(this.readyState!=='connecting'||!this.remoteHello||this.control?.readyState!=='open'||this.fast?.readyState!=='open')return;
    this.readyState='open';this.lastProgress=this.lastRepair=this.lastAckRepair=this.now();
    this.timer=this.repeat(()=>this.guard(()=>this.pump()),16);
    this.onopen?.();this.drain();
  }
  send(text){
    if(this.readyState!=='open')throw Error('Input transport is not open');
    if(typeof text!=='string'||bytes(text)>65536)throw Error('Invalid input transport message');
    if(isInput(text)){
      const size=bytes(text);
      if(size>8192)throw Error('Input frame exceeds transport limit');
      if(this.pending.size>=MAX_PENDING||this.pendingBytes+size>MAX_BYTES)throw Error('Unacknowledged input history is full');
      if(!this.pending.size)this.lastProgress=this.lastRepair=this.now();
      const entry={id:++this.sentInput,barrier:this.sentControl,data:text,size};
      this.pending.set(entry.id,entry);this.pendingBytes+=size;
      this.transmit(false,entry.id);
    }else{
      const id=this.sentControl+1;
      this.control.send(JSON.stringify({wire:WIRE,kind:'control',id,fence:this.sentInput,data:text}));
      this.sentControl=id;
    }
  }
  packet(limit,preferred){
    const entries=[];
    const envelope=()=>({wire:WIRE,kind:'inputs',ack:this.receivedInput,entries});
    const add=entry=>{
      const value={id:entry.id,barrier:entry.barrier,data:entry.data};
      entries.push(value);
      if(bytes(JSON.stringify(envelope()))>limit){entries.pop();return false;}
      return true;
    };
    if(preferred&&this.pending.has(preferred))add(this.pending.get(preferred));
    for(const entry of this.pending.values())if(entry.id!==preferred&&!add(entry))break;
    return envelope();
  }
  transmit(reliable,preferred){
    const lane=reliable?this.control:this.fast;
    if(lane?.readyState!=='open'||lane.bufferedAmount>(reliable?32768:65536))return false;
    const packet=this.packet(reliable?REPAIR_BYTES:PACKET_BYTES,preferred);
    // Oversized input is retained and delivered on the reliable repair lane.
    if(this.pending.size&&!packet.entries.length&&!this.ackDirty)return false;
    lane.send(JSON.stringify(packet));
    if(reliable){this.stats.reliableRepairs++;this.lastRepair=this.now();}
    else{this.stats.fastSent++;this.nextRetry=this.now()+RETRY_MS;}
    for(const entry of packet.entries){const pending=this.pending.get(entry.id);if(pending.sends)this.stats.retransmitted++;pending.sends=(pending.sends||0)+1;}
    this.ackDirty=false;
    return true;
  }
  pump(){
    if(this.readyState!=='open')return;
    const now=this.now();
    if((this.pending.size||this.ackDirty)&&now>=this.nextRetry)this.transmit(false);
    if(this.pending.size&&now-this.lastProgress>=REPAIR_MS&&now-this.lastRepair>=REPAIR_MS)this.transmit(true);
    if(this.ackRepair&&now-this.lastAckRepair>=REPAIR_MS&&this.control?.readyState==='open'&&this.control.bufferedAmount<=32768){
      this.control.send(JSON.stringify({wire:WIRE,kind:'inputs',ack:this.receivedInput,entries:[]}));
      this.stats.ackRepairs++;this.lastAckRepair=now;this.ackRepair=false;
    }
  }
  receive(text,fast){
    if(typeof text!=='string'||bytes(text)>131072)throw Error('Invalid input transport packet');
    const packet=JSON.parse(text);
    if(packet?.wire!==WIRE)throw Error('Input transport version differs; refresh all players');
    if(packet.kind==='hello'){
      if(fast||this.remoteHello)throw Error('Invalid input transport handshake');
      this.remoteHello=true;this.open();for(const early of this.early.splice(0))this.receive(early,true);return;
    }
    if(!this.remoteHello){
      if(fast&&packet.kind==='inputs'&&this.early.length<32){this.early.push(text);return;}
      throw Error('Input transport handshake is missing');
    }
    if(packet.kind==='control'){
      if(fast||packet.id!==this.receivedControl+1||!integer(packet.fence)||packet.fence>this.deliveredInput+MAX_PENDING||
        typeof packet.data!=='string'||bytes(packet.data)>65536||this.controls.size>=MAX_PENDING)throw Error('Invalid input control sequence');
      this.receivedControl=packet.id;this.controls.set(packet.id,packet);this.drain();return;
    }
    if(packet.kind!=='inputs'||!integer(packet.ack)||packet.ack>this.sentInput||!Array.isArray(packet.entries)||packet.entries.length>MAX_PENDING)throw Error('Invalid input acknowledgement');
    // Validate the entire bundle before acknowledging it or publishing inputs.
    const bundle=new Map();
    for(const entry of packet.entries){
      if(!integer(entry.id)||entry.id<1||entry.id>this.deliveredInput+MAX_PENDING||!integer(entry.barrier)||
        entry.barrier>this.receivedControl+MAX_PENDING||typeof entry.data!=='string'||bytes(entry.data)>8192||!isInput(entry.data))throw Error('Invalid input history entry');
      const previous=bundle.get(entry.id)||this.incoming.get(entry.id);
      if(previous&&(previous.data!==entry.data||previous.barrier!==entry.barrier))throw Error('Conflicting input history');
      bundle.set(entry.id,entry);
    }
    for(const [id,entry]of this.pending){if(id>packet.ack)break;this.pending.delete(id);this.pendingBytes-=entry.size;this.lastProgress=this.lastRepair=this.now();}
    if(fast)this.stats.fastReceived++;
    let fresh=0;
    for(const entry of packet.entries){
      if(entry.id<=this.receivedInput||this.received.has(entry.id))this.stats.duplicates++;
      else{this.incoming.set(entry.id,entry);this.received.add(entry.id);this.stats.received++;fresh++;}
    }
    if(packet.entries.length)this.ackRepair=fresh===0;
    while(this.received.delete(this.receivedInput+1))this.receivedInput++;
    if(packet.entries.length)this.ackDirty=true;
    this.drain();
  }
  drain(){
    if(this.draining||this.readyState!=='open')return;
    this.draining=true;
    try{
      let changed=true;
      while(changed&&this.readyState==='open'){
        changed=false;
        for(const [id,entry]of this.incoming){
          if(entry.barrier>this.deliveredControl)continue;
          this.incoming.delete(id);this.delivered.add(id);
          while(this.delivered.delete(this.deliveredInput+1))this.deliveredInput++;
          this.onmessage?.({data:entry.data});changed=true;
          if(this.readyState!=='open')break;
        }
        const control=this.controls.get(this.deliveredControl+1);
        if(this.readyState==='open'&&control&&control.fence<=this.deliveredInput){
          this.controls.delete(control.id);this.deliveredControl=control.id;
          this.onmessage?.({data:control.data});changed=true;
        }
      }
    }finally{this.draining=false;}
  }
  close(){
    if(this.readyState==='closed')return;
    this.readyState='closed';if(this.timer!==null)this.cancel(this.timer);this.timer=null;
    for(const lane of [this.control,this.fast])if(lane){lane.onopen=lane.onmessage=lane.onerror=lane.onclose=null;lane.close();}
    this.pending.clear();this.incoming.clear();this.received.clear();this.delivered.clear();this.controls.clear();this.early=[];this.pendingBytes=0;
    this.onclose?.();
  }
}

export function createInputChannel(pc,label){
  if(states.has(pc))throw Error('Input transport already exists');
  const channel=new InputTransport(label);states.set(pc,channel);
  channel.bind(pc.createDataChannel(label,{ordered:true}),false);
  channel.bind(pc.createDataChannel(label+'-fast',{ordered:false,maxRetransmits:0}),true);
  return channel;
}

export function acceptInputChannel(pc,raw,label,attach){
  if(raw.label!==label&&raw.label!==label+'-fast'){raw.close();throw Error('Unknown input transport lane');}
  let channel=states.get(pc);
  if(!channel){channel=new InputTransport(label);states.set(pc,channel);attach(channel);}
  channel.bind(raw,raw.label===label+'-fast');
  return channel;
}
