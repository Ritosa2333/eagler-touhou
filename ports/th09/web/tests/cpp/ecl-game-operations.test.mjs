import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,bits,report} from './helpers.mjs';
const ins=(op,args=[],mask=0,time=0)=>{const b=Buffer.alloc(12+4*args.length);b.writeInt32LE(time);b.writeInt16LE(op,4);b.writeInt16LE(b.length,6);b[9]=255;b.writeUInt16LE(mask,10);args.forEach((a,i)=>b.writeUInt32LE(a>>>0,12+i*4));return b;};
function program(command,character=false){
    const subs=[[command,ins(30,[10008],1,4)],[ins(6,[10000,character?271:135],1),ins(6,[10016,bits(character?.375:.75)],0,7)],[ins(30,[10000],1),ins(53)]];
    const h=Buffer.alloc(8+4*subs.length);h.writeUInt32LE(0x900);h.writeUInt16LE(subs.length,4);let off=h.length;
    const bodies=subs.map((s,i)=>{h.writeUInt32LE(off,8+i*4);const b=Buffer.concat([...s,ins(-1,[],0,0x7fffffff)]);off+=b.length;return b;});return Buffer.concat([h,...bodies]);
}
test('TH09 ECL cross-enemy calls, child creation, cancellation and scene bindings match original boundaries',async()=>{
    const m=await oracle(),c=await core(),manager=m.allocate(0x2ac450),owner=m.allocate(64),other=m.allocate(64),player=m.allocate(0x2000),otherPlayer=m.allocate(0x2000),scene=m.allocate(0x1000),position=m.allocate(12),buffer=c.allocate(65536),request=c.allocate(36);
    const d=new DataView(c.memory.buffer),fields=Array.from({length:c.ecl_var_field_count()},(_,i)=>Array.from({length:4},(_,k)=>d.getUint32(c.ecl_var_fields()+i*16+k*4,true))),extras=Array.from({length:c.ecl_vm_extra_count()},(_,i)=>Array.from({length:3},(_,k)=>d.getUint32(c.ecl_vm_extras()+i*12+k*4,true)));
    const nativeEvents=[],loaded=[];let f=0,current;const heap=m.heap;let checks=0;
    const addr=index=>manager+0x5758+index*0x5430;
    const event=words=>{const b=Buffer.alloc(48);words.forEach((v,i)=>b.writeUInt32LE(v>>>0,i*4));nativeEvents.push(b);};
    const vec=p=>m.readWords(p,3);
    m.replace(0x42c970,'read-ecl',()=>{const p=m.allocate(current.length);m.write(p,current);loaded.push(p);return p;},1);
    m.replace(0x43e380,'sound-event',()=>{event([0,...m.readWords(m.reg('ESP')+4,2)]);return 0;},2);
    for(const [address,velocity] of [[0x40cc00,false],[0x40ccc0,true]])m.replace(address,'effect-event',()=>{const a=m.readWords(m.reg('ESP')+4,velocity?5:4);event([1,a[0],a[velocity?3:2],a[velocity?4:3],Number(velocity),...vec(a[1]),...(velocity?vec(a[2]):[0,0,0])]);return 0;},velocity?5:4);
    m.replace(0x4067d0,'boss-indicator',()=>{const a=m.readWords(m.reg('ESP')+4,2);event([2,a[0],(a[1]<<16)>>16]);return 0;},2);
    m.replace(0x4067f0,'boss-position',()=>{const a=m.readWords(m.reg('ESP')+4,2);event([3,a[0],...vec(a[1])]);return 0;},2);
    m.replace(0x40f560,'release-attached-effects',()=>{const e=m.reg('ECX');event([4,m.bytes(e+0x336b,1)[0],m.u32(e+0x5414)]);m.u32(e+0x5414,0);return 0;});
    m.replace(0x4346a0,'score-popup',()=>{const a=m.readWords(m.reg('ESP')+4,4);event([5,a[2],a[3],...vec(a[1])]);return 0;},4);
    function compare(index,label){
        const enemy=addr(index),bases=[enemy,enemy+0x7f4,manager,player,otherPlayer,0,owner];
        for(let i=0;i<fields.length;++i){const [base,off,,n]=fields[i];assert.deepEqual(memory(c,c.enemy_manager_field(f,index,i),n),m.bytes(bases[base]+off,n),`${label} actor${index} field${base}/${off.toString(16)}`);}
        for(const [off,local,n] of extras)assert.deepEqual(memory(c,c.enemy_manager_actor(f,index)+local,n),m.bytes(enemy+off,n),`${label} actor${index} extra${off.toString(16)}`);
        const character=m.u32(enemy+0x7f4)===manager+0x188;assert.equal(c.enemy_manager_script(f,index,2),m.u32(enemy+0x7f8)-loaded[Number(character)],label+' cursor '+index);
        assert.deepEqual(memory(c,c.enemy_manager_time(f,index),12),m.bytes(enemy+0x7fc,12),label+' time '+index);
        for(let n=0;n<8;++n){const p=m.u32(manager+0x2ac388+4*n);assert.equal(c.enemy_manager_boss(f,n),p?(p-addr(0))/0x5430:-1,label+' boss '+n);}
    }
    function writeField(index,base,offset,value){const field=fields.findIndex(x=>x[0]===base&&x[1]<=offset&&x[1]+x[3]>offset);assert.ok(field>=0);const p=c.enemy_manager_field(f,index,field)+offset-fields[field][1],bases=[addr(index),addr(index)+0x7f4,manager,player,otherPlayer,0,owner];new DataView(c.memory.buffer).setUint32(p,value>>>0,true);m.u32(bases[base]+offset,value);}
    function writeExtra(index,offset,value){const field=extras.find(x=>x[0]===offset);assert.ok(field);const p=c.enemy_manager_actor(f,index)+field[1];const v=new DataView(c.memory.buffer);if(field[2]===2){v.setUint16(p,value&65535,true);m.write(addr(index)+offset,new Uint8Array(new Uint16Array([value]).buffer));}else{v.setUint32(p,value>>>0,true);m.u32(addr(index)+offset,value);}}
    function setup(command){
        if(f)c.enemy_manager_delete(f);f=c.enemy_manager_fixture();m.heap=heap;loaded.length=0;m.call(0x40fa70,{ecx:manager});
        m.u32(manager+0x320,owner);m.u32(manager+0x324,other);m.u32(owner,scene);m.u32(owner+4,player);m.u32(other+4,otherPlayer);m.u32(scene+24,0);m.u32(0x4a8110,0);
        for(let which=0;which<2;++which){current=program(command,Boolean(which));memory(c,buffer,current.length).set(current);assert.equal(c.enemy_manager_load(f,buffer,current.length,which),1);m.call(0x405660,{ecx:manager+0x188*which,args:[0]});}
        for(let i=0;i<fields.length;++i){const [base,off,,n]=fields[i];if(base<2)continue;const bases=[0,0,manager,player,otherPlayer,0,owner];m.write(bases[base]+off,memory(c,c.enemy_manager_field(f,999,i),n));}
        for(let n=0;n<2;++n){const b=Buffer.alloc(36);[n,bits(70+n*80),bits(95.5+n*100),bits(1),90,2,100,0,0].forEach((v,i)=>b.writeUInt32LE(v>>>0,i*4));memory(c,request,36).set(b);m.write(position,b.subarray(4,16));m.f32(0x4b36b8,1);m.f32(0x4a7e7c,1);m.u32(0x4b36d4,0);m.u32(0x4a7eb0,1);assert.equal((m.call(0x40f1d0,{ecx:manager,args:[n,position,90,2,100,0,0]})-addr(0))/0x5430,n);assert.equal(c.enemy_manager_spawn(f,request,0,1),n);}
        c.enemy_manager_set_boss(f,2,1);m.u32(manager+0x2ac390,addr(1));writeField(0,1,0x1c,2);writeField(0,1,0x20,0xaaccddff);writeField(0,5,0x4ace0c,0x758a);
        nativeEvents.length=0;
    }
    const opcodes=[86,87,88,89,93,94,95,124,127,139,140,147,148,163,175];
    try{
        for(const op of opcodes)for(const variable of [false,true])for(const extra of [false,true]){
            let args=[],mask=0;
            if(op===86){args=[10002,variable?10034:385,2];mask=variable?3:1;}
            if(op===87){args=[bits(10016),bits(variable?10033:7.25),2];mask=variable?3:1;}
            if(op===88||op===89){args=[variable?10000:2,op===88?2:7];mask=variable?1:0;}
            if(op===93||op===94){args=[1,...(variable?[10033,10035,10033]:[17.25,-11.5,.75]).map(bits),variable?10032:20,variable?10032:4,variable?10032:320];mask=variable?126:0;}
            if(op===124){args=[variable?10032:18];mask=variable?1:0;}
            if(op===127){args=[extra?-1:variable?10000:2];mask=variable&&!extra?1:0;}
            if(op===139||op===140){args=[variable?10032:14,variable?10032:5,10001,...(variable?[10033,10035,10033]:[.25,-.75,1.125]).map(bits)];mask=variable?63:4;}
            if([147,163,175].includes(op)){args=[variable?10032:739];mask=variable?1:0;}
            setup(ins(op,args,mask,1));
            if(op===127&&extra){c.enemy_manager_set_boss(f,2,0);m.u32(manager+0x2ac390,addr(0));writeField(0,0,0x336b,2);writeExtra(0,0x5414,3);}
            if(op===95){writeExtra(0,0x337c,0xcd);if(extra)writeExtra(1,0x337c,0x4f);else writeExtra(1,0x337c,0xcd);}
            if(extra&&(op===86||op===87))writeField(1,1,0x3c,bits(5.75));
            const expected=m.call(0x4086c0,{args:[addr(0)]})|0,actual=c.enemy_manager_run(f,0,1);assert.equal(actual,expected===0?1:0,'run '+op);
            assert.equal(c.enemy_manager_scene_count(f),nativeEvents.length,'event count '+op);assert.deepEqual(Buffer.from(memory(c,c.enemy_manager_scene_events(f),nativeEvents.length*48)),Buffer.concat(nativeEvents),'scene events '+op);
            for(const [n,p] of [[0,scene+24],[1,0x4a8110],[2,manager+0x2ac3dc],[3,manager+0x2ac440]])assert.equal(c.enemy_manager_scene_value(f,n),m.i32(p),'scene state '+op+'/'+n);
            compare(0,'op '+op);compare(1,'op '+op);if(op===93||op===94)compare(2,'child '+op);
            if(op===88){m.call(0x4086c0,{args:[addr(1)]});assert.equal(c.enemy_manager_run(f,1,1),1);compare(1,'cross call resume');}
            ++checks;
        }
        report('ecl-game-operations',{checks,opcodes,scope:'Full ECL execution and manager child creation; visual/sound/score outputs are recorded at explicit scene boundaries. Full scene rendering and scoring integration remain separate.'});
    }finally{if(f)c.enemy_manager_delete(f);c.release(buffer);c.release(request);m.close();}
});
