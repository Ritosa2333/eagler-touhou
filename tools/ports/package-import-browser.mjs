import assert from 'node:assert/strict';
import {randomUUID} from 'node:crypto';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {launchLauncherRuntime} from './launcher-runtime-start.mjs';
import {snapshotPackageRuntime} from './offline-installation-authority.mjs';
import {readPackageImportBefore,readPackageImportState} from './package-import-parity.mjs';
import {chooseSetting} from './launcher-ui-controls.mjs';

/** Each source owns a fresh browser store; the second document reuses that store. */
export async function runPackageImportRoutes({browser,game,localBase,onlineBase,originalBase,archiveIdentity,originalIdentity,expectedData,
  setupPage,drainPage,save}){
  const e={schema:'eagler-touhou/package-import-parity/1',routes:[],cleanupErrors:[]};
  const sources=['zip','online',...(PRODUCT_GAMES[game].package.rawDataImport?.fileNames?.length?['original-file']:[])];
  for(const source of sources){
    const context=await browser.newContext({viewport:{width:1280,height:900}}),row={source,contextId:randomUUID(),status:'running'};
    e.routes.push(row);let page,watcher;
    const base=source==='online'?onlineBase:source==='original-file'?originalBase:localBase;
    const appUrl=new URL(`?game=${encodeURIComponent(game)}`,base).href;row.appUrl=appUrl;
    const open=async()=>{
      page=await context.newPage();watcher=await setupPage(page,{source,base});
      await page.goto(appUrl,{waitUntil:'load',timeout:60000});
      await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60000});
      await page.evaluate(()=>navigator.serviceWorker.ready);
      if(!await page.evaluate(()=>!!navigator.serviceWorker.controller)){
        await page.reload({waitUntil:'load',timeout:60000});
        await page.waitForFunction(()=>window.__eaglerBoot?.done===true,null,{timeout:60000});
      }
      await page.waitForFunction(()=>!!navigator.serviceWorker.controller,null,{timeout:30000});
      const firstUse=page.locator('#firstUseNoticeDialog');
      if(await firstUse.count()&&await firstUse.evaluate(node=>node.open))await page.locator('#firstUseNoticeClose').click();
      await chooseSetting(page,'gameVersionSelect','original');
    };
    const start=async()=>{
      const music=page.locator('#musicSelect');
      if(await page.locator('#musicOption').getAttribute('hidden')===null)await chooseSetting(page,'musicSelect','none');
      const launch=await launchLauncherRuntime(page,{game,music:await music.inputValue()});
      const installed=await page.evaluate(readPackageImportState,game);
      const pinnedId=new URL(launch.frame.url()).searchParams.get('gameGeneration');assert.ok(pinnedId);
      const pinned=await page.evaluate(readPackageImportState,{game,generationId:pinnedId});
      const runtime=await page.evaluate(snapshotPackageRuntime,{game,generation:pinnedId,expectedData});
      return{installed,pinned,runtime,launch:launch.evidence};
    };
    const retire=async()=>{
      await page.goBack();
      await page.waitForFunction(()=>document.getElementById('gameFrame')?.contentWindow?.location.href==='about:blank',null,{timeout:30000});
      await drainPage(page);await watcher.flush();await watcher.stop();await page.close();page=null;
    };
    try{
      await open();row.before=await page.evaluate(readPackageImportBefore,game);
      assert.equal(row.before.currentGeneration,null,'A source route must start uninstalled');
      row.ui={gameVersion:await page.locator('#gameVersionSelect').inputValue(),imported:false};
      if(source!=='online'){
        row.input=source==='zip'?archiveIdentity:originalIdentity;
        assert.ok(row.input?.path,'Supported raw import needs its actual declared DATA filename');
        await page.locator('#gamePackageImport').click();
        await page.locator('#gameDataImportWindow').waitFor({state:'visible',timeout:15000});
        await page.locator('#gameDataImportInput').setInputFiles(row.input.path);
        await page.waitForFunction(()=>{const node=document.getElementById('gameDataImportWindow');return node?.hidden&&node.getAttribute('aria-busy')!=='true';},null,{timeout:180000});
        row.ui.imported=true;row.ui.importClosed=true;
      }
      row.phase='first-start';await save(e);Object.assign(row,await start());await watcher.flush();row.responses=[...watcher.responses];await save(e);
      await retire();
      await open();row.phase='reopened-start';await save(e);row.reopened={...(await start()),ui:{gameVersion:await page.locator('#gameVersionSelect').inputValue()}};
      await retire();row.status='passed';await save(e);
    }catch(error){
      row.status='failed';row.failure=String(error.stack||error);row.launchFailure=error.evidence??null;
      if(page&&!page.isClosed())row.failureUi=await page.evaluate(()=>({url:location.href,launch:document.getElementById('launch')?.textContent,
        status:document.getElementById('status')?.textContent,importOpen:!document.getElementById('gameDataImportWindow')?.hidden,
        decisions:[...document.querySelectorAll('dialog[open]')].map(node=>({id:node.id,text:node.textContent})),events:window.__offlineRuntimeEvents}));
    }finally{
      if(page&&!page.isClosed())try{await drainPage(page);}catch(error){e.cleanupErrors.push({source,phase:'protocol-drain',error:String(error.stack||error)});}
      if(page&&!page.isClosed()&&watcher){
        try{await watcher.flush();}catch(error){e.cleanupErrors.push({source,phase:'response-flush',error:String(error.stack||error)});}
        try{await watcher.stop();}catch(error){e.cleanupErrors.push({source,phase:'response-stop',error:String(error.stack||error)});}
      }
      try{await context.close();}catch(error){e.cleanupErrors.push({source,phase:'context-close',error:String(error.stack||error)});}
      await save(e);
    }
  }
  return e;
}
