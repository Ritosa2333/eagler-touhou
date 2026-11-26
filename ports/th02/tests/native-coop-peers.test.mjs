import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createConfirmedPeerRun} from '../web/native-coop-peers.mjs';

function pair({delay=3,identity=['same','same'],hash=[42,42],flushHello=true}={}){
    const pending=[[],[]], channels=[{},{}], frames=[[],[]], errors=[[],[]], checks=[[],[]];
    const samples=[0,0];
    const runs=channels.map((channel,seat)=>{
        channel.send=text=>pending[1-seat].push(text);
        return createConfirmedPeerRun({
            count:2,seat,delay,identity:identity[seat],
            transport:{channelFor:()=>channel,connect:()=>channel.onopen(),close(){}},
            world:{step(frame,masks){assert.equal(frame,frames[seat].length);frames[seat].push(masks);return false;},hash:()=>hash[seat]},
            sample:()=>++samples[seat]&(seat?7:3),
            checkpoint:(at,value)=>checks[seat].push([at,value]),
            fail:error=>errors[seat].push(error.message),
        });
    });
    const deliver=(seat,index=0)=>channels[seat].onmessage({data:pending[seat].splice(index,1)[0]});
    const flush=()=>{for(let seat=0;seat<2;++seat)while(pending[seat].length)deliver(seat);};
    for(const run of runs)run.start();
    if(flushHello)flush();
    return {runs,frames,errors,checks,samples,deliver,flush};
}

test('the relay may deliver an input before the hello without advancing the world',()=>{
    const p=pair({delay:0,flushHello:false}),[host,guest]=p.runs;
    p.deliver(0);
    assert.equal(host.advance(),false); // Announces P1's row and waits for P2.
    p.deliver(1,1);
    assert.equal(guest.advance(),false); // Has P1's row, but not the room identity.
    assert.equal(p.frames[1].length,0);
    p.deliver(1);
    assert(guest.advance());
    p.flush();
    assert(host.advance());
    assert.deepEqual(p.frames[0],p.frames[1]);
    assert.deepEqual(p.errors,[[],[]]);
});

test('a late row stalls without dropping or resampling input, and later rows may arrive first',()=>{
    const p=pair(),[host,guest]=p.runs;
    for(let i=0;i<3;++i){assert(host.advance());assert(guest.advance());}
    p.deliver(0,1);p.deliver(0,1);
    assert.equal(p.frames[0].length,3);
    assert.equal(host.advance(),false);
    assert.equal(host.advance(),false);
    assert.equal(p.samples[0],4);
    p.flush();
    for(let i=0;i<117;++i){assert(host.advance());assert(guest.advance());p.flush();}
    assert.deepEqual(p.frames[0],p.frames[1]);
    assert.deepEqual(p.checks,[[[120,42]],[[120,42]]]);
    assert.deepEqual(p.errors,[[],[]]);
});

test('room identity mismatch stops before the first simulation frame',()=>{
    const p=pair({identity:['old','new']});
    assert(p.runs.every(run=>!run.ready&&!run.advance()));
    assert(p.errors.every(errors=>errors.length===1&&/room rules/.test(errors[0])));
    assert.deepEqual(p.samples,[0,0]);
});

test('the first unequal confirmed checkpoint stops both peers',()=>{
    const p=pair({hash:[42,43]});
    for(let i=0;i<120;++i){
        for(const run of p.runs)assert(run.advance());
        p.flush();
    }
    assert(p.errors.every(errors=>errors.length===1&&/diverged at frame 120/.test(errors[0])));
    assert(p.runs.every(run=>!run.ready&&!run.advance()));
});
