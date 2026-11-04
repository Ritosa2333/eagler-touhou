// The original Game Over caption waits indefinitely for an extra key before
// presenting Continue. Keep its animation and remove only that wait loop.
export function installGameOverTransition(exe){
  const header=exe[8]*16+exe[9]*4096,at=header+0xc3f3;
  const expected=[0xc7,0x06,0x38,0x25,0,0,0xeb,0x05,0x9a,0xde,0x01,0x45,0x0f,0x83,0x3e,0x38,0x25,0,0x74,0xf4];
  if(!expected.every((byte,i)=>exe[at+i]===byte))throw Error('TH02 Game Over 原程序不匹配');
  exe[at+18]=0x90;exe[at+19]=0x90;
}
