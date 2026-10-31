"""Fresh synthetic web exports read by the pinned TH12 1.00b machine code.

Runs actual463af0 encryption, actual4639d0 decryption, actual44c710 LZ
inflation, and the complete actual43c350 loader. File reads, the CRT
allocator/memcpy/free and the stack-cookie check are host boundaries. No
original replay, graphics, audio, or full gameplay is executed or changed.
The fixture stores hashes and decoded synthetic records, never retail bytes.
"""
import argparse, base64, hashlib, json, pathlib, shutil, struct, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_EDI, UC_X86_REG_EIP, UC_X86_REG_ESP, UC_X86_REG_ESI
from native_anm_resource import check_original, EXE_SHA256

EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
sha = lambda value: hashlib.sha256(value).hexdigest()

# JS runs the production encoder. The test repeats this exact fresh input.
NODE_SOURCE = """
import {encodeReplay, defaultReplaySpellTimes, defaultReplayConfig} from './src/replay-recorder.mjs';
const initial = {score:1234567,power:143,pointValue:12345000,lives:6,lifeFragments:4,bombs:2,bombFragments:2,ufoColors:[1,2,0],rank:42,x:768,y:49920,nativeGlobals:[314,3,1,9876]};
function stage(number,frames,end){let previous=0;const inputs=Array.from({length:frames},(_,i)=>{const held=i%17===0?0:0x81|(i%5===0?0x40:0);const result={held,pressed:held&~previous,released:previous&~held};previous=held;return result;});return {number,seed:0x3456+number,initial:{...initial,score:initial.score+number},spellTimes:defaultReplaySpellTimes(number===7),inputs,fps:Array.from({length:Math.ceil(frames/30)},(_,i)=>60-i),end};}
const common={name:'TESTUSER',character:2,shot:1,difficulty:3,timestamp:1770000000,scoreStored:7654321,slowdown:1.25,continues:0};
const cases=[['single100-ended',{...common,stages:[stage(1,100,true)]}],['seven-stages',{...common,stages:Array.from({length:7},(_,i)=>stage(i+1,i===0?31:4,i===6)),cleared:true}],['extra31-practice',{...common,stages:[stage(7,31,true)],practice:true,cleared:true,includeUserBlocks:false}]];
console.log(JSON.stringify({configHex:Buffer.from(defaultReplayConfig()).toString('hex'),spellTimes:defaultReplaySpellTimes(),cases:cases.map(([name,options])=>({name,options,file:Buffer.from(encodeReplay(options)).toString('base64')}))}));
"""


