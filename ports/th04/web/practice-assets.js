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
  if(await sha256(fat.read(fat.find('GENSO/MAIN.EXE')))!==spec.sourceMainSha256)throw Error('练习源程序版本不匹配');
  const patched=exe.slice(),header=new DataView(patched.buffer,patched.byteOffset,patched.byteLength).getUint16(8,true)*16;
  const cs=spec.patch.code_segment,entries=spec.patch.practice.entries;
  for(const name of ['livesZero','bombsZero']){
    const branch=header+cs*16+entries[name]+9;
    if(patched[branch]!==0x74)throw Error('练习资源补丁版本不匹配');
    patched[branch]=0xeb;
  }
  fat.replace(fat.find('GENSO/MAIN.EXE'),patched);
  if(config.mode!=='target')return;
  const bytes=await verified(manifest.launcher),signature=new TextEncoder().encode(manifest.launcher.signature);
  const at=bytes.findIndex((b,i)=>b===signature[0]&&signature.every((v,j)=>bytes[i+j]===v))+signature.length;
  if(at<signature.length||bytes[at]!==2)throw Error('练习启动器配置无效');
  bytes[at+1]=config.setup.difficulty;bytes[at+2]=1;bytes[at+3]=2;
  bytes[at+4]=config.setup.character>>>1;bytes[at+5]=config.setup.character&1;
  fat.put('GENSO/PRACTICE.COM',bytes);
  const entry=fat.find('GENSO/GAME.BAT');let text=Array.from(fat.read(entry),b=>String.fromCharCode(b)).join(''),count=0;
  text=text.replace(/^op\r*$/gm,()=>{count++;return 'practice\r\nif errorlevel 1 goto fin\r\nmain\r';});
  if(count!==5)throw Error('练习启动脚本不匹配');fat.replace(entry,Uint8Array.from(text,c=>c.charCodeAt(0)));
}
