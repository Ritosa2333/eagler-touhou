import assert from 'node:assert/strict';

/** Hold one physical edge until a real menu consumer samples it or changes scene. */
export async function pressObservedMenuKey(io,{code,mask,inputField,transitionFields,timeoutMs=2000}){
  assert.ok(Number.isSafeInteger(mask)&&mask>0&&mask<=0x80000000&&Number.isInteger(Math.log2(mask)));
  assert.ok(typeof inputField==='string'&&Array.isArray(transitionFields)&&transitionFields.length>0);
  assert.ok(Number.isSafeInteger(timeoutMs)&&timeoutMs>0&&timeoutMs<=5000);
  const evidence={code,mask,inputField,transitionFields,before:await io.read(),samples:[],released:false};
  const valid=state=>{assert.ok(Number.isSafeInteger(state[inputField])&&state[inputField]>=0);for(const field of transitionFields)assert.ok(Object.hasOwn(state,field));};
  valid(evidence.before);
  assert.equal(evidence.before[inputField]&mask,0,'Menu key was already held before the requested physical edge');
  const end=io.now()+timeoutMs;
  try{
    await io.down(code);
    do{
      const state=await io.read();valid(state);assert.ok(evidence.samples.length<256);evidence.samples.push(state);
      if((state[inputField]&mask)!==0||transitionFields.some(field=>state[field]!==evidence.before[field])){
        evidence.acknowledged=true;return evidence;
      }
      await io.wait(20);
    }while(io.now()<end);
    throw Object.assign(Error('Physical menu key did not reach its native consumer: '+code),{evidence});
  }finally{await io.up(code);evidence.released=true;}
}
