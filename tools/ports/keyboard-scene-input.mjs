import assert from 'node:assert/strict';

/** Physical ownership is distinct from the bridge's merged physical/virtual key. */
export async function sendOwnedPhysicalKey({heldKeys,code,down,readBridge,dispatch}){
  assert.ok(heldKeys instanceof Set&&typeof code==='string'&&typeof down==='boolean');
  const before=await readBridge();
  assert.equal(before?.active,true,`Runtime keyboard bridge is unavailable before ${down?'down':'up'} ${code}`);
  const changed=heldKeys.has(code)!==down;
  let physicalReceipt=null;
  if(changed){
    if(down)heldKeys.add(code);
    try{
      physicalReceipt=await dispatch(code,down);
      assert.ok(physicalReceipt?.source==='physical'&&physicalReceipt.code===code&&physicalReceipt.down===down&&
        Array.isArray(physicalReceipt.transitions),`Physical ${code} ${down?'down':'up'} did not reach the production mapper`);
    }finally{if(!down)heldKeys.delete(code);}
  }
  const after=await readBridge();
  assert.equal(after?.active,true,`Runtime keyboard bridge retired during ${code}`);
  if(down)assert.equal(after.held,true,`Runtime bridge did not hold ${code} after physical keydown`);
  // A virtual control can keep the merged key down after physical keyup.
  // Native consumer scenarios verify the remaining owner's effect and release.
  return{code,down,changed,physicalHeld:heldKeys.has(code),bridgeHeld:after.held,physicalReceipt};
}
