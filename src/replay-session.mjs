// Atomic retail-header initialization. Resources/stage selection are prepared
// by the caller; simulation and the synchronous main birth belong to the core.
export {replayInputEnded} from './replay.mjs';
export function startReplayStage(core,replay,stage) {
  if(typeof core._th12_start_replay!=='function')throw Error('当前游戏核心不支持录像关卡初始化，请更新核心');
  if(!replay||!Number.isInteger(replay.character)||replay.character<0||replay.character>2||
    !Number.isInteger(replay.shot)||replay.shot<0||replay.shot>1||
    !Number.isInteger(replay.difficulty)||replay.difficulty<0||replay.difficulty>4||
    !stage||!Number.isInteger(stage.number)||stage.number<1||stage.number>7||
    !Number.isInteger(stage.seed)||stage.seed<0||stage.seed>0xffff)throw Error('录像关卡参数无效');
  const i=stage.initial;
  if(!i||!Array.isArray(i.ufoColors)||i.ufoColors.length!==3||
    !Array.isArray(i.nativeGlobals)||i.nativeGlobals.length!==4)throw Error('录像开局记录无效');
  const words=[i.x,i.y,i.power,i.lives,i.bombs,i.score,i.pointValue,i.lifeFragments,i.bombFragments,
    ...i.ufoColors,i.rank,...i.nativeGlobals];
  if(!words.every(n=>Number.isInteger(n)&&n>=-0x80000000&&n<=0x7fffffff))throw Error('录像开局字段无效');
  const pointer=core._malloc(words.length*4);
  try {
    const heap=core.HEAPU8;
    if(!Number.isSafeInteger(pointer)||pointer<=0||!heap||pointer+words.length*4>heap.byteLength)
      throw Error('录像开局记录分配失败');
    const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength);
    words.forEach((value,index)=>view.setInt32(pointer+index*4,value,true));
    if(core._th12_start_replay(replay.character,replay.shot,replay.difficulty,stage.seed,pointer,words.length)!==1)
      throw Error('录像关卡初始化失败');
  } finally {if(pointer)core._free(pointer);}
}
