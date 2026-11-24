#!/usr/bin/env python3
"""Read-only evidence for Perfect Freeze bullet-owner integration (task9)."""
import argparse
import hashlib
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'work/reverse-libs'))
import pefile  # noqa: E402

EXE_SHA = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
EXE = pathlib.Path(r'E:\gee\妖精大战争\th128.exe')
RANGES = {
    'phase120': (0x407d15, 0x407d74),
    'eligibility': (0x40eaf7, 0x40eb3b),
    'freeze_body': (0x40eb3b, 0x40ece8),
    'freeze_bullet': (0x40ece8, 0x40edc0),
    'freeze_tail': (0x40eee5, 0x40eef0),
    'bullet_reset': (0x407fb0, 0x408055),
}


def digest(data):
    return hashlib.sha256(data).hexdigest()


def read_code(pe, start, end):
    return pe.get_data(start - pe.OPTIONAL_HEADER.ImageBase, end - start)


def main():
    parser = argparse.ArgumentParser(description='Read-only Perfect Freeze bullet integration probe')
    parser.add_argument('--exe', type=pathlib.Path, default=EXE)
    parser.add_argument('--output', type=pathlib.Path,
                        default=ROOT / 'local/research/perfect-bullet-integration-fixtures.json')
    args = parser.parse_args()
    exe = args.exe.read_bytes()
    assert digest(exe) == EXE_SHA, 'unexpected original executable'
    pe = pefile.PE(data=exe, fast_load=True)
    code = {name: read_code(pe, start, end) for name, (start, end) in RANGES.items()}
    for name, data in code.items():
        assert data
    phase = code['phase120']
    assert bytes.fromhex('c78100b20600e02e0000') in phase  # manager+0x6b200 = 12000
    assert bytes.fromhex('899918b20600') in phase       # manager+0x6b218 = 1
    assert bytes.fromhex('8389fcb1060004') in phase     # manager+0x6b1fc |= 4
    eligibility = code['eligibility']
    assert bytes.fromhex('8bb3fcb10600c1ee0283e601') in eligibility  # bit4 test
    assert bytes.fromhex('83f801') in eligibility                   # policy 1
    assert bytes.fromhex('83f802') in eligibility                   # policy 2
    assert bytes.fromhex('83b99c1100003c') in eligibility           # age 60
    bullet = code['freeze_bullet']
    assert bytes.fromhex('68f81e4b00') in bullet                    # selector1 bitmap
    assert bytes.fromhex('b817000000e81391ffff') in bullet          # layer23 effect
    assert bytes.fromhex('83a3fcb10600fb') in code['freeze_tail']   # tail clears 4
    reset = code['bullet_reset']
    assert bytes.fromhex('6689882a0a0000') in reset                # Birth.state.motionState (+0xa2a)
    assert bytes.fromhex('c78084110000c1bdf0ff') in reset          # bullet +0x1184 timer
    assert bytes.fromhex('c78098110000ffffffff') in reset          # 0x408047 life timer = -1
    assert bytes.fromhex('8320fe') in reset                        # 0x408051 clear active bit
    assert reset[-1] == 0xc3                                       # 0x408054 ret

    large_path = ROOT / 'local/research/large-freeze-fixtures.json'
    large = json.loads(large_path.read_text(encoding='utf-8'))
    assert large['sourceExeSha256'] == EXE_SHA
    cases = []
    for index, case in enumerate(large['cases']):
        profile = bytes.fromhex(case['memory']['profile']['before'])
        bullet = bytes.fromhex(case['memory']['bullet']['before'])
        policy = struct.unpack_from('<i', profile, 0x118)[0]
        age = struct.unpack_from('<i', bullet, 0x119c)[0]
        active = bullet[0] & 1
        flags = struct.unpack_from('<I', bytes.fromhex(case['memory']['managerFlags']['before']), 0)[0]
        stop = case['stop']['address']
        eligible = stop == '0x0040eb3b'
        cases.append({'case': index, 'policy': policy, 'age': age, 'active': active,
                      'phase120Flag': bool(flags & 4), 'eligible': eligible,
                      'stop': stop})
    grouped = {}
    for case in cases:
        grouped.setdefault((case['policy'], case['phase120Flag']), set()).add(case['eligible'])
    for policy in (0, 1, 2, 3):
        assert grouped[(policy, True)] == {True}, 'phase120 must override profile policy'
    assert grouped[(1, False)] == {False}, 'ordinary policy1 must stay immune'
    assert grouped[(2, False)] == {True, False}, 'ordinary policy2 age gate must remain'

    type25 = json.loads((ROOT / 'local/generations/8c71c316a4b24e68827f9fc560242050/ja/'
                         'animations/bullet-type25-profile.json').read_text(encoding='utf-8'))
    assert type25['freezePolicy'] == 1 and type25['type'] == 25
    byte_count = 172032
    fixture = {
        'schema': 'th128-perfect-bullet-integration/1',
        'sourceExe': {'bytes': len(exe), 'sha256': EXE_SHA},
        'code': {name: {'start': hex(RANGES[name][0]), 'endExclusive': hex(RANGES[name][1]),
                        'bytes': len(data), 'sha256': digest(data),
                        'selectedBytes': data.hex() if len(data) <= 320 else None}
                 for name, data in code.items()},
        'phase120': {'phaseTicks': 120, 'savedPower': 12000, 'activeFlag': 1,
                     'overrideBit': {'manager+0x6b1fc': 4},
                     'prePhaseBulletClear': 'not observed in selected activation/phase slices; owner contract only'},
        'traversal': {'entry': '0x40eaa0', 'callbackJmp': '0x4101b0',
                      'bulletBase': '[0x4b8930]+0x64', 'strideBytes': 0x11b8,
                      'slots': 2000, 'activeByteBit': 1,
                      'profilePolicyOffset': 0x118, 'ageOffset': 0x119c,
                      'cellDomain': {'xMax': 7, 'yMax': 9},
                      'phase120Override': 'bit4 jumps directly to 0x40eb3b; policy1 and policy2/age60 checks are bypassed',
                      'ordinaryPolicy1': 'bit4 clear + profile+118==1 -> 0x40eed3 skip',
                      'ordinaryPolicy2': 'bit4 clear + profile+118==2 + age<60 -> 0x40eed3 skip'},
        'eligibilitySource': {'path': 'local/research/large-freeze-fixtures.json',
                              'sha256': digest(large_path.read_bytes())},
        'eligibilityCases': cases,
        'freezeBody': {'builder': '0x410590', 'duration': 200, 'mode': 0,
                       'maskSelector': 1, 'bitmap': '0x4b1ef8', 'rows': 21, 'wordsPerRow': 1,
                       'effectFactory': '0x407ec0', 'effectLayer': 23, 'effectScript': 1,
                       'maskMerge': '0x4106e0',
                       'bulletPost': '0x407fb0 resets Birth.state.motionState (+0xa2a) and age/life; 0x408047 life=-1; 0x408051 [bullet]&=~1; 0x408054 ret',
                       'motionStateField': 'Birth.state.motionState (+0xa2a)',
                       'directManagerConsumeCallObservedInSlice': False,
                       'retireMappedToManagerConsume': True,
                       'cancelAllObserved': False},
        'tail': {'clearOverride': '0x40eee5 [manager+0x6b1fc] &= ~4',
                 'maskCounter': '0x410960', 'maskBits': byte_count,
                 'lastAreaField': 'manager+0x6b214', 'cumulativeAreaField': '0x4b4d60',
                 'activeFlagCheck': 'manager+0x6b218 == 1',
                 'maskClear': '0x40f89d push 0x5400; 0x40f8a4 lea eax,[manager+0x65de4]; 0x40f8ac call 0x47bb30; 0x40f8ba manager+0x6b218=0',
                 'maskLifetime': 'per-callback scratch: settle area then clear all 172032 bits; Perfect ice expiry does not subtract',
                 'rewardGate': '0x40f7ea savedPower > 10000 skips Perfect charge gain'},
        'type25': {'type': 25, 'freezePolicy': 1, 'ordinaryZ': 'bit4 clear stays immune',
                   'perfectAtPhase120': 'eligible by native override'},
        'owners': {
            'activation': 'PerfectFreezeHost: protection/damage/effect/spend; no clear',
            'phase120': 'PerfectFreezeHost::tick calls Services::freezeBullets',
            'bulletMutation': 'BulletManager owns live slots; no frozen-state API currently',
            'iceTransfer': 'IceHost owns builder/mask/effect/expiry; convert() is parameterized by target mask; Perfect scratch and ordinary persistent masks are separate',
            'areaScoreCharge': 'WorldExports completion service + FreezeAccounting/FreezeRules',
            'items': 'ItemHost via IceHost expire (kind9/cancellation375); not in freeze traversal',
            'lasers': 'no populated owner; only laserCount==0 may be Empty',
            'maskOwnership': 'Perfect uses independent scratch mask cleared after settlement; ordinary persistent mask unchanged; ordinary activate remains rejected during Perfect batch; new ordinary bullets convert into scratch; second Perfect batch while live rejected',
        },
        'acceptanceCases': [
            {'id': 'policy1-ordinary-immune', 'bit4': False, 'policy': 1, 'age': 60, 'eligible': False},
            {'id': 'policy2-age59-ordinary-skip', 'bit4': False, 'policy': 2, 'age': 59, 'eligible': False},
            {'id': 'policy2-age60-ordinary-eligible', 'bit4': False, 'policy': 2, 'age': 60, 'eligible': True},
            {'id': 'perfect-policy1-override', 'bit4': True, 'policy': 1, 'age': 60, 'eligible': True},
            {'id': 'perfect-domain-x8-reject', 'bit4': True, 'cellX': 8, 'eligible': False},
            {'id': 'perfect-no-bullets-before-phase120', 'phase': 119, 'freezeCalls': 0},
            {'id': 'perfect-service-completed-requires-all-eligible', 'partial': True, 'completed': False},
            {'id': 'perfect-source-retires-via-consume-once', 'frozen': 1, 'consumeCalls': 1},
            {'id': 'perfect-missing-visual-fails', 'eligible': True, 'missingVisual': True, 'completed': False},
            {'id': 'perfect-unknown-mask-selector-rejected', 'selector': 6, 'completed': False},
            {'id': 'ordinary-activation-rejects-during-perfect', 'ordinaryActivate': False, 'error': 'perfect freeze batch'},
            {'id': 'ordinary-policy0-converts-during-perfect', 'ordinaryTick': True, 'consume': 1, 'sound69': 1},
            {'id': 'perfect-callback-completes-once', 'conversions': 2, 'perfectCompleteCalls': 1, 'power': 12000},
            {'id': 'perfect-second-batch-while-live-rejected', 'secondBatch': False, 'error': 'exclusive empty ice batch'},
            {'id': 'populated-laser-no-empty-success', 'laserCount': 1, 'completed': False},
        ],
        'unknowns': [
            'Ordinary Z activation/coexistence while Perfect objects are outstanding remains unsupported; ordinary bullets during the batch are supported via scratch conversions.',
            'WorldExports phase120 immediate settlement and per-callback power12000 perfectComplete wiring are not yet wired.',
            'Populated-laser ownership remains external and unsupported.',
        ],
    }
    args.output.write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf-8')
    print(f'wrote {args.output.relative_to(ROOT)}: {len(cases)} eligibility cases, '
          f'{len(fixture["acceptanceCases"])} acceptance cases')


if __name__ == '__main__':
    main()
