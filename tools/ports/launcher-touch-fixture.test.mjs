import assert from 'node:assert/strict';
import test,{before,after} from 'node:test';
import {createRequire} from 'node:module';
import {mkdtemp,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve,join} from 'node:path';
import {pathToFileURL} from 'node:url';

const root=resolve(import.meta.dirname,'../..');
const require=createRequire(resolve(root,'launcher/package.json'));
const {build}=require('esbuild');
const {JSDOM}=require('jsdom');
import {buildProductionPlayerHostRenderer,productionPlayerStylesheetLinks,readLauncherPublicAsset} from './launcher-player-host-scaffold.mjs';
let tempDir,bundlePath,fixture;
let productionMarkup;

before(async()=>{
  tempDir=await mkdtemp(join(tmpdir(),'launcher-touch-fixture-'));
  bundlePath=join(tempDir,'fixture.mjs');
  await build({entryPoints:[resolve(root,'tools/ports/launcher-touch-fixture.ts')],outfile:bundlePath,
    bundle:true,format:'esm',platform:'node',target:'node22'});
  fixture=await import(pathToFileURL(bundlePath).href);
  const renderer=await buildProductionPlayerHostRenderer({bundlePath:join(tempDir,'player-host.mjs'),builder:build});
  productionMarkup=renderer.render('th08');
});
after(async()=>{await rm(tempDir,{recursive:true,force:true});});

function rect(left,top,width,height){return{left,top,width,height,right:left+width,bottom:top+height,
  toJSON(){return{left:this.left,top:this.top,width:this.width,height:this.height,right:this.right,bottom:this.bottom};}};}

function surface({removeControl=null}={}){
  const dom=new JSDOM(`<!doctype html><html><head>${productionPlayerStylesheetLinks()}</head><body>${productionMarkup}</body></html>`,{url:'https://launcher.test/'});
  const {window}=dom,player=window.document.getElementById('player');
  if(removeControl)window.document.getElementById(removeControl)?.remove();
  const originalRect=window.HTMLElement.prototype.getBoundingClientRect;
  window.HTMLElement.prototype.getBoundingClientRect=function(){
    if(this===player)return rect(20,30,480,320);
    if(this.id==='touchLayoutSafeZone')return rect(28,38,464,304);
    if(this.id==='gameFrame')return rect(20,30,480,320);
    if(this.id==='touchDirectSurface')return rect(20,30,480,320);
    if(this.dataset.touchLayoutControl){
      const index=['focus','fire','bomb','joystick','escape','restart','function','thpracTab','thpracMenu']
        .indexOf(this.dataset.touchLayoutControl);
      return rect(50+(index%4)*80,60+Math.floor(index/4)*60,40,32);
    }
    if(this.dataset.control==='joystick')return rect(30,180,80,80);
    if(this.dataset.control==='directSurface')return rect(20,30,480,320);
    return originalRect.call(this);
  };
  return{dom,window,player,restore(){window.HTMLElement.prototype.getBoundingClientRect=originalRect;dom.window.close();}};
}

function setComputedStyle(window){
  const previous=globalThis.getComputedStyle;
  globalThis.getComputedStyle=window.getComputedStyle.bind(window);
  return()=>{if(previous)globalThis.getComputedStyle=previous;else delete globalThis.getComputedStyle;};
}

function createTouchRecorder(){
  let options;
  const createTouch=value=>{options=value;return{syncControls(){},dispose(){},cancelPointers(){},getSnapshot(){return{};}};};
  return{createTouch,get options(){return options;}};
}

