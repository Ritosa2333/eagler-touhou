import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createReadStream} from 'node:fs';
import {readFile,stat} from 'node:fs/promises';
import {createRequire,stripTypeScriptTypes} from 'node:module';
import {dirname,isAbsolute,relative,resolve} from 'node:path';

export async function fileIdentity(path){
  const before=await stat(path);assert.ok(before.isFile(),`Input must be a regular file: ${path}`);
  const digest=createHash('sha256');
  for await(const chunk of createReadStream(path))digest.update(chunk);
  const after=await stat(path);
  assert.equal(after.size,before.size,`Input size changed while hashing: ${path}`);
  assert.equal(after.mtimeMs,before.mtimeMs,`Input changed while hashing: ${path}`);
  assert.equal(after.ctimeMs,before.ctimeMs,`Input metadata changed while hashing: ${path}`);
  return{bytes:after.size,sha256:digest.digest('hex')};
}

export function identityDigest(value){
  const canonical=item=>Array.isArray(item)?item.map(canonical):item&&typeof item==='object'?
    Object.fromEntries(Object.keys(item).sort().map(key=>[key,canonical(item[key])])):item;
  return createHash('sha256').update(JSON.stringify(canonical(value))).digest('hex');
}

/** Follow actual JS/TS module imports; unrelated documents never enter a case identity. */
export async function dependencySnapshot(root,entrypoints){
  const require=createRequire(resolve(root,'launcher/package.json')),{parse}=require('acorn');
  const pending=entrypoints.map(path=>resolve(root,path)),files=new Map();
  const inside=path=>{const name=relative(root,path);return name!=='..'&&!name.startsWith('..\\')&&!name.startsWith('../')&&!isAbsolute(name);};
  const find=async candidates=>{for(const candidate of candidates){try{if((await stat(candidate)).isFile())return candidate;}catch(error){if(error.code!=='ENOENT'&&error.code!=='ENOTDIR')throw error;}}return null;};
  while(pending.length){
    const absolute=pending.pop();if(files.has(absolute))continue;
    assert.ok(inside(absolute),`Test dependency leaves repository: ${absolute}`);
    const bytes=await readFile(absolute),name=relative(root,absolute).replaceAll('\\','/');
    files.set(absolute,{path:name,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')});
    if(!/\.(?:[cm]?[jt]sx?)$/.test(absolute))continue;
    const text=/\.[cm]?ts$/.test(absolute)?stripTypeScriptTypes(bytes.toString('utf8'),{mode:'transform',sourceUrl:absolute}):bytes.toString('utf8');
    const source=parse(text,{ecmaVersion:'latest',sourceType:'module',allowHashBang:true}),imports=new Set(),urls=new Set();
    const visit=node=>{
      if(['ImportDeclaration','ExportAllDeclaration','ExportNamedDeclaration'].includes(node.type)&&typeof node.source?.value==='string')imports.add(node.source.value);
      if(node.type==='ImportExpression'&&typeof node.source?.value==='string')imports.add(node.source.value);
      if(node.type==='NewExpression'&&node.callee.name==='URL'&&node.arguments.length===2&&
        typeof node.arguments[0].value==='string'&&node.arguments[1].type==='MemberExpression'&&
        node.arguments[1].object.type==='MetaProperty'&&node.arguments[1].property.name==='url')urls.add(node.arguments[0].value);
      for(const value of Object.values(node)){
        if(Array.isArray(value)){for(const child of value)if(child?.type)visit(child);}
        else if(value?.type)visit(value);
      }
    };
    visit(source);
    for(const specifier of imports){
      if(!specifier.startsWith('.'))continue;
      const base=resolve(dirname(absolute),specifier);
      const found=await find([base,base+'.mjs',base+'.js',base+'.mts',base+'.ts',base+'/index.mjs',
        ...(specifier.endsWith('.js')?[base.slice(0,-3)+'.ts']:[]),
        // Literal imports evaluated inside Launcher page callbacks use its URL root.
        resolve(root,'launcher',specifier)]);
      assert.ok(found,`${name}: unresolved local test dependency ${specifier}`);pending.push(found);
    }
    for(const specifier of urls){
      if(!specifier.startsWith('.'))continue;
      const found=await find([resolve(dirname(absolute),specifier)]);
      if(found&&inside(found))pending.push(found);
    }
  }
  return [...files.values()].sort((a,b)=>a.path.localeCompare(b.path,'en'));
}
