import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import net from 'node:net';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {mkdirSync,writeFileSync} from 'node:fs';
import {th19StreamBegin,th19StreamEnd,decodeTh19Replay} from '../../../common/contracts/th19-recording.mjs';
const arg=(name,fallback)=>process.argv.find(x=>x.startsWith('--'+name+'='))?.slice(name.length+3)||fallback;
const output=resolve(arg('output','artifacts/spectator'));mkdirSync(output,{recursive:true});
const report={passed:false,errors:[],checks:[]},save=()=>writeFileSync(resolve(output,'spectator-report.json'),JSON.stringify(report,null,2)+'\n');save();
const port=await new Promise(resolve=>{const s=net.createServer();s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(()=>resolve(p));});});
const relay=spawn(process.execPath,[resolve(import.meta.dirname,'../../../services/relay/src/netplay-relay.mjs')],{windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
let log='';relay.stdout.on('data',b=>log+=b);relay.stderr.on('data',b=>log+=b);const sockets=[];
async function wait(check,label){const end=Date.now()+15000;while(!check()){if(Date.now()>end)throw Error(label+' timeout\n'+log);await new Promise(r=>setTimeout(r,10));}}
async function open(query){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${query}`),row={ws,json:[],closed:false};sockets.push(ws);ws.addEventListener('message',e=>{if(typeof e.data==='string')row.json.push(JSON.parse(e.data));});ws.addEventListener('close',()=>row.closed=true);await wait(()=>ws.readyState===1||row.closed,'connect');assert.equal(ws.readyState,1);return row;}
const send=(row,m)=>row.ws.send(JSON.stringify(m)),room=row=>row.json.findLast(m=>m.room)?.room;
const {default:puppeteer}=await import(pathToFileURL(process.env.TH19_PUPPETEER_MODULE||resolve(import.meta.dirname,'../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js')).href);
const browser=await puppeteer.launch({executablePath:process.env.TH19_BROWSER,headless:true,timeout:60000,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
try{
 await wait(()=>log.includes('listening'),'startup');
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));page.setDefaultTimeout(180000);
 const base=arg('url','http://127.0.0.1:8140/');
 await page.goto(new URL('tests/harness.html',base).href,{waitUntil:'domcontentloaded',timeout:120000});await page.waitForFunction(()=>window.__TH19_HARNESS?.frame?.contentWindow?.__th19Runtime);
 const tape=await page.evaluate(async({frames,difficulty})=>{
  const w=window.__TH19_HARNESS.frame.contentWindow,c=w.Module;c._th19_loop_stop();
  const manifest=await(await fetch(new URL('../runtime/manifest.json',location.href))).json();
  const config={rules:[0,1,1,4,4,1,1,3,5],protocol:1,build:manifest.buildId,seed:(190001^Math.imul(1,0x9e3779b9))>>>0,difficulty,characters:[15,7],delay:0,rollback:true};
  c._th19_network_set_rules(...config.rules);c._th19_network_begin(config.seed,config.difficulty,...config.characters);
  const {th19StreamFrame}=await import(new URL('../runtime/th19-recording.mjs',location.href)),rows=[];
  for(let frame=0;frame<frames;frame++){const a={keys:frame%360<270?3:1,direct:+(frame%60===0),x:frame%120===0?1:-1,y:0},b={keys:frame%240<120?1:5,direct:0,x:0,y:0};
   const result=c._th19_network_step(frame,a.keys,b.keys,a.x,a.y,b.x,b.y,a.direct,b.direct,0);rows.push(Array.from(th19StreamFrame(frame,a,b,{result,played:!!c._th19_played_frames(),hash:(frame+1)%120===0?c._th19_network_hash()>>>0:null})));}
  return {config,rows,frames,hash:c._th19_network_hash()>>>0};
 },{frames:Number(arg('frames','1200')),difficulty:Number(arg('difficulty','3'))});
 const id='th19mp-6731',run=`room=${id}&run=1&players=2`;
 const host=await open(`room=${id}&lobby=browser_host&member=member_browser_host`),guest=await open(`room=${id}&lobby=browser_guest&member=member_browser_guest`);
 send(host,{type:'take-seat',seat:0,loadout:0});send(guest,{type:'take-seat',seat:1,loadout:1});await wait(()=>room(host)?.seats?.[0]&&room(host)?.seats?.[1],'seats');
 send(host,{type:'set-ready',ready:true});send(guest,{type:'set-ready',ready:true});await wait(()=>room(host)?.seats.slice(0,2).every(s=>s?.ready),'ready');send(host,{type:'start'});await wait(()=>host.json.some(m=>m.type==='start'),'start');
 const p0=await open(run+'&player=0&member=member_browser_host'),s0=await open(run+'&player=0&signal=1&member=member_browser_host'),s1=await open(run+'&player=1&signal=1&member=member_browser_guest');
 send(s0,{type:'rtc-ready'});send(s1,{type:'rtc-ready'});await wait(()=>s0.json.some(m=>m.type==='route'),'route');
 const upload=bytes=>{const b=new Uint8Array(bytes.length+1);b[0]=0xe8;b.set(bytes,1);p0.ws.send(b);};upload(th19StreamBegin(tape.config));for(const row of tape.rows.slice(0,180))upload(Uint8Array.from(row));
 const late=await open(`room=${id}&lobby=browser_viewer&member=member_browser_viewer`);send(late,{type:'spectate'});await wait(()=>late.json.some(m=>m.type==='spectator-start'),'admission');
 await page.goto(new URL('tests/harness.html?'+new URLSearchParams({difficulty:String(tape.config.difficulty),spectator:'browser_viewer',member:'member_browser_viewer',room:id,relay:`ws://127.0.0.1:${port}/`}),base).href,{waitUntil:'domcontentloaded',timeout:120000});
 await page.waitForFunction(()=>window.__TH19_HARNESS?.frame?.contentWindow?.__th19Runtime?.netplay?.frame>=180);
 await page.evaluate(async()=>{
  const h=window.__TH19_HARNESS,run=h.frame.contentWindow.__th19Runtime.netplay.spectatorRun;
  await h.command('spectator-playback',{run,action:'pause'});await h.command('spectator-playback',{run,action:'seek',frame:0});
 });
 await page.waitForFunction(()=>window.__TH19_HARNESS.frame.contentWindow.__th19Runtime.netplay.frame===0);
 await page.evaluate(()=>{const h=window.__TH19_HARNESS;return h.command('spectator-playback',{run:h.frame.contentWindow.__th19Runtime.netplay.spectatorRun,action:'live'});});
 await page.waitForFunction(()=>window.__TH19_HARNESS.frame.contentWindow.__th19Runtime.netplay.frame>=180);
 report.checks.push('spectator pause, rewind and return-live use retained confirmed input');
 for(const row of tape.rows.slice(180))upload(Uint8Array.from(row));upload(th19StreamEnd(tape.frames));
 await page.waitForFunction(()=>window.__TH19_HARNESS.events.some(e=>e.event==='room-menu'));
 const files=await page.evaluate(async()=>(await window.__TH19_HARNESS.command('list')).files.filter(f=>f.path.endsWith('.rpy')));assert.equal(files.length,1);
 const file=await page.evaluate(path=>window.__TH19_HARNESS.command('read',{path}),files[0].path);const decoded=decodeTh19Replay(Uint8Array.from(file.bytes));assert.equal(decoded.meta.schema,'eagler-touhou/th19-replay/2');assert.equal(decoded.meta.frames,tape.frames);assert.equal(decoded.meta.partial,false);report.checks.push('late spectator catches up and saves a finalized v2 .rpy');
 await page.evaluate(path=>window.__TH19_HARNESS.command('playback',{action:'open',path}),files[0].path);
 await page.evaluate(async frames=>{await window.__TH19_HARNESS.command('playback',{action:'pause'});await window.__TH19_HARNESS.command('playback',{action:'seek',value:frames});},tape.frames);
 await page.waitForFunction(frames=>window.__TH19_HARNESS.frame.contentWindow.__th19Runtime?.player?.state().frame===frames,{},tape.frames);
 assert.equal(await page.evaluate(()=>window.__TH19_HARNESS.frame.contentWindow.Module._th19_network_hash()>>>0),tape.hash);
 await page.evaluate(()=>window.__TH19_HARNESS.command('playback',{action:'seek',value:0}));await page.evaluate(frames=>window.__TH19_HARNESS.command('playback',{action:'seek',value:frames}),tape.frames);
 await page.waitForFunction(frames=>window.__TH19_HARNESS.frame.contentWindow.__th19Runtime?.player?.state().frame===frames,{},tape.frames);
 assert.equal(await page.evaluate(()=>window.__TH19_HARNESS.frame.contentWindow.Module._th19_network_hash()>>>0),tape.hash);report.checks.push('observer saves versus recording; playback and rewind match all native hashes');
 assert.equal(p0.closed,false);assert.deepEqual(report.errors,[]);report.frames=tape.frames;report.difficulty=tape.config.difficulty;report.passed=true;save();console.log(JSON.stringify(report));
}catch(error){report.failure=error.stack;report.log=log;save();throw error;}finally{for(const ws of sockets)ws.close();relay.kill();await browser.close();}
