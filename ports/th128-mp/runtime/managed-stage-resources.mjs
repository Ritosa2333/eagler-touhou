import {openManagedDatPackage} from '../../th128/site/runtime/managed-dat-owner.mjs';
import {decodeDrawBuffer} from '../../th128/site/runtime/draw.mjs';
import {cirnoPalettes,renderCirnoPalette} from './coop-cirno-palettes.mjs';
import {readManagedProgramBytes} from './managed-program.mjs';

const utf8=new TextEncoder(),decoder=new TextDecoder('utf-8',{fatal:true});
const hashPattern=/^[a-f0-9]{64}$/;
const gameExe='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
const typedProfiles=new Set([6,13,16,21,24,25,30]);
const groupProfiles=new Map([[4,'native'],[10,'boss'],[23,'spell'],[29,'spell']]);
const normalKeys=['core','wasm','type9'];
const banks=Object.freeze([[11,'ascii'],[24,'title'],[25,'title_v']].map(Object.freeze));
const sha=async value=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',value)),byte=>byte.toString(16).padStart(2,'0')).join('');
const abort=()=>new DOMException('Managed TH128 multiplayer resources were retired','AbortError');
const need=(condition,message)=>{if(!condition)throw new Error(message);};
function localPath(input,page){
  need(typeof input==='string'&&/^\/(?:[a-zA-Z0-9_-][a-zA-Z0-9_.-]*\/)*[a-zA-Z0-9_-][a-zA-Z0-9_.-]*$/.test(input),'Managed program needs a same-origin path');
  const url=new URL(input,page);need(url.origin===page.origin&&url.pathname===input,'Managed program URL changed identity');return url.pathname;
}
async function hexBytes(profile,size,label){
  need(profile&&typeof profile.hex==='string'&&profile.hex.length===size*2&&/^[a-f0-9]+$/.test(profile.hex)&&
    hashPattern.test(profile.sha256??''),`Invalid ${label} profile`);
  const value=Uint8Array.from(profile.hex.match(/../g),pair=>parseInt(pair,16));
  need(await sha(value)===profile.sha256,`${label} profile checksum mismatch`);return value;
}

