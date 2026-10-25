import test from 'node:test';
import assert from 'node:assert/strict';
import {createNativeUiControls} from './native-ui-controls.mjs';

test('Native popup layout settles in its own window before targeting, without scrolling its parent',async()=>{
  let generation=1,advances=0;const raw=()=>{
    const w=Array(1024).fill(0);w.splice(0,8,2,1,generation,generation,1,1,1,0);w.splice(8,8,0,25,600,455,0,0,42,12);
    w.splice(128,16,28,300,20,460,200,18,1,Number(generation>1),1,generation,0,1,10,450,300,generation>1?480:450);
    return{available:true,words:w};
  };
  const frame={evaluate:async fn=>fn.name==='readNativeSnapshotRaw'?raw():{app:1,documentTimeOrigin:2,url:'https://localhost/runtime'}};
  const ui=createNativeUiControls({frame,page:{mouse:{wheel(){throw Error('Popup tried to scroll its parent');}}},
    runtime:{global:'testRuntime',module:'module',core:'core',app:'app'},contract:{snapshot:{export:'controls',wordCount:1024,wordType:'f64',app:true},width:640,height:480},advance:async()=>{generation++;advances++;}});
  const row=await ui.locate(28);assert.equal(row.state.controls[0].visible,true);assert.equal(advances,1);
});

test('Expanded child content waits for native scroll bounds before using the wheel',async()=>{
  let generation=1,advances=0,wheels=0,scrolled=false;const raw=()=>{
    const w=Array(1024).fill(0);w.splice(0,8,2,1,generation,generation,1,1,1,0);w.splice(8,8,0,25,600,455,scrolled?100:0,generation>1?100:0,42,12);
    w.splice(128,16,26,300,20,scrolled?40:460,200,18,1,Number(scrolled),1,generation,0,0,0,25,600,455);return{available:true,words:w};
  };
  const frame={evaluate:async fn=>fn.name==='readNativeSnapshotRaw'?raw():{app:1,documentTimeOrigin:2,url:'https://localhost/runtime'},locator:()=>({boundingBox:async()=>({x:0,y:0,width:640,height:480})})};
  const ui=createNativeUiControls({frame,page:{mouse:{move:async()=>{},wheel:async()=>{wheels++;scrolled=true;generation++;}}},
    runtime:{global:'testRuntime',module:'module',core:'core',app:'app'},contract:{snapshot:{export:'controls',wordCount:1024,wordType:'f64',app:true},width:640,height:480},advance:async()=>{generation++;advances++;}});
  assert.equal((await ui.locate(26)).state.controls[0].visible,true);assert.equal(advances,1);assert.equal(wheels,1);
});
