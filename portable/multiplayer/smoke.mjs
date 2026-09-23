import createModule from '/th08-sdl.mjs';

let core;
const Module=await createModule({canvas:document.getElementById('screen'),noInitialRun:true,
  instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('/th08-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}
});
if(core.sdl_files_variant(1)!==1)throw Error('Ordinary runtime rejected');
for(const path of ['/game','/fonts','/savesth08-multiplayer/replay'])Module.FS.mkdirTree(path);
for(const path of ['/input/th08.dat','/fonts/msgothic.ttc','/fonts/blend.bin','/fonts/cp932.bin']){
  const response=await fetch(path);if(!response.ok)throw Error(path);
  Module.FS.writeFile(path==='/input/th08.dat'?'/game/th08.dat':path,new Uint8Array(await response.arrayBuffer()));
}
function string(value,callback){const bytes=new TextEncoder().encode(value+'\0'),ptr=core.allocate(bytes.length);try{new Uint8Array(core.memory.buffer,ptr,bytes.length).set(bytes);return callback(ptr);}finally{core.deallocate(ptr);}}
string('#screen',core.sdl_canvas);core.sdl_music_enabled(0);
let app=0;
const nativeStatus=()=>Array.from(new Int32Array(core.memory.buffer,core.sdl_game_status(),10));
window.multiplayerSmoke={
  start(seed=1234,loadouts=null){
    if(app)throw Error('Use a fresh page for a new run');
    app=core.sdl_game_open(seed);if(!app)throw Error('Native app creation failed');
    if(loadouts){
      const words=[1,loadouts.length,0,1,seed,...loadouts.flatMap(character=>[character,0])];
      while(words.length<11)words.push(0);
      const ptr=core.allocate(44);try{
        new Uint32Array(core.memory.buffer,ptr,11).set(words);
        if(!core.multiplayer_configure(app,ptr,11))throw Error('Session rejected');
      }finally{core.deallocate(ptr);}
    }
    const total=core.sdl_prepare_total();for(let i=0;i<total;i++)if(core.sdl_prepare_next()<0)throw Error('Prepare failed '+i);
    if(!core.sdl_game_initialize())throw Error('Initialize failed');
    return nativeStatus();
  },
  ticks(count){for(let i=0;i<count;++i){const result=core.sdl_loop_tick(app,1/60,16);if(result)throw Error('Native tick failed '+result+' '+JSON.stringify(Array.from(new Int32Array(core.memory.buffer,core.diagnostics(app),16))));}return nativeStatus();},
  key(code,down){string(code,p=>core.sdl_key(p,down?1:0));},
  nativeStatus,
  status(){return Array.from(new Int32Array(core.memory.buffer,core.multiplayer_status(app),44));},
  diagnostics(){return Array.from(new Int32Array(core.memory.buffer,core.diagnostics(app),16));},
};
