// These exports return void in the production C++ ABI. Queue user/launcher
// mutations behind any suspended frame; queries retain their synchronous ABI.
const queuedVoid=new Set([
  'th11_music_enabled',
  'th11_loop_start','th11_loop_stop','th11_loop_pause','th11_touch','th11_touch_cancel',
  'th11_touch_options','th11_touch_controls','th11_touch_stick','sdl_loop_pause',
  'sdl_touch','sdl_touch_cancel','sdl_touch_options','sdl_touch_gestures','sdl_touch_mode','sdl_touch_controls']);
queuedVoid.add('th11_key');
queuedVoid.add('th11_keys_clear');
const boundCores=new WeakSet();
export function bindTh14WebGpuCore(module,raw) {
  if(boundCores.has(raw))return raw;
  const core=Object.assign(Object.create(null),raw);
  for(const name of queuedVoid)if(typeof raw[name]==='function')core[name]=(...args)=>{
    const runtime=module.th14WebGpuRuntime;
    if(!runtime)return raw[name](...args);
    void runtime.run(()=>raw[name](...args)).catch(()=>{});
  };
  boundCores.add(core);return core;
}
