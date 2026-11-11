from pathlib import Path
import os, struct, subprocess, tempfile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_16
from unicorn.x86_const import *

root=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as tmp:
    src=Path(tmp)/"menu.asm"; binary=Path(tmp)/"menu.bin"
    src.write_text("bits 16\ndw practice_assists_draw, practice_assists_open, practice_assists, practice_assists_drawn\npractice_assists_open: db 0\npractice_assists: db 0\npractice_text_saved: times 960 db 0\n%include 'practice-assists-draw.inc'\n")
    subprocess.run([os.environ.get("NASM","nasm"),"-f","bin","-I",str(root/"pc98")+"/",str(src),"-o",str(binary)],check=True)
    code=binary.read_bytes(); draw,opened,flags,drawn=struct.unpack_from("<4H",code)
    u=Uc(UC_ARCH_X86,UC_MODE_16);u.mem_map(0,0x100000);base=0x10000;u.mem_write(base,code)
    before=bytes((i*37+11)&255 for i in range(0x4000));u.mem_write(0xa0000,before)
    def run():
        u.reg_write(UC_X86_REG_CS,0x1000);u.reg_write(UC_X86_REG_DS,0x1000);u.reg_write(UC_X86_REG_SS,0x8000);u.reg_write(UC_X86_REG_SP,0xff00)
        u.mem_write(0x8ff00,struct.pack("<H",0xfff0));u.emu_start(base+draw,base+0xfff0,count=10000)
        assert u.reg_read(UC_X86_REG_IP)==0xfff0
    u.mem_write(base+opened,b"\x01");u.mem_write(base+flags,b"\x21");run()
    after=bytes(u.mem_read(0xa0000,0x4000)); changed={j for row in range(6) for cell in range(20) for plane in (0,0x2000) for j in (plane+176+row*160+cell*2,plane+177+row*160+cell*2)}
    assert all(before[i]==after[i] for i in range(len(before)) if i not in changed)
    for row in range(6):
        at=176+row*160; text=after[at:at+40:2].decode("ascii"); color=after[at+0x2000]
        assert (text.startswith("[") and text.rstrip().endswith("]")) == (row in (0,5)),text
        assert color==(0x81 if row in (0,5) else 0xe1)
    u.mem_write(base+opened,b"\0");run();assert bytes(u.mem_read(0xa0000,0x4000))==before
    assert u.mem_read(base+drawn,1)==b"\0"
print("PASS native assist menu bounds, colors and text-plane restoration")
