import {decodeDrawBuffer} from '../../th128/site/runtime/draw.mjs';
import {FrameStepper} from '../../th128/site/runtime/timing.mjs';

// Original front constructors: green pause99, red GameOver109, the same text82/75 capture.
const frontRoot=1,shadeRoot=2,backdropRoot=3,rowIRQ={resume:7,end:8,save:9,manual:10,restart:11};
let captureSerial=0;
export async function openManagedPauseArt({resources,factory,signal}){
  signal.throwIfAborted();
  const [wasm,front,text,help,title]=await Promise.all([resources.read('wasm'),resources.raw('front.anm'),resources.raw('text.anm'),resources.raw('help.anm'),resources.raw('title.anm')]);
  signal.throwIfAborted();let module=await factory({wasmBinary:wasm,print(){},printErr(message){throw Error(message);}});
  signal.throwIfAborted();
  const clock=new FrameStepper();let snapshot=null,previous=null,cached=null,revision=0,postrun=false,postrunArt=[];
  const error=()=>module.UTF8ToString(module._th128_error());
  const check=()=>{const message=error();if(message)throw Error(message);};
  const ok=result=>{if(result!==1)throw Error(error()||'Original menu ANM rejected');};
  function tick(count){for(let i=0;i<count;i++)ok(module._th128_tick());}
  function irq(root,code){module._th128_irq(root,code);check();}
  function bind(menu){
    module._th128_reset();check();
    if(!menu.replaySave){
      if(!menu.help){ok(module._th128_bind(5,menu.kind==='game-over'?109:99,frontRoot));irq(frontRoot,3);}
      else if(menu.help.page)ok(module._th128_bind(46,9,20));
      else for(let i=0;i<9;i++){ok(module._th128_bind(46,i,10+i));irq(10+i,i===menu.help.topic?2:3);}
    }
    ok(module._th128_bind(6,82,shadeRoot));ok(module._th128_bind(6,75,backdropRoot));
    tick(10);
  }
  function reset(){snapshot=previous=cached=null;postrun=false;postrunArt=[];clock.reset();}
  function close(){reset();module=null;signal.removeEventListener('abort',close);}
  try{
    module._th128_reset();check();
    for(const [bank,bytes] of [[5,front],[6,text],[46,help],[24,title]]){
      const ptr=module._malloc(bytes.length);if(!ptr)throw Error('Original menu ANM allocation failed');
      try{module.HEAPU8.set(bytes,ptr);ok(module._th128_load_anm(bank,ptr,bytes.length));}finally{module._free(ptr);}
    }
  }catch(error){close();throw error;}
  signal.addEventListener('abort',close,{once:true});
  function frame(menu,sourceFrame,now){
    if(!menu){if(snapshot||postrun)reset();return null;}
    signal.throwIfAborted();if(!module)throw Error('Original menu ANM is closed');
    const kind=menu.kind??'pause';
    if(!snapshot||previous.kind!==kind){reset();const serial=++captureSerial;snapshot={serial,backdrop:{epoch:serial,sourceFrame}};}
    if(snapshot.backdrop.sourceFrame!==sourceFrame)throw Error('Confirmed menu frame changed');
    const {steps}=clock.advance(now,true),helpChanged=!!menu.help!==!!previous?.help||menu.help?.page!==previous?.help?.page;
    const rebuild=!previous||Boolean(menu.replaySave)!==Boolean(previous.replaySave)||helpChanged||previous.confirm&&(!menu.confirm||previous.selection!==menu.selection);
    const differentHelp=menu.help&&menu.help.topic!==previous?.help?.topic;
    const selected=rebuild||previous.selection!==menu.selection,confirming=menu.confirm&&(rebuild||!previous.confirm);
    if(rebuild)bind(menu);
    if(menu.replaySave){/* The preview/typewriter overlays the original captured background. */}
    else if(menu.help){
      if(!rebuild&&differentHelp&&!menu.help.page){irq(10+previous.help.topic,3);irq(10+menu.help.topic,2);tick(1);}
    }else{
      if(selected){irq(frontRoot,rowIRQ[menu.selection]);tick(1);}
      if(confirming){irq(frontRoot,14);tick(10);}
      if(menu.confirm&&(confirming||previous.yes!==menu.yes)){irq(frontRoot,menu.yes?15:16);tick(1);}
    }
    const changed=!cached||selected||differentHelp||confirming||previous?.yes!==menu.yes||previous?.replaySave?.revision!==menu.replaySave?.revision;
    previous={...menu,kind};if(!steps&&!changed)return cached;tick(steps);
    const count=module._th128_draw_count();check();
    const draws=decodeDrawBuffer(module.HEAPU8.buffer,module._th128_draw_ptr(),count,module._th128_draw_stride());
    // Keep the authored row positions. Script 104 is the red menu's Continue row, not Restart.
    const records=draws.filter(r=>r.bank===46||r.bank===5&&!(kind==='game-over'&&r.script===104)).map(r=>({...r,id:47000000+r.id,drawPass:'original-pause-front'}));
    const shadeColor=draws.find(r=>r.bank===6&&r.script===80)?.color??0xffffff;
    const backdrop=draws.filter(r=>r.bank===6).map(r=>r.script===82
      ?{...r,id:47100000+r.id,color:(shadeColor|0xff000000)>>>0,drawPass:'original-pause-shade'}
      :{...r,id:47200000+r.id,drawPass:'original-pause-backdrop'});
    if(menu.replaySave)records.push(...menu.replaySave.records);
    cached={revision:++revision,records,originalRecords:[...backdrop,...records],snapshot};return cached;
  }
  function postrunFrame(save,now){
    signal.throwIfAborted();if(!module)throw Error('Original replay art is closed');
    if(!postrun){reset();postrun=true;module._th128_reset();check();
      // The original standalone postrun binds title.anm script 112 for Replay.
      ok(module._th128_bind(24,112,frontRoot));tick(1);
    }
    const {steps}=clock.advance(now,true);if(steps)tick(steps);
    if(steps||!postrunArt.length){const count=module._th128_draw_count();check();
      postrunArt=decodeDrawBuffer(module.HEAPU8.buffer,module._th128_draw_ptr(),count,module._th128_draw_stride());}
    return [...postrunArt,...save.records];
  }
  return Object.freeze({frame,postrunFrame,reset,close});
}
