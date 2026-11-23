import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createManagedTitlePresenter} from '../runtime/managed-title-presenter.mjs';

const tick=1000/60;
function fixture(options={}){
  const queue=new Map(),draws=[],errors=[];let serial=0;
  const f={ticks:0,visible:true,drawable:true,queue,draws,errors,
    take(){assert.equal(queue.size,1,'exactly one title callback should own the canvas');
      const [id,fn]=queue.entries().next().value;queue.delete(id);return fn;},
    frame(time){f.take()(time);},
  };
  f.presenter=createManagedTitlePresenter({
    title:{step(){f.ticks++;},status:()=>({page:0,ready:+(f.ticks>=2),transitioning:+(f.ticks<2),mainAction:0})},
    visible:()=>f.visible,draw(){draws.push(f.ticks);return f.drawable;},onError:error=>errors.push(error),
    requestFrame(fn){queue.set(++serial,fn);return serial;},cancelFrame:id=>queue.delete(id),...options,
  });
  return f;
}

test('a successful ready draw acknowledges once; refused draws and acknowledgement do not stop native ticks',async()=>{
  const f=fixture(),life=new AbortController();let receipts=0;
  const first=f.presenter.present(life.signal).then(drawn=>{assert.equal(drawn,true);receipts++;});
  f.frame(0);f.frame(tick);await Promise.resolve();assert.equal(receipts,0,'opening frames are not a ready receipt');
  f.drawable=false;f.frame(2*tick);f.frame(3*tick);await Promise.resolve();
  assert.equal(receipts,0);assert.equal(f.ticks,3,'a failed presentation must not freeze the ready native scene');
  f.drawable=true;f.frame(3.5*tick);await first;
  for(let half=8;half<=20;half++)f.frame(half*tick/2);
  assert.equal(receipts,1);assert.equal(f.ticks,10,'120 Hz presentation must preserve the native 60 Hz cadence');
  assert.equal(f.draws.at(-1),10);assert.equal(f.queue.size,1);
  const stale=f.take(),before=f.draws.length;life.abort();stale(11*tick);
  assert.equal(f.ticks,10);assert.equal(f.draws.length,before);assert.equal(f.queue.size,0);assert.deepEqual(f.errors,[]);
});

test('hidden rooms neither tick nor draw, even across a short hide with no browser RAF',async()=>{
  const f=fixture();f.visible=false;
  const ready=f.presenter.present();f.frame(5);f.frame(500);assert.equal(f.ticks,0);assert.deepEqual(f.draws,[]);
  f.visible=true;f.frame(550);f.frame(550+2*tick);await ready;
  f.visible=false;const draws=f.draws.length;f.frame(600);f.frame(655);assert.equal(f.ticks,2);assert.equal(f.draws.length,draws);
  f.visible=true;f.frame(700);assert.equal(f.ticks,2);f.frame(700+tick);assert.equal(f.ticks,3);
  // Some browsers suspend RAF for the whole hidden interval. The view forwards visibilitychange.
  f.visible=false;f.presenter.resetClock();f.visible=true;f.presenter.resetClock();
  f.frame(810);assert.equal(f.ticks,3);f.frame(810+tick);assert.equal(f.ticks,4);
  f.presenter.close();assert.equal(f.queue.size,0);
});

test('new room ownership supersedes a pending local Replay restore; start and return reuse the same title',async()=>{
  const f=fixture(),replay=new AbortController(),room=new AbortController();
  const local=f.presenter.present(replay.signal),old=f.take();
  const next=f.presenter.present(room.signal);next.catch(()=>{});
  old(99);assert.deepEqual(f.draws,[]);await assert.rejects(local,{name:'AbortError'});replay.abort();
  f.frame(100);f.frame(100+2*tick);assert.equal(await next,true);
  const oldRoom=f.take(),before=f.draws.length;f.presenter.stop();oldRoom(500);
  assert.equal(f.draws.length,before);assert.equal(f.ticks,2);assert.equal(f.queue.size,0);
  const back=f.presenter.present(room.signal);f.frame(10000);assert.equal(await back,true);
  assert.equal(f.ticks,2,'a return requires a fresh draw, not a second boot or accumulated world time');
  f.frame(10000+tick);assert.equal(f.ticks,3);
  const oldBack=f.take(),restored=f.presenter.present();room.abort();oldBack(11000);
  f.frame(12000);assert.equal(await restored,true);assert.equal(f.ticks,3);assert.equal(f.queue.size,1);
  const closed=f.take(),drawCount=f.draws.length;f.presenter.close();f.presenter.close();closed(13000);
  assert.equal(f.draws.length,drawCount);assert.equal(f.queue.size,0);
  await assert.rejects(f.presenter.present(),{name:'AbortError'});
});

