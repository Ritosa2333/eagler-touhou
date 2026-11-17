import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {writeFileSync,mkdirSync} from 'node:fs';
const out='outputs/th19-network-selection';mkdirSync(out,{recursive:true});
const b=await puppeteer.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,protocolTimeout:240000,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
try{const p=await b.newPage();await p.setViewport({width:1280,height:960});await p.goto('http://127.0.0.1:8148/tests/harness.html?autolaunch=0');await p.waitForFunction(()=>__TH19_HARNESS?.configured,{timeout:180000});
await p.evaluate(async()=>{await __TH19_HARNESS.command('launch');const f=document.createElement('iframe');f.src='/tests/harness.html?autolaunch=0';f.style.display='none';document.body.append(f);window.second=f;});
await p.waitForFunction(()=>second.contentWindow.__TH19_HARNESS?.configured,{timeout:180000});
await p.evaluate(async()=>{await second.contentWindow.__TH19_HARNESS.command('launch');
 const {Netplay}=await import('/runtime/netplay.mjs'),{ConfirmedTimeline}=await import('/runtime/replay-session.mjs');
 const cores=[__TH19_HARNESS.frame.contentWindow.__th19Runtime.core,second.contentWindow.__TH19_HARNESS.frame.contentWindow.__th19Runtime.core];
 window.progress=cores.map(c=>Array.from(c.HEAP32.subarray(c._th19_progress_status()/4,c._th19_progress_status()/4+83)));window.timelines=[];window.peers=cores.map((c,i)=>{c._th19_loop_stop();return new Netplay(c,{onBegin:config=>timelines[i]=new ConfirmedTimeline(config),onConfirmed:bytes=>timelines[i].append(bytes)});});
 const config={rules:[0,1,1,4,4,1,1,0,0],protocol:1,build:'a'.repeat(64),seed:19019,difficulty:2,characters:[18,10],delay:0,rollback:true};
 for(let i=0;i<2;i++){const n=peers[i];Object.assign(n,{config:structuredClone(config),helloConfig:JSON.stringify(config),side:i,delay:0,rollbackEnabled:true,socket:{route:'test'},connected:true});n.send=bytes=>queueMicrotask(()=>peers[1-i].receive(bytes.slice()));n.publish=()=>{};n.sample=()=>({keys:n.testKeys||0,direct:0,x:0,y:0});}
 const text=new TextEncoder().encode(JSON.stringify(config)),packet=new Uint8Array(text.length+6);packet.set([84,49,57,78,1,1]);packet.set(text,6);for(const n of peers)n.receive(packet);
 for(let i=0;i<42;i++){for(const n of peers)n.pumpSelection(n.selectionTick+17);await Promise.resolve();}
 if(timelines.length)throw Error('selection was recorded');
 });await p.screenshot({path:out+'/selection.png'});
const result=await p.evaluate(async()=>{
 for(let i=0;i<150;i++){for(let side=0;side<2;side++){const n=peers[side];n.testKeys=(i===2?(side?64:128):0)|(i===15&&side===0?1:0)|(i===25&&side===0?2:0)|(i===40||i===70?1:0);if(n.selecting)n.pumpSelection(n.selectionTick+17);}await Promise.resolve();if(peers.every(n=>n.active))break;}
 if(!peers.every(n=>n.active))throw Error('selection did not finish '+JSON.stringify(peers.map(n=>({frame:n.selectionFrame,own:n.selectionOwn,peer:n.selectionPeer,info:Array.from(n.core.HEAP32.subarray(n.core._th19_network_select_info()/4,n.core._th19_network_select_info()/4+5))}))));
 if(timelines.some(t=>t.length))throw Error('selection input entered replay');
 for(const n of peers)n.testKeys=0;
 for(let i=0;i<180;i++){for(const n of peers)n.pumpRollback(n.lastTick+17);await Promise.resolve();}
 const {VersusReplayRunner}=await import('/runtime/replay-session.mjs');
 const report={characters:timelines.map(t=>t.config.characters),selectionFrames:peers.map(n=>n.selectionFrame),recorded:timelines.map(t=>t.length)};
 const runner=new VersusReplayRunner(peers[0].core,timelines[0]);runner.reset();while(runner.frame<timelines[0].length)runner.step(false);report.replayed=runner.frame;for(let i=0;i<2;i++){const c=peers[i].core;const current=Array.from(c.HEAP32.subarray(c._th19_progress_status()/4,c._th19_progress_status()/4+83));if(JSON.stringify(current)!==JSON.stringify(progress[i]))throw Error('single-player unlock progress changed');}report.progressPreserved=true;return report;
});writeFileSync(out+'/report.json',JSON.stringify(result,null,2));console.log(result);
}finally{await b.close();}
