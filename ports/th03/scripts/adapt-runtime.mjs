import {readFile,writeFile,copyFile} from 'node:fs/promises';
import {resolve} from 'node:path';
export async function adaptRuntime({patch,replaceOnce,output,port,web,runtime,repo,files}){
  await patch('solo.js',text=>{
    const a=text.indexOf('async function storage('),b=text.indexOf('\nfunction validate(',a);
    if(a<0||b<0)throw Error('TH03 solo storage changed');
    text=text.slice(0,a)+`async function storage(language,mode,value){return undefined;}\n`+text.slice(b);
    text=replaceOnce(text,'const disk=playback?source:prepareSoloDisk(source);',`const disk=playback?source:prepareSoloDisk(source);
      if(!playback){const fat=new Fat12(disk);for(const [name,bytes]of globalThis.__eaglerPC98Files.memory)if(['YUME.NEM','YUME.CFG'].includes(name))fat.put('YUMEZIKU/'+name,bytes);}`,'main save import');
    text=replaceOnce(text,"bits=player.sample();for(const code of [...physical,...tapped])bits|=physicalActions[code]||0;tapped.clear();","bits=globalThis.__eaglerPC98Input.sample();tapped.clear();",'hosted solo input');
    text=replaceOnce(text,'const packed=player.pack(0),touch=unpackTouch(packed);',"const packed=packTouch(0,globalThis.__eaglerPC98Input.motion()),touch=unpackTouch(packed);",'hosted solo touch');
    text=replaceOnce(text,"import {unpackTouch} from './touch-input.js';","import {unpackTouch,packTouch} from './touch-input.js';",'touch codec');
    text=replaceOnce(text,'replayUI.offerSave(blob,value);','void managedSaveReplay(blob);','managed replay export');
    text=replaceOnce(text,'onExit:()=>{soloMusic?.reset();','onExit:()=>{void managedNaturalExit();soloMusic?.reset();','managed solo exit');
    return text;
  });
  await writeFile(resolve(output,'solo.js'),await readFile(resolve(output,'solo.js'),'utf8')+'\n'+await readFile(resolve(port,'web/managed-solo-api.js'),'utf8'));
  await patch('startup-assets.js',text=>replaceOnce(text,'async function cache(mode, key, value) {','async function cache(mode, key, value) {if(globalThis.__eaglerPC98Content)return undefined;','managed content cache').replace("new URL('runtime.json', vendor)","new URL('./vendor/dosbox-x/runtime.json', import.meta.url)").replace("new URL('wdosbox-x-fast.wasm', vendor).href","new URL('./vendor/dosbox-x/wdosbox-x-fast.wasm', import.meta.url).href"));
  await patch('app.js',text=>{
    text=replaceOnce(text,"&& /^[0-9]{4}$/.test(netQuery.get('room') || '')", "&& !!netQuery.get('room')",'main room identity');
    text=replaceOnce(text,"const response = await fetch(`/api/state?${query}`, {cache: 'no-store'});",'const response={ok:true,json:async()=>globalThis.th03RoomHost.state};','managed room settings');
    text=replaceOnce(text,'function report(error) {',"function report(error) {globalThis.__eaglerPC98Emit({event:'error',message:String(error.message||error)});",'managed error');
    text=replaceOnce(text,"if(netQuery.get('role')==='host')replayRecorder.connectNetwork",'if(false)replayRecorder.connectNetwork','standalone replay service');
    text=replaceOnce(text,"if (!controlsOnly&&(restartOnLoad || embedded || roomHost)) $('start').click();",'// The managed shell starts after configure.','managed lifecycle');
    return text;
  });
  await writeFile(resolve(output,'app.js'),await readFile(resolve(output,'app.js'),'utf8')+'\n'+await readFile(resolve(port,'web/managed-room-api.js'),'utf8'));
  await patch('controls.js',text=>{
    text=replaceOnce(text,"import {limitTouch,withoutPoint} from './touch-input.js';","import {limitTouch,withoutPoint,packTouch} from './touch-input.js';",'managed touch codec');
    return replaceOnce(text,'  function sampleLocal() {',`  let managedPrevious=0;
  function sampleLocal() {
    if(globalThis.__eaglerPC98Input){
      const network=getNetwork?.();if(!network)return;
      const bits=globalThis.__eaglerPC98Input.sample(),edge=bits&~managedPrevious;managedPrevious=bits;
      if(isPaused()){
        if(edge&1)onMenu(0,'up');if(edge&2)onMenu(0,'down');if(edge&(32|256|512))onMenu(0,'confirm');if(edge&(16|128))onMenu(0,'resume');
        network.clearLocal();return;
      }
      if(edge&128)onEscape();
      const actions=new Set();for(const [mask,action]of [[1,'up'],[2,'down'],[4,'left'],[8,'right'],[16,'attack'],[32,'shot'],[512,'rapid'],[64,'focus'],[256,'shot']])if(bits&mask)actions.add(action);
      network.setLocal(actions,packTouch(0,globalThis.__eaglerPC98Input.motion()),{focusEnabled:true,points:false});return;
    }`,'managed network input');
  });
  await patch('netplay.js',text=>{
    text="import {createManagedTransport} from './managed-transport.mjs';\n"+text;
    const a=text.indexOf('  async start(){'),b=text.indexOf('  async state(){',a);
    if(a<0||b<0)throw Error('TH03 transport lifecycle changed');
    text=text.slice(0,a)+`  async start(){
      const options=globalThis.__eaglerPC98Options();this.localSeat=options.netplayPlayer;
      this.network='lan';this.ice='all';this.metricsTimer=setInterval(()=>this.observeConnection(),500);
      this.managedTransport=createManagedTransport(options,['host','guest'],error=>this.fail(error));
      await new Promise((resolve,reject)=>{
        this.rtcResolve=resolve;this.rtcReject=reject;
        this.rtcTimer=setTimeout(()=>reject(Error('等待另一端连接超时')),120000);
        this.attach(this.managedTransport.channelFor(this.role==='host'?'guest':'host'));
        this.managedTransport.connect();
      });
    }
`+text.slice(b);
    return replaceOnce(text,'close(){if(this.closed)return;this.closed=true;', 'close(){if(this.closed)return;this.closed=true;this.managedTransport?.close();','managed transport cleanup');
  });
  await patch('room-title-worker.js',text=>{
    text=replaceOnce(text,"emulator = await (await import('./room-title-th05.js')).createTitleCore({canvas, disk, wasmBinary});","throw Error('TH03 title only');",'title core closure');
    text=replaceOnce(text,'async function boot({game, canvas, disk, wasmBinary, module})','async function boot({game, canvas, disk, wasmBinary, module,managedTitle=false})','managed title');
    return replaceOnce(text,"send('ready', {frames, pixels, program: m.UTF8ToString(m._emGetRunningProgram())});","send('ready', {frames, pixels, program: m.UTF8ToString(m._emGetRunningProgram())});if(managedTitle)return;",'hold title');
  });
  await patch('replay/audio.js',text=>text
    .replace("if(game==='01'){const {LocalAudio}=await import('../netplay/audio.js');native=new LocalAudio();await native.unlock();context=native.context;await context.suspend();return;}","if(game!=='03')throw Error('TH03 Replay only');")
    .replace("const {LocalBgm}=await import('../netplay/local-bgm.js');bgm=new LocalBgm(context,()=>{});","throw Error('TH03 Replay only');")
    .replace("const {LocalSe}=await import('../netplay/local-se.js');sfx=new LocalSe(context);","throw Error('TH03 Replay only');")
    .replace("const {LocalSfx}=await import('../netplay/local-sfx.js');sfx=new LocalSfx(context);","throw Error('TH03 Replay only');"));
  await patch('replay/external-loader.js',text=>text.replace("new URL('dosbox-jspi.mjs',vendor)","new URL('../vendor/dosbox-x/dosbox-jspi.mjs',import.meta.url)").replace("new URL('wdosbox-x-fast.wasm',vendor)","new URL('../vendor/dosbox-x/wdosbox-x-fast.wasm',import.meta.url)"));
  await patch('replay/battle-boot.js',()=>`export async function bootBattleResources(){throw Error('TH03 多人录像播放尚未接入');}\n`);
  await patch('bgm-version.js',text=>text.replace(/export async function loadMidiTracks\(signal\)\{[\s\S]*?\n\}/,"export async function loadMidiTracks(){return {};}"));
  await patch('site.css',text=>text.replace("url('assets/launcher-background.webp')",'none'));
  for(const name of await files(resolve(port,'web')))if(!name.endsWith('-api.js')){await copyFile(resolve(port,'web',name),resolve(output,name));runtime.push(name);}
  for(const name of ['directory-keyboard.mjs','keyboard-input-bridge.mjs','keyboard-owners.mjs','direct-input-bridge.mjs']){await copyFile(resolve(repo,'common/browser',name),resolve(output,name));runtime.push(name);}
  const strip=text=>text.replace(/<script\b[^>]*>[\s\S]*?<\/script>/g,'');
  await writeFile(resolve(output,'managed-room.html'),strip(await readFile(resolve(web,'local.html'),'utf8')));runtime.push('managed-room.html');
  let html=strip(await readFile(resolve(web,'solo.html'),'utf8'));
  html=html.replace('</head>',`<style>html,body{margin:0;background:#000}#site-header,main>*:not(#screen),.player-toolbar,.player-actions,.player-movement,.player-note,.touch-surface,[class^="touch-"],[data-layout-control]{display:none!important}main{margin:0;padding:0}#screen{position:fixed;inset:0;margin:0;padding:0}.player-stage{width:100%;height:100%}</style><script type="module" src="managed-shell.mjs"></script></head>`);
  await writeFile(resolve(output,'th03.html'),html);runtime.push('th03.html');
}
