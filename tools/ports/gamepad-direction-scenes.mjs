import assert from 'node:assert/strict';
export const GAMEPAD_DIRECTIONS=Object.freeze([[-1,0],[1,0],[0,-1],[0,1],[-1,-1],[1,1],[-1,1],[1,-1]].map(Object.freeze));

export async function runGamepadDirections(io){
  const neutral=await io.set({mode:'buttons',x:0,y:0});const trials=[];
  for(const mode of ['buttons','axes'])for(const [x,y] of GAMEPAD_DIRECTIONS){
    const before=await io.observe(),press=await io.set({mode,x,y});const held=[];let release;
    try{for(let n=0;n<4;n++){await io.step(1);held.push(await io.observe());}}
    finally{release=await io.set({mode,x:0,y:0});}
    const released=[];for(let n=0;n<7;n++){await io.step(1);released.push(await io.observe());}
    trials.push({mode,x,y,before,press,release,held,released});
  }
  return{neutral,trials};
}

export function assertGamepadDirections(e,{movement,observation,isGameplayReady,epoch}){
  assert.equal(e.neutral.acknowledged,true);assert.equal(e.neutral.source,'production-gamepad');assert.equal(e.neutral.epoch,epoch);
  assert.deepEqual(e.neutral.vector,{mode:'buttons',x:0,y:0});assert.equal(e.neutral.bits,0);
  assert.deepEqual(e.trials.map(row=>[row.mode,row.x,row.y]),['buttons','axes'].flatMap(mode=>GAMEPAD_DIRECTIONS.map(([x,y])=>[mode,x,y])));
  const state=value=>{
    assert.equal(value.localSeat,0);assert.equal(isGameplayReady(value),true);assert.equal(value.input.inputSemantics,observation.inputSemantics);
    assert.ok(Number.isFinite(value.position.x)&&Number.isFinite(value.position.y));
  };
  for(const row of e.trials){
    assert.equal(row.press.acknowledged,true);assert.equal(row.press.source,'production-gamepad');assert.equal(row.press.epoch,epoch);
    assert.deepEqual(row.press.vector,{mode:row.mode,x:row.x,y:row.y});
    assert.equal(row.release.acknowledged,true);assert.equal(row.release.source,'production-gamepad');assert.equal(row.release.epoch,epoch);
    assert.deepEqual(row.release.vector,{mode:row.mode,x:0,y:0});
    const semantic=(row.x<0?64:row.x>0?128:0)|(row.y<0?16:row.y>0?32:0);
    assert.equal(row.press.bits,semantic,'The production gamepad owner must have sampled neutral before accepting held controls');
    assert.equal(row.release.bits,0);
    const expected=(row.x<0?movement.directionMasks.ArrowLeft:row.x>0?movement.directionMasks.ArrowRight:0)|
      (row.y<0?movement.directionMasks.ArrowUp:row.y>0?movement.directionMasks.ArrowDown:0);
    assert.equal(row.before.input.value,0);assert.equal(row.held.length,4);assert.equal(row.released.length,7);
    const samples=[row.before,...row.held,...row.released];samples.forEach(state);
    const clock=row.before.capabilities.restartFrame;assert.ok(clock);
    for(let i=1;i<samples.length;i++)assert.equal(samples[i].frames[clock],samples[i-1].frames[clock]+1,'Each gamepad sample must advance one native world tick');
    for(const value of row.held.slice(-2))assert.equal(value.input.value,expected);
    const held=row.held.at(-1);
    for(const [axis,sign] of [['x',row.x],['y',row.y]]){
      const delta=held.position[axis]-row.before.position[axis];if(sign)assert.ok(delta*sign>0);else assert.equal(delta,0);
    }
    const stable=row.released.slice(-3);
    for(const value of stable){assert.equal(value.input.value,0);assert.deepEqual(value.position,stable[0].position);}
  }
}