// This owner consumes the already complete DAT + language + ten OGG supplied
// by the author Launcher. Game assets never fall back to a network URL or to
// the test rig's extracted bytes. Game code is a separate, verified Runtime.
export async function openManagedTh128StageResources({window=globalThis.window,document=window?.document,
  location=window?.location,signal,isCurrent=()=>true,program,loadProgram}){
  const page=new URL(location.href),controller=new AbortController(),ownSignal=controller.signal;
  need(typeof program?.generation==='string'&&hashPattern.test(program.generation),'Managed TH128 code generation is unavailable');
  const coop=program.gameplay==='same-stage-coop',reviewKeys=coop?[...normalKeys,'coopCore','coopWasm']:normalKeys;
  const pinned=Object.fromEntries(reviewKeys.map(key=>{
    const value=program[key];
    need(value&&Number.isSafeInteger(value.bytes)&&value.bytes>0&&value.bytes<=32*1024*1024&&hashPattern.test(value.sha256??''),`Missing managed program receipt: ${key}`);
    return [key,Object.freeze({url:localPath(value.url,page),bytes:value.bytes,sha256:value.sha256})];
  }));
  need(new Set(reviewKeys.map(key=>pinned[key].url)).size===reviewKeys.length,'Duplicate managed program receipt path');
  const generation=program.generation,cache=new Map();
  let closed=false,mounted;
  function check(){if(closed||signal?.aborted||!isCurrent())throw abort();}
  function close(){
    if(closed)return;closed=true;controller.abort();signal?.removeEventListener('abort',close);
    mounted?.owner.dispose();mounted?.provider.dispose();cache.clear();
  }
  const readProgram=loadProgram??(async path=>{
    const response=await fetch(new URL(path,page),{signal:ownSignal,redirect:'error'});
    need(response.ok,`Managed program HTTP ${response.status}`);return new Uint8Array(await response.arrayBuffer());
  });
  check();signal?.addEventListener('abort',close,{once:true});
  try{
    mounted=await openManagedDatPackage({window,document,location,signal:ownSignal,isCurrent:()=>!closed&&!signal?.aborted&&isCurrent()});check();
    const {provider,owner}=mounted,descriptor=provider.descriptor,allowed=new Set(provider.targets);
    const receipts=Object.entries(descriptor.files).sort(([a],[b])=>a<b?-1:a>b?1:0).map(([id,row])=>[id,row.target,row.bytes,row.sha256]);
    async function packageBytes(path){
      check();const target=localPath(path,page);need(allowed.has(target),'Undeclared managed package asset: '+target);
      const response=await provider.fetch(new URL(target,page).href,{signal:ownSignal});
      need(response.status===200,'Managed package asset unavailable: '+target);
      const body=new Uint8Array(await response.arrayBuffer());check();return body;
    }
    async function programBytes(key){
      const row=pinned[key],raw=await readProgram(row.url,ownSignal);check();
      need(raw instanceof Uint8Array&&raw.length===row.bytes,'Managed program byte receipt mismatch: '+key);
      const bytes=raw.slice();need(await sha(bytes)===row.sha256,'Managed program SHA-256 mismatch: '+key);check();return bytes;
    }
    await Promise.all(reviewKeys.map(async key=>cache.set(key,await programBytes(key))));
    check();const catalog=JSON.parse(decoder.decode(await packageBytes('/assets/catalog.json')));
    const manifestPath=catalog.variants?.zh?.manifest;
    need(typeof manifestPath==='string'&&/^generations\/[a-f0-9]{32}\/zh\/manifest\.json$/.test(manifestPath),'Managed TH128 language scope is not supported');
    const directory='/assets/'+manifestPath.slice(0,-'manifest.json'.length),manifest=JSON.parse(decoder.decode(await packageBytes('/assets/'+manifestPath)));
    need(manifest.schema==='th128-prepared-assets/1'&&manifest.game==='th128'&&manifest.variant==='zh','Invalid managed TH128 prepared language assets');
    const readAnimationFile=name=>packageBytes(directory+'animations/'+name);
    function raw(name){
      check();
      if(typeof name!=='string'||!/^[a-zA-Z0-9_-]{1,96}\.(?:ecl|anm|msg|sht|std)$/.test(name))return Promise.reject(new Error('Invalid managed TH128 raw asset name'));
      return packageBytes(directory+'runtime/'+name);
    }
    const byMusic=new Map();
    need(Array.isArray(manifest.music)&&manifest.music.length===10,'Managed TH128 stage requires ten OGG tracks');
    for(const row of manifest.music){
      const id=/^th128_(\d\d)\.wav$/.exec(row?.name)?.[1],target=`/music/th128_${id}.ogg`;
      const declared=descriptor.files[`ogg:th128_${id}.ogg`];
      need(id&&row.file===`music/ogg/th128_${id}.ogg`&&row.sha256===declared?.sha256&&row.bytes===declared?.bytes&&
        declared?.target===target&&allowed.has(directory+row.file)&&!byMusic.has(row.name),'Managed OGG track receipt differs');
      byMusic.set(row.name,Object.freeze({...row}));
    }
    async function music(name){
      check();const row=byMusic.get(name);need(row,'Unknown managed TH128 OGG track');return packageBytes(directory+row.file);
    }
    async function masks(){
      const record=JSON.parse(decoder.decode(await readAnimationFile('ice-masks.json')));
      need(record.schema==='th128-original-ice-masks/1'&&record.sourceExeSha256===gameExe&&record.profiles?.length===5,'Managed ice bitmap provenance differs');
      const parts=await Promise.all([21,25,33,49,65].map((rows,index)=>{
        const profile=record.profiles[index],words=index<3?1:2;
        need(profile.selector===index+1&&profile.rows===rows&&profile.words===words,'Managed ice bitmap layout differs');
        return hexBytes(profile,rows*words*4,'ice bitmap');
      }));
      const bytes=new Uint8Array(parts.reduce((count,part)=>count+part.length,0));let offset=0;
      for(const part of parts){bytes.set(part,offset);offset+=part.length;}return bytes;
    }
    async function numericProfile(type){
      if([8,18,20].includes(type)){
        const bytes=await packageBytes(`/assets/runtime/bullet-profile-${type}.bin`);
        need(bytes.length===288,'Managed binary bullet profile size differs');return bytes;
      }
      if(groupProfiles.has(type)){
        const kind=groupProfiles.get(type),payload=JSON.parse(decoder.decode(await readAnimationFile(`bullet-${kind}-profiles.json`)));
        const expected=kind==='native'?'th128-original-bullet-profiles/1':`th128-original-${kind}-bullet-profiles/1`;
        need(payload.schema===expected&&payload.sourceExeSha256===gameExe&&Array.isArray(payload.profiles)&&
          payload.profiles.length===(kind==='spell'?2:1),'Managed grouped bullet profile provenance differs');
        const rows=payload.profiles.filter(candidate=>candidate.type===type);
        need(rows.length===1,'Managed grouped bullet type is unavailable');return hexBytes(rows[0],288,`bullet${type}`);
      }
      if(typedProfiles.has(type)){
        const row=JSON.parse(decoder.decode(await readAnimationFile(`bullet-type${type}-profile.json`)));
        need(row.schema==='th128-original-typed-bullet-profile/1'&&row.sourceExeSha256===gameExe&&row.type===type&&row.bytes===288,'Managed typed bullet profile provenance differs');
        return hexBytes(row,288,`bullet${type}`);
      }
      throw Error(`Unsupported managed bullet type: ${type}`);
    }
    async function asset(key){
      if(key==='soundBindings')return packageBytes('/assets/runtime/sound-bindings.json');
      if(key==='atlas')return readAnimationFile('stage-catalog.json');
      if(key==='iceMasks')return masks();
      if(key==='baseBullet')return numericProfile(8);
      if(key==='bullet9')return cache.get('type9');
      if(key==='bullet32'){
        const path=provider.targets.filter(value=>value.endsWith('/zh/animations/bullet-a1-type32-profile.json'));
        need(path.length===1,'Managed A1 bullet table is unavailable');
        const bytes=await packageBytes(path[0]),row=JSON.parse(decoder.decode(bytes));
        need(row.schema==='th128-original-typed-bullet-profile/1'&&row.sourceExeSha256===gameExe&&row.type===32&&row.bytes===288,'Managed A1 bullet profile identity differs');
        await hexBytes(row,288,'bullet32');return bytes;
      }
      if(/^bullet\d{1,2}$/.test(key))return numericProfile(Number(key.slice(6)));
      if(/^anim_[a-zA-Z0-9_-]+$/.test(key))return readAnimationFile(key.slice(5)+'.json');
      throw Error(`Unsupported managed resource: ${key}`);
    }
    async function read(key){
      check();if(!cache.has(key))cache.set(key,asset(key));
      const value=await cache.get(key);check();return value.slice();
    }
    const bundle={schema:'th128-managed-stage/1',program:reviewKeys.map(key=>[key,pinned[key].bytes,pinned[key].sha256]),package:receipts};
    const identity=Object.freeze({generation,...(coop?{gameplay:'same-stage-coop'}:{}),coreSHA256:(coop?pinned.coopWasm:pinned.wasm).sha256,resourceSHA256:await sha(utf8.encode(JSON.stringify(bundle)))});
    check();return Object.freeze({identity,build:identity,revision:provider.revision,dataReceipt:owner.dataReceipt,
      base:new URL(directory,page).href,raw,read,music,endingManifest:manifest.nativeEndingAssets,musicTracks:Object.freeze([...byMusic.values()]),close,
      async fetch(input,init){
        check();const palette=coop&&cirnoPalettes.find(asset=>asset.url===input);
        if(!palette){const response=await provider.fetch(input,init);check();return response;}
        // Build only the chosen guest texture, using the caller's locally installed atlas.
        need((init?.method??'GET')==='GET'&&init?.body==null,'Invalid Cirno texture request');
        const url=new URL(palette.patchUrl),assetSignal=init?.signal?AbortSignal.any([ownSignal,init.signal]):ownSignal;
        const [data,layout]=await Promise.all([
          readManagedProgramBytes({...palette,url:url.pathname},page,{signal:assetSignal}),
          read('anim_pl00').then(bytes=>JSON.parse(decoder.decode(bytes))),
        ]);
        const atlas=layout.entries?.find(row=>row.index===0)?.texture;
        need(atlas?.width===256&&atlas?.height===256&&atlas.file==='textures/pl00-000.png','Original Cirno atlas unavailable');
        const response=await provider.fetch(new URL(atlas.file,new URL(directory,page)).href,{signal:assetSignal});
        need(response.ok,'Original Cirno texture is missing from the local Package');
        const image=await renderCirnoPalette(await response.blob(),data,assetSignal);check();
        return new Response(image,{headers:{'Content-Type':'image/png'}});
      },
      url:key=>{check();need(Object.hasOwn(pinned,key),'Unlisted managed program URL');return pinned[key].url;},
      async verifyMounts(rows){check();const verified=await owner.resources(rows);check();return verified;},
    });
  }catch(error){close();throw error;}
}

