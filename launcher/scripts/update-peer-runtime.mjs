// Update an existing Runtime's shared browser bridge without changing its WASM.
// The four bindings keep the native signatures and come from the canonical C++.
import {createHash} from 'node:crypto';
import {readFileSync,writeFileSync} from 'node:fs';
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const require=createRequire(new URL('../package.json',import.meta.url));
const {parse,parseExpressionAt}=require('acorn');
const sourcePath=fileURLToPath(new URL('../../common/src/netplay/BrowserPeerTransport.cpp',import.meta.url));
const names=['eagler_peer_connect','eagler_peer_send','eagler_peer_send_to','eagler_peer_send_repair_to'];
const digest=bytes=>createHash('sha256').update(bytes).digest('hex');
const parser={ecmaVersion:'latest',sourceType:'module'};
function bindings(source){
  return names.map(name=>{
    const match=new RegExp(`EM_JS\\([^,]+,\\s*${name},\\s*\\(([^)]*)\\),\\s*(\\{)`).exec(source);
    if(!match)throw Error('Shared binding missing: '+name);
    const params=match[1].split(',').filter(s=>s.trim()).map(s=>{
      const result=/([A-Za-z_]\w*)\s*$/.exec(s);if(!result)throw Error('Invalid native parameter');return result[1];});
    const begin=match.index+match[0].length-1;
    const prefix=`function ${name}(${params.join(',')})`;
    const text=prefix+source.slice(begin);
    const node=parseExpressionAt(text,0,parser);
    return {name,params,text:text.slice(0,node.end)};
  });
}
export function updatePeerRuntime(input,source=readFileSync(sourcePath,'utf8')){
  const replacements=bindings(source),found=new Map();
  function visit(node){
    if(!node||typeof node!=='object')return;
    if(node.type==='FunctionDeclaration'&&names.includes(node.id?.name)){
      if(found.has(node.id.name))throw Error('Ambiguous binding: '+node.id.name);
      found.set(node.id.name,node);
    }
    for(const [key,value] of Object.entries(node)){
      if(['start','end','loc'].includes(key))continue;
      if(Array.isArray(value))value.forEach(visit);else if(value&&typeof value==='object')visit(value);
    }
  }
  visit(parse(input,parser));
  const edits=replacements.map(binding=>{
    const before=found.get(binding.name);
    if(!before)throw Error('Runtime binding missing: '+binding.name);
    if(before.params.length!==binding.params.length||before.params.some(p=>p.type!=='Identifier'))
      throw Error('Native ABI mismatch: '+binding.name);
    return {...binding,start:before.start,end:before.end,beforeSha256:digest(input.slice(before.start,before.end))};
  }).sort((a,b)=>b.start-a.start);
  let output=input;
  for(const edit of edits)output=output.slice(0,edit.start)+edit.text+output.slice(edit.end);
  parse(output,parser);
  return {output,receipt:{schema:'eagler-touhou/browser-peer-update/1',sourceSha256:digest(source),
    inputSha256:digest(input),outputSha256:digest(output),
    bindings:edits.map(({name,params,beforeSha256,text})=>({name,parameters:params.length,beforeSha256,afterSha256:digest(text)}))}};
}
if(process.argv[1]&&resolve(process.argv[1])===fileURLToPath(import.meta.url)){
  const args=new Map(process.argv.slice(2).map(arg=>{
    const match=/^--(input|output|receipt)=(.+)$/.exec(arg);if(!match)throw Error('Expected --input, --output and --receipt');
    return [match[1],resolve(match[2])];}));
  if(args.size!==3||args.get('input')===args.get('output'))throw Error('Use distinct input and output files');
  const {output,receipt}=updatePeerRuntime(readFileSync(args.get('input'),'utf8'));
  writeFileSync(args.get('output'),output,{flag:'wx'});
  writeFileSync(args.get('receipt'),JSON.stringify(receipt,null,2)+'\n',{flag:'wx'});
  console.log(JSON.stringify(receipt));
}
