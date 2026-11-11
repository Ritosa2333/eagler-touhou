import {Fat12,sha256} from './solo-fat12.js';
import {normalizePractice} from './practice-config.js';
async function verified(item,gzip=false){
  const r=await fetch(new URL(item.url,import.meta.url),{cache:'no-store'});if(!r.ok)throw Error('练习资源读取失败');
  const bytes=new Uint8Array(await(gzip?new Response(r.body.pipeThrough(new DecompressionStream('gzip'))):r).arrayBuffer());
  if(item.size!==undefined&&bytes.length!==item.size||await sha256(bytes)!==item.sha256)throw Error('练习资源校验失败');return bytes;
}
export async function loadPracticeResources(language){
  const manifest=await fetch(new URL('practice-assets.json',import.meta.url),{cache:'no-store'}).then(r=>{if(!r.ok)throw Error('练习清单读取失败');return r.json();}),spec=manifest.languages[language];
  if(!spec)throw Error('未知练习语言');
  const core=await fetch(new URL('practice-core.json',import.meta.url),{cache:'no-store'}).then(r=>{if(!r.ok)throw Error('练习核心清单读取失败');return r.json();});
  return {manifest,spec,exe:await verified(spec,true),wasmBinary:await verified(core)};
}
export async function installPractice(data,resources,value){
  const config=normalizePractice(value),fat=new Fat12(data),{spec,manifest,exe}=resources;
  if(await sha256(fat.read(fat.find('HUMAROKU/MAIN.EXE')))!==spec.sourceMainSha256)throw Error('练习源程序版本不匹配');
  fat.replace(fat.find('HUMAROKU/MAIN.EXE'),exe);
  if(config.mode!=='target')return;
  const bytes=await verified(manifest.launcher),sig=new TextEncoder().encode(manifest.launcher.signature),found=bytes.findIndex((b,i)=>b===sig[0]&&sig.every((v,j)=>bytes[i+j]===v));
  if(found<0||bytes[found+sig.length]!==1)throw Error('练习启动器配置无效');const at=found+sig.length;
  bytes.set([config.setup.stage,config.setup.difficulty,0,0,config.setup.character],at+1);
  fat.put('HUMAROKU/WEBSTART.COM',bytes);
  const entry=fat.find('HUMAROKU/GAME.BAT');let text=Array.from(fat.read(entry),b=>String.fromCharCode(b)).join(''),count=0;
  text=text.replace(/^zun zunsoft\.com[ \t]*\r*$/gm,()=>{count++;return '';});if(count!==6)throw Error('练习标志动画入口不匹配');count=0;
  text=text.replace(/^(op|main)[ \t]*\r*$/gm,()=>{count++;return 'WEBSTART\r\nif errorlevel 1 goto fin\r\nmain\r';});
  if(count!==6)throw Error('练习启动脚本不匹配');fat.replace(entry,Uint8Array.from(text,c=>c.charCodeAt(0)));
}
