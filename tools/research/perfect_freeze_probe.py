import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-perfect-freeze-fixtures/1'

SLICES = {
    'xGate': (0x43b7a8, 0x43b7f3),
    'activationStart': (0x407810, 0x4078af),
    'selectorEffect': (0x407a70, 0x407b1c),
    'effectFollow': (0x407b77, 0x407baa),
    'deathbombEffect': (0x407c50, 0x407cc1),
    'cost': (0x427300, 0x42732a),
    'deathbombWindowGate': (0x43b8c1, 0x43b925),
    'systemTick': (0x407790, 0x4077d6),
    'systemRegister': (0x407490, 0x4074e8),
    'freezeCallback': (0x4101b0, 0x4101b5),
    'freezeEligibility': (0x40eaf7, 0x40eb3b),
    'freezeClear': (0x40eee5, 0x40eeec),
    'overrideSet': (0x407a40, 0x407a4d),
    'overrideSetInTick': (0x407d6d, 0x407d74),
}

ANCHORS = {
    'xGate': (
        'mov eax, dword ptr [0x4b892c]',
        'cmp dword ptr [eax + 0x3c], 0',
        'cmp dword ptr [0x4b4d70], 0x2710',
        'test byte ptr [0x4db210], 2',
        'call 0x407810',
        'push 0xffffd8f0',
        'call 0x427300',
        'call 0x425820',
    ),
    'activationStart': (
        'mov dword ptr [esi + 0x3c], ecx',
        'mov dword ptr [esi + 0x550], ecx',
        'call 0x4584a0',
        'call 0x407a70',
    ),
    'selectorEffect': (
        'fstp dword ptr [ebx + 0x548]',
        'fstp dword ptr [ebx + 0x54c]',
        'mov dword ptr [eax + 0x438dc], 0xc8',
        'mov dword ptr [eax + 0x438d8], 0xc7',
        'call 0x4670b0',
    ),
    'effectFollow': (
        'mov eax, 0x33',
        'call 0x4584a0',
        'call 0x407ec0',
    ),
    'deathbombEffect': (
        'mov dword ptr [eax + 0x438dc], 0xa',
        'mov dword ptr [eax + 0x438d8], 9',
    ),
    'cost': (
        'add ecx, dword ptr [ebp + 8]',
        'mov dword ptr [0x4b4d70], ecx',
        'cmp ecx, 0x7530',
    ),
    'deathbombWindowGate': (
        'push 0x3c',
        'call 0x403280',
        'call 0x407810',
        'call 0x427150',
        'push 0xffffd8f0',
        'call 0x427300',
        'mov dword ptr [esi + 0xf78], 1',
    ),
    'systemTick': (
        'mov eax, dword ptr [esi + 0x3c]',
        'dec eax',
        'call 0x407c50',
        'mov dword ptr [esi + 0x3c], 0',
    ),
    'systemRegister': (
        'mov dword ptr [esi + 8], 0x407790',
        'call 0x4681d0',
    ),
    'freezeCallback': ('jmp 0x40eaa0',),
    'freezeEligibility': (
        'mov esi, dword ptr [ebx + 0x6b1fc]',
        'shr esi, 2',
        'and esi, 1',
        'mov eax, dword ptr [edx + 0x11ac]',
        'mov eax, dword ptr [eax + 0x118]',
        'cmp eax, 1',
        'cmp eax, 2',
        'cmp dword ptr [ecx + 0x119c], 0x3c',
    ),
    'freezeClear': ('and dword ptr [ebx + 0x6b1fc], 0xfffffffb',),
    'overrideSet': ('or dword ptr [eax + 0x6b1fc], 4',),
    'overrideSetInTick': ('or dword ptr [ecx + 0x6b1fc], 4',),
}


def address(value):
    return f'0x{value:08x}'


def read(image, start, size):
    if type(size) is not int or not 0 < size <= 65536:
        raise ValueError('bounded read size')
    data = bytes(image.get_data(start - 0x400000, size))
    if len(data) != size:
        raise ValueError('truncated original read')
    return data


def decode(image, start, end, name):
    code = read(image, start, end - start)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    instructions = list(decoder.disasm(code, start))
    if sum(item.size for item in instructions) != len(code):
        raise ValueError(f'undecodable original slice {name}')
    text = '\n'.join(f'{item.mnemonic} {item.op_str}'.strip() for item in instructions)
    missing = [anchor for anchor in ANCHORS[name] if anchor not in text]
    if missing:
        raise ValueError(f'{name} missing anchors {missing}')
    return {'start': address(start), 'endExclusive': address(end),
            'sha256': hashlib.sha256(code).hexdigest(),
            'instructions': [{'address': address(item.address), 'bytes': item.bytes.hex(),
                              'text': f'{item.mnemonic} {item.op_str}'.strip()}
                             for item in instructions]}


