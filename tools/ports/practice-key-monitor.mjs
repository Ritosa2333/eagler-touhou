import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createNativeUiControls} from './native-ui-controls.mjs';
import {createPracticeUiInput} from './practice-ui-input.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed,assertControlledSceneWorld} from '../../tests/adaptation/adapters/scene.mjs';
import {decodeNativeControls} from '../../tests/adaptation/adapters/native-controls.mjs';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const popcount=value=>{let n=0;for(let b=value>>>0;b;b>>>=1)n+=b&1;return n;};

export function assertPracticeKeyMonitor(e,options,{csvBytes}={}){
  const c=options.keyMonitor,f=c.fields,consumer=options.consumerFields;
  assert.equal(e.schema,'eagler-touhou/practice-key-monitor/1');assertControlledSceneArmed(e.arm);
  assert.ok(e.rows.length>c.window+10&&e.rows.length<=1024);assert.deepEqual(e.cleanupErrors,[]);
  assert.equal(e.before.raw.words[f.recording],0);assert.equal(e.before.raw.words[f.count],0);assert.equal(e.before.raw.words[consumer.current],0);
  assert.equal(e.stopped.raw.words[f.recording],0);assert.equal(e.stopped.raw.words[f.count],e.rows.length);assert.equal(e.stopped.raw.words[f.apsCount],e.rows.length);
  const identity=e.before.identity,masks=[],aps=[];let prior;
  for(const [i,row] of e.rows.entries()){
    decodeNativeControls(row.raw);
    assert.deepEqual(row.identity,identity);assertControlledSceneWorld(row.scene);assert.equal(row.scene.requestGeneration,e.arm.requestGeneration);
    const w=row.raw.words;assert.equal(w[f.count],i+1);assert.equal(w[f.apsCount],i+1);
    if(prior){assert.equal(w[consumer.frame],prior[consumer.frame]+1);assert.equal(w[consumer.sequence],prior[consumer.sequence]+1);}prior=w;
    const mask=c.inputMasks.reduce((out,mask,bit)=>out|((w[consumer.current]&mask)?1<<bit:0),0);
    assert.equal(w[f.keys],mask,'Keyboard monitor disagrees with the native input consumer');masks.push(mask);
    let expected=0;for(let j=Math.max(0,i-c.window+1);j<=i;j++)expected+=popcount(masks[j]^(masks[j-1]??0));
    assert.equal(w[f.aps],expected,'APS does not count transitions in the last native 60 ticks');aps.push(expected);
  }
  assert.equal(e.pulses.length,c.pulses.length);
  for(const [i,pulse] of e.pulses.entries()){
    const spec=c.pulses[i];assert.deepEqual(pulse.keys,spec.keys);assert.ok(pulse.samples.length>=2);
    for(const sequence of pulse.samples){const row=e.rows.find(r=>r.raw.words[consumer.sequence]===sequence);assert.ok(row);assert.equal(row.raw.words[consumer.current],spec.expected);}
    const released=e.rows.find(r=>r.raw.words[consumer.sequence]===pulse.released);assert.ok(released);assert.equal(released.raw.words[consumer.current],0);
  }
  assert.ok(csvBytes instanceof Uint8Array);assert.equal(csvBytes.length,e.csv.bytes);assert.equal(hash(csvBytes),e.csv.sha256);
  const csv=c.header+masks.map((mask,i)=>[i+1,aps[i],...c.inputMasks.map((_,bit)=>mask&(1<<bit)?'O':'-')].join(',')+'\n').join('');
  assert.equal(Buffer.from(csvBytes).toString('utf8'),csv,'Actual CSV download differs from native recorded rows');
  return{complete:false,rows:e.rows.length,keyboard:true,aps:true,csv:true};
}

export async function runPracticeKeyMonitorBrowser({page,frame,target,adapter,arm,output,save}){
  const options=adapter.practice.options,c=options.keyMonitor,f=c.fields,consumer=options.consumerFields;
  const e={schema:'eagler-touhou/practice-key-monitor/1',arm,rows:[],pulses:[],cleanupErrors:[]};let manual=false,failure;
  const held=new Set(),clock=action=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action}));
  let ui;
  const advance=async()=>{
    await clock('tick');const row=await ui.read(),count=row.raw.words[f.count];
    if(count>e.rows.length){assert.equal(count,e.rows.length+1,'Native key record skipped a captured tick');
      e.rows.push({...row,scene:await frame.evaluate(controlValidationScene,validationSceneRequest(target))});}
  };
  ui=createNativeUiControls({page,frame,runtime:adapter.runtime,contract:options,advance,save:async receipt=>{e.ui=receipt;await save(e);}});
  const input=createPracticeUiInput({page,frame,adapter,advance}),press=input.press;
  const key=async(code,down)=>{await frame.locator('canvas').focus();await page.keyboard[down?'down':'up'](code);if(down)held.add(code);else held.delete(code);};
  try{
    assert.equal((await clock('begin')).mode,'native-manual');manual=true;for(let n=0;n<4;n++)await advance();e.before=await ui.read();
    await press(c.menuKey);await ui.until('Advanced tools not open',s=>s.open);
    await ui.click(c.toggle);await ui.click(c.record);await press(c.menuKey);await ui.until('Advanced tools not closed',s=>!s.open);
    for(const spec of c.pulses){
      for(const code of spec.keys)await key(code,true);
      for(let n=0;n<spec.ticks;n++)await advance();
      const pulse={keys:spec.keys,samples:e.rows.slice(-2).map(row=>row.raw.words[consumer.sequence])};e.pulses.push(pulse);
      for(const code of spec.keys)await key(code,false);for(let n=0;n<spec.release;n++)await advance();pulse.released=e.rows.at(-1).raw.words[consumer.sequence];
    }
    await press(c.menuKey);await ui.until('Advanced tools not open for export',s=>s.open);await ui.click(c.record);e.stopped=await ui.read();
    const [download]=await Promise.all([page.waitForEvent('download',{timeout:10000}),ui.click(c.download)]);
    assert.equal(await download.failure(),null);assert.equal(download.suggestedFilename(),'APS.csv');
    const path=resolve(output,'native-key-monitor.csv');await download.saveAs(path);const bytes=await readFile(path);
    e.csv={path,bytes:bytes.length,sha256:hash(bytes)};e.nativeInput=input.evidence;
    e.assessment=assertPracticeKeyMonitor(e,options,{csvBytes:bytes});await save(e);return e;
  }catch(error){failure=error;e.error=String(error.stack||error);await save(e);throw error;}
  finally{try{for(const code of held)await key(code,false);if(manual){await clock('end');manual=false;}}catch(error){e.cleanupErrors.push(String(error));}
    await save(e);if(e.cleanupErrors.length&&!failure)throw Error('Key monitor input cleanup failed');}
}
