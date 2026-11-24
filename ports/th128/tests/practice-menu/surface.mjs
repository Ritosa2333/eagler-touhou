// Purpose-built DOM boundary. No browser, graphics or game World.
export class Element{
 constructor(tag='div'){this.tagName=tag.toUpperCase();this.hidden=false;this.disabled=false;this.value='';this.checked=false;this.textContent='';this.attributes={};this.listeners=new Map();this.children=[];this.focused=false;this.ownerDocument={createElement:tag=>new Element(tag)};}
 addEventListener(type,fn){if(!this.listeners.has(type))this.listeners.set(type,new Set());this.listeners.get(type).add(fn);}
 removeEventListener(type,fn){this.listeners.get(type)?.delete(fn);}
 setAttribute(k,v){this.attributes[k]=String(v);}getAttribute(k){return this.attributes[k]??null;}
 replaceChildren(...xs){this.children=xs;if(this.tagName==='SELECT')this.value=xs[0]?.value??'';}
 focus(){this.focused=true;}
 emit(type,extra={}){const e={type,target:this,preventDefault(){this.defaultPrevented=true;},stopPropagation(){this.propagationStopped=true;},...extra};for(const fn of [...this.listeners.get(type)??[]])fn(e);return e;}
}
export function surface(){const root=new Element('section'),opener=new Element('button'),nodes={select:new Element('select'),start:new Element('button'),restart:new Element('button'),cancel:new Element('button'),status:new Element('p'),replace:new Element('input'),replaceRow:new Element('label')};root.hidden=true;root.querySelector=s=>nodes[s.match(/data-practice="([^"]+)"/)?.[1]]??null;return{root,opener,nodes};}
export const flush=()=>new Promise(r=>setImmediate(r));
export function deferred(){let resolve,reject;const promise=new Promise((a,b)=>{resolve=a;reject=b;});return{promise,resolve,reject};}
