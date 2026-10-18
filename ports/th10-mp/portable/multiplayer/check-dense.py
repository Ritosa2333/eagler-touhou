"""Real native 1200-bullet workload: bounded rollback, no latency claims."""
import argparse
import json
import statistics
import hashlib
import io
from pathlib import Path
from PIL import Image
from rollback_testkit import call, equal, fixture, net, submit, tick

parser=argparse.ArgumentParser()
parser.add_argument('--url',default='http://127.0.0.1:8140/')
parser.add_argument('--output',default='artifacts/multiplayer-tests/dense.json')
args=parser.parse_args()
report={'scope':'1200 native bullets, same-layout rollback oracle and local CPU/heap diagnostics','timings':[]}

with fixture(args.url,report,args.output) as test:
    reference,late=test.pair()
    for page in (reference,late):
        test.advance(page,59)
        assert call(page,'multiplayerSmoke.fixture(7)')
        assert call(page,'multiplayerSmoke.memoryStatus()')['words'][8]>=1200
    equal(reference,late,'dense initial state')
    initial_allocations=call(late,'multiplayerSmoke.memoryStatus()')['words'][7]
    for start in (60,80,100):
        while net(reference)[2]<start:
            frame=net(reference)[2]
            for page in (reference,late):
                submit(page,1,frame)
                report['timings'].append(call(page,'multiplayerSmoke.timedNetTick()')['milliseconds'])
            equal(reference,late,('dense exact',frame))
        direction=0x80 if start!=80 else 0x40
        for frame in range(start,start+12):
            submit(reference,1,frame,direction);tick(reference);tick(late)
        before=call(late,'multiplayerSmoke.memoryStatus()')
        assert before['words'][3]<=14 and before['words'][8]>=1200,before
        for frame in range(start,start+12):submit(late,1,frame,direction)
        for page in (reference,late):
            submit(page,1,start+12)
            milliseconds=call(page,'multiplayerSmoke.timedNetTick()')['milliseconds']
            report['timings'].append(milliseconds)
            if page is late:report.setdefault('correctionTickMs',[]).append(milliseconds)
        equal(reference,late,('dense corrected',start))
        after=call(late,'multiplayerSmoke.memoryStatus()')
        assert after['words'][7]==initial_allocations,('Replay storage leaked during rewind',after,initial_allocations)
        assert after['words'][10]>=1 and after['words'][11]>=12,after
        report.setdefault('corrections',[]).append({'frame':start,'before':before,'after':after})
    # Hash agreement alone does not prove rendering purity. Compare every
    # gameplay-field RGBA byte after correction; the timing-dependent HUD is
    # outside this explicitly declared rectangle.
    images=[]
    for name,page in [('reference',reference),('corrected',late)]:
        png=page.locator('#screen').screenshot()
        image=Image.open(io.BytesIO(png)).convert('RGBA')
        assert image.size==(640,480),image.size
        pixels=image.crop((32,16,416,464)).tobytes();images.append(pixels)
        target=Path(args.output).with_name('dense-'+name+'.png');target.write_bytes(png)
    assert images[0]==images[1],'Corrected gameplay pixels differ from exact forward Draw'
    report['pixelOracle']={'rectangle':[32,16,416,464],'bytes':len(images[0]),
                          'sha256':hashlib.sha256(images[0]).hexdigest(),'byteEqual':True}
    report['drawStateOracles']=[]
    for page in (reference,late):
        oracle=call(page,'multiplayerSmoke.drawStateOracle()')
        assert oracle[0]==1 and oracle[1]==100,('Authored Draw VM bytes differ',oracle)
        report['drawStateOracles'].append(oracle)
    report['collisionOracle']=call(reference,'multiplayerSmoke.collisionOracle()')
    assert report['collisionOracle'][0]==1 and report['collisionOracle'][1]==60068, report['collisionOracle']
    assert report['collisionOracle'][2]>10000,('Broadphase was not actually exercised',report['collisionOracle'])
    report['timingSummaryMs']={'median':statistics.median(report['timings']),'max':max(report['timings'])}
    report['note']='Software-rendered headless Chromium on this host; timing includes full correction ticks and does not prove mobile smoothness.'
    for page in (reference,late):assert call(page,'multiplayerSmoke.close()')
print(json.dumps({'passed':report['passed'],'timing':report.get('timingSummaryMs')}))
