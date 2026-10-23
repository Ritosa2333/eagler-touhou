/** Serialized read-only title adapter. Field positions and identity ABI belong to the title declaration. */
export function readOriginalMenuState({contract}={}){
  if(!contract?.runtime||!contract.statusExport||!contract.fields||!contract.identity)
    return{available:false,reason:'Register the original title-state and Runtime identity exports'};
  const mapping=contract.runtime,runtime=globalThis[mapping.global];
  if(!runtime)return{available:false,reason:`Declared Runtime ${mapping.global} is absent`};
  const core=runtime[mapping.core],module=runtime[mapping.module],owners=[core,module].filter(Boolean);
  const find=name=>{for(const owner of owners){const fn=owner[name]??owner[`_${name}`];if(typeof fn==='function')return(...args)=>fn.apply(owner,args);}return null;};
  const status=find(contract.statusExport),identity=find(contract.identity.export);
  if(!status||!identity)return{available:false,reason:'Declared title or identity export is absent'};
  const buffer=core?.memory?.buffer??module?.HEAPU8?.buffer;
  if(!buffer)throw new Error('Title-state reader has no declared Runtime memory');
  if(core?.memory?.buffer&&module?.HEAPU8?.buffer&&core.memory.buffer!==module.HEAPU8.buffer)
    throw new Error('Title-state reader found competing Runtime memories');
  const pointer=identity();
  if(!Number.isInteger(pointer)||pointer<=0||pointer%4||pointer+contract.identity.wordCount*4>buffer.byteLength)
    throw new Error('Title-state identity export returned an invalid buffer');
  const words=new Uint32Array(buffer,pointer,contract.identity.wordCount);
  if(words[0]!==contract.identity.magic||words[1]!==contract.identity.version||words[2]!==contract.identity.wordCount)
    throw new Error('Title-state identity ABI differs from its declaration');
  const runtimeIdentity=Object.fromEntries(['generation','pointer','epoch'].map(name=>[name,words[contract.identity.fields[name]]]));
  const get=name=>{const value=status(contract.fields[name]);if(!Number.isSafeInteger(value)||value>0xffffffff||value<(contract.signedFields?.includes(name)?-0x80000000:0))throw new Error(`Invalid native title ${name}`);return value;};
  const scene=get('scene'),title={screen:get('screen'),cursor:get('cursor'),phase:get('phase'),stateTimer2:get('stateTimer2'),
    confirmationReady:get('confirmationReady')===1,input:{current:get('currentInput'),previous:get('previousInput')}};
  if(contract.fields.startupElapsed!==undefined)title.startupElapsed=get('startupElapsed');
  return{available:true,runtime:runtimeIdentity,nativeScene:scene,scene:scene===contract.titleScene?'title':'other',title};
}
