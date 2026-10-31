"""Actual Player16,43add0 option reset and MarisaA laser release/repress."""
import argparse,json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
PROVIDER=ROOT/'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP=PROVIDER.read_text(encoding='utf-8').split('SPAWN=0x2070000;')[0]
records=[]
parser=argparse.ArgumentParser()
parser.add_argument('--bomb',action='store_true')
arguments=parser.parse_args()
for loadout in range(6):
    source=json.loads((ROOT/'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json').read_text())
    source['character'],source['shot']=divmod(loadout,2)
    power=400 if arguments.bomb else 300
    source['initial'].update({'power':power,'x':-7362,'y':55296})
    fixture_path=ROOT/f'artifacts/reverse/friendly-repress-input-{loadout}.json'
    fixture_path.write_text(json.dumps(source),encoding='utf-8')
    saved=sys.argv
    sys.argv=[str(PROVIDER),'--frames','0','--native-player','--native-items','--native-bullets',
      '--native-lasers','--active-collision','--instruction-budget','20000000','--input',str(fixture_path),
      '--output',str(ROOT/'artifacts/reverse/friendly-repress-bootstrap.json')]
    ns={'__file__':str(PROVIDER),'__name__':'original_friendly_repress'}
    try:exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
    finally:sys.argv=saved
    from unicorn.x86_const import UC_X86_REG_ECX,UC_X86_REG_EDI
    mu,put,get,call=(ns[k] for k in ['v','put','get','call'])
    player=ns['PLAYER']
    put(ns['MANAGER']+0x70,'i',1)
    rows=[]
    # Finish one 15-tick shooting cycle, stop long enough to release its latch,
    # then immediately repress while the laser's actual eight-tick tail lives.
    inputs=[129]*5+[9]*9+[8]+[9]*20
    if arguments.bomb:inputs=[129]*8+[99]+[129]*16
    for frame,held in enumerate(inputs):
        ns['code_hook'].__globals__['frame']=frame
        put(0x4d49d0,'I',held)
        call(0x436ba0,player)
        friendly=[]
        for slot in range(256):
            p=player+0xa58+slot*0x78
            phase=get(p+0x48,'i')[0]
            if phase:
                spec=get(p+0x74,'I')[0]
                friendly.append({'slot':slot,'phase':phase,'age':get(p+4,'i')[0],
                    'position':list(get(p+0x14,'2f')),'type':get(spec+29,'b')[0],
                    'option':get(spec+28,'b')[0],'damage':get(p+0x60,'i')[0],
                    'hitbox':list(get(p+0x68,'2f'))})
        rows.append({'frame':frame,'held':held,'playerFixed':list(get(player+0x988,'2i')),
          'shootAge':get(player+0xc424,'i')[0],
          'options':[{'fixed':list(get(player+0x82bc+i*0xe4,'2i')),
                      'reset':get(player+0x8334+i*0xe4,'i')[0]} for i in range(4)
                     if get(player+0x8260+i*0xe4,'i')[0]],
          'friendly':friendly})
        mu.reg_write(UC_X86_REG_EDI,0x5000000)
        call(0x460fe0)
    records.append({'loadout':loadout,'power':power,'initialFixed':[-7362,55296],'frames':rows})
    print(json.dumps({'loadout':loadout,'firstOptions':rows[0]['options'],
      'laserRepress':[(f['frame'],[(b['slot'],b['phase'],b['option']) for b in f['friendly'] if b['type']==2])
                      for f in rows if f['frame'] in [14,15,16,17]]}),flush=True)
output=ROOT/('tests/fixtures/friendly-bomb-options-original.json' if arguments.bomb
             else 'tests/fixtures/friendly-repress-original.json')
output.write_text(json.dumps({'schema':'th12-original-friendly-repress/1','originalExeSha256':ns['SHA'],
 'wholeGameOracle':False,'gpuExecuted':False,'scope':f'Controlled six-loadout power{power} header at-7362,55296. '
 f'Actual43add0 restores positions and option reset flags. Original full436ba0 thenPrimary27 for{len(inputs)}inputs. '
 'Explicit live-enemy count1 without Enemy18 or collision queries; original resource/heap/audio host boundaries. '
 'Snapshots precede Primary27; no Bomb17 or Camera14 pass; no replay acceptance or altered input stream.',
 'records':records},indent=2)+'\n',encoding='utf-8')
print(output)
