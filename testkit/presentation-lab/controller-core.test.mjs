import test from 'node:test';
import assert from 'node:assert/strict';
import {PresentationLabControllerCore} from './controller-core.mjs';

function fixture() {
  let ticks=0,draws=0,stopped=0,fault=false,generation=0,token=null;
  const driver={
    describe:()=>({driverApiVersion:1,game:'fixture',adapterVersion:'fixture/1',nativeAbi:'fixture/1',features:{freeze:true,resume:true,step:true,drawOnly:true,references:true,stateEvidence:true,timing:true,capture:true}}),
    freeze:()=>{stopped++;return token??=Object.freeze({sessionEpoch:1,generation:++generation});},
    resume:value=>{assert.equal(value,token);token=null;},
    advanceOneTick:()=>{ticks++;return {status:'advanced',advancedTicks:1};},
    drawOnly:({alpha})=>{assert.equal(ticks,0);assert.ok(alpha>=0&&alpha<=1);draws++;return {status:'drawn'};},
    setInput:()=>{},clearInput:()=>{},close:()=>{},
  };
  const record=reference=>({tick:ticks,reference,records:[]});
  const observer={
    observationApiVersion:1,scanSchema:'test/scan/1',sessionSchema:'test/session/1',
    enable:()=>{},readReferences:()=>({previous:record(0),current:record(1)}),readObservation:()=>record(null),
    readStateEvidence:()=>[ticks],readTiming:()=>({ticks}),readTrace:()=>[ticks],readScene:()=>({ticks}),
    worldFrozen:()=>false,readGate:()=>true,setNegativeControl:value=>{fault=value;},
    captureImage:(_frame,alpha)=>({alpha}),
    analyze:input=>({schema:'test/audit/1',tick:input.current.tick,objects:[],purity:true}),
    compact:report=>report,mergeIssues:()=>[],
  };
  const controller=new PresentationLabControllerCore({driver,observer,identity:{wasm:'fixture'}});
  return {controller,counts:()=>({ticks,draws,stopped,fault})};
}

test('alpha preview is draw-only and restores the negative control',()=>{
  const {controller,counts}=fixture();controller.preview(.5,{negativeControl:true});
  assert.deepEqual(counts(),{ticks:0,draws:1,stopped:1,fault:false});
});
test('fast stepping executes every complete fixed tick',()=>{
  const {controller,counts}=fixture();controller.step(60);assert.equal(counts().ticks,60);
  assert.throws(()=>controller.step(0),/1\.\.600/);
});
test('sweep samples stable alpha endpoints without advancing simulation',()=>{
  const {controller,counts}=fixture();const report=controller.sweep({images:true});
  assert.equal(counts().ticks,0);assert.equal(counts().draws,8);assert.equal(report.images.length,5);
});
