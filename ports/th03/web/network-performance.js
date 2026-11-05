// Bounded local history: no pixel readback, uploads, room tokens or addresses.
export function createNetworkHistory(limit=600){
  const samples=[];
  return {push(sample){samples.push(structuredClone(sample));if(samples.length>limit)samples.splice(0,samples.length-limit);},read(){return structuredClone(samples);}};
}
// Observational counters only: native values wrap at uint32 and rewind with
// snapshots. They are not a wall-clock throughput or precise fast-path ratio.
export function readNativeCoreDiagnostics(module){
  if(!module)return null;
  const api=module.nativeExports?.(),pointer=api?.thCpuState?.(0);
  if(!pointer||pointer+16>module.HEAPU8.byteLength)return null;
  const view=new DataView(module.HEAPU8.buffer,module.HEAPU8.byteOffset,module.HEAPU8.byteLength);
  return {schedulerMode:module.nativeScheduler?.mode()??null,cpuEnabled:!!view.getUint32(pointer,true),
    fastInstructionsRaw:view.getUint32(pointer+4,true),instructionFallbacksRaw:view.getUint32(pointer+8,true),
    counterSemantics:'uint32, rollbackable; includes fused wait instructions; excludes whole-dispatch fallbacks',
    planarEnabled:module.nativePlanar===true,writeJournal:!!module.thDirty};
}
export function mountNetworkPerformance({host,toolbar,getReport}){
  const save=()=>{
    const report=getReport(),url=URL.createObjectURL(new Blob([JSON.stringify(report,null,2)+'\n'],{type:'application/json'}));
    const link=document.createElement('a');link.href=url;link.download=`th03-network-${report.seat??'waiting'}-${Date.now()}.json`;link.click();setTimeout(()=>URL.revokeObjectURL(url),10000);
  };
  for(const target of [host.querySelector('.player-toolbar'),toolbar].filter(Boolean)){
    const button=document.createElement('button');button.type='button';button.textContent='日志';button.title='下载最近 10 分钟的联机运行日志';button.dataset.networkReport='';button.onclick=save;target.append(button);
  }
}
