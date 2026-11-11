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
const data=await readFile(resolve(portRoot,'content/th01.data'));
let browser,relay,log='';const sockets=[],errors=[],requests=[],failures=[],results=[];
const wait=async(check,label,ms=90000)=>{const end=Date.now()+ms;while(Date.now()<end){if(await check())return;await new Promise(r=>setTimeout(r,100));}throw Error(label+' timeout\n'+log);};
const html=`<!doctype html><style>body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data);});
window.addRuntime=(seat,mp)=>{const f=document.createElement('iframe');f.id='p'+seat;f.src='/runtime/th01.html?runtimeEpoch='+(seat+1)+(mp?'&runtimeVariant=multiplayer':'')+(window.forceNoWorker?'&netWorker=0':'');document.body.append(f);};
window.rpc=(seat,command,fields={})=>new Promise((yes,no)=>{const f=document.getElementById('p'+seat),request=crypto.randomUUID(),timer=setTimeout(()=>{cleanup();no(Error(command+' timeout'));},120000);function cleanup(){clearTimeout(timer);removeEventListener('message',recv);}function recv(e){if(e.source===f.contentWindow&&e.data?.request===request){cleanup();e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th01',epoch:seat+1,request,command,...fields},location.origin);});</script>`;
const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
  if(path==='/touch-driver.js'){res.setHeader('Content-Type','text/javascript');res.end(touchDriver);return;}
  if(path==='/data'){res.end(data);return;}
  if(/^\/music\/\d{2}\.ogg$/.test(path)){res.setHeader('Content-Type','audio/ogg');res.end(await readFile(resolve(portRoot,'content',path.slice(1))));return;}
  if(!path.startsWith('/runtime/')){res.writeHead(404).end();return;}
  const file=resolve(runtime,decodeURIComponent(path.slice(9)));if(!file.startsWith(runtime+sep))throw Error('path');
  res.setHeader('Content-Type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.wasm':'application/wasm','.css':'text/css','.json':'application/json'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));
}catch(error){res.writeHead(404).end(error.message);}});
await new Promise(r=>server.listen(Number(process.env.PC98_TEST_HTTP_PORT||18381),'127.0.0.1',r));
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
  if(process.env.PC98_TEST_RELAY==='1')await page.evaluateOnNewDocument(()=>{delete window.RTCPeerConnection;});
  if(process.env.PC98_TEST_AUDIO_GATE==='1')await page.evaluateOnNewDocument(()=>{
    const resume=AudioContext.prototype.resume;
    window.__audioResumeAttempts=0;window.__allowAudioResume=false;
    AudioContext.prototype.resume=function(){window.__audioResumeAttempts++;return window.__allowAudioResume?resume.call(this):Promise.reject(new DOMException('User gesture required','NotAllowedError'));};
  });
  page.on('console',m=>{if(m.type()==='error')console.error('CONSOLE',m.text());});
  const load=async(count,mp)=>{
    await page.goto('http://127.0.0.1:'+server.address().port);
    await page.evaluate(({count,mp,noWorker})=>{window.forceNoWorker=noWorker;for(let i=0;i<count;i++)addRuntime(i,mp);},{count,mp,noWorker:process.env.PC98_TEST_NO_WORKER==='1'});
    await wait(async()=>{const result=await page.evaluate(count=>({ready:events.filter(e=>e.event==='ready').length===count,errors:events.filter(e=>e.event==='error')}),count);if(result.errors.length)throw Error(JSON.stringify(result.errors));return result.ready;},'Runtime ready');
  };
  const verifyTouch=async()=>{
    const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));
    await frame.evaluate(()=>{const input=globalThis.__eaglerPC98Input,original=input.pointer.bind(input);globalThis.testMoves=[];globalThis.restoreTestPointer=()=>{input.pointer=original;};input.pointer=message=>{original(message);if(message.type==='move')globalThis.testMoves.push(input.motion());};});
    await page.evaluate(async()=>{
      const frame=document.getElementById('p0'),r=frame.getBoundingClientRect(),surface=document.createElement('div');surface.id='testTouchSurface';
      Object.assign(surface.style,{position:'absolute',left:r.left+'px',top:r.top+'px',width:r.width+'px',height:r.height+'px',zIndex:'999',touchAction:'none'});document.body.append(surface);
      const {bindPlayerDirectTouch}=await import('/touch-driver.js');
      bindPlayerDirectTouch({surface,frame,ios:false,allowed:()=>true,zoom:{isActive:()=>false},context:()=>({target:frame.contentWindow,targetOrigin:location.origin,protocol:'eagler-touhou/1',game:'th01',epoch:1,launched:true,ready:true,spectator:false})});frame.contentWindow.focus();
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
    const health=await frame.evaluate(async()=>{const m=await import('./app.js');return m.managedAPI.health();});
    assert(health.frames>0);assert.equal(health.menu,1,'single player starts at the native title menu');
    await new Promise(r=>setTimeout(r,1000));assert.equal((await frame.evaluate(async()=>(await import('./app.js')).managedAPI.health())).menu,1,'title remains open without input');
    results.push({solo:health});console.log('SOLO presented',health.frames);
    await page.evaluate(()=>rpc(0,'sync'));const saved=await page.evaluate(()=>rpc(0,'list'));
    assert(saved.files.some(file=>file.path==='scores.dat'),'native score must be exported');
    const score=await page.evaluate(()=>rpc(0,'read',{path:'scores.dat'}));for(let at=8;at<808;at+=20)assert.equal(new DataView(Uint8Array.from(score.bytes).buffer).getUint32(at,true),0,'fresh score table is empty');
  }
  if(!process.argv.includes('--solo-only')){
    relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:root,windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_RTC_TIMEOUT_MS:'10000'},stdio:['ignore','pipe','pipe']});
    relay.stdout.on('data',b=>{log+=b;});relay.stderr.on('data',b=>{log+=b;});
    await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(log),'relay startup',15000);
    const relayPort=/listening ws:\/\/127.0.0.1:(\d+)/.exec(log)[1],count=Number(process.env.PC98_TEST_PLAYERS||2),room='th01mp-managed'+Date.now().toString(36),generation='a'.repeat(64),lobbies=[];
    for(let seat=0;seat<count;seat++){
      const client=connection('ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,lobby:'pc98-public-'+seat,member:'pc98-private-'+seat,intent:seat?'join':'create',players:String(count),difficulty:'1'}));lobbies.push(client);
      await client.wait(m=>m.type==='state'||m.type==='error');await client.send({type:'take-seat',seat,loadout:0},m=>!!m.room?.seats[seat]||m.type==='error');
    }
    await lobbies[0].send({type:'settings',playerCount:count,difficulty:1,ballCount:6},m=>m.room?.ballCount===6);
    await load(count,true);console.log('MP ready');
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'configure',{music:'none',options:{multiplayerPreflight:true}});},count);
    const preview=await page.evaluate(()=>rpc(0,'prepare-room'));assert.equal(preview.presented,true);console.log('MP title presented');await page.screenshot({path:resolve(out,'room.png')});
    for(let seat=0;seat<count;seat++)await lobbies[seat].send({type:'set-ready',ready:true,runtimeGeneration:generation},m=>m.room?.seats[seat]?.ready||m.type==='error');
    const start=await lobbies[0].send({type:'start',adonisMode:process.env.PC98_TEST_ROLLBACK==='1'?0:1},m=>m.type==='start'||m.type==='error');
    await page.evaluate(async({count,room,relayPort,serial,generation,rollback})=>Promise.all(Array.from({length:count},async(_,s)=>{
      await rpc(s,'configure',{music:'ogg',options:{netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(serial),member:'pc98-private-'+s,runtimeGeneration:generation}),netplayPlayer:s,musicSource:s===0?'akyu':'original',netplayPlayerCount:count,netplayBallCount:6,netplayDifficulty:1,netplaySeed:1234,netplayAdonisMode:rollback?0:1,netplayLoadouts:Array.from({length:count},(_,i)=>({character:0,shot:0}))}});
      if(s===0)for(let i=1;i<=14;i++){const name=String(i).padStart(2,'0')+'.ogg',response=await fetch('/music/'+name),blob=await response.blob(),bytes=await blob.arrayBuffer(),sha256=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(x=>x.toString(16).padStart(2,'0')).join(''),url=URL.createObjectURL(blob);try{await rpc(s,'resources',{resources:[{url,path:'/bgm/akyu/'+name,bytes:bytes.byteLength,sha256}]});}finally{URL.revokeObjectURL(url);}}
      await rpc(s,'launch');
    })),{count,room,relayPort,serial:start.serial,generation,rollback:process.env.PC98_TEST_ROLLBACK==='1'});console.log('MP launch acknowledged');
    const frames=()=>page.frames().filter(f=>/th01.html\?runtimeEpoch=/.test(f.url()));
    const health=()=>Promise.all(frames().map(f=>f.evaluate(async()=>{const {managedAPI}=await import('./netplay/app.js');return managedAPI.health();})));
    await wait(async()=>{const errors=await page.evaluate(()=>events.filter(e=>e.event==='error'));if(errors.length)throw Error(JSON.stringify(errors));const states=await health();return states.length===count&&states.every(s=>s.running&&s.confirmed>=120&&s.frames>0);},'MP confirmed gameplay',120000);
    if(process.env.PC98_TEST_AUDIO==='1'){
      await wait(async()=>(await Promise.all(frames().map(f=>f.evaluate(()=>{const audio=window.th01Net.session.game.audio;return audio.context?.state==='running'&&!!audio.source;})))).every(Boolean),'music starts without an extra game-screen gesture',30000);
      console.log('PASS TH01 multiplayer music starts automatically');
    }
    if(process.env.PC98_TEST_AUDIO_GATE==='1'){
      for(const [seat,frame] of frames().entries()){
        const attempts=await frame.evaluate(()=>window.__audioResumeAttempts);assert(attempts>0,'launch must attempt audio');
        await frame.evaluate(seat=>{window.__allowAudioResume=true;const bridge=window.__eaglerDirectInputBridge,base={protocol:'eagler-touhou/1',game:'th01',epoch:seat+1,command:'direct-touch',id:77,x:.5,y:.5};if(!bridge.submit({...base,type:'down'})||!bridge.submit({...base,type:'up'}))throw Error('hosted touch rejected');},seat);
        await wait(()=>frame.evaluate(previous=>window.__audioResumeAttempts>previous&&window.th01Net.session.game.audio.context?.state==='running',attempts),'hosted touch resumes blocked audio',10000);
      }
      console.log('PASS TH01 hosted touch unlocks blocked multiplayer audio');
    }
    if(process.env.PC98_TEST_TOUCH==='1')await verifyTouch();
    for(const frame of frames()) assert.equal(await frame.evaluate(()=>window.th01Net.room.settings.count),6,'each peer receives the six-ball rule');
    for(const state of await health())assert.equal(state.rollbackEnabled,process.env.PC98_TEST_ROLLBACK==='1'&&process.env.PC98_TEST_NO_WORKER!=='1','room rollback setting must reach the simulation');
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
    if(process.env.PC98_TEST_LAG==='1'){
      await frames()[1].evaluate(()=>{const peer=window.th01Net.session.peers.get('host'),send=peer.channel.send.bind(peer.channel),end=performance.now()+4000;
        peer.channel.send=text=>{const data=JSON.parse(text);if(data.type==='input'&&performance.now()<end)setTimeout(()=>send(text),70);else send(text);};});
    }
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'keyboard',{code:s%2?'ArrowLeft':'ArrowRight',down:true});},count);
    if(process.env.PC98_TEST_LAG==='1')for(let i=0;i<8;i++){
      await new Promise(resolve=>setTimeout(resolve,130));
      await page.evaluate(async i=>{await rpc(1,'keyboard-clear');await rpc(1,'keyboard',{code:i%2?'ArrowLeft':'ArrowRight',down:true});},i);
    }
    const before=(await health())[0].confirmed;
    await wait(async()=>(await health()).every(s=>s.confirmed>=before+360),'moving players remain synchronized',30000);
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'keyboard-clear');},count);
    const states=await health();
    for(const frame of frames())assert.equal(await frame.evaluate(()=>window.th01Net.session.recorder.error),'','confirmed replay must remain valid during rollback');
    if(process.env.PC98_TEST_LAG==='1'){
      const checked=await frames()[0].evaluate(async()=>{
        const tape=window.th01Net.session.recorder.snapshot(),{NativeSimulation}=await import('./netplay/runtime-core.js');
        const core=(await WebAssembly.instantiateStreaming(fetch('./game.wasm'),{})).instance.exports;core.th_score_data();
        const simulation=new NativeSimulation(core,tape.header.settings,tape.header.hostSlot);simulation.reset();
        for(const [frame,row] of tape.rows.entries()){
          simulation.step(row.inputs);
          if(JSON.stringify(simulation.checksum())!==row.proof)throw Error('confirmed replay mismatch at '+frame);
        }
        return tape.rows.length;
      });
      assert(checked>120,'replay verification covers confirmed gameplay');
      console.log('MP confirmed replay verified',checked);
    }

    if(process.env.PC98_TEST_LAG==='1')assert(states.some(s=>s.rollbackCount>0&&s.replayedFrames>0),'late inputs must trigger actual repair');
    await page.screenshot({path:resolve(out,'multiplayer.png')});
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
  await writeFile(resolve(out,'result.json'),JSON.stringify({passed:true,results,failures,scope:'Actual TH01 WASM and supplied local DATA; Launcher wire protocol and main Relay. Full Launcher UI and remote/mobile networks have separate checks.'},null,2));
  console.log('PASS managed TH01',JSON.stringify(results));
}finally{
  await writeFile(resolve(out,'diagnostics.json'),JSON.stringify({errors,failures,log},null,2));
  for(const s of sockets)s.close();await browser?.close();relay?.kill();server.close();
}
