import createModule from '/th10-sdl.mjs';
let core;
const Module=await createModule({canvas:document.getElementById('screen'),noInitialRun:true,
 instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('/th10-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}
});
if(core.sdl_files_variant(1)!==1)throw Error('Ordinary runtime rejected');
for(const path of ['/game','/fonts','/savesth10-multiplayer/jp/replay'])Module.FS.mkdirTree(path);
for(const path of ['/input/th10.dat','/fonts/msgothic.ttc','/fonts/blend.bin','/fonts/codepages.bin']){
 const response=await fetch(path);if(!response.ok)throw Error(path);
 Module.FS.writeFile(path==='/input/th10.dat'?'/game/th10.dat':path,new Uint8Array(await response.arrayBuffer()));
}
function string(value,callback){const bytes=new TextEncoder().encode(value+'\0'),ptr=core.files_allocate(bytes.length);try{new Uint8Array(core.memory.buffer,ptr,bytes.length).set(bytes);return callback(ptr);}finally{core.files_free(ptr);}}
string('#screen',core.sdl_canvas);core.sdl_music_enabled(0);
let app=0,seatCount=0;
const status=()=>Array.from(new Int32Array(core.memory.buffer,core.multiplayer_status(app),44));
window.multiplayerSmoke={
 start(loadouts,local=0,difficulty=1,seed=1234){
  if(app)throw Error('Use a fresh page for a new run');
  app=core.sdl_game_open(0,seed);if(!app)throw Error('Native app creation failed');
  seatCount=loadouts.length;
  const words=[1,loadouts.length,local,difficulty,seed,...loadouts.flat()];while(words.length<11)words.push(0);
  const ptr=core.files_allocate(44);try{new Uint32Array(core.memory.buffer,ptr,11).set(words);if(!core.multiplayer_configure(app,ptr,11))throw Error('Session rejected');}finally{core.files_free(ptr);}
  return status();
 },
 ticks(count,buttons=[]){const ptr=core.files_allocate(seatCount*20);try{
  const inputs=new Uint32Array(core.memory.buffer,ptr,seatCount*5);
  inputs.fill(0);
  for(let seat=0;seat<seatCount;++seat)inputs[seat*5]=buttons[seat]||0;
  for(let i=0;i<count;++i){if(!core.multiplayer_commit_inputs(app,ptr,seatCount*5))throw Error('Input rejected');const result=core.sdl_loop_tick(app,1/60,16);if(result||core.application_error(app))throw Error('Native tick failed '+result+' '+core.application_error(app));}
 }finally{core.files_free(ptr);}return status();},
 status,
 nativeStatus(){return Array.from(new Int32Array(core.memory.buffer,core.sdl_game_status(),10));},
};
