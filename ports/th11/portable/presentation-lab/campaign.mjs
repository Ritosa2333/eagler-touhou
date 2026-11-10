// Headless replay campaign for interpolated-frame displacement.
// Usage:
//   node portable/presentation-lab/campaign.mjs --demo 0 --ticks 3600 --stride 30
//   node portable/presentation-lab/campaign.mjs --replay path/to/replay.rpy
import {launchBrowser} from '../../../../tools/browser/launch.mjs';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
const root=resolve(import.meta.dirname,'../..');
const argument=(name,fallback)=>{const i=process.argv.indexOf('--'+name);return i>=0?(process.argv[i+1]??fallback):fallback;};
const url=argument('url','http://127.0.0.1:8136/');
const demo=argument('demo',null),replayPath=argument('replay',null);
const ticks=Number(argument('ticks',3600)),stride=Number(argument('stride',30));
const thresholds={maxStep:Number(argument('max-step',48)),maxError:Number(argument('max-error',2.5)),minStep:Number(argument('min-step',8)),rebaseStep:Number(argument('rebase-step',256))};
const alphas=argument('alphas','0,0.25,0.5,0.75,1').split(',').map(Number);
if(!Number.isInteger(ticks)||ticks<1||!Number.isInteger(stride)||stride<1||stride>600)throw Error('Expected positive ticks and stride in 1..600');
if(alphas.length<2||alphas[0]!==0||alphas.at(-1)!==1||alphas.some((value,index)=>!Number.isFinite(value)||value<0||value>1||(index&&value<=alphas[index-1])))throw Error('Expected ordered alpha samples from 0 to 1');
const out=resolve(root,argument('out','artifacts/presentation-lab/campaign/'+(demo!==null?'demo'+demo:'replay')+'.json'));
const browser=await launchBrowser();
const page=await browser.newContext({viewport:{width:1280,height:900}}).then(context=>context.newPage());
const errors=[];page.on('pageerror',error=>errors.push(String(error)));
page.setDefaultTimeout(180000);
const result={target:url,demo:demo!==null?Number(demo):null,replay:replayPath,ticks,stride,thresholds,alphas,probes:0,checked:0,flagged:[],rebases:0,finished:null,errors};
try{
  await page.goto(url);
  await page.waitForFunction(()=>!!window.presentationLab);
  await page.evaluate(()=>window.presentationLab.boot().then(()=>true));
  await page.waitForFunction(()=>!!window.presentationLab.controller);
  result.build=await page.evaluate(()=>({...window.presentationLab.identity}));
  // Stop the rAF loop before loading: the Lab runtime skips the startup
  // signature, so launch() completes and the session is running.
  await page.evaluate(()=>window.presentationLab.controller.freeze());
  if(demo!==null)await page.evaluate(index=>window.presentationLab.controller.driver.loadDemo(Number(index)),demo);
  else if(replayPath)await page.evaluate(bytes=>window.presentationLab.controller.driver.loadReplay(Uint8Array.from(bytes)),Array.from(readFileSync(resolve(root,replayPath))));
  else throw Error('Pass --demo N or --replay PATH');
  const total=Math.ceil(ticks/stride);
  for(let batch=0;batch<total;batch++){
    try{await page.evaluate(count=>{window.presentationLab.controller.step(count);return true;},Math.min(stride,ticks-batch*stride));}
    catch(error){result.finished=String(error.message||error);break;}
    let probe;
    try{probe=await page.evaluate(async ({alphas,thresholds})=>{
      const {analyzeFlicker}=await import('/flicker.mjs');
      const controller=window.presentationLab.controller;
      const references=controller.references();
      const samples=alphas.map(alpha=>({alpha,records:controller.preview(alpha,{world:true}).records}));
      return analyzeFlicker({previous:references.previous,current:references.current,samples,thresholds});
    },{alphas,thresholds});}
    catch(error){
      result.probeError=String(error.message||error);
      try{result.coreError=await page.evaluate(()=>{const core=window.presentationLab.controller.core,pointer=core.th11_error(),memory=new Uint8Array(core.memory.buffer);let text='';for(let i=0;pointer+i<memory.length&&memory[pointer+i];++i)text+=String.fromCharCode(memory[pointer+i]);return text;});}catch{}
      break;
    }
    ++result.probes;result.checked+=probe.checked;result.rebases+=probe.rebases.length;
    for(const entry of probe.flagged)result.flagged.push({...entry,tick:entry.tick||(batch+1)*stride});
    if((batch+1)%20===0)console.log('probe',batch+1,'/',total,'flagged',result.flagged.length);
  }
}catch(error){result.error=String(error.stack||error);}
finally{
  result.complete=result.probes===Math.ceil(ticks/stride)&&!result.error&&!result.probeError&&!result.errors.length;
  mkdirSync(dirname(out),{recursive:true});
  writeFileSync(out,JSON.stringify(result,null,2)+'\n');
  console.log(JSON.stringify({out,probes:result.probes,checked:result.checked,flagged:result.flagged.length,rebases:result.rebases,finished:result.finished,error:result.error,errors:result.errors.length},null,2));
  if(!result.complete)process.exitCode=1;
  await browser.close();
}
