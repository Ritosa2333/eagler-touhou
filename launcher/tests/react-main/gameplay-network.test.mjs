import {authoredSourcesPlugin} from './authored-sources.mjs';
/** Actual main native-global/path contracts through fake ports, not live RTC. */
import test,{before,after} from 'node:test';import assert from 'node:assert/strict';import {build} from 'esbuild';import {mkdtemp,rm} from 'node:fs/promises';import {resolve} from 'node:path';import {pathToFileURL} from 'node:url';
let dir,createGameplayNetwork;
before(async()=>{dir=await mkdtemp(resolve('.cache/gameplay-network-'));const outfile=resolve(dir,'model.mjs');await build({entryPoints:['app/models/gameplay-network.ts'],outfile,bundle:true,format:'esm',platform:'node',logLevel:'silent',plugins: [authoredSourcesPlugin(resolve())]});({createGameplayNetwork}=await import(pathToFileURL(outfile).href));});after(async()=>{if(dir)await rm(dir,{recursive:true,force:true});});
function fixture(){
 const updates=new Set(),timeouts=new Map(),intervals=new Map();let serial=0,time=0,doc={};
 const state={epoch:1,game:'th10',launched:true,runtimeInfo:null};const peer={inputOpen:false,controlOpen:false,pc:{connectionState:'connected',iceConnectionState:'connected'}};
 const transport={peers:new Map([[1,peer]]),relay:{readyState:0},failed:false,error:'',rtcReadySent:false};
 const target={Module:{eaglerOptions:{netplayMode:'lan'}},__th10PeerTransport:transport,__eaglerNetplayTransport:'rtc',__eaglerNetplayPath:'direct',__eaglerNetplayLanActive:false};
 const context={product:'th10mp',runtimeVariant:'multiplayer',spectator:false,replayViewer:false,roomCode:'ABCD',player:0,playerCount:2,inputDelay:2,adonisMode:2,adonisSupported:true};
 const runtime={getSnapshot:()=>state,getMidiEventContext:()=>({epoch:state.epoch,document:doc,target}),subscribe(fn){updates.add(fn);return()=>updates.delete(fn);}};
 const model=createGameplayNetwork({runtime,context:()=>context,english:()=>true,translate:(key,p)=>`${key}:${JSON.stringify(p??{})}`,now:()=>time,timers:{setTimeout(fn,ms){const id=++serial;timeouts.set(id,{fn,ms});return id;},clearTimeout(id){timeouts.delete(id);},setInterval(fn,ms){const id=++serial;intervals.set(id,{fn,ms});return id;},clearInterval(id){intervals.delete(id);}}});
 return{model,state,target,context,transport,peer,timeouts,intervals,advance(ms){time+=ms;},poll(){for(const[id,{fn}]of[...timeouts]){timeouts.delete(id);fn();}},replace(){doc={};for(const fn of updates)fn();}};
}
test('room/native transport connection is not ready until all actual input and control channels open',async()=>{const f=fixture();let done=false;const waiting=f.model.waitForGameplayPath({isCurrent:()=>true}).then(()=>done=true);await Promise.resolve();assert.equal(done,false);assert.equal([...f.timeouts.values()][0].ms,250);f.peer.inputOpen=true;f.poll();await Promise.resolve();assert.equal(done,false);f.peer.controlOpen=true;f.poll();await waiting;assert.equal(done,true);f.model.dispose();});
const playback={version:1,run:'run_1',revision:1,min:0,position:600,received:1200,edge:1190,target:900,paused:false,seeking:true,following:false,ended:false,nodes:[]};
test('relay and spectator use actual launched relay socket rather than room probes',async()=>{for(const spectator of [false,true]){const f=fixture();f.context.spectator=spectator;f.state.runtimeInfo={spectatorPlayback:{...playback,seeking:false,position:900}};f.target.__eaglerNetplayTransport='relay';let done=false;const wait=f.model.waitForGameplayPath({isCurrent:()=>true}).then(()=>done=true);f.poll();await Promise.resolve();assert.equal(done,false);f.transport.relay.readyState=1;f.poll();await wait;assert.equal(done,true);f.model.dispose();}});
test('cooperative spectator loading cover waits for a valid completed native restore',async()=>{
 const f=fixture();f.context.spectator=true;f.target.__eaglerNetplayTransport='relay';f.transport.relay.readyState=1;
 let done=false;const wait=f.model.waitForGameplayPath({isCurrent:()=>true}).then(()=>done=true);
 await Promise.resolve();assert.equal(done,false,'an open receiver has not restored a world');
 f.state.runtimeInfo={spectatorPlayback:{...playback,seeking:false,position:2000}};f.poll();await Promise.resolve();assert.equal(done,false,'invalid reports cannot open the loading cover');
 f.state.runtimeInfo={spectatorPlayback:{...playback}};f.poll();await Promise.resolve();assert.equal(done,false,'initial fixed-target reconstruction stays covered');
 f.state.runtimeInfo={spectatorPlayback:{...playback,revision:2,position:800}};f.poll();await Promise.resolve();assert.equal(done,false);
 f.state.runtimeInfo={spectatorPlayback:{...playback,revision:3,position:900,seeking:false}};f.poll();await wait;assert.equal(done,true,'historical playback need not follow live');f.model.dispose();
});
test('TH09 readiness retains its own native timeline path',async()=>{const f=fixture();f.state.game='th09';f.context.product='th09mp';f.context.spectator=true;f.target.__th09PeerTransport=f.transport;f.target.__eaglerNetplayTransport='relay';f.transport.relay.readyState=1;await f.model.waitForGameplayPath({isCurrent:()=>true});f.model.dispose();});
test('ordinary runtime module does not expose MP diagnostics even selected MP metadata exists',()=>{const f=fixture();f.target.Module.eaglerOptions.netplayMode='';f.model.refresh();assert.equal(f.model.readNativeSnapshot(),null);assert.deepEqual(f.model.getSnapshot().diagnostics,{});assert.equal(f.model.getSnapshot().playerStatusVisible,false);f.model.dispose();});
test('failed path rejects original failure, timeout is120s and room/document replacement cancels',async()=>{
 const a=fixture();a.transport.failed=true;a.transport.error='channel failed';await assert.rejects(a.model.waitForGameplayPath({isCurrent:()=>true}),/channel failed/);a.model.dispose();
 const b=fixture();const timeout=b.model.waitForGameplayPath({isCurrent:()=>true});b.advance(120000);b.poll();await assert.rejects(timeout,/gameplayPathTimeout/);b.model.dispose();
 const c=fixture();const old=c.model.waitForGameplayPath({isCurrent:()=>true});c.replace();c.poll();await assert.rejects(old,{name:'AbortError',message:'EAGLER_RUNTIME_SESSION_SUPERSEDED'});c.model.dispose();
});
test('readiness disconnect overlay uses original connectedOnce and exact channel state',()=>{const f=fixture();f.peer.inputOpen=f.peer.controlOpen=true;f.model.refresh();assert.equal(f.model.getSnapshot().connection.hidden,true);f.peer.inputOpen=false;f.model.refresh();assert.equal(f.model.getSnapshot().connection.reconnecting,true);assert.equal(f.model.getSnapshot().returnToRoom,true);f.context.replayViewer=true;f.model.refresh();assert.equal(f.model.getSnapshot().connection,null);assert.equal(f.model.getSnapshot().playerStatusVisible,false);f.model.dispose();});
test('quality uses actual selected RTC pair and late stats cannot affect a replacement document',async()=>{const f=fixture();f.peer.pc.getStats=async()=>new Map([['transport',{type:'transport',selectedCandidatePairId:'pair'}],['pair',{currentRoundTripTime:.077}]]);await f.model.sampleQuality();f.model.refresh();assert.equal(f.model.getSnapshot().playerStatus[0].text,'P2 direct 77ms');assert.equal(f.model.getSnapshot().playerStatus[0].variationMs,null);assert.match(f.model.getSnapshot().diagnostics.runtimeNetplayQualityDiag,/77ms/);
 await f.model.sampleQuality();await f.model.sampleQuality();f.model.refresh();assert.equal(f.model.getSnapshot().playerStatus[0].variationMs,0,'zero variation is displayed only after sufficient real samples');
 let resolveStats;f.peer.pc.getStats=()=>new Promise(r=>resolveStats=r);const stale=f.model.sampleQuality();f.replace();resolveStats(new Map([['pair',{type:'candidate-pair',nominated:true,state:'succeeded',currentRoundTripTime:9}]]));await stale;f.model.refresh();assert.doesNotMatch(f.model.getSnapshot().diagnostics.runtimeNetplayQualityDiag,/9000ms/);f.model.dispose();});
