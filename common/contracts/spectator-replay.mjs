export const SPECTATOR_REPLAY_LIMITS = Object.freeze({bytes:8*1024*1024,chunk:32768,header:20});
export function parseSpectatorReplayChunk(wire){
 const {bytes,chunk,header}=SPECTATOR_REPLAY_LIMITS;
 if(!(wire instanceof Uint8Array)||wire.length<=header||wire.length>header+chunk||wire[0]!==0xe9||wire[1]!==1||wire[2]>1||wire[3])return null;
 const v=new DataView(wire.buffer,wire.byteOffset,wire.byteLength),total=v.getUint32(12,true),offset=v.getUint32(16,true);
 const session=v.getUint32(8,true).toString(16).padStart(8,'0')+v.getUint32(4,true).toString(16).padStart(8,'0');
 if(/^0+$/.test(session)||!total||total>bytes||offset>total||wire.length-header>total-offset)return null;
 return {session,total,offset,extended:wire[2]===1,data:wire.subarray(header)};
}
export class SpectatorReplayTransfer {
 constructor(session){this.session=session;this.received=0;this.bytes=null;this.complete=false;this.extended=false;}
 accept(wire){
  const part=parseSpectatorReplayChunk(wire);
  if(!part||part.session!==this.session||this.complete||part.offset!==this.received)return false;
  if(!this.bytes){if(part.offset)return false;this.bytes=new Uint8Array(part.total);this.extended=part.extended;}
  if(part.total!==this.bytes.length||part.extended!==this.extended)return false;
  this.bytes.set(part.data,part.offset);this.received+=part.data.length;
  this.complete=this.received===this.bytes.length;return true;
 }
 *chunks(){
  if(!this.complete)return;
  for(let offset=0;offset<this.bytes.length;offset+=SPECTATOR_REPLAY_LIMITS.chunk){
   const data=this.bytes.subarray(offset,offset+SPECTATOR_REPLAY_LIMITS.chunk),wire=new Uint8Array(20+data.length),v=new DataView(wire.buffer);
   wire.set([0xe9,1,Number(this.extended),0]);v.setUint32(4,parseInt(this.session.slice(8),16),true);v.setUint32(8,parseInt(this.session.slice(0,8),16),true);
   v.setUint32(12,this.bytes.length,true);v.setUint32(16,offset,true);wire.set(data,20);yield wire;
  }
 }
}
