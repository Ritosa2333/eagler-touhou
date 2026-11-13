// Link only with -sASYNCIFY and TH14_WEBGPU_BACKEND. The C++ byte ABI is also
// used by the standalone native host; no game logic is implemented here.
addToLibrary({
  th14_webgpu_open__async:'auto',
  th14_webgpu_open: async function (pointer) {
    if (!Module['th14WebGpuRuntime']) {
      const {Th14WebGpuRuntime}=await import(locateFile('Th14WebGpuRuntime.mjs'));
      Module['th14WebGpuRuntime']=new Th14WebGpuRuntime(Module);
    }
    const runtime=Module['th14WebGpuRuntime'];
    try { await runtime.open(); }
    catch { return 0; }
    // Reacquire HEAP views after await: initialization may grow Wasm memory.
    HEAPF32.set(runtime.host.alphaBoundaries,pointer>>>2);return 1;
  },
  th14_webgpu_close:() => { const runtime=Module['th14WebGpuRuntime']; if (runtime && runtime.host) runtime.host.close(); },
  th14_webgpu_upload:(handle,width,height,pointer) => Module['th14WebGpuRuntime'].host.upload(handle>>>0,width,height,HEAPU8.subarray(pointer,pointer+width*height*4)),
  th14_webgpu_depth:(handle,width,height,format) => Module['th14WebGpuRuntime'].host.depth(handle>>>0,width,height,format),
  th14_webgpu_draw:(packet,vertices,count) => Module['th14WebGpuRuntime'].host.draw(HEAPU8.subarray(packet,packet+448),HEAPU8.subarray(vertices,vertices+count*40),count),
  th14_webgpu_draw_instances:(packet,vertices,count,instances,instanceCount) => Module['th14WebGpuRuntime'].host.drawInstances(
    HEAPU8.subarray(packet,packet+448),HEAPU8.subarray(vertices,vertices+count*40),count,
    HEAPU8.subarray(instances,instances+instanceCount*148),instanceCount),
  th14_webgpu_clear:(target,depth,flags,color,value,stencil,rects,count) => Module['th14WebGpuRuntime'].host.clear(target>>>0,depth>>>0,flags,color>>>0,value,stencil,HEAP32.subarray(rects>>>2,(rects>>>2)+count*4)),
  th14_webgpu_copy:(source,rect,target,point) => Module['th14WebGpuRuntime'].host.copy(source>>>0,HEAP32.subarray(rect>>>2,(rect>>>2)+4),target>>>0,HEAP32.subarray(point>>>2,(point>>>2)+2)),
  th14_webgpu_read__async:'auto',
  th14_webgpu_read: async function (handle,pointer,size) {
    const raw=await Module['th14WebGpuRuntime'].host.read(handle>>>0);
    if(raw.byteLength!==size)throw new Error('WebGPU readback size mismatch');
    HEAPU8.set(raw,pointer);
  },
  th14_webgpu_resample:(source,rect,target,to,weights,width,height,inputMask,outputMask) => Module['th14WebGpuRuntime'].host.resample(
    source>>>0,HEAP32.subarray(rect>>>2,(rect>>>2)+4),target>>>0,HEAP32.subarray(to>>>2,(to>>>2)+4),
    weights?HEAPU8.subarray(weights,weights+width*height*8):null,width,height,
    HEAPU32.subarray(inputMask>>>2,(inputMask>>>2)+4),HEAPU32.subarray(outputMask>>>2,(outputMask>>>2)+4)),
  th14_webgpu_release:(handle) => Module['th14WebGpuRuntime'].host.release(handle>>>0),
  th14_webgpu_present:(handle) => Module['th14WebGpuRuntime'].host.present(handle>>>0),
  th14_webgpu_schedule:(callback,data,delay) => Module['th14WebGpuRuntime'].schedule(callback,data,delay),
  th14_webgpu_request_frames:(callback,data) => Module['th14WebGpuRuntime'].startFrames(callback,data),
  th14_webgpu_archive_open:(pointer,size) => {
    try{Module['th14WebGpuRuntime'].openArchive(HEAPU8.subarray(pointer,pointer+size));return 1;}
    catch(error){console.error('TH14 archive worker:',error);return 0;}
  },
  th14_webgpu_archive_cancel:() => Module['th14WebGpuRuntime']?.cancelArchive(),
  th14_webgpu_archive_status:() => Module['th14WebGpuRuntime'].archiveStatus(),
  th14_webgpu_archive_size__deps:['$UTF8ToString'],
  th14_webgpu_archive_size:(name) => Module['th14WebGpuRuntime'].archiveSize(UTF8ToString(name)),
  th14_webgpu_archive_copy__deps:['$UTF8ToString'],
  th14_webgpu_archive_copy:(name,pointer,size) => Module['th14WebGpuRuntime'].archiveCopy(UTF8ToString(name),HEAPU8.subarray(pointer,pointer+size)),
  th14_webgpu_anm_size__deps:['$UTF8ToString'],
  th14_webgpu_anm_size:(name) => Module['th14WebGpuRuntime'].archiveAnmSize(UTF8ToString(name)),
  th14_webgpu_anm_copy__deps:['$UTF8ToString'],
  th14_webgpu_anm_copy:(name,pointer,size) => Module['th14WebGpuRuntime'].archiveAnmCopy(UTF8ToString(name),HEAPU8.subarray(pointer,pointer+size)),
  th14_webgpu_resource_size__deps:['$UTF8ToString'],
  th14_webgpu_resource_size:(kind,name) => Module['th14WebGpuRuntime'].archiveParsedSize(kind,UTF8ToString(name)),
  th14_webgpu_resource_copy__deps:['$UTF8ToString'],
  th14_webgpu_resource_copy:(kind,name,pointer,size) => Module['th14WebGpuRuntime'].archiveParsedCopy(kind,UTF8ToString(name),HEAPU8.subarray(pointer,pointer+size)),
  th14_webgpu_archive_schedule__deps:['$UTF8ToString'],
  th14_webgpu_archive_schedule:(callback,data,pointers,count) => {
    const names=[];
    for(let i=0;i<count;++i)names.push(UTF8ToString(HEAPU32[(pointers>>>2)+i]));
    Module['th14WebGpuRuntime'].scheduleArchive(callback,data,names);
  },
});
