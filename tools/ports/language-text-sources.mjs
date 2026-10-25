import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileIdentity} from '../testing/dependency-snapshot.mjs';

export function languageSourceFnv(bytes){let value=14695981039346656037n;for(const byte of bytes)value=BigInt.asUintN(64,(value^BigInt(byte))*1099511628211n);return value.toString(16).padStart(16,'0');}

/** Extract original source bytes once for every language context, independently of the running native loader. */
export async function prepareLanguageTextSources({root,output,target,adapter,dataIdentity,packs={}}){
  const declaration=adapter.language.consumers?.dialogue;if(!declaration)return null;
  assert.equal(declaration.archiveKind,'pbgz');const directory=resolve(output,'original-text-sources');await mkdir(directory,{recursive:false});
  const tool=resolve(root,'launcher/scripts/touhou_formats.py'),files={};
  assert.deepEqual(await fileIdentity(target.data),dataIdentity);
  for(const asset of declaration.assets){
    assert.match(asset,/^[A-Za-z0-9_.-]+$/);const path=resolve(directory,asset);
    execFileSync('python',[tool,'extract-pbgz-entry','--archive',target.data,'--entry',asset,'--output',path],{cwd:root,windowsHide:true,maxBuffer:1024*1024});
    const bytes=await readFile(path);files[asset]={kind:'original-data',path,...await fileIdentity(path),fnv64:languageSourceFnv(bytes)};
  }
  assert.deepEqual(await fileIdentity(target.data),dataIdentity);
  const translations=await prepareLanguagePackTextSources({output,declaration,packs});
  return{schema:'eagler-touhou/language-text-sources/1',game:target.game,variant:target.variant,archive:{path:target.data,...dataIdentity},
    tool:{path:tool,...await fileIdentity(tool)},files,translations};
}

/** Copy independently decoded pack members once, retaining their exact mounted names. */
export async function prepareLanguagePackTextSources({output,declaration,packs}){
  assert.match(declaration.mountRoot,/^\/[A-Za-z0-9_/-]+\/$/);assert.ok(!declaration.mountRoot.includes('..'));
  const translations={};
  for(const [language,pack]of Object.entries(packs)){
    assert.match(language,/^[A-Za-z0-9_-]+$/);const files={};
    for(const asset of declaration.assets){
      assert.match(asset,/^[A-Za-z0-9_.-]+$/);const member=declaration.mountRoot+asset,matches=pack.files.filter(file=>file.path===member);
      assert.ok(matches.length<=1,'Duplicate translated source member');if(!matches.length)continue;
      const bytes=Buffer.from(matches[0].bytes);assert.ok(bytes.length>0&&bytes.length<=16*1024*1024);
      const directory=resolve(output,'translated-text-sources',language);await mkdir(directory,{recursive:true});
      const path=resolve(directory,asset);await writeFile(path,bytes,{flag:'wx'});
      files[asset]={kind:'language-pack',member,path,...await fileIdentity(path),fnv64:languageSourceFnv(bytes)};
    }
    translations[language]={files};
  }
  return translations;
}

export function resolveLanguageTextSource(sources,{language,original,asset}){
  const translated=original?null:sources.translations?.[language]?.files?.[asset];
  const selected=translated??sources.files[asset];assert.ok(selected,'Declared source asset is unavailable');return selected;
}
