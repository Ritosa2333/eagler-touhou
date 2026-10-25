/** Serialized reader for a title-declared, bounded, read-only native snapshot. */
export function readNativeSnapshotRaw({runtime,snapshot:spec}={}){
  const invalid=reason=>({available:false,reason});
  const identifier=value=>typeof value==='string'&&/^[A-Za-z_$][\w$]*$/.test(value);
  if(!runtime||!['global','module','core','app'].every(name=>identifier(runtime[name]))||!spec||
    !Number.isSafeInteger(spec.wordCount)||spec.wordCount<=0||spec.wordCount>4096||
    !['f64','u32','i32'].includes(spec.wordType)||!identifier(spec.export)||typeof spec.app!=='boolean')
    return invalid('Native snapshot contract is incomplete');
  const root=typeof window==='object'?window:globalThis,mounted=root[runtime.global],module=mounted?.[runtime.module],core=mounted?.[runtime.core]??module,
    app=mounted?.[runtime.app];
  if(!mounted||(spec.app&&(!Number.isSafeInteger(app)||app<0)))return invalid('Runtime iframe identity unavailable');
  let snapshot;
  for(const owner of [core,module]){const candidate=owner?.[spec.export]??owner?.[`_${spec.export}`];if(typeof candidate==='function'){snapshot=candidate.bind(owner);break;}}
  if(!snapshot)return invalid('Native snapshot export or wasm memory unavailable');
  const pointer=spec.app?snapshot(app):snapshot(),coreBuffer=core?.memory?.buffer,moduleBuffer=module?.HEAPU8?.buffer;
  if(coreBuffer&&moduleBuffer&&coreBuffer!==moduleBuffer)return invalid('Native snapshot owners expose different wasm memories');
  const buffer=coreBuffer??moduleBuffer,ArrayType=spec.wordType==='f64'?Float64Array:spec.wordType==='u32'?Uint32Array:Int32Array,
    bytes=spec.wordCount*ArrayType.BYTES_PER_ELEMENT;
  if(!buffer)return invalid('Native snapshot export or wasm memory unavailable');
  if(!Number.isSafeInteger(pointer)||pointer<=0||pointer%ArrayType.BYTES_PER_ELEMENT||pointer+bytes>buffer.byteLength)
    return invalid('Native snapshot pointer is outside wasm memory');
  return{available:true,words:Array.from(new ArrayType(buffer,pointer,spec.wordCount))};
}
