import argparse
import hashlib
import json
import struct
from pathlib import Path

from bullet_probe import NativeBullet, ROOT, STACK_POINTER, datum, emulate
from enemy_probe import emulate as emulate_large

PLAYER=0x390000
CLOCK=0x4b6f58

def report_destination(path):
    target=Path(path).resolve()
    allowed=ROOT/'local/research'
    if allowed.resolve()!=allowed or target.parent!=allowed or target.suffix!='.json' or not target.name.startswith('player-window-'):
        raise ValueError('report must be deliverable local/research/player-window-*.json')
    return target

class NativePlayerWindow(NativeBullet):
    def animation_choice(self,current,previous):
        if any(type(value) is not int or not -4096<=value<=4096 for value in (current,previous)):
            raise ValueError('bounded actual horizontal input components required')
        state=bytearray(0xf68);struct.pack_into('<i',state,0xf64,previous)
        result=emulate([self.chunk(0x43b1b7,0x43b230)],0x43b1b7,
            [datum('player',PLAYER,bytes(state),writable=False)],registers={'esi':PLAYER,'edi':current&0xffffffff},
            stops=[0x43b230],stack={12:struct.pack('<i',current)})
        result['script']=result['registers']['ebx'] if result['stop']['reason']=='unsupported_call' else None
        result['memory'].pop('player');return result

    def timers(self,state,focus):
        if any(type(value) is not int or not 0<=value<=65535 for value in (state,focus)):
            raise ValueError('bounded explicit state/focus unit timers required')
        timers=struct.pack('<iifIIiifII',state-1,state,float(state),CLOCK,1,focus-1,focus,float(focus),CLOCK,1)
        result=emulate([self.chunk(0x43bea1,0x43c1bc)],0x43bea1,
            [datum('timers',PLAYER+0xf80,timers),datum('clock',CLOCK,struct.pack('<f',1),writable=False),
             self.constant(0x4a6698,8),self.constant(0x4a6694,4),self.constant(0x4a6588,8)],
            registers={'esi':PLAYER,'ebp':STACK_POINTER+0x200},stops=[0x43bf75])
        final=bytes.fromhex(result['memory']['timers']['after'])
        result['stateTimer']=list(struct.unpack_from('<iiIII',final,0))
        result['focusTimer']=list(struct.unpack_from('<iiIII',final,20))
        result['completedPlayerTick']=False
        return result

    def constructor_clear(self):
        size=0x43a94
        regions=[]
        for offset in range(0,size,65536):
            regions.append(datum(f'dirty-{offset}',0x300000+offset,b'\xa5'*min(65536,size-offset)))
        regions.append(datum('cpuMemset',0x4dcd68,0,writable=False))
        result=emulate_large([self.chunk(0x43a4f0,0x43a51c),self.chunk(0x47bb30,0x47bbaa)],0x43a4f0,
            regions,registers={'edi':0x300000},stops=[0x43a500],budget=180000)
        cleared=b''.join(bytes.fromhex(result['memory'][f'dirty-{offset}']['after']) for offset in range(0,size,65536))
        result['zeroedBytes']=len(cleared)
        result['allBytesZero']=not any(cleared)
        result['fields']={'flags438ec':struct.unpack_from('<I',cleared,0x438ec)[0],
                          'optional87a8':struct.unpack_from('<I',cleared,0x87a8)[0],
                          'optional87ac':struct.unpack_from('<I',cleared,0x87ac)[0]}
        result['initialPattern']='a5';result['clearedRegionBytes']=size;result['completedConstructor']=False
        result['regions']=[{'name':region['name'],'address':hex(region['address']),'bytes':len(region['bytes']),
                           'beforeSha256':hashlib.sha256(region['bytes']).hexdigest(),
                           'afterSha256':hashlib.sha256(bytes.fromhex(result['memory'][region['name']]['after'])).hexdigest()}
                          for region in regions]
        result.pop('memory')
        if result['stop']['reason']!='boundary' or not result['allBytesZero']:
            raise ValueError('original constructor clear did not complete its declared prefix')
        return result

    def null_detach(self):
        result=emulate([self.chunk(0x4679b0,0x4679f6),self.chunk(0x467420,0x467479)],0x4679b0,
            [datum('handle',PLAYER,0),datum('managerPointer',0x4d2e50,0,writable=False)],registers={'esi':PLAYER})
        result['handleAfter']=struct.unpack('<I',bytes.fromhex(result['memory']['handle']['after']))[0]
        return result

def probe(executable):
    native=NativePlayerWindow(executable)
    try:
        cases=[native.timers(state,focus) for state,focus in ((0,0),(7,3),(8,120),(65535,65535))]
        clear=native.constructor_clear();detach=native.null_detach()
        animations=[native.animation_choice(current,previous) for current,previous in ((-576,0),(-576,-576),(576,0),(576,-576),(0,-576),(0,576),(0,0))]
        if any(case['stop']['reason']!='boundary' for case in cases) or detach['stop']['reason']!='returned':
            raise ValueError('original timer/detach boundary was not completed')
        after=hashlib.sha256(native.executable.read_bytes()).hexdigest()
        return {'schema':'th128-original-player-window-prefixes/1','nativeCpuTrace':False,'windowsGameProcessLaunched':False,
                'executable':{'sha256':hashlib.sha256(native.binary).hexdigest(),'sha256After':after,
                              'unchanged':hashlib.sha256(native.binary).hexdigest()==after},
                'code':native.code,'constants':native.constants,'timerCases':cases,'constructorClear':clear,'nullDetach':detach,
                'animationChoices':animations,
                'limits':{'constructorBytes':0x43a94,'constructorInstructions':180000,'timerInstructions':2048},
                'limitations':['Original-instruction emulation, not native Windows CPU or full initialization/frame traces.',
                               'Constructor-clear prefix stops before global player publication and unexecuted FS restoration.',
                               'Timers use explicitly supplied valid initialized objects and exact global unit clock.',
                               'Zero optional-handle cleanup is a complete helper path, not completion of other initialization calls.']}
    finally:native.image.close()

def main(argv=None):
    parser=argparse.ArgumentParser(description='Bounded original player initialization/window prefixes')
    parser.add_argument('--exe',required=True)
    parser.add_argument('--output',default=str(ROOT/'local/research/player-window-fixtures.json'))
    arguments=parser.parse_args(argv)
    destination=report_destination(arguments.output)
    report=probe(arguments.exe)
    if not report['executable']['unchanged']:raise ValueError('original executable changed')
    destination.parent.mkdir(parents=True,exist_ok=True)
    destination.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print('Four exact unit-timer prefixes, a real constructor clear and zero-handle helper path verified.')

if __name__=='__main__':main()
