import {test} from 'vitest';
import assert from 'node:assert/strict';
import {createDecisionStore} from '../../app/models/decisions.ts';
import {createGameUpdatePrompt} from '../../app/models/game-update-prompt.ts';

const flush = async () => {for(let i=0;i<20;i++)await Promise.resolve();};
function fixture() {
  const decisions=createDecisionStore(), restarts=[], errors=[];
  let context={product:'th06',gameVersion:'original',epoch:1}, busy=false;
  const prompt=createGameUpdatePrompt({decisions,translate:key=>key,current:()=>context,busy:()=>busy,
    restart:async value=>{restarts.push(value);},onError:error=>errors.push(error)});
  return {decisions,prompt,restarts,errors,get context(){return context;},set context(value){context=value;},
    set busy(value){busy=value;}};
}
test('completed code and DATA downloads share one two-choice prompt, with next launch preserving the current game',async()=>{
  const f=fixture();f.prompt.ready(f.context);f.prompt.ready(f.context);await flush();
  const request=f.decisions.getSnapshot();
  assert.equal(request.confirmText,'update.restartNow');assert.equal(request.cancelText,'update.nextLaunch');
  assert.equal(request.secondaryText,undefined);
  f.decisions.resolve('cancel');await flush();f.prompt.ready(f.context);await flush();
  assert.deepEqual(f.restarts,[]);assert.equal(f.decisions.getSnapshot(),null);f.prompt.dispose();
});
test('immediate restart belongs only to the exact product, version and epoch that displayed the update',async()=>{
  for(const changed of [null,{product:'th07'},{gameVersion:'beauty'},{epoch:2}]){
    const f=fixture();f.prompt.ready(f.context);await flush();
    if(changed)f.context={...f.context,...changed};
    f.decisions.resolve('confirm');await flush();
    assert.equal(f.restarts.length,changed?0:1);f.prompt.dispose();
  }
});
test('updates wait for existing decisions and game operations, then remain available',async()=>{
  const f=fixture(),other=f.decisions.askDecision({message:'existing'});
  f.prompt.ready(f.context);await flush();assert.equal(f.decisions.getSnapshot().message,'existing');
  f.busy=true;f.decisions.resolve('cancel');await other;await flush();assert.equal(f.decisions.getSnapshot(),null);
  f.busy=false;f.prompt.flush();await flush();assert.equal(f.decisions.getSnapshot().title,'update.readyTitle');
  f.decisions.resolve('confirm');await flush();assert.equal(f.restarts.length,1);f.prompt.dispose();
});
test('late downloads and disposed prompts never restart a replacement game',async()=>{
  const f=fixture(),old=f.context;f.context={...old,epoch:2};f.prompt.ready(old);await flush();
  assert.equal(f.decisions.getSnapshot(),null);
  f.prompt.ready(f.context);await flush();f.prompt.dispose();f.decisions.resolve('confirm');await flush();
  assert.deepEqual(f.restarts,[]);
});
test('an update popup closes when its game exits without dismissing another decision',async()=>{
  const f=fixture();f.prompt.ready(f.context);await flush();assert.ok(f.decisions.getSnapshot());
  f.context=null;f.prompt.flush();await flush();assert.equal(f.decisions.getSnapshot(),null);
  assert.deepEqual(f.restarts,[]);f.prompt.dispose();
});
