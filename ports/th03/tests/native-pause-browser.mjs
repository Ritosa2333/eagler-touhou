import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {createRequire} from 'node:module';
import {spawn} from 'node:child_process';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url)),puppeteer=require('puppeteer-core');
const root=resolve(import.meta.dirname,'../../..'),portRoot=resolve(import.meta.dirname,'..'),runtime=resolve(portRoot,'build-eagler'),out=resolve(portRoot,'.cache/browser');
await mkdir(out,{recursive:true});
const data=await readFile(resolve(portRoot,'content/th03.data'));
let browser,relay,page,log='',battleReplayBytes;const sockets=[],errors=[],requests=[],failures=[],results=[];
const wait=async(check,label,ms=90000)=>{const end=Date.now()+ms;while(Date.now()<end){if(await check())return;await new Promise(r=>setTimeout(r,100));}throw Error(label+' timeout events='+JSON.stringify(await page.evaluate(()=>events))+'\n'+log);};
const html=`<!doctype html><style>body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data);});
window.addRuntime=(seat,mp)=>{const f=document.createElement('iframe');f.id='p'+seat;f.src='/runtime/th03.html?runtimeEpoch='+(seat+1)+(mp?'&runtimeVariant=multiplayer':'');document.body.append(f);};
window.rpc=(seat,command,fields={})=>new Promise((yes,no)=>{const f=document.getElementById('p'+seat),request=crypto.randomUUID(),timer=setTimeout(()=>{cleanup();no(Error(command+' timeout'));},120000);function cleanup(){clearTimeout(timer);removeEventListener('message',recv);}function recv(e){if(e.source===f.contentWindow&&e.data?.request===request){cleanup();e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th03',epoch:seat+1,request,command,...fields},location.origin);});</script>`;
const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
  if(path==='/data'){res.end(data);return;}
  if(!path.startsWith('/runtime/')){res.writeHead(404).end();return;}
  const file=resolve(runtime,decodeURIComponent(path.slice(9)));if(!file.startsWith(runtime+sep))throw Error('path');
  res.setHeader('Content-Type',({'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.wasm':'application/wasm','.css':'text/css','.json':'application/json'})[extname(file)]||'application/octet-stream');let bytes=await readFile(file);if(process.env.PC98_TEST_WORKER==='1'&&path==='/runtime/managed-room.mjs')bytes=Buffer.from(bytes.toString().replace("netWorker:'0'","netWorker:'1'"));res.end(bytes);
}catch(error){res.writeHead(404).end(error.message);}});
await new Promise(r=>server.listen(Number(process.env.PC98_TEST_HTTP_PORT||18383),'127.0.0.1',r));
function connection(url){
  const socket=new WebSocket(url),messages=[],waiters=[];sockets.push(socket);
  socket.addEventListener('message',event=>{const m=JSON.parse(event.data);messages.push(m);for(const waiter of [...waiters])if(waiter.test(m))waiter.resolve(m);});
  return {socket,async send(message,test){messages.length=0;const pending=this.wait(test);socket.send(JSON.stringify(message));return pending;},wait(test,timeout=10000){const previous=messages.find(test);if(previous)return Promise.resolve(previous);return new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('lobby timeout '+JSON.stringify(messages)+' '+log)),timeout);const waiter={test,resolve(m){clearTimeout(timer);waiters.splice(waiters.indexOf(waiter),1);m.type==='error'?no(Error(m.error)):yes(m);}};waiters.push(waiter);});}};
}
try{
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
  page=await browser.newPage();await page.setViewport({width:1920,height:500});
  if(process.env.PC98_TEST_REPLAY_AUDIO==='1')await page.evaluateOnNewDocument(()=>{
    const connect=AudioNode.prototype.connect;
    AudioNode.prototype.connect=function(destination,...args){
      if(destination instanceof AudioDestinationNode&&this instanceof GainNode){const analyser=this.context.createAnalyser();analyser.fftSize=1024;connect.call(this,analyser);connect.call(analyser,destination);(globalThis.__testReplayAnalysers??=[]).push(analyser);return destination;}
      return connect.call(this,destination,...args);
    };
  });
  page.on('pageerror',e=>{errors.push(e.message);console.error('PAGE',e.message);});
  page.on('request',r=>requests.push(r.url()));page.on('requestfailed',r=>failures.push({url:r.url(),error:r.failure()?.errorText}));
  page.on('response',r=>{if(r.status()>=400)failures.push({url:r.url(),status:r.status()});});
  if(process.env.PC98_TEST_RELAY==='1')await page.evaluateOnNewDocument(()=>{delete window.RTCPeerConnection;});
  page.on('console',m=>{if(m.type()==='error')console.error('CONSOLE',m.text());});
  const load=async(count,mp)=>{
    await page.goto('http://127.0.0.1:'+server.address().port);
    await page.evaluate(({count,mp})=>{for(let i=0;i<count;i++)addRuntime(i,mp);},{count,mp});
    await wait(async()=>{const result=await page.evaluate(count=>({ready:events.filter(e=>e.event==='ready').length===count,errors:events.filter(e=>e.event==='error')}),count);if(result.errors.length)throw Error(JSON.stringify(result.errors));return result.ready;},'Runtime ready');
  };
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
    await page.evaluate(async({count,room,relayPort,serial,generation,rollback,disableFocus})=>Promise.all(Array.from({length:count},async(_,s)=>{
      await rpc(s,'configure',{music:'ogg',options:{netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(serial),member:'pc98-private-'+s,runtimeGeneration:generation}),netplayPlayer:s,netplayPlayerCount:count,netplayDisableFocus:disableFocus,netplayDifficulty:1,netplaySeed:1234,netplayAdonisMode:rollback?0:1,netplayLoadouts:Array.from({length:count},(_,i)=>({character:0,shot:0}))}});await rpc(s,'launch');
    })),{count,room,relayPort,serial:start.serial,generation,rollback:process.env.PC98_TEST_ROLLBACK==='1',disableFocus:process.env.PC98_TEST_DISABLE_FOCUS==='1'});console.log('MP launch acknowledged');
    const frames=()=>page.frames().filter(f=>/th03.html\?runtimeEpoch=/.test(f.url()));
    const health=()=>Promise.all(frames().map(f=>f.evaluate(async()=>{const {managedAPI}=await import('./managed-room.mjs');return managedAPI.health();})));
    await wait(async()=>{const errors=await page.evaluate(()=>events.filter(e=>e.event==='error'));if(errors.length)throw Error(JSON.stringify(errors));const states=await health();return states.length===count&&states.every(s=>s.running&&s.confirmed>=120&&s.frames>0);},'MP confirmed gameplay',120000);
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'keyboard',{code:s%2?'ArrowLeft':'ArrowRight',down:true});},count);
    const before=(await health())[0].confirmed;
    await wait(async()=>(await health()).every(s=>s.confirmed>=before+360),'moving players remain synchronized',30000);
    await page.evaluate(async count=>{for(let s=0;s<count;s++)await rpc(s,'keyboard-clear');},count);
    for(let i=0;i<30;i++){
      if((await frames()[0].evaluate(()=>window.th03SyncState?.phase))===1)break;
      await page.evaluate(async()=>{for(let seat=0;seat<2;seat++)await rpc(seat,'keyboard',{code:'KeyZ',down:true});});await new Promise(r=>setTimeout(r,160));
      await page.evaluate(async()=>{for(let seat=0;seat<2;seat++)await rpc(seat,'keyboard',{code:'KeyZ',down:false});});await new Promise(r=>setTimeout(r,500));
    }
    await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.phase))===1,'battle phase',60000);
    for(let seat=0;seat<count;seat++){
      await page.evaluate(seat=>rpc(seat,'keyboard',{code:'KeyZ',down:true}),seat);
      await wait(async()=>!!(await frames()[seat].evaluate(()=>window.__eaglerPC98Input.sample())&512),'shot pressed',5000);
      await page.evaluate(seat=>rpc(seat,'keyboard',{code:'KeyZ',down:false}),seat);
      await wait(async()=>!(await frames()[seat].evaluate(()=>window.__eaglerPC98Input.sample())&512),'shot released',5000);
    }
    console.log('PASS both players release shot');
    if(process.env.PC98_TEST_REPLAY_AUDIO==='1'){
      await page.evaluate(()=>rpc(0,'keyboard',{code:'KeyZ',down:true}));await new Promise(resolve=>setTimeout(resolve,5000));await page.evaluate(()=>rpc(0,'keyboard',{code:'KeyZ',down:false}));
    }
    await page.evaluate(()=>rpc(0,'keyboard',{code:'Space',down:true}));await new Promise(r=>setTimeout(r,1200));
    const focus=await Promise.all(frames().map(f=>f.evaluate(async()=>(await import('./managed-room.mjs')).managedAPI.health().players.find(p=>p.seat===0)?.focused)));
    assert(focus.every(x=>x===(process.env.PC98_TEST_DISABLE_FOCUS!=='1')),'room focus policy controls native player focus');
    await page.evaluate(()=>rpc(0,'keyboard',{code:'Space',down:false}));await new Promise(r=>setTimeout(r,300));
    const send=async(seat,code)=>{await page.evaluate(({seat,code})=>rpc(seat,'keyboard',{code,down:true}),{seat,code});await new Promise(r=>setTimeout(r,160));await page.evaluate(({seat,code})=>rpc(seat,'keyboard',{code,down:false}),{seat,code});};
    await send(0,'Escape');await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.pauseSeat))===0,'left pause',30000);
    await new Promise(r=>setTimeout(r,1500));await page.screenshot({path:resolve(out,'native-left.png')});
    assert((await Promise.all(frames().map(f=>f.evaluate(()=>document.querySelector('#pause-menu').hidden)))).every(Boolean),'pause is rendered by the game');
    await send(0,'ArrowDown');await wait(async()=>(await Promise.all(frames().map(f=>f.evaluate(()=>window.th03SyncState?.pauseSelection)))).every(x=>x===1),'native menu selection',10000);await send(1,'ArrowDown');await new Promise(r=>setTimeout(r,500));assert((await Promise.all(frames().map(f=>f.evaluate(()=>window.th03SyncState?.pauseSelection)))).every(x=>x===1),'only the pausing player controls the menu');await page.screenshot({path:resolve(out,'native-left-down.png')});
    await send(0,'Escape');await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.pauseSeat))===null,'left resume',30000);
    await send(1,'Escape');await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.pauseSeat))===1,'right pause',30000);
    await new Promise(r=>setTimeout(r,1500));await page.screenshot({path:resolve(out,'native-right.png')});const rightCanvas=await (await frames()[1].$('#canvas')).boundingBox();await page.mouse.click(rightCanvas.x+rightCanvas.width*480/640,rightCanvas.y+rightCanvas.height*120/400);await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.pauseSeat))===null,'right resume',30000);
    await send(0,'Escape');await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.pauseSeat))===0,'round menu',30000);await new Promise(r=>setTimeout(r,500));await send(0,'ArrowDown');await new Promise(r=>setTimeout(r,400));await send(0,'KeyZ');
    await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.phase))===2,'round surrender',30000);
    await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.phase))===1,'next round',60000);console.log('PASS native round surrender');
    assert.equal((await page.evaluate(()=>events.filter(e=>e.th03Match))).length,0,'one round is not a match win');
    const roundDecides=process.env.PC98_TEST_ROUND_DECIDES==='1',loser=roundDecides?0:1;
    await send(loser,'Escape');await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.pauseSeat))===loser,'match menu',30000);await new Promise(r=>setTimeout(r,500));await send(loser,roundDecides?'ArrowDown':'ArrowUp');await new Promise(r=>setTimeout(r,400));await send(loser,'KeyZ');
    await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.phase))===2,'match surrender',30000);
    for(let i=0;i<40;i++){
      const active=await Promise.all(frames().map(f=>f.evaluate(()=>window.th03SyncState?.selecting)));
      if(active.every(Boolean))break;
      await send(0,'KeyZ');await new Promise(r=>setTimeout(r,600));
    }
    await wait(async()=>(await Promise.all(frames().map(f=>f.evaluate(()=>window.th03SyncState?.selecting)))).every(Boolean),'match returns to selection',60000);
    await wait(()=>page.evaluate(()=>events.filter(e=>e.th03Match).length===1),'confirmed match outcome',10000);
    if(process.env.PC98_TEST_REPLAY_AUDIO==='1'){
      await wait(()=>page.evaluate(()=>events.some(e=>e.th03Replay?.blob instanceof Blob)),'recorded match Replay',60000);
      battleReplayBytes=await page.evaluate(async()=>Array.from(new Uint8Array(await events.find(e=>e.th03Replay?.blob instanceof Blob).th03Replay.blob.arrayBuffer())));
    }
    assert.equal((await page.evaluate(()=>events.filter(e=>e.th03Match)))[0].th03Match.winner,roundDecides?1:0);
    const match=(await page.evaluate(()=>events.find(e=>e.th03Match))).th03Match;
    const score=await lobbies[0].send({type:'th09-match',serial:start.serial,...match},m=>m.room?.seats?.[match.winner]?.stats?.wins===1);
    assert.equal(score.room.seats[match.winner].stats.wins,1);assert.equal(score.room.seats[match.winner^1].stats.losses,1);
    await lobbies[1].wait(m=>m.room?.seats?.[match.winner]?.stats?.wins===1);
    console.log('PASS native match surrender');
    if(process.env.PC98_TEST_MATCH_SPECTATOR==='1'){
    for(let i=0;i<30;i++){
      if((await frames()[0].evaluate(()=>window.th03SyncState?.phase))===1)break;
      for(let seat=0;seat<2;seat++)await send(seat,'KeyZ');
      await new Promise(r=>setTimeout(r,400));
    }
    await wait(async()=>(await frames()[0].evaluate(()=>window.th03SyncState?.phase))===1,'second match begins',60000);
    await wait(async()=>((await frames()[0].evaluate(()=>window.th03Replay?.snapshot()?.points?.map(p=>p.frame)||[])).length>=3),'second match initializer',90000);
    const matchPoints=await frames()[0].evaluate(()=>window.th03Replay.snapshot().points.map(p=>p.frame));console.log('MATCH_INITIALIZERS',JSON.stringify(matchPoints));
    await new Promise(r=>setTimeout(r,25000));
    const spectator=connection('ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,lobby:'late-spectator',member:'late-spectator-private',intent:'join',players:'2',difficulty:'1'}));
    await spectator.wait(m=>m.type==='state'||m.type==='error');
    await spectator.send({type:'spectate'},m=>m.room?.spectators?.some(p=>p.clientId==='late-spectator')||m.type==='error');
    await spectator.wait(m=>m.type==='spectator-start'||m.type==='error',90000);
    await page.evaluate(()=>addRuntime(2,true));
    await wait(()=>page.evaluate(()=>events.some(e=>e.epoch===3&&e.event==='ready')),'late spectator runtime ready');
    await page.evaluate(async({room,relayPort,serial,generation})=>{await rpc(2,'configure',{music:'none',options:{netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(serial),member:'late-spectator-private',spectator:'late-spectator',runtimeGeneration:generation}),netplayPlayer:0,netplayPlayerCount:2,netplaySpectator:true,netplaySpectatorId:'late-spectator'}});await rpc(2,'launch');},{room,relayPort,serial:start.serial,generation});
    await wait(async()=>page.evaluate(()=>events.some(e=>e.epoch===3&&e.event==='first-frame'||e.epoch===3&&e.event==='error')),'late spectator picture',120000);
    const late=await page.evaluate(()=>({errors:events.filter(e=>e.epoch===3&&e.event==='error'),state:events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback}));
    assert.deepEqual(late.errors,[]);assert.equal(late.state.min,matchPoints.at(-1));assert(late.state.position>=matchPoints.at(-1));
    assert(late.state.nodes.every(node=>node.frame>=late.state.min),'timeline nodes must be within the retained match');
    console.log('PASS late spectator starts second match',JSON.stringify(late.state));
    await page.evaluate(async()=>{await rpc(2,'network-cancel');document.getElementById('p2').remove();});
    }
    const states=await health();await page.screenshot({path:resolve(out,'multiplayer.png')});
    const route=process.env.PC98_TEST_RELAY==='1'?'relay':'rtc';
    assert(log.includes('mode='+route),'must exercise the requested '+route+' route: '+log);
    assert.equal((await page.evaluate(count=>events.filter(e=>e.event==='first-frame'&&e.epoch<=count),count)).length,count);results.push({players:count,states});console.log('MP confirmed gameplay',JSON.stringify(states));
    await page.evaluate(async count=>Promise.all(Array.from({length:count},(_,s)=>rpc(s,'network-cancel'))),count);
    await wait(()=>page.evaluate(count=>events.filter(e=>e.event==='exit'&&e.status==='success'&&e.epoch<=count).length===count,count),'all runtime exits',15000);
    console.log('MP all runtime instances exited');
    if(battleReplayBytes){
      await page.evaluate(()=>addRuntime(2,false));await wait(()=>page.evaluate(()=>events.some(e=>e.event==='ready'&&e.epoch===3)),'Replay runtime ready');
      await page.evaluate(bytes=>rpc(2,'write',{path:'replay/audio.th03r',bytes}),battleReplayBytes);
      const viewer=()=>page.frames().find(f=>f.url().includes('runtimeEpoch=3'));
      for(const music of ['ogg','none']){
        await page.evaluate(music=>rpc(2,'configure',{music,language:'ja',options:{replayViewer:true,musicSource:'original'},resources:[],runtimeResources:[],sharedResources:[]}),music);
        await page.evaluate(()=>rpc(2,'launch',{launchSerial:1}));await page.evaluate(()=>rpc(2,'playback',{action:'open',path:'replay/audio.th03r',stage:0}));
        let musicPeak=0,effectsPeak=0,effectsPlayed=0,musicStarted=false,last;
        await wait(async()=>{
          const result=await viewer().evaluate(async()=>{const h=(await import('./managed-solo.mjs')).managedAPI.health();let peak=0;for(const analyser of globalThis.__testReplayAnalysers||[]){const values=new Float32Array(analyser.fftSize);analyser.getFloatTimeDomainData(values);for(const value of values)peak=Math.max(peak,Math.abs(value));}return {h,peak};});
          last=result.h;musicPeak=Math.max(musicPeak,result.peak);effectsPeak=Math.max(effectsPeak,last.audio.effects?.peak||0);effectsPlayed=Math.max(effectsPlayed,last.audio.effects?.played||0);musicStarted||=!!last.audio.music&&last.audio.musicStatus?.startsWith('原版 FM');
          const errors=await page.evaluate(()=>events.filter(e=>e.event==='error'&&e.epoch===3));if(errors.length)throw Error(JSON.stringify(errors));
          return effectsPlayed>0&&effectsPeak>0.0001&&(music==='none'||musicStarted&&musicPeak>0.0001);
        },`Replay ${music} original audio`,90000);
        if(music==='none')assert.equal(last.audio.music,null,'disabling music must keep effects without starting BGM');
        if(music==='ogg'){
          await page.evaluate(()=>rpc(2,'playback',{action:'pause'}));await wait(async()=>{const a=await viewer().evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.health().audio);return a.state==='suspended'&&a.effectsState==='suspended';},'Replay audio paused',10000);
          await page.evaluate(()=>rpc(2,'playback',{action:'play'}));await wait(async()=>{const a=await viewer().evaluate(async()=>(await import('./managed-solo.mjs')).managedAPI.health().audio);return a.state==='running'&&a.effectsState==='running';},'Replay audio resumed',10000);
        }
        await page.evaluate(()=>rpc(2,'network-cancel'));
      }
      console.log('PASS Replay original FM music, effects, pause and muted-music mode');
    }
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
