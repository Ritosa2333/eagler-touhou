// Read-only PMD event bridge. Reader state rolls back with the guest; Web Audio
// only receives copied events after the corresponding frame is confirmed.
export class NativeMusic {
  constructor(patch) {
    this.patch=patch;this.signature=new TextEncoder().encode(patch.signature);
    this.reset();
  }
  reset(){this.cursor=0;this.at=-1;this.sequence=0;this.track=null;}
  capture(){return {cursor:this.cursor,at:this.at,sequence:this.sequence,track:this.track};}
  restore(state){Object.assign(this,state);}
  valid(heap,at){
    if(this.patch.kind==='th04-native')return at>=0&&at+675<=heap.length
      &&this.signature.every((b,i)=>heap[at+i]===b)&&heap[at+25]===1
      &&new DataView(heap.buffer,heap.byteOffset,heap.byteLength).getUint16(at+20,true)>0
      &&new DataView(heap.buffer,heap.byteOffset,heap.byteLength).getUint32(at+16,true)>0;
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
  read(heap){
    if(this.patch.kind==='th04-native')return this.readCoop(heap);
    if(!this.valid(heap,this.at)){
      this.at=-1;
      const end=Math.min(heap.length,this.cursor+2*1024*1024);
      const slice=heap.subarray(this.cursor,Math.min(heap.length,end+this.signature.length));
      for(let i=slice.indexOf(this.signature[0]);i>=0&&this.cursor+i<end;i=slice.indexOf(this.signature[0],i+1)){
        const at=this.cursor+i;
        if(this.valid(heap,at)){this.at=at;this.sequence=0;break;}
      }
      this.cursor=end===heap.length?0:end;
    }
    if(this.at<0)return [];
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
  readCoop(heap){
    if(!this.valid(heap,this.at)){
      this.at=-1;
      const end=Math.min(heap.length,this.cursor+2*1024*1024),slice=heap.subarray(this.cursor,Math.min(heap.length,end+this.signature.length));
      for(let i=slice.indexOf(this.signature[0]);i>=0&&this.cursor+i<end;i=slice.indexOf(this.signature[0],i+1)){
        if(this.valid(heap,this.cursor+i)){this.at=this.cursor+i;break;}
      }
      this.cursor=end===heap.length?0:end;
    }
    if(this.at<0)return [];
    const p=this.patch,o=p.observer,d=this.at-p.mailbox_cs_offset+(p.data_segment-p.code_segment)*16;
    if(d<0||d+65536>heap.length)throw Error('合作音乐内存布局不匹配');
    if(heap[d+o.bgm_mode]!==0)throw Error('合作游戏原生 BGM 未关闭');
    const v=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),sequence=v.getUint16(d+o.sequence,true),ax=v.getUint16(d+o.command,true);
    let filename='';for(let i=0;i<13&&heap[d+o.filename+i];i++)filename+=String.fromCharCode(heap[d+o.filename+i]);
    const name=/^(st\d\d[bc]?|end[12]|staff|name|op|logo)\./.exec(filename.toLowerCase())?.[1];
    const changed=sequence!==this.sequence,events=[];
    if(name){const hash=p.trackHashes[name];if(hash===undefined)throw Error('缺少合作音乐：'+name);
      if(name!==this.track){events.push([0,hash]);if(changed&&(ax>>>8)!==0)events.push([ax,hash]);}
      else if(changed)events.push([ax,hash]);
      this.track=name;
    }else if(changed&&this.track)events.push([ax,p.trackHashes[this.track]]);
    this.sequence=sequence;return events;
  }
}

export class ConfirmedMusic {
  constructor(player){this.player=player;this.frames=new Map();}
  stage(frame,events){if(events.length)this.frames.set(frame,events);}
  rewind(frame){for(const key of this.frames.keys())if(key>=frame)this.frames.delete(key);}
  confirm(prefix){
    for(const [frame,events] of this.frames)if(frame<prefix){
      for(const [ax,hash] of events)this.player.command(ax,hash);
      this.frames.delete(frame);
    }
  }
}
