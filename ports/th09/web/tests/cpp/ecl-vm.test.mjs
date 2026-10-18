import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,bits,report} from './helpers.mjs';
function instruction(op,args=[],mask=0,time=0,difficulty=255){
    const b=Buffer.alloc(12+args.length*4);b.writeInt32LE(time);b.writeInt16LE(op,4);b.writeInt16LE(b.length,6);b[9]=difficulty;b.writeUInt16LE(mask,10);
    args.forEach((v,i)=>b.writeUInt32LE(v>>>0,12+i*4));return b;
}
const sentinel=()=>instruction(-1,[],0,0x7fffffff);
function program(subs){
    const header=Buffer.alloc(8+subs.length*4);header.writeUInt32LE(0x900);header.writeUInt16LE(subs.length,4);let offset=header.length;
    const scripts=subs.map(s=>Buffer.concat([...s,sentinel()]));scripts.forEach((s,i)=>{header.writeUInt32LE(offset,8+i*4);offset+=s.length;});return Buffer.concat([header,...scripts]);
}
async function harness(motion=false,emission=false,animation=false){
    const m=await oracle(),c=await core(),enemy=m.allocate(0x6000),manager=m.allocate(animation?0x2ac450:0x400),player=m.allocate(0x2000),otherPlayer=m.allocate(0x2000),owner=m.allocate(64),otherOwner=m.allocate(64),laserPool=m.allocate(64*0x59c),sprite=m.allocate(0x44),p=c.allocate(65536);
    const context=enemy+0x7f4,bases=[enemy,context,manager,player,otherPlayer,0,owner],heap=m.heap;let raw,loaded=0,fixture=0;
    const d=new DataView(c.memory.buffer),fields=Array.from({length:c.ecl_var_field_count()},(_,i)=>Array.from({length:4},(_,k)=>d.getUint32(c.ecl_var_fields()+i*16+k*4,true)));
    m.replace(0x42c970,'read-ecl',()=>{loaded=m.allocate(raw.length);m.write(loaded,raw);return loaded;},1);
    const extras=Array.from({length:c.ecl_vm_extra_count()},(_,i)=>Array.from({length:3},(_,k)=>d.getUint32(c.ecl_vm_extras()+i*12+k*4,true)));
    if(!motion)m.replace(0x408180,'post-script-enemy-motion',()=>0);if(!emission&&!animation)m.replace(0x408560,'post-script-fire-and-pose',()=>0);
    const animationEvents=[];m.replace(0x403e00,'start-enemy-animation',()=>{
        const [object,script]=m.readWords(m.reg('ESP')+4,2),slot=(object-enemy-8)/0x2a4,b=Buffer.alloc(12);b.writeInt32LE(slot);b.writeInt32LE(m.reg('ECX')===0x515151?1:0,4);b.writeUInt32LE(script,8);animationEvents.push(b);m.write(object+0x21a,new Uint8Array(new Int16Array([script]).buffer));return 0;
    },2);
    const events=[];m.replace(0x4131c0,'bullet-emission',()=>{events.push(m.bytes(m.u32(m.reg('ESP')+4),0x210));return 0;},1);
    const laserEvents=[];let nextLaser=0;m.replace(0x413290,'laser-emission',()=>{
        const source=m.u32(m.reg('ESP')+4),dest=laserPool+nextLaser++*0x59c;laserEvents.push(m.bytes(source,0x210));
        m.write(dest+0x548,m.bytes(source+4,12));m.u32(dest+0x554,m.u32(source+16));m.u32(dest+0x564,m.u32(source+0x1dc));m.f32(dest+0x568,3.5);m.u32(dest+0x584,1);return dest;
    },1);
    function setup(data,life=0){
        if(fixture)c.ecl_vm_delete(fixture);fixture=c.ecl_vm_create();raw=data;m.heap=heap;
        nextLaser=0;m.view(laserPool,64*0x59c).fill(0);
        for(const [a,n] of [[enemy,0x6000],[manager,0x400],[player,0x2000],[otherPlayer,0x2000],[owner,64],[otherOwner,64]])m.view(a,n).fill(0);
        m.u32(enemy,manager);m.u32(enemy+0x2ce0,context);m.u32(manager+0x320,owner);m.u32(manager+0x324,otherOwner);m.u32(owner+4,player);m.u32(otherOwner+4,otherPlayer);
        if(animation){m.u32(manager+0x2ac420,0x414141);m.u32(manager+0x2ac424,0x515151);}
        memory(c,p,data.length).set(data);assert.equal(c.ecl_vm_load(fixture,p,data.length,0),1);
        assert.equal(m.call(0x405660,{ecx:manager,args:[0]}),0);m.call(0x406850,{ecx:manager,args:[context,0]});
        m.write(enemy+0x2d70,[255,255]);
        m.write(enemy+0x3390,[255,255]);
        fields.forEach(([base,off,,size],i)=>m.write(bases[base]+off,memory(c,c.ecl_vm_field(fixture,i),size)));
        extras.forEach(([off,local,size])=>m.write(enemy+off,memory(c,c.ecl_vm_base(fixture)+local,size)));
        set(0,0x2e48,life);set(5,0x4ace0c,0x841a);set(0,0x2ce8,7);
        // Main/async contexts start with deterministic locals and call parameters.
        const local=fields.findIndex(f=>f[0]===1),args=fields.findIndex(f=>f[0]===2&&f[1]===0x168),floats=fields.findIndex(f=>f[0]===2&&f[1]===0x178);
        for(let i=0;i<8;++i){new DataView(c.memory.buffer).setInt32(c.ecl_vm_field(fixture,local)+i*4,7+i,true);m.i32(context+0x1c+i*4,7+i);
            new DataView(c.memory.buffer).setFloat32(c.ecl_vm_field(fixture,local)+32+i*4,-5.5+i*.75,true);m.f32(context+0x3c+i*4,-5.5+i*.75);}
        for(let i=0;i<4;++i){new DataView(c.memory.buffer).setInt32(c.ecl_vm_field(fixture,args)+i*4,23+i,true);m.i32(manager+0x168+i*4,23+i);
            new DataView(c.memory.buffer).setFloat32(c.ecl_vm_field(fixture,floats)+i*4,2.25+i,true);m.f32(manager+0x178+i*4,2.25+i);}
    }
    function set(kind,offset,value,real=false){
        const idx=fields.findIndex(f=>f[0]===kind&&f[1]<=offset&&f[1]+f[3]>offset);assert.ok(idx>=0);
        const address=c.ecl_vm_field(fixture,idx)+offset-fields[idx][1];const v=new DataView(c.memory.buffer);
        if(real){v.setFloat32(address,value,true);m.f32(bases[kind]+offset,value);}else {v.setUint32(address,value>>>0,true);m.u32(bases[kind]+offset,value);}
    }
    function verify(label){
        fields.forEach(([base,off,,size],i)=>assert.deepEqual(memory(c,c.ecl_vm_field(fixture,i),size),m.bytes(bases[base]+off,size),`${label} field ${base}/${off.toString(16)}`));
        extras.forEach(([off,local,size])=>assert.deepEqual(memory(c,c.ecl_vm_base(fixture)+local,size),m.bytes(enemy+off,size),`${label} extra ${off.toString(16)}`));
        for(let slot=-1;slot<4;++slot){
            const object=slot<0?enemy:m.u32(enemy+0x33d8+slot*4),ctx=slot<0?context:object+8;
            if(slot>=0){assert.equal(Boolean(c.ecl_vm_context(fixture,slot,0)),Boolean(object),label+' async '+slot);if(!object)continue;}
            for(const [part,off,size] of [[0,0x1c,120],[1,8,12],[2,0x94,12],[3,0xa0,384]]){
                const expected=m.bytes(ctx+off,size);if(part===3){const v=new DataView(expected.buffer);for(let n=0;n<8;++n)v.setUint32(n*48,v.getUint32(n*48,true)?1:0,true);}
                assert.deepEqual(memory(c,c.ecl_vm_context(fixture,slot,part),size),expected,`${label} context ${slot}/${part}`);
            }
            assert.equal(c.ecl_vm_meta(fixture,slot,0),new DataView(m.bytes(ctx+0x228,2).buffer).getInt16(0,true),label+' sub');
            assert.equal(c.ecl_vm_meta(fixture,slot,1),m.u32(ctx+4)-loaded,label+' pc');
            assert.equal(c.ecl_vm_meta(fixture,slot,2),new DataView(m.bytes(slot<0?enemy+0x2d28:object+6,2).buffer).getInt16(0,true),label+' depth');
        }
        for(const [part,off] of [[0,0x2d80],[1,0x2d8c]])assert.deepEqual(memory(c,c.ecl_vm_motion(fixture,part),12),m.bytes(enemy+off,12),label+' motion '+part);
        for(let slot=0;slot<32;++slot){const original=m.u32(enemy+0x32d8+slot*4),actual=c.ecl_vm_laser(fixture,slot);assert.equal(Boolean(actual),Boolean(original),label+' laser '+slot);
            if(actual)assert.deepEqual(memory(c,actual+0x548,0x52),m.bytes(original+0x548,0x52),label+' laser state '+slot);}
        const tableEnd=8+raw.readUInt16LE(4)*4+raw.readUInt16LE(6)*4;
        assert.deepEqual(memory(c,c.ecl_vm_data(fixture)+tableEnd,raw.length-tableEnd),m.bytes(loaded+tableEnd,raw.length-tableEnd),label+' bytecode mutations');
    }
    return {c,m,set,setup,verify,setSprite(){const b=Buffer.alloc(0x44);[.125,.25,.75,.875].forEach((n,i)=>b.writeFloatLE(n,0x20+i*4));memory(c,c.ecl_vm_sprite(fixture),b.length).set(b);m.write(sprite,b);m.u32(enemy+0x22c,sprite);},verifyTrail(label){assert.deepEqual(memory(c,c.ecl_vm_trail(fixture),194*28),m.bytes(enemy+0x3e68,194*28),label+' trail vertices');},setFlags(flags,difficulty=0){c.ecl_vm_set_flags(fixture,flags,difficulty);m.u32(enemy+0x337c,flags);m.view(enemy+0x3388,1)[0]=difficulty;},get fixture(){return fixture;},step(rate=1,step=1,mask=1,flags=0){
        events.length=0;
        laserEvents.length=0;
        animationEvents.length=0;
        m.f32(0x4b36b8,rate);m.f32(0x4a7e7c,step);m.u32(0x4b36d4,flags);m.u32(0x4a7eb0,mask);
        const expected=m.call(0x4086c0,{args:[enemy],limit:20000000})|0,actual=c.ecl_vm_step(fixture,rate,step,mask,flags,Number(motion),Number(emission),Number(animation));
        assert.equal(c.ecl_vm_emission_count(fixture),events.length,'emission count');
        events.forEach((event,index)=>assert.deepEqual(memory(c,c.ecl_vm_emissions(fixture)+index*0x210,0x210),event,'emission '+index));
        assert.equal(c.ecl_vm_laser_event_count(fixture),laserEvents.length,'laser count');laserEvents.forEach((event,index)=>assert.deepEqual(memory(c,c.ecl_vm_laser_events(fixture)+index*0x210,0x210),event,'laser emission '+index));
        assert.equal(c.ecl_vm_animation_event_count(fixture),animationEvents.length,'animation count');assert.deepEqual(memory(c,c.ecl_vm_animation_events(fixture),animationEvents.length*12),new Uint8Array(Buffer.concat(animationEvents)),'animation events');
        assert.equal(actual,expected,'executor status; opcode '+c.ecl_vm_meta(fixture,-1,4));return actual;
    },close(){if(fixture)c.ecl_vm_delete(fixture);c.release(p);m.close();}};
}

