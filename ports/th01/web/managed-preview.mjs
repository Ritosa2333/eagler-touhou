export function titlePreview(host){
 let worker,canvas,task,serial=0;
 return {stop(){serial++;worker?.terminate();canvas?.remove();worker=canvas=null;task=null;},prepare(){if(task)return task;const own=++serial;task=(async()=>{
  const bytes=await(await fetch('./game.wasm',{cache:'no-store'})).arrayBuffer();if(own!==serial)throw Error('标题预览已取消');
  canvas=document.createElement('canvas');canvas.width=640;canvas.height=400;canvas.style.cssText='position:absolute;inset:0;width:100%;height:100%;object-fit:contain;image-rendering:pixelated';host.append(canvas);
  worker=new Worker(new URL('./room-title-worker.js',import.meta.url),{type:'module'});
  await new Promise((resolve,reject)=>{const timer=setTimeout(()=>reject(Error('TH01 标题预览超时')),15000);worker.onerror=e=>{clearTimeout(timer);reject(Error(e.message));};worker.onmessage=e=>{if(e.data.type==='ready'){clearTimeout(timer);resolve();}else if(e.data.type==='error'){clearTimeout(timer);reject(Error(e.data.message));}};const moduleTask=WebAssembly.compile(bytes);moduleTask.then(module=>{if(own!==serial)return reject(Error('标题预览已取消'));const offscreen=canvas.transferControlToOffscreen();worker.postMessage({module,canvas:offscreen},[offscreen]);},reject);});
  await new Promise(resolve=>requestAnimationFrame(()=>requestAnimationFrame(resolve)));return {presented:true};
 })().catch(error=>{if(own===serial)this.stop();throw error;});return task;}};
}
