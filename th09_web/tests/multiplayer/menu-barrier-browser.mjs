import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
const artifactDirectory=resolve(process.env.PC_BUILD||resolve(root,'artifacts/sdl3'));
const label=process.env.RUN_LABEL||'menu-barrier';assert.match(label,/^[\w-]+$/);
const host=await presentationServer(0,{artifactDirectory});
const browser=await launchBrowser(),page=await browser.newPage(),errors=[];
page.on('pageerror',e=>errors.push(e.stack));
try {
 await page.goto(host.url);await page.evaluate(()=>openProbe(0,1,2,3));
 const result=await page.evaluate(()=>{
  const before=core._th09_probe_state_hash(),at=core._th09_probe_menu_barrier()/4;
  const [passed,cases,retiringActions,unguardedSteps]=core.HEAPU32.slice(at,at+4);
  return {passed,cases,retiringActions,unguardedSteps,restored:before===core._th09_probe_state_hash()};
 });
 assert.equal(result.passed,1);assert.ok(result.restored);assert.equal(result.cases,11520);
 assert.ok(result.retiringActions>100&&result.unguardedSteps>1000);assert.equal(errors.length,0);
 const wasm=JSON.parse(readFileSync(resolve(artifactDirectory,'build.json'))).sha256;
 const report={passed:true,wasm,result,scope:'Actual TH09 menu transition action coverage and exact surrounding state restore, not netplay/lifecycle acceptance'};
 writeFileSync(resolve(root,'artifacts/multiplayer-tests',label+'-report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify(report));
}catch(error){writeFileSync(resolve(root,'artifacts/multiplayer-tests',label+'-failure.json'),JSON.stringify({error:error.stack,errors},null,2));throw error;}
finally{await browser.close();host.netplay.close();await new Promise(r=>host.server.close(r));}