test('disposal rejects pending wait and retires both original250ms/1000ms intervals',async()=>{const f=fixture();assert.deepEqual([...f.intervals.values()].map(x=>x.ms),[250,1000]);const wait=f.model.waitForGameplayPath({isCurrent:()=>true});f.model.dispose();await assert.rejects(wait,{name:'AbortError',message:'EAGLER_RUNTIME_SESSION_SUPERSEDED'});assert.equal(f.timeouts.size,0);assert.equal(f.intervals.size,0);});

test('native peer/path records preserve original array rejection and entry limits',()=>{const f=fixture();f.target.__eaglerNetplayRtcPaths=[[],{peer:1,path:'direct'},{peer:2,path:'turn'},{peer:3,path:'other'}];f.target.__eaglerNetplayLanPeers=[[],{player:0},{player:1},{player:2},{player:3}];const net=f.model.readNativeSnapshot();assert.deepEqual(net.rtcPaths.map(x=>x.peer),[1,2]);assert.deepEqual(net.lanPeers.map(x=>x.player),[0,1,2]);f.model.dispose();});

test('input recovery counters are shown independently of stable RTT and omitted when unsupported',()=>{
 const f=fixture();f.model.refresh();assert.equal(f.model.getSnapshot().diagnostics.runtimeNetplayRecoveryDiag,undefined);
 f.target.__eaglerNetplayInputRepairs=31;f.target.__eaglerNetplayMissingInputs=4;f.model.refresh();
 assert.match(f.model.getSnapshot().diagnostics.runtimeNetplayRecoveryDiag,/diagnostics.inputRecovery.*"repairs":31.*"missing":4/);
 f.context.spectator=true;f.model.refresh();assert.equal(f.model.getSnapshot().diagnostics.runtimeNetplayRecoveryDiag,undefined);f.model.dispose();
});

for (const spectator of [false,true]) test(`native stopped game exposes an actionable return for spectator=${spectator}`,async()=>{
 const f=fixture(); f.context.spectator=spectator; f.target.__eaglerNetplayFailed=true;
 f.target.__eaglerNetplayError='native simulation stopped'; f.model.refresh();
 assert.equal(f.model.getSnapshot().connection.ended,true);
 assert.equal(f.model.getSnapshot().connection.hidden,false);
 assert.equal(f.model.getSnapshot().returnToRoom,true);
 assert.equal(f.model.getSnapshot().calibrationEligible,false);
 assert.equal(f.model.readNativeSnapshot().error,'native simulation stopped');
 await assert.rejects(f.model.waitForGameplayPath({isCurrent:()=>true}),/native simulation stopped/);
 f.model.dispose();
});