test('fixture binds layout and real touch controller to the SSR production Player and control tree',()=>{
  const host=surface(),restoreStyle=setComputedStyle(host.window),recorder=createTouchRecorder();
  try{
    const receipt=fixture.install({game:'th08',epoch:7},{document:host.window.document,window:host.window,
      location:host.window.location,createTouch:recorder.createTouch});
    const owner=host.window.__acceptanceTouch;
    assert.equal(receipt.layoutTarget,'player');
    assert.equal(receipt.layoutFallbackControls.length,0);
    assert.equal(owner.nodes.directSurface,host.window.document.getElementById('touchDirectSurface'));
    assert.equal(owner.nodes.directSurface.closest('#player'),host.player);
    assert.equal(owner.nodes.fire,host.window.document.getElementById('touchFire'));
    assert.equal(owner.nodes.restart,host.window.document.getElementById('touchRestart'));
    assert.equal(host.window.document.querySelectorAll('#touchFire').length,1);
    assert.equal(recorder.options.elements.fire.closest('#player'),host.player);
    assert.equal(recorder.options.elements.directSurface,owner.nodes.directSurface);

    const tree=owner.layout.begin('landscape');
    assert.equal(tree.target,'player');
    assert.equal(tree.controls.length,9);
    assert.ok(tree.controls.every(control=>control.mounted&&control.owner==='player'));
    const storageKey='eagler-touhou-touch-layout-v1';
    owner.layout.updateControl('fire',{x:.4,y:.6,scale:1.2,priority:1});
    const saved=owner.layout.save();
    assert.equal(saved.persisted,true);
    const applied=owner.layout.apply('landscape','fire');
    assert.equal(applied.target,'player');
    assert.equal(applied.available,true);
    assert.equal(applied.storageKey,storageKey);

    const pointer=new host.window.Event('pointerdown',{bubbles:true});
    Object.assign(pointer,{pointerId:5,pointerType:'touch',clientX:100,clientY:100});
    owner.nodes.directSurface.dispatchEvent(pointer);
    const journal=owner.magnifier.pointerEventsSince(0);
    assert.equal(journal.events.at(-1).surfaceOwner,'player');
    assert.equal(journal.events.at(-1).surfaceTarget,'touchDirectSurface');
    assert.equal(journal.events.at(-1).isTrusted,false,'Node fixture must not claim browser-trusted input');
  }finally{
    try{fixture.dispose(host.window);}finally{restoreStyle();host.restore();}
  }
});

test('fixture fails closed without the launched Player surface',()=>{
  const host=surface(),orphanFrame=host.window.document.createElement('iframe');
  host.window.document.body.append(orphanFrame);host.player.remove();
  try{
    assert.throws(()=>fixture.install({game:'th08',epoch:8},{document:host.window.document,window:host.window,
      location:host.window.location,createTouch:createTouchRecorder().createTouch}),/selected Player surface/);
  }finally{host.restore();}
});

test('fixture requires production controls and disposes listeners without removing Player-owned DOM',()=>{
  const incomplete=surface({removeControl:'touchFocus'});
  try{assert.throws(()=>fixture.install({game:'th08',epoch:8},{document:incomplete.window.document,window:incomplete.window,
    location:incomplete.window.location,createTouch:createTouchRecorder().createTouch}),/Production Player scaffold is missing #touchFocus/);}
  finally{incomplete.restore();}
  const host=surface(),restoreStyle=setComputedStyle(host.window),recorder=createTouchRecorder();
  try{
    fixture.install({game:'th08',epoch:9},{document:host.window.document,window:host.window,
      location:host.window.location,createTouch:recorder.createTouch});
    const owner=host.window.__acceptanceTouch;
    assert.equal(host.player.classList.contains('touch-enabled'),true);
    assert.equal(owner.layoutFallbackNodes.length,0);
    const player=host.player,controls=player.querySelectorAll('[data-touch-layout-control]').length;
    fixture.dispose(host.window);
    assert.equal(host.window.__acceptanceTouch,undefined);
    assert.equal(host.player,player);
    assert.equal(player.querySelectorAll('[data-touch-layout-control]').length,controls);
    assert.equal(host.player.classList.contains('touch-enabled'),false);
  }finally{
    try{fixture.dispose(host.window);}finally{restoreStyle();host.restore();}
  }
});

test('production host CSS routes every real url() asset through the public-file policy',async()=>{
  const css=await readLauncherPublicAsset('/launcher-public/styles.css');
  assert.equal(css?.contentType,'text/css; charset=utf-8');
  const urls=[...css.bytes.toString('utf8').matchAll(/url\(\s*(['"]?)([^)'\"]+)\1\s*\)/g)]
    .map(match=>match[2]).filter(value=>!/^(?:data:|https?:|#)/i.test(value));
  for(const value of urls){
    const path=new URL(value,'https://launcher.test/launcher-public/styles.css').pathname;
    assert.ok(await readLauncherPublicAsset(path),`Production stylesheet asset is not routable: ${path}`);
  }
  assert.match(productionPlayerStylesheetLinks(),/\/launcher-public\/styles\.css/);
  assert.match(productionPlayerStylesheetLinks(),/\/launcher-app\/workspace-viewport\.css/);
  assert.equal(await readLauncherPublicAsset('/launcher-public/%2e%2e/private.json'),null);
});
