import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
test('test shelf has five standalone links between solo and multiplayer, with native activation',async()=>{
 const env=installMountedDom(),React=await import('react'),{createRoot}=await import('react-dom/client');
 const work=await mkdtemp(resolve('.cache/classics-test-')),out=resolve(work,'fixture.mjs');let root;
 try{
  await build({stdin:{contents:`export {LibrarySurface} from './app/components/launcher/LibrarySurface.tsx';export {LocaleProvider} from './app/i18n.tsx';`,resolveDir:process.cwd(),loader:'tsx'},outfile:out,bundle:true,platform:'node',format:'esm',packages:'external',jsx:'automatic',logLevel:'silent'});
  const {LibrarySurface,LocaleProvider}=await import(pathToFileURL(out));const holder=document.createElement('div');document.body.append(holder);root=createRoot(holder);
  const props={products:[],onSelect:()=>assert.fail('managed select'),onActivate:()=>assert.fail('managed activate'),masthead:null,footer:null,lobbyHref:'/lobby',roomUsersArtwork:'/users.svg',optionsOpen:false,onBack(){},interactive:false};
  await React.act(()=>root.render(React.createElement(LocaleProvider,{locale:'en'},React.createElement(LibrarySurface,props))));
  assert.equal(holder.querySelector('.classics-shelf'),null);
  await React.act(()=>root.render(React.createElement(LocaleProvider,{locale:'en'},React.createElement(LibrarySurface,{...props,classicsAssetUrl:path=>new URL(path,'https://example.invalid/').href}))));
  const sections=[...holder.querySelector('.game-library').children];assert.deepEqual(sections.map(e=>e.dataset.shelf),['singleplayer','classics','multiplayer']);
  const links=[...holder.querySelectorAll('.classics-shelf a.game')];assert.equal(links.length,5);
  assert.deepEqual(links.map(a=>a.href),['01','02','03','04','05'].map(id=>`https://example.invalid/th${id}`));
  let prevented;holder.addEventListener('click',e=>{prevented=e.defaultPrevented;e.preventDefault();},{once:true});
  await React.act(()=>links[0].dispatchEvent(new MouseEvent('click',{bubbles:true,cancelable:true})));assert.equal(prevented,false);
  assert.equal(holder.querySelectorAll('.classics-shelf button').length,5);
 }finally{if(root)await React.act(()=>root.unmount());env.close();await rm(work,{recursive:true,force:true});}
});
