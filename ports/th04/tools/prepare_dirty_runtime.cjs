const fs=require('node:fs'),path=require('node:path'),{spawnSync}=require('node:child_process');
const acorn=require('../build/acorn/package/dist/acorn.js');
const root=path.resolve(__dirname,'..'),out=path.join(root,process.env.DIRTY_RUNTIME_DIR||'build/core-dirty-runtime');fs.mkdirSync(out,{recursive:true});
let source=fs.readFileSync(path.join(root,process.env.DIRTY_SOURCE||'web/vendor/dosbox-x/dosbox-rollback.mjs'),'utf8');
const ast=acorn.parse(source,{ecmaVersion:'latest',sourceType:'module'}),edits=[],stats={indices:0,set:0};
const heapName=node=>node?.type==='Identifier'&&/^HEAP(?:U?\d+|F\d+)$/.test(node.name)?node.name:null;
const width=name=>Number(name.match(/\d+/)[0])/8;
let factory;const search=[ast];while(search.length){const node=search.pop();if(node.type==='FunctionExpression'&&node.params?.some(p=>p.type==='AssignmentPattern'&&p.left.name==='moduleArg')){factory=node;break;}for(const value of Object.values(node)){if(Array.isArray(value))for(const child of value){if(child&&typeof child.type==='string')search.push(child);}else if(value&&typeof value.type==='string')search.push(value);}}
if(!factory)throw Error('module factory scope missing');const stack=[factory];
while(stack.length){const node=stack.pop();
 const left=node.type==='AssignmentExpression'?node.left:node.type==='UpdateExpression'?node.argument:null;
 if(left?.type==='MemberExpression'&&left.computed){const name=heapName(left.object);if(name){edits.push({start:left.property.start,end:left.property.end,text:`thDirtyIndex((${source.slice(left.property.start,left.property.end)}),${width(name)})`});stats.indices++;}}
 if(left?.type==='MemberExpression'&&left.computed&&!heapName(left.object)&&left.object.type==='Identifier'){
  edits.push({start:left.property.start,end:left.property.end,text:`thDirtyAliasIndex(${left.object.name},(${source.slice(left.property.start,left.property.end)}))`});stats.indices++;
 }
 if(node.type==='CallExpression'&&node.callee.type==='MemberExpression'){
  const name=heapName(node.callee.object)|| (node.callee.object.type==='Identifier'?node.callee.object.name:null),method=node.callee.property.name;
  if(name&&['set','fill','copyWithin'].includes(method)&&node.arguments.length){
   edits.push({start:node.callee.start,end:node.callee.end,text:method==='set'?'thDirtySet':method==='fill'?'thDirtyFill':'thDirtyCopyWithin'});edits.push({start:node.arguments[0].start,end:node.arguments[0].start,text:name+','});stats.set++;
  }
 }
 for(const [key,value]of Object.entries(node)){if(key==='start'||key==='end')continue;if(Array.isArray(value))for(const child of value){if(child&&typeof child.type==='string')stack.push(child);}else if(value&&typeof value.type==='string')stack.push(value);}
}
edits.sort((a,b)=>b.start-a.start||b.end-a.end);let boundary=source.length;
for(const edit of edits){if(edit.end>boundary)throw Error('overlapping heap mutations');source=source.slice(0,edit.start)+edit.text+source.slice(edit.end);boundary=edit.start;}
const pages=JSON.parse(fs.readFileSync(path.join(root,process.env.DIRTY_PAGES||'build/core-dirty-pages.json')));
source=source.replace('var Module=moduleArg;',`var Module=moduleArg;
Module.thDirty=new WebAssembly.Memory({initial:1,maximum:1});
Module.thDirtyStatic=${JSON.stringify(pages.staticPages)};
const thDirtyBytes=new Uint8Array(Module.thDirty.buffer);
Module.thDirtyMark=(at,length)=>{if(!length)return;const first=at>>>12,last=(at+length-1)>>>12;thDirtyBytes.fill(1,first,last+1);};
function thDirtyIndex(index,bytes){Module.thDirtyMark(index*bytes,bytes);return index;}
function thDirtyAliasIndex(heap,index){if(ArrayBuffer.isView(heap)&&heap.buffer===wasmMemory.buffer)Module.thDirtyMark(heap.byteOffset+index*heap.BYTES_PER_ELEMENT,heap.BYTES_PER_ELEMENT);return index;}
function thDirtySet(heap,...args){if(ArrayBuffer.isView(heap)&&heap.buffer===wasmMemory.buffer)Module.thDirtyMark(heap.byteOffset+(args[1]??0)*heap.BYTES_PER_ELEMENT,args[0].length*heap.BYTES_PER_ELEMENT);return heap.set(...args);}
function thDirtyFill(heap,...args){if(ArrayBuffer.isView(heap)&&heap.buffer===wasmMemory.buffer)Module.thDirtyMark(heap.byteOffset,heap.byteLength);return heap.fill(...args);}
function thDirtyCopyWithin(heap,...args){if(ArrayBuffer.isView(heap)&&heap.buffer===wasmMemory.buffer)Module.thDirtyMark(heap.byteOffset,heap.byteLength);return heap.copyWithin(...args);}
`);
source=source.replace('var wasmImports={','var wasmImports={thDirty:Module.thDirty,');
fs.writeFileSync(path.join(out,'dosbox-rollback.mjs'),source);
let helper=fs.readFileSync(path.join(root,'tools/snapshot-diff.wat'),'utf8');
helper=helper.replace('(import "env" "memory" (memory $live 1))','(import "env" "memory" (memory $live 1))\n (import "env" "thDirty" (memory $dirty 1))');
const start=helper.indexOf(' (func (export "scan")'),end=helper.indexOf(' (func (export "copyPage")',start);
helper=helper.slice(0,start)+` (func (export "scan") (param $pages i32) (result i32) (local $page i32) (local $count i32)
 (block $done (loop $next
  (br_if $done (i32.ge_u (local.get $page) (local.get $pages)))
  (if (i32.load8_u $dirty (local.get $page)) (then
   (if (i32.eqz (call $equal (local.get $page))) (then
    (i32.store $shadow (i32.shl (local.get $count) (i32.const 2)) (local.get $page))
    (local.set $count (i32.add (local.get $count) (i32.const 1)))))
   (i32.store8 $dirty (local.get $page) (i32.const 0))))
  (local.set $page (i32.add (local.get $page) (i32.const 1))) (br $next))) (local.get $count))
`+helper.slice(end);
fs.writeFileSync(path.join(out,'snapshot-diff.wat'),helper);
const assembler=path.join(root,'build/binaryen-native-132/binaryen-version_132/bin/wasm-as.exe');
const built=spawnSync(assembler,[path.join(out,'snapshot-diff.wat'),'--enable-multimemory','--enable-simd','--enable-bulk-memory','-o',path.join(out,'snapshot-diff.wasm')],{encoding:'utf8'});if(built.status!==0)throw Error(built.stderr);
let diff=fs.readFileSync(path.join(root,'web/dosbox-memory-diff.js'),'utf8');
diff=diff.replace('{env:{memory:module.nativeExports().memory}}','{env:{memory:module.nativeExports().memory,thDirty:module.thDirty}}');
diff=diff.replace('changes(){const count=','changes(){for(const page of module.thDirtyStatic)new Uint8Array(module.thDirty.buffer)[page]=1;const count=');
fs.writeFileSync(path.join(out,'dosbox-memory-diff.js'),diff);
let runtime=fs.readFileSync(path.join(root,'web/netplay/runtime.js'),'utf8');
if(!runtime.includes('thDirtyMark'))runtime=runtime.replace('function stepInner(inputs){','function stepInner(inputs){\n if(mailbox>=0)emulator.module.thDirtyMark?.(guestBase(),1048576);');
fs.writeFileSync(path.join(out,'runtime.js'),runtime);
fs.copyFileSync(path.join(root,'web/dosbox-snapshots.js'),path.join(out,'dosbox-snapshots.js'));
console.log('Prepared isolated dirty runtime',stats);
