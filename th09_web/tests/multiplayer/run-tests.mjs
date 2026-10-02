import {spawnSync} from 'node:child_process';
import {mkdirSync,readFileSync,writeFileSync,existsSync,readdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

const root=fileURLToPath(new URL('../../',import.meta.url));
const common=resolve(root,'../third_party/eagler-common');
const sdk=process.env.WASI_SDK_BIN;
if(!sdk)throw Error('Set WASI_SDK_BIN to the workspace WASI SDK bin directory');
const compiler=resolve(sdk,process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error('WASI compiler not found');
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const flags=['-std=c++17','-O2','-g','-ffp-contract=off','-fno-exceptions',
  '-I'+resolve(common,'include'),'-I'+resolve(root,'cpp/multiplayer'),
];
const sha=b=>createHash('sha256').update(b).digest('hex');
const headers=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?headers(resolve(dir,e.name)):
  /\.(hpp|h|inc)$/.test(e.name)?[resolve(dir,e.name)]:[]);
const headerInputs=[...headers(resolve(root,'cpp/multiplayer')),...headers(resolve(root,'cpp/game')),...headers(resolve(common,'include'))];
const headerHashes=Object.fromEntries(headerInputs.map(p=>[p,sha(readFileSync(p))]));
function compile(args){
  const result=spawnSync(compiler,args,{encoding:'utf8',windowsHide:true});
  if(result.error)throw result.error;
  if(result.status!==0)throw Error(result.stdout+result.stderr);
}
const suites={
  'frame-schedule':[],
  session:[resolve(root,'cpp/multiplayer/RollbackSession.cpp'),
    ...['NetplayCore','NetplayProtocol','NetplaySession','SessionChannel'].map(n=>resolve(common,'src/netplay',n+'.cpp'))],
  adonis:[resolve(root,'cpp/multiplayer/RollbackSession.cpp'),
    ...['NetplayCore','NetplayProtocol','NetplaySession','SessionChannel'].map(n=>resolve(common,'src/netplay',n+'.cpp'))],
  'dynamic-state':[resolve(root,'cpp/multiplayer/DynamicState.cpp')],
  'bullet-snapshot':[
    ...['BulletManager','BulletExtras','BulletPattern','Rng','Timer','GameMath'].map(n=>resolve(root,'cpp/game',n+'.cpp')),
    resolve(common,'src/netplay/RollbackJournal.cpp')],
};
for(const [suite,implementation] of Object.entries(suites)){
  const wasm=resolve(out,suite+'.wasm');
  const sources=[resolve(root,'tests/multiplayer',suite+'.cpp'),...implementation];
  // RTTI is a test oracle for dynamic-type preservation only. Production
  // implementations and all attack constructors are checked without it below.
  compile([...flags,...(suite==='session'?['-fno-rtti']:[]),'-Wl,-z,stack-size=1048576',...sources,'-o',wasm]);
  const bytes=readFileSync(wasm),wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
  const {instance}=await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:wasi.wasiImport});
  const exit=wasi.start(instance);
  if(exit)throw Error('TH09 rollback '+suite+' gate failed: '+exit);
  writeFileSync(resolve(out,suite+'-report.json'),JSON.stringify({passed:true,
    scope:'TH09 rollback component gate; not whole-game, browser, phone or performance acceptance',
    wasmSha256:sha(bytes),compiler:spawnSync(compiler,['--version'],{encoding:'utf8',windowsHide:true}).stdout.trim(),
    flags,headers:headerHashes,sources:Object.fromEntries(sources.map(p=>[p,sha(readFileSync(p))]))},null,2)+'\n');
}
compile([...flags,'-fno-rtti','-fsyntax-only',resolve(root,'cpp/multiplayer/DynamicState.cpp'),
  ...['CharacterAttacks','FieldAttacks','MystiaAttacks','AttackQueue'].map(n=>resolve(root,'cpp/game',n+'.cpp'))]);
console.log('Production attack constructors and checkpoint adapters compile without RTTI: PASS');
