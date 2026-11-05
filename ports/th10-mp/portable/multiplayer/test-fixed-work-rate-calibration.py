"""Post-fence CPU intervention tests. Never imports the runner or opens a browser."""
import ast
import copy
import math
import time
import unittest
from pathlib import Path


path=Path(__file__).with_name('check-performance.py')
tree=ast.parse(path.read_text(encoding='utf-8'),filename=str(path))
names={'thread_time_metrics_raw','fixed_work_calibration_fingerprint','run_fixed_work_rate_calibration'}
definitions=[n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name in names]
assert {n.name for n in definitions}==names
ns={'math':math,'time':time,'call':lambda page,expression,arg=None:page.evaluate(expression,arg)}
exec(compile(ast.Module(body=definitions,type_ignores=[]),str(path),'exec'),ns)
calibrate=ns['run_fixed_work_rate_calibration']


def probe(seat,rate=4):
    result={'schema':1,'completed':True,'passed':True,'seat':seat,'errors':[],
        'threadTimeMeasured':False,'boundary':'post-state-audio-fence/pre-bulk',
        'timeOriginMs':1000+seat,'finalTimeOriginMs':1000+seat,
        'policy':{'iterations':2000000,'warmupGroups':2,'measurementGroups':5,
            'initialSeed':123456789,'indexMultiplier':0x9e3779b9},
        'module':{'byteLength':109,'imports':[],'definedMemories':0,
            'exports':[{'name':'kernel','kind':'function'}],'sectionIds':[1,3,7,10]}}
    for phase,count,offset in [('warmup',2,0),('measurements',5,2)]:
        rows=[];result[phase]={'groups':rows}
        for i in range(count):
            order=['js','wasm'] if i%2 else ['wasm','js']
            rows.append({'phase':phase,'index':i,'globalIndex':i+offset,'iterations':2000000,
                'seed':(123456789+(i+offset)*0x9e3779b9)&0xffffffff,'order':order,'matches':True,
                'runs':[{'engine':engine,'result':700+i+offset,'completed':True,'wallMs':rate*10} for engine in order]})
    return result


class Session:
    def __init__(self,seat,events,faults=None):
        self.seat=seat;self.events=events;self.faults=faults or {};self.calls=[];self.rate=4
    def send(self,method,params):
        assert method=='Emulation.setCPUThrottlingRate'
        self.rate=params['rate'];self.calls.append(self.rate)
        self.events.append(('rate',self.seat,self.rate))
        failure=self.faults.get(len(self.calls))
        if failure=='malformed':return None
        if failure:raise RuntimeError(failure)
        return {}


class Page:
    def __init__(self,seat,session,events,probe_fault=None,done=True):
        self.seat=seat;self.session=session;self.events=events;self.probe_fault=probe_fault
        self.done=done;self.contexts=0;self.probes=0
    def evaluate(self,expression,arg=None):
        if 'options=>multiplayerSmoke.fixedWorkProbe' in expression:
            self.probes+=1;self.events.append(('probe',self.seat,self.session.rate))
            value=probe(self.seat,self.session.rate)
            if self.probe_fault and self.probe_fault[0]==self.probes:
                fault=self.probe_fault[1]
                if fault=='throw':raise RuntimeError('probe failure')
                if fault=='realm':value['timeOriginMs']=value['finalTimeOriginMs']=9999
                if fault=='work':value['measurements']['groups'][0]['seed']+=1
                if fault=='result':
                    for run in value['measurements']['groups'][0]['runs']:run['result']+=1
            return value
        self.contexts+=1;self.events.append(('context',self.seat,self.session.rate))
        return {'done':self.done,'timeOriginMs':1000+self.seat,'nowMs':100,
                'visibility':'visible','hidden':False,'focused':True}


def fixture(count=2,session_faults=None,probe_fault=None):
    events=[];sessions=[Session(i,events,session_faults if i==0 else None) for i in range(count)]
    pages=[Page(i,sessions[i],events,probe_fault if i==0 else None) for i in range(count)]
    report={'cpuRates':[4]*count,'cpuThrottle':[{'seat':i,'rate':4,'acknowledged':True,'sessionId':'original-'+str(i)} for i in range(count)],
        'fixedWorkProbe':[probe(i) for i in range(count)],'summary':{'originalNativeMs':88,'passed':False},
        'threadTimeMetrics':{'samples':['original-4x-only']}}
    return report,pages,sessions,events