// The room title has its own display state. Actual presentation is the
// host's decision; reading draw commands does not acknowledge a browser draw.
export async function openManagedRoomTitle({resources,factory,onFactory,onTitleTick,signal}){
  const [wasm,...anm]=await Promise.all([resources.read('wasm'),...banks.map(([,name])=>resources.raw(`${name}.anm`))]);
  signal?.throwIfAborted();onFactory('title');
  let module=await factory({wasmBinary:wasm,print(){},printErr(message){throw Error(message);}}),ticks=0;
  const check=()=>{signal?.throwIfAborted();if(!module)throw abort();};
  const error=()=>module.UTF8ToString(module._th128_error());
  const ok=(status,label)=>need(status===1,error()||label);
  check();
  for(const [index,[bank]] of banks.entries()){
    const bytes=anm[index],pointer=module._malloc(bytes.length);need(pointer,'Managed title bank allocation failed');
    try{module.HEAPU8.set(bytes,pointer);ok(module._th128_load_anm(bank,pointer,bytes.length),'Managed title bank load failed');}
    finally{module._free(pointer);}
  }
  ok(module._th128_boot_title(0x1234),'Managed title boot failed');
  return Object.freeze({banks,ticks:()=>ticks,close(){module=null;},
    step(){check();onTitleTick();ok(module._th128_tick(),'Managed title tick failed');ticks++;},
    status(){check();return JSON.parse(module.UTF8ToString(module._th128_title_menu_state()));},
    draws(){check();const count=module._th128_draw_count();need(!error(),error());return decodeDrawBuffer(module.HEAPU8.buffer,module._th128_draw_ptr(),count,module._th128_draw_stride());},
  });
}
