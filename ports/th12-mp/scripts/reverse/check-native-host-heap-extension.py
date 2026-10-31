"""Host capacity checks and actual short-prefix equivalence for the opt-in arena.

Controlled malloc/realloc/free requests test only the declared host boundary.
Separately, original selected-manager CPU traces execute the same unmodified
60-frame replay prefix using the old provider, default provider and an enabled
but unused1MiB extension. No fixture/GPU/full-game acceptance is produced.
"""
import gc, hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BEFORE = ROOT / 'artifacts/reverse/native-provider-before-host-heap-extension.py'
OUT = ROOT / 'artifacts/reverse/host-heap-extension-check'
OUT.mkdir(parents=True, exist_ok=True)
sys.path.insert(0, str(ROOT / 'scripts/reverse'))
SOURCE = PROVIDER.read_text(encoding='utf-8')
INPUT = ROOT / 'artifacts/reverse/world-input-th12_14-s3-oracle-3700-inputs.json'

def run(source, arguments, bootstrap=False):
    saved = sys.argv
    sys.argv = [str(PROVIDER), *arguments]
    ns = {'__file__': str(PROVIDER), '__name__': 'native_host_heap_capacity_evidence'}
    try:
        exec(compile(source.split('SPAWN=0x2070000;')[0] if bootstrap else source,
                     str(PROVIDER), 'exec'), ns)
    finally:
        sys.argv = saved
    return ns

def fitting_records(audit):
    alloc_regions = ['heap'] + (['heapExtension'] if 'heapExtension' in audit['regions'] else [])
    fixed = [value for key, value in audit['regions'].items() if key not in alloc_regions]
    for record in audit['allocations']:
        start, end = int(record['address'], 16), int(record['endExclusive'], 16)
        assert any(int(audit['regions'][name][0], 16) <= start < end <= int(audit['regions'][name][1], 16)
                   for name in alloc_regions), record
        assert all(end <= int(a, 16) or start >= int(b, 16) for a, b in fixed), record

evidence = []
# Default0 keeps the old capacity, alignment, error and ledger commit behavior.
ns = run(SOURCE, ['--frames', '0', '--output', str(OUT / 'controlled-default.json')], True)
ns['put'](ns['SP'], 'I', ns['STOP'])
from unicorn.x86_const import UC_X86_REG_ESP
ns['v'].reg_write(UC_X86_REG_ESP, ns['SP'])
assert ns['allocate_bytes'](0, 'controlledZero') == 0x3000000
assert ns['allocate_bytes'](0x1000000 - 16, 'controlledFill') == 0x3000010
before = ns['capture_heap_audit']()
try:
    ns['allocate_bytes'](1, 'controlledExhaustion')
except RuntimeError as error:
    assert str(error) == 'Synthetic host heap exhausted before allocation 0x4000000..0x4000010'
    default_error = str(error)
else:
    raise AssertionError('Disabled extension unexpectedly allocated')
assert ns['capture_heap_audit']() == before
assert 'heapExtension' not in before['regions']
fitting_records(before)
evidence.append({'case': 'default0-capacity-and-failure', 'error': default_error, 'audit': before})
del ns
gc.collect()

# The CRT API uses exactly the same allocator and can preserve a primary block
# across a real host HeapReAlloc transition to the extension. Free has its
# existing metadata-only/no-reuse semantics, not a new gameplay allocator.
arguments = ['--frames', '0', '--native-player', '--native-items', '--native-bullets', '--native-lasers',
             '--active-collision', '--capture-text-output', '--instruction-budget', '20000000',
             '--input', str(INPUT), '--host-heap-extra-mib', '1', '--output', str(OUT / 'controlled-spill.json')]
ns = run(SOURCE, arguments, True)
host = ns['crt_host']
call, allocate, get = (ns[key] for key in ['call', 'allocate_bytes', 'get'])
baseline = ns['capture_heap_audit']()
try:
    allocate(0x2000000, 'controlledOversized')
except RuntimeError as error:
    oversized_error = str(error)
else:
    raise AssertionError('Oversized spill unexpectedly fit')
assert ns['capture_heap_audit']() == baseline
small = call(host.addresses['HeapAlloc'], host.heap, 8, 32)
pattern = bytes(range(32))
ns['v'].mem_write(small, pattern)
remaining = ns['heap_regions']['heap'][1] - ns['heap']
allocate(remaining - 16, 'controlledPrimaryTail')
primary_before_spill = ns['heap']
reallocated = call(host.addresses['HeapReAlloc'], host.heap, 8, small, 64)
assert reallocated == 0x8000000
assert bytes(ns['v'].mem_read(reallocated, 64)) == pattern + bytes(32)
assert call(host.addresses['HeapSize'], host.heap, 0, reallocated) == 64
cursor = ns['heap']
assert call(host.addresses['HeapFree'], host.heap, 0, reallocated) == 1
assert ns['heap'] == cursor
next_pointer = call(0x46c9ea, 1)
assert next_pointer == 0x8000040
allocate(ns['heap_regions']['heapExtension'][1] - ns['heap'], 'controlledExtensionFill')
final_audit = ns['capture_heap_audit']()
try:
    allocate(1, 'controlledExtensionExhaustion')
