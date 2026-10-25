import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createNativeUiControls} from './native-ui-controls.mjs';
import {createPracticeUiInput} from './practice-ui-input.mjs';
import {decodeNativeControls} from '../../tests/adaptation/adapters/native-controls.mjs';
import {controlTouchMotionClock,touchClockRequest} from '../../tests/adaptation/adapters/touch-clock.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed,assertControlledSceneStimulus} from '../../tests/adaptation/adapters/scene.mjs';
import {requestDeathbombHit,readDeathbombState,deathbombRequest} from '../../tests/adaptation/adapters/deathbomb.mjs';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const textHash=text=>{let value=2166136261;for(const byte of Buffer.from(text))value=Math.imul(value^byte,16777619)>>>0;return value;};

export function assertPracticeDisplay(e,options,{license,images}={}){
  const c=options.display,f=c.fields;assert.equal(e.schema,'eagler-touhou/practice-display-options/1');assertControlledSceneArmed(e.arm);
  assert.deepEqual(e.cleanupErrors,[]);assert.equal(typeof e.enabled,'boolean');assert.equal(e.hit.accepted,true);
  assert.ok(e.samples.length>=4&&e.samples.length<=20);const identity=e.samples[0].identity;let previous;
  for(const row of e.samples){decodeNativeControls(row.raw);assert.deepEqual(row.identity,identity);
    assertControlledSceneStimulus(row.scene,previous?.scene,{kind:'single-projectile'});previous=row;
    const w=row.raw.words,d=row.projectile;assert.equal(w[f.generation],w[3]);assert.equal(w[f.option],Number(e.enabled));
    assert.equal(d.available,true);assert.equal(d.complete,true);assert.equal(d.request.status,1);assert.ok(d.projectile.liveCount>0);
    if(!e.enabled){assert.equal(w[f.bullets],0);assert.equal(w[f.vertices],0);continue;}
    assert.equal(w[f.bullets],1);assert.equal(w[f.lasers],0);assert.ok(w[f.age]>0);assert.ok(w[f.vertices]>=8);
    const r=Math.fround,x=w[f.x],y=w[f.y],hx=r(w[f.hitX]*.5),hy=r(w[f.hitY]*.5),p=r(w[f.playerWidth]*.5);
    assert.deepEqual(w.slice(f.left,f.left+4),[r(r(r(x-hx)-p)+c.offset.x),r(r(r(y-hy)-p)+c.offset.y),
      r(r(r(x+hx)+p)+c.offset.x),r(r(r(y+hy)+p)+c.offset.y)]);
  }
  assert.ok(Array.isArray(images)&&images.length===e.images.length&&images.length>0);
  for(const [i,bytes] of images.entries()){const image=e.images[i];assert.ok(bytes.length>1000);assert.equal(hash(bytes),image.sha256);assert.equal(bytes.length,image.bytes);
    assert.deepEqual(Array.from(bytes.subarray(0,8)),[137,80,78,71,13,10,26,10]);assert.deepEqual(image.identity,identity);}
  if(e.enabled){
    assert.ok(license?.text&&license.source);assert.deepEqual(e.license.source,license.source);
    const w=e.license.shown.raw.words;assert.equal(w[f.licenseBytes],Buffer.byteLength(license.text));assert.equal(w[f.licenseHash],textHash(license.text));assert.ok(w[f.licenseVertices]>0);
    assert.equal(w[f.licenseFlag],1);assert.equal(w[f.aboutLines],4);
    for(const row of [e.license.before,e.license.hidden]){assert.equal(row.raw.words[f.licenseFlag],0);assert.equal(row.raw.words[f.licenseBytes],0);}
    assert.deepEqual(e.license.shown.identity,identity);assert.deepEqual(e.license.hidden.identity,identity);
    assert.ok(e.images.length>=2);
  }
  return{complete:false,hitbox:e.enabled?'actual geometry and vertices':'live projectile without overlay',license:e.enabled};
}

export async function runPracticeDisplay({page,frame,target,adapter,arm,enabled,output,repoRoot,save}){
  const options=adapter.practice.options,c=options.display,f=c.fields;
  const e={schema:'eagler-touhou/practice-display-options/1',arm,enabled,samples:[],images:[],cleanupErrors:[]};let manual=false,failure;
  const clock=action=>frame.evaluate(controlTouchMotionClock,touchClockRequest({...target,action})),advance=()=>clock('tick');
  const ui=createNativeUiControls({page,frame,runtime:adapter.runtime,contract:options,advance,save:async receipt=>{e.ui=receipt;await save(e);}});
  const input=createPracticeUiInput({page,frame,adapter,advance}),press=input.press;
  const read=async()=>({...await ui.read(),scene:await frame.evaluate(controlValidationScene,validationSceneRequest(target)),
    projectile:await frame.evaluate(readDeathbombState,deathbombRequest(target))});
  const image=async name=>{const path=resolve(output,`native-${name}.png`);const bytes=await frame.locator('canvas').screenshot({path});
    const row=await ui.read();e.images.push({path,bytes:bytes.length,sha256:hash(bytes),identity:row.identity,generation:row.state.generation,frame:row.raw.words[options.consumerFields.frame]});};
  try{
    assert.equal((await clock('begin')).mode,'native-manual');manual=true;
    for(let n=0;n<600;n++){if((await ui.read()).raw.words[options.consumerFields.life]===0)break;await advance();}
    assert.equal((await ui.read()).raw.words[options.consumerFields.life],0);
    await press('Backspace');await press('F1');await press('Backspace');await press(c.menuKey);
    await ui.until('Display options not open',s=>s.open);
    if(Boolean((await ui.read()).raw.words[f.option])!==enabled)await ui.click(c.hitbox);
    let license;
    if(enabled){
      await ui.click(c.about);await ui.until('About section did not finish expanding',s=>s.controls.some(item=>item.semantic===c.about&&item.value===1));await ui.locate(c.license);e.license={before:await ui.read()};await ui.click(c.license);
      // License is longer than the child window. Scroll the real child to its text.
      for(let n=0;n<8;n++){const row=await ui.read();if(row.raw.words[f.licenseVertices]>0){e.license.shown=row;break;}
        const clip=row.state.scroll,box=await frame.locator('canvas').boundingBox(),scale=Math.min(box.width/options.width,box.height/options.height);
        await page.mouse.move(box.x+box.width/2,box.y+(clip.top+clip.bottom)/2*scale);await page.mouse.wheel(0,120);await advance();}
      assert.ok(e.license.shown,'Actual license text never emitted visible vertices');license=await c.loadLicense(repoRoot);e.license.source=license.source;await image('license');
      await ui.click(c.license);e.license.hidden=await ui.read();await image('about');
    }
    await press(c.menuKey);await ui.until('Display options did not close',s=>!s.open);for(let n=0;n<3;n++)await advance();
    e.hit=await frame.evaluate(requestDeathbombHit,deathbombRequest(target));assert.equal(e.hit.accepted,true);
    for(let n=0;n<6;n++){await advance();e.samples.push(await read());}
    await image(enabled?'hitbox-on':'hitbox-off');e.nativeInput=input.evidence;e.assessment=assertPracticeDisplay(e,options,{license,images:await Promise.all(e.images.map(row=>readFile(row.path)))});await save(e);return e;
  }catch(error){failure=error;e.error=String(error.stack||error);await save(e);throw error;}
  finally{try{if(manual)await clock('end');}catch(error){e.cleanupErrors.push(String(error));}await save(e);if(e.cleanupErrors.length&&!failure)throw Error('Practice display cleanup failed');}
}
