import {spawnSync} from 'node:child_process';
import {mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';

// Execute the actual native request owner. Network/frontier integration is
// covered by the production browser run, not by a second simulated save owner.
const root=resolve(import.meta.dirname,'../..'),workspace=resolve(root,'../..');
const temporary=mkdtempSync(resolve(tmpdir(),'th15-pending-save-'));
const executable=resolve(temporary,process.platform==='win32'?'pending-save.exe':'pending-save');
function run(program,args){
  const result=spawnSync(program,args,{cwd:workspace,encoding:'utf8',maxBuffer:8*1024*1024});
  if(result.error)throw result.error;
  if(result.stdout)process.stdout.write(result.stdout);
  if(result.stderr)process.stderr.write(result.stderr);
  if(result.status!==0)throw Error(`${program} exited ${result.status}`);
}
try{
  run(process.env.CXX||'g++',['-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-I'+resolve(root,'web/cpp/multiplayer'),resolve(import.meta.dirname,'pending-save-test.cpp'),'-o',executable]);
  run(executable,[]);
}finally{rmSync(temporary,{recursive:true,force:true});}
