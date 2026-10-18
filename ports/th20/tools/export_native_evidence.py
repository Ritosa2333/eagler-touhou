"""Export exact disassembly ranges underlying manually recovered C++ functions."""
from pathlib import Path
import hashlib
import json
import sys
ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / '.cache/binary_python'))
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

report = json.loads((ROOT / 'reports/native_validation.json').read_text('utf-8'))
raw = Path(report['source_exe']).read_bytes()
if hashlib.sha256(raw).hexdigest() != report['source_sha256']:
    raise ValueError('Source executable has changed')
pe = pefile.PE(data=raw)
disasm = Cs(CS_ARCH_X86, CS_MODE_32)
target = ROOT / 'analysis/native_verified'
target.mkdir(parents=True, exist_ok=True)
for name, evidence in report['evidence'].items():
    address, end = int(evidence['va'], 16), int(evidence['end_exclusive'], 16)
    code = pe.get_data(address - pe.OPTIONAL_HEADER.ImageBase, end-address)
    lines = [f'; {name}: original th20.exe code, SHA256 {report["source_sha256"]}',
             '; Recovered C++: native_recovered/native_core.hpp',
             '; Called helpers remain visible in analysis/binary/disassembly.asm']
    for ins in disasm.disasm(code, address):
        lines.append(f'{ins.address:08x}  {ins.bytes.hex():<32} {ins.mnemonic:<8} {ins.op_str}')
    (target / (name + '.asm')).write_text('\n'.join(lines) + '\n', 'utf-8')
print(f'Exported {len(report["evidence"])} evidence ranges to {target}')
