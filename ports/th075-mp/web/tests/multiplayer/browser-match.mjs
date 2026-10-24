import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
const root=resolve(import.meta.dirname,'../../..'),launcher=resolve(process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../eagler-touhou'));
const require=createRequire(resolve(launcher,'package.json'));
const {default:puppeteer}=await import(pathToFileURL(require.resolve('puppeteer-core')));
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
import {writeFile,mkdir} from 'node:fs/promises';
const output=resolve(root,'web/artifacts/multiplayer-browser');await mkdir(output,{recursive:true});
const rollback=process.argv.includes('--rollback');
const outputFile=name=>resolve(output,name),finishOnly=process.argv.includes('--finish-only'),disconnect=process.argv.includes('--disconnect'),spectatorFailure=process.argv.includes('--spectator-failure'),target=finishOnly||disconnect||spectatorFailure?400:1800;
const base=process.env.EAGLER_TH075_MP_TEST_RELAY||'ws://127.0.0.1:18376/',room='th075mp-'+String(1000+Math.floor(Math.random()*9000)),sockets=[];
const clients=[0,1,2].map(i=>'mp_test_'+room+'_'+i),members=clients.map(id=>'member_'+id);
function next(socket,predicate=()=>true){return new Promise((resolve,reject)=>{
 const cleanup=()=>{clearTimeout(timer);socket.removeEventListener('message',message);socket.removeEventListener('close',closed);socket.removeEventListener('error',closed);};
 const closed=()=>{cleanup();reject(Error('Lobby connection ended: '+socket.url));};
 const message=event=>{const m=JSON.parse(event.data);if(m.type==='error'){cleanup();reject(Error(m.error));}else if(predicate(m)){cleanup();resolve(m);}};
 const timer=setTimeout(()=>{cleanup();reject(Error('Lobby timeout: '+socket.url));},10000);
 socket.addEventListener('message',message);socket.addEventListener('close',closed);socket.addEventListener('error',closed);
});}

async function send(s,m,p){const wait=next(s,p);s.send(JSON.stringify(m));return wait;}
let browser;
try{
 for(let i=0;i<3;i++){const s=new WebSocket(base+'?room='+room+'&lobby='+clients[i]+'&member='+members[i]+(i===0?'&intent=create':''));sockets.push(s);await next(s,m=>m.type==='state');if(i<2)await send(s,{type:'take-seat',seat:i,loadout:i===0?0:18},m=>m.room?.seats[i]?.clientId===clients[i]);else await send(s,{type:'spectate'},m=>m.room?.spectators?.some(v=>v.clientId===clients[i]));}
 for(let seat=0;seat<2;seat++)await send(sockets[seat],{type:'set-ready',ready:true},m=>m.room?.seats?.[seat]?.clientId===clients[seat]&&m.room.seats[seat].ready);
 const start=await send(sockets[0],{type:'start',adonisMode:rollback?0:1,predictionLimit:rollback?8:undefined,inputDelay:rollback?0:2,inputDelayAuto:false},m=>m.type==='start');console.log('room started',start.serial,start.room.seed);
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:[...(process.env.EAGLER_TH075_BROWSER_GPU==='hardware'?['--use-angle=d3d11']:['--use-angle=swiftshader','--enable-unsafe-swiftshader']),'--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 const pages=await Promise.all([0,1,2].map(async i=>{const c=await browser.createBrowserContext(),p=await c.newPage();if(process.argv.includes('--relay'))await p.evaluateOnNewDocument(()=>{globalThis.RTCPeerConnection=undefined;});p.on('console',m=>{if(m.type()==='error')console.log('browser',i,m.text());});p.on('pageerror',e=>console.log('pageerror',i,e.message));await p.goto(process.env.EAGLER_TH075_MP_TEST_URL||'http://127.0.0.1:18375/');await p.waitForFunction(()=>host.events.some(e=>e.event==='ready')||host.error(),{timeout:120000});return p;}));
 await Promise.all(pages.map((p,i)=>p.evaluate(async({i,run,seed,room,relay,client,member,rollback})=>{if(host.error())throw Error(JSON.stringify(host.error()));await host.request('configure',{language:'ja',music:'none',options:{netplayMode:'lan',netplayUrl:`${relay}?room=${room}&run=${run}&${i===2?'spectator='+client:'player='+i}&players=2&member=${member}`,netplaySpectator:i===2,netplaySpectatorId:i===2?client:'',netplaySpectatorCount:1,netplayPlayer:i===2?null:i,netplayPlayerCount:2,netplaySeed:seed,netplayDifficulty:1,netplayAdonisMode:rollback?0:1,netplayPredictionLimit:rollback?8:0,netplayInputDelay:rollback?0:2,netplayInputDelayAuto:false,netplayLoadouts:[{character:0,shot:0},{character:2,shot:0}],netplayIceServers:[],multiplayerLocalPlayerVisibility:true}});await host.request('launch');},{i,run:start.serial,seed:1947,room,relay:base,client:clients[i],member:members[i],rollback})));
 if(rollback)await Promise.all(pages.slice(0,2).map(p=>p.evaluate(()=>{const w=document.querySelector('iframe').contentWindow,t=w.__eaglerPeerTransport,receive=t.receiveBinary.bind(t);let packets=0;w.__rollbackFault={delayed:0,dropped:0};t.receiveBinary=function(data){const b=(data instanceof ArrayBuffer||data instanceof w.ArrayBuffer)?new Uint8Array(data):ArrayBuffer.isView(data)?new Uint8Array(data.buffer,data.byteOffset,data.byteLength):null;if(b&&b[4]===4&&b[5]===1&&host.state()?.[9]===1&&host.state()?.[2]>220){++packets;if(packets%23===0){++w.__rollbackFault.dropped;return;}++w.__rollbackFault.delayed;setTimeout(()=>receive(data),25+(packets%5)*15);}else receive(data);};host.checkpoints=new Map();const observe=()=>{const s=host.state();if(s&&s[53]!==0xffffffff)host.checkpoints.set(s[53],s[54]);requestAnimationFrame(observe);};observe();})));
 if(rollback)await pages[2].evaluate(()=>{host.checkpoints=new Map();const observe=()=>{const s=host.state();if(s&&s[53]!==0xffffffff)host.checkpoints.set(s[53],s[54]);requestAnimationFrame(observe);};observe();});
 let moved=false,movingAt=0,initialX=[];const asFloat=n=>new Float32Array(new Uint32Array([n]).buffer)[0];
 const states=[],matchDeadline=Date.now()+240000;for(let n=0;Date.now()<matchDeadline;n++){const s=await Promise.all(pages.map(p=>p.evaluate(()=>({s:host.state(),error:host.error(),route:document.querySelector('iframe').contentWindow.__eaglerNetplayTransport,events:host.events.filter(e=>e.event==='error')}))));if(n%20===0)console.log('state',s.map(v=>({frame:v.s[2],route:v.route,error:v.error})));if(s.some(v=>v.error))throw Error(JSON.stringify(s));states.push(s);if(s.every(v=>v.s[2]>=target))break;if(n===80){await pages[2].evaluate(()=>host.key('ArrowUp',true));await pages[0].evaluate(()=>host.key('Escape',true));}if(n===83)await pages[0].evaluate(()=>host.key('Escape',false));if(n===90)assert.ok(s.every(v=>v.s[10]===1),'pause did not synchronize');if(n===95)await pages[1].evaluate(()=>host.key('Escape',true));if(n===98)await pages[1].evaluate(()=>host.key('Escape',false));if(n===110)assert.ok(s.every(v=>v.s[10]===0),'resume did not synchronize');if(n===120)await pages[0].evaluate(()=>document.querySelector('iframe').contentWindow.Module._th075_mp_restart());if(n===130)assert.ok(s.every(v=>v.s[8]===1),'restart did not synchronize');if(!movingAt&&s.slice(0,2).every(v=>v.s[12]>220)){
initialX=[s[0].s[16],s[0].s[32]];movingAt=Math.max(s[0].s[2],s[1].s[2]);
await pages[0].evaluate(()=>host.key('ArrowRight',true));await pages[1].evaluate(()=>host.key('ArrowLeft',true));
}
if(movingAt&&!moved&&s.slice(0,2).every(v=>v.s[2]>=movingAt+60)){
assert.ok(asFloat(s[0].s[16])>asFloat(initialX[0])+5,'P1 right input did not move its fighter');
assert.ok(asFloat(s[0].s[32])<asFloat(initialX[1])-5,'P2 left input did not move its fighter');
await pages[0].evaluate(()=>host.key('ArrowRight',false));await pages[1].evaluate(()=>host.key('ArrowLeft',false));moved=true;
console.log('PASS assigned-seat movement',initialX.map(asFloat),[s[0].s[16],s[0].s[32]].map(asFloat));
}
if(moved&&n>25&&!(n>=75&&n<=110)&&n%3===0)await Promise.all(pages.slice(0,2).map((p,i)=>p.evaluate(async({n,i})=>{for(const code of ['KeyZ','KeyX','KeyC','KeyV'])await host.key(code,false);await host.key(['KeyZ','KeyX','KeyC','KeyV'][Math.floor(n/3+i)%4],n%2===0);},{n,i})));await new Promise(r=>setTimeout(r,200));}
 assert.ok(moved,'match ended before movement acceptance');
 let comparisons=[];
 if(rollback){const checkpoints=await Promise.all(pages.map(p=>p.evaluate(()=>Array.from(host.checkpoints))));const maps=checkpoints.map(v=>new Map(v));for(let j=1;j<3;++j){let same=0;for(const [f,h]of maps[0])if(maps[j].has(f)){assert.equal(maps[j].get(f),h,'confirmed checkpoint mismatch '+f);++same;}assert.ok(same>=3,'insufficient confirmed checkpoints');comparisons.push(same);}
 const stats=await Promise.all(pages.slice(0,2).map(p=>p.evaluate(()=>({state:host.state().slice(48,55),fault:document.querySelector('iframe').contentWindow.__rollbackFault}))));assert.ok(stats.every(v=>v.state[0]>0&&v.state[1]>0&&v.fault.delayed>0&&v.fault.dropped>0),JSON.stringify(stats));console.log('PASS actual rollback correction and confirmed spectator hashes',JSON.stringify(stats));
 }else{const histories=await Promise.all(pages.map(p=>p.evaluate(()=>Array.from(host.history))));const maps=histories.map(h=>new Map(h));for(let j=1;j<3;j++){let same=0;for(const [f,v]of maps[0])if(maps[j].has(f)){assert.deepEqual(maps[j].get(f),v,'world divergence at '+f+' peer '+j);++same;}assert.ok(same>0,'no shared frame samples for peer '+j);comparisons.push(same);}}
 console.log('matching observed confirmed frames vs P1',comparisons);if(spectatorFailure){
const before=await Promise.all(pages.slice(0,2).map(p=>p.evaluate(()=>host.state()[2])));
await pages[2].evaluate(()=>document.querySelector('iframe').contentWindow.__eaglerPeerTransport.relay.close(4008,'Acceptance spectator failure'));
await pages[2].waitForFunction(()=>!!host.error(),{timeout:10000});
await Promise.all(pages.slice(0,2).map((p,i)=>p.waitForFunction(n=>host.state()[2]>n+90&&!host.error(),{timeout:10000},before[i])));
console.log('PASS broken spectator fails explicitly while both players advance');
}else if(disconnect){
await pages[1].evaluate(()=>document.querySelector('iframe').contentWindow.__eaglerPeerTransport.fail('Acceptance disconnect'));
await pages[0].waitForFunction(()=>host.state()[1]===4,{timeout:10000});
const frozen=await pages[0].evaluate(()=>host.state());await new Promise(r=>setTimeout(r,1000));
assert.deepEqual(await pages[0].evaluate(()=>host.state()),frozen,'disconnected world continued or reset');
await writeFile(outputFile('disconnect.json'),JSON.stringify({frame:frozen[2],state:frozen}));
console.log('PASS shared disconnect notification freezes the retained world');
}else{
const last=states.at(-1);await Promise.all(pages.slice(0,2).map(p=>p.evaluate(()=>host.request('keyboard-clear'))));
await pages[0].evaluate(()=>host.key('Escape',true));
await Promise.all(pages.slice(0,2).map(p=>p.waitForFunction(()=>host.state()[10]===1,{timeout:15000})));
await pages[0].evaluate(()=>host.key('Escape',false));
await pages[0].evaluate(()=>host.key('ArrowDown',true));await pages[0].waitForFunction(()=>host.state()[13]===1,{timeout:5000});await pages[0].evaluate(()=>host.key('ArrowDown',false));await new Promise(r=>setTimeout(r,100));
await pages[0].evaluate(()=>host.key('KeyZ',true));await pages[0].waitForFunction(()=>host.state()[9]===6||host.state()[15]||host.events.some(e=>e.event==='exit'),{timeout:15000});await pages[0].evaluate(()=>host.key('KeyZ',false));
await Promise.all(pages.map(p=>p.waitForFunction(()=>document.querySelector('iframe').contentDocument.querySelector('#th075ReplaySavePrompt[open]')||host.events.some(e=>e.event==='exit')||host.error(),{timeout:40000})));
for(const p of pages.slice(0,2)){
 const downloads=await p.evaluate(async()=>{const w=document.querySelector('iframe').contentWindow,c=w.Module,dialog=w.document.querySelector('#th075ReplaySavePrompt[open]');if(!dialog)throw Error('Missing completed Replay prompt');return Promise.all([...dialog.querySelectorAll('a[download]')].map(async a=>({name:a.download,download:Array.from(new Uint8Array(await(await fetch(a.href)).arrayBuffer())),stored:Array.from(c.FS.readFile('/save/replay/'+a.download))})));});
 assert.ok(downloads.length);for(const file of downloads){assert.ok(file.download.length>64);assert.deepEqual(file.download,file.stored,'Replay download differs from native file');}
 await p.evaluate(()=>document.querySelector('iframe').contentDocument.querySelector('#th075ReplayReturn').click());
}
assert.equal(await pages[2].evaluate(()=>!!document.querySelector('iframe').contentDocument.querySelector('#th075ReplaySavePrompt')),false,'spectator must not save a player Replay');
await Promise.all(pages.map(p=>p.waitForFunction(()=>host.events.some(e=>e.event==='exit')||host.error(),{timeout:40000})));
assert.ok((await Promise.all(pages.map(p=>p.evaluate(()=>host.error())))).every(v=>!v));console.log('PASS synchronized battle exit, terminal fence and spectator finish');
const replays=await Promise.all(pages.slice(0,2).map(p=>p.evaluate(()=>{const c=document.querySelector('iframe').contentWindow.Module;return c.FS.readdir('/save/replay').filter(n=>n.endsWith('.rep')).map(n=>({name:n,bytes:Array.from(c.FS.readFile('/save/replay/'+n))}));})));assert.ok(replays[0].length>=(finishOnly?1:2));await writeFile(outputFile('replays.json'),JSON.stringify(replays));assert.deepEqual(replays[0],replays[1],'completed native replay bytes diverged');console.log('identical completed native replay; current files',replays[0].map(v=>[v.name,v.bytes.length]));assert.ok(last.every(v=>v.s[2]>=target),JSON.stringify(last));assert.ok(last.every(v=>v.s[1]===3));await pages[0].screenshot({path:outputFile('match.png')});await writeFile(outputFile('browser-match.json'),JSON.stringify({comparisons,states},null,2));console.log('PASS actual two-runtime match, frames',last.map(v=>v.s[2]),'routes',last.map(v=>v.route));
await pages[0].reload();await pages[0].waitForFunction(()=>host.events.some(e=>e.event==='ready')||host.error(),{timeout:120000});
const persisted=await pages[0].evaluate(()=>{const c=document.querySelector('iframe').contentWindow.Module;return c.FS.readdir('/save/replay').filter(n=>n.endsWith('.rep')).map(name=>({name,bytes:Array.from(c.FS.readFile('/save/replay/'+name))}));});
assert.deepEqual(persisted,replays[0]);console.log('PASS Replay prompt, exact download bytes and fresh Runtime IDBFS persistence');
}
}catch(e){console.log('FAIL',e.message);for(const p of await browser?.pages()||[])console.log('diagnostic',await p.evaluate(()=>typeof host==='undefined'?null:({s:host.state(),e:host.error(),events:host.events.slice(-8),scene:document.querySelector('iframe').contentWindow.Module?._th075_scene(),phase:document.querySelector('iframe').contentWindow.Module?._th075_phase()})).catch(()=>null));throw e;}finally{for(const s of sockets)s.close();await browser?.close();}
