"""Pure summary regression tests; never import argparse, Playwright or the runner."""
import ast
import math
from pathlib import Path
import unittest


def load_summary():
    path=Path(__file__).with_name('check-performance.py')
    tree=ast.parse(path.read_text(encoding='utf-8'),filename=str(path))
    definitions=[node for node in tree.body
                 if isinstance(node,ast.FunctionDef) and node.name in {'distribution','summarize'}]
    if sorted(node.name for node in definitions)!=['distribution','summarize']:
        raise AssertionError('Expected exactly the two summary functions')
    namespace={'math':math}
    exec(compile(ast.Module(body=definitions,type_ignores=[]),str(path),'exec'),namespace)
    return namespace['distribution'],namespace['summarize']


distribution,summarize=load_summary()


def fixture(cursor_mode='forward'):
    rows=[];high=0
    for i in range(411):
        cursor=(80+i-160 if cursor_mode=='retreat' else 159) if 160<=i<190 and cursor_mode!='forward' else i
        high=max(high,cursor)
        work=7 if cursor_mode=='retreat' and 160<=i<190 else 1
        rows.append([i*10,work,cursor,cursor,65536,high,cursor+1,-1])
    return {'initialFrame':0,'targetFrame':400,'rows':rows,
            'presents':[i*10 for i in range(411)],
            'finalMemory':{'words':[0]*10+[2,30]}}


def touch_fixture():
    period=1000/120;trace=[]
    for first,last,action in [(0,0,0),(1,2,1),(3,3,2),(4,5,2)]:
        first_at,last_at=1300+first*period,1300+last*period
        actual=last_at+2
        trace.append([first,last,first_at,last_at,actual,actual-first_at,2,last-first+1,action])
    return {'enabled':True,'targetHz':120,'plannedEvents':8,'accountedEvents':6,
            'submittedEvents':3,'coalescedEvents':2,'skippedNotReadyEvents':1,'unservicedAtStop':2,
            'timerCallbacks':4,'begins':1,'moves':2,'cancels':0,'maxTimerLatenessMs':period+2,
            'maxLatestTargetAgeMs':2,'traceDropped':0,'trace':trace,
            'firstSubmittedMs':trace[1][4],'lastSubmittedMs':trace[-1][4]}


class PerformanceSummaryTests(unittest.TestCase):
    def test_distribution_keeps_strict_long_gap_thresholds(self):
        self.assertIsNone(distribution([]))
        result=distribution([100.1,0,25,25.1,50,50.1,100])
        self.assertEqual((result['count'],result['p50'],result['p99']),(7,50,100.1))
        self.assertEqual((result['over25ms'],result['over50ms'],result['over100ms']),(5,3,1))

    def test_contiguous_forward_cadence_does_not_claim_motion(self):
        result=summarize(fixture())
        self.assertTrue(result['desktopCadenceTargetMet'])
        self.assertEqual(result['logicHz'],100)
        self.assertEqual(result['advanceGapMs']['max'],10)
        self.assertEqual(result['motionContinuityAcceptance'],'not measured')

    def test_retreat_keeps_every_intervening_callback_and_cost(self):
        data=fixture('retreat');result=summarize(data)
        window=result['window']
        self.assertEqual((window['startRowExclusive'],window['endRowInclusive']),(120,388))
        self.assertEqual((window['startMs'],window['endMs']),(1200,3880))
        self.assertEqual((window['callbacks'],window['newObservedFrames']),(268,268))
        self.assertEqual(result['callbackWorkMs']['count'],268)
        self.assertEqual(result['callbackWorkMs']['max'],7)
        self.assertAlmostEqual(result['callbackWorkPerNewFrameMs'],(238+30*7)/268)
        self.assertEqual(result['callbackArrivalGapMs']['max'],10)
        self.assertEqual(result['nativeCursorRegressions'],1)
        self.assertEqual(result['maxObservedCursorRetreatFrames'],79)
        self.assertEqual(result['advanceGapMs']['max'],310)
        legacy=fixture('retreat');legacy['rows']=[row[:5] for row in legacy['rows']]
        self.assertEqual(summarize(legacy)['window'],window)

    def test_presentations_cannot_hide_long_native_advance_stall(self):
        result=summarize(fixture('stall'))
        self.assertEqual(result['presentHz'],100)
        self.assertEqual(result['presentGapMs']['max'],10)
        self.assertEqual(result['nativeCursorRegressions'],0)
        self.assertEqual(result['advanceGapMs']['max'],310)
        self.assertGreater(result['advanceGapMs']['over100ms'],0)
        self.assertFalse(result['desktopCadenceTargetMet'])

    def test_fabricated_observed_frontier_and_session_change_rejected(self):
        data=fixture();data['rows'][170][5]+=1
        with self.assertRaisesRegex(AssertionError,'Observed frontier'):
            summarize(data)
        data=fixture();data['sessionIdentityChanges']=[{'at':1700}]
        with self.assertRaisesRegex(AssertionError,'session identity'):
            summarize(data)

    def test_required_progress_and_presentation_evidence_remain_gated(self):
        data=fixture();data['presents']=[]
        with self.assertRaisesRegex(AssertionError,'presentations'):
            summarize(data)
        data=fixture();data['targetFrame']=900
        with self.assertRaisesRegex(AssertionError,'bounds not reached'):
            summarize(data)

    def test_touch_submissions_coalescing_and_unserviced_plan_are_separate(self):
        data=fixture();data['touchMotion']=touch_fixture()
        motion=summarize(data)['touchMotion']
        self.assertEqual((motion['plannedEvents'],motion['submittedEvents'],motion['coalescedEvents']),(8,3,2))
        self.assertEqual((motion['skippedNotReadyEvents'],motion['unservicedAtStop']),(1,2))
        self.assertEqual(motion['recordedWindowSubmittedEvents'],3)
        self.assertAlmostEqual(motion['recordedWindowSubmittedSpanMs'],25)
        self.assertAlmostEqual(motion['submittedHz'],80)
        self.assertEqual(motion['recordedTimerLatenessMs']['count'],4)
        self.assertAlmostEqual(motion['recordedTimerLatenessMs']['max'],1000/120+2)

    def test_inconsistent_or_absent_sustained_touch_is_rejected(self):
        for field,value,message in [('accountedEvents',7,'accounting mismatch'),
                                    ('plannedEvents',9,'Unserviced touch plan'),
                                    ('moves',0,'No sustained native touch')]:
            with self.subTest(field=field):
                data=fixture();data['touchMotion']=touch_fixture();data['touchMotion'][field]=value
                with self.assertRaisesRegex(AssertionError,message):
                    summarize(data)
        data=fixture();data['touchMotion']=touch_fixture()
        for row in data['touchMotion']['trace']:
            row[4]+=10000
        with self.assertRaisesRegex(AssertionError,'inside the measured callback window'):
            summarize(data)


if __name__=='__main__':
    unittest.main()
