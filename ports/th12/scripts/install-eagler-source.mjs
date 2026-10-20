// Materialize code in the monorepo's declared source owner. Never copy ignored
// retail files, build output, .git metadata or a developer checkout wholesale.
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(import.meta.dirname,'..');
const value=process.argv.find(argument=>argument.startsWith('--workspace='))?.slice(12);
if(!value)throw Error('Usage: node scripts/install-eagler-source.mjs --workspace=EAGLER_TOUHOU');
const workspace=path.resolve(value),target=path.join(workspace,'ports/th12'),marker=path.join(target,'eagler-source.json');
if(!fs.existsSync(path.join(workspace,'launcher/config/workspace.json')))throw Error('Not an Eagler Touhou monorepo');
if(fs.existsSync(target)&&!fs.existsSync(marker)&&fs.readdirSync(target).length)throw Error('ports/th12已有独立源码，请先合并已有改动');
const git=(...args)=>{const result=spawnSync('git',['-c','safe.directory='+root.replaceAll('\\','/'),...args],{cwd:root,encoding:'utf8',windowsHide:true});if(result.error)throw result.error;if(result.status)throw Error(result.stderr);return result.stdout;};
const names=[...new Set(git('ls-files','--cached','--others','--exclude-standard','-z').split('\0').filter(Boolean))].sort();
const previous=fs.existsSync(marker)?JSON.parse(fs.readFileSync(marker,'utf8')):null,identities={},plan=[];
if(previous&&(previous.schema!=='th12-eagler-source/1'||previous.repository!=='https://github.com/Tweet-META/th12'))throw Error('ports/th12不属于此源码同步入口');
for(const name of Object.keys(previous?.files??{}))if(!names.includes(name))throw Error('源码已移除文件，请先处理旧的 ports/th12/'+name);
for(const name of names){
  if(name.startsWith('/')||name.includes('\\')||name.split('/').some(part=>!part||part==='.'||part==='..'||part==='.git'))throw Error('Invalid source path: '+name);
  const source=path.join(root,name),info=fs.lstatSync(source);if(!info.isFile()||info.isSymbolicLink())throw Error('Unsupported source entry: '+name);
  const bytes=fs.readFileSync(source),destination=path.join(target,name),sha256=createHash('sha256').update(bytes).digest('hex');
  if(fs.existsSync(destination)){
    const current=createHash('sha256').update(fs.readFileSync(destination)).digest('hex');
    if(current!==sha256&&current!==previous?.files?.[name]?.sha256)throw Error('目标源码有独立改动，请先合并：'+destination);
  }
  identities[name]={bytes:bytes.length,sha256};plan.push({destination,bytes});
}
for(const {destination,bytes}of plan){fs.mkdirSync(path.dirname(destination),{recursive:true});fs.writeFileSync(destination,bytes);}
fs.writeFileSync(marker,JSON.stringify({schema:'th12-eagler-source/1',repository:'https://github.com/Tweet-META/th12',branch:git('branch','--show-current').trim()||null,baseCommit:git('rev-parse','HEAD').trim(),files:identities},null,2)+'\n');
console.log(JSON.stringify({sourceOwner:target,files:names.length}));