test('TH09 ECL arithmetic and control opcodes reproduce original context, timers and RNG',async()=>{
    const h=await harness();let checks=0;const tested=[];
    try{
        for(const mode of [0,1,2])for(let op=0;op<=51;++op){
            if([1,4,5,36].includes(op))continue;
            let args,mask=0;
            if(op===2)args=[5];
            else if(op>=40){const real=op%2;args=real?[bits(-5.5),bits(mode?3.25:-5.5),3,0]:[7,mode?3:7,3,0];
                if(mode===2){args[0]=real?bits(10016):10000;args[1]=real?bits(10017):10001;mask=3;}}
            else {
                const integer=[5,6,8,10,11,12,13,14,20,21,22,23,24,30,31].includes(op);
                args=integer?[10000,3,4,0,0]:[bits(10016),bits(3.25),bits(-2.125),bits(.375),bits(-6.25)];mask=1;
                if(op===38){args[1]=bits(10017);mask=3;}
                if(mode===1){args[0]=integer?17:bits(4.75);mask&=~1;}
                if(mode===2&&op!==38){args[1]=integer?10008:bits(10024);args[2]=integer?10009:bits(10025);mask|=6;}
            }
            const first=instruction(op,args,mask),marker=instruction(6,[10001,5],1);
            if(op>=40)first.writeInt32LE(first.length+marker.length,24);
            const data=program([[first,marker,instruction(6,[10001,9],1,3)]]);
            for(const rate of [1,.5]){
                h.setup(data);h.set(0,0x2d08,2.75,true);h.set(0,0x2d0c,1.25,true);h.set(0,0x2cec,3);
                for(let frame=0;frame<12;++frame){h.step(rate);h.verify(`op ${op}/${mode}/${rate}/${frame}`);++checks;}
            }
            tested.push(op);
        }
        report('ecl-arithmetic',{passed:true,checks,opcodes:[...new Set(tested)].sort((a,b)=>a-b),scope:'Script state and arithmetic; post-script enemy motion/fire/pose are separate boundaries.'});
    }finally{h.close();}
});

