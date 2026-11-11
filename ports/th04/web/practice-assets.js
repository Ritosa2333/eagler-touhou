import {Fat12,sha256} from './solo-fat12.js';
import {normalizePractice} from './practice-config.js';
async function verified(item,gzip=false){
  const response=await fetch(new URL(item.url,import.meta.url),{cache:'no-store'});if(!response.ok)throw Error('练习资源读取失败');
  const bytes=new Uint8Array(await (gzip?new Response(response.body.pipeThrough(new DecompressionStream('gzip'))):response).arrayBuffer());
  if(bytes.length!==item.size||await sha256(bytes)!==item.sha256)throw Error('练习资源校验失败');return bytes;
}
export async function loadPracticeResources(language){
  const response=await fetch(new URL('practice-assets.json',import.meta.url),{cache:'no-store'});if(!response.ok)throw Error('练习清单读取失败');
  const manifest=await response.json(),spec=manifest.languages[language];if(!spec)throw Error('未知练习语言');
  const core=await fetch(new URL('practice-core.json',import.meta.url),{cache:'no-store'}).then(r=>r.json());
  const wasmResponse=await fetch(new URL(core.url,import.meta.url),{cache:'no-store'}),wasmBinary=new Uint8Array(await wasmResponse.arrayBuffer());
  if(!wasmResponse.ok||await sha256(wasmBinary)!==core.sha256)throw Error('练习核心校验失败');
  return {manifest,spec,exe:await verified(spec,true),wasmBinary};
}
export async function installPractice(data,resources,value){
  const config=normalizePractice(value),fat=new Fat12(data),{spec,manifest,exe}=resources;
  if(await sha256(fat.read(fat.find('GENSO/MAIN.EXE')))!==spec.sourceMainSha256)throw Error('练习源程序版本不匹配');
  fat.replace(fat.find('GENSO/MAIN.EXE'),exe);
  if(config.mode!=='target')return;
  const bytes=await verified(manifest.launcher),signature=new TextEncoder().encode(manifest.launcher.signature);
  const at=bytes.findIndex((b,i)=>b===signature[0]&&signature.every((v,j)=>bytes[i+j]===v))+signature.length;
  if(at<signature.length||bytes[at]!==2)throw Error('练习启动器配置无效');
  bytes[at+1]=config.setup.difficulty;bytes[at+2]=1;bytes[at+3]=0;
  bytes[at+4]=config.setup.character>>>1;bytes[at+5]=config.setup.character&1;
  fat.put('GENSO/PRACTICE.COM',bytes);
  const entry=fat.find('GENSO/GAME.BAT');let text=Array.from(fat.read(entry),b=>String.fromCharCode(b)).join(''),count=0;
  text=text.replace(/^op\r*$/gm,()=>{count++;return 'practice\r\nif errorlevel 1 goto fin\r\nmain\r';});
  if(count!==5)throw Error('练习启动脚本不匹配');fat.replace(entry,Uint8Array.from(text,c=>c.charCodeAt(0)));
}
