"""Explicit original-only subsystem Oracle. Never imported by candidate tests.

Executes TH12's original RNG functions and original Replay decrypt function in
an isolated x86 diagnostic process. No retail files or game state are patched.
This is subsystem evidence, not a whole-game Replay determinism proof.
"""
import pathlib,sys,hashlib,json,struct,base64
TITLE=pathlib.Path(__file__).resolve().parents[2];WORKSPACE=TITLE.parent
sys.path.insert(0,str(WORKSPACE/'tools/python'))
import pefile
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32,UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EIP,UC_X86_REG_EAX,UC_X86_REG_ESI
exe=WORKSPACE/'th12/th12.exe';pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase
mu=Uc(UC_ARCH_X86,UC_MODE_32);mu.mem_map(base,(pe.OPTIONAL_HEADER.SizeOfImage+4095)&~4095);mu.mem_write(base,pe.get_memory_mapped_image())
STACK=0x1000000;DATA=0x2000000;TEMP=0x3000000;STOP=0x4000000
for addr,size in [(STACK,0x10000),(DATA,0x200000),(TEMP,0x200000),(STOP,4096)]:mu.mem_map(addr,size)
def ret(value,cleanup=0):
    sp=mu.reg_read(UC_X86_REG_ESP);target=struct.unpack('<I',mu.mem_read(sp,4))[0]
    mu.reg_write(UC_X86_REG_EAX,value);mu.reg_write(UC_X86_REG_ESP,sp+4+cleanup);mu.reg_write(UC_X86_REG_EIP,target)
def hook(uc,address,size,data):
    if address==0x46d04a:ret(TEMP) # fixture allocator; bytes start zeroed
    elif address==0x46c941:ret(0) # release fixture-owned temporary storage
mu.hook_add(UC_HOOK_CODE,hook,begin=0x46c941,end=0x46d04a)
def call(address,args=(),eax=0,esi=DATA):
    sp=STACK+0xf000;mu.mem_write(sp,struct.pack('<'+'I'*(len(args)+1),STOP,*args))
    mu.reg_write(UC_X86_REG_ESP,sp);mu.reg_write(UC_X86_REG_EAX,eax);mu.reg_write(UC_X86_REG_ESI,esi)
    mu.emu_start(address,STOP,count=10000000)
    if mu.reg_read(UC_X86_REG_EIP)!=STOP:raise RuntimeError('original instruction budget exceeded')
    return mu.reg_read(UC_X86_REG_EAX)
next16=bytearray();next32=bytearray();seed32=bytearray()
for seed in range(65536):
    mu.mem_write(DATA,struct.pack('<HHI',seed,0,0));result=call(0x4643d0)
    next16.extend(struct.pack('<H',result&65535))
    if struct.unpack('<I',mu.mem_read(DATA+4,4))[0]!=1:raise RuntimeError('original RNG16 counter invalid')
    mu.mem_write(DATA,struct.pack('<HHI',seed,0,0));result=call(0x464440)
    next32.extend(struct.pack('<I',result));seed32.extend(mu.mem_read(DATA,2))
    if seed%16384==0:print('Original RNG seeds:',seed,flush=True)
fixtures=[]
for folder,pattern in [(WORKSPACE/'th12/replay','*.rpy'),(TITLE/'local/retail','demo*.rpy')]:
    for file in sorted(folder.glob(pattern)):
        b=file.read_bytes();n=struct.unpack_from('<I',b,28)[0];mu.mem_write(DATA,b[36:36+n])
        call(0x4639d0,(DATA,n,0xe1,0x800,n),eax=0x5e)
        call(0x4639d0,(DATA,n,0x3a,0x40,n),eax=0x7d)
        fixtures.append({'id':file.name,'replaySha256':hashlib.sha256(b).hexdigest(),'decryptedSha256':hashlib.sha256(mu.mem_read(DATA,n)).hexdigest()})
        print('Original Replay decrypt:',file.name,flush=True)
identity=json.loads((TITLE/'local/content-identity.json').read_text())
proposal={'schema':'th12-subsystem-oracle/1','scope':['rng16-all-seeds','rng32-all-seeds','retail-replay-decrypt'],
          'wholeGameDeterminism':False,'original':identity,'provider':{'name':'original-x86-instructions','version':'1','unicorn':'2.1.4','rng16':'0x4643d0','rng32':'0x464440','decrypt':'0x4639d0','sampling':'after-original-function-return'},
          'completion':{'rngSeeds':65536,'replayFixtures':len(fixtures)},'next16':base64.b64encode(next16).decode(),'next32':base64.b64encode(next32).decode(),'seedAfter32':base64.b64encode(seed32).decode(),'replayDecryption':fixtures}
raw=(json.dumps(proposal,separators=(',',':'),ensure_ascii=False)+'\n').encode();digest=hashlib.sha256(raw).hexdigest()
output=TITLE/'local/oracle-proposals'/f'{digest}.json';output.parent.mkdir(parents=True,exist_ok=True)
output.write_bytes(raw);print('PROPOSAL',output,flush=True)
