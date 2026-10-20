import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve,basename} from 'node:path';
import {selectRuntimeTargets,readVerifiedRuntimeFile} from './acceptance-contracts.mjs';
import {createRuntimeFixtureSite} from './runtime-fixture-site.mjs';
import {readRuntimeObservation} from './runtime-observations.mjs';
import {runWithReport} from '../testing/cli-report.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {assertCliArguments} from '../testing/cli-options.mjs';
import {practiceBuildFeature} from './practice-observations.mjs';
import {createStageRecorder} from './runtime-desktop-stage.mjs';
import {auditBrowserFinalErrors} from '../browser/final-error-audit.mjs';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';

const option=name=>process.argv.find(value=>value.startsWith(`--${name}=`))?.slice(name.length+3);
if(!option('output'))throw Error('Required: --output=directory');
const output=resolve(option('output')),repoRoot=resolve(import.meta.dirname,'../..');
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');

function practiceProtocolInitScript({game,epoch}){
  if(window!==window.top)return;
  const pending=new Set();
  Object.defineProperty(window,'__flushPracticeProtocolCaptures',{configurable:true,value:async()=>{
    while(pending.size)await Promise.all([...pending]);
    if(window.__practiceProtocolCaptureError)throw new Error(window.__practiceProtocolCaptureError);
  }});
  addEventListener('message',event=>{
    const frame=document.querySelector('iframe'),data=event.data;
    if(event.origin!==location.origin||event.source!==frame?.contentWindow||data?.protocol!=='eagler-touhou/1'||
      data.game!==game||data.epoch!==epoch)return;
    if(data.event!=='error'&&data.event!=='abort'&&data.ok!==false)return;
    const operation=Promise.resolve(window.__capturePracticeProtocolFailure({...data,origin:event.origin}))
      .catch(error=>{window.__practiceProtocolCaptureError=String(error?.stack||error);});
    pending.add(operation);void operation.finally(()=>pending.delete(operation));
  });
}

async function bindPracticeProtocolCapture(page,game,epoch,record){
  await page.exposeBinding('__capturePracticeProtocolFailure',(source,event)=>{
    const samePage=source.page===page,mainFrame=source.frame===page.mainFrame();
    if(!samePage||!mainFrame){record.protocolErrors.push({type:'untrusted-binding-source',samePage,mainFrame,event});return;}
    record.protocolErrors.push(event);
  });
  await page.addInitScript(practiceProtocolInitScript,{game,epoch});
}

