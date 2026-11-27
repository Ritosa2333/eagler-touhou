import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createReadStream,createWriteStream,readFileSync,mkdirSync,writeFileSync} from 'node:fs';
import {pipeline} from 'node:stream/promises';
import {resolve,sep,extname} from 'node:path';
import {createHash} from 'node:crypto';
import {gzipSync} from 'node:zlib';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';

const args=Object.fromEntries(process.argv.slice(2).map(value=>{const i=value.indexOf('=');return [value.slice(2,i),value.slice(i+1)];}));
if(!args.assets)throw Error('需要 --assets=四段原版 WMV 所在目录');
const output=resolve(args.output||'ports/thkkd/build/video-core/benchmark'),assets=resolve(args.assets);
const roots={baseline:resolve(args.baseline||'ports/thkkd/build/video-core/baseline'),candidate:resolve(args.candidate||'launcher/third_party/thkkd-video'),wrapper:resolve('launcher/node_modules/@ffmpeg/ffmpeg/dist/esm')};
const names=['back100','back200','back201','back300'];mkdirSync(output,{recursive:true});
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const report={schema:'thkkd/video-core-browser/1',clock:args['fixed-clock']==='1'?'fixed-20ms':'real',variants:{},inputs:Object.fromEntries(names.map(name=>{const bytes=readFileSync(resolve(assets,name+'.wmv'));return [name,{bytes:bytes.length,sha256:sha(bytes)}];})),checks:[],errors:[]};
for(const variant of ['baseline','candidate']){
  const bytes=readFileSync(resolve(roots[variant],'ffmpeg-core.wasm'));
  report.variants[variant]={wasmBytes:bytes.length,gzipBytes:gzipSync(bytes,{level:9}).length,wasmSha256:sha(bytes),files:[]};
}
assert.ok(report.variants.candidate.wasmBytes<=8*1024*1024);assert.ok(report.variants.candidate.gzipBytes<=3*1024*1024);
const server=createServer(async(request,response)=>{
  try{
    const path=decodeURIComponent(new URL(request.url,'http://localhost').pathname);
    response.setHeader('Cross-Origin-Opener-Policy','same-origin');response.setHeader('Cross-Origin-Embedder-Policy','require-corp');
    if(path==='/'){response.setHeader('Content-Type','text/html');response.end('<!doctype html><title>视频转码验证</title>');return;}
    const saved=path.match(/^\/output\/(baseline|candidate)\/(back100|back200|back201|back300)\.webm$/);
    if(request.method==='POST'&&saved){const dir=resolve(output,saved[1]);mkdirSync(dir,{recursive:true});await pipeline(request,createWriteStream(resolve(dir,saved[2]+'.webm')));response.end('saved');return;}
    const instrumented=path.match(/^\/(baseline|candidate)\/measured\.js$/);
    if(instrumented){response.setHeader('Content-Type','text/javascript');response.end(`import createCore from './ffmpeg-core.js';export default async options=>{const core=await createCore(options),exec=core.exec;core.exec=(...args)=>{const savedNow=Date.now;let tick=1700000000000;if(${args['fixed-clock']==='1'})Date.now=()=>tick+=20;let result;try{result=exec(...args);}finally{Date.now=savedNow;}core.logger({type:'stdout',message:'THKKD_TEST_MEMORY:'+core.HEAPU8.byteLength});return result;};return core;};`);return;}
    let file;
    const source=path.match(/^\/source\/(back100|back200|back201|back300)\.wmv$/);
    if(source)file=resolve(assets,source[1]+'.wmv');
    else {const m=path.match(/^\/(baseline|candidate|wrapper)\/([^/]+\.(?:js|wasm))$/);if(m){file=resolve(roots[m[1]],m[2]);assert.ok(file.startsWith(roots[m[1]]+sep));}}
    if(!file){response.writeHead(404);response.end();return;}
    response.setHeader('Content-Type',extname(file)==='.js'?'text/javascript':extname(file)==='.wasm'?'application/wasm':'application/octet-stream');
    const stream=createReadStream(file);stream.on('error',()=>{response.statusCode=404;response.end();});stream.pipe(response);
  }catch(error){response.statusCode=500;response.end(String(error));}
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH||'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,protocolTimeout:1200000,args:['--mute-audio']}),page=await browser.newPage();
page.on('pageerror',error=>report.errors.push(error.message));report.browser=await browser.version();
await page.exposeFunction('videoTestProgress',message=>console.log(JSON.stringify(message)));
try{
  await page.goto(`http://127.0.0.1:${server.address().port}/`);
  const order=args.order?args.order.split(','):['baseline','candidate'];assert.deepEqual([...order].sort(),['baseline','candidate']);
  for(const variant of order){
    const result=await page.evaluate(async({variant,names})=>{
      const {FFmpeg}=await import('/wrapper/index.js');const worker=new FFmpeg(),logs=[],rows=[];let progress=[],peak=0;
      worker.on('log',({message})=>{logs.push(message);if(message.startsWith('THKKD_TEST_MEMORY:'))peak=Math.max(peak,Number(message.split(':')[1]));});
      worker.on('progress',event=>{progress.push(event);});
      const started=performance.now();await worker.load({coreURL:location.origin+'/'+variant+'/measured.js',wasmURL:location.origin+'/'+variant+'/ffmpeg-core.wasm',classWorkerURL:location.origin+'/wrapper/worker.js'});const loadMs=performance.now()-started;
      try{
        for(const name of names){
          progress=[];const bytes=new Uint8Array(await(await fetch('/source/'+name+'.wmv')).arrayBuffer());await worker.writeFile('original.wmv',bytes);
          const start=performance.now();const code=await worker.exec(['-i','original.wmv','-an','-c:v','libvpx','-crf','10','-b:v','2M','-deadline','realtime','-cpu-used','8','-fps_mode','vfr','browser.webm']);const milliseconds=performance.now()-start;
          if(code!==0)throw Error(variant+' '+name+' 转码失败: '+logs.slice(-12).join('\n'));
          const result=await worker.readFile('browser.webm');if(typeof result==='string')throw Error('视频输出不是二进制');
          const saved=await fetch('/output/'+variant+'/'+name+'.webm',{method:'POST',body:result});if(!saved.ok)throw Error('保存对照视频失败');
          const row={name,milliseconds,outputBytes:result.length,wasmLinearMemoryPeakBytes:peak,progressEvents:progress.length,progressFinite:progress.every(p=>Number.isFinite(p.progress)&&Number.isFinite(p.time))};rows.push(row);await videoTestProgress({variant,...row});
          await worker.deleteFile('original.wmv');await worker.deleteFile('browser.webm');
        }
        const capabilities={};if(variant==='candidate')for(const key of ['decoders','encoders','demuxers','muxers','protocols','filters']){logs.length=0;const code=await worker.exec(['-'+key]);if(code!==0)throw Error('组件枚举失败 '+key);capabilities[key]=logs.filter(line=>!line.startsWith('THKKD_TEST_MEMORY:')).join('\n');}
        return {loadMs,files:rows,capabilities,wasmLinearMemoryPeakBytes:peak};
      }finally{worker.terminate();}
    },{variant,names});
    Object.assign(report.variants[variant],result);writeFileSync(resolve(output,'progress.json'),JSON.stringify(report,null,2));
  }
  for(const variant of ['baseline','candidate'])for(const row of report.variants[variant].files){assert.ok(row.progressEvents>1);assert.equal(row.progressFinite,true);}
  report.lifecycle=await page.evaluate(async()=>{
    const {FFmpeg}=await import('/wrapper/index.js');const options={coreURL:location.origin+'/candidate/ffmpeg-core.js',wasmURL:location.origin+'/candidate/ffmpeg-core.wasm',classWorkerURL:location.origin+'/wrapper/worker.js'};
    let worker=new FFmpeg();await worker.load(options);await worker.writeFile('bad.wmv',new Uint8Array([1,2,3]));
    const invalid=await worker.exec(['-i','bad.wmv','-an','-c:v','libvpx','bad.webm']);if(invalid===0)throw Error('损坏输入被接受');await worker.deleteFile('bad.wmv');await worker.deleteFile('bad.webm').catch(()=>{});worker.terminate();worker=new FFmpeg();await worker.load(options);
    await worker.writeFile('original.wmv',new Uint8Array(await(await fetch('/source/back201.wmv')).arrayBuffer()));
    const command=['-i','original.wmv','-an','-c:v','libvpx','-crf','10','-b:v','2M','-deadline','realtime','-cpu-used','8','-fps_mode','vfr','-frames:v','1','short.webm'];
    if(await worker.exec(command)!==0)throw Error('错误后重新转换失败');await worker.deleteFile('short.webm');worker.terminate();
    worker=new FFmpeg();await worker.load(options);await worker.writeFile('original.wmv',new Uint8Array(await(await fetch('/source/back100.wmv')).arrayBuffer()));
    const progress=new Promise(resolve=>worker.on('progress',resolve));const running=worker.exec(['-i','original.wmv','-an','-c:v','libvpx','-deadline','realtime','cancel.webm']).then(()=>false,()=>true);
    await Promise.race([progress,new Promise((_,reject)=>setTimeout(()=>reject(Error('取消测试未开始转码')),30000))]);worker.terminate();if(!await running)throw Error('终止后待处理任务未拒绝');
    worker=new FFmpeg();await worker.load(options);await worker.writeFile('original.wmv',new Uint8Array(await(await fetch('/source/back201.wmv')).arrayBuffer()));
    try{if(await worker.exec(command)!==0)throw Error('取消后重新转换失败');const output=await worker.readFile('short.webm');return {invalidRejected:true,errorThenRetry:true,terminatedPendingRequest:true,cancelThenRetry:output.length>0};}finally{worker.terminate();}
  });
  const total=variant=>report.variants[variant].files.reduce((sum,row)=>sum+row.milliseconds,0);report.timingRatio=total('candidate')/total('baseline');
  report.checks.push('four-complete-videos-transcoded-with-existing-parameters','finite-progress','single-worker-reused-and-files-released','invalid-input-and-retry','terminate-and-retry','linear-memory-high-water-mark');
  assert.deepEqual(report.errors,[]);report.result='passed';console.log(JSON.stringify({result:report.result,timingRatio:report.timingRatio,baselineMs:total('baseline'),candidateMs:total('candidate'),candidateBytes:report.variants.candidate.wasmBytes}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);}
finally{writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();await new Promise(resolve=>server.close(resolve));}
