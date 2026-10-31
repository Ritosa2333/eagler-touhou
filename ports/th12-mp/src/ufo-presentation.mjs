// Read-only FontManager submissions from TH12 1.00b 44a4f0/44a5d0.
// Enemy identity, clocks, rewards and ANM health bars remain in the core.
const f32=Math.fround;
const width=(value,size)=>String(Math.trunc(value)).padStart(size,' ');
const precision=(value,size)=>(value<0?'-':'')+String(Math.abs(Math.trunc(value))).padStart(size,'0');
const grouped=value=>String(Math.trunc(value)).replace(/\B(?=(\d{3})+(?!\d))/g,',');
// UFO-specific immediate callbacks:449fa0 registers44a870 (panel text) at
// priority60 and44a880 (reward/timer text) at22. These are not Font queues.
export function ufoTextRenderPriority(pose) {
  return pose.font===3||(pose.font===2&&pose.drawFlag===0)?60:22;
}

export function readCoreUfo(core) {
  if(!core._th12_ufo||!core._th12_ufo_size)return null;
  const ptr=core._th12_ufo(),size=core._th12_ufo_size(),heap=core.HEAPU8;
  if(!Number.isSafeInteger(ptr)||!Number.isSafeInteger(size)||ptr<0||size<2||size>4096||
    !heap||ptr+size>heap.byteLength)throw Error('UFO 呈现记录无效');
  const state=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(heap.subarray(ptr,ptr+size)));
  const fields=['enemyId','age','duration','fill','bucketA','bucketB','displayFrames','multiplier','score','rewardX','rewardY','rewardZ'];
  const integers=['enemyId','age','duration','bucketA','bucketB','displayFrames','score'];
  if(!state||Array.isArray(state)||!fields.every(key=>Number.isFinite(state[key]))||
    !integers.every(key=>Number.isInteger(state[key]))||state.enemyId<0||state.enemyId>0xffffffff)
    throw Error('UFO 呈现状态无效');
  if(state.enemy!==null&&(!state.enemy||Array.isArray(state.enemy)||
    !['id','x','y','z','health','maxHealth'].every(key=>Number.isFinite(state.enemy[key]))||
    !['id','health','maxHealth'].every(key=>Number.isInteger(state.enemy[key]))))
    throw Error('UFO 敌人状态无效');
  return state;
}

function pose(text,font,x,y,z=0,options={}) {
  return {text,font,x:f32(x),y:f32(y),z:f32(z),scaleX:1,scaleY:1,
    colorARGB:0xffffffff,alignX:1,alignY:1,drawFlag:0,...options};
}

// enemy uses the live Enemy's world coordinates, while rewardX/Y are the
// retained screen coordinates at defeat, not the moving UFO's coordinates.
export function ufoTextPoses(input={}) {
  if(!input)return [];
  const {enemyId=0,age=0,duration=720,fill=0,bucketA=0,bucketB=0,
    enemy=null,displayFrames=0,multiplier=0,score=0,rewardX=0,rewardY=0,rewardZ=0}=input;
  const output=[];
  if(enemyId&&age>=20) {
    output.push(pose(width(bucketA,3),3,512,260),pose(width(bucketB,3),3,512,280));
    output.push(pose(width(Math.trunc(f32(fill)*100),3)+'%',2,512,310));
  }
  if(displayFrames>0) {
    const reward={alignX:0,alignY:0,drawFlag:1};
    output.push(pose('*'+f32(multiplier).toFixed(1),4,rewardX,rewardY,rewardZ,
      {...reward,scaleX:2,scaleY:2,colorARGB:displayFrames%10<5?0xffffff00:0xff808080}));
    if(score>0)output.push(pose(grouped(score),4,rewardX,f32(f32(rewardY)+32),rewardZ,reward));
  }
  if(enemyId&&age>=60&&enemy?.id===enemyId) {
    // 44a7b6 stores seconds to f32 before both truncations. Computing
    // hundredths directly from integer ticks changes native boundary values.
    const seconds=f32((duration-age)/60),whole=Math.trunc(seconds),fraction=Math.trunc(seconds*100)%100;
    const x=f32(f32(f32(enemy.x)+224)-12),y=f32(f32(f32(enemy.y)+16)+20);
    output.push(pose(width(whole,2)+':'+precision(fraction,2),2,x,y,enemy.z??0,
      {colorARGB:0xc080ffc0,drawFlag:1}));
  }
  return output;
}
