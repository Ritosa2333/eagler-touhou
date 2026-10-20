"""Observe original ECL453 operand dispatch and Enemy property writes.

Only the native provider's mapped resource/bootstrap host is reused. Original
Enemy birth, operand reader, property handler and Primary ANM execute. Synthetic
ECL is explicitly added to research memory; no retail inputs are changed.
"""
import json, pathlib, struct, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('if args.routine not in routines:')[0]

def integer(n): return struct.pack('<i', n)
def ins(op, payload=(), time=0, mask=0):
    data = b''.join(payload)
    return struct.pack('<iHHHBBI', time, op, len(data)+16, mask, 63, len(payload), 0) + data
def program(code):
    name = b'EffectProbe\0'; start = (40+len(name)+3)&~3
    body = b'ECLH'+bytes(12)+b''.join(code)
    raw = bytearray(start+len(body)); raw[:4] = b'SCPT'
    struct.pack_into('<H', raw, 4, 1); struct.pack_into('<H', raw, 16, 1)
    struct.pack_into('<I', raw, 36, start); raw[40:40+len(name)] = name
    raw[start:] = body; return bytes(raw), start

rows = []
for value in [None, -1, 0, 1, 37, 87]:
    saved = sys.argv; sys.argv = [str(PROVIDER), '--frames', '1', '--seed', '1234']
    ns = {'__file__': str(PROVIDER), '__name__': 'native_ecl_effect_property'}
    try: exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
    finally: sys.argv = saved
    v, put, get, call = (ns[n] for n in ['v', 'put', 'get', 'call'])
    from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE
    from unicorn.x86_const import UC_X86_REG_EBX, UC_X86_REG_EIP, UC_X86_REG_ESP, UC_X86_REG_EDI
    current = [0]; writes = []; dispatch = []
    def code_hook(mu, address, size, data):
        if address == 0x413840:
            current[0] = get(mu.reg_read(UC_X86_REG_ESP)+4, 'I')[0]
        if address == 0x4154c0:
            state = mu.reg_read(UC_X86_REG_EBX)
            context = get(current[0]+0x174c, 'I')[0]
            command = get(context+4, 'I')[0]
            dispatch.append({'entry': hex(address), 'state': hex(state),
                             'opcode': get(command+4, 'H')[0]})
    def write_hook(mu, access, address, size, value_written, data):
        if current[0] and current[0]+0x1600 <= address < current[0]+0x1740:
            writes.append({'pc': hex(mu.reg_read(UC_X86_REG_EIP)),
                          'offset': hex(address-current[0]), 'size': size,
                          'value': value_written})
    v.hook_add(UC_HOOK_CODE, code_hook, begin=0x413840, end=0x413840)
    v.hook_add(UC_HOOK_CODE, code_hook, begin=0x4154c0, end=0x4154c0)
    v.hook_add(UC_HOOK_MEM_WRITE, write_hook)
    code = [ins(258, [integer(2)]), ins(259, [integer(0), integer(7)])]
    if value is not None:
        code.extend([ins(453, [integer(value)]), ins(449, [integer(value)])])
    code.append(ins(83, [integer(100000)]))
    base = 0x4300000; raw, header = program(code)
    v.mem_write(base, raw); ns['routines']['EffectProbe'] = (base+40, base+header)
    put(ns['PROGRAM']+8, 'I', len(ns['routines']))
    for index, (_, (text, routine)) in enumerate(sorted(ns['routines'].items())):
        put(ns['PROGRAM']+0x100+index*8, 'II', text, routine)
    request = 0x2070000; put(request, 'fffiiiII', 25, 100, 0, 0, 0, 100, 0, 0)
    enemy = call(0x412990, base+40, request); state = enemy+0x1040
    before = list(get(state+0x1660, '28i'))
    # First manager encounter only clears the creation guard. The next one
    # runs the original waiting context and property persists normally.
    call(0x413190); call(0x413190)
    rows.append({'value': value, 'syntheticEclHex': raw.hex(),
                 'dispatch': dispatch, 'writes': writes, 'propertiesAt1660': before,
                 'propertiesAfterTwoManagerTicks': list(get(state+0x1660, '28i')),
                 'finalFlags': get(state+0x16b8,'I')[0],
                 'rng': list(get(0x4ce568, 'II'))})

target = ROOT/'artifacts/reverse/ecl-effect-property-observation.json'
target.write_text(json.dumps({'schema': 'th12-ecl-effect-property-observation/1',
                             'originalExeSha256': ns['SHA'], 'wholeGameOracle': False,
                             'cases': rows}, indent=2)+'\n', encoding='utf-8')
fixture = ROOT/'tests/fixtures/enemy-hit-properties-original.json'
fixture.write_text(json.dumps({'schema':'th12-original-enemy-hit-properties/1',
    'originalExeSha256':ns['SHA'],'wholeGameOracle':False,
    'scope':'Actual Enemy construction and ECL453/449 literal operands with real ANM, followed by two actual Enemy18 passes. Property values come from the original EnemyState, not candidate code.',
    'cases':[{'value':r['value'],'hitSound':r['propertiesAt1660'][11],
              'tinted':bool(r['finalFlags']&0x40000000),'rng':r['rng']}
             for r in rows]},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'output': str(target), 'fixture':str(fixture),'cases':len(rows)}))