def resource(path, metadata):
    data = path.read_bytes()
    scripts = []
    if metadata.exists():
        scripts = [item['index'] for item in json.loads(metadata.read_text(encoding='utf-8')).get('scripts', [])]
    return {'path': str(path.relative_to(ROOT)).replace('\\', '/'),
            'sha256': hashlib.sha256(data).hexdigest(), 'bytes': len(data),
            'scripts': scripts}


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        slices = {name: decode(image, start, end, name) for name, (start, end) in SLICES.items()}
        peak = struct.unpack('<f', read(image, 0x4a69ec, 4))[0]
        constants = {
            'perfectCostRaw': 10000,
            'perfectCapRaw': 30000,
            'protectionPeakFloat': peak,
            'perfectProtectionCounter': int(round(peak)),
            'xKeyBit': 2,
            'systemModeOffset': 0x3c,
            'systemPointer': address(0x4b892c),
            'chargeGlobal': address(0x4b4d70),
            'keysGlobal': address(0x4db210),
            'busyOffset': 0x6b218,
        }
        raw = (ROOT / 'local/catalog.json')
        catalog = json.loads(raw.read_text(encoding='utf-8'))
        raw_dir = (ROOT / 'local' / catalog['variants']['ja']['manifest']).parent / 'raw'
        raw_dir = raw_dir.resolve()
        metadata_dir = raw_dir.parent / 'animations'
        resources = {
            'front': resource(raw_dir / 'front.anm', metadata_dir / 'front.json'),
            'pl00': resource(raw_dir / 'pl00.anm', metadata_dir / 'pl00.json'),
            'ice': resource(raw_dir / 'ice.anm', metadata_dir / 'ice.json'),
        }
        large = json.loads((ROOT / 'local/research/large-freeze-fixtures.json').read_text(encoding='utf-8'))
        policies = [{'type': item['type'], 'freezePolicy': item['freezePolicy'],
                     'appearanceKind': item['appearanceKind']} for item in large['profiles']]
        eligibility = [{'policy': item['policy'], 'age': item['age'],
                        'managerOverride': item['override'], 'eligible': item['eligible']}
                       for item in large['cases']]
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'apiReturnsAssigned': False,
            'slices': slices,
            'constants': constants,
            'gates': {
                'normalXActivation': '43b7a8',
                'deathbombWindowXActivation': '43b8c1',
                'requiresSystemPointer': True,
                'requiresMode3cZero': True,
                'requiresChargeAtLeast': 10000,
                'requiresXKeyBit': 2,
            },
            'cost': {'perfectFreezeRaw': 10000, 'capRaw': 30000,
                     'chargeAddCall': '0x427300', 'chargeSpendArgument': -10000},
            'protection': {
                'normalPerfectTicks': 200,
                'deathbombTicks': 10,
                'normalEvidence': '0x407a70: 438d8=199, 438dc=200, 438e0=200.0',
                'deathbombEvidence': '0x407c50: 438d8=9, 438dc=10',
                'normalNotPermanent': True,
            },
            'freezeTraversal': {
                'entry': '0x40eaa0',
                'callback': '0x4101b0 jmp 0x40eaa0',
                'eligibilityPrefix': '0x40eaf7',
                'managerOverrideOffset': 0x6b1fc,
                'managerOverrideBit': 4,
                'overrideSetSites': ['0x407a45', '0x407d6d'],
                'overrideClearSite': '0x40eee5',
                'profilePolicyOffset': 0x118,
                'profilePolicyBranches': {'1': 'ineligible', '2': 'age>=60 eligible', 'other': 'eligible'},
                'setter407a40Callers': [],
                'setter407a40TiedTo407a70': False,
            },
            'overridePhase': {
                'systemStartSite': '0x407810',
                'systemStateOffset': 0x3c,
                'systemTickCallback': '0x407790',
                'systemRegistrationCall': '0x4681d0',
                'tickDecrementsStateThenCalls': '0x407c50',
                'overrideSetSite': '0x407d6d',
                'overridePhaseValue': 0x78,
                'setter407a45Reachable': False,
            },
            'effectBinding': {
                'bank': 36,
                'script': 0x24,
                'layer': 0x17,
                'resource': 'ice.anm',
                'managerResourceOffset': 0x65ddc,
                'evidence': '0x407a70 ECX=[manager+0x65ddc]; loader stores ice.anm slot36 there; pushes 0,0x17,0x24',
                'frontCandidateRejected': True,
            },
            'resources': resources,
            'bulletPolicy': {
                'source': 'local/research/large-freeze-fixtures.json',
                'profiles': policies,
                'eligibilityCases': eligibility,
                'perfectOverrideImmuneProven': False,
                'decision': 'delegated to real bullet owner; no blanket override',
            },
            'laserPolicy': {
                'emptyContext': 'allowed only with laserCount=0',
                'populatedContext': 'requires explicit real freezeLasers service',
                'voidAllLaserSuccess': False,
            },
            'limitations': [
                'No full-screen bullet/laser/damage implementation in this module.',
                'Exact perfect freeze effect bank/script and 0x407a70 subcall semantics are not fully proven.',
                'No original EXE execution; static bounded PE/capstone evidence only.',
            ],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/perfect-freeze-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/perfect-freeze-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('perfect freeze slices %d, cost %d, protection %d/%d, policies %d'
          % (len(fixture['slices']), fixture['cost']['perfectFreezeRaw'],
             fixture['protection']['normalPerfectTicks'], fixture['protection']['deathbombTicks'],
             len(fixture['bulletPolicy']['profiles'])))


if __name__ == '__main__':
    main()
