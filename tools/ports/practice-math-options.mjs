import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createNativeUiControls} from './native-ui-controls.mjs';
import {createPracticeUiInput} from './practice-ui-input.mjs';
import {decodeNativeControls} from '../../tests/adaptation/adapters/native-controls.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed,assertControlledSceneWorld} from '../../tests/adaptation/adapters/scene.mjs';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');

export function assertPracticeMathOptions(e,options,{reference,original,imported,exported}){
  const c=options.math,f=c.fields;assert.equal(e.schema,'eagler-touhou/practice-math/1');assertControlledSceneArmed(e.arm);
  assert.deepEqual(e.cleanupErrors,[]);assert.deepEqual(e.trials.map(t=>t.id),c.modes.map(m=>m.id));
  for(const [name,bytes] of Object.entries({original,imported,exported})){
    assert.equal(bytes.length,c.tableBytes);assert.equal(e.files[name].bytes,bytes.length);assert.equal(e.files[name].sha256,hash(bytes));
  }
  assert.deepEqual(exported,imported);assert.notDeepEqual(original,imported);
  let changed=0;for(let i=0;i<original.length;i+=4){const a=original.readUInt32LE(i),b=imported.readUInt32LE(i);
    if(a>0&&a<0x7f800000){assert.equal(b,a+1);changed++;}else assert.equal(a,b);}
  assert.ok(changed>500000);
  const identity=e.trials[0].samples[0].identity;
  for(const [index,trial] of e.trials.entries()){
    const mode=c.modes[index];assert.ok(trial.samples.length>=8&&trial.samples.length<=40);let prior;
    for(const row of trial.samples){
      decodeNativeControls(row.raw);assert.deepEqual(row.identity,identity);assertControlledSceneWorld(row.scene);assert.equal(row.scene.requestGeneration,e.arm.requestGeneration);
      const w=row.raw.words;assert.equal(w[f.option],1);assert.equal(w[f.selected],mode.index);assert.equal(w[f.enabled],1);assert.equal(w[f.cpu],mode.code);
      if(prior)assert.ok(w[f.sequence]>prior);prior=w[f.sequence];
      const seed=reference.seed(w[f.square],mode.index,trial.id==='custom-imported'?imported:original);assert.equal(w[f.seed],seed,'Real normalization used the wrong CPU/table seed');
      const input=w.slice(f.input,f.input+3).map(c.float),square=c.float(w[f.square]),seedFloat=c.float(seed),r=Math.fround;
      assert.equal(c.bits(r(r(r(input[0]*input[0])+r(input[1]*input[1]))+r(input[2]*input[2]))),w[f.square]);
      const scale=r(r(.5*seedFloat)*r(3-r(r(square*seedFloat)*seedFloat)));
      assert.equal(w[f.scale],c.bits(scale));assert.deepEqual(w.slice(f.output,f.output+3),input.map(value=>c.bits(r(scale*value))));
    }
  }
  assert.deepEqual(e.reference,reference.source);return{complete:false,modes:c.modes.map(m=>m.id),tableRoundTrip:true,actualNormalization:true};
}

export async function runPracticeMathOptions({page,frame,target,adapter,arm,output,repoRoot,save}){
  const options=adapter.practice.options,c=options.math,f=c.fields;
  const e={schema:'eagler-touhou/practice-math/1',arm,trials:[],files:{},cleanupErrors:[]};let manual=false,failure;
  const clock=action=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action})),advance=()=>clock('tick');
  const ui=createNativeUiControls({page,frame,runtime:adapter.runtime,contract:options,advance,save:async receipt=>{e.ui=receipt;await save(e);}});
  const input=createPracticeUiInput({page,frame,adapter,advance}),press=input.press;
  const read=async()=>({...await ui.read(),scene:await frame.evaluate(controlValidationScene,validationSceneRequest(target))});
  const download=async name=>{const [item]=await Promise.all([page.waitForEvent('download',{timeout:10000}),ui.click(c.export)]);
    assert.equal(await item.failure(),null);const path=resolve(output,`rsqrt-${name}.bin`);await item.saveAs(path);const bytes=await readFile(path);
    e.files[name]={path,bytes:bytes.length,sha256:hash(bytes)};return bytes;};
  try{
    assert.equal((await clock('begin')).mode,'native-manual');manual=true;for(let n=0;n<4;n++)await advance();
    await press(c.menuKey);await ui.until('Advanced math tools not open',s=>s.open);await ui.click(c.toggle);
    const original=await download('original'),imported=Buffer.from(original);
    for(let offset=0;offset<imported.length;offset+=4){const word=imported.readUInt32LE(offset);if(word>0&&word<0x7f800000)imported.writeUInt32LE(word+1,offset);}
    const path=resolve(output,'rsqrt-imported.bin');await writeFile(path,imported);e.files.imported={path,bytes:imported.length,sha256:hash(imported)};
    const reference=await c.loadReference(repoRoot);e.reference=reference.source;let exported;
    for(const mode of c.modes){
      if(mode.id==='custom-imported'){
        const [chooser]=await Promise.all([page.waitForEvent('filechooser',{timeout:10000}),ui.click(c.import)]);await chooser.setFiles(path);
        // The production file picker is asynchronous. Re-export until the imported bytes reach the native consumer.
        for(let n=0;n<20;n++){await advance();exported=await download('exported');if(hash(exported)===hash(imported))break;await page.waitForTimeout(20);}
        assert.deepEqual(exported,imported,'Native file import never committed the selected table');
      }
      if((await ui.read()).raw.words[f.selected]!==mode.index){
        await ui.click(c.cpu);e.popup=await ui.until('Native CPU popup has not submitted its choices',s=>s.controls.some(item=>item.semantic===c.choices[mode.index]));await save(e);await ui.click(c.choices[mode.index]);
        await ui.until('Actual CPU combo did not select the requested mode',row=>row.controls.some(item=>item.semantic===c.cpu&&item.value===mode.index));
      }
      await press(c.menuKey);await ui.until('Math tools did not close',s=>!s.open);
      const trial={id:mode.id,samples:[]};e.trials.push(trial);const before=(await read()).raw.words[f.sequence];
      for(let n=0;n<120&&trial.samples.length<8;n++){await advance();const row=await read(),w=row.raw.words;
        if(w[f.sequence]>before&&w[f.enabled]===1&&w[f.cpu]===mode.code&&(!trial.samples.length||w[f.sequence]>trial.samples.at(-1).raw.words[f.sequence]))trial.samples.push(row);}
      assert.equal(trial.samples.length,8,'Actual gameplay never consumed the selected normalization mode');await save(e);
      await press(c.menuKey);await ui.until('Math tools did not reopen',s=>s.open);
    }
    e.nativeInput=input.evidence;e.assessment=assertPracticeMathOptions(e,options,{reference,original,imported,exported});await save(e);return e;
  }catch(error){failure=error;e.error=String(error.stack||error);await save(e);throw error;}
  finally{try{if(manual)await clock('end');}catch(error){e.cleanupErrors.push(String(error));}await save(e);if(e.cleanupErrors.length&&!failure)throw Error('Math options cleanup failed');}
}
