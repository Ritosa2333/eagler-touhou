import createModule from '/th08-sdl.mjs';

let core;
const buildIdentity=await fetch('/build.json').then(response=>{if(!response.ok)throw Error('Missing Runtime identity');return response.json();});
let instantiatedSha256='';
const Module=await createModule({canvas:document.getElementById('screen'),noInitialRun:true,
  async instantiateWasm(imports,ready){
    const response=await fetch('/th08-sdl.wasm');if(!response.ok)throw Error('Missing Runtime WASM');
    const bytes=await response.arrayBuffer();instantiatedSha256=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),byte=>byte.toString(16).padStart(2,'0')).join('');
    if(instantiatedSha256!==buildIdentity.wasm)throw Error('Mixed Runtime identity');
    const {instance,module}=await WebAssembly.instantiate(bytes,imports);core=instance.exports;ready(instance,module);return core;
  }
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
  identity(){return {...buildIdentity,wasmSha256:instantiatedSha256,fixtureBuild:typeof core.mp_fixture_die==='function'};},
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
  commit(buttons){
    const ptr=core.allocate(buttons.length*2);
    try{new Uint16Array(core.memory.buffer,ptr,buttons.length).set(buttons);return core.multiplayer_commit_inputs(app,ptr,buttons.length)!==0;}
    finally{core.deallocate(ptr);}
  },
  nativeStatus,
  status(){return Array.from(new Int32Array(core.memory.buffer,core.multiplayer_status(app),44));},
  fixtureDie(seat){if(!core.mp_fixture_die)throw Error('Not a fixture build');return core.mp_fixture_die(app,seat);},
  fixturePlace(seat,x,y,dx=1,dy=1){if(!core.mp_fixture_place)throw Error('Not a fixture build');return core.mp_fixture_place(app,seat,x,y,dx,dy);},
  fixturePower(seat,power){if(!core.mp_fixture_power)throw Error('Not a fixture build');return core.mp_fixture_power(app,seat,power);},
  fixtureItems(kind,seat=0){if(!core.mp_fixture_items)throw Error('Not a fixture build');return core.mp_fixture_items(app,kind,seat);},
  fixtureItemStatus(){if(!core.mp_fixture_item_status)throw Error('Not a fixture build');return Array.from(new Uint32Array(core.memory.buffer,core.mp_fixture_item_status(app),9));},
  fixtureStatus(){if(!core.mp_fixture_status)throw Error('Not a fixture build');return Array.from(new Int32Array(core.memory.buffer,core.mp_fixture_status(app),20));},
  diagnostics(){return Array.from(new Int32Array(core.memory.buffer,core.diagnostics(app),16));},
};
