// A display-only copy of the exact verified WASM module. It never steps the
// simulation, receives input, emits sound or participates in synchronization.
let core,context,image,layout,snapshotBefore,snapshotAfter;
self.onmessage=async({data})=>{
  try{
    if(data.type==='init'){
      core=(await WebAssembly.instantiate(data.module,{})).exports;
      const heapBytes=core.memory.buffer.byteLength,version=core.th_render_snapshot_info?.(0)===1?1:0;
      const offset=version?core.th_pixels():0,bytes=version?core.th_render_snapshot_info(1):0;
      if(version&&(!Number.isInteger(offset)||offset<0||bytes!==640*400*4||offset+bytes>heapBytes))throw Error('Invalid display snapshot layout');
      layout=[version,heapBytes,offset,bytes];
      if(!Array.isArray(data.layout)||data.layout.length!==4||!data.layout.every((value,i)=>value===layout[i]))throw Error('Display snapshot layout mismatch');
      // Fixed WASM memory permits reusing the destination spans across frames.
      snapshotBefore=new Uint8Array(core.memory.buffer,0,offset);
      snapshotAfter=new Uint8Array(core.memory.buffer,offset+bytes);
      const canvas=new OffscreenCanvas(640,400);
      context=canvas.getContext('2d',{alpha:false});
      if(!context)throw Error('OffscreenCanvas 2D unavailable');
      image=new ImageData(new Uint8ClampedArray(core.memory.buffer,core.th_pixels(),640*400*4),640,400);
      self.postMessage({type:'ready',layout});return;
    }
    if(data.type!=='render'||!core||!context||!(data.snapshot instanceof ArrayBuffer)||data.snapshot.byteLength!==layout[1]-layout[3])throw Error('Invalid display snapshot');
    const started=performance.now();
    const offset=snapshotBefore.length;
    if(offset)snapshotBefore.set(new Uint8Array(data.snapshot,0,offset));
    snapshotAfter.set(new Uint8Array(data.snapshot,offset));
    core.th_render_for_slot(Number.isInteger(data.localSlot)?data.localSlot:-1);context.putImageData(image,0,0);
    const bitmap=context.canvas.transferToImageBitmap(),cost=performance.now()-started;
    self.postMessage({type:'frame',frame:data.frame,snapshot:data.snapshot,bitmap,cost},[data.snapshot,bitmap]);
  }catch(error){self.postMessage({type:'error',message:error.message||String(error)});}
};
