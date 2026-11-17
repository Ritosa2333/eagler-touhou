// On-demand only: STOP and invalid opcodes share ZUNINIT's INT 06h handler.
// Validate both pinned handlers and the IVT before reading the resident flag.
export function readZunWarning(heap){
  if(!(heap instanceof Uint8Array))return null;
  const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength);
  const stop=[0x2e,0x80,0x3e,0x16,2,0,0x75,9,0x2e,0xc6,6,0x16,2,1,0xe8,0x13,0,0xcf];
  const copy=[...stop];copy[13]=2;copy[15]=1;
  for(let at=heap.indexOf(0x15);at>=0;at=heap.indexOf(0x15,at+1)){
    if(at<20||at+8>heap.length||heap[at+1]!==1||heap[at+4]!==3||heap[at+5]!==1)continue;
    const segment=view.getUint16(at+2,true),base=at-20,resident=base+segment*16;
    if(!segment||segment>=0xa000||segment!==view.getUint16(at+6,true)||base+0xa0000>heap.length)continue;
    if(!stop.every((b,i)=>heap[resident+0x103+i]===b)||!copy.every((b,i)=>heap[resident+0x115+i]===b))continue;
    const active=heap[resident+0x216],frames=[];
    if(active===1||active===2){
      // No CPU SP export exists. Report possible saved interrupt frames only.
      const expectedReturn=active===1?0x114:0x126;
      for(let pos=base;pos<base+0xa0000-8&&frames.length<8;pos+=2){
        if(view.getUint16(pos,true)!==expectedReturn)continue;
        const ip=view.getUint16(pos+2,true),cs=view.getUint16(pos+4,true),flags=view.getUint16(pos+6,true),linear=cs*16+ip;
        if(cs<0x100||cs>=0xa000||linear>=0xa0000||!(flags&2)||(flags&0x8028))continue;
        frames.push({stackLinear:pos-base,ip,cs,flags,code:Array.from(heap.subarray(base+linear,base+Math.min(linear+32,0xa0000)))});
      }
    }
    return {residentSegment:segment,active,kind:active===0?'idle':active===1?'STOP-or-invalid-opcode':active===2?'COPY':'unknown',
      possibleInterruptFrames:frames,frameCaveat:'Memory candidates only; no live CPU SP export. STOP and invalid opcode share INT 06h.'};
  }
  return null;
}

export function downloadFaultReport(report,filename='th04-solo-fault.json'){
  const url=URL.createObjectURL(new Blob([JSON.stringify(report,null,2)+'\n'],{type:'application/json'}));
  const a=document.createElement('a');a.href=url;a.download=filename;a.click();setTimeout(()=>URL.revokeObjectURL(url),10000);
}

// On-demand, read-only CPU evidence, including after the MAIN mailbox is gone.
export function readSoloFault(heap,module){
  if(module?.mobileRuntime?.core==='dosbox-x'){
    const text=pointer=>pointer?module.UTF8ToString(pointer):'';
    return {core:'DOSBox-X',program:text(module._emGetRunningProgram?.()),metrics:text(module._getCPUMetrics?.()),
      heapBytes:heap?.length??null,virtualNow:module.virtualNow??null,continuation:module.mobileRuntime.continuation,
      sleepCount:module.sleep_count??null,sleepTime:module.sleep_time??null,render:module.framePresenter?.info()??null,
      nativeWarning:readZunWarning(heap)};
  }
  if(!(heap instanceof Uint8Array)||heap.length<4590280)return null;
  const v=new DataView(heap.buffer,heap.byteOffset,heap.byteLength);
  const ip=v.getUint32(4590048,true),cs=v.getUint32(4590192,true),linear=cs+ip;
  // Direct bytes are reliable only in the ordinary guest RAM window. Extended
  // memory and paging may use a different physical backing; keep registers.
  const code=linear<0xa0000?Array.from(heap.subarray(2492848+linear,2492848+Math.min(linear+32,0xa0000))):null;
  return {ip,previousIp:v.getUint32(4590052,true),csBase:cs,dsBase:v.getUint32(4590240,true),
    flags:v.getUint32(4590044,true),registerBytes:Array.from(heap.subarray(4590000,4590280)),
    linearCode:code,codeAssumption:'ordinary guest RAM, before any paging translation'};
}

export function mountSoloFault({host,getEmulator,readState,readContext}){
  const button=document.createElement('button');button.type='button';button.dataset.faultDownload='';button.textContent='下载故障信息';
  const output=document.createElement('p');output.setAttribute('role','status');output.hidden=true;output.style.fontSize='11px';
  host.querySelector('.player-toolbar').append(button);button.after(output);
  const errors=[];
  const remember=message=>{if(errors.length===8)errors.shift();errors.push({at:new Date().toISOString(),message:String(message).slice(0,4000)});};
  window.addEventListener('error',event=>remember(event.error?.stack||event.message));
  window.addEventListener('unhandledrejection',event=>remember(event.reason?.stack||event.reason));
  button.onclick=async()=>{
    const emulator=getEmulator();if(!emulator){output.hidden=false;output.textContent='请先启动游戏。';return;}
    button.disabled=true;
    try{
      let cpu=null,captureError=null;
      if(emulator.isWorker){
        if(!emulator.closed)try{cpu=await emulator.call('soloFault');emulator.lastDiagnostics=cpu;}catch(error){captureError=error.message;}
        cpu??=emulator.lastDiagnostics??null;
      }else cpu=readSoloFault(emulator.module.HEAPU8,emulator.module);
      let game=null;try{game=readState();}catch(error){captureError??=error.message;}
      const report={schema:'touhou-solo-fault/1',recordedAt:new Date().toISOString(),
        environment:{userAgent:navigator.userAgent,visibility:document.visibilityState},context:readContext(),
        emulator:{state:emulator.state,thread:emulator.isWorker?'worker':'main',wasm:emulator.module.mobileRuntime,
          cpuCore:emulator.module.mobileRuntime?.cpuCore,
          frames:emulator.module.localFrameCount??null,uploads:emulator.module.frameUploads??null,
          render:emulator.module.framePresenter?.info()??emulator.module.renderMetrics??null,
          error:emulator.failedError?.message??null,captureError},game,cpu,errors:[...errors]};
      downloadFaultReport(report);
      output.hidden=false;output.textContent='故障信息已下载，只保存在本机。黑屏或紫屏后可用此文件排查。';
    }catch(error){output.hidden=false;output.textContent='读取故障信息失败：'+error.message;}
    finally{button.disabled=false;}
  };
}
