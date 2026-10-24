import {spawnSync} from 'node:child_process';
import {mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';

const root=resolve(import.meta.dirname,'../..'),workspace=resolve(root,'../..');
const temporary=mkdtempSync(resolve(tmpdir(),'th15-frame-schedule-'));
const executable=resolve(temporary,process.platform==='win32'?'frame-schedule.exe':'frame-schedule');
function run(program,args){
 const result=spawnSync(program,args,{cwd:workspace,encoding:'utf8',maxBuffer:8*1024*1024});
 if(result.error)throw result.error;
 if(result.stdout)process.stdout.write(result.stdout);
 if(result.stderr)process.stderr.write(result.stderr);
 if(result.status!==0)throw Error(`${program} exited ${result.status}`);
}
const common=['NetplayCore','NetplayProtocol','NetplaySession','WebSocketTransport','BrowserPeerTransport','SessionChannel','InputReplay'];
try{
 run(process.env.CXX||'g++',['-O2','-std=c++17','-DTH15_MULTIPLAYER=1','-DTH_NATIVE_PLATFORM=1',
  '-I'+resolve(root,'web/cpp/multiplayer'),'-I'+resolve(workspace,'common/include'),resolve(import.meta.dirname,'frame-schedule-test.cpp'),
  ...['NetplayRuntime','SessionSetup','InputLanes'].map(name=>resolve(root,'web/cpp/multiplayer',name+'.cpp')),
  ...common.map(name=>resolve(workspace,'common/src/netplay',name+'.cpp')),'-o',executable]);
 run(executable,[]);
}finally{rmSync(temporary,{recursive:true,force:true});}
