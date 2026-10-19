// Actual native dialog, React decision component and update prompt owner.
// Native sync/retirement/next-generation behavior has separate mounted tests.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {mkdir,readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {build} from 'esbuild';
import {authoredSourcesPlugin} from './react-main/authored-sources.mjs';
import {loadPlaywright} from '../../tools/browser/playwright.mjs';
const project=resolve(import.meta.dirname,'..'),out=resolve(project,'artifacts/validation/game-update-dialog');
await mkdir(out,{recursive:true});
const app=await build({stdin:{resolveDir:project,loader:'tsx',contents:`
  import React,{useSyncExternalStore} from 'react';import {createRoot} from 'react-dom/client';
  import {ConfirmationDialog} from './app/components/ConfirmationDialog.tsx';
  import {LocaleProvider,translate} from './app/i18n.tsx';
  import {createDecisionStore} from './app/models/decisions.ts';
  import {createGameUpdatePrompt} from './app/models/game-update-prompt.ts';
  const locale=new URL(location.href).searchParams.get('locale')||'zh-CN',decisions=createDecisionStore();
  const context={product:'th06',gameVersion:'original',epoch:1};let restarts=0;
  const owner=createGameUpdatePrompt({decisions,translate:(key,params)=>translate(locale,key,params),
    current:()=>context,busy:()=>false,restart:async()=>{restarts++;},onError:error=>{throw error;}});
  function App(){const request=useSyncExternalStore(decisions.subscribe,decisions.getSnapshot);
    return <LocaleProvider locale={locale}><ConfirmationDialog open={!!request} {...request}
      onConfirm={()=>decisions.resolve('confirm')} onCancel={()=>decisions.resolve('cancel')}/></LocaleProvider>;}
  createRoot(document.getElementById('app')).render(<App/>);
  window.fixture={ready:()=>owner.ready(context),restarts:()=>restarts,pending:decisions.getSnapshot};
`},bundle:true,write:false,format:'esm',platform:'browser',jsx:'automatic',loader:{'.css':'empty'},plugins:[authoredSourcesPlugin(project)],define:{'process.env.NODE_ENV':'"production"'}});
const styles=await readFile(resolve(project,'public/styles.css'));
const server=createServer((req,res)=>{
  const path=new URL(req.url,'http://localhost').pathname;
  if(path==='/app.mjs'){res.setHeader('Content-Type','text/javascript');res.end(app.outputFiles[0].contents);}
  else if(path==='/styles.css'){res.setHeader('Content-Type','text/css');res.end(styles);}
  else if(path==='/'){res.end('<!doctype html><meta name="viewport" content="width=device-width,initial-scale=1"><link rel="stylesheet" href="/styles.css"><div id="app"></div><script type="module" src="/app.mjs"></script>');}
  else res.writeHead(404).end();
});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
const base='http://127.0.0.1:'+server.address().port,playwright=await loadPlaywright(),report=[];
try {
  for(const engine of ['chromium','firefox','webkit']){
    const browser=await playwright[engine].launch({headless:true});
    try {
      const context=await browser.newContext({viewport:{width:390,height:844},reducedMotion:'reduce'}),page=await context.newPage(),errors=[];
      page.on('pageerror',error=>errors.push(error.message));
      for(const [locale,choice] of [['zh-CN','cancel'],['en','escape'],['en','restart']]){
        await page.goto(base+'/?locale='+locale);await page.waitForFunction(()=>window.fixture);
        assert.equal(await page.locator('#decisionDialog').isVisible(),false);
        await page.evaluate(()=>fixture.ready());await page.locator('#decisionDialog').waitFor({state:'visible'});
        assert.equal(await page.locator('#decisionConfirm').textContent(),locale==='zh-CN'?'立即重启':'Restart now');
        assert.equal(await page.locator('#decisionCancel').textContent(),locale==='zh-CN'?'下次启动使用新版':'Use on next launch');
        assert.equal(await page.locator('#decisionSecondary').isVisible(),false);
        assert.equal(await page.evaluate(()=>document.activeElement?.id),'decisionCancel');
        if(locale==='zh-CN')await page.screenshot({path:resolve(out,engine+'-mobile.png')});
        if(choice==='cancel')await page.locator('#decisionCancel').click();
        else if(choice==='escape')await page.keyboard.press('Escape');
        else {await page.locator('#decisionConfirm').focus();await page.keyboard.press('Enter');}
        await page.locator('#decisionDialog').waitFor({state:'hidden'});
        await page.waitForFunction(()=>fixture.pending()===null);
        assert.equal(await page.evaluate(()=>fixture.restarts()),choice==='restart'?1:0,engine+' '+choice);
        report.push({engine,locale,choice,passed:true});
      }
      assert.deepEqual(errors,[]);
    }finally{await browser.close();}
  }
  await writeFile(resolve(out,'report.json'),JSON.stringify({passed:true,report},null,2));console.log(JSON.stringify({passed:true,report}));
}finally{server.closeAllConnections();await new Promise(r=>server.close(r));}
