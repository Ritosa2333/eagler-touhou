import {parseTh19StreamPacket,TH19_RECORDING_LIMITS} from '../../../common/contracts/th19-recording.mjs';
// Only authenticated P1 uploads this lane; spectators never send gameplay data.
export class Th19SpectatorHistory {
 constructor(){this.history=[];this.bytes=0;this.next=0;this.ended=false;this.config=null;this.terminal=false;}
 accept(bytes){
  const row=parseTh19StreamPacket(bytes);
  if(this.ended)throw Error('TH19 spectator stream already ended');
  if(row.kind==='begin'){
   if(this.config){if(JSON.stringify(this.config)===JSON.stringify(row.config))return null;throw Error('TH19 spectator initializer changed');}
   this.config=row.config;
  }else{
   if(!this.config||row.frame!==this.next)throw Error('TH19 spectator frame gap');
   if(row.kind==='frame'){if(this.terminal)throw Error('TH19 input after terminal result');++this.next;this.terminal=row.result===2;}else this.ended=true;
  }
  if(this.next>TH19_RECORDING_LIMITS.frames||this.bytes+bytes.length>12*1024*1024)throw Error('TH19 spectator history limit');
  const owned=Buffer.from(bytes);this.history.push(owned);this.bytes+=owned.length;return row;
 }
}
