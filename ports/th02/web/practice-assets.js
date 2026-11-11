import {Fat12,sha256} from './solo-fat12.js';
import {normalizePractice} from './practice-config.js';
import {assetBytes,freshJSON} from './preload-cache.js';
const resourcesByLanguage=new Map();let manifests,core;
function verified(item,gzip=false){return assetBytes(item.url,{hash:item.sha256,size:item.size,gzip});}
export function loadPracticeResources(language){
  // Runtime generations own these immutable resources; never cache a patched disk.
  let task=resourcesByLanguage.get(language);
  if(!task){
    task=(async()=>{
      const [manifest,coreManifest]=await (manifests??=Promise.all([freshJSON('practice-assets.json'),freshJSON('practice-core.json')]));
      const spec=manifest.languages[language];if(!spec)throw Error('未知练习语言');
      const [exe,wasmBinary]=await Promise.all([verified(spec,true),core??=verified(coreManifest)]);
      return {manifest,spec,exe,wasmBinary};
    })();
    resourcesByLanguage.set(language,task);
    task.catch(()=>{resourcesByLanguage.delete(language);manifests=core=undefined;});
  }
  return task;
}
export async function installPractice(data,resources,value){
  const config=normalizePractice(value),fat=new Fat12(data),{spec,manifest,exe}=resources;
  if(await sha256(fat.read(fat.find('HUMAROKU/MAIN.EXE')))!==spec.sourceMainSha256)throw Error('练习源程序版本不匹配');
  const patched=exe.slice(),header=new DataView(patched.buffer,patched.byteOffset,patched.byteLength).getUint16(8,true)*16;
  const cs=spec.patch.codeSegment,entries=spec.patch.practice.entries;
  for(const name of ['livesZero','bombsZero']){
    const branch=header+cs*16+entries[name]+7;
    if(patched[branch]!==0x74)throw Error('练习资源补丁版本不匹配');
    patched[branch]=0xeb;
  }
  // Refresh the native score HUD immediately before the arrival pause.
  const code=header+cs*16,call=entries.pauseCall??entries.frame+43,cave=entries.entryPause-98;
  const v=new DataView(patched.buffer,patched.byteOffset,patched.byteLength);
  const pause=call+3+v.getInt16(code+call+1,true);
  patched.set([0x6a,0x06,0x66,0xff,0x36,0x28,0x0b,0xe8,0,0,0xe9,0,0],code+cave);
  v.setInt16(code+cave+8,(0xdc55-cs*16)-(cave+10),true);
  v.setInt16(code+cave+11,pause-(cave+13),true);
  v.setInt16(code+call+1,cave-(call+3),true);
  fat.replace(fat.find('HUMAROKU/MAIN.EXE'),patched);
  if(config.mode!=='target'&&!config.directStart)return;
  const bytes=await verified(manifest.launcher),sig=new TextEncoder().encode(manifest.launcher.signature),found=bytes.findIndex((b,i)=>b===sig[0]&&sig.every((v,j)=>bytes[i+j]===v));
  if(found<0||bytes[found+sig.length]!==1)throw Error('练习启动器配置无效');const at=found+sig.length;
  bytes.set([config.setup.stage,config.setup.difficulty,config.directStart?config.setup.lives:0,config.directStart?config.setup.bombs:config.setup.stage===5?1:3,config.setup.character],at+1);
  fat.put('HUMAROKU/WEBSTART.COM',bytes);
  const entry=fat.find('HUMAROKU/GAME.BAT');let text=Array.from(fat.read(entry),b=>String.fromCharCode(b)).join(''),count=0;
  text=text.replace(/^zun zunsoft\.com[ \t]*\r*$/gm,()=>{count++;return '';});if(count!==6)throw Error('练习标志动画入口不匹配');count=0;
  text=text.replace(/^(op|main)[ \t]*\r*$/gm,()=>{count++;return 'WEBSTART\r\nif errorlevel 1 goto fin\r\nmain\r';});
  if(count!==6)throw Error('练习启动脚本不匹配');fat.replace(entry,Uint8Array.from(text,c=>c.charCodeAt(0)));
}
