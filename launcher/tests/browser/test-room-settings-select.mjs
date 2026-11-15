import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {authoredSourcesPlugin} from '../../lib/authored-source-resolution.mjs';

const project=fileURLToPath(new URL('../../',import.meta.url));
const fixture=`
import React from 'react';
import {createPortal} from 'react-dom';
import {createRoot} from 'react-dom/client';
import {RoomSettingsDrawer} from './app/components/room/RoomSettingsDrawer.tsx';
import {SettingsBody} from './app/components/settings/SettingsBody.tsx';
import {OptionsPanel} from './app/components/launcher/OptionsPanel.tsx';
import {createLibraryProducts} from './app/components/launcher/products.ts';
import {createGameSettingsModel} from './app/models/game-settings.ts';
import {LocaleProvider} from './app/i18n.tsx';
import './app/ui/foundation.css';
import './app/ui/tokens.css';
import './app/ui/system.css';
import './app/components/room/room-workspace.css';
const productId=new URLSearchParams(location.search).get('game');
const model=createGameSettingsModel({storage:localStorage});
model.hydrate({productId,uiLocale:'ja',languages:[{id:'ja',title:'日本語'},{id:'zh-CN',title:'简体中文'}],
  musicAvailability:{audio:true,remoteOggAdvertised:false},webMidiAvailable:false,mobile:innerWidth<=760});
const noop=()=>{}, midi={enabled:false,outputs:[],selectedId:''};
const actions={externalMidi:{getSnapshot:()=>midi,subscribe:()=>noop,setEnabled:noop}};
const product=createLibraryProducts([productId],noop,noop)[0];
function App(){
  return <LocaleProvider locale="en">
    <main id="player" className="room-workspace"/>
    {createPortal(<RoomSettingsDrawer roomOpen open onCloseRequest={noop}/>,document.body)}
    <OptionsPanel product={product} open={false} roomDrawer onBack={noop} assetUrl={noop}>
      <SettingsBody model={model} actions={actions} onOpenTouchLayout={noop}/>
    </OptionsPanel>
  </LocaleProvider>;
}
createRoot(document.getElementById('root')).render(<App/>);
`;
const {outputFiles}=await build({absWorkingDir:project,stdin:{contents:fixture,loader:'tsx',resolveDir:project},
  bundle:true,format:'esm',jsx:'automatic',write:false,outdir:'.cache/room-settings-select',
  external:['/assets/*','assets/*'],plugins:[authoredSourcesPlugin(project)]});
const routes=new Map([
  ['/', ['text/html; charset=utf-8','<!doctype html><meta name="viewport" content="width=device-width,initial-scale=1"><link rel="stylesheet" href="/fixture.css"><div id="root"></div><script type="module" src="/fixture.js"></script>']],
  ...outputFiles.map(file=>[file.path.endsWith('.js')?'/fixture.js':'/fixture.css',[file.path.endsWith('.js')?'text/javascript':'text/css',file.contents]]),
]);
const server=createServer((request,response)=>{
  const value=routes.get(new URL(request.url,'http://127.0.0.1').pathname);
  if(!value){response.writeHead(404).end();return;}
  response.setHeader('content-type',value[0]);response.end(value[1]);
});
await new Promise(done=>server.listen(0,'127.0.0.1',done));
let browser;
try {
  browser=await puppeteer.launch({headless:true,executablePath:await findChromiumExecutable()});
  for(const [game,width,height] of [['th17mp',1440,900],['th06mp',390,844]]){
    const page=await browser.newPage();
    const mobile=width<760;
    await page.setViewport({width,height,isMobile:mobile,hasTouch:mobile});
    const activate=selector=>mobile?page.tap(selector):page.click(selector);
    await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
    await page.goto(`http://127.0.0.1:${server.address().port}/?game=${game}`);
    await page.waitForSelector('#mpSettingsRoomDrawer:not([hidden])');
    const fields=game==='th17mp'?[['mpRenderResolution','1'],['mpLanguageSelect','zh-CN']]:[['mpLanguageSelect','zh-CN']];
    for(const [id,value] of fields){
      await activate('#'+id+'-trigger');
      const option=`[data-slot="select-content"] [role="option"][data-value="${value}"]`;
      const item=await page.waitForSelector(option,{visible:true,timeout:3000}).catch(()=>null);
      assert(item,`${game} ${id} popup must open from a pointer click`);
      const hit=await page.$eval(option,item=>{
        const rect=item.getBoundingClientRect();
        return item.contains(document.elementFromPoint(rect.x+rect.width/2,rect.y+rect.height/2));
      });
      assert.equal(hit,true,`${game} ${id} popup must be above the room drawer`);
      await activate(option);
      assert.equal(await page.$eval('#'+id,select=>select.value),value);
    }
    await page.close();
  }
  console.log('PASS room settings dropdown pointer selection on desktop and mobile viewports');
} finally {
  await browser?.close();
  server.close();
}
