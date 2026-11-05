// Receive order is independent of the viewer's playback cursor. A new match
// waits for the old confirmed tail unless the viewer explicitly chooses live.
const isBegin = packet => packet.length===74||packet.length===102;
export class SpectatorTape {
  constructor() { this.frame=0;this.round=0;this.ended=false;this.started=false;this.packets=[];this.head=0;this.frontier=0;this.bytes=0;this.begins=0;this.matches=[];this.current=null; }
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
    if(this.packets.length-this.frontier>=262144||this.bytes+packet.length>16*1024*1024)
      throw Error('TH09 待播放对局积压超限，请重新加入观战；玩家对局继续');
    const saved=packet.slice(),frame=new DataView(saved.buffer).getUint32(8,true);
    if(isBegin(saved)) {
      const previous=this.matches.at(-1);
      this.matches.push({index:this.packets.length,packet:saved,frame,offset:previous?previous.offset+previous.length:0,length:0,ended:false,rounds:[]});
      ++this.begins;
    } else {
      const match=this.matches.at(-1);
      if(match) {
        if(saved.length===46||saved.length===126)++match.length;
        else if(saved.length===24)match.ended=true;
        else if(saved.length===28)match.rounds.push({position:match.offset+frame-match.frame,round:new DataView(saved.buffer).getUint32(24,true)});
      }
    }
    this.packets.push(saved);this.bytes+=packet.length;
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
    if(isBegin(packet)){--this.begins;this.current=this.matches.find(match=>match.index===this.head)??null;}
    if(this.head>=this.frontier){this.bytes-=packet.length;this.frontier=this.head+1;}
    ++this.head;
    // Replay within the current match remains available. Starting the next
    // match releases the completed prefix instead of growing across games.
    if(isBegin(packet)&&this.current?.index>0){
      const count=this.current.index;
      this.packets.splice(0,count);this.head-=count;this.frontier=Math.max(0,this.frontier-count);
      this.matches=this.matches.filter(match=>match.index>=count);
      for(const match of this.matches)match.index-=count;
    }
  }
  timeline(played,delay=90) {
    const current=this.current,last=this.matches.at(-1);
    if(!current||!last)return null;
    const first=this.matches.find(match=>match.packet.length===102);
    const position=current.offset+Math.max(0,Math.min(current.length,played-current.frame));
    const edge=last.offset+(last.ended?last.length:Math.max(0,last.length-delay));
    return {min:first?Math.min(first.offset,position):position,position,edge,total:last.offset+last.length,
      marks:this.matches.flatMap((match,index)=>[{position:match.offset,kind:'match',active:match===current,label:`第 ${index+1} 局`},
        ...match.rounds.map(point=>({...point,kind:'round',label:`${point.round+1} 回合`}))])};
  }
  locate(position) {
    if(!Number.isFinite(position))return null;
    const match=this.matches.findLast(match=>match.offset<=position);
    if(!match||match.packet.length!==102||position>match.offset+match.length)return null;
    return {match,frame:match.frame+Math.floor(position-match.offset)};
  }
  restore(match) {
    while(this.frontier<match.index)this.bytes-=this.packets[this.frontier++].length;
    this.head=match.index;
    this.begins=this.matches.filter(item=>item.index>=this.head).length;
    this.shift();
  }
}
