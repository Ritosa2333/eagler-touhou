const digest=/^[a-f0-9]{64}$/,maxBinary=32*1024*1024;
const baseKeys=['core','wasm','type9','peer','peerWasm'];
const check=(condition,error)=>{if(!condition)throw new Error(error);};
const hash=async bytes=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),byte=>byte.toString(16).padStart(2,'0')).join('');

function ownerPage(input){
  const page=new URL(input),match=/\/runtime\/th128\/multiplayer\/([a-f0-9]{64})\/th128\.html$/.exec(page.pathname);
  check(['https:','http:'].includes(page.protocol)&&!page.username&&!page.password&&match,'TH128 multiplayer Runtime generation is missing');
  check(page.searchParams.getAll('runtimeVariant').join(',')==='multiplayer'&&page.searchParams.getAll('managedData').join(',')==='1',
    'TH128 multiplayer Runtime needs the managed multiplayer variant');
  return {page,generation:match[1],base:new URL('./',page)};
}

function ownedPath(file,base){
  check(typeof file==='string'&&/^\/(?:[a-zA-Z0-9_-][a-zA-Z0-9_.-]*\/)*[a-zA-Z0-9_-][a-zA-Z0-9_.-]*$/.test(file),'Invalid TH128 program path');
  const url=new URL(file,base);
  check(url.origin===base.origin&&url.pathname===file&&url.pathname.startsWith(base.pathname),'TH128 program path is outside the owned generation');
  return url;
}

async function readBody(response,limit,{signal,expected}={}){
  check(response.ok&&!response.redirected&&response.body,'TH128 program response is not available');
  const declared=response.headers.get('Content-Length');
  if(declared!==null)check(/^\d+$/.test(declared)&&Number(declared)<=limit&&(expected===undefined||Number(declared)===expected),
    'TH128 program length differs from its receipt');
  const reader=response.body.getReader(),parts=[];let bytes=0;
  try{
    while(true){
      signal?.throwIfAborted();const next=await reader.read();if(next.done)break;
      bytes+=next.value.length;check(bytes<=limit,'TH128 program exceeds its byte limit');parts.push(next.value);
    }
  }catch(error){await reader.cancel().catch(()=>{});throw error;}
  finally{reader.releaseLock();}
  signal?.throwIfAborted();
  check(expected===undefined||bytes===expected,'TH128 program length differs from its receipt');
  const result=new Uint8Array(bytes);let offset=0;
  for(const part of parts){result.set(part,offset);offset+=part.length;}return result;
}

export async function readManagedProgram(input,{signal,fetchImpl=globalThis.fetch}={}){
  const {generation,base}=ownerPage(input),index=new URL('runtime/program.json',base);
  const response=await fetchImpl(index,{signal,redirect:'error'});
  const manifest=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(await readBody(response,8192,{signal})));
  const coop=manifest?.schema==='th128mp-program/2',keys=coop?[...baseKeys,'coopCore','coopWasm']:baseKeys;
  check((coop||manifest?.schema==='th128mp-program/1')&&digest.test(manifest.coreGeneration)&&Object.keys(manifest.files??{}).length===keys.length,
    'Invalid TH128 multiplayer program manifest');
  const common=`runtime/builds/${manifest.coreGeneration}/`,paths={core:common+'core.mjs',wasm:common+'core.wasm',
    type9:'runtime/bullet-type9-profile.json',peer:'runtime/transport/author-peer.mjs',peerWasm:'runtime/transport/author-peer.wasm'};
  if(coop){const id=manifest.coopGeneration;check(typeof id==='string'&&digest.test(id)&&manifest.files.coopWasm?.sha256===id&&id!==manifest.files.wasm?.sha256,'Invalid separate co-op battle identity');
    const peer=manifest.peerGeneration;check(typeof peer==='string'&&digest.test(peer),'Missing sender-aware cooperative transport');
    for(const [key,ext] of [['peer','mjs'],['peerWasm','wasm']])paths[key]=`runtime/transport/${peer}/author-peer.${ext}`;
    for(const [key,ext] of [['coopCore','mjs'],['coopWasm','wasm']])paths[key]=`runtime/coop/${id}/coop-core.${ext}`;}
  const files=Object.fromEntries(keys.map(key=>{
    const pin=manifest.files[key];
    check(pin?.path===paths[key],`Invalid TH128 program path for ${key}`);
    check(Number.isSafeInteger(pin.bytes)&&pin.bytes>0&&pin.bytes<=maxBinary&&digest.test(pin.sha256),`Invalid TH128 program receipt for ${key}`);
    return [key,Object.freeze({url:new URL(pin.path,base).pathname,bytes:pin.bytes,sha256:pin.sha256})];
  }));
  return Object.freeze({generation,...(coop?{gameplay:'same-stage-coop'}:{}),...files});
}

export async function readManagedProgramBytes(pin,input,{signal,fetchImpl=globalThis.fetch}={}){
  const {base}=ownerPage(input);
  check(Number.isSafeInteger(pin?.bytes)&&pin.bytes>0&&pin.bytes<=maxBinary&&digest.test(pin.sha256),'Invalid TH128 program byte receipt');
  const url=ownedPath(pin.url,base),response=await fetchImpl(url,{signal,redirect:'error'});
  const bytes=await readBody(response,pin.bytes,{signal,expected:pin.bytes});
  check(await hash(bytes)===pin.sha256,'TH128 program SHA-256 does not match its receipt');signal?.throwIfAborted();return bytes;
}
