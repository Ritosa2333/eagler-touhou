import assert from 'node:assert/strict';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve,sep} from 'node:path';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const artifactDirectory=resolve(process.env.PC_BUILD||fileURLToPath(new URL('../../artifacts/sdl3/',import.meta.url)));
const output=resolve(artifactDirectory,'browser/netplay')+sep;mkdirSync(output,{recursive:true});
const wasm=JSON.parse(readFileSync(resolve(artifactDirectory,'build.json'))).sha256;
const {server,url,netplay:relay}=await presentationServer(0,{artifactDirectory}),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),contexts=await Promise.all([browser.newContext({viewport:{width:700,height:620}}),browser.newContext({viewport:{width:700,height:620}})]),pages=await Promise.all(contexts.map(c=>c.newPage())),errors=[],states=[];
for(const p of pages){p.on('pageerror',e=>errors.push(e.stack));p.on('console',e=>{if(e.type()==='error')errors.push(e.text());});}
const state=()=>Promise.all(pages.map(p=>p.evaluate(()=>({status:__th09Runtime.status(),network:__th09Runtime.netplay.info(),rollback:Array.from(__th09Runtime.core.HEAPU32.subarray(__th09Runtime.core._th09_rollback_info()/4,__th09Runtime.core._th09_rollback_info()/4+8)),message:document.querySelector('#network-status').textContent}))));
const progress=setInterval(async()=>{console.log(JSON.stringify(await state().catch(()=>null)));},10000);
const deadline=setTimeout(()=>browser.close(),180000);
const wait=fn=>Promise.all(pages.map(p=>p.waitForFunction(fn,null,{timeout:60000})));
const key=async(side,key)=>{const p=pages[side];await p.keyboard.press(key,{delay:90});const released=await p.evaluate(()=>__th09Runtime.status().title[0]);await p.waitForFunction(f=>__th09Runtime.status().title[0]>=f+3,released);};
async function alignPeers(){
 const counters=await Promise.all(pages.map(p=>p.evaluate(()=>__th09Runtime.netplay.info()[3]))),target=Math.max(...counters)+8;
 await Promise.all(pages.map(p=>p.evaluate(target=>__th09Runtime.core._th09_probe_frame_limit(target),target)));
 const result=await Promise.all(pages.map(p=>p.evaluate(async target=>{const r=__th09Runtime,c=r.core;let confirmed=0;for(let n=0;n<500;++n){if(!c._th09_game_tick(0))throw Error('Alignment simulation failed');confirmed=c.HEAPU32[c._th09_rollback_info()/4+1];if(r.netplay.info()[3]===target&&confirmed===target)break;await new Promise(resolve=>setTimeout(resolve,2));}return {frame:r.netplay.info()[3],confirmed,hash:c._th09_network_hash()>>>0,state:r.status()};},target)));
 for(const s of result){assert.equal(s.frame,target);assert.equal(s.confirmed,target);}
 await Promise.all(pages.map(p=>p.evaluate(()=>__th09Runtime.core._th09_probe_frame_limit(0xffffffff))));return result;
}
try{
 for(const p of pages){await p.goto(url+'/app/th09.html');await p.locator('#start').click();await p.waitForFunction(()=>window.__th09Runtime,{timeout:120000});await p.waitForFunction(()=>__th09Runtime.status().title[0]>30);}
 await pages[0].locator('#network-open').click();await pages[0].locator('#network-create').click();await pages[0].waitForFunction(()=>/房间 [0-9A-F]{12}/.test(document.querySelector('#network-status').textContent));const code=(await pages[0].locator('#network-status').textContent()).match(/[0-9A-F]{12}/)[0];
 await pages[1].locator('#network-open').click();await pages[1].locator('#network-code').fill(code);await pages[1].locator('#network-join').click();await wait(()=>__th09Runtime.netplay.active);await Promise.all(pages.map(p=>p.locator('#network-close').click()));
 await wait(()=>__th09Runtime.status().title[2]===7&&__th09Runtime.status().title[3]===1);await key(0,'KeyZ');await wait(()=>__th09Runtime.status().title[2]===8&&__th09Runtime.status().title[3]===1);
 await key(0,'KeyZ');await key(1,'ArrowRight');await key(1,'KeyZ');await wait(()=>__th09Runtime.status().title[2]===16&&__th09Runtime.status().title[3]===1);await key(0,'KeyZ');await wait(()=>!__th09Runtime.status().title[1]);
 await pages[0].keyboard.down('ControlLeft');await wait(()=>__th09Runtime.status().touch[0]===1);await pages[0].keyboard.up('ControlLeft');await pages[0].keyboard.down('KeyZ');await pages[1].keyboard.down('KeyZ');await pages[0].keyboard.down('ArrowLeft');await pages[1].keyboard.down('ArrowRight');await wait(()=>__th09Runtime.netplay.info()[3]>420);
 await pages[0].evaluate(()=>__th09Runtime.save());
 await pages[0].keyboard.up('ArrowLeft');await pages[1].keyboard.up('ArrowRight');await key(0,'Escape');await wait(()=>__th09Runtime.status().touch[0]===0);states.push({name:'paused',peers:await state()});
 // The shared menu uses the OR of both seats. Let the first Escape release
 // enter confirmed history before pressing Escape on the other seat.
 const released=Math.max(...await Promise.all(pages.map(p=>p.evaluate(()=>__th09Runtime.netplay.info()[3]))))+8;
 await Promise.all(pages.map(p=>p.waitForFunction(f=>__th09Runtime.netplay.info()[3]>=f,released)));
 await key(1,'Escape');await wait(()=>__th09Runtime.status().touch[0]===1);
 // Put both peers at exactly the same input frame and inspect independent C++ worlds.
 let snapshots=await Promise.all(pages.map(p=>p.evaluate(async()=>{const r=__th09Runtime;return await new Promise(resolve=>{const inspect=()=>{if(r.netplay.info()[3]>=720){r.core._th09_loop_pause(1);resolve({frame:r.netplay.info()[3],hash:r.core._th09_network_hash()>>>0,state:r.status()});}else requestAnimationFrame(inspect);};inspect();});})));snapshots=await alignPeers();assert.equal(snapshots[0].frame,snapshots[1].frame);assert.equal(snapshots[0].hash,snapshots[1].hash);assert.equal(snapshots[0].state.session[5],9);states.push({name:'synchronized',snapshots});
 // While both queues are stopped at the same frame, inject only the round
 // outcome to cover original result/recording menus; gameplay equivalence is
 // checked separately by the unforced native demo comparisons.
 for(const p of pages)await p.evaluate(()=>{__th09Runtime.core._th09_probe_end_round(0);__th09Runtime.core._th09_loop_pause(0);__th09Runtime.core._th09_key(29,1);});
 await wait(()=>__th09Runtime.status().session[1]>1100);
 const second=await Promise.all(pages.map(p=>p.evaluate(async()=>{const r=__th09Runtime;return new Promise(resolve=>{const inspect=()=>{if(r.netplay.info()[3]>=1500){r.core._th09_loop_pause(1);resolve(r.netplay.info()[3]);}else requestAnimationFrame(inspect);};inspect();});})));const aligned=await alignPeers();assert.equal(aligned[0].frame,aligned[1].frame);assert.equal(aligned[0].hash,aligned[1].hash);
 for(const p of pages)await p.evaluate(()=>{__th09Runtime.core._th09_probe_end_round(0);__th09Runtime.core._th09_loop_pause(0);__th09Runtime.core._th09_key(29,1);});
 await wait(()=>__th09Runtime.status().session[0]===3);await wait(()=>__th09Runtime.netplay.info()[3]>1900);for(const p of pages){await p.keyboard.up('KeyZ');await p.evaluate(()=>__th09Runtime.core._th09_key(29,0));}
 // Retry replaces the whole world, but keeps the transport/input epoch alive.
 await key(0,'KeyZ');await wait(()=>__th09Runtime.status().session[0]===1&&__th09Runtime.status().session[1]<100);states.push({name:'retry',peers:await state()});
 for(const p of pages)await p.evaluate(()=>__th09Runtime.core._th09_key(29,1));
 await wait(()=>__th09Runtime.status().touch[0]===1&&__th09Runtime.status().session[1]>60);
 await Promise.all(pages.map(p=>p.evaluate(()=>__th09Runtime.core._th09_loop_pause(1))));await alignPeers();
 for(const p of pages)await p.evaluate(()=>{const c=__th09Runtime.core;c._th09_probe_end_round(0);c._th09_probe_end_round(0);c._th09_key(29,1);c._th09_loop_pause(0);});
 await wait(()=>__th09Runtime.status().session[0]===3);for(const p of pages)await p.evaluate(()=>__th09Runtime.core._th09_key(29,0));
 const menuReady=Math.max(...await Promise.all(pages.map(p=>p.evaluate(()=>__th09Runtime.netplay.info()[3]))))+30;
 await Promise.all(pages.map(p=>p.waitForFunction(f=>__th09Runtime.netplay.info()[3]>=f,menuReady)));
 await key(0,'ArrowDown');await key(0,'ArrowDown');await key(0,'KeyZ');await wait(()=>__th09Runtime.status().title[2]===15&&__th09Runtime.status().title[3]===2&&!__th09Runtime.netplay.active);
 await pages[0].waitForFunction(()=>__th09Runtime.status().title[5]>=12);await key(0,'KeyZ');await pages[0].waitForFunction(()=>__th09Runtime.status().title[3]===4);await pages[0].waitForFunction(()=>__th09Runtime.status().title[5]>=11);await key(0,'KeyZ');await pages[0].waitForFunction(()=>__th09Runtime.listFiles().some(f=>f.name==='th9_01.rpy'));
 assert.equal(await pages[1].evaluate(()=>__th09Runtime.listFiles().some(f=>f.name==='th9_01.rpy')),false,'Replay is local to the saving peer');states.push({name:'localReplayAfterMatch',peers:await state()});
 // A settled connection must release its C++ epoch before another room starts.
 for(const p of pages)await p.evaluate(()=>__th09Runtime.core._th09_game_restart());
 await pages[0].locator('#network-open').click();await pages[0].locator('#network-create').click();await pages[0].waitForFunction(()=>/房间 [0-9A-F]{12}/.test(document.querySelector('#network-status').textContent));const nextCode=(await pages[0].locator('#network-status').textContent()).match(/[0-9A-F]{12}/)[0];
 await pages[1].locator('#network-open').click();await pages[1].locator('#network-code').fill(nextCode);await pages[1].locator('#network-join').click();await wait(()=>__th09Runtime.netplay.active);states.push({name:'reconnected',peers:await state()});
 await pages[0].evaluate(()=>__th09Runtime.netplay.close());await wait(()=>!__th09Runtime.netplay.active);
 for(let n=0;n<2;++n)await pages[n].screenshot({path:output+'player-'+n+'.png'});assert.equal(relay.roomCount(),0);assert.equal(errors.length,0,errors.join('\n'));writeFileSync(output+'report.json',JSON.stringify({passed:true,wasm,physicalPhone:false,states,errors},null,2));console.log(JSON.stringify({passed:true,wasm,states}));
}catch(e){writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,errors,states,current:await state().catch(()=>null)},null,2));console.error(e);process.exitCode=1;}
finally{clearInterval(progress);clearTimeout(deadline);await browser.close();relay.close();await new Promise(resolve=>server.close(resolve));}
