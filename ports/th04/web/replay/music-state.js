// Presentation state belongs to the independent replay instance. No sound effects
// or audio-device clocks are serialized; a restored track starts at its beginning.
export function createMusicState(type='music'){
  let name='',start=null,controls=[];
  const event=(ax,key)=>type==='music'?{type,event:{ax,name:key}}:{type,event:[ax,key]};
  function observe(value){
    const ax=type==='music'?value.ax:value[0],key=type==='music'?value.name:value[1];
    if(!Number.isInteger(ax)||ax<0||ax>65535)throw Error('音乐状态命令无效');
    if(type==='music'&&key&&key!==name){name=key;start=event(0,name);controls=[];}
    const command=ax>>>8;
    if(command===0){start=event(ax,type==='music'?name:key);controls=[];}
    else if(command===1){start=null;controls=[];}
    else if(start&&command===0x19)controls=[event(ax,type==='music'?name:key)];
    else if(start&&command===2){controls=controls.filter(e=>((type==='music'?e.event.ax:e.event[0])>>>8)!==2);controls.push(event(ax,type==='music'?name:key));}
  }
  function capture(){return structuredClone({version:1,type,name,start,controls});}
  function restore(saved){
    if(saved?.version!==1||saved.type!==type||typeof saved.name!=='string'||saved.name.length>128||!Array.isArray(saved.controls)||saved.controls.length>2)throw Error('音乐检查点无效');
    const events=[...(saved.start?[saved.start]:[]),...saved.controls];
    for(const e of events){
      const ax=type==='music'?e.event?.ax:e.event?.[0],key=type==='music'?e.event?.name:e.event?.[1];
      if(e.type!==type||!Number.isInteger(ax)||ax<0||ax>65535||(type==='music'?typeof key!=='string'||key.length>128:!Number.isInteger(key)||key<0||key>0xffffffff))throw Error('音乐检查点事件无效');
    }
    ({name,start,controls}=structuredClone(saved));
    return events;
  }
  return {observe,capture,restore};
}
