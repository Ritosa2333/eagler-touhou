"""Actual text81 Main background ANM and CPU quad at IRQ2/IRQ3 boundaries.

Texture pixels and device submission have the same explicit host boundaries
as the original custom-geometry sampler. No game instructions, VM poses,
interpolation, native quad corners or RNG results are replaced.
"""
import hashlib, json, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
source = ROOT / 'scripts/reverse/sample-custom-geometry.py'
scope = {'__file__': str(source), '__name__': 'background_main_quad_native_definitions'}
exec(compile(source.read_text(encoding='utf-8').split('\nrecords=[]', 1)[0], str(source), 'exec'), scope)
scope['RAW'] = (ROOT / 'local/retail/text.anm').read_bytes()
Native, EAX, ECX = [scope[key] for key in ['Native', 'UC_X86_REG_EAX', 'UC_X86_REG_ECX']]
records = []
for interrupt in [0, 2, 3]:
    for offset in [[0, 0], [.4, 0], [3.25, -2.5]]:
        native = Native()
        vm, manager = 0x2100000, 0x4000000
        native.call(0x454d10, [vm, 81], {ECX: native.resource['resource']})
        native.put(manager + 0xb0, '2f', *offset)
        if interrupt: native.put(vm + 0x3c4, 'h', interrupt)
        samples = []
        for tick in range(121):
            if interrupt or tick: native.call(0x455630, [vm])
            if tick not in [0, 1, 30, 60, 119, 120]: continue
            native.draws.clear()
            before = native.get(0x4ce568, '2I')
            native.call(0x45c900, [manager], {EAX: vm})
            assert before == native.get(0x4ce568, '2I')
            samples.append({'tick': tick, 'state': native.state(vm),
                            'vertices': native.draws[0]['vertices'] if native.draws else [],
                            'rngBeforeDraw': before, 'rngAfterDraw': native.get(0x4ce568, '2I')})
        records.append({'interrupt': interrupt, 'offset': offset, 'samples': samples})
output = ROOT / 'tests/fixtures/background-main-quad-native.json'
output.write_text(json.dumps({'schema': 'th12-native-background-main-quad/1',
    'originalExeSha256': scope['EXE_SHA256'], 'textAnmSha256': hashlib.sha256(scope['RAW']).hexdigest(),
    'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Real text.anm81 bind, controlled IRQ2/IRQ3 or no IRQ, original455630 interpolation and45c900 quad paths plus45a4a0 CPU batch copy. Camera offsets are explicit ANMManager+b0/b4 inputs. Texture-free resource host, heap and GPU output boundaries inherited from sample-custom-geometry.py. Main-VM application call sites and pixels are outside this fixture.',
    'records': records}, indent=2)+'\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'scenarios': len(records),
                  'samples': sum(len(r['samples']) for r in records)}))
