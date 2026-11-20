// MZ image offsets for the regular and Extra BONUS waits, and three callers
// that immediately repeat the same wait without changing the screen.
const waitCalls=[0x1400a,0x14189];
const duplicateCalls=[0x17d99,0x19426,0x1ac2f];
const wait=0x8ac0,flag=0x8adf,flagLo=flag&255,flagHi=flag>>8;

export function installBonusConfirm(exe,patch){
 const base=patch.headerSize+patch.codeSegment*16;
 const code=Uint8Array.of(
  0x2e,0xc6,0x06,flagLo,flagHi,1,    // publish ready after drawing every bonus row
  0x6a,2,0x9a,0xc8,0x01,0x45,0x0f,  // original frame_delay(2)
  0x2e,0x80,0x3e,flagLo,flagHi,2,    // wait for the synchronized confirmation
  0x75,0xf1,
  0x2e,0xc6,0x06,flagLo,flagHi,0,
  0xcb,0xcb                         // return; second consecutive wait returns immediately
 );
 const oldWait=[0x9a,0x6c,0,0x45,0x0f],calls=[...waitCalls,...duplicateCalls];
 if(patch.codeSegment!==0xb16||exe.subarray(base+wait,base+flag+1).some(Boolean)||
    calls.some(at=>!oldWait.every((byte,i)=>exe[patch.headerSize+at+i]===byte)))throw Error('TH02 结算确认补丁版本不匹配');
 const view=new DataView(exe.buffer,exe.byteOffset,exe.byteLength),table=view.getUint16(24,true),count=view.getUint16(6,true);
 const relocations=new Set(Array.from({length:count},(_,i)=>view.getUint16(table+i*4,true)+view.getUint16(table+i*4+2,true)*16));
 const frameDelaySegment=patch.codeSegment*16+wait+11;
 if(table+(count+1)*4>patch.headerSize||calls.some(at=>!relocations.has(at+3))||relocations.has(frameDelaySegment))throw Error('TH02 结算确认重定位不匹配');
 for(const at of calls){
  view.setUint16(patch.headerSize+at+1,wait+(duplicateCalls.includes(at)?code.length-1:0),true);
  view.setUint16(patch.headerSize+at+3,patch.codeSegment,true);
 }
 exe.set(code,base+wait);
 view.setUint16(table+count*4,frameDelaySegment%16,true);
 view.setUint16(table+count*4+2,frameDelaySegment>>4,true);
 view.setUint16(6,count+1,true);
}

export function bonusConfirmAddress(mailbox,patch){return mailbox-patch.entries.mailbox+flag;}
export function freshBonusConfirm(inputs,previous){return inputs.some((bits,seat)=>(bits&~previous[seat]&(32|256))!==0);}
