// Exercises the native frame bridge through the actual Eagler relay, using
// the shared transport on both routes, independently of the Launcher UI.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {readFile} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
import {previewServer} from '../native-solo/web/serve.mjs';

const root=resolve(import.meta.dirname,'../../..');
const {launch}=createRequire(new URL('../../../launcher/package.json',import.meta.url))('puppeteer-core');
const build=resolve(process.argv[2]??resolve(root,'ports/th02/native-solo/build-web'));
const count=Number(process.env.TH02_TEST_SEATS??2);
const route=process.env.TH02_TEST_ROUTE??'rtc';
const generation='a'.repeat(64);
const room=`th02mp-native-${Date.now().toString(36)}`,delay=3;
let port,log='';
const relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{
    cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe'],
    env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',
        EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:'',
        EAGLER_NETPLAY_RELAY_DELAY_MS:route==='relay'?'28':'0',
        EAGLER_NETPLAY_RELAY_JITTER_MS:route==='relay'?'16':'0',
        EAGLER_NETPLAY_RTC_TIMEOUT_MS:'30000'},
});
relay.stdout.on('data',chunk=>{log+=chunk;port=Number(log.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/)?.[1])||port;});
relay.stderr.on('data',chunk=>{log+=chunk;});
const sleep=ms=>new Promise(done=>setTimeout(done,ms));
async function waitFor(check,label){
    const deadline=Date.now()+12000;
    while(!check()){
        if(Date.now()>=deadline)throw Error(label+' timed out\n'+log);
        await sleep(15);
    }
}
const sockets=[];
function lobby(seat){
    const member=`th02_native_member_${seat}`;
    const url=new URL(`ws://127.0.0.1:${port}/`);
    url.search=new URLSearchParams({room,lobby:`th02_native_client_${seat}`,member,players:String(count),difficulty:'1'});
    const ws=new WebSocket(url),messages=[];
    sockets.push(ws);
    ws.onmessage=event=>messages.push(JSON.parse(event.data));
    return {member,messages,send:message=>ws.send(JSON.stringify(message)),
        state:()=>messages.findLast(message=>message.room&&typeof message.room==='object')?.room};
}

