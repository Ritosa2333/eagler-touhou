import {spawnSync} from 'node:child_process';
import {mkdtempSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';

// This checks the real protocol/frontier adapter without a browser or SDK.
// The complete game-memory restoration path has a separate Wasm test.
const root=resolve(import.meta.dirname,'../..');
const workspace=resolve(root,'../..');
const out=mkdtempSync(resolve(tmpdir(),'th15-network-'));
const executable=resolve(out,process.platform==='win32'?'network.exe':'network');
const run=(program,args)=>{
 const result=spawnSync(program,args,{cwd:workspace,encoding:'utf8',maxBuffer:8*1024*1024});
 if(result.error)throw result.error;
 if(result.stdout)process.stdout.write(result.stdout);
 if(result.stderr)process.stderr.write(result.stderr);
 if(result.status!==0)throw Error(`${program} exited ${result.status}`);
};
const common=['NetplayCore','NetplayProtocol','NetplaySession','WebSocketTransport','BrowserPeerTransport','SessionChannel','InputReplay'];
const title=['NetplayRuntime','SessionSetup','InputLanes','ReplayArchive'];
try {
 run(process.env.CXX||'g++',['-O2','-std=c++17','-DTH15_MULTIPLAYER=1','-DTH_NATIVE_PLATFORM=1',
  '-I'+resolve(root,'web/cpp/multiplayer'),'-I'+resolve(workspace,'common/include'),
  resolve(root,'portable/multiplayer/network-protocol.cpp'),
  ...title.map(name=>resolve(root,'web/cpp/multiplayer',name+'.cpp')),
  ...common.map(name=>resolve(workspace,'common/src/netplay',name+'.cpp')),'-o',executable]);
 run(executable,[]);
 console.log('PASS TH15 2P/3P correction restart, immutable capture, confirmed publication fence, mode-0/mode-2 Replay');
} finally {rmSync(out,{recursive:true,force:true});}
