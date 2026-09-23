"""Real native 1200-bullet workload: bounded rollback, no latency claims."""
import argparse
import json
import statistics
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
            report['timings'].append(call(page,'multiplayerSmoke.timedNetTick()')['milliseconds'])
        equal(reference,late,('dense corrected',start))
        after=call(late,'multiplayerSmoke.memoryStatus()')
        assert after['words'][7]==initial_allocations,('Replay storage leaked during rewind',after,initial_allocations)
        assert after['words'][10]>=1 and after['words'][11]>=12,after
        report.setdefault('corrections',[]).append({'frame':start,'before':before,'after':after})
    report['timingSummaryMs']={'median':statistics.median(report['timings']),'max':max(report['timings'])}
    report['note']='Software-rendered headless Chromium on this host; timing includes full correction ticks and does not prove mobile smoothness.'
    for page in (reference,late):assert call(page,'multiplayerSmoke.close()')
print(json.dumps({'passed':report['passed'],'timing':report.get('timingSummaryMs')}))
