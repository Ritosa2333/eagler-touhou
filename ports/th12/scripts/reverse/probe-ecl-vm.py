"""Explicit original 467170 VM arithmetic/typed-stack sampling, not game golden."""
import pathlib,sys,hashlib,struct,json
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EAX,UC_X86_REG_EIP
exe=TITLE.parent/'th12/th12.exe';digest=hashlib.sha256(exe.read_bytes()).hexdigest()
assert digest=='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
def i(v):return struct.pack('<I',v&0xffffffff)
def f(v):return struct.pack('<f',v)
def ins(op,args=(),refs=0):
    if op in (43,45,86,87,89):refs|=1
    b=bytearray(16+len(args)*4);struct.pack_into('<iHHHBBI',b,0,0,op,len(b),refs,63,len(args),0)
    for j,v in enumerate(args):b[16+j*4:20+j*4]=v
    return bytes(b)
cases=[
 ('int-overflow-add',[ins(42,[i(0x7fffffff)]),ins(42,[i(1)]),ins(50),ins(43,[i(0)])]),
 ('int-exact-large',[ins(42,[i(0x71234567)]),ins(43,[i(0)])]),
 ('int-to-float-store',[ins(42,[i(16777217)]),ins(45,[f(0)])]),
 ('float-to-int-store',[ins(44,[f(-3.75)]),ins(43,[i(0)])]),
 ('int-negate',[ins(42,[i(123456789)]),ins(84),ins(43,[i(0)])]),
 ('float-negate',[ins(44,[f(3.5)]),ins(85),ins(45,[f(0)])]),
 ('norm-squared',[ins(86,[f(0),f(3),f(4)])]),
 ('angle-between',[ins(87,[f(0),f(10),f(20),f(30),f(40)])]),
 ('normalize',[ins(44,[f(10)]),ins(45,[f(0)]),ins(82,[f(0)],1)]),
 ('angle-difference',[ins(89,[f(0),f(3),f(-3)])]),
]
rows=[]
for name,instructions in cases:
    mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(base,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(base,pe.get_memory_mapped_image())
    STACK,CTX,CODE,STOP=0x1000000,0x2000000,0x3000000,0x100f000
    for address in (STACK,CTX,CODE):mu.mem_map(address,0x10000)
    mu.mem_write(CODE,b''.join(instructions)+ins(83,[i(10000)])+ins(10))
    mu.mem_write(CTX,struct.pack('<fI',0,CODE));mu.mem_write(CTX+0x1008,i(256));mu.mem_write(CTX+0x101c,i(63))
    sp=STACK+0xe000;mu.mem_write(sp,i(STOP));mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_EAX,CTX)
    mu.emu_start(0x467170,STOP,count=100000)
    if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('VM instruction budget exceeded')
    raw=bytes(mu.mem_read(CTX+8,4));value=struct.unpack('<f',raw)[0]
    rows.append({'case':name,'storedBits':raw.hex(),'storedInt':struct.unpack('<i',raw)[0],
                 'storedFloat':value if abs(value)<float('inf') else str(value),
                 'programHex':(b''.join(instructions)+ins(83,[i(10000)])+ins(10)).hex()})
out=TITLE/'artifacts/reverse/ecl/vm-probe.json';out.parent.mkdir(parents=True,exist_ok=True)
out.write_text(json.dumps({'exeSha256':digest,'entry':'0x467170','scope':'typed VM operations only',
                          'externalCallsIntercepted':False,'wholeGameOracle':False,'cases':rows},indent=2)+'\n')
print(json.dumps(rows,indent=2))
