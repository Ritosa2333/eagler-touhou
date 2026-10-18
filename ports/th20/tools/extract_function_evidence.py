"""Disassemble selected original function ranges, without changing the EXE."""
import argparse,csv,hashlib,json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'.cache/binary_python'))
import capstone,pefile
parser=argparse.ArgumentParser()
parser.add_argument('--source',type=pathlib.Path,default=pathlib.Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe'))
parser.add_argument('--output',type=pathlib.Path,required=True)
parser.add_argument('addresses',nargs='+',type=lambda s:int(s,16))
args=parser.parse_args()
raw=args.source.read_bytes()
digest=hashlib.sha256(raw).hexdigest()
if digest!='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897':raise SystemExit('Original hash mismatch')
pe=pefile.PE(data=raw);md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
rows={int(r['entry_va'],16):r for r in csv.DictReader((ROOT/'analysis/ghidra/functions.csv').open(encoding='utf8'))}
args.output.mkdir(parents=True,exist_ok=True)
for va in args.addresses:
    row=rows[va]
    # body_bytes can exclude padding/gaps. Include until the next known entry;
    # callers must still distinguish function bodies from padding/jump tables.
    following=min(x for x in rows if x>va)
    size=following-va
    data=pe.get_data(va-pe.OPTIONAL_HEADER.ImageBase,size)
    text=''.join(f'{i.address:08x} {i.bytes.hex():24} {i.mnemonic:10} {i.op_str}\n' for i in md.disasm(data,va))
    (args.output/f'{va:08x}.asm').write_text(text,encoding='ascii')
print(json.dumps({'sha256':digest,'functions':len(args.addresses),'directory':str(args.output)}))