test('TH09 enemy movement commands match original polar, eased and orbit motion',async()=>{
    const h=await harness(true);let checks=0;const f=bits;
    const cases={63:[f(35.25),f(96.75)],64:[9,3,f(-37.5),f(122.25)],65:[f(.375),f(2.25)],66:[9,4,f(-.75),f(2.375)],
        67:[7,2,f(1.75)],68:[f(-.25),f(1.375)],69:[9,5,f(.875),f(1.625)],70:[f(.0625)],71:[f(-.03125)],
        72:[9,f(-17.25),f(92.5),f(.875),f(-.0625),f(12.5),f(.375)],73:[9,f(-1.125),f(.09375),f(.625)],74:[9,f(.125),f(.0625)],
        75:[f(-111),f(23),f(113),f(411)],76:[],77:[f(12.5),f(13.25)],78:[f(24.5),f(32.75)],79:[63],80:[63],81:[63],82:[f(47.25)],178:[9,3,f(2.125)]};
    try{
        for(const [op,args] of Object.entries(cases))for(const rate of [1,.5,.99])for(const mirrored of [false,true]){
            const commands=[instruction(75,[f(-128),f(16),f(128),f(432)]),instruction(63,[f(20),f(100)]),instruction(65,[f(.25),f(1.5)]),instruction(+op,args)];
            h.setup(program([commands]));
            h.setFlags(mirrored?0x8000:0);
            for(let frame=0;frame<20;++frame){h.step(rate);h.verify(`movement ${op}/${rate}/${mirrored}/${frame}`);++checks;}
        }
        report('enemy-motion',{passed:true,checks,opcodes:Object.keys(cases).map(Number),scope:'Movement commands and velocity update. Focus capture and post-script emission are tested separately when integrated.'});
    }finally{h.close();}
});

