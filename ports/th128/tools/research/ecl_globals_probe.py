import argparse
import functools
import hashlib
import math
import struct
import sys
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from th128_assets.formats import require
from th128_assets.pipeline import PROJECT,atomic_json
from research.rng_probe import EXPECTED_EXE,step32


FIELDS={-9997:0x11FC,-9996:0x1200,-9995:0x1230,-9994:0x1234,-9993:0x1264,-9992:0x1268,-9988:0x143C}


@functools.lru_cache(maxsize=1)
def mapped_original(filename):
    import pefile
    with Path(filename).open('rb') as source:data=source.read(16*1024*1024+1)
    require(hashlib.sha256(data).hexdigest()==EXPECTED_EXE,'global resolver addresses require fixed Japanese executable')
    image=pefile.PE(data=data)
    mapped=image.get_memory_mapped_image()
    image.close()
    return mapped


def sample_float_global(executable,identifier,seed,fields=None,player=(0.0,400.0),difficulty=1):
    from unicorn import Uc,UcError,UC_ARCH_X86,UC_MODE_32,UC_PROT_READ,UC_PROT_EXEC
    from unicorn.x86_const import UC_X86_REG_ECX,UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_FPCW
    require(identifier in {*FIELDS,-10000,-9999,-9998,-9987,-9991,-9990,-9959,-9965,-9964},'unsupported probe global')
    require(type(seed) is int and 0<=seed<=65535,'invalid RNG seed')
    memory=mapped_original(str(executable.resolve()))
    emulator=Uc(UC_ARCH_X86,UC_MODE_32)
    for address,size in [(0x41D000,4096),(0x46A000,4096),(0x4A6000,4096)]:
        emulator.mem_map(address,size)
        emulator.mem_write(address,memory[address-0x400000:address-0x400000+size])
        emulator.mem_protect(address,size,UC_PROT_READ|UC_PROT_EXEC if address<0x4A6000 else UC_PROT_READ)
    for address,size in [(0x4B4000,4096),(0x4B8000,4096),(0x4D2000,8192),(0x300000,0x6000),(0x380000,8192),(0x100000,65536),(0x200000,4096)]:
        emulator.mem_map(address,size)
    emulator.mem_write(0x4B8A80,struct.pack('<I',0x380000))
    emulator.mem_write(0x380ECC,struct.pack('<2f',*player))
    emulator.mem_write(0x4B4D0C,struct.pack('<i',difficulty))
    emulator.mem_write(0x4D2AE0,struct.pack('<HHI',seed,0,0))
    for offset,value in (fields or {}).items():
        require(offset in FIELDS.values() and math.isfinite(value),'unsupported entity fixture')
        emulator.mem_write(0x300000+offset,struct.pack('<f',value))
    emulator.mem_map(0x210000,4096)
    emulator.mem_write(0x200000,b'\xd9\x1d'+struct.pack('<I',0x210000))
    emulator.mem_protect(0x200000,4096,UC_PROT_READ|UC_PROT_EXEC)
    emulator.mem_write(0x108000,struct.pack('<Ii',0x200000,identifier))
    emulator.reg_write(UC_X86_REG_ECX,0x300000)
    emulator.reg_write(UC_X86_REG_ESP,0x108000)
    emulator.reg_write(UC_X86_REG_FPCW,0x37F)
    try:emulator.emu_start(0x41D2C0,0x200006,count=1024,timeout=100000)
    except UcError as error:raise RuntimeError(f'global probe stopped: {error}') from error
    require(emulator.reg_read(UC_X86_REG_EIP)==0x200006,'global resolver instruction budget')
    raw=bytes(emulator.mem_read(0x210000,4))
    final_seed,padding,calls=struct.unpack('<HHI',emulator.mem_read(0x4D2AE0,8))
    return {'identifier':identifier,'value':struct.unpack('<f',raw)[0],'bits':raw.hex(),'seed':final_seed,'calls':calls}


def main(argv=None):
    parser=argparse.ArgumentParser(description='Bounded original ECL global getter samples; no game process.')
    parser.add_argument('--exe',type=Path,required=True)
    arguments=parser.parse_args(argv)
    records=[]
    for seed in [0,1,0x1234,32768,65535]:
        result,next_seed,calls=step32(seed,0)
        for identifier in [-10000,-9999,-9987,-9998]:
            record=sample_float_global(arguments.exe,identifier,seed)
            unit=struct.unpack('<f',struct.pack('<f',result/4294967296))[0]
            signed=struct.unpack('<f',struct.pack('<f',result/2147483648-1))[0]
            expected=result&0x7FFFFFFF if identifier==-10000 else unit if identifier==-9999 else signed if identifier==-9987 else signed*3.1415927410125732
            bits=struct.pack('<f',expected).hex()
            require(record['bits']==bits and record['seed']==next_seed and record['calls']==calls,'native global RNG mismatch')
            records.append({'initialSeed':seed,**record})
    for identifier,offset in FIELDS.items():
        value=-17.25+len(records)
        record=sample_float_global(arguments.exe,identifier,1234,{offset:value})
        require(record['value']==value and record['calls']==0,'native field getter mismatch')
        records.append({'fieldOffset':hex(offset),**record})
    report={'schema':'th128-original-ecl-globals/1','exeSha256':EXPECTED_EXE,'floatResolver':'0x41d2c0',
            'floatDispatch':'0x41daa8','emulatedCases':records,'windowsGameProcessLaunched':False,
            'limitations':['Original-byte emulation, not native CPU execution.','Only listed float getters and RNG paths are verified; other globals remain unsupported.',
                           'Post-return fstp instrumentation records the float result; it is not original code.']}
    destination=PROJECT/'local/research/ecl-globals.json'
    require(destination.parent.resolve()==destination.parent.absolute(),'research directory redirected')
    atomic_json(destination,report)
    print(f'{len(records)} original-byte ECL global resolver cases verified.')


if __name__=='__main__':main()
