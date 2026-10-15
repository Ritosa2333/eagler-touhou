// Receive order is independent of the viewer's playback cursor. A new match
// waits for the old confirmed tail unless the viewer explicitly chooses live.
const isBegin = packet => packet.length===74||packet.length===102;
export class SpectatorTape {
  constructor() { this.frame=0;this.round=0;this.ended=false;this.started=false;this.packets=[];this.head=0;this.bytes=0;this.begins=0; }
  begin(frame) {
    if(frame<this.frame || (this.started&&!this.ended))throw Error('TH09 新局缺少上一局结束或起点回退');
    this.frame=frame;this.round=0;this.started=true;this.ended=false;
  }
  input(frame) {
    if(this.ended||frame!==this.frame)throw Error('TH09 观战帧序号不连续');
    this.started=true;++this.frame;
  }
  end(frame) {
    if(this.ended||frame!==this.frame)throw Error('TH09 观战结束帧无效');
    this.ended=true;
  }
  roundStart(frame,round) {
    if(this.ended||frame!==this.frame||round<=this.round||round>1024)
      throw Error('TH09 回合刻度无效');
    this.round=round;
  }
  append(packet) {
    if(this.packets.length-this.head>=32404||this.bytes+packet.length>2*1024*1024)
      throw Error('TH09 待播放对局积压超限，请重新加入观战；玩家对局继续');
    this.packets.push(packet.slice());this.bytes+=packet.length;if(isBegin(packet))++this.begins;
  }
  peek() { return this.packets[this.head]; }
  advanceToLatestBegin() {
    // Only an explicit go-live action may discard unwatched matches.
    let discard=-1;
    for(let i=this.head;i<this.packets.length;++i)
      if(isBegin(this.packets[i]))discard=i-this.head;
    if(discard<0)return false;
    while(discard-->0)this.shift();
    return true;
  }
  shift() {
    const packet=this.peek();if(!packet)return;
    this.bytes-=packet.length;if(isBegin(packet))--this.begins;
    this.packets[this.head++]=null;
    if(this.head===this.packets.length){this.packets=[];this.head=0;}
    else if(this.head>=4096){this.packets=this.packets.slice(this.head);this.head=0;}
  }
}