except RuntimeError as error:
    extension_error = str(error)
else:
    raise AssertionError('Full extension unexpectedly allocated')
assert ns['capture_heap_audit']() == final_audit
assert final_audit['extension']['primaryHighWaterExclusive'] == hex(primary_before_spill)
assert final_audit['extension']['primaryTailBytes'] == 16
assert final_audit['remainingBytes'] == 0
assert final_audit['extension']['remainingExtensionBytes'] == 0
assert host.counts['HeapAlloc'] > 0 and host.counts['HeapReAlloc'] > 0 and host.counts['HeapFree'] > 0
fitting_records(final_audit)
evidence.append({'case': 'bounded-spill-crt-realloc-free', 'oversizedError': oversized_error,
                 'extensionError': extension_error, 'primaryHighWaterBeforeSpill': hex(primary_before_spill),
                 'reallocatedPointer': hex(reallocated), 'nativeAllocationAfterFree': hex(next_pointer),
                 'copied32BytesAndZeroFilledTail': True, 'audit': final_audit,
                 'crtHost': host.metadata()})
del ns, host
gc.collect()

# Validation rejects malformed CLI capacities before any emulated execution.
for capacity in ['-1', '257']:
    try:
        run(SOURCE, ['--host-heap-extra-mib', capacity], True)
    except SystemExit as error:
        assert str(error) == 'Host heap extension must be between0 and256MiB'
    else:
        raise AssertionError('Invalid host extension size was accepted')

traces = {}
common = ['--frames', '60', '--native-player', '--native-items', '--native-bullets', '--native-lasers',
          '--native-camera-effects', '--active-collision', '--capture-text-output',
          '--instruction-budget', '20000000', '--input', str(INPUT)]
for name, source, extra in [('before', BEFORE.read_text(encoding='utf-8'), []),
                            ('default0', SOURCE, []), ('enabled1', SOURCE, ['--host-heap-extra-mib', '1'])]:
    output = OUT / (name + '-native60.json')
    ns = run(source, [*common, *extra, '--output', str(output)])
    traces[name] = json.loads(output.read_text(encoding='utf-8'))
    del ns
    gc.collect()
assert traces['before'] == traces['default0'], 'Default0 changed original prefix or metadata'
baseline = traces['before']
extension = traces['enabled1']
for key in set(baseline) | set(extension):
    if key != 'hostHeapAudit':
        assert baseline.get(key) == extension.get(key), ('Prefix field changed with unused extension', key)
for key, value in baseline['hostHeapAudit'].items():
    if key != 'regions':
        assert value == extension['hostHeapAudit'][key], ('Host prefix changed', key)
for key, value in baseline['hostHeapAudit']['regions'].items():
    assert value == extension['hostHeapAudit']['regions'][key]
assert not extension['hostHeapAudit']['extension']['spillUsed']
assert baseline['completedFrames'] == extension['completedFrames'] == 60
assert baseline['partial'] and extension['partial'] and not baseline['wholeGameOracle'] and not extension['wholeGameOracle']
fitting_records(extension['hostHeapAudit'])
result = {'schema': 'th12-host-heap-extension-evidence/1', 'originalExeSha256': baseline['originalExeSha256'],
          'providerBeforeSha256': hashlib.sha256(BEFORE.read_bytes()).hexdigest(),
          'providerAfterSha256': hashlib.sha256(PROVIDER.read_bytes()).hexdigest(),
          'scope': 'Controlled host-only capacity/alignment/realloc/free failure checks plus actual60 '
                   'selected-manager original replay input ticks. Old/default0 captures are completely '
                   'equal. An unused1MiB extension changes only its declared host-region metadata. '
                   'No production change, replay input change, accepted golden or whole-game oracle.',
          'controlled': evidence,
          'nativePrefix': {'stage': baseline['nativeStageResources']['number'],
                           'completedFrames': 60, 'allOldAndDefaultFieldsEqual': True,
                           'allBehaviorAndOriginalHostAddressesWithExtensionEqual': True,
                           'spillUsed': False, 'traceFiles': {name: str(OUT / (name + '-native60.json')) for name in traces}}}
output = OUT / 'evidence.json'
output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'controlledCases': len(evidence),
                  'actualPrefixCases': 3, 'completedNativeFrames': 180,
                  'allOldDefaultAndExtensionBehaviorEqual': True}))
