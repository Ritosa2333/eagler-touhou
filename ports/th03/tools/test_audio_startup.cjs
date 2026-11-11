const assert=require('node:assert/strict');
const fs=require('node:fs');
const path=require('node:path');
const vm=require('node:vm');
const root=path.resolve(__dirname,'../..');
for(const project of ['th03-netplay-lab','th04-coop-lab']){
  const helper=require(path.join(root,project,'tools/mobile-runtime/solo-audio.cjs'));
  const generated=project==='th03-netplay-lab'?['np21-solo.js','np21-60.js']:['np21-60.js'];
  for(const name of generated){
    const source=fs.readFileSync(path.join(root,project,'web/vendor/np2',name),'utf8');
    assert(source.startsWith(helper.injection())||source.includes(helper.injection()),`${project}/${name} must include the corrected source`);
  }
  let timer,cleared=false,renders=0,opening=true;
  const context={state:'running',sampleRate:44100,currentTime:1,destination:{},
    createBuffer:()=>({}),createBufferSource:()=>({connect(){},start(){},stop(){},disconnect(){}}),
    addEventListener(){},removeEventListener(){}};
  const audio={},Module={SDL2:{audio,audioContext:context}};
  const scope={setInterval:fn=>(timer=fn,1),clearInterval:()=>cleared=true};
  vm.createContext(scope);vm.runInContext(helper.injection()+'\nglobalThis.open=openBufferedSoloAudio;',scope);
  scope.open(Module,2,4096,()=>{assert(!opening,'Native callback re-entered audio initialization');renders++;});
  assert.equal(renders,0,'No synchronous render while SDL is opening the device');
  opening=false;timer();assert(renders>0,'PCM playback starts on the first deferred tick');
  assert.equal(audio.currentOutputBuffer,undefined);
  audio.scriptProcessorNode.disconnect();assert(cleared);const previous=renders;timer();assert.equal(renders,previous);
  console.log(`PASS ${project}: deferred audio startup, PCM playback, shutdown and generated runtime consistency`);
}
