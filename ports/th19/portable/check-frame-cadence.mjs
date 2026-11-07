import {execFileSync} from 'node:child_process';
import {mkdtempSync,rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve,join} from 'node:path';
const temp=mkdtempSync(join(tmpdir(),'th19-clock-'));
try{
 const output=resolve(temp,process.platform==='win32'?'clock.exe':'clock');
 execFileSync(process.env.CXX||'g++',['-std=c++17','-O2','-UNDEBUG',resolve(import.meta.dirname,'check-frame-cadence.cpp'),'-o',output],{stdio:'inherit',windowsHide:true});
 execFileSync(output,[],{stdio:'inherit',windowsHide:true});
 console.log(JSON.stringify({passed:true,game:'th19',logicHz:60,refreshHz:[15,20,30,50,59,60,75,90,120,144,165,240],pauseClearsDebt:true}));
}finally{rmSync(temp,{recursive:true,force:true});}
