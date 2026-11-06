import {execFileSync} from 'node:child_process';
import {readFileSync,mkdtempSync,unlinkSync,rmdirSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
const game=resolve(import.meta.dirname,'../web/cpp/game');
const fixture=JSON.parse(readFileSync(new URL('../../../archive/th19-junowen-interop-20261106/native-primitives.json',import.meta.url),'utf8'));
const input=fixture.randomFloats.map(v=>`${v.state} ${v.scale} ${+v.signed} ${v.output.state} ${v.output.bits}`).join('\n')+'\n';
const directory=mkdtempSync(resolve(tmpdir(),'th19-rng-test-'));
const executable=resolve(directory,process.platform==='win32'?'test.exe':'test');
try{
 execFileSync(process.env.CXX||'g++',['-std=c++17','-O2','-UNDEBUG','-ffp-contract=off','-I'+game,resolve(import.meta.dirname,'anm-native-random.cpp'),resolve(game,'AnmVm.cpp'),'-o',executable],{stdio:'inherit',windowsHide:true});
 console.log(execFileSync(executable,[],{input,encoding:'utf8',windowsHide:true}));
}finally{
 try{unlinkSync(executable);}catch(error){if(error.code!=='ENOENT')throw error;}
 rmdirSync(directory);
}