test('TH09 ECL subroutines, loops and parallel contexts preserve original execution order',async()=>{
    const h=await harness();let checks=0;
    const main=[instruction(6,[10000,4],1),instruction(52,[1]),instruction(135,[0,2]),instruction(2,[3]),instruction(30,[10008],1),instruction(135,[1,3]),instruction(135,[2,2]),instruction(135,[3,3]),instruction(2,[7]),instruction(30,[10008],1)];
    const loop=[instruction(30,[10008],1),instruction(5,[0,-16,10000],4),instruction(53)];
    const asyncA=[instruction(52,[1]),instruction(30,[10008],1),instruction(2,[3]),instruction(53)];
    const asyncB=[instruction(6,[10000,2],1),instruction(52,[1]),instruction(2,[4]),instruction(53)];
    try{
        for(const rate of [1,.5,.99]){
            h.setup(program([main,loop,asyncA,asyncB]));
            for(let frame=0;frame<50;++frame){h.step(rate);h.verify(`calls ${rate}/${frame}`);++checks;}
        }
        report('ecl-control',{passed:true,checks,parallelContexts:4,subroutines:4});
    }finally{h.close();}
});

test('TH09 ECL linear and Hermite interpolators preserve easing and target writes',async()=>{
    const h=await harness();let checks=0;
    try{
        for(let curve=0;curve<8;++curve)for(let easing=0;easing<7;++easing)for(const rate of [1,.5]){
            const data=program([[instruction(36,[bits(10016),7,curve,easing,bits(-3.25),bits(4.5),bits(2.75),bits(-.375)]),instruction(36,[bits(10042),5,curve,easing,bits(1.25),bits(2.5),bits(-.5),bits(.75)])]]);
            h.setup(data,100);
            for(let frame=0;frame<18;++frame){h.step(rate);h.verify(`interpolation ${curve}/${easing}/${rate}/${frame}`);++checks;}
        }
        report('ecl-interpolation',{passed:true,checks,curves:8,easingModes:7});
    }finally{h.close();}
});

