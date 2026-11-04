import {readFile,writeFile,copyFile} from 'node:fs/promises';
import {resolve} from 'node:path';

export async function adaptRuntime({patch,replaceOnce,output,port,web,runtime,repo,files}) {
  const managedRoomApi=await readFile(resolve(port,'web/managed-room-api.js'),'utf8');
  await patch('app.js',text=>{
    const start=text.indexOf('function loadScores(){'),end=text.indexOf('const musicTracks=new Map();',start);
    if(start<0||end<0)throw Error('TH01 score API changed');
    text=text.slice(0,start)+`function loadScores(){
      const saved=globalThis.__eaglerPC98Files.memory.get('scores.dat');
      if(saved){if(saved.length!==core.th_score_size())throw Error('TH01 成绩文件大小无效');new Uint8Array(core.memory.buffer,core.th_score_data(),saved.length).set(saved);core.th_score_load(saved.length);}
      scoreRevision=core.th_get(110,0);formalScores=new Uint8Array(core.memory.buffer,core.th_score_data(),core.th_score_size()).slice();
    }
    function saveScores(){const revision=core.th_get(110,0);if(revision===scoreRevision)return;scoreRevision=revision;
      if(stageSelection(sessionConfig?.startStage).practice)return;
      formalScores=new Uint8Array(core.memory.buffer,core.th_score_data(),core.th_score_size()).slice();
      void globalThis.__eaglerPC98Files.write('scores.dat',formalScores).catch(error=>globalThis.__eaglerPC98Emit({event:'error',message:error.message}));
    }
    `+text.slice(end);
    text=replaceOnce(text,'function buttons(){','function nativeButtons(){','native input sampling');
    text=replaceOnce(text,'if(configuring){last=now;','managedMenuInput();if(configuring){last=now;','managed menu input');
    text=replaceOnce(text,'drawMs+=performance.now()-start;++draws;','drawMs+=performance.now()-start;++draws;++managedFrames;','render health');
    text=replaceOnce(text,"window.addEventListener('blur',()=>{if(core&&started&&!configuring)setPaused(true);});","window.addEventListener('blur',release);",'hosted focus release');
    text=replaceOnce(text,'else if(action===3||action===4){paused=true;stopEffects();release();}',"else if(action===3||action===4){paused=true;stopEffects();release();if(action===3)void managedExit();}",'managed room exit');
    text=text.replace("const dialog=$('solo-replay-dialog');\n  if(dialog){","const dialog=null;\n  if(dialog){");
    return text;
  });
  await writeFile(resolve(output,'app.js'),await readFile(resolve(output,'app.js'),'utf8')+'\n'+await readFile(resolve(port,'web/managed-solo-api.js'),'utf8'));
  await patch('netplay/session.js',text=>{
    text=replaceOnce(text,"import {createRelay} from './relay.js';","import {createManagedTransport} from '../managed-transport.mjs';\nconst createRelay=(_identity,_generation,roles,fail)=>createManagedTransport(globalThis.__eaglerPC98Options(),roles,fail);",'main transport');
    text=replaceOnce(text,"if(this.identity.role==='host')this.recorder.connectNetwork",'if(false)this.recorder.connectNetwork','standalone replay service');
    text=replaceOnce(text,"if(this.sawGameplay&&nativeState?.menu===1){this.fail(Error('已返回标题，本局结束。'),false);return;}","if(this.sawGameplay&&nativeState?.menu===1){void globalThis.__eaglerPC98RoomEnd();return;}",'confirmed native room exit');
    return text;
  });
  await writeFile(resolve(output,'netplay/app.js'),`import {NetSession} from './session.js';
const $=id=>document.getElementById(id),roles=['host','guest','guest2'];
let identity,room,session,returningTask=null;
const status=()=>{},render=()=>{};
const post=()=>{throw Error('房间由主项目管理');};
function clear(){globalThis.__eaglerPC98Input.clear();}
$('canvas').addEventListener('pointerdown',()=>{$('canvas').focus();void session?.game.unlockAudio().catch(()=>{});});
addEventListener('pagehide',()=>session?.close());
${managedRoomApi}`);
  await patch('netplay/runtime.js',text=>{
    text=replaceOnce(text,"localStorage.setItem('th01-native-net-scores-v1',btoa(String.fromCharCode(...data)));","void globalThis.__eaglerPC98Files.write('scores.dat',data);",'managed network score file');
    return replaceOnce(text,'await this.startRenderer(module,rendererSource);','// Render against the live core framebuffer.','managed native renderer');
  });
  await patch('netplay/runtime-worker-client.js',text=>replaceOnce(text,"localStorage.setItem('th01-native-net-scores-v1',btoa(String.fromCharCode(...result.scores)));","void globalThis.__eaglerPC98Files.write('scores.dat',new Uint8Array(result.scores));",'managed worker score file'));
  await writeFile(resolve(output,'room-title-worker.js'),`onmessage=async({data})=>{try{const core=(await WebAssembly.instantiate(data.module,{})).exports;core.th_init(1,1,0x12345678,0);core.th_menu_open(1,0);core.th_render();data.canvas.getContext('2d',{alpha:false}).putImageData(new ImageData(new Uint8ClampedArray(core.memory.buffer,core.th_pixels(),640*400*4),640,400),0,0);postMessage({type:'ready'});}catch(error){postMessage({type:'error',message:error.message});}};`);
  await patch('replay/audio.js',text=>{
    const start=text.indexOf('        context=new AudioContext'),end=text.indexOf('      })();',start);
    if(start<0||end<0)throw Error('TH01 replay audio adapter changed');
    return text.slice(0,start)+"        throw Error('TH01 原生录像不包含独立音乐轨');\n"+text.slice(end);
  });
  await patch('site.css',text=>text.replace("url('assets/launcher-background.webp')",'none'));
  await patch('bgm-version.js',text=>text.replace(/export async function loadMidiTracks\(signal\)\{[\s\S]*?\n\}/,"export async function loadMidiTracks(){return {};}"));
  for(const name of await files(resolve(port,'web')))if(/^managed-[^/]+\.(?:mjs|js)$/.test(name)&&!name.endsWith('-api.js')){await copyFile(resolve(port,'web',name),resolve(output,name));runtime.push(name);}
  for(const name of ['pc98-spectator-publisher.mjs','pc98-local-spectator.mjs','pc98-managed-spectator.mjs','directory-keyboard.mjs','keyboard-input-bridge.mjs','keyboard-owners.mjs','direct-input-bridge.mjs']){await copyFile(resolve(repo,'common/browser',name),resolve(output,name));runtime.push(name);}
  const strip=text=>text.replace(/<script\b[^>]*>[\s\S]*?<\/script>/g,'');
  let html=strip(await readFile(resolve(web,'solo.html'),'utf8'));
  html=html.replace(/<body([^>]*)>([\s\S]*?)<\/body>/,(_match,attributes,body)=>`<body${attributes}><template id="solo-content">${body}</template><script>const template=document.getElementById('solo-content');if(new URLSearchParams(location.search).get('runtimeVariant')!=='multiplayer')document.body.append(template.content.cloneNode(true));template.remove();</script></body>`);
  html=html.replace('</head>',`<style>html,body{margin:0;background:#000}#site-header,main>:not(#player),.diagnostics{display:none!important}main{margin:0;padding:0}#player{position:fixed;inset:0;border:0;padding:0;margin:0}#player>:not(#stage),#touch{display:none!important}#stage{width:100%;height:100%;max-width:none;max-height:none;aspect-ratio:auto;display:grid;place-items:center}#game{width:min(100vw,160vh);height:min(100vh,62.5vw);max-width:none;max-height:none;object-fit:contain;image-rendering:pixelated}</style><script type="module" src="managed-shell.mjs"></script></head>`);
  await writeFile(resolve(output,'th01.html'),html);runtime.push('th01.html');
}
