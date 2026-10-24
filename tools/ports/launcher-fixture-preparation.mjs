import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {spawn} from 'node:child_process';
import {createServer} from 'node:net';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {PRODUCT_CONTENT} from '../../launcher/lib/content-definition.mjs';
import {extractGameDataLayout} from '../../launcher/lib/runtime-data-layout.mjs';
import {createPublicationHostSeed} from '../../launcher/lib/publication-host-seed.mjs';
import {readVerifiedRuntimeFile} from './acceptance-contracts.mjs';
const root=resolve(import.meta.dirname,'../..'),launcher=resolve(root,'launcher');
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const appendTail=(current,chunk)=>`${current}${chunk}`.slice(-300_000);
const pause=ms=>new Promise(resolve=>setTimeout(resolve,ms));

/** Shared Launcher packaging inputs and owned local-server lifecycle. */
export async function availableFixturePort(){
  const probe=createServer();
  await new Promise((accept,reject)=>{probe.once('error',reject);probe.listen(0,'127.0.0.1',accept);});
  const port=probe.address().port;
  await new Promise((accept,reject)=>probe.close(error=>error?reject(error):accept()));
  return port;
}
export async function assertPortAvailable(port){
  const probe=createServer();
  await new Promise((accept,reject)=>{probe.once('error',reject);probe.listen(port,'127.0.0.1',accept);});
  await new Promise((accept,reject)=>probe.close(error=>error?reject(error):accept()));
}

export async function runLogged(command,args,{cwd,stdoutPath,stderrPath,label}){
  const child=spawn(command,args,{cwd,windowsHide:true,stdio:['ignore','pipe','pipe']});
  let stdout='',stderr='';
  child.stdout.on('data',chunk=>{stdout=appendTail(stdout,chunk.toString());});
  child.stderr.on('data',chunk=>{stderr=appendTail(stderr,chunk.toString());});
  const [code,signal]=await new Promise((accept,reject)=>{
    child.once('error',reject);child.once('close',(exitCode,exitSignal)=>accept([exitCode,exitSignal]));
  });
  if(stdoutPath)await writeFile(stdoutPath,stdout);
  if(stderrPath)await writeFile(stderrPath,stderr);
  if(code!==0)throw Error(`${label} exited ${code??signal}: ${stderr.slice(-6000)||stdout.slice(-6000)}`);
  return{exitCode:code,stdoutTail:stdout.slice(-8000),stderrTail:stderr.slice(-8000)};
}

export async function startStaticServer(site,port){
  const script=resolve(launcher,'scripts/serve-static.mjs');
  const child=spawn(process.execPath,[script,site,String(port)],{cwd:launcher,windowsHide:true,stdio:['ignore','pipe','pipe']});
  let stdout='',stderr='',closed=null;
  child.stdout.on('data',chunk=>{stdout=appendTail(stdout,chunk.toString());});
  child.stderr.on('data',chunk=>{stderr=appendTail(stderr,chunk.toString());});
  child.once('error',error=>{closed=error;});
  child.once('close',(code,signal)=>{closed=new Error(`static server exited ${code??signal}: ${stderr.slice(-4000)||stdout.slice(-4000)}`);});
  const baseUrl=`http://127.0.0.1:${port}/`,deadline=Date.now()+10_000;
  while(Date.now()<deadline){
    if(closed)throw closed;
    try{const response=await fetch(new URL('deployment.json',baseUrl),{redirect:'error',signal:AbortSignal.timeout(1500)});
      if(response.ok)return{child,baseUrl,stdout:()=>stdout,stderr:()=>stderr};
    }catch{}
    await pause(100);
  }
  child.kill();
  throw Error(`Local site did not become ready on ${baseUrl}: ${stderr.slice(-4000)||stdout.slice(-4000)}`);
}

export async function stopChild(child){
  if(!child||child.exitCode!==null||child.signalCode!==null)return;
  child.kill();
  const closed=await Promise.race([new Promise(accept=>child.once('close',()=>accept(true))),pause(3000).then(()=>false)]);
  if(!closed)throw Error(`Static server process ${child.pid??'unknown'} did not exit after shutdown`);
}

function runtimeLayout(target,identity){
  const product=PRODUCT_GAMES[target.game];
  if(product.dataProvider==='retail-memory')return PRODUCT_CONTENT[target.game]?.dataLayout;
  if(product.dataProvider!=='emscripten-preload')throw Error(`${target.game}: unsupported DATA provider ${product.dataProvider}`);
  return null;
}

export async function preloadLayout(target,identity){
  const product=PRODUCT_GAMES[target.game];
  if(product.dataProvider==='retail-memory'){
    const layout=runtimeLayout(target,identity);
    assert.ok(layout,`${target.game}: declared retail DATA layout is missing`);
    return layout;
  }
  for(const name of Object.keys(identity.files).filter(path=>/\.(?:js|mjs)$/i.test(path))){
    const bytes=await readVerifiedRuntimeFile(target,identity,name);
    try{return extractGameDataLayout(bytes.toString('utf8'),target.game).layout;}catch{}
  }
  throw Error(`${target.id}: packaged Runtime has no verifiable Emscripten DATA layout`);
}

export function createSiteSeed(game,data,dataLayout,profile='web-validation-offline-desktop'){
  const seed=createPublicationHostSeed(profile);
  seed.schema='eagler-touhou/site-manifest/1';
  seed.shared={resourceMode:'hosted',vanillaFont:'shared/msgothic.ttc?v=fixture',unicodeFont:'shared/unifont.otf?v=fixture'};
  const placeholderHash=digest(Buffer.from('unselected product seed'));
  for(const [id,product] of Object.entries(PRODUCT_GAMES)){
    const entry=seed.games[id];
    Object.assign(entry,{
      runtime:`runtime/${id}/${id}.html?hosted=1&v=seed`,
      ...(product.multiplayerRuntime?{multiplayerRuntime:`runtime/${id}/multiplayer/${id}.html?hosted=1&v=seed`}:{}),
      gameData:{path:product.package.dataTarget.slice(1),bytes:4,sha256:placeholderHash,
        version:`sha256-${placeholderHash}`,layout:'sha256-'+ '0'.repeat(64)},
      features:{thprac:false,focusHitbox:false},
      music:{midi:{files:[],...(product.musicCapabilities.midi?{}:{supported:false})}},
    });
  }
  seed.games[game].gameData={path:PRODUCT_GAMES[game].package.dataTarget.slice(1),bytes:data.bytes,
    sha256:data.sha256,version:`sha256-${data.sha256}`,layout:dataLayout};
  return seed;
}

export function featureConfig({resourceMode='import',languages={},practiceFeatures={}}={}){
  assert.ok(practiceFeatures&&typeof practiceFeatures==='object'&&!Array.isArray(practiceFeatures),'Practice feature requests must be a game-to-boolean object');
  for(const [game,enabled] of Object.entries(practiceFeatures)){
    assert.ok(Object.hasOwn(PRODUCT_GAMES,game),`Unknown Practice feature product ${game}`);
    assert.equal(typeof enabled,'boolean',`${game}: Practice feature request must be boolean`);
    assert.ok(PRODUCT_GAMES[game].features.thprac,`${game}: product does not declare thprac capability`);
  }
  return{schema:'eagler-touhou/server-features/1',resourceMode,
    games:Object.fromEntries(Object.entries(PRODUCT_GAMES).map(([game,product])=>[game,{
      ...(product.features.languages?{languages:languages[game]??['ja']}:{}),...(product.features.thprac?{thprac:Object.hasOwn(practiceFeatures,game)?practiceFeatures[game]:false}:{})
    }]))};
}
