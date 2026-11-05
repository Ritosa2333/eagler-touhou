// Read-only PMD event bridge. Reader state rolls back with the guest; Web Audio
// only receives copied events after the corresponding frame is confirmed.
export class NativeMusic {
  constructor(patch) {
    this.patch=patch;this.signature=new TextEncoder().encode(patch.signature);
    this.reset();
  }
  reset(){this.cursor=0;this.at=-1;this.sequence=0;this.ramBase=-1;}
  capture(){return {cursor:this.cursor,at:this.at,sequence:this.sequence,ramBase:this.ramBase};}
  restore(state){Object.assign(this,state);}
  valid(heap,at){
    if(at<0||at+20+64*8>heap.length||!this.signature.every((b,i)=>heap[at+i]===b))return false;
    const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength);
    const segment=view.getUint16(at+16,true);
    const base=at-this.patch.mailbox_com_offset-segment*16;
    // Static COM bytes in disk buffers have a zero segment. Require the live
    // INT 60h vector to point into this resident allocation as well.
    return segment>0&&base>=0&&base+0x100000<=heap.length
      &&view.getUint16(base+0x60*4+2,true)===segment
      &&view.getUint16(base+0x60*4,true)>=0x100
      &&view.getUint16(base+0x60*4,true)<this.patch.mailbox_com_offset;
  }
  dosRange(heap) {
    // The DOS allocation outlives the TSR observation bridge. Keep its base
    // through EXEC/uninstall so checksum and pause scans never use an empty
    // range or search unrelated host buffers after losing the music vector.
    if(this.valid(heap,this.at)){
      const segment=new DataView(heap.buffer,heap.byteOffset,heap.byteLength).getUint16(this.at+16,true);
      this.ramBase=this.at-this.patch.mailbox_com_offset-segment*16;
    }
    const start=this.ramBase;
    if(!Number.isSafeInteger(start)||start<0||start+0x100000>heap.length)return null;
    return {start,end:start+0xa0000};
  }
  read(heap,scanBytes=2*1024*1024){
    if(!this.valid(heap,this.at)){
      this.at=-1;
      const end=Math.min(heap.length,this.cursor+scanBytes);
      const slice=heap.subarray(this.cursor,Math.min(heap.length,end+this.signature.length));
      for(let i=slice.indexOf(this.signature[0]);i>=0&&this.cursor+i<end;i=slice.indexOf(this.signature[0],i+1)){
        const at=this.cursor+i;
        if(this.valid(heap,at)){this.at=at;this.sequence=0;break;}
      }
      this.cursor=end===heap.length?0:end;
    }
    if(this.at<0)return [];
    this.dosRange(heap);
    const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength);
    const latest=view.getUint16(this.at+18,true),events=[];
    let count=(latest-this.sequence)&65535;
    if(count>64){this.sequence=(latest-64)&65535;count=64;}
    for(let i=0;i<count;i++){
      const event=this.at+20+(this.sequence&63)*8,expected=(this.sequence+1)&65535;
      if(view.getUint16(event,true)!==expected)throw Error('本地音乐事件队列损坏');
      events.push([view.getUint16(event+2,true),view.getUint32(event+4,true)]);
      this.sequence=expected;
    }
    return events;
  }
}

export class ConfirmedMusic {
  constructor(player){this.player=player;this.frames=new Map();this.confirmed=0;}
  stage(frame,events){if(frame>=this.confirmed&&events.length)this.frames.set(frame,events);}
  rewind(frame){for(const key of this.frames.keys())if(key>=frame)this.frames.delete(key);}
  confirm(prefix){
    for(const [frame,events] of this.frames)if(frame<prefix){
      for(const [ax,hash] of events)this.player.command(ax,hash);
      this.frames.delete(frame);
    }
    this.confirmed=Math.max(this.confirmed,prefix);
  }
}
