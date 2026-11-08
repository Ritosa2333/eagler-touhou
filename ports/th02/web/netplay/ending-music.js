// MAINE and OP load original PMD songs into DOS conventional memory after
// MAIN's music mailbox has been released. Track exact song headers there.
export function endingMusicSignatures(tracks){
  return [...tracks.values()].filter(track=>/^(end1|ending|op)\.m$/i.test(track.name)&&track.bytes.length>=32)
    .map(track=>({name:track.name.slice(0,-2).toLowerCase(),bytes:track.bytes.slice(0,32)}));
}
export function scanEndingMusic(heap,base,signatures,previous=[]){
  const found=[];
  for(const {name,bytes} of signatures){
    // PMD headers start with zero, which is common throughout unused memory.
    // Anchor on a nonzero byte so scanning never visits every empty byte.
    const high=bytes.findIndex(value=>value>=128),nonzero=bytes.findIndex(value=>value!==0),anchor=high>=0?high:Math.max(0,nonzero);
    for(let hit=heap.indexOf(bytes[anchor],base+anchor);hit>=0&&hit-anchor+bytes.length<=base+0xa0000;hit=heap.indexOf(bytes[anchor],hit+1)){
      const at=hit-anchor;if(bytes.every((value,i)=>heap[at+i]===value)){found.push(`${name}:${at-base}`);break;}
    }
  }
  const added=found.filter(value=>!previous.includes(value));
  return {found,name:added.length===1?added[0].split(':')[0]:null};
}
