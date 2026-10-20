"""Actual ECL419 observes the MSG opcode12 pulse before HUD25 clears it.

Original Enemy/ECL, MSG constructor/interpreter and HUD25 execute with literal
synthetic SCPT/MSG files. Host-only startup boundaries are explicitly inherited;
no ECL return, message timer, pulse, instruction or RNG is replaced.
"""
import json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('SPAWN=0x2070000;')[0]
def word(n): return struct.pack('<i', n)
def ecl(op, args=()):
    raw = b''.join(args)
    return struct.pack('<iHHHBBI', 0, op, len(raw)+16, 0, 63, len(args), 0)+raw
def msg(time, op, payload=b''): return struct.pack('<HBB', time, op, len(payload))+payload
records = []
for name in ['no-message', 'active-message-pulse', 'message-completes-without-pulse']:
    saved = sys.argv
    sys.argv = [str(PROVIDER), '--frames', '0', '--native-player', '--native-items',
        '--native-bullets', '--native-lasers', '--active-collision', '--instruction-budget', '20000000',
        '--input', str(ROOT / 'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json'),
        '--output', str(ROOT / 'artifacts/reverse/ecl-message-wait-bootstrap.json')]
    ns = {'__file__': str(PROVIDER), '__name__': 'original_ecl_message_wait'}
    try: exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
    finally: sys.argv = saved
    from unicorn.x86_const import UC_X86_REG_EDI
    mu, put, get, call = (ns[k] for k in ['v', 'put', 'get', 'call'])
    code = ([ecl(418, [word(0)])] if name != 'no-message' else [])
    code += [ecl(419), ecl(415, [word(77)]), ecl(419), ecl(415, [word(88)]),
             ecl(83, [word(100000)]), ecl(10)]
    header = 52
    raw = bytearray(header+16+sum(map(len, code)))
    raw[:4] = b'SCPT'; struct.pack_into('<H', raw, 4, 1); struct.pack_into('<H', raw, 16, 1)
    struct.pack_into('<I', raw, 36, header); raw[40:49] = b'WaitProbe'; raw[49] = 0
    raw[header:header+4] = b'ECLH'; raw[header+16:] = b''.join(code)
    commands = [msg(0, 10, b'\0')]
    if name == 'active-message-pulse': commands += [msg(1, 12), msg(2, 11, word(4))]
    else: commands += [msg(1, 11, word(4))]
    commands += [msg(7, 0)]
    message = struct.pack('<III', 1, 12, 256)+b''.join(commands)
    message_base, ecl_base = 0x43f0000, 0x4300000
    mu.mem_write(message_base, message); put(ns['stage_manager']+0x6d34, 'I', message_base)
    mu.mem_write(ecl_base, bytes(raw)); ns['routines']['WaitProbe'] = (ecl_base+40, ecl_base+header)
    put(ns['PROGRAM']+8, 'I', len(ns['routines']))
    for index, (_, (text, routine)) in enumerate(sorted(ns['routines'].items())):
        put(ns['PROGRAM']+0x100+index*8, 'II', text, routine)
    request = 0x2070000; put(request, 'fffiiiII', 0, 0, 0, 0, 0, 10000, 0, 0)
    enemy = call(0x412990, ecl_base+40, request)
    def capture(tick):
        context = get(enemy+4, 'I')[0]
        message_pointer = get(ns['stage_manager']+0x6d30, 'I')[0]
        result = {'inputTick': tick, 'eclByteOffset': get(context+4, 'I')[0]-ecl_base-header-16,
                  'eclTime': get(context, 'f')[0], 'messageActive': bool(message_pointer)}
        if message_pointer:
            result.update({'messageClock': get(message_pointer+0x1c, 'i')[0],
                           'messageWait': get(message_pointer+0x30, 'i')[0],
                           'messageByteOffset': get(message_pointer+0x64, 'I')[0]-message_base,
                           'gate8c': call(0x412720)})
        return result
    frames = [capture(-1)]
    for tick in range(14):
        put(0x4d49d0, 'I', 0); put(0x4d49dc, 'I', 0)
        call(0x413190, ns['MANAGER'])
        mu.reg_write(UC_X86_REG_EDI, ns['stage_manager']); call(0x41e020)
        frames.append(capture(tick))
    records.append({'name': name, 'syntheticEclHex': bytes(raw).hex(),
                    'syntheticMsgHex': message.hex(), 'frames': frames})
output = ROOT / 'tests/fixtures/ecl-message-wait-original.json'
output.write_text(json.dumps({'schema': 'th12-original-ecl-message-wait/1',
    'originalExeSha256': ns['SHA'], 'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Actual Enemy constructor/Enemy18, ECL418/419, MSG41fbe0/41f900/41fcc0, '
             '412720 and HUD25(41e020). Synthetic literal SCPT/MSG; constructed full selected-manager startup '
             'and resource/heap/audio host boundaries. No Player/Bomb/ANM scheduler update or GPU execution.',
    'records': records}, indent=2)+'\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': len(records),
    'pulse': [f for r in records for f in r['frames'] if f.get('gate8c')]}))
