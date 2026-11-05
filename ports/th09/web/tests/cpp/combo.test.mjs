import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('TH09 combo rewards preserve score thresholds, attack meters, transfer randomness and chain timers',async()=>{
    const m=await oracle(),c=await core(),f=c.combo_fixture(),combo=m.allocate(0x44),player=m.allocate(0x30500),owner=m.allocate(64),other=m.allocate(64),scene=m.allocate(0x12000),character=m.allocate(0x200),transfers=m.allocate(4096*0xd8),pos=m.allocate(12),p=c.allocate(12);
    m.u32(combo,player);m.u32(player+12,owner);m.u32(player+0x10,other);m.u32(player+0x30338,character);m.u32(0x4a7e38,scene);
    const position=Buffer.alloc(12);[71.375,95.625,.5].forEach((v,i)=>position.writeFloatLE(v,i*4));m.write(pos,position);memory(c,p,12).set(position);
    const events=[];let nextTransfer=0,hasBoss=false,checks=0,totalTransfers=0;
    const record=words=>{const b=Buffer.alloc(48);words.forEach((v,i)=>b.writeUInt32LE(v>>>0,i*4));events.push(b);};const vec=q=>m.readWords(q,3);
    m.replace(0x403e50,'combo-attack',()=>{const a=m.readWords(m.reg('ESP')+4,4);record([0,a[0],a[1]]);return 0;},4);
    m.replace(0x40f7d0,'opposing-boss-query',()=>hasBoss?0x123456:0,1);
    m.replace(0x40f860,'combo-score',()=>{record([1,m.u32(m.reg('ESP')+4)]);return 0;},1);
    m.replace(0x4346a0,'combo-popup',()=>{const a=m.readWords(m.reg('ESP')+4,4);record([2,...vec(a[1]),a[2],a[3]]);return 0;},4);
    const callback=m.registerImport({dll:'fixture',name:'character-attack-meter',handler:()=>{record([3,...m.readWords(combo+0x2c,3)]);return 0;},argc:0});m.u32(combo+0x40,callback);
    m.replace(0x40ccc0,'combo-transfer',()=>{const a=m.readWords(m.reg('ESP')+4,5);record([4,a[0],...vec(a[1]),...vec(a[2])]);const q=transfers+nextTransfer++*0xd8;m.view(q,0xd8).fill(0);return q;},5);
    const otherManager=m.allocate(0x2ac450);m.u32(other+0x10,otherManager);
    function compare(label){
        assert.deepEqual(memory(c,c.combo_part(f,0),0x3c),m.bytes(combo+4,0x3c),label+' combo state');assert.deepEqual(memory(c,c.combo_part(f,1),8),m.bytes(0x4ace0c,8),label+' RNG');
        assert.equal(c.combo_event_count(f),events.length,label+' events');assert.deepEqual(Buffer.from(memory(c,c.combo_events(f),events.length*48)),Buffer.concat(events),label+' event parameters');
        assert.equal(c.combo_transfer_count(f),nextTransfer,label+' transfers');for(let n=0;n<nextTransfer;++n)assert.deepEqual(memory(c,c.combo_transfer(f,n),10),m.bytes(transfers+n*0xd8+0xa0,10),label+' transfer '+n);
    }
    try{
        const samples=[];
        for(const rank of [0,1,8,18,24])for(let difficulty=0;difficulty<5;++difficulty)for(const side of [0,1])for(const rate of [1,.5,.99])for(const blocked of [false,true])samples.push({rank,difficulty,side,rate,blocked,kind:0});
        for(const old of [99980,100000,299970,300000,499980,500000,999980,999990])for(const boss of [false,true])for(const hits of [0,1,8,9,14,29,39])samples.push({rank:8,difficulty:1,side:0,rate:1,blocked:false,kind:1,old,boss,hits});
        for(const [index,s] of samples.entries()){
            events.length=0;nextTransfer=0;hasBoss=s.boss??(index%3===0);const spirits=index%4===0?25:24,seed=(index*343+15)&65535,flags=index%9===0?32:0;
            c.combo_setup(f,s.rank,s.difficulty,s.side,Number(s.blocked),Number(hasBoss),spirits,s.rate,flags,seed);
            const initial=Buffer.alloc(0x3c),hits=s.hits??index%42,score=s.old??index*140,normalStart=index%119,spiritStart=index%59;
            initial.writeInt32LE(hits,0);initial.writeInt32LE(hits+3,4);initial.writeInt32LE(score,8);initial.writeInt32LE(index%3===0?score+100:score,12);
            initial.writeInt32LE(3,16);initial.writeFloatLE(18.75,20);initial.writeInt32LE(18,24);
            initial.writeInt32LE(4,28);initial.writeFloatLE(index%63+.375,32);initial.writeInt32LE(index%63,36);
            initial.writeInt32LE(normalStart,40);initial.writeInt32LE(spiritStart,44);initial.writeInt32LE(7,48);initial.writeInt32LE(index%53,52);initial.writeInt32LE(17,56);
            memory(c,c.combo_part(f,0),initial.length).set(initial);m.write(combo+4,initial);
            m.u32(scene+0x11ea8,Number(s.blocked));m.u32(player+8,s.side);m.u32(otherManager+0x2ac3b8,spirits);m.u32(0x4a7e44,s.rank);m.u32(0x4a7eac,s.difficulty);m.u32(0x4ace0c,seed);m.u32(0x4ace10,0);
            for(let side=0;side<2;++side){m.u32(0x4b3178+side*0xf0+0xcc,32+side*304);m.u32(0x4b3178+side*0xf0+0xd0,16);}
            m.u32(0x4b3448,0x4b3178+s.side*0xf0);m.f32(0x4a80e0,17.25);m.f32(0x4a80e4,13.125);m.f32(0x4a80e8,384);m.f32(0x4b36b8,s.rate);m.u32(0x4b36d4,flags);
            const normal=s.kind?0:index%3===0?0:30+(index%5)*47,spirit=s.kind?0:4+(index%4)*37,attack=index%21,amount=s.kind?101:10+(index%9)*613,kill=index%2;
            m.call(kill?0x40f8b0:0x41d150,{ecx:combo,args:[pos,normal,spirit,attack,amount],limit:1000000});assert.equal(c.combo_add(f,p,normal,spirit,attack,amount,kill),1,'combo add '+index);compare('case '+index);++checks;totalTransfers+=nextTransfer;
            for(const flush of [false,true]){events.length=0;m.call(flush?0x41d7e0:0x40f8c0,{ecx:combo});c.combo_reset(f,Number(flush));compare('reset '+index+'/'+flush);++checks;}
        }
        report('combo',{checks,samples:samples.length,transfers:totalTransfers,scope:'Complete original combo reward/reset/flush paths; attack and effect creation captured at game-manager boundaries, with final transfer parameters and RNG compared.'});
    }finally{c.combo_delete(f);c.release(p);m.close();}
});
