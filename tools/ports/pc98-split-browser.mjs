import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createServer} from 'node:http';
import {mkdtemp, readFile, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {extname, join, resolve, sep} from 'node:path';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../launcher/lib/chromium-executable.mjs';
import {publishPC98Blocks} from '../../launcher/lib/pc98-static-publication.mjs';
import {pc98RequiredBlockFiles} from '../../launcher/package/pc98-block-selection.mjs';

const require = createRequire(new URL('../../launcher/package.json',import.meta.url));
const puppeteer = require('puppeteer-core'), root = resolve(import.meta.dirname,'../..');
const temp = await mkdtemp(join(tmpdir(),'pc98-split-browser-'));
let browser, server;
try {
  const game = process.argv[2] || 'th02', language = process.argv[3] || 'ja', multiplayer = process.argv[4] === 'mp';
  if (!['th02','th03','th04'].includes(game) || !['ja','lang_zh-hans'].includes(language)) throw Error('Invalid test target');
  const source = resolve(root,'ports',game,'content'), runtime = resolve(root,'ports',game,'build-eagler');
  const files = {}, components = {}, digest = bytes => createHash('sha256').update(bytes).digest('hex');
  const descriptorFile = async (source,target) => {const bytes=await readFile(join(temp,source));return {source,target,bytes:bytes.length,sha256:digest(bytes),revision:digest(bytes).slice(0,16)};};
  await publishPC98Blocks({game,source,data:resolve(source,`${game}.data`),staging:temp,descriptorFile,packageFiles:files,components});
  files.data={source:`games/${game}/${game}.data`,target:`/${game}.data`,revision:'1'};
  const descriptor={schema:'eagler-touhou/package/1',game,revision:'1',files,components,
    runtimeRequirement:{protocol:'eagler-touhou/1',target:game,dataFile:'data'},base:{files:['data']}};
  const selected=pc98RequiredBlockFiles(descriptor,{game,language,multiplayer}).map(id=>files[id]);
  const html=`<!doctype html><script>
    window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
    addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
    window.rpc=(command,fields={})=>new Promise((ok,fail)=>{const f=document.querySelector('iframe'),request=crypto.randomUUID();const timer=setTimeout(()=>{removeEventListener('message',receive);fail(Error(command+' timeout'))},120000);function receive(e){if(e.source!==f.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);e.data.ok?ok(e.data):fail(Error(e.data.error))}addEventListener('message',receive);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:${JSON.stringify(game)},epoch:1,request,command,...fields},location.origin)});
    </script><iframe src="/runtime/${game}.html?runtimeEpoch=1${multiplayer?'&runtimeVariant=multiplayer':''}" style="width:640px;height:480px"></iframe>`;
  server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;
    if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
    let file;if(path==='/data')file=join(temp,`games/${game}/${game}.data`);
    else if(path.startsWith('/asset/')){const i=Number(path.slice(7));if(!Number.isInteger(i)||!selected[i])throw Error('asset');file=join(temp,selected[i].source);}
    else if(path.startsWith('/runtime/')){file=resolve(runtime,decodeURIComponent(path.slice(9)));if(!file.startsWith(runtime+sep))throw Error('path');}
    else throw Error('not found');
    const bytes=await readFile(file);res.writeHead(200,{'Content-Type':({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(file)]||'application/octet-stream'}).end(bytes);
  }catch(error){res.writeHead(404).end(error.message);}});
  await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
  const page=await browser.newPage();page.on('pageerror',e=>console.error('PAGE',e.message));
  await page.goto(`http://127.0.0.1:${server.address().port}`);
  await page.waitForFunction(()=>events.some(e=>e.event==='ready')||events.some(e=>e.event==='error'),{timeout:90000});
  const initial=await page.evaluate(()=>events.filter(e=>e.event==='error'));assert.deepEqual(initial,[]);
  await page.evaluate(async selected=>{for(let i=0;i<selected.length;i++){const file=selected[i],blob=await(await fetch('/asset/'+i)).blob(),url=URL.createObjectURL(blob);try{await rpc('resources',{resources:[{url,path:file.target,bytes:file.bytes,sha256:file.sha256}]});}finally{URL.revokeObjectURL(url);}}},selected);
  await page.evaluate(language=>rpc('configure',{music:'none',language,options:{}}),language);
  if(multiplayer)await page.evaluate(()=>rpc('prepare-room'));
  else {await page.evaluate(()=>rpc('launch'));await page.waitForFunction(()=>events.some(e=>e.event==='first-frame')||events.some(e=>e.event==='error'),{timeout:90000});
    await page.evaluate(()=>rpc('sync'));const result=await page.evaluate(()=>rpc('list'));
    const names={th02:['HUUHI.DAT','HUUMA.CFG'],th03:['YUME.NEM','YUME.CFG'],th04:['GENSOU.SCR','MIKO.CFG']}[game];
    for(const name of names)assert(result.files.some(file=>file.path===name),`missing saved file ${name}`);
  }
  const errors=await page.evaluate(()=>events.filter(e=>e.event==='error'));assert.deepEqual(errors,[]);
  console.log(`PASS ${game} ${language} ${multiplayer?'room preview':'solo first frame'} (${selected.length} files)`);
} finally {await browser?.close();if(server)await new Promise(resolve=>server.close(resolve));await rm(temp,{recursive:true,force:true});}
