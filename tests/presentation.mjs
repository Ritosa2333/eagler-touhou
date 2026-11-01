import assert from 'node:assert/strict';
import {FrameClock,PresentationClock} from '../web/clock.mjs';
for(const fps of [26,56,60,120])for(const hz of [60,75,90,120,144,165,240]){
 const logic=new FrameClock(),limited=new PresentationClock(),free=new PresentationClock();let ticks=0,presented=0,unlimited=0;logic.add(0);
 for(let n=1;n<=hz*10;n++){const now=n*1000/hz;logic.add(now);while(logic.consume(1/fps))ticks++;if(limited.present(now,true))presented++;if(free.present(now,false))unlimited++;}
 assert.equal(ticks,fps*10,`TH05 ${fps} fps logic on ${hz} Hz`);assert.ok(presented>=599&&presented<=601,`TH05 60 fps presentation on ${hz} Hz: ${presented}`);assert.equal(unlimited,hz*10);
}
const clock=new FrameClock();clock.add(0);clock.add(30000);let steps=0;while(clock.consume(1/56))steps++;assert.ok(steps<=16);clock.reset();assert.equal(clock.consume(1/56),false);
console.log('TH05 fixed game cadence and independent 60/unlimited presentation: PASS');