test('TH09 ECL emissions retain original payloads, repeats, timers and random evaluation order',async()=>{
    const h=await harness(false,true);let checks=0;
    try{
        for(let pattern=0;pattern<9;++pattern)for(const variable of [false,true])for(const periodic of [false,true])for(const rate of [1,.5,.99]){
            const packed=variable?(10001<<16)|10000:(3<<16)|2;
            const args=variable?[packed,10000,10001,bits(10033),bits(10035),bits(10033),bits(10035),0x80200]:[packed,7,3,bits(2.75),bits(1.25),bits(-.875),bits(.2),0x80200];
            const commands=[instruction(110,[bits(4.25),bits(-7.5)]),instruction(111,[3,0x10,1,60,17,bits(2.5),bits(-999)]),instruction(113,[12,24])];
            if(periodic)commands.push(instruction(106,[7]),instruction(107));
            commands.push(instruction(96+pattern,args,variable?255:0),instruction(112));
            if(periodic)commands.push(instruction(108,[],0,6),instruction(105,[5],0,6));
            else commands.push(instruction(109,[],0,2));
            commands.push(instruction(113,[-1,32],0,9));
            h.setup(program([commands]),100);h.set(0,0x2d74,17.25,true);h.set(0,0x2d78,131.125,true);
            for(let frame=0;frame<24;++frame){h.step(rate);h.verify(`emitter ${pattern}/${variable}/${periodic}/${rate}/${frame}`);++checks;}
        }
        report('ecl-emissions',{passed:true,checks,opcodes:Array.from({length:18},(_,i)=>i+96),scope:'Original script execution and periodic emission compared at the bullet-creation boundary.'});
    }finally{h.close();}
});

