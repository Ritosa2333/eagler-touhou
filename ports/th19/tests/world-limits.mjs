import {execFileSync} from 'node:child_process';
import {mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve,join} from 'node:path';
const root=resolve(import.meta.dirname,'..'),game=resolve(root,'web/cpp/game');
const temporary=mkdtempSync(join(tmpdir(),'th19-world-limits-'));
try{
 const executable=resolve(temporary,process.platform==='win32'?'check.exe':'check');
 // Retail uses SSE f32 operations; MinGW32 otherwise retains x87 intermediates.
 const floatFlags=process.platform==='win32'&&['x64','ia32'].includes(process.arch)?['-msse2','-mfpmath=sse']:[];
 execFileSync(process.env.CXX||'g++',['-std=c++17','-O1',...floatFlags,'-UNDEBUG','-I'+game,resolve(import.meta.dirname,'world-limits.cpp'),...['AnmResource','AnmVm','EclProgram','EclVm','GameObject','GameWorld'].map(name=>resolve(game,name+'.cpp')),'-o',executable],{stdio:'inherit',windowsHide:true});
 execFileSync(executable,[],{stdio:'inherit',windowsHide:true});
}finally{rmSync(temporary,{recursive:true,force:true});}
