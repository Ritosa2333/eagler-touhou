import assert from 'node:assert/strict';
import {test} from 'node:test';
import {ConfirmedRoomClock,createConfirmedPeerRun} from '../web/native-coop-peers.mjs';

function pair({delay=3,identity=['same','same'],hash=[42,42],flushHello=true,wire}={}){
    const pending=[[],[]], channels=[{},{}], frames=[[],[]], errors=[[],[]], checks=[[],[]];
    const samples=[0,0];
    const runs=channels.map((channel,seat)=>{
        channel.send=text=>{
            if(wire&&JSON.parse(text).type==='input')wire(1-seat,text);
            else pending[1-seat].push(text);
        };
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
    return {runs,frames,errors,checks,samples,deliver,flush,
        receive:(seat,data)=>channels[seat].onmessage({data})};
}

test('the relay may deliver an input before the hello without advancing the world',()=>{
    const p=pair({delay:0,flushHello:false}),[host,guest]=p.runs;
    p.deliver(0);
    assert(host.capture());
    assert.equal(host.advance(),false); // Announces P1's row and waits for P2.
    p.deliver(1,1);
    assert.equal(guest.advance(),false); // Has P1's row, but not the room identity.
    assert.equal(p.frames[1].length,0);
    p.deliver(1);
    assert(guest.capture());
    assert(guest.advance());
    p.flush();
    assert(host.advance());
    assert.deepEqual(p.frames[0],p.frames[1]);
    assert.deepEqual(p.errors,[[],[]]);
});

test('a late row stalls without dropping or resampling input, and later rows may arrive first',()=>{
    const p=pair(),[host,guest]=p.runs;
    for(let i=0;i<3;++i){assert(host.capture());assert(guest.capture());assert(host.advance());assert(guest.advance());}
    p.deliver(0,1);p.deliver(0,1);
    assert.equal(p.frames[0].length,3);
    assert(host.capture());
    assert.equal(host.advance(),false);
    assert.equal(host.advance(),false);
    assert.equal(p.samples[0],4);
    p.flush();
    assert(guest.capture());
    p.flush();
    for(let i=0;i<117;++i){assert(host.advance());assert(guest.advance());assert(host.capture());assert(guest.capture());p.flush();}
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
        for(const run of p.runs){assert(run.capture());assert(run.advance());}
        p.flush();
    }
    assert(p.errors.every(errors=>errors.length===1&&/diverged at frame 120/.test(errors[0])));
    assert(p.runs.every(run=>!run.ready&&!run.advance()));
});

test('capture continues through a missing row, catches up in bounded batches and preserves ordered inputs',()=>{
    const p=pair(),clocks=p.runs.map(()=>new ConfirmedRoomClock(10));
    const tick=(now,deliver=true)=>{
        clocks.forEach((clock,seat)=>clock.frame(now,p.runs[seat]));
        if(deliver)p.flush();
    };
    for(let i=0;i<8;++i)tick(i*10,false);
    assert.deepEqual(p.runs.map(r=>r.frame),[3,3]);
    assert.deepEqual(p.samples,[8,8]);
    for(let i=8;i<30;++i)tick(i*10,false);
    assert.deepEqual(p.samples,[8,8],'bounded local future lead');
    p.flush();
    tick(300);
    assert.deepEqual(p.runs.map(r=>r.frame),[5,5]);
    assert(clocks.every(clock=>clock.catchups===1));
    for(let i=31;i<150;++i)tick(i*10);
    assert.deepEqual(p.frames[0],p.frames[1]);
    assert.deepEqual(p.errors,[[],[]]);
    assert(p.runs.every(r=>r.frame>=120));
    assert(p.runs.every(r=>r.captured===r.frame),'the extra capture lead drains after recovery');
});

test('independent capture sustains the retrace rate under delay above the three-frame input offset',t=>{
    function measure(independent){
        let now=0;
        const inFlight=[],period=10;
        const p=pair({wire(seat,text){
            const frame=JSON.parse(text).frame;
            inFlight.push({seat,text,at:now+33+((frame*7+seat*5)%12)});
        }});
        const clocks=p.runs.map(()=>new ConfirmedRoomClock(period));
        for(;now<2400;++now){
            for(let i=inFlight.length-1;i>=0;--i){
                const {seat,text,at}=inFlight[i];
                if(at<=now){inFlight.splice(i,1);p.receive(seat,text);}
            }
            p.runs.forEach((run,seat)=>{
                if(independent)clocks[seat].frame(now,run);
                else if(now%period===0){
                    if(run.captured===run.frame)run.capture();
                    run.advance();
                }
            });
            p.flush();
        }
        const frames=Math.min(...p.runs.map(r=>r.frame));
        assert.deepEqual(p.errors,[[],[]]);
        assert.deepEqual(p.frames[0].slice(0,frames),p.frames[1].slice(0,frames));
        return frames;
    }
    const baseline=measure(false),independent=measure(true);
    t.diagnostic(`baseline=${baseline} independent=${independent} of 240 retraces`);
    assert(independent>=235&&independent>=baseline+45);
});
