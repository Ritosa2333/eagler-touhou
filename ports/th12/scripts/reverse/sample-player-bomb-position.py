"""Original Player16: Bomb construction precedes normal movement.

Controlled loadout, position, state timer and held input; actual Player/Bomb
constructors, 436ba0 and ANM/resource binding. No original inputs are edited.
"""
import argparse, json, pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('SPAWN=0x2070000;')[0]
rows = []
parser = argparse.ArgumentParser()
parser.add_argument('--boundaries',action='store_true')
parser.add_argument('--respawn',action='store_true')
arguments = parser.parse_args()
for loadout, name in enumerate(['reimu-a','reimu-b','marisa-a','marisa-b','sanae-a','sanae-b']):
    if arguments.respawn and loadout!=5: continue
    fixture_path = ROOT / f'artifacts/reverse/enemy-input-bomb-lifecycle-{name}.json'
    if not fixture_path.exists():
        fixture = json.loads((ROOT / 'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json').read_text())
        fixture['character'], fixture['shot'] = divmod(loadout,2)
        fixture_path.write_text(json.dumps(fixture),encoding='utf-8')
    cases = [(1,120,0x12,2,False,1),(1,120,0x52,2,False,1),(1,120,0x1a,2,False,1),
             (4,7,0x12,2,False,1),(1,120,0x12,0,False,1)]
    if arguments.boundaries:
        cases = [(1,120,0x12,2,True,1),(1,120,0x12,2,False,0),(4,7,0x12,2,True,0),
                 (0,59,0x12,2,False,1),(0,60,0x12,2,False,1),(0,61,0x12,2,False,1),
                 (0,60,0x12,2,True,1),(0,60,0x12,2,False,0)]
    if arguments.respawn:
        cases = [(0,age,0,2,False,1) for age in [0,1,7,15,29,30,60]]
        cases += [(1,age,0,2,False,1) for age in [0,29,30]]
        cases += [(2,age,0,2,False,1) for age in [29,30,31]]
    for state, age, held, stock, message_active, live_enemies in cases:
        saved = sys.argv
        sys.argv = [str(PROVIDER),'--frames','0','--native-player','--native-items',
            '--native-bullets','--native-lasers','--active-collision','--instruction-budget','20000000',
            '--input',str(fixture_path),'--output',str(ROOT/'artifacts/reverse/player-bomb-position-bootstrap.json')]
        ns = {'__file__':str(PROVIDER),'__name__':'original_player_bomb_position'}
        try: exec(compile(BOOTSTRAP,str(PROVIDER),'exec'),ns)
        finally: sys.argv=saved
        from unicorn import UC_HOOK_CODE
        from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESI, UC_X86_REG_ESP
        mu,put,get,call=(ns[k] for k in ['v','put','get','call'])
        player,bomb=ns['PLAYER'],ns['bomb_manager']
        put(ns['MANAGER']+0x70,'i',live_enemies)
        if arguments.respawn and state==0:
            # Establish +a08 and the expanding death-position source by
            # executing the actual state2 transition, not copying its output.
            put(player+0x97c,'3f',20.,300.,0.)
            put(player+0x988,'2i',20*128,300*128)
            put(player+0xa28,'i',2)
            mu.reg_write(UC_X86_REG_EAX,player+0xa30)
            call(0x4067e0,30)
            put(0x4d49d0,'I',0)
            call(0x436ba0,player)
        if message_active:
            # Player16 tests only the active MSG pointer at6d30; this controlled
            # fixture never advances the synthetic message payload.
            message = ns['allocate_bytes'](0x100,'controlledActiveMessage')
            mu.mem_write(message,bytes(0x100))
            put(get(0x4b43e4,'I')[0]+0x6d30,'I',message)
        put(0x4b0ca0,'i',stock)
        put(player+0xa48,'i',120)
        put(player+0x988,'2i',111,54144)
        put(player+0x97c,'3f',111/128,423.,0.)
        if state==4:
            put(player+0xc404,'i',0)
            mu.reg_write(UC_X86_REG_ESI,player)
            call(0x438370)
        elif state in [0,2]:
            put(player+0xa28,'i',state)
        mu.reg_write(UC_X86_REG_EAX,player+0xa30)
        call(0x4067e0,age)
        put(0x4d49d0,'I',held)
        trace=[]
        def observe(uc,address,size,user):
            trace.append({'entry':hex(address),'position':list(get(player+0x97c,'2f')),
                          'fixedPosition':list(get(player+0x988,'2i'))})
            sp=mu.reg_read(UC_X86_REG_ESP)
            if address in [0x40caa0,0x4286f0]:
                from unicorn.x86_const import UC_X86_REG_EBX
                pointer=mu.reg_read(UC_X86_REG_EBX if address==0x40caa0 else UC_X86_REG_ESI)
                trace[-1].update({'clearOrigin':list(get(pointer,'3f')),
                                 'radius':get(sp+4,'f')[0],'flags':get(sp+8,'i')[0],
                                 'protection':get(sp+12,'i')[0]})
            elif address==0x40d230:
                from unicorn.x86_const import UC_X86_REG_EBX
                trace[-1]['skipProtected']=mu.reg_read(UC_X86_REG_EBX)
            elif address==0x428750:
                from unicorn.x86_const import UC_X86_REG_EBX,UC_X86_REG_EDI
                trace[-1].update({'convert':mu.reg_read(UC_X86_REG_EBX),'force':mu.reg_read(UC_X86_REG_EDI)})
        for address in [0x406bf0,0x4364f0,0x40d230,0x428750,0x40caa0,0x4286f0]:
            mu.hook_add(UC_HOOK_CODE,observe,begin=address,end=address)
        call(0x436ba0,player)
        rows.append({'initial':{'loadout':loadout,'state':state,'stateAge':age,'held':held,
                     'stock':stock,'fixedX':111,'fixedY':54144},
                     'after':{'state':get(player+0xa28,'i')[0],'stateAge':get(player+0xa34,'i')[0],
                       'playerPosition':list(get(player+0x97c,'2f')),'fixedPosition':list(get(player+0x988,'2i')),
                       'bombActive':get(bomb+0x3c,'i')[0],'bombPosition':list(get(bomb+0x508,'2f'))},'trace':trace})
        rows[-1]['initial'].update({'messageActive':message_active,'liveEnemies':live_enemies})
        if arguments.respawn:
            rows[-1]['after']['sources']=[]
            rows[-1]['after']['options']=[list(get(player+0x82bc+slot*0xe4,'2i'))
                for slot in range(4) if get(player+0x8260+slot*0xe4,'i')[0]]
            for slot in range(128):
                source=player+0x8988+slot*0x74
                if get(source+112,'I')[0]&1:
                    rows[-1]['after']['sources'].append({'position':list(get(source+24,'2f')),
                        'radius':get(source,'f')[0],'growth':get(source+4,'f')[0],
                        'previousTicks':get(source+76,'i')[0],'ticks':get(source+80,'i')[0],
                        'damage':get(source+96,'i')[0],'flags':get(source+112,'I')[0]})
        print(json.dumps({'loadout':loadout,'state':state,'held':held,'stock':stock,'trace':trace}),flush=True)
output=ROOT/('tests/fixtures/player-respawn-original.json' if arguments.respawn
             else 'tests/fixtures/player-bomb-gates-original.json' if arguments.boundaries
             else 'tests/fixtures/player-bomb-position-original.json')
scope = ('Original six-loadout startup, full436ba0. Controlled header, fixed position111/128,423, '
         'state timer, live enemy count and active MSG pointer. Actual438370 for deathbomb. '
         'No Bomb17, ANM27 or GPU pass; resource/heap/audio host boundaries of selected-manager provider.')
if arguments.respawn:
    scope += (' State0 cases first execute original state2 age30 transition at20,300, establishing '
              'the death-position source and+a08 before resetting controlled state0 age and player position. '
              'Clear arguments are observed at their original entry, without interception.')
output.write_text(json.dumps({'schema':'th12-original-player-bomb-position/1','originalExeSha256':ns['SHA'],
 'wholeGameOracle':False,'gpuExecuted':False,'scope':scope,
 'rows':rows},indent=2)+'\n',encoding='utf-8')
print(output)
