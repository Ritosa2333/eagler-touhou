export const launchDefaults=Object.freeze({stage:0,difficulty:1,lives:2,bombs:3,focusEnabled:true,focusPercent:50,teamTargeting:true});
export function launchConfig(settings){
 const bounds={stage:[0,5],difficulty:[0,3],lives:[0,5],bombs:[0,5],p1:[0,2],p2:[0,2],p3:[0,2],players:[2,3]};
 for(const [key,[lo,hi]]of Object.entries(bounds))if(!Number.isInteger(settings[key])||settings[key]<lo||settings[key]>hi)throw Error(`TH02 开局设置无效：${key}`);
 for(const key of ['focusEnabled','teamTargeting'])if(settings[key]!==undefined&&typeof settings[key]!=='boolean')throw Error(`TH02 开局设置无效：${key}`);
 if(settings.focusPercent!==undefined&&settings.focusPercent!==50)throw Error('TH02 低速移速固定为 50%');
 return Uint8Array.of(1,settings.stage,settings.difficulty,settings.lives,settings.bombs,settings.p1);
}
export function configureLauncher(original,settings){
 const signature=new TextEncoder().encode('TH02WEBSTARTv1!'),defaults=[1,0,1,2,3,0],config=launchConfig(settings);
 // A near jump precedes the marker. Match uniquely and check default bytes
 // before editing the private executable; never alter the shared download.
 const matches=[];
 for(let at=0;at<=original.length-signature.length-defaults.length;at++)if(signature.every((b,i)=>original[at+i]===b))matches.push(at+signature.length);
 if(matches.length!==1||!defaults.every((b,i)=>original[matches[0]+i]===b))throw Error('TH02 网页启动器版本不匹配');
 const bytes=original.slice();bytes.set(config,matches[0]);return bytes;
}
