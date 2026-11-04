// PC-98 spectator batches carry an immutable, contiguous confirmed prefix.
// Compression and the game-specific row validation remain in the browser.
const MAX_FRAMES=216000,MAX_BYTES=48*1024*1024,MAX_PACKET=16384;
const products=new Set(['th01mp','th02mp','th03mp','th04mp']);
export function isPc98SpectatorRoom(room){return products.has(room.split('-')[0]);}
export class Pc98SpectatorRelay{
  constructor(players){this.players=players;this.history=[];this.bytes=0;this.next=0;this.started=false;this.ended=false;}
  accept(payload){
    if(!(payload instanceof Uint8Array)||payload.length<16||payload.length>MAX_PACKET||
       payload[0]!==0x50||payload[1]!==0x39||payload[2]!==0x38||payload[3]!==0x53||payload[4]!==1||payload[6]!==this.players||payload[7])throw Error('invalid PC98 spectator packet');
    const view=new DataView(payload.buffer,payload.byteOffset,payload.byteLength),kind=payload[5],first=view.getUint32(8,true),count=view.getUint32(12,true);
    if(this.ended||first!==this.next)throw Error('PC98 spectator history gap');
    if(kind===1){if(this.started||first||count||payload.length===16)throw Error('invalid PC98 spectator opening');this.started=true;}
    else if(kind===2){if(!this.started||count<1||count>60||first+count>MAX_FRAMES||payload.length===16)throw Error('invalid PC98 spectator batch');this.next+=count;}
    else if(kind===3){if(!this.started||count||payload.length!==16)throw Error('invalid PC98 spectator end');this.ended=true;}
    else throw Error('invalid PC98 spectator event');
    if(this.bytes+payload.length>MAX_BYTES)throw Error('PC98 spectator history full');
    const bytes=Buffer.from(payload);this.history.push(bytes);this.bytes+=bytes.length;return kind;
  }
}
