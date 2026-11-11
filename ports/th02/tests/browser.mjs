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
const split=process.env.PC98_TEST_SPLIT==='1'?await(await import('../../../tools/ports/pc98-split-harness.mjs')).pc98SplitFixture(portRoot,'th02'):null;
const data=split?.data??await readFile(resolve(portRoot,'content/th02.data'));
let browser,relay,log='';const sockets=[],errors=[],requests=[],failures=[],results=[];
const wait=async(check,label,ms=90000)=>{const end=Date.now()+ms;while(Date.now()<end){if(await check())return;await new Promise(r=>setTimeout(r,100));}throw Error(label+' timeout\n'+log);};
const html=`<!doctype html><style>body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data);});
window.addRuntime=(seat,mp)=>{const f=document.createElement('iframe');f.id='p'+seat;f.src='/runtime/th02.html?runtimeEpoch='+(seat+1)+(mp?'&runtimeVariant=multiplayer':'');document.body.append(f);};
window.rpc=(seat,command,fields={})=>new Promise((yes,no)=>{const f=document.getElementById('p'+seat),request=crypto.randomUUID(),timer=setTimeout(()=>{cleanup();no(Error(command+' timeout'));},120000);function cleanup(){clearTimeout(timer);removeEventListener('message',recv);}function recv(e){if(e.source===f.contentWindow&&e.data?.request===request){cleanup();e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th02',epoch:seat+1,request,command,...fields},location.origin);});</script>`;
const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
  if(path==='/touch-driver.js'){res.setHeader('Content-Type','text/javascript');res.end(touchDriver);return;}
  if(path==='/data'){res.end(data);return;}
  if(split&&path.startsWith('/split/')){const index=Number(path.slice(7));if(!Number.isInteger(index)||!split.buffers[index])throw Error('invalid split resource');res.end(split.buffers[index]);return;}
  if(!path.startsWith('/runtime/')){res.writeHead(404).end();return;}
  const file=resolve(runtime,decodeURIComponent(path.slice(9)));if(!file.startsWith(runtime+sep))throw Error('path');
  res.setHeader('Content-Type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.wasm':'application/wasm','.css':'text/css','.json':'application/json'})[extname(file)]||'application/octet-stream');let bytes=await readFile(file);if(process.env.PC98_TEST_TOUCH==='1'&&path==='/runtime/managed-solo.mjs')bytes=Buffer.concat([bytes,Buffer.from('\nexport function testTouchState(){return readState();}\n')]);res.end(bytes);
}catch(error){res.writeHead(404).end(error.message);}});
await new Promise(r=>server.listen(Number(process.env.PC98_TEST_HTTP_PORT||18382),'127.0.0.1',r));
function connection(url){
  const socket=new WebSocket(url),messages=[],waiters=[];sockets.push(socket);
  socket.addEventListener('message',event=>{const m=JSON.parse(event.data);messages.push(m);for(const waiter of [...waiters])if(waiter.test(m))waiter.resolve(m);});
  return {socket,async send(message,test){messages.length=0;const pending=this.wait(test);socket.send(JSON.stringify(message));return pending;},wait(test){const previous=messages.find(test);if(previous)return Promise.resolve(previous);return new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('lobby timeout '+JSON.stringify(messages)+' '+log)),10000);const waiter={test,resolve(m){clearTimeout(timer);waiters.splice(waiters.indexOf(waiter),1);m.type==='error'?no(Error(m.error)):yes(m);}};waiters.push(waiter);});}};
}
try{
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
  const page=await browser.newPage();await page.setViewport({width:1920,height:500});
  page.on('pageerror',e=>{errors.push(e.message);console.error('PAGE',e.message);});
  page.on('request',r=>requests.push(r.url()));page.on('requestfailed',r=>failures.push({url:r.url(),error:r.failure()?.errorText}));
  page.on('response',r=>{if(r.status()>=400)failures.push({url:r.url(),status:r.status()});});
  if(process.env.PC98_TEST_SIGNAL_DROP==='1')await page.evaluateOnNewDocument(()=>{
    const Native=window.WebSocket;window.__pc98SignalSockets=[];
    window.WebSocket=class extends Native {constructor(url,...args){super(url,...args);if(new URL(url,location.href).searchParams.get('signal')==='1')window.__pc98SignalSockets.push(this);}};
  });
  if(process.env.PC98_TEST_RELAY==='1')await page.evaluateOnNewDocument(()=>{delete window.RTCPeerConnection;});
  if(process.env.PC98_TEST_AUDIO_RATE_FALLBACK==='1')await page.evaluateOnNewDocument(()=>{
    const NativeAudioContext=window.AudioContext;
    window.AudioContext=class extends NativeAudioContext {
      constructor(options){if(options?.sampleRate===44100)throw new DOMException('unsupported sample rate','NotSupportedError');super({sampleRate:48000});}
    };
  });
  page.on('console',m=>{if(m.type()==='error')console.error('CONSOLE',m.text());});
  const load=async(count,mp)=>{
    await page.goto('http://127.0.0.1:'+server.address().port);
    await page.evaluate(({count,mp})=>{for(let i=0;i<count;i++)addRuntime(i,mp);},{count,mp});
    await wait(async()=>{const result=await page.evaluate(count=>({ready:events.filter(e=>e.event==='ready').length===count,errors:events.filter(e=>e.event==='error')}),count);if(result.errors.length)throw Error(JSON.stringify(result.errors));return result.ready;},'Runtime ready');
    if(split)await page.evaluate(async({count,resources})=>{for(let seat=0;seat<count;seat++)for(let i=0;i<resources.length;i++){const f=resources[i],blob=await(await fetch('/split/'+i)).blob(),url=URL.createObjectURL(blob);try{await rpc(seat,'resources',{resources:[{url,path:f.target,bytes:f.bytes,sha256:f.sha256}]});}finally{URL.revokeObjectURL(url);}}},{count,resources:split.resources});
  };
  const installTouch=async(seat=0)=>page.evaluate(async seat=>{
    document.getElementById('testTouchSurface')?.remove();
    const frame=document.getElementById('p'+seat),r=frame.getBoundingClientRect();
    const surface=document.createElement('div');surface.id='testTouchSurface';
    Object.assign(surface.style,{position:'absolute',left:r.left+'px',top:r.top+'px',width:r.width+'px',height:r.height+'px',zIndex:'999',touchAction:'none'});document.body.append(surface);
    const {bindPlayerDirectTouch}=await import('/touch-driver.js');
    bindPlayerDirectTouch({surface,frame,ios:false,allowed:()=>true,zoom:{isActive:()=>false},context:()=>({target:frame.contentWindow,targetOrigin:location.origin,protocol:'eagler-touhou/1',game:'th02',epoch:seat+1,launched:true,ready:true,spectator:false})});
    frame.contentWindow.focus();
  },seat);
  const dragTouch=async(startX,endX,seat=0)=>{
    await installTouch(seat);const cdp=await page.createCDPSession();
    const send=(type,x)=>cdp.send('Input.dispatchTouchEvent',{type,touchPoints:type==='touchEnd'?[]:[{x,y:240,id:1}]});
    await send('touchStart',startX);
    // A real change of focus across the iframe must not cancel hosted touch.
    await page.evaluate(()=>{const b=document.createElement('button');document.body.append(b);b.focus();b.remove();});
    for(let i=1;i<=8;i++){await send('touchMove',startX+(endX-startX)*i/8);await new Promise(r=>setTimeout(r,100));}
    await send('touchEnd',endX);await cdp.detach();
  };
  if(!process.argv.includes('--net-only')){
    await load(1,false);console.log('SOLO ready');
    await page.evaluate(()=>rpc(0,'configure',{music:'ogg',options:{touchEnabled:true,touchSensitivity:150,alwaysHitbox:true}}));
    await page.evaluate(()=>rpc(0,'launch'));console.log('SOLO launch acknowledged');
    await wait(()=>page.evaluate(()=>events.some(e=>e.event==='first-frame')),'solo presentation');
    if(process.env.PC98_TEST_TOUCH==='1'){
      const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));
      const state=()=>frame.evaluate(async()=>(await import('./managed-solo.mjs')).testTouchState());
      for(let i=0;i<40;i++){
        if((await state())?.mode===1)break;
        await page.evaluate(()=>rpc(0,'direct-touch',{type:'down',id:7,x:.5,y:.5}));await new Promise(r=>setTimeout(r,100));
        await page.evaluate(()=>rpc(0,'direct-touch',{type:'up',id:7,x:.5,y:.5}));await new Promise(r=>setTimeout(r,400));
      }
      await wait(async()=>{const s=await state();return s?.mode===1&&s.ticks>60&&!s.hit&&s.x>0;},'touch gameplay',90000);
      await new Promise(r=>setTimeout(r,2000));const before=await state();
      await dragTouch(256,333);await new Promise(r=>setTimeout(r,400));
      const after=await state();console.log('SOLO touch position',before,after);assert.notEqual(after?.x,before?.x,'touch drag moves the native solo player');
      await frame.evaluate(()=>{const bridge=globalThis.__eaglerDirectInputBridge;for(const [type,x] of [['down',.6],['move',.45],['up',.45]])assertBridge(bridge.submit({protocol:'eagler-touhou/1',game:'th02',epoch:1,command:'direct-touch',type,id:9,x,y:.5}));function assertBridge(ok){if(!ok)throw Error('touch bridge refused input');}});
      await new Promise(r=>setTimeout(r,400));const quick=await state();assert.ok(quick.x<after.x,'a swipe released before the next sample still moves the player');
      const preserved=await frame.evaluate(()=>{const b=globalThis.__eaglerDirectInputBridge,base={protocol:'eagler-touhou/1',game:'th02',epoch:1};b.submit({...base,command:'keyboard',code:'KeyZ',key:'z',keyCode:90,down:true});b.submit({...base,command:'touch-cancel'});const bits=globalThis.__eaglerPC98Input.sample();b.submit({...base,command:'keyboard',code:'KeyZ',key:'z',keyCode:90,down:false});return bits;});assert.ok(preserved&32,'touch cancellation keeps a held keyboard action');
    }
    await page.screenshot({path:resolve(out,'solo.png')});
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch='));
    const health=await frame.evaluate(async()=>{const m=await import('./managed-solo.mjs');return m.managedAPI.health();});
    assert(health.frames>0);results.push({solo:health});console.log('SOLO presented',health.frames);
    await page.evaluate(()=>rpc(0,'sync'));const saved=await page.evaluate(()=>rpc(0,'list'));
    assert(saved.files.some(file=>file.path==='HUUHI.DAT'),'native score must be exported');
    assert(saved.files.some(file=>file.path==='HUUMA.CFG'),'native config must be exported');
  }
  if(!process.argv.includes('--solo-only')){
    relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:root,windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_RTC_TIMEOUT_MS:'10000'},stdio:['ignore','pipe','pipe']});
    relay.stdout.on('data',b=>{log+=b;});relay.stderr.on('data',b=>{log+=b;});
    await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(log),'relay startup',15000);
    const relayPort=/listening ws:\/\/127.0.0.1:(\d+)/.exec(log)[1],count=Number(process.env.PC98_TEST_PLAYERS||2),room='th02mp-managed'+Date.now().toString(36),generation='a'.repeat(64),lobbies=[];
    for(let seat=0;seat<count;seat++){
      const client=connection('ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,lobby:'pc98-public-'+seat,member:'pc98-private-'+seat,intent:seat?'join':'create',players:String(count),difficulty:'1'}));lobbies.push(client);
      await client.wait(m=>m.type==='state'||m.type==='error');await client.send({type:'take-seat',seat,loadout:seat%3},m=>!!m.room?.seats[seat]||m.type==='error');
    }
    await load(count,true);console.log('MP ready');
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'configure',{music:'none',options:{multiplayerPreflight:true}});},count);
    const preview=await page.evaluate(()=>rpc(0,'prepare-room'));assert.equal(preview.presented,true);console.log('MP title presented');await page.screenshot({path:resolve(out,'room.png')});
    for(let seat=0;seat<count;seat++)await lobbies[seat].send({type:'set-ready',ready:true,runtimeGeneration:generation},m=>m.room?.seats[seat]?.ready||m.type==='error');
    const start=await lobbies[0].send({type:'start',adonisMode:process.env.PC98_TEST_ROLLBACK==='1'?0:1},m=>m.type==='start'||m.type==='error');
    await page.evaluate(async({count,room,relayPort,serial,generation,rollback})=>Promise.all(Array.from({length:count},async(_,s)=>{
      await rpc(s,'configure',{music:'ogg',options:{touchEnabled:true,touchSensitivity:150,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(serial),member:'pc98-private-'+s,runtimeGeneration:generation}),netplayPlayer:s,netplayPlayerCount:count,netplayDifficulty:1,netplaySeed:1234,netplayAdonisMode:rollback?0:1,netplayLoadouts:Array.from({length:count},(_,i)=>({character:0,shot:i%3}))}});await rpc(s,'launch');
    })),{count,room,relayPort,serial:start.serial,generation,rollback:process.env.PC98_TEST_ROLLBACK==='1'});console.log('MP launch acknowledged');
    const frames=()=>page.frames().filter(f=>/th02.html\?runtimeEpoch=/.test(f.url()));
    const health=()=>Promise.all(frames().map(f=>f.evaluate(async()=>{const {managedAPI}=await import('./netplay/room.js');return managedAPI.health();})));
    await wait(async()=>{const errors=await page.evaluate(()=>events.filter(e=>e.event==='error'));if(errors.length)throw Error(JSON.stringify(errors));const states=await health();return states.length===count&&states.every(s=>s.running&&s.confirmed>=120&&s.frames>0);},'MP confirmed gameplay',120000);
    if(process.env.PC98_TEST_FOCUS_SYNC==='1')await page.evaluate(()=>rpc(0,'keyboard',{code:'ShiftLeft',down:true}));
    if(process.env.PC98_TEST_SIGNAL_DROP==='1'){
      const before=(await health())[0].confirmed;
      await frames()[1].evaluate(()=>{if(!window.__pc98SignalSockets?.length)throw Error('signaling socket not found');window.__pc98SignalSockets[0].close();});
      await wait(async()=>{const states=await health();return states.every(s=>s.running&&!s.stopped&&s.confirmed>=before+180);},'gameplay survives signaling disconnect',20000);
      await wait(()=>frames()[1].evaluate(()=>window.__pc98SignalSockets.length>=2&&window.__pc98SignalSockets.at(-1).readyState===WebSocket.OPEN),'signaling reconnect',10000);
      console.log('PASS gameplay continues after signaling socket closes');
    }
    if(process.env.PC98_TEST_LAG==='1')await wait(async()=>(await health()).every(s=>s.confirmed>=320),'rollback activation',45000);
    if(process.env.PC98_TEST_LAG==='1')await frames()[1].evaluate(()=>{
      const transport=globalThis.__th02PeerTransport,end=performance.now()+5000;
      for(const peer of transport.peers.values()){
        const channel=peer.channel,send=channel.send.bind(channel);
        channel.send=text=>{const data=JSON.parse(text);if(data.type==='input'&&performance.now()<end)setTimeout(()=>{if(channel.readyState==='open')send(text);},70);else send(text);};
      }
    });
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'keyboard',{code:s%2?'ArrowLeft':'ArrowRight',down:true});},count);
    if(process.env.PC98_TEST_LAG==='1')for(let i=0;i<8;i++){await new Promise(r=>setTimeout(r,130));await page.evaluate(async i=>{await rpc(1,'keyboard-clear');await rpc(1,'keyboard',{code:i%2?'ArrowLeft':'ArrowRight',down:true});},i);}
    const before=(await health())[0].confirmed;
    try{await wait(async()=>{const failures=await page.evaluate(()=>events.filter(e=>e.event==='error'));if(failures.length)throw Error(JSON.stringify(failures));return (await health()).every(s=>s.confirmed>=before+360);},'moving players remain synchronized',45000);}catch(error){console.log('Synchronization diagnostic',JSON.stringify(await health()),JSON.stringify(await page.evaluate(()=>events.filter(e=>e.event==='error'))));throw error;}
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'keyboard-clear');},count);
    if(process.env.PC98_TEST_INPUT_LATENCY==='1'){
      const f=frames()[0],position=async()=>(await f.evaluate(async()=>(await import('./netplay/room.js')).managedAPI.sampleGame()))?.players?.[0]?.x;
      await wait(async()=>Number.isFinite(await position()),'latency player state');const samples=[];
      for(let i=0;i<5;i++){const before=await position(),code=before>200?'ArrowLeft':'ArrowRight',started=performance.now();
        await page.evaluate(code=>rpc(0,'keyboard',{code,down:true}),code);const deadline=performance.now()+10000;while(await position()===before){if(performance.now()>deadline)throw Error('input response timeout');await new Promise(r=>setTimeout(r,5));}
        samples.push(performance.now()-started);await page.evaluate(code=>rpc(0,'keyboard',{code,down:false}),code);await new Promise(r=>setTimeout(r,150));}
      console.log('INPUT RESPONSE TH02 (RPC to observed native position, ms)',JSON.stringify(samples));
    }
    if(process.env.PC98_TEST_TOUCH==='1'){
      for(let seat=0;seat<count;seat++){
        const position=async(frame=0)=>{const s=await frames()[frame].evaluate(async()=>(await import('./netplay/room.js')).managedAPI.sampleGame());return s?.players?.[seat]?.x;};
        await wait(async()=>Number.isFinite(await position()),'native player state',30000);
        const before=await position(),direction=before>200?-1:1,offset=seat*640;
        await dragTouch(offset+320,offset+320+direction*96,seat);
        await wait(async()=>direction*((await position())-before)>0,'launcher touch moves multiplayer P'+(seat+1),15000);
        const moved=await position();
        await wait(async()=>{const values=await Promise.all(Array.from({length:count},(_,i)=>position(i)));return values.every(value=>Math.abs(value-moved)<32);},'touch position reaches all peers',15000);
        const quickBefore=await position(),quickDirection=quickBefore>200?-1:1;
        await frames()[seat].evaluate(({seat,quickDirection})=>{
          const b=globalThis.__eaglerDirectInputBridge,base={protocol:'eagler-touhou/1',game:'th02',epoch:seat+1,command:'direct-touch',y:.5};
          for(const [type,id,x] of [['down',7,.5],['move',7,.5+quickDirection*.04],['up',7,.5+quickDirection*.04],['down',8,.5],['up',8,.5]])
            if(!b.submit({...base,type,id,x}))throw Error('touch bridge rejected gesture');
        },{seat,quickDirection});
        try {await wait(async()=>quickDirection*((await position())-quickBefore)>0,'consecutive quick touch moves multiplayer P'+(seat+1),15000);}
        catch(error){console.log('Touch diagnostic',seat,quickBefore,await position(),await frames()[seat].evaluate(async()=>({game:await (await import('./netplay/room.js')).managedAPI.sampleGame(),health:(await import('./netplay/room.js')).managedAPI.health()})));throw error;}
        console.log('PASS TH02 multiplayer touch P'+(seat+1));
      }
    }
    if(process.env.PC98_TEST_LAG==='1'){const counts=await Promise.all(frames().map(f=>f.evaluate(()=>({rollbacks:globalThis.__eaglerNetplayLanRollback,replayed:globalThis.__eaglerNetplayLanResimulated}))));assert(counts.some(v=>v.rollbacks>0&&v.replayed>0),'late input must trigger native rollback');console.log('PASS delayed input rollback',counts);}
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
  await writeFile(resolve(out,'result.json'),JSON.stringify({passed:true,results,failures,scope:'Actual TH02 WASM and supplied local DATA; Launcher wire protocol and main Relay. Full Launcher UI and remote/mobile networks have separate checks.'},null,2));
  console.log('PASS managed TH02',JSON.stringify(results));
}finally{
  await writeFile(resolve(out,'diagnostics.json'),JSON.stringify({errors,failures,log},null,2));
  for(const s of sockets)s.close();await browser?.close();relay?.kill();server.close();
}
