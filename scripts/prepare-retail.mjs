// User-invoked local content preparation. The retail executable is read as
// data only. This entry point never imports an Oracle, Unicorn, or a browser.
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const EXE_SHA256='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417';
const usage=`Prepare your own TH12 1.00b content without launching the game.

node scripts/prepare-retail.mjs --original=<installation> --thdat=<thdat executable>

Options (also accept a separate value after the option):
  --original  Installation containing th12.exe, th12.dat, thbgm.dat
              Environment: TH12_ORIGINAL; default: local/original
  --thdat     thtk thdat executable; environment: TH12_THDAT or THDAT
              Default: thdat on PATH
  --python    Python 3 executable; environment: TH12_PYTHON; default: python
  --retail    Extracted archive files; TH12_RETAIL; default: local/retail
  --assets    Prepared loose assets; TH12_ASSETS; default: site/assets
  --output    DATA and music output; TH12_CONTENT; default: artifacts/eagler-content
  --help      Show this message without preparing files

Python packages: Pillow, pefile, imageio-ffmpeg.
Only the pinned unmodified Japanese 1.00b executable is supported.
`;

function argumentsOf(argv){
  const names=new Set(['original','thdat','python','retail','assets','output']);
  const options={};
  for(let index=0;index<argv.length;index++){
    const argument=argv[index];if(argument==='--help'||argument==='-h'){options.help=true;continue;}
    const match=/^--([a-z]+)(?:=(.*))?$/.exec(argument);
    if(!match||!names.has(match[1]))throw Error('Unknown preparation option: '+argument);
    const value=match[2]??argv[++index];if(!value||value.startsWith('--'))throw Error('Missing value for --'+match[1]);
    options[match[1]]=value;
  }
  return options;
}
const inside=(child,parent)=>{const relative=path.relative(parent,child);return relative===''||relative!=='..'&&!relative.startsWith('..'+path.sep)&&!path.isAbsolute(relative);};
// Resolve existing ancestors as well, so output-directory symlinks cannot
// redirect generated files into the original installation.
function canonical(file){
  let ancestor=path.resolve(file);const suffix=[];
  while(!fs.existsSync(ancestor)){const parent=path.dirname(ancestor);if(parent===ancestor)throw Error('Cannot resolve path: '+file);suffix.unshift(path.basename(ancestor));ancestor=parent;}
  return path.join(fs.realpathSync(ancestor),...suffix);
}
function executable(value){return /[\\/]/.test(value)?path.resolve(value):value;}
function run(label,command,args,options={}){
  console.log(label);
  const result=spawnSync(command,args,{cwd:root,stdio:'inherit',windowsHide:true,shell:false,...options});
  if(result.error)throw Error(label+': '+result.error.message);
  if(result.status!==0)throw Error(label+': process exited '+(result.status??result.signal??'without status'));
}
function requireFile(file){if(!fs.existsSync(file)||!fs.statSync(file).isFile())throw Error('Missing required input: '+file);}

