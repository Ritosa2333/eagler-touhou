import test from'node:test';import assert from'node:assert/strict';
import{Element}from'../practice-menu/surface.mjs';import{mountThprac128Menu}from'../../site/runtime/thprac128-menu.mjs';
class Select extends Element{constructor(){super('select');}get value(){return this.selected??'';}set value(v){this.selected=(this.children??[]).some(n=>n.value===String(v))?String(v):'';}}
test('empty native selects do not discard the initial practice mode and Normal difficulty',()=>{
 const root=new Element('section'),opener=new Element('button'),nodes={};
 for(const key of ['mode','startPoint','path','stage','difficulty','warp','section','phase','locale','score','motivation','perfect_freeze','ice_power','ice_area','enabled','dlg','start','restart','cancel','status','replace','replaceRow'])nodes[key]=['mode','startPoint','path','stage','difficulty','warp','section','phase','locale'].includes(key)?new Select():new Element();
 root.querySelector=s=>nodes[s.match(/data-thprac="([^"]+)"/)?.[1]]??null;root.querySelectorAll=()=>[];
 const menu=mountThprac128Menu({root,opener,getAvailability:()=>({ready:true,busy:false,inRun:false}),launch:async()=>{throw Error('constructor must not launch');}});
 assert.equal(nodes.mode.value,'1','practice default lost while native select had no options');assert.equal(nodes.difficulty.value,'1','Normal default lost while native select had no options');assert.equal(nodes.warp.disabled,false);assert.equal(root.hidden,true);menu.dispose();
});
