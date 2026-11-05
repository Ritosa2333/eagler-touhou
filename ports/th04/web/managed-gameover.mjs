// Keep the original Game Over caption visible briefly before Continue.
export function installGameOverTransition(exe){
  const header=exe[8]*16+exe[9]*4096,at=header+0xe607;
  const prefix=[0x6a,0,0x9a,0x33,0x01],suffix=[0xe8,0x3a,0x27,0xe8,0x66,0x00,0xb4,0];
  if(!prefix.every((byte,i)=>exe[at+i]===byte)||!suffix.every((byte,i)=>exe[at+7+i]===byte))throw Error('TH04 Game Over 原程序不匹配');
  exe[at+1]=30;
}
