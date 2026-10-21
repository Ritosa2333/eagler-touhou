/** Invoke a title's production Replay finalizer/opener with a bounded UTF-8 path. */
export function callNativeReplayFile({runtime:mapping,contract,action,stage,path}){
  if(!mapping||!contract||!['finalize','open'].includes(action))throw Error('Declare the native Replay file operation');
  const root=typeof window==='object'?window:globalThis,runtime=root[mapping.global];
  const core=runtime?.[mapping.core],module=runtime?.[mapping.module],app=runtime?.[mapping.app];
  const api=name=>core?.[name]??core?.[`_${name}`]??module?.[name]??module?.[`_${name}`];
  const fn=api(contract.export),allocate=api(contract.allocateExport),release=api(contract.freeExport);
  if(typeof fn!=='function'||typeof allocate!=='function'||typeof release!=='function'||contract.app&&app==null)
    return{status:'incomplete',reason:'The declared native Replay file operation or allocator is unavailable'};
  let text,args;
  if(action==='finalize'){
    if(!Number.isSafeInteger(contract.slot)||contract.slot<1||typeof contract.name!=='string'||
      !contract.name.trim()||contract.name.length>8||/[\x00-\x1f]/.test(contract.name))throw Error('Invalid native Replay save declaration');
    text=contract.name;
    args=pointer=>[contract.slot,pointer];
  }else{
    if(!Number.isSafeInteger(stage)||stage<0||typeof path!=='string'||!path.startsWith('replay/')||
      path.includes('..')||/[\\\x00-\x1f]/.test(path)||!/^\/[^\x00-\x1f]*$/.test(contract.saveRoot))throw Error('Invalid native Replay open path');
    text=`${contract.saveRoot.replace(/\/$/,'')}/${path}`;
    args=pointer=>[pointer,stage];
  }
  const encoded=new TextEncoder().encode(`${text}\0`),pointer=allocate(encoded.length);
  try{
    const buffer=core?.memory?.buffer??module?.HEAPU8?.buffer;
    if(!Number.isSafeInteger(pointer)||pointer<=0||!buffer||pointer+encoded.length>buffer.byteLength)
      throw Error('Native Replay string allocation is outside wasm memory');
    new Uint8Array(buffer,pointer,encoded.length).set(encoded);
    const values=args(pointer);if(contract.app)values.unshift(app);
    const result=fn(...values);
    return{status:result===1||result===true?'passed':'failed',action,nativeResult:result,
      ...(action==='finalize'?{path:contract.path,slot:contract.slot,name:contract.name}:{path,stage})};
  }finally{if(Number.isSafeInteger(pointer)&&pointer>0)release(pointer);}
}
