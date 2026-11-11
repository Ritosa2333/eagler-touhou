"""Run the actual TSR's INT 60h wrapper with a stub PMD driver in Unicorn."""
import json, struct, sys
from pathlib import Path
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16, UC_HOOK_CODE
from unicorn.x86_const import *

ROOT=Path(__file__).resolve().parents[1]
network='--network-audio' in sys.argv
meta=json.loads((ROOT/'web/network-audio.json').read_text()) if network else json.loads((ROOT/'web/native-patch.json').read_text())['startup']['music']
code=(ROOT/('web/native/network-audio.com' if network else 'web/native/music.com')).read_bytes()
CS,SS=0x2000,0x7000
mail=CS*16+meta['mailbox_com_offset']
entry=0x100+code.index(b'PMD')-2
uc=Uc(UC_ARCH_X86,UC_MODE_16);uc.mem_map(0,0x100000)
uc.mem_write(CS*16+0x100,code)
uc.mem_write(mail-10,struct.pack('<HH',0,0x5000))
uc.mem_write(0x50000,b'\xcf')  # PMD's IRET
header=bytes(range(25));uc.mem_write(0x60100,header)
calls=[]
def driver(uc,address,size,user):
    if address!=0x50000:return
    ax=uc.reg_read(UC_X86_REG_AX);calls.append(ax)
    if ax>>8==6:
        uc.reg_write(UC_X86_REG_DS,0x6000);uc.reg_write(UC_X86_REG_DX,0x100)
uc.hook_add(UC_HOOK_CODE,driver,begin=0x50000,end=0x50000)
def command(ax):
    for reg,value in [(UC_X86_REG_CS,CS),(UC_X86_REG_SS,SS),(UC_X86_REG_SP,0xff00),
                      (UC_X86_REG_DS,0x3000),(UC_X86_REG_ES,0x4000),(UC_X86_REG_EBX,0x12345678),
                      (UC_X86_REG_ECX,0x76543210),(UC_X86_REG_EDX,0x34567890),
                      (UC_X86_REG_ESI,0x98765432),(UC_X86_REG_EDI,0xabcdef01),(UC_X86_REG_AX,ax)]:
        uc.reg_write(reg,value)
    uc.mem_write(SS*16+0xff00,struct.pack('<HHH',0x8000,0x1000,0x202))
    calls.clear();uc.emu_start(CS*16+entry,0x18000,count=10000)
    assert uc.reg_read(UC_X86_REG_SP)==0xff06
    for reg,value in [(UC_X86_REG_DS,0x3000),(UC_X86_REG_ES,0x4000),(UC_X86_REG_EBX,0x12345678),
                      (UC_X86_REG_ECX,0x76543210),(UC_X86_REG_EDX,0x34567890),
                      (UC_X86_REG_ESI,0x98765432),(UC_X86_REG_EDI,0xabcdef01)]:
        assert uc.reg_read(reg)==value,(reg,uc.reg_read(reg),value)
    return calls[:]
assert command(0)==[0x600,0]+[0x1e00+i for i in range(15)]
signature=0x811c9dc5
for byte in header:signature=((signature^byte)*0x01000193)&0xffffffff
assert struct.unpack('<HHI',uc.mem_read(mail+20,8))==(1,0,signature)
for i,ax in enumerate([0x202,0x19ff,0x100],2):
    assert command(ax)==[ax]
    assert struct.unpack('<HHI',uc.mem_read(mail+20+(i-1)*8,8))==(i,ax,signature)
before=uc.mem_read(mail+18,2)
assert command(0x1003)==[0x1003]  # work-area query, forwarded without an event
assert command(0x500)==[0x500]    # query forwarded without music event
assert uc.mem_read(mail+18,2)==before
for effect in range(1,23):
    previous=struct.unpack('<H',uc.mem_read(mail+18,2))[0]
    ax=0x0c00|effect
    assert command(ax)==[ax]  # actual PMD_SE_PLAY, forwarded unchanged
    latest=struct.unpack('<H',uc.mem_read(mail+18,2))[0]
    assert latest==(previous+int(network))&65535
    if network:
        assert struct.unpack('<HHI',uc.mem_read(mail+20+(previous&63)*8,8))==(latest,ax,signature)
if network:
    uc.mem_write(mail+18,struct.pack('<H',65535))
    assert command(0x0c16)==[0x0c16]
    assert struct.unpack('<HHI',uc.mem_read(mail+20+63*8,8))==(0,0x0c16,signature)
    assert uc.mem_read(mail+18,2)==b'\0\0'
print('PASS:', 'network BGM+22 SE events and sequence wrap;' if network else 'original music-only bridge;', 'original PMD forwarding/masking, queries and register preservation')
