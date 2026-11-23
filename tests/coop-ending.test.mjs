import test from 'node:test';import assert from 'node:assert/strict';import {createCoopEndingOwner} from '../runtime/coop-ending.mjs';
test('a sealed main clear exclusively owns the original ending, applies media before advancing and returns to the saved result',async()=>{
 let ticks=0,acks=0,input=[],frames=[];const life=new AbortController(),delegate={index:0,stage:3,fresh:true,
 step(keys){assert.equal(acks,1,"boot media is acknowledged before tick 1");input.push(keys);ticks++;return this.snapshot();},snapshot(){return {endingTicks:ticks,media:[{serial:1,operation:8,path:'bgm/th128_09.wav',durationFrames:0,acknowledged:!!acks}],originalReturnRequested:ticks===2,returnScene:ticks===2?16:0};},
 acknowledge(command){assert.equal(command.serial,1);acks++;}};
 const owner=createCoopEndingOwner({native:delegate,signal:life.signal,paint:s=>frames.push(s.endingTicks),media:c=>assert.equal(c.operation,8),onComplete:s=>frames.push('done:'+s.returnScene)});
 await owner.advance(0);owner.key({code:'KeyZ',down:true});owner.key({code:'KeyZ',down:false});await owner.advance(20);assert.equal(acks,1);assert.equal(input[0].pressed,1);
 await owner.advance(40);assert.deepEqual(frames,[1,2,'done:16']);assert.ok(owner.finished());await owner.advance(60);assert.equal(ticks,2);
});
