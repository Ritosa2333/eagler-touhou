import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, EXE_SHA256, datum
from enemy_probe import emulate
from freeze_probe import address


def probe(executable):
    import capstone
    from capstone.x86_const import X86_OP_IMM, X86_OP_MEM, X86_OP_REG

    native = NativeBullet(executable)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    decoder.skipdata = True

    def decoded(start, end):
        location, payload = native.chunk(start, end)
        return list(decoder.disasm(payload, location))

    def registrations(start, end):
        priority, result = None, []
        for instruction in decoded(start, end):
            if not instruction.id:
                continue
            operands = instruction.operands
            if (instruction.mnemonic == 'mov' and operands[0].type == X86_OP_REG
                    and instruction.reg_name(operands[0].reg) == 'ebx' and operands[1].type == X86_OP_IMM):
                priority = operands[1].imm
            if (instruction.mnemonic == 'mov' and operands[0].type == X86_OP_MEM
                    and operands[0].mem.disp == 8 and operands[1].type == X86_OP_IMM):
                result.append({'store': address(instruction.address), 'callback': operands[1].imm, 'priority': priority})
        return result

    def callback_layer(target):
        layer = None
        for instruction in decoded(target, target + 0x100):
            if not instruction.id:
                continue
            operands = instruction.operands
            if (instruction.mnemonic == 'mov' and operands[0].type == X86_OP_REG
                    and instruction.reg_name(operands[0].reg) == 'eax' and operands[1].type == X86_OP_IMM):
                layer = operands[1].imm
            if instruction.mnemonic == 'xor' and instruction.op_str == 'eax, eax':
                layer = 0
            if (instruction.mnemonic == 'call' and operands[0].type == X86_OP_IMM and operands[0].imm == 0x466ce0):
                if layer is None:
                    raise ValueError('original layer callback did not establish EAX')
                return layer
        raise ValueError('original callback does not call native layer dispatcher')

    try:
        layers = []
        for registration in registrations(0x463900, 0x464220):
            if not 0x4666d0 <= registration['callback'] <= 0x466ad0:
                continue
            layer = callback_layer(registration['callback'])
            layers.append({**registration, 'callback': address(registration['callback']), 'layer': layer})
        main = registrations(0x433f80, 0x434220)
        stage_end = next(item for item in main if item['callback'] == 0x433850)
        for target in (0x433a20, 0x433a80):
            registration = next(item for item in main if item['callback'] == target)
            layers.append({**registration, 'callback': address(target), 'layer': callback_layer(target)})
        if len(layers) != 31 or len({item['layer'] for item in layers}) != 31:
            raise ValueError('unexpected original layer registration count')
        binds = decoded(0x435d20, 0x435df9)
        entry_load = next(item for item in binds if item.address == 0x435d46)
        script = next(item for item in binds if item.address == 0x435d62)
        if entry_load.mnemonic != 'mov' or entry_load.operands[1].mem.disp != 0x28:
            raise ValueError('unexpected original text entry2 surface alias')
        if script.mnemonic != 'mov' or script.operands[1].imm != 82:
            raise ValueError('unexpected original Main text script')
        for start, end in ((0x433700, 0x43384a), (0x433850, 0x4338ef), (0x433a20, 0x433a80),
                           (0x465350, 0x465395), (0x465802, 0x46582e)):
            native.chunk(start, end)
        flag_cases = []
        for opcode, start, end, values in ((65, 0x45acb8, 0x45acff, (0, 1, 0x10001, 0x20002, 0x30003, 0xffffffff)),
                                           (66, 0x45aa7f, 0x45aaa7, (0, 3, 6, 7)),
                                           (67, 0x45b67c, 0x45b6c2, (0, 1, 12, 21, 31, 10))):
            table = struct.unpack('<I', native.read(0x45d0d4 + (opcode + 1) * 4, 4))[0]
            if table != start:
                raise ValueError('unexpected original ANM flag dispatch')
            chunks = [native.chunk(start, end), native.chunk(0x459e1a, 0x459e31)]
            if opcode == 67:
                chunks.append(native.chunk(0x45c53f, 0x45c550))
            for value in values:
                state = bytearray(0x4e8)
                struct.pack_into('<I', state, 0x4b0, 6)
                result = emulate(chunks, start,
                    [datum('vm', 0x380000, bytes(state)),
                     datum('command', 0x390000, struct.pack('<HHhHI', opcode, 12, 0, 0, value), writable=False)],
                    registers={'edi': 0x380000, 'esi': 0x390000}, stops=(0x459e1a,), budget=128)
                if opcode == 67 and value == 10:
                    if result['stop'].get('target') != '0x004631a0':
                        raise ValueError(result['stop'])
                elif result['stop'] != {'reason': 'boundary', 'address': '0x00459e1a'}:
                    raise ValueError(result['stop'])
                flags = struct.unpack_from('<I', bytes.fromhex(result['memory']['vm']['after']), 0x4b0)[0]
                result.update(opcode=opcode, value=value, flagsAfter=flags, anchorX=(flags >> 20) & 3,
                              anchorY=(flags >> 22) & 3, mode=(flags >> 24) & 31, blend=(flags >> 5) & 7)
                flag_cases.append(result)
        return {'schema': 'th128-original-draw-pipeline/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'layers': sorted(layers, key=lambda item: item['layer']), 'flagCases': flag_cases,
            'stageSurface': {'captureEndPriority': stage_end['priority'], 'endCallback': address(stage_end['callback']),
                'sourceTextureEntry': entry_load.operands[1].mem.disp // 20,
                'mainScript': script.operands[1].imm, 'compositePriority': next(item['priority'] for item in layers if item['layer'] == 29),
                'compositeCallback': '0x00433a20', 'sourceSurfaceGlobal': '0x004d3014',
                'destinationSurfaceGlobal': '0x004d3018', 'mainVmGlobal': '0x004d3024'},
            'allocationExecuted': False, 'graphicsApiExecuted': False, 'windowsGameProcessLaunched': False,
            'evidenceKind': 'original-byte static callsite recovery, not a rendered/native frame trace',
            'unsupportedLayers': [27, 28], 'code': native.code,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    report = probe(arguments.exe)
    (ROOT / 'local/research/draw-pipeline-fixtures.json').write_text(
        json.dumps(report, indent=2) + '\n', encoding='utf-8', newline='\n')
    print('31 original layer priorities; actual text@R entry2/Main82, target switch12, composite29 at14')
