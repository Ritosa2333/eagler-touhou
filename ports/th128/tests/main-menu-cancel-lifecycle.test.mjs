import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import vm from 'node:vm';
const source=fs.readFileSync(process.env.TH128_PLAY_SOURCE??new URL('../site/play.mjs',import.meta.url),'utf8');
function production(name){
  const match=new RegExp('^(?:async )?function '+name+'\\(','m').exec(source);
  assert(match);const end=source.indexOf('\n}',match.index)+2;
  return source.slice(match.index,end);
}
function setup(controller,{world=false}={}){
  const pages=[],returns=[];
  const state={mainController:controller,worldMode:world,active:false,paused:true,action:0,pressed:new Set(),autoFlow:true};
  const menu={launching:true,launchId:3,launchPreviousAutoFlow:false,launchReturn:'route'};
  const context={state,menu,status(){},resetHighFrameratePresentation(){},showMenuPage:page=>pages.push(page),
    enterFaultMenu:message=>{throw Error(message);},returnToTitle(){returns.push(true);state.active=true;state.paused=false;}};
  vm.createContext(context);vm.runInContext(['cancelLaunch','backFromLaunch'].map(production).join('\n'),context);
  return{...context,pages,returns};
}
for(const name of ['cancelLaunch','backFromLaunch']){
  for(const controller of [null,{prepared:true,disposed:true}]){
    test(name+' rebuilds title after the native route owner was disposed '+String(controller===null?'missing':'disposed'),()=>{
      const env=setup(controller);if(name==='backFromLaunch')env.menu.launching=false;
      env[name]();
      assert.equal(env.returns.length,1,'a missing title controller must not be recreated on route page2');
      assert.equal(env.pages.length,0);
    });
  }
  test(name+' preserves a live prepared route during canceled preloading',()=>{
    const env=setup({prepared:true,disposed:false});if(name==='backFromLaunch')env.menu.launching=false;
    env[name]();assert.equal(env.returns.length,0);assert.deepEqual(env.pages,['route']);
  });
  test(name+' retains the existing world pause path',()=>{
    const env=setup(null,{world:true});if(name==='backFromLaunch')env.menu.launching=false;
    env[name]();assert.equal(env.returns.length,0);assert.deepEqual(env.pages,['pause']);assert.equal(env.state.paused,true);
  });
}
