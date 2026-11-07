// Explicit development-only diagnostics. Never create a second GL context,
// poll getError, read pixels, finish the GPU, or upload reports automatically.
export function installGpuDiagnostics(core,canvas,build,host=window){
 if(typeof core._sdl_test_gpu_flags!=='function'||typeof core._sdl_test_capture_copies!=='function')return null;
 let profile={copies:true,flush:false,resetState:false};const events=[];
 const record=(kind,detail)=>{events.push({at:Date.now(),kind,detail});if(events.length>64)events.shift();};
 const api={
  configure(next={}){
   if(Object.keys(next).some(k=>!Object.hasOwn(profile,k)||typeof next[k]!=='boolean'))throw Error('Expected boolean copies/flush/resetState');
   profile={...profile,...next};core._sdl_test_gpu_flags(+profile.flush|(+profile.resetState<<1));core._sdl_test_capture_copies(+profile.copies);
   record('profile',{...profile});return {...profile};
  },
  mark(note='visible trails'){record('observation',String(note).slice(0,512));},
  report(){
   const gl=canvas.getContext('webgl2');let gpu=null;
   if(gl){const ext=gl.getExtension('WEBGL_debug_renderer_info');gpu={vendor:gl.getParameter(gl.VENDOR),renderer:gl.getParameter(gl.RENDERER),version:gl.getParameter(gl.VERSION),shadingLanguage:gl.getParameter(gl.SHADING_LANGUAGE_VERSION),unmaskedVendor:ext?gl.getParameter(ext.UNMASKED_VENDOR_WEBGL):null,unmaskedRenderer:ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):null,contextLost:gl.isContextLost(),attributes:gl.getContextAttributes(),extensions:gl.getSupportedExtensions()};}
   const p=core._sdl_stats?.()>>>2;
   return {schema:'th15-mobile-gpu/1',time:new Date().toISOString(),build,userAgent:host.navigator.userAgent,profile:{...profile},gpu,statistics:p?Array.from(core.HEAPU32.subarray(p,p+18)):null,events:events.map(e=>({...e}))};
  }
 };
 canvas.addEventListener('webglcontextlost',()=>record('context-lost',null));
 canvas.addEventListener('webglcontextrestored',()=>record('context-restored',null));
 host.th15GpuDiagnostics=api;return api;
}