class RateCalibrationTests(unittest.TestCase):
    def test_disabled_does_not_touch_report_pages_or_sessions(self):
        class Untouchable:
            def __getattribute__(self,name):raise AssertionError('Disabled observer touched inputs')
            def __len__(self):raise AssertionError('Disabled observer touched inputs')
        report={}
        self.assertIsNone(calibrate(False,report,Untouchable(),Untouchable(),False,False))
        self.assertEqual(report,{})

    def test_original_probes_and_metrics_unchanged_original_sessions_only(self):
        report,pages,sessions,events=fixture();original=copy.deepcopy(report)
        result=calibrate(True,report,pages,sessions,True,True)
        self.assertTrue(result['passed'])
        self.assertEqual({k:v for k,v in report.items() if k!='fixedWorkRateCalibration'},original)
        self.assertEqual(events,[('context',0,4),('context',1,4),('rate',0,1),('probe',0,1),
            ('rate',0,4),('probe',0,4),('context',0,4),('rate',1,1),('probe',1,1),
            ('rate',1,4),('probe',1,4),('context',1,4)])
        for seat,peer in enumerate(result['peers']):
            self.assertEqual(peer['sessionId'],'original-'+str(seat))
            self.assertTrue(peer['originalThrottleSession'] and peer['restored4xAcknowledged'])
            self.assertEqual([c['params']['rate'] for c in peer['commands']],[1,4])

    def test_all_gates_precede_rate_mutation(self):
        for failure in ['native','fence','rate','missing-probe','bad-second-probe','second-running','session-alias']:
            with self.subTest(failure=failure):
                report,pages,sessions,events=fixture()
                if failure=='rate':report['cpuRates'][0]=1
                if failure=='missing-probe':report['fixedWorkProbe'].pop()
                if failure=='bad-second-probe':report['fixedWorkProbe'][1]['passed']=False
                if failure=='second-running':pages[1].done=False
                if failure=='session-alias':sessions[1]=sessions[0]
                with self.assertRaises(AssertionError):
                    calibrate(True,report,pages,sessions,failure!='native',failure!='fence')
                self.assertFalse(any(e[0] in ['rate','probe'] for e in events))

    def test_ambiguous_first_command_is_restored(self):
        report,pages,sessions,events=fixture(session_faults={1:'reply lost after apply'})
        with self.assertRaisesRegex(RuntimeError,'reply lost'):
            calibrate(True,report,pages,sessions,True,True)
        peer=report['fixedWorkRateCalibration']['peers'][0]
        self.assertEqual(sessions[0].calls,[1,4])
        self.assertEqual(sessions[0].rate,4)
        self.assertTrue(peer['restored4xAcknowledged'])
        self.assertIn('reply lost',peer['commands'][0]['error'])
        self.assertEqual(peer['commands'][1]['stage'],'finally-restore-4x')

    def test_probe_failure_restores_and_retains_failed_segment(self):
        report,pages,sessions,events=fixture(probe_fault=(1,'throw'))
        with self.assertRaisesRegex(RuntimeError,'probe failure'):
            calibrate(True,report,pages,sessions,True,True)
        peer=report['fixedWorkRateCalibration']['peers'][0]
        self.assertEqual(sessions[0].calls,[1,4])
        self.assertIn('probe failure',peer['segments'][0]['error'])
        self.assertTrue(peer['restored4xAcknowledged'])
        self.assertFalse(report['fixedWorkRateCalibration']['passed'])

    def test_restoration_failure_gets_one_bounded_fallback(self):
        for failures,restored in [({2:'restore reply lost'},True),({2:'restore failed',3:'fallback failed'},False),({1:'malformed'},True)]:
            with self.subTest(failures=failures):
                report,pages,sessions,events=fixture(session_faults=failures)
                with self.assertRaises((RuntimeError,AssertionError)):
                    calibrate(True,report,pages,sessions,True,True)
                peer=report['fixedWorkRateCalibration']['peers'][0]
                self.assertEqual(sessions[0].calls,[1,4,4] if 2 in failures else [1,4])
                self.assertEqual(peer['restored4xAcknowledged'],restored)
                self.assertFalse(report['fixedWorkRateCalibration']['passed'])

    def test_after_restore_probe_failure_does_not_reset_again(self):
        report,pages,sessions,events=fixture(probe_fault=(2,'throw'))
        with self.assertRaisesRegex(RuntimeError,'probe failure'):
            calibrate(True,report,pages,sessions,True,True)
        self.assertEqual(sessions[0].calls,[1,4])
        self.assertTrue(report['fixedWorkRateCalibration']['peers'][0]['restored4xAcknowledged'])

    def test_different_realm_work_or_checksum_cannot_be_compared(self):
        for fault in ['realm','work','result']:
            with self.subTest(fault=fault):
                report,pages,sessions,events=fixture(probe_fault=(1,fault))
                with self.assertRaises(AssertionError):
                    calibrate(True,report,pages,sessions,True,True)
                self.assertEqual(sessions[0].calls,[1,4])
                self.assertFalse(report['fixedWorkRateCalibration']['passed'])


if __name__=='__main__':unittest.main()
