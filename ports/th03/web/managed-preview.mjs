// The same native title worker used by the standalone room. Readiness follows
// its first real presentation; this instance never becomes a gameplay engine.
export function titlePreview(host) {
  let worker, canvas, task, serial=0, rejectCurrent, timeout;
  function stop() {
    serial++; clearTimeout(timeout); worker?.terminate(); worker=null; canvas?.remove(); canvas=null;
    rejectCurrent?.(Error('标题预览已取消')); rejectCurrent=null; task=null;
  }
  return {
    stop,
    prepare(language='ja') {
      if (task) return task;
      const own=++serial;
      task=(async () => {
        const {loadVerifiedBytes}=await import('./startup-assets.js');const freshJSON=async path=>(await fetch(path)).json();const assetBytes=(path,options)=>loadVerifiedBytes(path,options.hash,{gzip:options.gzip});
        const [manifest,runtime]=await Promise.all([freshJSON('disks/manifest.json'),freshJSON('vendor/dosbox-x/runtime.json')]);
        const spec=manifest.games['3-'+(language==='ja'?'jp':'cn')];
        const [disk,wasmBinary]=await Promise.all([
          assetBytes(spec.url,{hash:spec.sha256,size:spec.size,gzip:true}),
          assetBytes('vendor/dosbox-x/wdosbox-x-rollback.wasm',{hash:runtime.files['wdosbox-x-rollback.wasm']})
        ]);
        if (own!==serial) throw Error('标题预览已取消');
        canvas=document.createElement('canvas'); canvas.width=640;canvas.height=400;host.append(canvas);
        canvas.style.cssText='position:absolute;inset:0;width:100%;height:100%;object-fit:contain;image-rendering:pixelated';
        worker=new Worker(new URL('./room-title-worker.js',import.meta.url),{type:'module'});
        await new Promise((resolve,reject) => {
          rejectCurrent=reject;
          timeout=setTimeout(() => reject(Error('原生标题加载超时')),90000);
          const finish=(error) => {clearTimeout(timeout);rejectCurrent=null;error?reject(error):resolve();};
          worker.onerror=event=>finish(Error(event.message||'原生标题加载失败'));
          worker.onmessage=({data})=>{if(data.type==='ready') finish();else if(data.type==='error') finish(Error(data.message));};
          const offscreen=canvas.transferControlToOffscreen();
          worker.postMessage({type:'boot',game:'03',managedTitle:true,canvas:offscreen,disk,wasmBinary},[offscreen,disk.buffer,wasmBinary.buffer]);
        });
        // Let the browser composite the offscreen presentation before ack.
        await new Promise(resolve=>requestAnimationFrame(()=>requestAnimationFrame(resolve)));
        if (own!==serial) throw Error('标题预览已取消');
        return {presented:true};
      })().catch(error=>{if(own===serial)stop();throw error;});
      return task;
    }
  };
}