test('abort or close before readiness rejects the waiting caller; an already-aborted request cannot preempt another',async()=>{
  const f=fixture(),live=new AbortController(),dead=new AbortController();dead.abort();
  const pending=f.presenter.present(live.signal);await assert.rejects(f.presenter.present(dead.signal),{name:'AbortError'});
  assert.equal(f.queue.size,1);live.abort();await assert.rejects(pending,{name:'AbortError'});assert.equal(f.queue.size,0);
  const closing=f.presenter.present();f.presenter.close();await assert.rejects(closing,{name:'AbortError'});assert.equal(f.queue.size,0);
});

test('late title bank preparation cannot schedule a paint after a world takes over or after a new title request',async()=>{
  const loads=[];const f=fixture({prepare:signal=>new Promise((resolve,reject)=>loads.push({signal,resolve,reject}))});
  const first=f.presenter.present();assert.equal(loads.length,1);assert.equal(f.queue.size,0);
  f.presenter.stop();await assert.rejects(first,{name:'AbortError'});assert.equal(loads[0].signal.aborted,true);
  const second=f.presenter.present(),third=f.presenter.present();await assert.rejects(second,{name:'AbortError'});
  loads[0].resolve();loads[1].reject(Error('retired bank'));await Promise.resolve();
  assert.equal(f.queue.size,0);assert.deepEqual(f.draws,[]);assert.deepEqual(f.errors,[]);
  loads[2].resolve();await Promise.resolve();f.frame(0);f.frame(2*tick);assert.equal(await third,true);
  f.presenter.stop();assert.equal(loads[2].signal.aborted,true);
});

test('only stable Main acknowledges; a native menu action or foreign page is never drawn as a room preview',async()=>{
  const state={page:0,ready:1,transitioning:1,mainAction:0};
  const f=fixture({title:{status:()=>state,step(){}}});let shown=false;
  const ready=f.presenter.present().then(()=>{shown=true;});f.frame(0);await Promise.resolve();assert.equal(shown,false);
  state.transitioning=0;f.frame(tick);await ready;
  const before=f.draws.length;state.mainAction=1;f.frame(2*tick);
  assert.equal(f.draws.length,before);assert.equal(f.queue.size,0);assert.equal(f.errors.length,1,'report errors after an already acknowledged frame');
  state.mainAction=0;state.page=6;const foreign=f.presenter.present();f.frame(3*tick);
  await assert.rejects(foreign);assert.equal(f.draws.length,before);assert.equal(f.queue.size,0);assert.equal(f.errors.length,1);
});

test('a postrun Replay return may present a single title backdrop without leaving a room clock behind',async()=>{
  const f=fixture(),replay=new AbortController();
  const backdrop=f.presenter.present(replay.signal,{once:true});f.frame(0);f.frame(2*tick);
  assert.equal(await backdrop,true);assert.equal(f.queue.size,0);assert.equal(f.draws.at(-1),2);
  const room=f.presenter.present();replay.abort();f.frame(4000);assert.equal(await room,true);
  f.frame(4000+tick);assert.equal(f.draws.at(-1),3);assert.equal(f.queue.size,1);f.presenter.close();
});

test('retirement inside a native callback stops the rest of that clock batch before reading or painting again',async()=>{
  let reads=0,steps=0;
  const f=fixture({title:{step(){steps++;f.presenter.stop();},
    status(){reads++;return {page:0,ready:1,transitioning:0,mainAction:0};}}});
  const ready=f.presenter.present();f.frame(0);await ready;const before=reads;
  f.frame(3*tick);assert.equal(steps,1);assert.equal(reads,before);assert.equal(f.draws.length,1);assert.equal(f.queue.size,0);
  const g=fixture({title:{step(){},status(){g.presenter.stop();return {page:0,ready:1,transitioning:0,mainAction:0};}}});
  const retired=g.presenter.present();g.frame(0);await assert.rejects(retired,{name:'AbortError'});assert.deepEqual(g.draws,[]);
});
