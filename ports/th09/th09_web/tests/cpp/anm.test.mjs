import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,bits,report} from './helpers.mjs';
const view=(c,p,n)=>new Uint8Array(c.memory.buffer,p,n);
function normalize(bytes,{file,script,sprites}){
    const copy=bytes.slice(),v=new DataView(copy.buffer,copy.byteOffset,copy.byteLength);
    for(const [offset,base] of [[0x204,file],[0x21c,script],[0x220,script],[0x224,sprites],[0x234,script]]){const p=v.getUint32(offset,true);v.setUint32(offset,p?p-base+1:0,true);}return copy;
}
function differences(a,b){const av=new DataView(a.buffer,a.byteOffset),bv=new DataView(b.buffer,b.byteOffset),out=[];for(let i=0;i<a.length;i+=4)if(av.getUint32(i,true)!==bv.getUint32(i,true))out.push({offset:'0x'+i.toString(16),cpp:'0x'+av.getUint32(i,true).toString(16),original:'0x'+bv.getUint32(i,true).toString(16),cppFloat:av.getFloat32(i,true),originalFloat:bv.getFloat32(i,true)});return out.slice(0,12);}
function program(op){
    const commands=[];let offset=0;
    const push=(code,args=[],time=0,mask=0)=>{const instruction={code,args,time,mask,offset};commands.push(instruction);offset+=8+args.length*4;return instruction;};
    push(3,[0]);for(let i=0;i<4;i++){push(37,[10000+i,3+i],0,1);push(38,[bits(10004+i),bits((i+1)*0.25)],0,1);}
    const f=bits;let args=[],mask=0;
    const table={3:[2],4:[0,3],5:[10000,0,3],6:[f(10.25),f(-20.5),f(1.5)],7:[f(1.25),f(.75)],8:[191],9:[77,111,155],12:[f(.5),f(-.25),f(1.5)],13:[f(.04),f(-.12),f(.07)],14:[f(.01),f(-.005)],15:[42,13],16:[1],17:[f(10.25),f(-20.5),f(1.5),19],18:[f(10.25),f(-20.5),f(1.5),19],19:[f(10.25),f(-20.5),f(1.5),19],21:[12],24:[1],25:[5],26:[f(-.25)],27:[f(1.125)],28:[1],29:[f(2.5),f(.5),17],30:[1],31:[1],32:[17,2,f(10.25),f(-20.5),f(1.5)],33:[13,4,55,123,200],34:[9,3,10],35:[15,5,f(.15),f(-1.2),f(1.75)],36:[17,6,f(2.5),f(.5)],79:[5],80:[f(.03)],81:[f(-.025)],82:[2],83:[3],84:[15,120,210],85:[35],86:[15,2,77,12,231],87:[19,6,15],88:[0x100]};
    if(op in table)args=table[op];
    if(op===5)mask=1;
    if(op>=37&&op<=58){const float=(op%2)===0;args=[float?f(10004):10000,float?f(1.25):7];if(op>=49)args.push(float?f(.75):3);mask=1;}
    if(op===59){args=[10000,1234];mask=1;}
    if(op>=60&&op<=65){args=[f(10004),f(op===64?-.5:1.125)];mask=1;}
    if(op===66){args=[f(10004)];mask=1;}
    if(op>=67&&op<=78){args=op%2?[10000,3,0,3]:[f(10004),f(.25),0,3];mask=1;}
    const target=push(op,args,1,mask);push(8,[231],2);const landing=push(3,[1],3);
    if(op===4)target.args[0]=landing.offset;if(op===5)target.args[1]=landing.offset;if(op>=67&&op<=78)target.args[2]=landing.offset;
    push(2,[],45);
    push(21,[7],0);push(33,[5,1,111,77,42],0);push(89,[],4);
    push(21,[-1],0);push(84,[24,48,96],0);push(89,[],3);push(-1,[],0);
    const bytes=new Uint8Array(offset),v=new DataView(bytes.buffer);
    for(const command of commands){const p=command.offset;v.setInt16(p,command.code,true);v.setUint16(p+2,8+command.args.length*4,true);v.setInt16(p+4,command.time,true);v.setUint16(p+6,command.mask,true);command.args.forEach((value,i)=>v.setUint32(p+8+i*4,value>>>0,true));}return bytes;
}
test('C++ ANM instructions, interpolation and interrupts match full original VM state',async()=>{
    const m=await oracle(),c=await core(),f=c.anm_create(),p=c.allocate(8192),vm=m.allocate(0x2a4),manager=m.allocate(64),file=m.allocate(28),sprites=m.allocate(16*68),script=m.allocate(8192);let checks=0;
    const originalBases={file,script,sprites};
    m.u32(0x4dc550,manager);m.write(sprites,view(c,c.anm_sprites(f),16*68));m.u32(file,7);m.u32(file+4,sprites);m.u32(file+12,sprites);
    try{for(const [precision,cw,rate] of [[32,0x7f,1],[32,0x7f,.5]])for(let op=0;op<89;op++){
        const bytes=program(op);view(c,p,bytes.length).set(bytes);m.write(script,bytes);m.view(manager,64).fill(0);m.u32(0x4ace0c,0x1234);m.u32(0x4ace10,0);m.f32(0x4b36b8,rate);m.u32(0x4b36d4,0);m.reg('FPCW',cw);
        m.call(0x403a10,{ecx:vm});c.anm_start(f,p,bytes.length,rate,0);m.call(0x4395a0,{ecx:file,args:[vm,script]});
        const cppBases={file:c.anm_file(f),script:c.anm_script(f),sprites:c.anm_sprites(f)};
        const compare=frame=>{assert.equal(c.anm_invalid(f),0);const actual=normalize(view(c,c.anm_vm(f),0x2a4),cppBases),expected=normalize(m.bytes(vm,0x2a4),originalBases),diff=differences(actual,expected);assert.equal(diff.length,0,`${precision}/${rate} op=${op} frame=${frame} ${JSON.stringify(diff)}`);assert.deepEqual(view(c,c.anm_rng(f),8),m.bytes(0x4ace0c,8));assert.equal(c.anm_count(f),m.u32(manager+16));assert.deepEqual(view(c,c.anm_script(f),bytes.length),m.bytes(script,bytes.length));checks++;};
        compare(-1);
        for(let frame=0;frame<70;frame++){const interrupt=frame===8?7:frame===22?19:0;if(interrupt)m.view(vm+0x1fe,2).set(new Uint8Array(new Int16Array([interrupt]).buffer));const actual=c.anm_step(f,interrupt),expected=m.call(0x436f30,{ecx:manager,args:[vm]});assert.equal(actual,expected);compare(frame);}
    }report('anm-synthetic',{passed:true,checks,opcodes:90,precision:[32],rates:[1,.5],comparison:'all 676 VM bytes, RNG, modified script and execution count'});}finally{c.anm_delete(f);c.release(p);m.close();}
});
