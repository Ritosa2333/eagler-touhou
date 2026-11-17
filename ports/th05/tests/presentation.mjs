import {test} from 'node:test';
import assert from 'node:assert/strict';
import {FrameClock,PresentationClock} from '../web/clock.mjs';

test('TH05 cadence, fresh-frame delivery, jitter and accelerated cap',()=>{
 for(const fps of [26,56,60,120])for(const hz of [60,75,90,120,144,165,240]){
  const logic=new FrameClock(),limited=new PresentationClock(),free=new PresentationClock();let ticks=0,presented=0,unlimited=0;logic.add(0);
  for(let n=1;n<=hz*10;n++){const now=n*1000/hz;logic.add(now);while(logic.consume(1/fps))ticks++;if(limited.present(now,true))presented++;if(free.present(now,false))unlimited++;}
  assert.equal(ticks,fps*10,`TH05 ${fps} fps logic on ${hz} Hz`);assert.ok(presented>=599&&presented<=601,`TH05 60 fps presentation on ${hz} Hz: ${presented}`);assert.equal(unlimited,hz*10);
 }
 const clock=new FrameClock();clock.add(0);clock.add(30000);let steps=0;while(clock.consume(1/56))steps++;assert.ok(steps<=4);clock.reset();assert.equal(clock.consume(1/56),false);
 for(const fps of [26,56,60])for(const hz of [60,75,90,120,144,165,240])for(const jitter of [0,1]){
  const logic=new FrameClock(),screen=new PresentationClock();let generated=0,visible=0;logic.add(0);
  for(let n=1;n<=hz*30;n++){const time=n*1000/hz+(jitter?Math.sin(n*1.37)*.35:0);logic.add(time);let steps=0;while(logic.consume(1/fps)){generated++;steps++;}if(screen.fresh(time,true,steps,1/fps))visible++;}
  assert.equal(visible,generated,`${fps} fps fresh frames on ${hz} Hz (jitter ${jitter})`);
  assert.ok(Math.abs(visible-fps*30)<=1);
 }
 for(const hz of [75,90,120,144]){
  const logic=new FrameClock(),screen=new PresentationClock();let visible=0;logic.add(0);
  for(let n=1;n<=hz*30;n++){const time=n*1000/hz;logic.add(time);let steps=0;while(logic.consume(1/120))steps++;if(screen.fresh(time,true,steps,1/120))visible++;}
  assert.ok(Math.abs(visible-60*30)<=1,'accelerated game retains 60 Hz cap');
 }
});
