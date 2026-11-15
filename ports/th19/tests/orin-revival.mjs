import {execFileSync} from 'node:child_process';
import {mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve,join} from 'node:path';
const root=resolve(import.meta.dirname,'..'),game=resolve(root,'web/cpp/game');
const temporary=mkdtempSync(join(tmpdir(),'th19-orin-revival-'));
try{
 const executable=resolve(temporary,process.platform==='win32'?'check.exe':'check');
 execFileSync(process.env.CXX||'g++',['-std=c++17','-O1','-UNDEBUG','-I'+game,resolve(import.meta.dirname,'orin-revival.cpp'),...['AnmResource','AnmVm','EclProgram','EclVm','GameObject','GameWorld'].map(name=>resolve(game,name+'.cpp')),'-o',executable],{stdio:'inherit',windowsHide:true});
 execFileSync(executable,process.argv.slice(2),{stdio:'inherit',windowsHide:true});
}finally{rmSync(temporary,{recursive:true,force:true});}

