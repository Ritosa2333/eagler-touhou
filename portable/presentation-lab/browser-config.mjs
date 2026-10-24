import {LabController} from './controller.mjs';
const protocol='eagler-touhou/1',game='th11';
let runtimeEpoch=0;
export default {
  game,title:'帧间显微镜',dataLabel:'选择自己的 th11.dat',dataUrl:'/input/th11.dat',
  runtimeUrl(epoch){runtimeEpoch=epoch;return `/runtime/th11.html?hosted=1&managedData=1&gameGeneration=presentation-lab&runtimeEpoch=${epoch}`;},
  runtime(target){
    const epoch=runtimeEpoch;let serial=0;
    const post=message=>new Promise((resolve,reject)=>{
      const request=`th11-lab-${epoch}-${++serial}`;
      const cleanup=()=>{clearTimeout(timer);window.removeEventListener('message',reply);};
      const reply=event=>{
        const value=event.data;
        if(event.source!==target||event.origin!==location.origin||value?.protocol!==protocol||value.game!==game||value.epoch!==epoch||value.request!==request)return;
        cleanup();value.ok?resolve(value):reject(Error(value.error||'TH11 Lab command failed'));
      };
      const timer=setTimeout(()=>{cleanup();reject(Error('TH11 Lab command timeout: '+message.command));},120000);
      window.addEventListener('message',reply);
      try{target.postMessage({protocol,game,epoch,...message,request},location.origin);}
      catch(error){cleanup();reject(error);}
    });
    const shim={get core(){return target.core;},get Module(){return target.Module;},command:post,launch:()=>post({command:'launch'}),
      stop:()=>{try{target.core?.th11_loop_stop?.();}catch{}}};
    return shim;
  },
  async configure(runtime){
    await runtime.command({command:'configure',language:'ja',music:'none',options:{touchEnabled:false},
      sharedResources:[{path:'/msgothic.ttc',url:'/msgothic.ttc'}],resources:[]});
  },
  async installReplay(runtime,bytes){runtime.labReplay=Uint8Array.from(bytes);},
  createController:(runtime,identity)=>{
    const controller=new LabController(runtime,identity);
    const params=new URLSearchParams(location.search);
    if(runtime.labReplay){
      controller.freeze();controller.driver.loadReplay(runtime.labReplay);
    }else if(params.has('stage')){
      controller.freeze();
      const stage=Number(params.get('stage')||1),character=Number(params.get('character')||0),difficulty=Number(params.get('difficulty')||1);
      if(!controller.driver.startStage({stage:Number.isInteger(stage)?stage:1,character:Number.isInteger(character)?character:0,difficulty:Number.isInteger(difficulty)?difficulty:1}))
        throw Error('TH11 diagnostic stage seed failed');
    }
    return controller;
  },
};
