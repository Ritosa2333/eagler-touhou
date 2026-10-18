import {spawnSync} from 'node:child_process';
import {mkdirSync,readFileSync,writeFileSync,existsSync,readdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve,relative} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

const root=fileURLToPath(new URL('../../',import.meta.url));
const sdk=process.env.WASI_SDK_BIN;
if(!sdk)throw Error('Set WASI_SDK_BIN to a WASI SDK bin directory');
const compiler=resolve(sdk,process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error('WASI compiler not found');
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const wasm=resolve(out,'versus-rules.wasm');
const sources=[resolve(root,'tests/multiplayer/versus-rules.cpp'),
  ...['ShotControl','PlayerLife','PlayerHazards','AttackAreas','PlayerCollision',
    'PlayerItems','MatchRules','AttackController','MatchScene','StageSelection','Rng','Timer','GameMath']
    .map(name=>resolve(root,'cpp/game',name+'.cpp'))];
const flags=['-std=c++17','-O2','-ffp-contract=off','-fno-exceptions','-fno-rtti',
  '-Wno-invalid-offsetof','-Wl,-z,stack-size=1048576'];
const built=spawnSync(compiler,[...flags,...sources,'-o',wasm],{encoding:'utf8'});
if(built.error)throw built.error;
if(built.status!==0)throw Error(built.stdout+built.stderr);
const bytes=readFileSync(wasm),wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:wasi.wasiImport});
if(wasi.start(instance))throw Error('TH09 versus applicability gate failed');
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const headers=directory=>readdirSync(directory,{withFileTypes:true}).flatMap(entry=>
  entry.isDirectory()?headers(resolve(directory,entry.name)):
    /\.(hpp|h|inc)$/.test(entry.name)?[resolve(directory,entry.name)]:[]);
const inputs=[...sources,...headers(resolve(root,'cpp/game')),
  resolve(root,'tests/cpp/attack-controller-fixture.hpp'),resolve(root,'tests/cpp/match-scene-fixture.hpp')];
writeFileSync(resolve(out,'versus-rules-report.json'),JSON.stringify({passed:true,
  scope:'Asset-free native-rule regression; no original EXE oracle, full-world replay, browser, network or device acceptance',
  wasmSha256:sha(bytes),flags,
  compiler:spawnSync(compiler,['--version'],{encoding:'utf8'}).stdout.trim(),
  sources:Object.fromEntries(inputs.map(path=>[relative(root,path),sha(readFileSync(path))])),
  scenarios:['Quick-charge thresholds and gates','Fatal hit, repeated collision and native recovery',
    'Existing Tewi automatic defence','Symmetric field-local rewards and all four item kinds',
    'Opponent boss slot and native initial HP','Versus rounds and match-complete transition']},null,2)+'\n');
