"""Real all-seat inputs, asynchronous corrections, unequal presentation cadence.

No invulnerability fixture. Exercise live Reimu A shooting, top-line pickups,
natural deaths/Spirit and native ANM/Item owners, not just isolated render modes.
"""
import argparse
import json
from rollback_testkit import call, equal, fixture, hashes, net, state, submit, tick

parser=argparse.ArgumentParser()
parser.add_argument('--url',default='http://127.0.0.1:8138/')
parser.add_argument('--output',default='artifacts/multiplayer-tests/presentation-rollback.json')
parser.add_argument('--frames',type=int,default=1800)
parser.add_argument('--absolute-touch',action='store_true')
args=parser.parse_args()
report={'scope':'Reimu A/P2 live input, top-line movement, real deaths, 1..7-frame rollback and unequal repeated Draw',
        'checkpoints':[],'draws':0,'lastFrame':59}

def controls(seat,frame):
    # Reach the automatic item collection line, sweep laterally and retreat.
    phase=(frame//90+seat*2)%8
    direction=[0x10,0x10,0x40,0x40,0x20,0x20,0x80,0x80][phase]
    if args.absolute_touch and seat==0:
        return {'buttons':1,'analogMode':2,'x':8300.0,'y':36500.0,'touchUsed':True,'unlimited':True}
    return 1|direction|(4 if frame//37%2 else 0)

with fixture(args.url,report,args.output) as test:
    reference,late=test.pair(local=1)
    for page in (reference,late):test.advance(page,59)
    first=60
    while first<60+args.frames:
        depth=1+(first//7)%7
        last=min(first+depth,60+args.frames)
        for frame in range(first,last):
            for page in (reference,late):
                assert call(page,'x=>multiplayerSmoke.captureLocal(...x)',[frame,controls(1,frame)])
            submit(reference,0,frame,controls(0,frame))
            tick(reference);tick(late)
            if args.absolute_touch and frame>61:
                p=call(late,'x=>multiplayerSmoke.playerDrawProbe(...x)',[0,1.0])
                report.setdefault('touchPredictions',[]).append({'frame':frame,'probe':p})
                if p[4]==1:
                    assert abs(p[2]-83.0)<.01 and abs(p[3]-365.0)<.01,('A missing absolute-touch packet fabricated a target at the origin',frame,p)
            before=hashes(late)
            for alpha in [0.0,0.2,0.7,1.0][:1+frame%4]:
                assert call(late,'a=>multiplayerSmoke.presentationDraw(a)',alpha)
                after=hashes(late)
                assert before[1:28]==after[1:28] and before[38:]==after[38:],('Draw mutated canonical owner',frame,alpha,before,after)
                report['draws']+=1
            report['lastFrame']=frame
        for frame in range(first,last):submit(late,0,frame,controls(0,frame))
        # This authoritative tick reconciles the prediction before comparing.
        for page in (reference,late):
            assert call(page,'x=>multiplayerSmoke.captureLocal(...x)',[last,controls(1,last)])
            submit(page,0,last,controls(0,last));tick(page)
        equal(reference,late,('after correction',last))
        if last%60<8:
            probes=[]
            for seat in (0,1):
                for alpha in (0.0,0.5,1.0):
                    p=call(late,'x=>multiplayerSmoke.playerDrawProbe(...x)',[seat,alpha]);probes.append(p)
                    if p[1]>=6 and p[4] in (1,3,4):
                        # Lifecycle changes can snap: accept either current or
                        # its one-tick interpolation, never a playfield-origin jump.
                        expected=[(p[2]+223.5,p[3]+15.5),
                                  (p[5]+(p[2]-p[5])*alpha+223.5,p[6]+(p[3]-p[6])*alpha+15.5)]
                        assert any(abs(p[7]-x)<2 and abs(p[8]-y)<2 for x,y in expected),('Player sprite left owner position',last,seat,alpha,p)
            report['checkpoints'].append({'frame':last,'state':state(late),'probes':probes})
        first=last+1
    report['final']={'state':state(late),'hashes':hashes(late),'net':net(late),'memory':call(late,'multiplayerSmoke.memoryStatus()')}
print(json.dumps({'passed':report['passed'],'lastFrame':report['lastFrame'],'draws':report['draws']}))
