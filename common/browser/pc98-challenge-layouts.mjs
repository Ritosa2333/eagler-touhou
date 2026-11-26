// Read and write only the live cooperative resource slots; never alter a disk
// or another seat's local input. The resource table lives in the cooperative mailbox.
export function challengeStockLayout(game, heap, mailbox, patch, resident=-1){
  if(!(heap instanceof Uint8Array)||!Number.isInteger(mailbox)||mailbox<0)throw Error('挑战模式内存无效');
  if(game!=='04')throw Error('挑战模式作品无效');
  const spec=patch.resource_schema;
  if(!spec||!Number.isInteger(spec.offset)||!Number.isInteger(spec.stride)||spec.stride<7||resident<0)throw Error('TH04 资源表无效');
  const addresses=Array.from({length:3},(_,i)=>({lives:mailbox+spec.offset+i*spec.stride+4,bombs:mailbox+spec.offset+i*spec.stride+5}));
  if(addresses.some(s=>!Number.isSafeInteger(s.lives)||!Number.isSafeInteger(s.bombs)||s.lives<0||s.bombs<0||s.lives>=heap.length||s.bombs>=heap.length)||resident+13>=heap.length)throw Error('挑战模式资源地址越界');
  return {
    read(players){if(!Number.isInteger(players)||players<2||players>3)throw Error('挑战模式人数无效');return addresses.slice(0,players).map(s=>({lives:heap[s.lives],bombs:heap[s.bombs]}));},
    write(stocks,offlineMask=0,mark=()=>{}){
      if(!Array.isArray(stocks)||stocks.length<2||stocks.length>3||stocks.some(s=>!s||!Number.isInteger(s.lives)||s.lives<1||s.lives>255||!Number.isInteger(s.bombs)||s.bombs<0||s.bombs>255))throw Error('挑战模式资源无效');
      stocks.forEach((s,i)=>{if(offlineMask&(1<<i))return;const a=addresses[i];mark(a.lives,1);mark(a.bombs,1);heap[a.lives]=s.lives;heap[a.bombs]=s.bombs;
        if(i===0){mark(resident+11,1);mark(resident+13,1);heap[resident+11]=s.lives;heap[resident+13]=s.bombs;}});
    },
  };
}

export function writeChallengeHud(heap,mailbox,mailboxOffset,countsOffset,counts,mark=()=>{}){
  const at=mailbox-mailboxOffset+countsOffset;
  if(!(heap instanceof Uint8Array)||!Number.isInteger(at)||at<0||at+12>heap.length||!Array.isArray(counts)||counts.length<2||counts.length>3||counts.some(n=>!Number.isInteger(n)||n<-2147483648||n>2147483647))throw Error('挑战模式 HUD 状态无效');
  mark(at,12);const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength);
  for(let i=0;i<3;i++)view.setInt32(at+i*4,counts[i]??0,true);
}