class Native:
    def __init__(self, file=b''):
        self.mu = Uc(UC_ARCH_X86, UC_MODE_32)
        self.mu.mem_map(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)
        self.mu.mem_write(0x400000, PE.get_memory_mapped_image())
        self.mu.mem_map(0x1000000, 0x10000)
        self.mu.mem_map(0x2000000, 0x1000000)
        self.stack, self.stop, self.manager = 0x100e000, 0x100f000, 0x2000000
        self.heap, self.file, self.file_cursor = 0x2100000, bytes(file), 0
        self.entered = set()
        self.mu.hook_add(UC_HOOK_CODE, self.hook)

    def put(self, address, fmt, *values):
        self.mu.mem_write(address, struct.pack('<' + fmt, *values))

    def read(self, address, fmt):
        return struct.unpack('<' + fmt, self.mu.mem_read(address, struct.calcsize('<' + fmt)))

    def alloc(self, size):
        address = self.heap
        self.heap += max(16, (size + 15) & ~15)
        if self.heap >= 0x3000000:
            raise RuntimeError('Synthetic probe heap exhausted')
        return address

    def return_to_caller(self, eax=None, args=0):
        sp = self.mu.reg_read(UC_X86_REG_ESP)
        if eax is not None:
            self.mu.reg_write(UC_X86_REG_EAX, eax)
        self.mu.reg_write(UC_X86_REG_EIP, self.read(sp, 'I')[0])
        self.mu.reg_write(UC_X86_REG_ESP, sp + 4 + args)

    def hook(self, mu, address, size, data):
        if address in (0x43c350, 0x463af0, 0x4639d0, 0x44c710):
            self.entered.add(address)
        sp = mu.reg_read(UC_X86_REG_ESP)
        if address in (0x46d04a, 0x46c9ea):
            self.return_to_caller(self.alloc(self.read(sp + 4, 'I')[0]))
        elif address == 0x47a330:
            dst, src, length = self.read(sp + 4, '3I')
            if length:
                mu.mem_write(dst, bytes(mu.mem_read(src, length)))
            self.return_to_caller(dst)
        elif address == 0x477420:
            dst, value, length = self.read(sp + 4, '3I')
            if length:
                mu.mem_write(dst, bytes([value & 255]) * length)
            self.return_to_caller(dst)
        elif address in (0x46c941, 0x46c932, 0x4641e0):
            self.return_to_caller()
        elif address == 0x46cbbb:
            # Only filename sprintf from43c350; actual disk IO is excluded.
            dst = self.read(sp + 4, 'I')[0]
            mu.mem_write(dst, b'replay/synthetic.rpy\0')
            self.return_to_caller(len('replay/synthetic.rpy'))
        elif address == 0x463df0:
            self.return_to_caller(1, 4)
        elif address == 0x464070:
            self.return_to_caller(0)
        elif address == 0x464190:
            length = mu.reg_read(UC_X86_REG_EDI)
            data = self.file[self.file_cursor:self.file_cursor + length]
            if len(data) != length:
                raise RuntimeError('Native loader requested bytes outside synthetic file')
            address = self.alloc(length)
            mu.mem_write(address, data)
            self.file_cursor += length
            self.return_to_caller(address)

    def call(self, address, args=(), eax=0, esi=0):
        self.put(self.stack, 'I' * (len(args) + 1), self.stop, *args)
        self.mu.reg_write(UC_X86_REG_ESP, self.stack)
        self.mu.reg_write(UC_X86_REG_EAX, eax)
        self.mu.reg_write(UC_X86_REG_ESI, esi)
        self.mu.emu_start(address, self.stop, count=5000000)
        if self.mu.reg_read(UC_X86_REG_EIP) != self.stop:
            raise RuntimeError(f'Native{address:x} exhausted instruction budget')
        return self.mu.reg_read(UC_X86_REG_EAX)

    def crypto(self, source, key, step, block, address):
        pointer = self.alloc(len(source) + 16)
        if source:
            self.mu.mem_write(pointer, source)
        self.call(address, (pointer, len(source), step, block, len(source)), eax=key)
        return bytes(self.mu.mem_read(pointer, len(source)))

    def load(self):
        filename = 0x2001000
        self.mu.mem_write(filename, b'synthetic.rpy\0')
        result = self.call(0x43c350, (self.manager, filename))
        if result:
            return {'returnCode': result}
        header, raw_address = self.read(self.manager + 0x18, '2I')
        raw_size = self.read(header + 32, 'I')[0]
        raw = bytes(self.mu.mem_read(raw_address, raw_size))
        stages = []
        for number in range(1, 8):
            inp, _, fps, _, record, _ = self.read(self.manager + number * 36 + 0xa8, '6I')
            if not record:
                continue
            frame_count, payload = self.read(record + 4, '2I')
            stages.append({'number': number, 'headerOffset': record - raw_address,
                           'frames': frame_count, 'payload': payload,
                           'inputOffset': inp - raw_address, 'fpsOffset': fps - raw_address,
                           'inputSha256': sha(bytes(self.mu.mem_read(inp, frame_count * 6))),
                           'fps': list(self.mu.mem_read(fps, payload - frame_count * 6)),
                           'firstInput': list(self.read(inp, '3H')),
                           'lastInput': list(self.read(inp + (frame_count - 1) * 6, '3H')),
                           'stageHeaderHex': bytes(self.mu.mem_read(record, 160)).hex()})
        return {'returnCode': result, 'rawSha256': sha(raw), 'rawHex': raw.hex(),
                'bytesRead': self.file_cursor, 'stages': stages,
                'entered': [f'{address:08x}' for address in sorted(self.entered)]}


