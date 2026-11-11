"""Execute TH04's guest F5 timeout handling with native stage state."""
from pathlib import Path
import os
import struct
import subprocess
import tempfile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
from unicorn.x86_const import UC_X86_REG_CS, UC_X86_REG_DS, UC_X86_REG_SP, UC_X86_REG_SS

ROOT = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as tmp:
    src, out = Path(tmp)/'time.asm', Path(tmp)/'time.bin'
    src.write_text('bits 16\ndw practice_time_lock,practice_assists\npractice_assists: db 0\n'
                   f'%include "{(ROOT / "patches/practice-time.inc").as_posix()}"\n', encoding='ascii')
    subprocess.run([os.environ.get('NASM', 'nasm'), '-f', 'bin', str(src), '-o', str(out)], check=True)
    code = out.read_bytes()
entry, flag = struct.unpack_from('<HH', code)

def run(stage, phase, *, boss=0x1234, mid=0, extra=None, character=0,
        counter=None, counter_value=0, timer=0, enabled=True, phase_mid=None, state=None):
    u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000)
    u.mem_write(0x1000,code);u.mem_write(0x18000,b'\x00\x20')
    data=0x40000
    def w8(at,value):u.mem_write(data+at,bytes([value]))
    def w16(at,value):u.mem_write(data+at,struct.pack('<H',value))
    u.mem_write(0x1000+flag,bytes([16 if enabled else 0]))
    w16(0xbcd0,extra if extra is not None else boss);w16(0x46b6,mid)
    w8(0x5394,stage);w8(0x53d9,phase);w8(0x5398,character)
    w8(0x53c5,phase if phase_mid is None else phase_mid);w16(0x53da,timer)
    if counter is not None:w8(counter,counter_value)
    for at,value in (state or {}).items():w8(at,value)
    for reg,value in ((UC_X86_REG_CS,0x100),(UC_X86_REG_DS,0x4000),(UC_X86_REG_SS,0x1800),(UC_X86_REG_SP,0)):
        u.reg_write(reg,value)
    u.emu_start(0x1000+entry,0x3000,count=2000)
    return (struct.unpack('<H',u.mem_read(data+0x53da,2))[0],
            u.mem_read(data+counter,1)[0] if counter is not None else None)

cases=[
    (dict(stage=0,phase=3,timer=1499), (0,None)),
    (dict(stage=0,phase=3,timer=1498), (1498,None)),
    (dict(stage=0,phase=2,counter=0xbced,counter_value=14), (0,0)),
    (dict(stage=0,phase=2,counter=0xbced,counter_value=13), (0,13)),
    (dict(stage=2,phase=3,counter=0x53df,counter_value=14,state={0x46e4:1}), (0,0)),
    (dict(stage=3,phase=11,character=1,timer=998), (0,None)),
    (dict(stage=6,phase=2,counter=0x46ae,counter_value=34), (0,0)),
    (dict(stage=6,phase=2,extra=0xc7da,counter=0x53df,counter_value=20), (0,0)),
    (dict(stage=1,phase=1,boss=0,mid=0x1234,counter=0x4274,counter_value=15), (0,0)),
    (dict(stage=0,phase=3,boss=0,mid=0x1234,counter=0x4277,counter_value=3), (0,2)),
    (dict(stage=0,phase=3,timer=1499,enabled=False), (1499,None)),
    (dict(stage=0,phase=3,timer=1499,boss=0), (1499,None)),
]
for args,expected in cases:
    actual=run(**args)
    assert actual==expected,(args,actual,expected)
print(f'TH04 native time lock: {len(cases)} cases passed')
