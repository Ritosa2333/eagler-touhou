import {provideTh128ManagedPackage} from '/host/author-package.mjs';
import {buildMultiplayerGameplayRelayUrl,buildMultiplayerLobbyRelayUrl} from '/host/author-relay-url.mjs';
import {readZipDirectory,readEntryData} from '/host/zip-reader.mjs';

const doc=document,origin=location.origin,button=doc.querySelector('#start'),status=doc.querySelector('#status'),log=doc.querySelector('#observed');
const clients=['p1','p2'].map((id,seat)=>({frame:doc.getElementById(id),seat,epoch:71+seat,
  member:crypto.randomUUID(),client:crypto.randomUUID(),calls:0,read:new Set(),messages:[],socket:null,lobby:null}));
const report={checks:[],phase:'waiting'};
function publish(){status.textContent=`启动器接口验收：${report.phase}`;log.textContent=JSON.stringify(report);}
const check=(name,yes,evidence)=>{report.checks.push({name,passed:!!yes,evidence});publish();if(!yes)throw Error(name);};
const sleep=ms=>new Promise(ok=>setTimeout(ok,ms));
async function until(fn,label,limit=20000){
  const deadline=performance.now()+limit;
  do{const got=fn();if(got)return got;const error=clients.flatMap(c=>c.messages).find(row=>row.event==='error'||row.type==='error');
    if(error)throw Error(label+': '+String(error.message??error.error));await sleep(32);
  }while(performance.now()<deadline);
  throw Error(label+' 超时');
}
function mount(parent){
  window.addEventListener('message',event=>{
    if(event.origin!==origin||event.data?.game!=='th128'||event.data?.protocol!=='eagler-touhou/1')return;
    const client=clients.find(c=>c.frame.contentWindow===event.source&&c.epoch===event.data.epoch);
    if(client)client.messages.push(event.data);
  });
  // This owned test parent supplies only the actual installed files to each
  // navigation. It calls the same author Launcher provider as the app.
  window.__eaglerPrepareManagedRuntimeDataV1=async request=>{
    const client=clients.find(c=>c.epoch===request.epoch);
    if(!client||request.game!=='th128'||request.generation!==parent.id||request.packageVersion!=='th128-dat-ogg/1'||client.calls++)throw Error('Invalid Package owner');
    const read=async(_,id)=>{
      const target=parent.descriptor.files[id],entry=parent.readEntries.get(target.source);
      const bytes=await readEntryData(entry);client.read.add(id);
      return {fileId:id,path:target.target,bytes:bytes.length,buffer:bytes.slice().buffer};
    };
    return provideTh128ManagedPackage({generation:parent,epoch:request.epoch,checkCurrent:()=>{
      if(client.frame.contentWindow?.location.pathname!==client.path)throw Error('Managed child navigation retired');
    },readData:generation=>read(generation,'game-data'),readResource:read});
  };
}
function draw(client){
  const canvas=client.frame.contentDocument?.querySelector('#game');
  return canvas?{scene:canvas.dataset.scene,frame:Number(canvas.dataset.frame??-1),epoch:Number(canvas.dataset.epoch??0)}:null;
}
function measureDisplay(frameWindow){
  const value={visible:frameWindow.document.visibilityState,callbacks:0,gaps:[]};
  let prior=null;
  function frame(time){
    value.callbacks++;value.visible=frameWindow.document.visibilityState;
    if(prior!==null){value.gaps.push(Math.round((time-prior)*10)/10);if(value.gaps.length>16)value.gaps.shift();}
    prior=time;value.focused=frameWindow.document.hasFocus();
    if(value.callbacks<160)frameWindow.requestAnimationFrame(frame);
  }
  frameWindow.requestAnimationFrame(frame);return value;
}
let nextRequest=0;
async function command(client,cmd,params={}){
  const request=`owned-mp-${++nextRequest}`;
  client.frame.contentWindow.postMessage({...params,protocol:'eagler-touhou/1',game:'th128',epoch:client.epoch,request,command:cmd},origin);
  const reply=await until(()=>client.messages.find(row=>row.request===request),cmd+' P'+client.seat);
  if(!reply.ok)throw Error(cmd+' P'+client.seat+': '+reply.error);return reply;
}
async function lobby(client,config){
  const {url}=buildMultiplayerLobbyRelayUrl(config.relay,{product:'th128mp',roomCode:'34772',clientId:client.client,memberId:client.member,
    intent:client.seat===0?'create':'join',visibility:'private',playerCount:2,difficulty:1});
  const socket=client.socket=new WebSocket(url),messages=[];client.lobby=messages;
  socket.addEventListener('message',event=>{const value=JSON.parse(event.data);messages.push(value);if(value.type==='error')client.messages.push(value);});
  await until(()=>socket.readyState===1,'房间连接');
  socket.send(JSON.stringify({type:'take-seat',seat:client.seat,loadout:0}));
  await until(()=>messages.find(value=>value.room?.seats?.[client.seat]?.clientId===client.client),'房间席位');
  socket.send(JSON.stringify({type:'resource-progress',status:'ready',stage:'runtime',percent:100,runtimeGeneration:config.generation}));
  await until(()=>messages.find(value=>value.room?.seats?.[client.seat]?.resource?.status==='ready'),'房间资源就绪');
  socket.send(JSON.stringify({type:'set-ready',ready:true,runtimeGeneration:config.generation}));
}
async function start(config){
  button.disabled=true;
  try{
    report.phase='connecting-package';publish();
    const contents=new Uint8Array(await (await fetch('/host/package.zip',{redirect:'error'})).arrayBuffer());
    const files=new Map(readZipDirectory(contents).entries.map(row=>[row.name,row]));
    const descriptor=JSON.parse(new TextDecoder().decode(await readEntryData(files.get('package.json'))));
    const parent={game:'th128',id:'owned-runtime-device',descriptor,
      files:Object.fromEntries(Object.entries(descriptor.files).map(([id,row])=>[id,{objectId:id,revision:row.revision}]))};
    // The file table above is the provider's installed object metadata. The
    // independent reader retains the ZIP entries, not edited package files.
    const entries=files;mount({...parent,readEntries:entries});
    for(const client of clients){
      const url=new URL(config.entry,origin);client.path=url.pathname;
      url.search=new URLSearchParams({hosted:'1',runtimeEpoch:String(client.epoch),runtimeVariant:'multiplayer',managedData:'1',managedPackage:'1',gameGeneration:parent.id});
      client.frame.src=url.href;
      await until(()=>client.messages.find(row=>row.event==='ready'&&row.roomPreview&&row.roomReturn),'子页面就绪',30000);
    }
    check('two independent navigations read all twelve actual package files',clients.every(c=>c.calls===1&&c.read.size===12),clients.map(c=>c.read.size));
    report.phase='title';publish();
    if(config.observeCadence)report.display={parent:measureDisplay(window),children:clients.map(c=>measureDisplay(c.frame.contentWindow))};
    const first=await Promise.all(clients.map(async c=>{
      await command(c,'configure',{music:'none',language:'zh'});await command(c,'prepare-room');return draw(c);
    }));
    check('native Title was drawn in both managed children',first.every(s=>s?.scene==='title'&&s.frame>=72),first);
    for(const client of clients)await lobby(client,config);
    const begun=clients[0];
    await until(()=>begun.lobby.find(row=>row.room?.seats?.slice(0,2).length===2&&row.room.seats.slice(0,2).every(seat=>seat?.ready)),'起局共识');
    begun.socket.send(JSON.stringify({type:'start',adonisMode:0,inputDelay:0,predictionLimit:2}));
    const start=await until(()=>begun.lobby.find(row=>row.type==='start'),'房间开始');
    check('author lobby selected a verified Runtime generation',start.room?.runtimeGeneration===config.generation,start.room?.runtimeGeneration);
    report.phase='launch';publish();
    const launchSerial=3;
    await Promise.all(clients.map(async c=>{
      const url=buildMultiplayerGameplayRelayUrl(config.relay,{product:'th128mp',roomCode:'34772',runId:start.serial,
        role:{player:c.seat},memberId:c.member,runtimeGeneration:config.generation});
      await command(c,'configure',{music:'none',options:{netplayMode:'lan',netplayPlayer:c.seat,netplayPlayerCount:2,
        netplayUrl:url,netplaySeed:34772,netplayDifficulty:1,netplaySpectator:false,netplaySpectatorId:'',netplaySpectatorCount:0,
        netplayInputDelay:0,netplayAdonisMode:0,netplayPredictionLimit:2,netplayRouteChoice:'direct',netplayIceServers:[],
        netplayLoadouts:[{character:0,shot:0},{character:0,shot:0}]}});
      await command(c,'launch',{launchSerial});
      await until(()=>c.messages.find(row=>row.event==='first-frame'&&row.launchSerial===launchSerial),'真游戏首帧');
    }));
    const paths=clients.map(c=>c.messages.find(row=>row.event==='runtime-info'&&row.netplayTransport)?.netplayTransport);
    check('the two compiled author peer instances really selected WebRTC',paths.every(path=>path==='rtc'),paths);
    const progress=await until(()=>{const frames=clients.map(draw);return frames.every(x=>x?.scene==='match'&&x.frame>=4)?frames:null;},'原生对局推进',12000);
    check('only actual native gameplay presentations produced first-frame acknowledgements',progress.every(x=>x.epoch===1&&x.frame>=4),progress);
    report.phase='return-room';publish();
    const saved=clients.map(c=>c.frame.contentDocument);
    await Promise.all(clients.map(c=>command(c,'return-room')));
    const after=clients.map(draw);
    check('return-room restores the same original title in the same documents',clients.every((c,i)=>c.frame.contentDocument===saved[i]&&
      c.messages.some(row=>row.event==='room-menu'&&row.launchSerial===launchSerial)&&after[i].scene==='title'&&after[i].frame===first[i].frame),after);
    const retired=await Promise.all(clients.map(c=>command(c,'exit')));
    check('both managed runtime channel owners acknowledge exit',retired.every(row=>row.ok===true));
    report.phase='done';
  }catch(error){report.phase='failed';report.problem=String(error.message);report.child=clients.map(c=>({draw:draw(c),messages:c.messages.slice(-18),
    status:c.frame.contentDocument?.querySelector('#status')?.textContent,files:c.read.size}));}
  finally{for(const c of clients){c.socket?.close(1000,'leave room');}publish();}
}

const config=await (await fetch('/host/config.json',{redirect:'error'})).json();
button.disabled=false;button.addEventListener('click',()=>void start(config),{once:true});publish();