def main():
    args = argparse.ArgumentParser()
    args.add_argument('--out', type=pathlib.Path, default=ROOT / 'tests/fixtures/replay-export-native.json')
    options = args.parse_args()
    generated = json.loads(subprocess.check_output([shutil.which('node') or 'node', '--input-type=module', '-e', NODE_SOURCE], cwd=ROOT, text=True))
    samples = []
    for sample in generated['cases']:
        file = base64.b64decode(sample['file'])
        native = Native(file)
        loaded = native.load()
        assert loaded['returnCode'] == 0
        assert loaded['entered'] == ['0043c350', '0044c710', '004639d0']
        # A separate direct call proves two real decryptors hand the LZ stream
        # to the real decompressor. The complete loader above registers stages.
        packed_size, raw_size = struct.unpack_from('<2I', file, 28)
        packed = file[36:36 + packed_size]
        decoded = native.crypto(packed, 0x5e, 0xe1, 0x800, 0x4639d0)
        decoded = native.crypto(decoded, 0x7d, 0x3a, 0x40, 0x4639d0)
        src, dst = native.alloc(len(decoded) + 16), native.alloc(raw_size + 16)
        native.mu.mem_write(src, decoded)
        returned = native.call(0x44c710, (src, len(decoded), dst), eax=raw_size)
        assert returned == dst
        assert bytes(native.mu.mem_read(dst, raw_size)).hex() == loaded['rawHex']
        bad_magic, bad_version = bytearray(file), bytearray(file)
        bad_magic[0] ^= 1
        bad_version[4] = 3
        loaded.pop('rawHex')
        samples.append({'name': sample['name'], 'options': sample['options'],
                        'fileSize': len(file), 'fileSha256': sha(file),
                        'fileHeaderHex': file[:36].hex(), 'compressedSha256': sha(decoded),
                        'native': loaded, 'badSignatureReturn': Native(bad_magic).load()['returnCode'],
                        'badVersionReturn': Native(bad_version).load()['returnCode']})
    crypto_samples = []
    for block, key, step in [(0x40, 0x7d, 0x3a), (0x800, 0x5e, 0xe1)]:
        for length in sorted(set([0, 1, 15, 16, 17, 31, 32, 63, 64, 65, block // 4 - 1,
                                 block // 4, block // 4 + 1, block - 1, block, block + 1,
                                 block + block // 4 - 1, block + block // 4, block + block // 4 + 1])):
            raw = bytes((i * 73 + 19) & 255 for i in range(length))
            native = Native()
            encoded = native.crypto(raw, key, step, block, 0x463af0)
            assert native.crypto(encoded, key, step, block, 0x4639d0) == raw
            crypto_samples.append({'length': length, 'block': block, 'key': key, 'step': step,
                                   'sourceSha256': sha(raw), 'encryptedSha256': sha(encoded)})
    native = Native()
    config = native.alloc(60)
    #423250 copies current controller action globals; pin its retail defaults.
    native.put(0x4d49ec, '9H', 3, 2, 7, 8, 0xffff, 0xffff, 0xffff, 0xffff, 1)
    native.call(0x423250, esi=config)
    config_hex = bytes(native.mu.mem_read(config, 60)).hex()
    assert config_hex == generated['configHex']
    spell_times = native.alloc(84)
    native.mu.reg_write(UC_X86_REG_EAX, spell_times)
    native.mu.reg_write(UC_X86_REG_ECX, 0)
    native.mu.reg_write(UC_X86_REG_EDX, 20)
    native.mu.emu_start(0x43b080, 0x43b090, count=1000)
    assert native.mu.reg_read(UC_X86_REG_EIP) == 0x43b090
    default_spell_times = list(native.read(spell_times, '21i'))
    assert default_spell_times == generated['spellTimes']
    fixture = {'schema': 'th12-native-replay-export/1', 'originalExeSha256': EXE_SHA256,
               'originalReplayLoaded': False, 'syntheticExportLoaded': True,
               'wholeGameOracle': False, 'originalFilesModified': False, 'gpuExecuted': False,
               'provider': 'Actual43c350 loader/signature/version/stage pointer registration; actual463af0/4639d0 crypto; actual44c710 LZ; actual423250 config defaults; actual43b080 spell markers.',
               'hostBoundaries': ['File open/read/close and filename sprintf receive synthetic exports in memory.',
                                  'CRT malloc/memcpy/memset/free and stack-cookie check are host callbacks.'],
               'limitations': ['No original replay or user recording is read; samples use 100 or fewer playable frames per stage.',
                               'No full gameplay scheduler, UI playback, audio, GPU, or native43bc10 save-to-disk execution.',
                               'Pure literal LZ is legal but is not expected to reproduce retail compression bytes.'],
               'defaultConfigHex': config_hex, 'defaultSpellTimes': default_spell_times,
               'cryptoSamples': crypto_samples, 'samples': samples}
    options.out.write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf8')
    print(json.dumps({'path': str(options.out), 'loaderSamples': len(samples), 'cryptoSamples': len(crypto_samples)}))


if __name__ == '__main__':
    main()
