import {LabController} from './controller.mjs';
const protocol='eagler-touhou/1',game='th11';
let runtimeEpoch=0;
export default {
  game,title:'帧间显微镜',dataLabel:'选择自己的 th11.dat',dataUrl:'/input/th11.dat',
  runtimeUrl(epoch){runtimeEpoch=epoch;return `/runtime/th11.html?hosted=1&managedData=1&gameGeneration=presentation-lab&runtimeEpoch=${epoch}`;},
  runtime(target){
    const post=message=>new Promise((resolve,reject)=>{
      try{
        target.postMessage({protocol,game,epoch:runtimeEpoch,...message},location.origin);
        setTimeout(resolve,message.command==='launch'?600:150);
      }catch(error){reject(error);}
    });
    const shim={get core(){return target.core;},get Module(){return target.Module;},command:post,launch:()=>post({command:'launch'}),
      stop:()=>{try{target.core?.th11_loop_stop?.();}catch{}}};
    return shim;
  },
  async configure(runtime){
    await runtime.command({command:'configure',language:'ja',music:'none',options:{touchEnabled:false},
      sharedResources:[{path:'/msgothic.ttc',url:'/msgothic.ttc'}],resources:[]});
  },
  createController:(runtime,identity)=>{
    const controller=new LabController(runtime,identity);
    const params=new URLSearchParams(location.search);
    if(params.has('stage')){
      const stage=Number(params.get('stage')||1),character=Number(params.get('character')||0),difficulty=Number(params.get('difficulty')||1);
      if(!controller.driver.startStage({stage:Number.isInteger(stage)?stage:1,character:Number.isInteger(character)?character:0,difficulty:Number.isInteger(difficulty)?difficulty:1}))
        throw Error('TH11 diagnostic stage seed failed');
    }
    return controller;
  },
};
