import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,bits,report} from './helpers.mjs';
const ins=(op,args=[],mask=0,time=0)=>{const b=Buffer.alloc(12+4*args.length);b.writeInt32LE(time);b.writeInt16LE(op,4);b.writeInt16LE(b.length,6);b[9]=255;b.writeUInt16LE(mask,10);args.forEach((a,i)=>b.writeUInt32LE(a>>>0,12+i*4));return b;};
function program(character){
    const subs=[[ins(65,[bits(.25),bits(1.75)]),ins(6,[10000,character?92:41],1)],[ins(131,[character?79:63]),ins(144,[5,2])],[ins(1)],[ins(30,[10000],1),ins(52,[4])],[ins(30,[10001],1),ins(53)]];
    const h=Buffer.alloc(8+4*subs.length);h.writeUInt32LE(0x900);h.writeUInt16LE(subs.length,4);let off=h.length;
    const bodies=subs.map((s,i)=>{h.writeUInt32LE(off,8+i*4);const b=Buffer.concat([...s,ins(-1,[],0,0x7fffffff)]);off+=b.length;return b;});return Buffer.concat([h,...bodies]);
}
test('Enemy manager template and both creation paths match original, including immediate scripts and pool exhaustion',async()=>{
    const m=await oracle(),c=await core(),f=c.enemy_manager_fixture(),manager=m.allocate(0x2ac450),owner=m.allocate(64),other=m.allocate(64),player=m.allocate(0x2000),otherPlayer=m.allocate(0x2000),position=m.allocate(12),locals=m.allocate(120),buffer=c.allocate(65536),request=c.allocate(36),inherited=c.allocate(120);
    const d=new DataView(c.memory.buffer),fields=Array.from({length:c.ecl_var_field_count()},(_,i)=>Array.from({length:4},(_,k)=>d.getUint32(c.ecl_var_fields()+i*16+k*4,true))),extras=Array.from({length:c.ecl_vm_extra_count()},(_,i)=>Array.from({length:3},(_,k)=>d.getUint32(c.ecl_vm_extras()+i*12+k*4,true)));
    const sources=[program(false),program(true)],loaded=[];let current;let checks=0;
    m.replace(0x42c970,'ecl-resource',()=>{const q=m.allocate(current.length);m.write(q,current);loaded.push(q);return q;},1);
    m.call(0x40fa70,{ecx:manager});m.u32(manager+0x320,owner);m.u32(manager+0x324,other);m.u32(owner+4,player);m.u32(other+4,otherPlayer);
    const addr=index=>index===999?manager+0x328:manager+0x5758+index*0x5430;
    function compare(index,label,context=true){
        const enemy=addr(index),bases=[enemy,enemy+0x7f4,manager,player,otherPlayer,0,owner];
        for(let i=0;i<fields.length;++i){const [base,offset,,size]=fields[i];if(!context&&base===1)continue;assert.deepEqual(memory(c,c.enemy_manager_field(f,index,i),size),m.bytes(bases[base]+offset,size),`${label} field ${base}/${offset.toString(16)}`);}
        for(const [offset,local,size] of extras)assert.deepEqual(memory(c,c.enemy_manager_actor(f,index)+local,size),m.bytes(enemy+offset,size),`${label} extra ${offset.toString(16)}`);
        if(context){
            assert.deepEqual(memory(c,c.enemy_manager_time(f,index),12),m.bytes(enemy+0x7fc,12),label+' primary timer');
            const character=m.u32(enemy+0x7f4)===manager+0x188,sub=new DataView(m.bytes(enemy+0x7f4+0x228,2).buffer).getInt16(0,true);
            assert.equal(c.enemy_manager_script(f,index,0),sub,label+' sub');assert.equal(c.enemy_manager_script(f,index,1),Number(character),label+' program');
            assert.equal(c.enemy_manager_script(f,index,2),m.u32(enemy+0x7f8)-loaded[Number(character)],label+' cursor');
        }
    }
    function active(index,value){c.enemy_manager_active(f,index,value);m.u32(addr(index)+0x337c,(m.u32(addr(index)+0x337c)&~1)|(value&1));}
    function spawn(sub,life,score,mirror,character,inherit,rate=1,clear=true){
        if(clear)active(0,0);const bytes=Buffer.alloc(36);[sub,bits(105.75),bits(81.125),bits(.25),life,0xf9,score,mirror,character].forEach((v,i)=>bytes.writeUInt32LE(v>>>0,4*i));memory(c,request,36).set(bytes);m.write(position,bytes.subarray(4,16));
        m.f32(0x4b36b8,rate);m.f32(0x4a7e7c,1);m.u32(0x4b36d4,0);m.u32(0x4a7eb0,1);
        const expected=m.call(inherit?0x40f340:0x40f1d0,{ecx:manager,args:[sub,position,life,0xf9,score,inherit?locals:mirror,character],limit:20000000});
        const actual=c.enemy_manager_spawn(f,request,inherit?inherited:0,rate),index=(expected-(manager+0x5758))/0x5430;
        assert.equal(actual,index,'allocated index');assert.equal(c.enemy_manager_failed(f),m.u32(manager+0x2ac42c),'allocation result');
        if(index<128)compare(index,`creation sub${sub} life${life} score${score} mirror${mirror} char${character} inherit${inherit} rate${rate}`);++checks;return index;
    }
    try{
        for(let which=0;which<2;++which){current=sources[which];memory(c,buffer,current.length).set(current);assert.equal(c.enemy_manager_load(f,buffer,current.length,which),1);assert.equal(m.call(0x405660,{ecx:manager+which*0x188,args:[0]}),0);}
        // Seed external state from the authored fixture; the template itself is
        // compared to the untouched original initializer before any creation.
        for(let i=0;i<fields.length;++i){const [base,off,,n]=fields[i];if(base<2)continue;const bases=[0,0,manager,player,otherPlayer,0,owner];m.write(bases[base]+off,memory(c,c.enemy_manager_field(f,999,i),n));}
        compare(999,'template',false);
        const initial=Buffer.alloc(120);for(let i=0;i<30;++i)initial.writeInt32LE(i*7+1,i*4);m.write(locals,initial);memory(c,inherited,120).set(initial);
        for(const rate of [1,.5])for(let sub=0;sub<4;++sub)for(const life of [-1,0,37])for(const score of [-1,120])for(const mirror of [0,1,2])for(const character of [0,1])for(const inherit of [0,1])spawn(sub,life,score,mirror,character,inherit,rate);
        for(let i=0;i<128;++i)active(i,0);for(let i=0;i<130;++i)assert.equal(spawn(0,50,100,0,0,0,1,false),Math.min(i,128));
        active(17,0);assert.equal(spawn(1,-1,-1,1,1,1,1,false),17);
        report('enemy-manager-create',{checks,capacity:128,creationPaths:2,scope:'Original manager initializer and complete allocation/initial-script execution with inherited locals, both programs, failure and exhaustion. This does not yet cover the full enemy frame callback.'});
    }finally{c.enemy_manager_delete(f);c.release(buffer);c.release(request);c.release(inherited);m.close();}
});
