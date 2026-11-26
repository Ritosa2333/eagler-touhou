import test from 'node:test';
import assert from 'node:assert/strict';
import {Renderer} from '../web/renderer.mjs';
import {commandMotion} from '../web/presentation-motion.mjs';
import {ReplayPresentationClock,PresentationClock} from '../web/presentation-clock.mjs';
import {Platform} from '../web/platform.mjs';

const quad=(x=100)=>({texture:{texture:1},identity:'object:birth:0',points:[x,100,x+32,100,x+32,116,x,116],uv:[0,0,1,0,1,1,0,1],tint:[1,1,1,1],depth:[1,.5,1,.5,1,.5,1,.5],mode:1,opaque:false,area:[0,0,640,480]});
const transform=(x=100,angle=0,scale=1)=>({x,y:100,angle,scale,left:-16,right:16,top:-8,bottom:8});
test('旋转沿最短角差推进，缩放围绕原中心且保持矩形形状',()=>{
  const before={...quad(),transform:transform(100,179*Math.PI/180,1.8)},current={...quad(120),transform:transform(120,-179*Math.PI/180,2)};
  const motion=commandMotion(current,before);
  assert.equal(motion.x,20);assert.equal(motion.y,0);assert.ok(Math.abs(motion.angle-2*Math.PI/180)<1e-12);
  assert.ok(Math.abs(current.transform.scale*(1+motion.scale*.5)-2.1)<1e-12);assert.equal(motion.originX,120);
});
test('同一精灵的图集换帧使用当前 UV，连续位置继续平滑',()=>{
  const before={...quad(),transform:transform()},current={...quad(108),transform:transform(108),uv:[.5,0,1,0,1,1,.5,1]};
  assert.equal(commandMotion(current,before).x,8);
  assert.equal(commandMotion({...current,transform:{...current.transform,right:32}},before),null);
});
test('圆形分段数改变时，后一个圆的图元身份与位移仍稳定',()=>{
  const renderer=Object.assign(Object.create(Renderer.prototype),{current:[],commands:[],previous:new Map(),identities:new Map(),white:{texture:1,width:1,height:1},brightness:[1,1,1],mode:0,area:[0,0,640,480]});
  for(let tick=0;tick<2;tick++){renderer.clear();renderer.tick=tick;renderer.scene='game';renderer.identity='circle-owner';renderer.circle(100,100,12+tick,[1,1,1,1]);renderer.circle(200+tick*4,200,16,[1,1,1,1]);renderer.commit();}
  const second=renderer.commands.filter(command=>command.identity.startsWith('circle-owner:circle:1:'));assert.equal(second.length,16);
  for(const command of second){const motion=commandMotion(command,renderer.previous.get(command.identity));assert.ok(motion);assert.equal(motion.points[0],4);assert.equal(motion.points[1],0);}
});
test('贴图、混合、裁剪、透视、圆形分段和瞬移切换保持当前状态',()=>{
  const before=quad(),current=quad(108);assert.deepEqual(commandMotion(current,before).points,[8,0,8,0,8,0,8,0]);
  for(const patch of [{texture:{texture:2}},{mode:3},{opaque:true},{area:[32,16,416,464]},{uv:[.5,0,1,0,1,1,.5,1]},{depth:[2,.5,1,.5,1,.5,1,.5]},{topology:'circle:13:true'},quad(200)])assert.equal(commandMotion({...current,...patch},before),null);
  assert.equal(commandMotion({...current,transform:transform(108)},before),null);
  assert.equal(commandMotion({...current,transform:transform(108,Math.PI)}, {...before,transform:transform()}),null);
});
test('额外呈现复用 GPU 顶点，新快照重新上传，命令与历史保持只读',()=>{
  let uploads=0,fraction=0;const renderer=Object.create(Renderer.prototype);
  const gl=new Proxy({bufferData(){uploads++;},uniform1f(_location,value){fraction=value;}},{get:(object,key)=>key in object?object[key]:/^[A-Z_]+$/.test(String(key))?1:()=>{}});
  const previous=quad(),current=quad(108);Object.assign(renderer,{gl,vertices:new Float32Array(96),commands:[current],previous:new Map([[current.identity,previous]])});
  const snapshot=JSON.stringify([renderer.commands,[...renderer.previous]]);
  for(let i=0;i<144;i++)renderer.present((i%12)/12);
  assert.equal(uploads,1);assert.equal(fraction,11/12);assert.equal(renderer.vertices[0],108);assert.equal(renderer.vertices[10],8);
  assert.equal(JSON.stringify([renderer.commands,[...renderer.previous]]),snapshot);
  renderer.current=[quad(116)];renderer.tick=2;renderer.committedTick=1;renderer.scene=renderer.committedScene='game';renderer.commit();renderer.present(.5);assert.equal(uploads,2);
  renderer.commands=[];renderer.previous.clear();renderer.present(.5);assert.equal(renderer.batches.length,0);
});
test('60 Hz 模式使用原顶点布局，切回高刷后重新准备运动参数',()=>{
  const sizes=[],alphas=[],renderer=Object.create(Renderer.prototype),current=quad(108);
  const gl=new Proxy({bufferData(_target,data){sizes.push(data.length);},uniform1f(_location,value){alphas.push(value);}},{get:(object,key)=>key in object?object[key]:/^[A-Z_]+$/.test(String(key))?1:()=>{}});
  Object.assign(renderer,{gl,attributes:[{location:0,size:2,offset:0},{location:4,size:4,offset:40}],vertices:new Float32Array(96),commands:[current],previous:new Map([[current.identity,quad()]])});
  renderer.setSmoothing(false);renderer.present(1);assert.equal(renderer.vertexStride,10);assert.deepEqual(sizes,[60]);assert.equal(alphas[0],0);
  renderer.setSmoothing(true);renderer.present(.5);assert.equal(renderer.vertexStride,16);assert.deepEqual(sizes,[60,96]);assert.equal(alphas[1],.5);assert.equal(renderer.vertices[10],8);
});
test('60 至 360 Hz 呈现限频保持 60 次每秒，解除限制跟随每次回调',()=>{
  for(const hz of [60,75,120,144,165,240,360])for(const limited of [true,false]){
    const clock=new PresentationClock();let count=0;for(let i=0;i<hz*10;i++)count+=Number(clock.due(i*1000/hz,limited));
    assert.equal(count,limited?600:hz*10);
  }
});
test('录像暂停恢复、切速和定位后等待新逻辑步，结束立即冻结显示',()=>{
  const clock=new ReplayPresentationClock(),playing={path:'replay/test.rpy',paused:false,seeking:false,ended:false,rate:1};
  clock.setPlayback(playing);clock.stepped(100);assert.equal(clock.alpha(110,playing,false),.6);
  clock.setPlayback({...playing,frame:8});assert.equal(clock.alpha(110,playing,false),.6);
  for(const changed of [{paused:true},{rate:2},{seeking:true},{path:'replay/next.rpy'},{ended:true}]){
    clock.setPlayback({...playing,...changed});clock.setPlayback(playing);assert.equal(clock.alpha(200,playing,false),0);
    clock.stepped(200);assert.equal(clock.alpha(210,playing,false),.6);
  }
  clock.stepped(220,true);assert.equal(clock.alpha(240,playing,false),0);
});
test('无音乐模式的回放定位和推进保留空音乐句柄',()=>{
  const platform=Object.assign(Object.create(Platform.prototype),{music:false,sounds:new Map([[1,{bgm:true,playing:true}]]),graphs:new Map()});
  platform.setSeeking(true);platform.seekAudioTick();platform.setSeeking(false);assert.equal(platform.silent,false);
});