function main(){
  const options=argumentsOf(process.argv.slice(2));if(options.help){console.log(usage);return;}
  const original=canonical(options.original??process.env.TH12_ORIGINAL??path.join(root,'local/original'));
  const retail=canonical(options.retail??process.env.TH12_RETAIL??path.join(root,'local/retail'));
  const assets=canonical(options.assets??process.env.TH12_ASSETS??path.join(root,'site/assets'));
  const output=canonical(options.output??process.env.TH12_CONTENT??path.join(root,'artifacts/eagler-content'));
  const thdat=executable(options.thdat??process.env.TH12_THDAT??process.env.THDAT??'thdat');
  const python=executable(options.python??process.env.TH12_PYTHON??'python');
  for(const name of ['th12.exe','th12.dat','thbgm.dat'])requireFile(path.join(original,name));
  if(createHash('sha256').update(fs.readFileSync(path.join(original,'th12.exe'))).digest('hex')!==EXE_SHA256)throw Error('The PE tables require the unmodified TH12 1.00b executable (SHA-256 '+EXE_SHA256+').');
  const destinations=[retail,assets,output];
  for(const destination of destinations){
    if(inside(destination,original)||inside(original,destination)||inside(root,destination))throw Error('Generated directories must be separate from the original installation and source root: '+destination);
    for(const source of ['src','scripts','docs'])if(inside(destination,path.join(root,source)))throw Error('Generated directories cannot be source directories: '+destination);
  }
  for(let left=0;left<destinations.length;left++)for(let right=left+1;right<destinations.length;right++)if(inside(destinations[left],destinations[right])||inside(destinations[right],destinations[left]))throw Error('Retail, assets, and DATA output directories must not overlap.');
  const local=canonical(path.join(root,'local'));if(inside(local,original))throw Error('The local preparation workspace cannot be inside the original installation.');
  const environment={...process.env,TH12_ORIGINAL:original,TH12_NODE:process.execPath};
  run('Checking Python content-preparation dependencies',python,['-c',"import PIL, pefile, imageio_ffmpeg"],{env:environment});
  fs.mkdirSync(local,{recursive:true});
  const work=fs.mkdtempSync(path.join(local,'prepare-retail-'));
  const extracted=path.join(work,'retail'),prepared=path.join(work,'assets'),content=path.join(work,'content');
  fs.mkdirSync(extracted);fs.mkdirSync(prepared);
  environment.TH12_RETAIL=extracted;environment.TH12_ASSETS=prepared;environment.TH12_ARTWORK=path.join(content,'artwork');
  try{
    // thtk's documented syntax is `thdat -x version archive`; -x12 selects
    // TH12. Extraction targets the child process's current directory.
    run('Extracting th12.dat with thtk',thdat,['-x12',path.join(original,'th12.dat')],{cwd:extracted,env:environment});
    // thdat may continue after an entry error; each preparation step also
    // requires its complete input set before DATA can be published.
    for(const name of ['default.ecl','pl00.anm','title.anm','musiccmt.txt','thbgm.fmt','demo0.rpy'])requireFile(path.join(extracted,name));
    for(const script of ['prepare-assets.py','prepare-presentation.py','prepare-sounds.py','prepare-music.py','prepare-menu-assets.py','prepare-host-artwork.py'])run('Preparing '+script,python,[path.join(root,'scripts',script)],{env:environment});
    run('Building local managed DATA',process.execPath,[path.join(root,'scripts/prepare-managed-content.mjs'),'--assets='+prepared,'--output='+content],{env:environment});
    // Copies only merge generated files; no existing user directory is
    // removed. DATA is built from the fresh staging directory, so stale loose
    // files in a previous destination cannot enter the new package.
    for(const [source,destination] of [[extracted,retail],[prepared,assets],[content,output]]){fs.mkdirSync(destination,{recursive:true});fs.cpSync(source,destination,{recursive:true,force:true});}
    fs.copyFileSync(path.join(root,'local/content-identity.json'),path.join(output,'source-identity.json'));
    console.log(JSON.stringify({originalVersion:'1.00b',retail,assets,content:output,data:path.join(output,'th12.data'),artwork:path.join(output,'artwork/th12-card.webp'),sourceIdentity:path.join(output,'source-identity.json'),nativeCodeExecuted:false}));
    // Only this freshly allocated staging tree is removed. Failed runs leave
    // their staging tree intact for diagnosis.
    const resolvedWork=canonical(work);
    if(resolvedWork===work&&inside(resolvedWork,local)&&resolvedWork!==local&&path.basename(resolvedWork).startsWith('prepare-retail-'))fs.rmSync(resolvedWork,{recursive:true,force:true});
  }catch(error){console.error('Preparation stopped; staging files retained at '+work);throw error;}
}
try{main();}catch(error){console.error(error.message);process.exitCode=1;}
