// Scope deterministic clocks to the DOSBox Emscripten loader.
export async function loadExternalDosboxFactory({vendor,module,host,wasmBinary}){
  const loaderUrl=new URL('dosbox-jspi.mjs',vendor),wasmUrl=new URL('wdosbox-x-fast.wasm',vendor);
  const [sourceResponse,wasmResponse]=await Promise.all([fetch(loaderUrl,{cache:'no-store'}),wasmBinary?Promise.resolve(null):fetch(wasmUrl,{cache:'no-store'})]);
  if(!sourceResponse.ok||wasmResponse&&!wasmResponse.ok)throw Error('单人 Replay 核心加载失败');
  const source=await sourceResponse.text(),plain=source.replace(/\bexport\s+default\s+WDOSBOXXJSPI\s*;?/, '');
  if(plain===source)throw Error('单人 Replay 加载器格式不支持');
  const compiled=await WebAssembly.compile(wasmBinary||await wasmResponse.arrayBuffer());
  module.instantiateWasm=(imports,receive)=>{
    host.attach(module,imports);
    if(module.practiceSignal)imports.env.th02_practice=module.practiceSignal;
    // Compilation is already asynchronous above. Throwing an instantiation
    // error here rejects Emscripten's factory promise instead of leaving its
    // callback promise pending forever.
    const instance=new WebAssembly.Instance(compiled,imports);
    receive(instance,compiled);
    return instance.exports;
  };
  return new Function('Date','performance',plain+';return WDOSBOXXJSPI;')(host.Date,host.performance);
}
