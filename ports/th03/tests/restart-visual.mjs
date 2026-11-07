import assert from 'node:assert/strict';
import {createRestartVisualGate,isRestartStageIntro} from '../web/restart-visual.js';
const width=640,height=400,atlas=new Uint8Array(width*height*3),intro=new Uint8Array(atlas.length);
for(let i=0;i<intro.length;i+=3){intro[i]=0;intro[i+1]=0;intro[i+2]=160;atlas[i]=120;atlas[i+2]=120;}
assert.equal(isRestartStageIntro(atlas,width,height),false);assert.equal(isRestartStageIntro(intro,width,height),true);
let predicate,releases=0,time=0;const presenter={holdUntil(value){predicate=value;},releaseGate(){releases++;}};
let gate=createRestartVisualGate(presenter,()=>time);
assert.equal(predicate(intro,width,height),false);gate.program('MAINL.EXE');assert.equal(predicate(intro,width,height),false);assert.equal(predicate(atlas,width,height),false);assert.equal(predicate(intro,width,height),true);assert.equal(gate.closed,true);
gate=createRestartVisualGate(presenter,()=>time);gate.observe(null);gate.observe({phase:1});assert.equal(releases,1);assert.equal(gate.closed,true);
gate=createRestartVisualGate(presenter,()=>time);time=15001;gate.observe(null);assert.equal(releases,2);
console.log('TH03 restart visual gate PASS');
