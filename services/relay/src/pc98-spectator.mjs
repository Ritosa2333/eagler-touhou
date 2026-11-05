// PC-98 spectator batches carry an immutable, contiguous confirmed prefix.
// Compression and the game-specific row validation remain in the browser.
import {createHash} from 'node:crypto';
const MAX_FRAMES=216000,MAX_BYTES=128*1024*1024,MAX_PACKET=16384,MAX_POINT=16*1024*1024;
const products=new Set(['th01mp','th02mp','th03mp','th04mp']);
export function isPc98SpectatorRoom(room){return products.has(room.split('-')[0]);}
export class Pc98SpectatorRelay{
  constructor(players){this.players=players;this.history=[];this.bytes=0;this.next=0;this.started=false;this.ended=false;this.point=null;this.lastPoint=-1;}
  accept(payload){
    if(!(payload instanceof Uint8Array)||payload.length<16||payload.length>MAX_PACKET||
       payload[0]!==0x50||payload[1]!==0x39||payload[2]!==0x38||payload[3]!==0x53||payload[4]!==1||payload[6]!==this.players||payload[7])throw Error('invalid PC98 spectator packet');
    const view=new DataView(payload.buffer,payload.byteOffset,payload.byteLength),kind=payload[5],first=view.getUint32(8,true),count=view.getUint32(12,true);
    if(this.ended||(kind!==4&&first!==this.next))throw Error('PC98 spectator history gap');
    if(kind===1){if(this.started||first||count||payload.length===16)throw Error('invalid PC98 spectator opening');this.started=true;}
    else if(kind===2){if(!this.started||count<1||count>60||first+count>MAX_FRAMES||payload.length===16)throw Error('invalid PC98 spectator batch');this.next+=count;}
    else if(kind===3){if(!this.started||this.point||count||payload.length!==16)throw Error('invalid PC98 spectator end');this.ended=true;}
    else if(kind===4){
      if(!this.started||first>this.next||count||payload.length<=88)throw Error('invalid PC98 spectator checkpoint');
      const size=view.getUint32(16,true),offset=view.getUint32(20,true),sha=Buffer.from(payload.subarray(24,88)).toString('ascii'),chunk=payload.subarray(88);
      if(size<1||size>MAX_POINT||!/^[a-f0-9]{64}$/.test(sha)||offset+chunk.length>size)throw Error('invalid PC98 checkpoint metadata');
      if(!this.point){if(offset||first<=this.lastPoint||this.lastPoint<0&&first!==0)throw Error('invalid PC98 checkpoint order');this.point={frame:first,size,sha,offset:0,hash:createHash('sha256')};}
      const point=this.point;if(point.frame!==first||point.size!==size||point.sha!==sha||point.offset!==offset)throw Error('PC98 checkpoint chunk gap');
      point.hash.update(chunk);point.offset+=chunk.length;
      if(point.offset===size){if(point.hash.digest('hex')!==sha)throw Error('PC98 checkpoint digest mismatch');this.lastPoint=first;this.point=null;}
    }
    else throw Error('invalid PC98 spectator event');
    if(this.bytes+payload.length>MAX_BYTES)throw Error('PC98 spectator history full');
    const bytes=Buffer.from(payload);this.history.push(bytes);this.bytes+=bytes.length;return kind;
  }
  bootstrap(){const p=Buffer.alloc(16);p.set([0x50,0x39,0x38,0x53,1,5,this.players,0]);p.writeUInt32LE(this.next,8);return p;}
}