async function drainPracticeProtocolCapture(page){
  if(!page||page.isClosed())return;
  await page.evaluate(async()=>{
    if(typeof window.__flushPracticeProtocolCaptures==='function')await window.__flushPracticeProtocolCaptures();
  });
}
const result=await runWithReport({output,schema:'eagler-touhou/practice-browser/1'},async(report,save)=>{
  assertCliArguments(process.argv.slice(2),{values:['output','manifest','game','variant']});
  assert.equal(option('variant')??'normal','normal','Practice menu driver requires an ordinary Runtime');
  assert.ok(option('manifest')&&option('game'),'Required: --manifest=private-inputs.json --game=explicit-selection');
  const config=JSON.parse(await readFile(resolve(option('manifest')),'utf8'));
  const selection=selectRuntimeTargets(config,{game:option('game'),variant:'normal'});
  Object.assign(report,{selection:{games:selection.games,variant:'normal',expectedTargets:selection.expected},
    scope:'Current ordinary Runtime | native practice overlay open/close and disabled control | screenshots for locale review',
    coverage:{ids:['PRACTICE-01','PRACTICE-02','PRACTICE-03','PRACTICE-07'],complete:false,
      scope:'Built feature declaration, configured locale and enabled state, actual native menu opening/closing in four independent ordinary instances. Scene transitions, parameter application and replay metadata have separate scenarios.'},targets:[]});
  for(const name of ['practice-browser.mjs','practice-observations.mjs','runtime-desktop-readiness.mjs','runtime-fixture-site.mjs','runtime-observations.mjs','runtime-desktop-stage.mjs','../browser/fixture-server.mjs','../browser/response-body-capture.mjs','../browser/final-error-audit.mjs','../../common/contracts/product-catalog.mjs','../../tests/adaptation/adapters/title-flow.mjs']){
    const bytes=await readFile(new URL(name,import.meta.url));
    (report.harnessSources??={})[name]=sha256(bytes);await writeFile(resolve(output,basename(name)),bytes);
  }
  const site=await createRuntimeFixtureSite({repoRoot,targets:selection.targets,shared:config.shared,protocol:'https'});
  report.transport={base:site.base,protocol:site.protocol,tlsIdentity:site.tlsIdentity};
  report.sharedInputs=await site.verifyShared();
  let browser;
  try{
    const {launchBrowser}=await import('../browser/launch.mjs');
    browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required',
      `--ignore-certificate-errors-spki-list=${site.tlsIdentity.spkiSha256}`]});
    report.browser=browser.version();
    for(const target of selection.targets){
      const titleFlow=resolveTitleFlow({game:target.game,variant:target.variant});
      const record={id:target.id,game:target.game,variant:target.variant,checks:[],errors:[],consoleErrors:[],httpErrors:[],requestFailures:[],protocolErrors:[],responseErrors:[],teardownErrors:[],locales:[]};
      report.targets.push(record);await save();
      const stage=createStageRecorder(record,save);
      let context,responseProbe,page;
      try{
        record.identity=await stage('identity-preflight',{},()=>site.verify(target));
        const manifest=JSON.parse(await readVerifiedRuntimeFile(target,record.identity.runtime,'manifest.json'));
        record.feature=practiceBuildFeature(manifest,PRODUCT_GAMES[target.game]);
        record.checks.push({id:'verified-built-practice-feature',status:'passed',enabled:record.feature});
        if(!record.feature){
          if(PRODUCT_GAMES[target.game].features.thprac){
            record.inputIssue='practice-enabled-package-required';
            throw Error('Practice behavior lane requires a practice-enabled package; the selected valid package has practice disabled');
          }
          record.status='not-applicable';record.reason='This product does not support practice';continue;
        }
        for(const [epoch,enabled,locale] of [[1,true,'ja-JP'],[2,true,'en-US'],[3,true,'zh-CN'],[4,false,'en-US']]){
          const caseRecord={epoch,enabled,locale,status:'running'};
          record.locales.push(caseRecord);await save();
          try{
          context=await browser.newContext({viewport:{width:1100,height:820}});
          await context.addInitScript(()=>{
            if(window===window.top)return;
            window.__practiceBrowserEvents=[];
            addEventListener('eagler-thprac-menu',event=>window.__practiceBrowserEvents.push({open:event.detail?.open,at:performance.now()}));
          });
          page=await context.newPage();
          await bindPracticeProtocolCapture(page,target.game,epoch,record);
          responseProbe=await site.watchPage(page,{output,label:`${target.id}-${epoch}`});
          caseRecord.responseIntegrity={responses:responseProbe.responses,errors:responseProbe.errors,capture:responseProbe.capture};
          page.on('pageerror',error=>record.errors.push(String(error)));
          page.on('console',message=>{if(message.type()==='error')record.consoleErrors.push(message.text());});
          page.on('response',response=>{if(response.status()>=400)record.httpErrors.push({url:response.url(),status:response.status()});});
          page.on('requestfailed',request=>record.requestFailures.push({url:request.url(),error:request.failure()?.errorText??null}));
          await stage('host-navigation',{epoch,locale,enabled},()=>page.goto(site.hostUrl(target,epoch)));
          await responseProbe.flush();
          await stage('runtime-ready',{epoch},()=>page.waitForFunction(epoch=>events.some(event=>event.event==='ready'&&event.epoch===epoch),epoch,{timeout:120000}));
          const element=await page.locator('iframe').elementHandle(),frame=await element.contentFrame();assert.ok(frame);
          const sharedResources=Object.keys(config.shared??{}).map(name=>({path:`/${name}`,url:`/shared/${name}`}));
          await stage('configure',{epoch,locale,enabled},()=>page.evaluate(({sharedResources,enabled,locale})=>rpc('configure',{language:'ja',music:'none',resources:[],runtimeResources:[],sharedResources,
            thpracCatalog:[],options:{touchEnabled:false,alwaysHitbox:false,thpracEnabled:enabled,thpracLocale:locale,limitPresentationTo60:false}}),{sharedResources,enabled,locale}));
          await stage('launch',{epoch},()=>page.evaluate(()=>rpc('launch')));
          await stage('first-frame',{epoch},()=>page.waitForFunction(epoch=>events.some(event=>event.event==='first-frame'&&event.epoch===epoch),epoch,{timeout:120000}));
          await responseProbe.flush();
          await frame.locator('canvas').first().focus();
          const observe=()=>frame.evaluate(readRuntimeObservation,target);
          const ready=state=>titleFlow.isGameplayReady(state,'practice');
          caseRecord.gameplay=await stage('enter-gameplay',{epoch},async()=>{
            const waitUntil=async(label,predicate,timeoutMs=30000)=>{
              const started=Date.now();let state;
              do{state=await observe();if(predicate(state))return state;await page.waitForTimeout(75);}
              while(Date.now()-started<timeoutMs);
              throw new Error(`${label}: native condition not reached: ${JSON.stringify(state)}`);
            };
            const state=await titleFlow.enterGameplay({observe,waitUntil,lane:'practice',epoch,label:'Practice overlay requires actual gameplay',
              press:async code=>{const key=code==='KeyZ'?'z':code;await page.keyboard.press(key,{delay:90});await page.waitForTimeout(140);}});
            assert.ok(ready(state),'Practice overlay requires actual gameplay before opening');
            return state;
          });
          const menu=()=>frame.evaluate(game=>{
            const runtime=window[`__${game}Runtime`],module=runtime?.Module??window.Module;
            return{open:module?.eaglerThpracMenuOpen===true,options:module?.eaglerOptions?{
                thpracEnabled:module.eaglerOptions.thpracEnabled,thpracLocale:module.eaglerOptions.thpracLocale}:null,
              events:window.__practiceBrowserEvents??[]};
          },target.game);
          const before=await menu();caseRecord.before=before;await save();
          assert.equal(before.open,false,'Fresh gameplay inherited an open practice overlay');
          await page.keyboard.press('Backspace',{delay:100});
          if(enabled)await frame.waitForFunction(game=>(window[`__${game}Runtime`]?.Module??window.Module)?.eaglerThpracMenuOpen===true,target.game,{timeout:5000});
          else await page.waitForTimeout(500);
          const opened=await menu();caseRecord.opened=opened;await save();
          const screenshot=`${target.id}-${enabled?locale:'disabled'}-practice.png`;
          await stage('menu-screenshot',{epoch},()=>page.screenshot({path:resolve(output,screenshot),timeout:15000}));
          caseRecord.screenshot=screenshot;await save();
          assert.equal(opened.open,enabled,'Practice toggle does not match built/configured feature');
          assert.equal(opened.options?.thpracEnabled,enabled,'Native UI options did not receive the enabled state');
          assert.equal(opened.options?.thpracLocale,locale,'Native UI options did not receive the configured locale');
          if(enabled){
            assert.ok(opened.events.some(event=>event.open===true),'Native practice owner did not publish menu opening');
            await page.keyboard.press('Backspace',{delay:100});
            await frame.waitForFunction(game=>(window[`__${game}Runtime`]?.Module??window.Module)?.eaglerThpracMenuOpen===false,target.game,{timeout:5000});
          }
          const closed=await menu();caseRecord.closed=closed;await save();
          assert.equal(closed.open,false,'Practice menu failed to close');
          if(enabled)assert.ok(closed.events.slice(opened.events.length).some(event=>event.open===false),
            'Native practice owner did not publish menu closing');
          Object.assign(caseRecord,{status:'passed',closed,
            localeReview:'Screenshot records displayed glyphs and labels; native locale correctness requires image review'});
          await responseProbe.flush();
          }catch(error){Object.assign(caseRecord,{status:'failed',error:String(error.stack||error)});}
          finally{
            if(responseProbe){
              try{await responseProbe.flush();}catch(error){record.responseErrors.push({epoch,phase:'flush-before-stop',error:String(error.stack||error)});}
              try{await responseProbe.stop();}catch(error){record.teardownErrors.push({epoch,phase:'response-probe.stop',error:String(error.stack||error)});}
              try{await responseProbe.flush();}catch(error){record.responseErrors.push({epoch,phase:'flush-after-stop',error:String(error.stack||error)});}
              for(const error of responseProbe.errors??[])record.responseErrors.push({epoch,error});
            }
            try{await drainPracticeProtocolCapture(page);}catch(error){record.teardownErrors.push({epoch,phase:'protocol-capture.drain',error:String(error.stack||error)});}
            if(page&&!page.isClosed())try{await page.close();}catch(error){record.teardownErrors.push({epoch,phase:'page.close',error:String(error.stack||error)});}
            if(context)try{await context.close();}catch(error){record.teardownErrors.push({epoch,phase:'context.close',error:String(error.stack||error)});}
            responseProbe=null;context=null;page=null;await save();
          }
        }
        assert.deepEqual(record.errors,[],'Browser errors');assert.deepEqual(record.consoleErrors,[],'Runtime errors');assert.deepEqual(record.httpErrors,[],'HTTP errors');
        assert.equal(record.locales.length,4,'Every enabled locale and the disabled control must run');
        assert.ok(record.locales.every(item=>item.status==='passed'),'A practice locale or disabled control failed; see its saved native state');
        record.status='passed';
      }catch(error){record.status='failed';record.error=String(error.stack||error);
        const active=record.locales.find(item=>item.status==='running');if(active)Object.assign(active,{status:'failed',error:record.error});}
      finally{
        if(responseProbe){
          try{await responseProbe.flush();}catch(error){record.responseErrors.push({phase:'target-flush-before-stop',error:String(error.stack||error)});}
          try{await responseProbe.stop();}catch(error){record.teardownErrors.push({phase:'target-response-probe.stop',error:String(error.stack||error)});}
          try{await responseProbe.flush();}catch(error){record.responseErrors.push({phase:'target-flush-after-stop',error:String(error.stack||error)});}
          for(const error of responseProbe.errors??[])record.responseErrors.push({error});
        }
        try{await drainPracticeProtocolCapture(page);}catch(error){record.teardownErrors.push({phase:'target-protocol-capture.drain',error:String(error.stack||error)});}
        if(page&&!page.isClosed())try{await page.close();}catch(error){record.teardownErrors.push({phase:'target-page.close',error:String(error.stack||error)});}
        if(context)try{await context.close();}catch(error){record.teardownErrors.push({phase:'target-context.close',error:String(error.stack||error)});}
        responseProbe=null;context=null;page=null;
        try{record.postflight=await site.verify(target);}catch(error){record.status='failed';record.postflightError=String(error.stack||error);}
        record.finalErrorAudit=auditBrowserFinalErrors({pageErrors:record.errors,consoleErrors:record.consoleErrors,
          httpErrors:[...record.httpErrors,...record.requestFailures],protocolErrors:record.protocolErrors,
          responseErrors:record.responseErrors,teardownErrors:record.teardownErrors});
        if(record.finalErrorAudit.status==='failed'){
          record.preFinalAuditStatus=record.status;record.status='failed';
          record.error??=`Final browser error audit failed: ${record.finalErrorAudit.failedChannels.join(', ')}`;
        }
        await save();console.log(`${target.id}: ${record.status.toUpperCase()} ${record.error?.split('\n')[0]??''}`);
      }
    }
  }finally{
    report.teardownErrors=[];
    if(browser)try{await browser.close();}catch(error){report.teardownErrors.push({phase:'browser.close',error:String(error.stack||error)});}
    if(site)try{await site.close();}catch(error){report.teardownErrors.push({phase:'fixture.close',error:String(error.stack||error)});}
    report.httpFailures=site.httpFailures;
  }
  report.sharedInputsPostflight=await site.verifyShared();
  for(const [name,expected] of Object.entries(report.harnessSources))
    assert.equal(sha256(await readFile(new URL(name,import.meta.url))),expected,`Practice harness changed: ${name}`);
  report.finalErrorAudit=auditBrowserFinalErrors({
    pageErrors:report.targets.flatMap(target=>target.errors),
    consoleErrors:report.targets.flatMap(target=>target.consoleErrors),
    httpErrors:[...report.targets.flatMap(target=>[...target.httpErrors,...target.requestFailures]),...site.httpFailures],
    protocolErrors:report.targets.flatMap(target=>target.protocolErrors),
    responseErrors:report.targets.flatMap(target=>target.responseErrors),
    teardownErrors:[...report.teardownErrors,...report.targets.flatMap(target=>target.teardownErrors)],
  });
  report.passed=report.targets.length===selection.expected.length&&report.targets.every(target=>['passed','not-applicable'].includes(target.status))&&site.httpFailures.length===0&&report.finalErrorAudit.status==='passed';
  if(!report.passed&&report.finalErrorAudit.status==='failed')report.status='failed';
});
if(!result.passed){process.exitCode=1;if(result.failure)console.error(result.failure);}
