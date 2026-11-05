import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {presentationServer} from '../../scripts/presentation-server.mjs';

const {chromium}=createRequire(process.env.BROWSER_TEST_PACKAGE||resolve(import.meta.dirname,'../../package.json'))('playwright');
assert.ok(process.env.PC_BUILD,'Set PC_BUILD to the private development build');
const host=await presentationServer(0,{artifactDirectory:resolve(process.env.PC_BUILD)});
const browser=await chromium.launch({headless:true,executablePath:process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE,
  args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
try{
  const page=await browser.newPage();
  await page.goto(host.url);
  const result=await page.evaluate(async()=>{
    await openProbe(0,1,2,3,false);
    const states=[];
    for(let cycle=0;cycle<5;++cycle){
      if(!core._th09_probe_return_title(0))throw Error('Return to native title failed');
      states.push(core._th09_probe_menu_hash_isolation());
      if(cycle<4&&!core._th09_probe_open(cycle%2,(cycle+1)%3,2,3))throw Error('Native world construction failed');
    }
    return states;
  });
  assert.deepEqual(result,[1,1,1,1,1]);
  console.log('PASS native menu hash ignores retired world charge queues across five game/title cycles');
}finally{
  await browser.close();host.netplay.close();await new Promise(r=>host.server.close(r));
}
