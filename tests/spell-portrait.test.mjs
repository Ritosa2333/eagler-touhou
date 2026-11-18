import test from 'node:test';import assert from 'node:assert/strict';import {quadVertices} from '../site/runtime/draw.mjs';
test('original first-stage spell portrait accepts only its unrotated zero-depth mode2 projection',()=>{
 const record={mode:2,bank:3,script:11,rx:0,ry:0,rotation:0,z:0,x:336,y:0,width:400,height:512,sx:1,sy:1,anchorX:0,anchorY:1};
 assert.deepEqual(quadVertices(record),quadVertices({...record,mode:0}));
 assert.throws(()=>quadVertices({...record,rx:1}));assert.throws(()=>quadVertices({...record,script:12}));assert.throws(()=>quadVertices({...record,z:1}));
});
test('Extra uses the original flat boss04 cut-in without accepting other mode2 geometry',()=>{
 const actual={mode:2,bank:3,entry:20,sprite:43,script:17,layer:6,rx:0,ry:0,rotation:0,z:0,x:336,y:0,width:374,height:512,sx:1,sy:1,anchorX:0,anchorY:1};
 assert.deepEqual(quadVertices(actual),[[149,0],[523,0],[523,512],[149,512]]);
 for(const other of [{entry:21},{sprite:44},{script:18},{layer:9},{rx:.25},{ry:.25},{rotation:.25},{z:.25}])assert.throws(()=>quadVertices({...actual,...other}),/geometry mode 2/);
});
test('all four original third-stage cut-ins use their measured unrotated screen quads',()=>{
 const actual=[
  {"id":30000016,"bank":3,"entry":56,"sprite":111,"script":36,"layer":6,"mode":2,"anchorX":0,"anchorY":1,"x":464,"y":-31.9999981,"z":0,"width":383,"height":448,"sx":1,"sy":1,"rx":0,"ry":0,"rotation":0},
  {"id":30000016,"bank":3,"entry":58,"sprite":113,"script":38,"layer":6,"mode":2,"anchorX":0,"anchorY":1,"x":464,"y":-31.9999981,"z":0,"width":391,"height":448,"sx":1,"sy":1,"rx":0,"ry":0,"rotation":0},
  {"id":30000016,"bank":3,"entry":57,"sprite":112,"script":37,"layer":6,"mode":2,"anchorX":0,"anchorY":1,"x":464,"y":-31.9999981,"z":0,"width":334,"height":448,"sx":1,"sy":1,"rx":0,"ry":0,"rotation":0},
  {"id":30000016,"bank":3,"entry":59,"sprite":114,"script":39,"layer":6,"mode":2,"anchorX":0,"anchorY":1,"x":476.799988,"y":-35.2000008,"z":0,"width":448,"height":448,"sx":1,"sy":1,"rx":0,"ry":0,"rotation":0}
 ];
 for(const row of actual){
  assert.deepEqual(quadVertices(row),quadVertices({...row,mode:0}));
  assert.throws(()=>quadVertices({...row,entry:20}),/geometry mode 2/);
  assert.throws(()=>quadVertices({...row,z:1}),/geometry mode 2/);
 }
});
