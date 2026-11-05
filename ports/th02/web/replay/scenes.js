import {stable} from './tape.js';

export function validScene(scene){
  return scene===null||scene===undefined||scene&&typeof scene==='object'&&!Array.isArray(scene)&&
    ['stage','round','selection','ending'].includes(scene.kind)&&
    (!['stage','round'].includes(scene.kind)||Number.isInteger(scene[scene.kind]))&&
    Object.keys(scene).every(key=>key==='kind'||['stage','route','round'].includes(key)&&Number.isInteger(scene[key])&&scene[key]>=0&&scene[key]<=65535);
}
export const sceneKey=scene=>scene?stable(scene):'';
export function sceneLabel(scene,game){
  if(!scene)return '开局';
  if(scene.kind==='selection')return '角色选择';
  if(scene.kind==='ending')return '结局／菜单';
  if(scene.kind==='round')return `对局段 ${scene.round}`;
  if(game==='01')return `第 ${scene.stage} 关${scene.stage>5?' · '+(scene.route?'地狱':'魔界'):''}`;
  if(game==='04'&&scene.stage===6)return 'Extra';
  if(game==='02'&&scene.stage===5)return 'Extra';
  return `第 ${scene.stage+1} 面`;
}
