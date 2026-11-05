// Opt-in, bounded local recording. No uploads, timers or per-frame work while idle.
export function summarize(values){
  if(!values.length)return {count:0,meanMs:null,p95Ms:null,maxMs:null,over50ms:0};
  const sorted=[...values].sort((a,b)=>a-b);
  return {count:values.length,meanMs:values.reduce((a,b)=>a+b,0)/values.length,
    p95Ms:sorted[Math.min(sorted.length-1,Math.ceil(sorted.length*.95)-1)],
    maxMs:sorted.at(-1),over50ms:values.filter(v=>v>50).length};
}

export function mountSoloPerformance({host,game,getEmulator,readState}){
  const bar=host.querySelector('.player-toolbar');
  const record=document.createElement('button');record.type='button';record.dataset.perfRecord='';record.textContent='记录性能（15 秒）';
  const download=document.createElement('button');download.type='button';download.dataset.perfDownload='';download.textContent='下载性能报告';download.hidden=true;
  const output=document.createElement('pre');output.className='player-diagnostics';output.dataset.perfResult='';output.hidden=true;output.setAttribute('role','status');
  // Keep results in the settings scroll area, clear of the game's touch surface.
  output.style.cssText='position:static;max-height:180px;margin-top:8px';
  bar.append(record,download);bar.after(output);
  let active=null,report=null;
  const show=text=>{output.hidden=false;output.textContent=text;};
  const round=v=>v==null?'未取得':v.toFixed(1);
  function finish(reason){
    const run=active;if(!run)return;
    active=null;cancelAnimationFrame(run.raf);clearTimeout(run.timeout);
    document.removeEventListener('visibilitychange',run.visibility);
    if(run.module.observeLocalFrame===run.observe)delete run.module.observeLocalFrame;
    const now=performance.now(),elapsed=now-run.start,final=readState(),config=run.emulator.config;
    const native=summarize(run.costs),raf=summarize(run.intervals);
    const stable=run.stable&&final?.playing&&final.generation===run.state?.generation&&final.ticks>=run.state?.ticks;
    report={schema:'touhou-solo-performance/1',build:'20261013-th03-dosbox-x-v1',game,recordedAt:new Date().toISOString(),
      completion:reason,durationMs:elapsed,environment:{userAgent:navigator.userAgent,
        hardwareConcurrency:navigator.hardwareConcurrency||null,deviceMemoryGiB:navigator.deviceMemory||null,
        devicePixelRatio,viewport:{width:innerWidth,height:innerHeight},screen:{width:screen.width,height:screen.height},
        crossOriginIsolated:globalThis.crossOriginIsolated===true},
      emulator:{executionThread:run.module.executionThread||'main',core:run.module.mobileRuntime?.core,cpuCore:'simple',cycles:'max 100%',sampleHz:config.SampleHz,
        soundBoard:config.SNDboard,heapMiB:(run.module.heapBytes??run.module.HEAPU8?.length)/1048576,
        audioState:run.module.SDL2?.audioContext?.state||null,audioSampleHz:run.module.SDL2?.audioContext?.sampleRate||null},
      nativeCallbacks:{...native,hz:run.costs.length*1000/elapsed,
        busyPercent:run.costs.reduce((a,b)=>a+b,0)*100/elapsed},
      animationFrames:{...raf,hz:run.intervals.length?run.intervals.length*1000/run.intervals.reduce((a,b)=>a+b,0):null},
      game:{start:run.state,end:final,continuousBattle:!!stable,
        ticksHz:stable?(final.ticks-run.state.ticks)*1000/elapsed:null},
      notes:['Native callbacks may batch multiple emulated frames; callback Hz is not game FPS.',
        'Busy percent covers the emulator callback, excluding separate audio callbacks and other page work.',
        'Game tick rate is only reported for an observed continuous battle; original PC-98 timing is not exactly 60 Hz.']};
    record.disabled=false;record.textContent='再记录 15 秒';download.hidden=false;
    show(`${reason==='complete'?'记录完成':'记录已中止'} · ${round(elapsed/1000)} 秒\n`
      +`模拟器回调：${round(report.nativeCallbacks.hz)} 次/秒，平均 ${round(native.meanMs)} ms，P95 ${round(native.p95Ms)} ms\n`
      +`回调占用：${round(report.nativeCallbacks.busyPercent)}%；超过 50 ms：${native.over50ms} 次\n`
      +`游戏推进：${round(report.game.ticksHz)} 步/秒${stable?'':'（请在实际战斗中记录，并避免暂停或切换关卡）'}\n`
      +'可下载报告进行对比。报告只保存在本页，不会自动上传。');
  }
  record.onclick=()=>{
    const emulator=getEmulator();if(!emulator||emulator.state!=='running'){show('请先启动游戏，进入实际战斗后再记录。');return;}
    if(active)return;
    const module=emulator.module,state=readState(),start=performance.now();
    const run=active={emulator,module,state,start,costs:[],intervals:[],last:null,stable:!!state?.playing,seconds:-1};
    report=null;download.hidden=true;record.disabled=true;
    run.observe=cost=>{if(active===run&&run.costs.length<8192)run.costs.push(cost);};
    module.observeLocalFrame=run.observe;
    run.visibility=()=>{if(document.hidden)finish('page-hidden');};
    document.addEventListener('visibilitychange',run.visibility);
    const tick=now=>{
      if(active!==run)return;
      if(emulator.state!=='running'){finish('emulator-stopped');return;}
      if(run.last!==null&&run.intervals.length<8192)run.intervals.push(now-run.last);run.last=now;
      const current=readState();if(!current?.playing||current.generation!==state?.generation)run.stable=false;
      const seconds=Math.max(0,Math.ceil((15000-(now-start))/1000));
      if(seconds!==run.seconds){run.seconds=seconds;record.textContent=`记录中 ${seconds} 秒`;show('正在记录性能，请保持正常游玩。');}
      if(now-start>=15000){finish('complete');return;}
      run.raf=requestAnimationFrame(tick);
    };
    run.raf=requestAnimationFrame(tick);run.timeout=setTimeout(()=>finish('complete'),15000);
  };
  download.onclick=()=>{
    if(!report)return;
    const url=URL.createObjectURL(new Blob([JSON.stringify(report,null,2)+'\n'],{type:'application/json'}));
    const a=document.createElement('a');a.href=url;a.download=`th${game}-solo-performance.json`;a.click();setTimeout(()=>URL.revokeObjectURL(url),10000);
  };
  return {read:()=>report,stop:()=>finish('cancelled')};
}
