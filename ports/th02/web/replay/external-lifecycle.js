// Read the existing solo bridge and original game data only. This observer
// runs at an ordinary JSPI sleep; it never writes to DOS or steps the guest.
export function createSoloReplayLifecycle({game,patch},memory){
  const signature=new TextEncoder().encode(patch.signature);
  const mailboxOffset=patch.entries?.mailbox??patch.mailboxCsOffset??patch.mailbox_cs_offset;
  const dataDelta=((patch.dataSegment??patch.data_segment)-(patch.codeSegment??patch.code_segment))*16;
  const residentOffset={'02':0x27fc,'03':0x1d90,'04':0xba86}[game];
  const residentSignature=new TextEncoder().encode({'02':'MIKOConfig','03':'YUMEConfig','04':'HUMAConfig'}[game]);
  let at=-1,cursor=0,started=false,generation=-1,fatal=false,lastStage=0;
  function matches(h,address,bytes){return address>=0&&address+bytes.length<=h.length&&bytes.every((b,i)=>h[address+i]===b);}
  function mapping(h,v,address){
    if(!matches(h,address,signature)||address+32>h.length)return null;
    const data=address-mailboxOffset+dataDelta,ds=v.getUint16(address+20,true),base=data-ds*16;
    if(!ds||data<0||data+residentOffset+4>h.length)return null;
    const resident=base+v.getUint16(data+residentOffset+2,true)*16+v.getUint16(data+residentOffset,true);
    if(!matches(h,resident,residentSignature)||resident+80>h.length)return null;
    return {data,resident,mode:h[address+22],gen:v.getUint16(address+(game==='03'?28:24),true)};
  }
  return ()=>{
    const h=memory();if(!h)return null;
    const v=new DataView(h.buffer,h.byteOffset,h.byteLength);
    let mapped=mapping(h,v,at);
    // MAIN may be unloaded/reallocated between stages. Validate the original
    // resident pointer too, and keep looking while an old mailbox says exit.
    if(!mapped||mapped.mode===0){
      if(!mapped)at=-1;
      if(cursor>=h.length)cursor=0;
      const end=Math.min(h.length,cursor+1024*1024);
      const slice=h.subarray(cursor,Math.min(h.length,end+signature.length-1));
      for(let p=slice.indexOf(signature[0]);p>=0&&cursor+p<end;p=slice.indexOf(signature[0],p+1)){
        const candidate=mapping(h,v,cursor+p);
        if(candidate&&(!mapped||candidate.mode!==0)){at=cursor+p;mapped=candidate;if(candidate.mode!==0)break;}
      }
      cursor=end;
    }
    if(!mapped)return null;
    const {data,resident,mode,gen}=mapped;
    let active=false,paused=false,finish=null,stage=lastStage;
    if(game==='02'){
      if(h[resident+0x27])return null; // attract demo
      const currentStage=h[resident+11];if(currentStage!==0x7f)stage=currentStage;
      active=mode===1||mode===2;paused=mode===2;
      const miss=h[data+0xaa7],frame=h[data+0x2b99],lives=h[data+0xb2c];
      // lives counts spare lives. A respawn from one spare life reaches zero
      // at frame 43, and must not be confused with the final death.
      if(!miss||lives>0)fatal=false;
      if(active&&miss&&lives===0&&frame>0&&frame<24)fatal=true;
      if(started&&fatal&&miss&&lives===0&&frame===0)finish='game-over';
      if(started&&v.getUint16(resident+18,true)>0)finish='game-over';
      if(started&&mode===0)finish=finish||'end';
      if(started&&currentStage===0x7f)finish='clear';
    }else if(game==='04'){
      if(h[resident+62])return null;
      stage=h[data+0x5394];active=mode===1||mode===2;paused=mode===2;
      const lives=h[resident+11],miss=h[data+0x466a],quit=h[data+0x5392],sequence=h[resident+48];
      // 40..33 is the original eight-frame deathbomb window. A successful
      // bomb clears miss_time there and must not seal a living player's run.
      if(active&&miss>0&&miss<=32&&lives<=1)fatal=true;
      if(lives>1)fatal=false;
      if(started&&fatal&&!miss&&lives<=1)finish='game-over';
      if(started&&h[resident+29]>0)finish='game-over';
      if(started&&[0xfd,0xfe,0xff].includes(sequence))finish='clear';
      if(started&&quit===1)finish='quit';
      // quit=2 and mode=0 are normal cross-stage loading.
    }else{
      if(h[resident+0x39]||h[resident+0x0e]||!h[resident+0x0f]||![1,0x80].includes(h[resident+0x28]))return null;
      const story=h[resident+0x28]===1,defeat=h[data+0x38dc],quit=h[data+0x65a0];
      stage=h[resident+0x33];active=mode===1;
      // The existing pause hook publishes phase=0 without changing generation.
      // Round initialization changes generation; death/round results have
      // their own original flags. phase=0 alone does not mean paused.
      paused=started&&mode===0&&gen===generation&&!defeat&&!quit&&!h[data+0x65a6+0x1f]&&!h[data+0x65a6+0x80+0x1f];
      if(started&&quit)finish='quit';
      if(started&&defeat===2){
        if(story){
          if(h[resident+0x17]===1&&h[resident+0x34]===0)finish='game-over';
          if(h[resident+0x17]===0&&stage>=8)finish='clear';
        }else if(h[data+0x65a6+0x6c]>=2||h[data+0x65a6+0x80+0x6c]>=2)finish='end';
      }
      if(started&&story&&stage>=9)finish='clear';
    }
    if(active&&!started)started=true;
    if(active)generation=gen;
    lastStage=stage;
    return {active:started,paused,finish,stage};
  };
}
