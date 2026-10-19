import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {existsSync,readFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..'),out=resolve(root,'web/artifacts/touch-shield');
mkdirSync(out,{recursive:true});const temp=resolve(out,'temp');mkdirSync(temp,{recursive:true});
const sdk=process.env.TH15_EMSDK??resolve(root,'../../th08/tools/emsdk');
const emcc=['install','upstream'].map(p=>resolve(sdk,p,'emscripten/emcc.py')).find(existsSync);assert(emcc,'Set TH15_EMSDK');
const result=spawnSync('python',[emcc,'-O2','-std=c++17','-sDEFAULT_TO_CXX=1','--no-entry','-sSTANDALONE_WASM=1',resolve(import.meta.dirname,'touch-shield-check.cpp'),'-o',resolve(out,'verify.wasm')],{env:{...process.env,EM_CONFIG:resolve(sdk,'.emscripten'),TEMP:temp,TMP:temp},encoding:'utf8',windowsHide:true});
if(result.error||result.status)throw result.error??Error(result.stdout+result.stderr);
const wasi=new WASI({version:'preview1'}),{instance}=await WebAssembly.instantiate(readFileSync(resolve(out,'verify.wasm')),{wasi_snapshot_preview1:wasi.wasiImport});wasi.initialize(instance);assert.equal(instance.exports.verify(),0);
// The touch adapter must select all owners from the same local multiplayer
// seat, including guests whose character differs from the host.
const source=readFileSync(resolve(root,'web/cpp/sdl/BrowserRuntime.cpp'),'utf8');
assert(source.includes('const auto& local=*stage.battle.pilots[network.Setup().local_player]'));
assert(source.includes('pilot=local.player;character=local.character;bomb=local.bomb;session=&local.session;'));
assert(source.includes('||mp_playback||network.Spectator()'));
assert(source.includes('character==3&&player.life.state==4&&session->bomb_state==1&&bomb'));
console.log('PASS: three shield layers, death/release/pause cancellation, local-seat owner and spectator guards (no device/netplay parity claim)');
