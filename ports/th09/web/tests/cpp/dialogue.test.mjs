import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root} from './helpers.mjs';
test('All original dialogue resources, text, portrait control and victory selection match native execution',async()=>{
    const m=await oracle(),c=await core(),f=c.dialogue_fixture(),parts=Array.from({length:7},(_,i)=>c.dialogue_part(f,i)),native=m.allocate(0x1d70),stage=m.allocate(16),players=[m.allocate(0x31000),m.allocate(0x31000)],huds=[m.allocate(0xa680),m.allocate(0xa680)],backgrounds=[m.allocate(32),m.allocate(32)];
    let dv=new DataView(c.memory.buffer),events=[],texts=[],panels=[],file=0,checks=0,scripts=0,draws=0,selections=0,lines=0;
    const fields=Array.from({length:c.dialogue_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.dialogue_fields()+(i*3+j)*4,true))),arg=i=>m.u32(m.reg('ESP')+4+i*4),event=(...a)=>events.push([...a,...Array(8-a.length).fill(0)].map(x=>x|0));
    function index(vm){const n=(vm-native-0x14)/0x2a4;assert.equal(n,Math.floor(n));return n;}
    function clear(){events=[];texts=[];panels=[];}
    m.replace(0x403e00,'dialogue-start-animation',()=>{const vm=arg(0),script=arg(1);event(0,index(vm),m.reg('ECX')-0x100,script);m.view(vm,0x2a4).fill(0);m.write(vm+0x21a,[script&255,(script>>>8)&255]);m.u32(vm+0x1f8,1);m.u32(vm+0x1f0,0xffffffff);return 0;},2);
    m.replace(0x436ac0,'dialogue-sprite',()=>{event(1,index(arg(0)),m.reg('ECX')-0x100,arg(1));m.write(arg(0)+0x214,[arg(1)&255,(arg(1)>>>8)&255]);return 0;},2);
    m.replace(0x436f30,'dialogue-animation-update',()=>{event(2,index(arg(0)));m.i32(arg(0)+0x100,m.i32(arg(0)+0x100)+1);return 0;},1);
    for(const [fn,right] of [[0x43a950,0],[0x43aa50,1]])m.replace(fn,'dialogue-animation-draw',()=>{event(3,index(arg(0)),right);return 0;},1);
    m.replace(0x43bdc0,'dialogue-text',()=>{const a=[];for(let p=arg(4),n=0;n<512;++p,++n){const ch=m.bytes(p,1)[0];if(!ch)break;a.push(ch);}event(4,index(arg(1)),arg(2),arg(3),a.length);texts.push(...a,0);++lines;return 0;});
    m.replace(0x431930,'dialogue-music',()=>{event(5,arg(0));return 0;},1);m.replace(0x42fd30,'dialogue-stop-music',()=>{event(5,-1);return 0;});
    m.replace(0x42fe20,'dialogue-music-fade',()=>{event(6);return 0;},1);m.replace(0x415e50,'dialogue-results',()=>{event(7);return 0;});
    m.replace(0x422d20,'dialogue-transition',()=>{assert.equal(m.reg('ECX'),5);assert.equal(m.reg('EDX'),300);assert.deepEqual(Array.from({length:5},(_,i)=>arg(i)),[0xffffff,0,0,35,2]);event(9);return 0;},5);
    m.replace(0x403c20,'dialogue-enter-player',()=>{const s=players.indexOf(m.reg('ECX'));assert.equal(arg(0),1);event(10,s);return 0;},1);
    m.replace(0x41a210,'dialogue-show-hud',()=>{event(11,huds.indexOf(m.reg('ECX')));return 0;});
    for(let s=0;s<2;++s){m.u32(0x4a7d94+s*0x38,players[s]);m.u32(0x4a7da8+s*0x38,huds[s]);m.u32(0x4a7d90+s*0x38,backgrounds[s]);m.u32(huds[s]+0xa678,0x101+s);m.u32(huds[s]+0xa67c,0x103);for(const [n,off] of [[0,0x303f0],[1,0x303f8],[2,0x303fc]]){dv.setInt32(parts[1]+s*12+n*4,30+s*80+n*7,true);m.i32(players[s]+off,30+s*80+n*7);}}
    m.u32(0x4a7e80,stage);m.write(stage+12,[17,0]);m.u32(0x4b36cc,0x100);m.u32(0x4b36d4,0);
    const device=m.allocate(4),vt=m.allocate(0x140);m.u32(device,vt);m.u32(0x4b3108,device);for(const [slot,argc] of [[0xfc,4],[0x130,2]])m.u32(vt+slot,m.registerImport({dll:'draw',name:'state',argc,handler:()=>0}));
    m.u32(vt+0x120,m.registerImport({dll:'draw',name:'quad',argc:5,handler:()=>{assert.equal(arg(1),5);assert.equal(arg(2),2);event(12,4);panels.push(...m.bytes(arg(3),80));return 0;}}));
    for(const a of [0x4396a0,0x40e380,0x415d00,0x40e370,0x415d10])m.replace(a,'dialogue-render-state',()=>0);m.replace(0x42ff40,'dialogue-render-setting',()=>0,2);
    function compare(label){
        dv=new DataView(c.memory.buffer);assert.equal(c.dialogue_invalid(f),0,label+' valid');assert.equal(c.dialogue_cursor(f),m.u32(native+4)-file,label+' cursor');
        for(const [off,at,size] of fields)assert.deepEqual(memory(c,parts[0]+at,size),m.bytes(native+off,size),label+' field '+off.toString(16));
        for(let s=0;s<2;++s)assert.equal(dv.getInt32(parts[2]+s*4,true),m.i32(backgrounds[s]+0x18),label+' background '+s);
        assert.equal(memory(c,parts[3],1)[0],m.bytes(0x4a7ecd,1)[0],label+' game-over');assert.equal(memory(c,parts[4],1)[0],m.bytes(0x4a7ece,1)[0],label+' match-end');assert.equal(dv.getInt32(parts[5],true),m.i32(0x4b3690),label+' transition');
        assert.equal(c.dialogue_event_count(f),events.length,label+' events');events.forEach((e,i)=>assert.deepEqual(Array.from({length:8},(_,j)=>dv.getInt32(c.dialogue_events(f)+i*32+j*4,true)),e,label+' event'+i));
        assert.equal(c.dialogue_text_size(f),texts.length,label+' text size');assert.deepEqual(memory(c,c.dialogue_text(f),texts.length),Uint8Array.from(texts),label+' decrypted text');
        assert.equal(c.dialogue_panel_size(f),panels.length,label+' panel size');assert.deepEqual(memory(c,c.dialogue_panel(f),panels.length),Uint8Array.from(panels),label+' panel');++checks;
    }
    try{
        for(const filename of readdirSync(resolve(root,'reference/assets')).filter(n=>n.endsWith('.msg')).sort()){
            const bytes=readFileSync(resolve(root,'reference/assets',filename)),count=bytes.readUInt32LE(0),data=c.allocate(bytes.length);memory(c,data,bytes.length).set(bytes);assert.equal(c.dialogue_load(f,data,bytes.length),1,filename+' parse');c.release(data);file=m.allocate(bytes.length);m.write(file,bytes);
            for(let i=0;i<count;++i){const off=bytes.readUInt32LE(4+i*8);if(off)m.u32(file+4+i*8,file+off);}
            m.u32(native,file);m.u32(native+12,file);
            for(let script=0;script<count;++script){
                if(!bytes.readUInt32LE(4+script*8))continue;const flip=script%2,mode=filename.includes('_match')?2:script%2;
                for(const p of [parts[2],parts[2]+4,parts[5]])memory(c,p,4).fill(0);memory(c,parts[3],1)[0]=0;memory(c,parts[4],1)[0]=0;for(let s=0;s<2;++s)m.u32(backgrounds[s]+0x18,0);m.u32(0x4a7ecc,0);m.u32(0x4b3690,0);
                clear();m.call(0x416160,{ecx:native,args:[script,flip]});assert.equal(c.dialogue_begin(f,script,flip,0,0),1);compare(filename+' begin '+script);++scripts;
                const rate=[1,.5,.99][script%3];m.f32(0x4b36b8,rate);m.i32(0x4a7ea8,mode);
                for(let frame=0;frame<1500;++frame){
                    const held=script%13===0&&frame<160?0:0x100,pressed=frame%7===0?1:0;m.write(0x4acf34+0x2c,[held&255,held>>>8]);m.write(0x4acf34+0x32,[pressed,0]);clear();const original=m.call(0x416590,{ecx:native})|0,local=c.dialogue_step(f,held,pressed,rate,mode,17);assert.equal(local,original);compare(filename+'/'+script+'/'+frame);
                    if(original<0||m.bytes(0x4a7ecd,2).some(Boolean)||m.u32(0x4b3690))break;
                    if(frame%29===0){clear();m.call(0x417130,{ecx:native});c.dialogue_draw(f);compare(filename+' draw '+script+'/'+frame);++draws;}
                    assert.notEqual(frame,1499,filename+' script did not end '+script);
                }
            }
            if(filename.includes('_match'))for(let n=0;n<128;++n){
                const flip=n%2,opponent=n%16,seed=351+n*197;m.u32(0x4ace0c,seed);m.u32(0x4ace10,0);memory(c,parts[6],8).set(m.bytes(0x4ace0c,8));m.i32(0x4a7db0+(1-flip)*0x38,opponent);
                clear();m.call(0x4162d0,{ecx:native,args:[flip]});assert.equal(c.dialogue_begin(f,0,flip,1,opponent),1);compare(filename+' victory '+n);assert.deepEqual(memory(c,parts[6],8),m.bytes(0x4ace0c,8));++selections;
            }
        }
        report('dialogue',{checks,scripts,draws,selections,lines,resources:30,scope:'All original Japanese MSG scripts, native byte text decryption, flip/portrait/speaker state, wait/skip timing, victory weighting/RNG, draw order and dialogue-panel vertices. Animation programs, glyph rasterization, audio and scene transitions are recorded boundaries.'});
    }finally{c.dialogue_delete(f);m.close();}
});
