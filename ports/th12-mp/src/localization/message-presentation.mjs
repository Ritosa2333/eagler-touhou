// Local text surfaces indexed by the original MSG program counter. The core
// always runs retail bytes, so language and ruby layout never enter netplay.
const textOps=new Set([15,16,17]);
const toHex=bytes=>Array.from(bytes,b=>b.toString(16).padStart(2,'0')).join('');
function decrypt(bytes){
  const out=[];let key=0x77,step=7;
  for(const byte of bytes){const value=byte^key;key=(key+step)&255;step=(step+16)&255;if(!value)break;out.push(value);}
  return Uint8Array.from(out);
}
function commands(data,start,end){
  const view=new DataView(data.buffer,data.byteOffset,data.byteLength),out=[];
  for(let at=start;at+4<=end;){
    const size=4+data[at+3],op=data[at+2];
    if(at+size>end)throw Error('原版对话指令范围无效');
    out.push({at,next:at+size,time:view.getUint16(at,true),op,raw:textOps.has(op)?decrypt(data.subarray(at+4,at+size)):null});
    at+=size;if(op===0)break;
  }
  return out;
}
export function messagePresentation(data,patch){
  const view=new DataView(data.buffer,data.byteOffset,data.byteLength),count=view.getUint32(0,true),entries=[];
  for(let entry=0;entry<count;entry++){
    const start=view.getUint32(4+8*entry,true),end=entry+1<count?view.getUint32(12+8*entry,true):data.length;
    const source=commands(data,start,end),byOffset=new Map(source.map(c=>[c.at,c])),replacements=new Map();
    let previous=start-1;
    for(const anchor of patch.entries[entry].anchors){
      const group=source.filter(c=>textOps.has(c.op)&&c.at<anchor.offset&&c.at>previous);
      // Text commands between control anchors execute atomically per timestamp.
      // Assign each translated group after its final original instruction.
      for(const record of anchor.texts){
        const sameTime=group.filter(c=>c.time===record.time),last=sameTime.at(-1);
        if(!last)throw Error('语言对话文字时间与原版文字组不匹配');
        const list=replacements.get(last.at)??[];
        list.push({...record,raw:record.originalOffset===undefined
          ?Uint8Array.from(record.utf8Hex.match(/../g)??[],v=>parseInt(v,16)):byOffset.get(record.originalOffset).raw});
        replacements.set(last.at,list);
      }
      for(const command of group)if(!replacements.has(command.at))replacements.set(command.at,[]);
      previous=anchor.offset;
    }
    let line=0,initialized=0,speaker=0,style=0,records=Array.from({length:4},()=>({textHex:''}));
    const states=new Map(),save=pc=>states.set(pc,records.map(r=>({...r})));
    const write=(slot,raw,offset=0,spacing=0,fontStyle=style)=>{
      records[slot]={textHex:toHex(raw),offset,spacing,style:fontStyle,color:[0xf8f08f,0x8088ff,0xd8d8d8][speaker]};
    };
    save(start);
    for(const command of source){
      if(textOps.has(command.op)){
        for(const text of replacements.get(command.at)??[]){
          if(text.op!==17){write(text.op-15,text.raw);continue;}
          if(!line&&!initialized){records=Array.from({length:4},()=>({textHex:''}));initialized=1;}
          if(text.raw[0]===124){
            const first=text.raw.indexOf(44),second=text.raw.indexOf(44,first+1);
            const prefix=new TextDecoder().decode(text.raw.subarray(0,second));
            const [,offset,spacing]=/^\|(-?\d+),(-?\d+)$/.exec(prefix)??[];
            if(offset===undefined)throw Error('语言对话排版无效');
            write(line+2,text.raw.subarray(second+1),Number(offset),Number(spacing),2);
          }else{write(line,text.raw);if(!line)line=1;else line=initialized=0;}
        }
      }else if(command.op>=7&&command.op<=9){speaker=command.op-7;style=+(command.op===9);line=initialized=0;}
      else if(command.op===26)style=1;
      else if(command.op===11)line=initialized=0;
      save(command.op===0?command.at:command.next);
    }
    entries.push(states);
  }
  return message=>{
    const records=entries[message?.entry]?.get(message.pc);
    return records?{...message,textRecords:records,localizedPresentation:true}:message;
  };
}
