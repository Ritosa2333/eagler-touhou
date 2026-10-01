"""Final death, Spirit and item Draw at unequal refresh rates; native owners only."""
import argparse
import json
from pathlib import Path
from rollback_testkit import call, equal, fixture, hashes, net, state, submit, tick

parser=argparse.ArgumentParser()
parser.add_argument('--url',default='http://127.0.0.1:8138/')
parser.add_argument('--output',default='artifacts/multiplayer-tests/ghost-presentation.json')
args=parser.parse_args()
report={'scope':'native P1 final death/Spirit, repeated render-only Draw, exact state and sprite geometry', 'probes':[]}

with fixture(args.url,report,args.output) as test:
    reference,high=test.pair(local=1)
    for page in (reference,high):
        test.advance(page,59)
        assert call(page,'multiplayerSmoke.fixture(11)')
    for frame in range(60,240):
        value=1|(0x40 if frame//9%2 else 0x80)
        submit(reference,0,frame,value)
        tick(reference)
        if (frame-60)%7==6:
            # The remote P1 has already been predicted six frames. Correct
            # those frames before observing the next authoritative frontier.
            for delayed in range(frame-6,frame+1):
                submit(high,0,delayed,1|(0x40 if delayed//9%2 else 0x80))
        tick(high)
        if (frame-60)%7==6:equal(reference,high,('corrected before Draw',frame))
        before=hashes(high)
        for alpha in (0.0,0.25,0.75,1.0):
            assert call(high,'a=>multiplayerSmoke.presentationDraw(a)',alpha)
            after=hashes(high)
            assert before[1:28]==after[1:28] and before[38:]==after[38:],('Render-only mutated canonical state',frame,alpha,before,after)
        if (frame-60)%7==6:equal(reference,high,('corrected after repeated Draw',frame))
        if frame in (60,70,98,100,120,180,239):
            for alpha in (0.0,0.25,0.75,1.0):
                probe=call(high,'x=>multiplayerSmoke.playerDrawProbe(...x)',[0,alpha])
                report['probes'].append({'frame':frame,'alpha':alpha,'probe':probe})
                if probe[4]==3:
                    assert probe[1]>=6,('Spirit not rendered',probe)
                    x=probe[5]+(probe[2]-probe[5])*alpha+223.5
                    y=probe[6]+(probe[3]-probe[6])*alpha+15.5
                    assert abs(probe[7]-x)<1 and abs(probe[8]-y)<1,('Spirit sprite jumped away from logical owner',frame,alpha,x,y,probe)
    # Reconcile the last partial batch too; screenshots/final hashes must not
    # compare a confirmed endpoint against an intentionally predicted tail.
    for frame in range(net(high)[4]+1,240):
        submit(high,0,frame,1|(0x40 if frame//9%2 else 0x80))
    for page in (reference,high):
        submit(page,0,240,1|(0x40 if 240//9%2 else 0x80));tick(page)
    equal(reference,high,'final fully corrected Spirit')
    assert state(high)[12]==3,('P1 did not enter native Spirit',state(high))
    for name,page in [('reference',reference),('high',high)]:
        path=Path(args.output).with_name('ghost-'+name+'.png')
        path.parent.mkdir(parents=True,exist_ok=True)
        page.locator('#screen').screenshot(path=str(path))
    report['final']={'state':state(high),'hashes':hashes(high),'net':net(high)}
print(json.dumps({'passed':report['passed'],'probes':len(report['probes'])}))