const served=new Map([
    ['/peer.mjs',resolve(root,'ports/th02/web/native-coop-peers.mjs')],
    ['/touch-input.js',resolve(root,'ports/th02/web/touch-input.js')],
    ['/full.mjs',resolve(root,'ports/th02/native-solo/web/full.mjs')],
    ['/managed-transport.mjs',resolve(root,'common/browser/managed-transport.mjs')],
    ['/input-transport.mjs',resolve(root,'common/browser/input-transport.mjs')],
    ['/clock.mjs',resolve(root,'ports/th02/native-solo/web/clock.mjs')],
]);
const fixture=`<!doctype html><canvas width="640" height="400" style="width:640px;height:400px"></canvas>
<script type="module">
import {playFull} from './full.mjs';
import {createConfirmedPeerRun} from './peer.mjs';
import {packTouch} from './touch-input.js';
import {createManagedTransport} from './managed-transport.mjs';
import {RetraceClock} from './clock.mjs';
window.launch = async (options, rules) => {
    const seat=options.netplayPlayer, count=options.netplayPlayerCount;
    const clock=new RetraceClock(9328*1000/526315);
    const state=window.run={hashes:{}, error:null, hold:false, samples:0};
    const fail=error=>{state.error=error.message;};
    let peer;
    const player=await playFull(document.querySelector('canvas'),undefined,{
        hosted:true,confirmedCoop:{rules,seat,music:1},
        beforeFrame(){if(peer?.ready&&!state.hold)clock.frame(performance.now(),()=>1,()=>peer.advance());}
    });
    const transport=createManagedTransport(options,['host','guest','guest2'],fail,'TH02');
    state.transport=transport.status;
    peer=createConfirmedPeerRun({
        count,seat,delay:options.netplayInputDelay,identity:[options.netplayUrl.replace(/member=[^&]+/,'member='),rules],
        transport,world:player.confirmed,fail,checkpoint:(frame,hash)=>{state.hashes[frame]=hash;},
        sample(){const f=state.samples++;const buttons=(f%97<27?(seat%2?8:4):0)|(f%47<18?16:0)|(f===180&&seat===1?32:0)|(f%120>65?1024:0);
            return packTouch(buttons,{active:seat===1&&f>=150&&f<180,begin:f===150,x:48,y:-16,unlimited:false});}
    });
    state.status=()=>({frame:peer.frame,samples:state.samples,simulated:player.health().gameFrame,presented:player.health().frames,
        audio:player.health().audio,hash:state.hashes[240],route:window.__eaglerNetplayTransport,error:state.error});
    state.close=()=>{peer.close();player.stop();};
    player.unlock();peer.start();
};
</script>`;
const site=previewServer(build);
const assets=site.listeners('request')[0];
site.removeListener('request',assets);
site.on('request',async(req,res)=>{
    if(req.url==='/test.html'){res.setHeader('Content-Type','text/html');res.end(fixture);}
    else if(served.has(req.url)){res.setHeader('Content-Type','text/javascript');res.end(await readFile(served.get(req.url)));}
    else assets(req,res);
});
let browser;
const errors=[],pages=[];
try{
    await waitFor(()=>port,'relay');
    const clients=[];
    for(let seat=0;seat<count;++seat){
        const client=lobby(seat);clients.push(client);
        await waitFor(()=>client.state(),'lobby');
        client.send({type:'take-seat',seat,loadout:seat,name:'SoEW '+seat});
        await waitFor(()=>client.state()?.seats[seat],'seat');
        if(!seat){
            client.send({type:'settings',playerCount:count,difficulty:1});
            await waitFor(()=>client.state()?.playerCount===count,'capacity');
        }
    }
    for(const c of clients)c.send({type:'set-ready',ready:true,runtimeGeneration:generation,movementMode:'touch'});
    await waitFor(()=>clients[0].state()?.seats.slice(0,count).every(s=>s?.ready),'ready');
    clients[0].send({type:'start',adonisMode:1,inputDelay:0,predictionLimit:0});
    await waitFor(()=>{
        const error=clients[0].messages.find(m=>m.type==='error');
        if(error)throw new Error(JSON.stringify(error));
        return clients[0].messages.some(m=>m.type==='start');
    },'run');
    const run=clients[0].messages.find(m=>m.type==='start').serial;
    await new Promise(done=>site.listen(0,'127.0.0.1',done));
    const url=`http://127.0.0.1:${site.address().port}/test.html`;
    browser=await launch({executablePath:await findChromiumExecutable(),headless:true,
        args:['--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
    for(let seat=0;seat<count;++seat){
        const page=await browser.newPage();
        pages.push(page);
        if(route==='relay')await page.evaluateOnNewDocument(()=>{delete window.RTCPeerConnection;});
        page.on('pageerror',error=>errors.push(String(error)));
    }
    await Promise.all(pages.map(async(page,seat)=>{
        await page.goto(url);
        await page.waitForFunction(()=>typeof window.launch==='function');
        const netplayUrl=`ws://127.0.0.1:${port}/?`+new URLSearchParams({room,run:String(run),member:clients[seat].member,runtimeGeneration:generation});
        const options={netplayUrl,netplayPlayer:seat,netplayPlayerCount:count,netplayInputDelay:delay};
        await page.evaluate(({options,rules})=>window.launch(options,rules),{options,rules:[count,0,1,2,3,0,1,2,739,3,0]});
    }));
    const status=()=>Promise.all(pages.map(page=>page.evaluate(()=>window.run.status())));
    async function waitForRun(reached,label){
        const deadline=Date.now()+12000;
        for(;;){
            const states=await status();
            for(const {error} of states)if(error)throw Error(error);
            if(states.every(reached))return states;
            if(Date.now()>=deadline)throw Error(label+' timed out');
            await sleep(30);
        }
    }
    await waitForRun(state=>state.frame>=50&&state.audio.playing,'first frames and FM playback');
    await pages.at(-1).evaluate(()=>{window.run.hold=true;});
    await sleep(400);
    const stalled=await status();
    await sleep(200);
    const later=await status();
    assert.deepEqual(later.map(s=>s.frame),stalled.map(s=>s.frame),'a missing seat must stall every simulation clock');
    assert(later.every((s,i)=>s.audio.playing&&s.audio.musicTrack===stalled[i].audio.musicTrack),'waiting for inputs must not stop or restart FM');
    assert(stalled.every(s=>s.frame<=stalled.at(-1).samples+delay),'the other seats may only consume previously sent rows');
    assert(stalled.every(s=>s.frame===s.simulated));
    await pages.at(-1).evaluate(()=>{window.run.hold=false;});
    const finished=await waitForRun(state=>state.hash!==undefined,'frame 240 checkpoint');
    assert(finished.every(s=>s.route===route&&s.hash===finished[0].hash&&s.presented>0&&s.frame===s.simulated));
    assert.deepEqual(errors,[]);
    console.log(JSON.stringify({route,count,stalled:stalled.map(s=>s.frame),hash:finished[0].hash}));
}catch(error){
    console.error(log,errors,await Promise.all(pages.map(page=>page.evaluate(()=>({
        status:window.run?.status?.(), ready:window.run?.transport?.rtcReadySent,
        socket:window.run?.transport?.relay?.readyState,
    })))));
    throw error;
}finally{
    if(browser)await browser.close();
    if(site.listening)await new Promise(done=>site.close(done));
    for(const socket of sockets)socket.close();
    relay.kill();
}