test('TH09 ECL laser creation and reference commands match original script state',async()=>{
    const h=await harness();let checks=0;
    try{
        for(const op of [114,115])for(const variable of [false,true])for(const rate of [1,.5,.99]){
            const args=variable?[(10001<<16)|1,...[10033,10035,10042,10043,10016,10017].map(bits),10000,10001,10002,10003,10004,5]:[(2<<16)|1,...[.375,2.125,0,7.5,64,12.5].map(bits),12,25,7,4,5,1];
            const commands=[instruction(110,[bits(2.25),bits(7.125)]),instruction(116,[3]),instruction(op,args,variable?0x1fff:0),instruction(116,[7]),instruction(op===114?115:114,args,variable?0x1fff:0),
                instruction(117,[3,bits(variable?10033:.375)],variable?2:0,2),instruction(118,[3,bits(variable?10033:.125)],variable?2:0,3),
                instruction(119,[7,bits(variable?10042:3.5),bits(variable?10033:-1.25),bits(variable?10035:.5)],variable?14:0,4),
                instruction(120,[4],0,5),instruction(120,[7],0,6),instruction(121,[3],0,7),instruction(121,[3],0,8),instruction(120,[3],0,9),
                instruction(167,[7,bits(variable?10033:.875)],variable?2:0,10),instruction(170,[7,variable?10032:129],variable?2:0,11),
                instruction(171,[7,bits(variable?10033:134)],variable?2:0,12),instruction(172,[7,bits(variable?10035:-9.5),bits(variable?10033:97.75)],variable?6:0,13),instruction(154,[],0,14)];
            h.setup(program([commands]),50);h.set(0,0x2d74,-57.5,true);h.set(0,0x2d78,131.125,true);h.set(3,0x1b88,27.375,true);h.set(3,0x1b8c,311.75,true);
            for(let frame=0;frame<32;++frame){h.step(rate);h.verify(`laser ${op}/${variable}/${rate}/${frame}`);++checks;}
        }
        report('ecl-lasers',{passed:true,checks,opcodes:[114,115,116,117,118,119,120,121,154,167,170,171,172],scope:'Original ECL execution with recorded laser-creation boundary; reference state, angle/position changes, activity queries and stop transitions.'});
    }finally{h.close();}
});

test('TH09 enemy life stages, timeout, drops, flags and animation control preserve script state',async()=>{
    const h=await harness();let checks=0;
    const cases={128:[3,2,7,8,11],129:[255],130:[0xfffd],131:[270],132:[123],133:[2,750,23],134:[225,4],138:[0x80fe23],143:[3],144:[17,2],145:[3],149:[7],150:[1,19],152:[bits(-.875),bits(.625),1,2,3,4],153:[],155:[7],156:[1],159:[3],160:[99],161:[bits(.25)],162:[],165:[bits(1.125)],173:[1],177:[991],182:[3],183:[5],185:[7],187:[3]};
    try{
        for(const [key,literal] of Object.entries(cases))for(const variable of [false,true])for(const rate of [1,.5,.99]){
            const op=Number(key);let args=[...literal],mask=0;
            if(variable&&!([128,129,130,138,145,150,153,155,156,162].includes(op))){
                if(op===133){args=[10000,10032,10032];mask=7;}
                else if(op===152){args=[bits(10033),bits(10035),10032,10032,10032,10032];mask=63;}
                else if(op===165||op===161){args=[bits(10033)];mask=1;}
                else {args=args.map(()=>10032);mask=(1<<args.length)-1;}
            }
            const commands=[instruction(130,[0xfffa]),instruction(132,[43]),instruction(op,args,mask,1)];
            h.setup(program([commands]),50);h.set(1,0x1c,2);h.setFlags(0x65454545);
            for(let frame=0;frame<12;++frame){h.step(rate);h.verify(`status ${op}/${variable}/${rate}/${frame}`);++checks;}
        }
        const holes=[83,84,85,90,91,92,122,123,141,142,158,164,168,174,176,179,180,181,184];
        h.setup(program([holes.map(op=>instruction(op))]));h.step();h.verify('original unused command slots');++checks;
        report('ecl-enemy-status',{passed:true,checks,opcodes:Object.keys(cases).map(Number),unusedOpcodes:holes,scope:'Original script state; enemy damage and stage transition callbacks remain separate integration work.'});
    }finally{h.close();}
});

