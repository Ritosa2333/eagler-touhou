import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {createRequire} from 'node:module';
import {spawn} from 'node:child_process';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url)),puppeteer=require('puppeteer-core');
const {build}=require('esbuild');
const touchDriver=(await build({entryPoints:[resolve(import.meta.dirname,'../../../launcher/app/services/player-direct-touch.ts')],bundle:true,format:'esm',platform:'browser',write:false})).outputFiles[0].contents;
const root=resolve(import.meta.dirname,'../../..'),portRoot=resolve(import.meta.dirname,'..'),runtime=resolve(portRoot,'build-eagler'),out=resolve(portRoot,'.cache/browser');
await mkdir(out,{recursive:true});
const split=process.env.PC98_TEST_SPLIT==='1'?await(await import('../../../tools/ports/pc98-split-harness.mjs')).pc98SplitFixture(portRoot,'th03'):null;
const data=split?.data??await readFile(resolve(portRoot,'content/th03.data'));
let browser,relay,page,log='';const sockets=[],errors=[],requests=[],failures=[],results=[];
const wait=async(check,label,ms=90000)=>{const end=Date.now()+ms;while(Date.now()<end){if(await check())return;await new Promise(r=>setTimeout(r,100));}throw Error(label+' timeout events='+JSON.stringify(await page.evaluate(()=>events))+'\n'+log);};
const html=`<!doctype html><style>body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data);});
window.addRuntime=(seat,mp)=>{const f=document.createElement('iframe');f.id='p'+seat;f.src='/runtime/th03.html?runtimeEpoch='+(seat+1)+(mp?'&runtimeVariant=multiplayer':'');document.body.append(f);};
window.rpc=(seat,command,fields={})=>new Promise((yes,no)=>{const f=document.getElementById('p'+seat),request=crypto.randomUUID(),timer=setTimeout(()=>{cleanup();no(Error(command+' timeout'));},120000);function cleanup(){clearTimeout(timer);removeEventListener('message',recv);}function recv(e){if(e.source===f.contentWindow&&e.data?.request===request){cleanup();e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th03',epoch:seat+1,request,command,...fields},location.origin);});</script>`;
const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
  if(path==='/touch-driver.js'){res.setHeader('Content-Type','text/javascript');res.end(touchDriver);return;}
  if(path==='/data'){res.end(data);return;}
  if(split&&path.startsWith('/split/')){const index=Number(path.slice(7));if(!Number.isInteger(index)||!split.buffers[index])throw Error('invalid split resource');res.end(split.buffers[index]);return;}
  if(/^\/music\/\d{2}\.ogg$/.test(path)){res.setHeader('Content-Type','audio/ogg');res.end(await readFile(resolve(portRoot,'content',path.slice(1))));return;}
  if(!path.startsWith('/runtime/')){res.writeHead(404).end();return;}
  const file=resolve(runtime,decodeURIComponent(path.slice(9)));if(!file.startsWith(runtime+sep))throw Error('path');
  res.setHeader('Content-Type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.wasm':'application/wasm','.css':'text/css','.json':'application/json'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));
}catch(error){res.writeHead(404).end(error.message);}});
await new Promise(r=>server.listen(Number(process.env.PC98_TEST_HTTP_PORT||18383),'127.0.0.1',r));
function connection(url){
  const socket=new WebSocket(url),messages=[],waiters=[];sockets.push(socket);
  socket.addEventListener('message',event=>{const m=JSON.parse(event.data);messages.push(m);for(const waiter of [...waiters])if(waiter.test(m))waiter.resolve(m);});
  return {socket,async send(message,test){messages.length=0;const pending=this.wait(test);socket.send(JSON.stringify(message));return pending;},wait(test){const previous=messages.find(test);if(previous)return Promise.resolve(previous);return new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('lobby timeout '+JSON.stringify(messages)+' '+log)),10000);const waiter={test,resolve(m){clearTimeout(timer);waiters.splice(waiters.indexOf(waiter),1);m.type==='error'?no(Error(m.error)):yes(m);}};waiters.push(waiter);});}};
}
try{
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
  page=await browser.newPage();await page.setViewport({width:1920,height:500});
  page.on('pageerror',e=>{errors.push(e.message);console.error('PAGE',e.message);});
  page.on('request',r=>requests.push(r.url()));page.on('requestfailed',r=>failures.push({url:r.url(),error:r.failure()?.errorText}));
  page.on('response',r=>{if(r.status()>=400)failures.push({url:r.url(),status:r.status()});});
  if(process.env.PC98_TEST_RELAY==='1')await page.evaluateOnNewDocument(()=>{delete window.RTCPeerConnection;});
  if(process.env.PC98_TEST_AUDIO_RATE_FALLBACK==='1')await page.evaluateOnNewDocument(()=>{
    if(location.search.includes('runtimeVariant=multiplayer')&&location.search.includes('runtimeEpoch=1'))return;
    const NativeAudioContext=window.AudioContext;
    window.AudioContext=class extends NativeAudioContext {
      constructor(options){if(options?.sampleRate===44100)throw new DOMException('unsupported sample rate','NotSupportedError');super({sampleRate:48000});}
    };
  });
  if(process.env.PC98_TEST_PACKET_LOSS==='1')await page.evaluateOnNewDocument(()=>{
    const send=RTCDataChannel.prototype.send;window.testLoss={enabled:false,sent:0,dropped:0};
    RTCDataChannel.prototype.send=function(data){
      if(window.testLoss.enabled&&this.label.endsWith('-fast')){
        const n=++window.testLoss.sent;
        // 20% scattered loss plus short bursts, in both directions.
        if(n%5===0||n%70<5){window.testLoss.dropped++;return;}
      }
      return send.call(this,data);
    };
  });
  page.on('console',m=>{if(m.type()==='error')console.error('CONSOLE',m.text());});
  const load=async(count,mp)=>{
    await page.goto('http://127.0.0.1:'+server.address().port);
    await page.evaluate(({count,mp})=>{for(let i=0;i<count;i++)addRuntime(i,mp);},{count,mp});
    await wait(async()=>{const result=await page.evaluate(count=>({ready:events.filter(e=>e.event==='ready').length===count,errors:events.filter(e=>e.event==='error')}),count);if(result.errors.length)throw Error(JSON.stringify(result.errors));return result.ready;},'Runtime ready');
    if(split)await page.evaluate(async({count,resources})=>{for(let seat=0;seat<count;seat++)for(let i=0;i<resources.length;i++){const f=resources[i],blob=await(await fetch('/split/'+i)).blob(),url=URL.createObjectURL(blob);try{await rpc(seat,'resources',{resources:[{url,path:f.target,bytes:f.bytes,sha256:f.sha256}]});}finally{URL.revokeObjectURL(url);}}},{count,resources:split.resources});
  };
  const verifyTouch=async()=>{
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));
    await frame.evaluate(()=>{const input=globalThis.__eaglerPC98Input,original=input.pointer.bind(input);globalThis.testMoves=[];globalThis.restoreTestPointer=()=>{input.pointer=original;};input.pointer=message=>{original(message);if(message.type==='move')globalThis.testMoves.push(input.motion());};});
    await page.evaluate(async()=>{
      const frame=document.getElementById('p0'),r=frame.getBoundingClientRect(),surface=document.createElement('div');surface.id='testTouchSurface';
      Object.assign(surface.style,{position:'absolute',left:r.left+'px',top:r.top+'px',width:r.width+'px',height:r.height+'px',zIndex:'999',touchAction:'none'});document.body.append(surface);
      const {bindPlayerDirectTouch}=await import('/touch-driver.js');
      bindPlayerDirectTouch({surface,frame,ios:false,allowed:()=>true,zoom:{isActive:()=>false},context:()=>({target:frame.contentWindow,targetOrigin:location.origin,protocol:'eagler-touhou/1',game:'th03',epoch:1,launched:true,ready:true,spectator:false})});frame.contentWindow.focus();
    });
    const cdp=await page.createCDPSession();await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{x:256,y:240,id:1}]});
    await page.evaluate(()=>{const button=document.createElement('button');document.body.append(button);button.focus();button.remove();});
    await cdp.send('Input.dispatchTouchEvent',{type:'touchMove',touchPoints:[{x:320,y:240,id:1}]});
    await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});await cdp.detach();
    const moves=await frame.evaluate(()=>{globalThis.restoreTestPointer();return globalThis.testMoves;});assert(moves.some(m=>m.active&&m.x>0),'launcher drag survives iframe focus loss');
    await page.evaluate(()=>document.getElementById('testTouchSurface').remove());console.log('PASS launcher touch and focus');
  };
  if(!process.argv.includes('--net-only')){
    await load(1,false);console.log('SOLO ready');
    await page.evaluate(()=>rpc(0,'configure',{music:'ogg',options:{touchEnabled:true,touchSensitivity:150,alwaysHitbox:true}}));
    await page.evaluate(()=>rpc(0,'launch'));console.log('SOLO launch acknowledged');
    await wait(()=>page.evaluate(()=>events.some(e=>e.event==='first-frame')),'solo presentation');
    if(process.env.PC98_TEST_TOUCH==='1')await verifyTouch();
    await page.screenshot({path:resolve(out,'solo.png')});
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch='));
    const health=await frame.evaluate(async()=>{const m=await import('./managed-solo.mjs');return m.managedAPI.health();});
    assert(health.frames>0);results.push({solo:health});console.log('SOLO presented',health.frames);
    await page.evaluate(()=>rpc(0,'sync'));const saved=await page.evaluate(()=>rpc(0,'list'));
    assert(saved.files.some(file=>file.path==='YUME.NEM'),'native score must be exported');
    const score=await page.evaluate(()=>rpc(0,'read',{path:'YUME.NEM'}));
    const {decodeScoreSection}=await import('../web/scores.js');
    const bytes=Uint8Array.from(score.bytes);
    for(let at=0;at<bytes.length;at+=206)assert(decodeScoreSection(bytes.subarray(at,at+206)).subarray(84,184).every(value=>value===32),'fresh score table is empty');
    assert(saved.files.some(file=>file.path==='YUME.CFG'),'native config must be exported');
  }
  if(!process.argv.includes('--solo-only')){
    relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:root,windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_RTC_TIMEOUT_MS:'10000'},stdio:['ignore','pipe','pipe']});
    relay.stdout.on('data',b=>{log+=b;});relay.stderr.on('data',b=>{log+=b;});
    await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(log),'relay startup',15000);
    const relayPort=/listening ws:\/\/127.0.0.1:(\d+)/.exec(log)[1],count=Number(process.env.PC98_TEST_PLAYERS||2),room='th03mp-managed'+Date.now().toString(36),generation='a'.repeat(64),lobbies=[];
    for(let seat=0;seat<count;seat++){
      const client=connection('ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,lobby:'pc98-public-'+seat,member:'pc98-private-'+seat,intent:seat?'join':'create',players:String(count),difficulty:'1'}));lobbies.push(client);
      await client.wait(m=>m.type==='state'||m.type==='error');await client.send({type:'take-seat',seat,loadout:0},m=>!!m.room?.seats[seat]||m.type==='error');
    }
    await load(count,true);console.log('MP ready');
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'configure',{music:'none',options:{multiplayerPreflight:true}});},count);
    const preview=await page.evaluate(()=>rpc(0,'prepare-room'));assert.equal(preview.presented,true);console.log('MP title presented');await page.screenshot({path:resolve(out,'room.png')});
    for(let seat=0;seat<count;seat++)await lobbies[seat].send({type:'set-ready',ready:true,runtimeGeneration:generation},m=>m.room?.seats[seat]?.ready||m.type==='error');
    const start=await lobbies[0].send({type:'start',adonisMode:process.env.PC98_TEST_ROLLBACK==='1'?0:1},m=>m.type==='start'||m.type==='error');
    if(process.env.PC98_TEST_MIXED==='1')await page.evaluate(()=>{document.getElementById('p1').contentWindow.OffscreenCanvas=undefined;});
    await page.evaluate(async({count,room,relayPort,serial,generation,rollback})=>Promise.all(Array.from({length:count},async(_,s)=>{
      await rpc(s,'configure',{music:'ogg',options:{netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(serial),member:'pc98-private-'+s,runtimeGeneration:generation}),netplayPlayer:s,musicSource:s===0?'akyu':'original',netplayPlayerCount:count,netplayDifficulty:1,netplaySeed:1234,netplayAdonisMode:rollback?0:1,netplayLoadouts:Array.from({length:count},(_,i)=>({character:0,shot:0}))}});
      if(s===0)for(let i=1;i<=21;i++){const name=String(i).padStart(2,'0')+'.ogg',response=await fetch('/music/'+name),blob=await response.blob(),bytes=await blob.arrayBuffer(),sha256=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(x=>x.toString(16).padStart(2,'0')).join(''),url=URL.createObjectURL(blob);try{await rpc(s,'resources',{resources:[{url,path:'/bgm/akyu/'+name,bytes:bytes.byteLength,sha256}]});}finally{URL.revokeObjectURL(url);}}
      await rpc(s,'launch');
    })),{count,room,relayPort,serial:start.serial,generation,rollback:process.env.PC98_TEST_ROLLBACK==='1'});console.log('MP launch acknowledged');
    const frames=()=>page.frames().filter(f=>/th03.html\?runtimeEpoch=/.test(f.url()));
    const health=()=>Promise.all(frames().map(f=>f.evaluate(async()=>{const {managedAPI}=await import('./managed-room.mjs');return managedAPI.health();})));
    await wait(async()=>{const errors=await page.evaluate(()=>events.filter(e=>e.event==='error'));if(errors.length)throw Error(JSON.stringify(errors));const states=await health();return states.length===count&&states.every(s=>s.running&&s.confirmed>=120&&s.frames>0);},'MP confirmed gameplay',120000);
    if(process.env.PC98_TEST_TOUCH==='1')await verifyTouch();
    if(process.env.PC98_TEST_PACKET_LOSS==='1'){
      for(let i=0;i<30;i++){
        if(await frames()[0].evaluate(()=>window.th03SyncState?.phase===1))break;
        await page.evaluate(async()=>{for(let seat=0;seat<2;seat++)await rpc(seat,'keyboard',{code:'KeyZ',down:true});});await new Promise(r=>setTimeout(r,160));
        await page.evaluate(async()=>{for(let seat=0;seat<2;seat++)await rpc(seat,'keyboard',{code:'KeyZ',down:false});});await new Promise(r=>setTimeout(r,500));
      }
      await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.phase))===1,'battle phase',60000);
      for(const frame of frames())await frame.evaluate(()=>{window.testLoss.enabled=true;});
      const start=await health();
      for(let i=0;i<5;i++){
        await page.evaluate(async i=>{for(let seat=0;seat<2;seat++){await rpc(seat,'keyboard-clear');await rpc(seat,'keyboard',{code:i%2?'ArrowLeft':'ArrowRight',down:true});}},i);
        await new Promise(r=>setTimeout(r,1000));
      }
      await page.evaluate(async()=>{for(let seat=0;seat<2;seat++)await rpc(seat,'keyboard-clear');});
      const end=await health();
      const drops=await Promise.all(frames().map(f=>f.evaluate(()=>window.testLoss)));
      for(let i=0;i<count;i++){assert(drops[i].dropped>0,'fast lane must actually drop packets');assert(end[i].running&&end[i].confirmed>start[i].confirmed+60,'session must keep confirming frames under loss');}
      console.log('PACKET LOSS',JSON.stringify({drops,start,end}));
    }
    const threadStates=await health();
    assert.equal(threadStates[0].executionThread,'worker','managed TH03 should use the simulation worker');
    assert.equal(threadStates[1].executionThread,process.env.PC98_TEST_MIXED==='1'?'page':'worker','fallback thread selection');
    for(const [index,frame] of frames().entries()){
      const audio=await frame.evaluate(async()=>{
        const version=await import('./bgm-version.js');
        if(version.bgmVersion()!=='new')return {version:version.bgmVersion(),duration:0};
        const manifest=await (await fetch('./bgm/akyu/manifest.json')).json();
        const track=Object.values(manifest.tracks)[0];
        const bytes=await (await fetch('./bgm/'+track.url)).arrayBuffer();
        const context=new AudioContext();const decoded=await context.decodeAudioData(bytes);await context.close();
        return {version:version.bgmVersion(),duration:decoded.duration};
      });
      assert.equal(audio.version,index===0?'new':'old','music choice is local to each player');if(index===0)assert(audio.duration>1,'album track decodes');
    }
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'keyboard',{code:s%2?'ArrowLeft':'ArrowRight',down:true});},count);
    const before=(await health())[0].confirmed;
    await wait(async()=>(await health()).every(s=>s.confirmed>=before+360),'moving players remain synchronized',30000);
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'keyboard-clear');},count);
    const states=await health();await page.screenshot({path:resolve(out,'multiplayer.png')});
    const route=process.env.PC98_TEST_RELAY==='1'?'relay':'rtc';
    assert(log.includes('mode='+route),'must exercise the requested '+route+' route: '+log);
    assert.equal((await page.evaluate(()=>events.filter(e=>e.event==='first-frame'))).length,count);results.push({players:count,states});console.log('MP confirmed gameplay',JSON.stringify(states));
    await page.evaluate(async count=>Promise.all(Array.from({length:count},(_,s)=>rpc(s,'network-cancel'))),count);
    await wait(()=>page.evaluate(count=>events.filter(e=>e.event==='exit'&&e.status==='success').length===count,count),'all runtime exits',15000);
    console.log('MP all runtime instances exited');
  }
  assert.deepEqual(errors,[]);
  assert.deepEqual(failures.filter(f=>!f.url.endsWith('/favicon.ico')&&!f.error?.includes('ERR_ABORTED')),[],'runtime assets including music must load');
  assert(!requests.some(url=>/\/api\//.test(url)),'standalone room APIs must never be requested');
  await writeFile(resolve(out,'result.json'),JSON.stringify({passed:true,results,failures,scope:'Actual TH03 WASM and supplied local DATA; Launcher wire protocol and main Relay. Full Launcher UI and remote/mobile networks have separate checks.'},null,2));
  console.log('PASS managed TH03',JSON.stringify(results));
}finally{
  await writeFile(resolve(out,'diagnostics.json'),JSON.stringify({errors,failures,log},null,2));
  for(const s of sockets)s.close();await browser?.close();relay?.kill();server.close();
}
