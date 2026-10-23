// Trusted title contracts decode their own ABI. This module never accepts a
// raw report's claimed layout, owner registry or derived verdict as authority.
export function assertNativeVisualEvidence(row,contract,label='visual window',retainedLifecycle=null){
  if(!contract||typeof contract.readNativeWindow!=='function')throw Error(`${label}: no registered native visual contract`);
  if(row?.build?.game!==contract.game)throw Error(`${label}: wrong registered title`);
  return contract.readNativeWindow(row,{label,retainedLifecycle});
}
