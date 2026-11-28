export class Surface {
 constructor(name='div'){this.nodeName=name.toUpperCase();this.children=[];this.listeners=new Map();this.attributes=new Map();this.dataset={};this.style={setProperty(name,value){this[name]=value;},removeProperty(name){delete this[name];}};this.classList={values:new Set(),add(...names){names.forEach(n=>this.values.add(n));},remove(...names){names.forEach(n=>this.values.delete(n));},toggle(name,force){const add=force??!this.values.has(name);if(add)this.values.add(name);else this.values.delete(name);return add;},contains(name){return this.values.has(name);}};this.parentElement=null;this.hidden=false;this.capture=new Set();}
 appendChild(node){node.parentElement=this;this.children.push(node);return node;}
 append(...nodes){for(const node of nodes)this.appendChild(node);}
 setAttribute(name,value){this.attributes.set(name,String(value));}
 getAttribute(name){return this.attributes.get(name)??null;}
 addEventListener(name,fn){if(!this.listeners.has(name))this.listeners.set(name,new Set());this.listeners.get(name).add(fn);}
 removeEventListener(name,fn){this.listeners.get(name)?.delete(fn);}
 dispatchEvent(event){if(!event.target)event.target=this;event.currentTarget=this;for(const fn of [...(this.listeners.get(event.type)??[])])fn(event);return !event.defaultPrevented;}
 setPointerCapture(id){this.capture.add(id);}
 hasPointerCapture(id){return this.capture.has(id);}
 releasePointerCapture(id){if(this.capture.delete(id))this.dispatchEvent({type:'lostpointercapture',pointerId:id});}
 getBoundingClientRect(){return this.rect??{left:0,top:0,width:1280,height:960};}
 focus(){}
 closest(selector){return selector==='.screen'?this.parentElement:null;}
 remove(){if(this.parentElement)this.parentElement.children=this.parentElement.children.filter(n=>n!==this);this.parentElement=null;}
 get listenerCount(){return [...this.listeners.values()].reduce((a,x)=>a+x.size,0);}
}
export function environment(){
 const window=new Surface('window'),document=new Surface('document'),container=new Surface('section'),canvas=new Surface('canvas');
 window.innerWidth=1280;window.innerHeight=960;
 document.documentElement=new Surface('html');
 document.hidden=false;document.createElement=name=>new Surface(name);document.defaultView=window;canvas.ownerDocument=document;container.appendChild(canvas);
 window.KeyboardEvent=class {constructor(type,init){this.type=type;Object.assign(this,init);this.defaultPrevented=false;}preventDefault(){this.defaultPrevented=true;}};
 const emit=(target,type,id=1,x=80,y=850,pointerType='touch')=>{const event={type,pointerId:id,clientX:x,clientY:y,pointerType,button:0,preventDefault(){this.defaultPrevented=true;}};target.dispatchEvent(event);return event;};
 const key=(type,code)=>window.dispatchEvent(new window.KeyboardEvent(type,{code,cancelable:true,bubbles:true}));
 const find=(node,attribute,value)=>node.getAttribute(attribute)===value?node:node.children.map(child=>find(child,attribute,value)).find(Boolean);
 return {window,document,container,canvas,emit,key,find};
}