test('TH09 repeated polar evaluation and player-relative random direction preserve RNG and outputs',async()=>{
    const h=await harness();let checks=0;
    try{
        for(const op of [166,169])for(const variable of [false,true])for(const x of [0,95.99,96,96.01,191,288,288.01,390])for(const playerX of [20,192,310]){
            const args=op===166?[bits(10016),bits(10017),bits(variable?10033:.75),bits(variable?10032:5.75)]:[bits(10016)];
            const mask=op===166?(variable?15:3):1;
            h.setup(program([[instruction(op,args,mask),instruction(op,args,mask,2)]]));h.set(0,0x2d74,x,true);h.set(3,0x1b88,playerX,true);
            for(let frame=0;frame<5;++frame){h.step();h.verify(`extended arithmetic ${op}/${variable}/${x}/${playerX}/${frame}`);++checks;}
        }
        report('ecl-extra-arithmetic',{passed:true,checks,opcodes:[166,169]});
    }finally{h.close();}
});

test('TH09 enemy trail setup computes original vertex colors and texture coordinates',async()=>{
    const h=await harness();let checks=0;
    try{
        for(const flags of [0,1,2,8,9,10,255])for(const length of [6,12,36,96])for(const interval of [1,2,3,6])for(const variable of [false,true]){
            const args=variable?[flags,10000,10001,10002]:[flags,length,12,interval];
            h.setup(program([[instruction(157,args,variable?14:0)]]));h.set(1,0x1c,length);h.set(1,0x20,12);h.set(1,0x24,interval);h.setSprite();
            h.step();h.verify(`trail ${flags}/${length}/${interval}/${variable}`);h.verifyTrail(`trail ${flags}/${length}/${interval}/${variable}`);++checks;
        }
        report('enemy-trail-setup',{passed:true,checks,scope:'Original ECL 157 plus UV/color/reciprocal-W preparation for all generated vertices; trail movement and rasterization still require integration.'});
    }finally{h.close();}
});

test('TH09 enemy animation selection, extra layers and movement poses match original callbacks',async()=>{
    const h=await harness(true,false,true);let checks=0;
    try{
        for(let op=54;op<=62;++op)for(const variable of [false,true])for(const alternate of [false,true])for(const rate of [1,.5,.99]){
            let args=[],mask=0;
            if([54,55,58,59].includes(op)){args=[variable?10032:20];mask=variable?1:0;}
            else if(op===56||op===60){args=variable?Array(6).fill(10032):[20,21,22,23,24,25];mask=variable?63:0;}
            else if(op===57||op===61){args=variable?[10000,10032]:[1,21];mask=variable?3:0;}
            const commands=[instruction(alternate?59:55,[30]),instruction(op,args,mask,1),instruction(65,[bits(0),bits(1.5)],0,4),instruction(65,[bits(3.1415927),bits(1.5)],0,8),instruction(65,[bits(0),bits(0)],0,12),instruction(57,[1,-1],0,16)];
            h.setup(program([commands]),100);h.set(1,0x1c,1);h.setFlags(alternate?0x80008000:0);
            for(let frame=0;frame<24;++frame){h.step(rate);h.verify(`animation ${op}/${variable}/${alternate}/${rate}/${frame}`);++checks;}
        }
        report('ecl-animation',{passed:true,checks,opcodes:[54,55,56,57,58,59,60,61,62],scope:'ECL animation commands and native post-script movement poses. ANM starts are recorded boundaries; independent ANM tests cover actual script execution.'});
    }finally{h.close();}
});
