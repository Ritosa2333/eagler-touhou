import {spawnSync} from 'node:child_process';
import {mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';

const temporary=mkdtempSync(resolve(tmpdir(),'th15-presentation-pool-'));
const executable=resolve(temporary,process.platform==='win32'?'presentation-pool.exe':'presentation-pool');
function run(program,args){
 const result=spawnSync(program,args,{encoding:'utf8',maxBuffer:1024*1024});
 if(result.error)throw result.error;
 if(result.stdout)process.stdout.write(result.stdout);
 if(result.stderr)process.stderr.write(result.stderr);
 if(result.status!==0)throw Error(`${program} exited ${result.status}`);
}
try{
 run(process.env.CXX||'g++',['-O2','-std=c++17',resolve(import.meta.dirname,'presentation-pool-test.cpp'),'-o',executable]);
 run(executable,[]);
}finally{rmSync(temporary,{recursive:true,force:true});}
