// Native identity-table regression. This is not a Wasm/gameplay/FPS test.
import {spawnSync} from 'node:child_process';
import {mkdtempSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import os from 'node:os';
const root=resolve(import.meta.dirname,'../..');
const artifacts=resolve(root,'web/artifacts/presentation-identity');mkdirSync(artifacts,{recursive:true});
const out=mkdtempSync(resolve(artifacts,'native-'));
const compiler=process.env.CXX||(process.platform==='win32'?'g++':'c++');
const source=resolve(import.meta.dirname,'presentation-identity-test.cpp');
const header=resolve(root,'web/cpp/sdl/FrameIdentityTable.hpp');
const binary=resolve(out,process.platform==='win32'?'identity-test.exe':'identity-test');
const hash=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const receipt={schema:'th15-presentation-identity-native/1',startedAt:new Date().toISOString(),
 scope:{native:true,gpu:false,gameplay:false,wasm:false},
 platform:process.platform,arch:process.arch,cpu:os.cpus()[0]?.model,
 sources:{[source]:hash(source),[header]:hash(header)},commands:[],passed:false};
function run(name,command,args){
 const p=spawnSync(command,args,{cwd:root,encoding:'utf8',timeout:60000,maxBuffer:4*1024*1024,windowsHide:true});
 writeFileSync(resolve(out,name+'.log'),(p.stdout||'')+(p.stderr||''));
 receipt.commands.push({name,command,args,exitCode:p.status,signal:p.signal,error:p.error?String(p.error):null});
 if(p.error||p.status!==0)throw Error(name+' failed: '+(p.error||p.stderr||p.stdout));
 return p.stdout;
}
try{
 receipt.compiler=run('version',compiler,['--version']).split('\n')[0];
 run('compile',compiler,['-std=c++17','-O2','-ffp-contract=off','-fno-exceptions','-fno-rtti',source,'-o',binary]);
 const output=run('test',binary,[]);process.stdout.write(output);
 const line=output.split('\n').find(s=>s.startsWith('FRAME_IDENTITY_TABLE '));
 if(!line)throw Error('Missing actual regression result');
 receipt.result=JSON.parse(line.slice('FRAME_IDENTITY_TABLE '.length));
 if(receipt.result.checks<4000000)throw Error('Incomplete identity-table regression');
 receipt.binarySha256=hash(binary);receipt.passed=true;
}catch(error){receipt.failure=String(error);process.stderr.write(String(error)+'\n');process.exitCode=1;}
finally{receipt.completedAt=new Date().toISOString();writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');console.log('Native identity evidence: '+out);}
