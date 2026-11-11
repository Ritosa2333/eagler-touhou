import {FireControl} from './fire-control.js';
import {unpackTouch,MAX_INPUT} from './touch-input.js';
import {createVersusPause} from './native-versus-pause.mjs';

// Re-simulate only the immutable input prefix emitted by the players.
export function createConfirmedDriver(engine,options={}){
 const fire=[new FireControl(),new FireControl()],pause=createVersusPause();let frame=0,focus=[false,false];
 return {pause,get frame(){return frame;},get focus(){return focus;},step(pair){
  if(!Array.isArray(pair)||pair.length!==2||pair.some(v=>!Number.isSafeInteger(v)||v<0||v>MAX_INPUT))throw Error('确认输入无效');
  let inputs=pair;const menu=pause.advance(inputs,engine._th03_classic_scene(),engine._th03_classic_surrender_allowed());
  if(menu.paused){fire.forEach(f=>f.reset());focus=[false,false];frame++;return;}
  if(menu.resumed){fire.forEach(f=>f.reset());if(menu.result>1)engine._th03_classic_surrender(menu.seat,menu.result===3);inputs=[0,0];}
  inputs.forEach((value,seat)=>{const t=unpackTouch(value);engine._th03_classic_touch(seat,t.active,t.begin||false,t.unlimited,t.x,t.y);});
  const selecting=engine._th03_classic_scene()===3;
  const lanes=inputs.map((bits,index)=>(bits&31)|((selecting?fire[index].selection(!!(bits&512),!!(bits&32)):fire[index].sample(!!(bits&512),!!(bits&32),frame*1000/60,!(bits&1024)))?32:0));
  focus=inputs.map(value=>options.focus!==false&&!!(value&64));engine._th03_classic_assist_pair(focus[0],focus[1]);
  const system=lanes[0]|((engine._th03_classic_scene()!==9&&(inputs[0]&128))?0x1000:0)|((inputs[0]&256)?0x2000:0);
  const charge=((inputs[0]&32)?1:0)|((inputs[1]&32)?2:0);
  const result=engine._th03_classic_step_pair(lanes[0],lanes[1],system,charge);if(result!==0)throw Error('原生对局重放失败：'+result);frame++;
 }};
}
